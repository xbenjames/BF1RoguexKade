"""Crisp vector drawing for the minimaps (the stock maps are drawn by hand: straight edges, few shapes).
- every building is traced to a polygon and simplified (straight edges, true rectangles snapped)
- the play area is the ground you can actually walk to from the CPs, walls and buildings block it
- clutter (props, plants, lamps, crates, tiny bits) is left out"""
import re
import numpy as np
import cv2

CLUTTER = re.compile(r"planter|lamp|lampost|post|tree|leaf|flower|plant|bush|shrub|grass|fern|vine|bench|chair|table|crate|barrel|"
                     r"box|sign|banner|flag|light|torch|debris|rubble|rock|stone|pebble|weed|fx|mist|cloud|sky(?!way)|dust|god|decal|"
                     r"billboard|trench|collapse|falcon|millen|cable|rafter|log|trunk|stump|fallen|turret|gun|console|terminal|panel|droid|speeder|ship|fighter|vehicle|spawn|recharge|item|holo", re.I)


DARK = re.compile(r"reflection|water|pool", re.I)     # drawn in the darker band blue (naboo2 rotunda ring)


def is_dark(layer):
    return any(DARK.search(p["cls"]) for p in layer[2])


def tri_mask(px, shape, ss=1.0):
    """fill triangles (k,3,2 pixel coords) into a uint8 mask"""
    m = np.zeros(shape, np.uint8)
    if len(px):
        pts = np.round(px * 16 * ss).astype(np.int32)
        a = pts.astype(np.int64)
        area = np.abs((a[:, 1, 0] - a[:, 0, 0]) * (a[:, 2, 1] - a[:, 0, 1]) - (a[:, 2, 0] - a[:, 0, 0]) * (a[:, 1, 1] - a[:, 0, 1]))
        pts = pts[area > 0]
        cv2.fillPoly(m, list(pts), 255, lineType=cv2.LINE_8, shift=4)
    return m


def simplify(mask, eps_px, min_area_px, snap_rect=0.9):
    """mask -> list of (outer polygon, [holes]) simplified; rectangles snapped to their min-area rectangle"""
    cnts, hier = cv2.findContours(mask, cv2.RETR_CCOMP, cv2.CHAIN_APPROX_NONE)
    out = []
    if hier is None:
        return out
    hier = hier[0]
    for i, c in enumerate(cnts):
        if hier[i][3] != -1:
            continue
        a = cv2.contourArea(c)
        if a < min_area_px:
            continue
        poly = shape_poly(c, a, eps_px, snap_rect)
        holes = []
        j = hier[i][2]
        while j != -1:
            h = cnts[j]
            ha = cv2.contourArea(h)
            if ha >= min_area_px:
                holes.append(shape_poly(h, ha, eps_px, snap_rect))
            j = hier[j][0]
        out.append((poly, holes))
    return out


def shape_poly(c, a, eps_px, snap_rect):
    rect = cv2.minAreaRect(c)
    ra = rect[1][0] * rect[1][1]
    if ra > 0 and a / ra >= snap_rect:
        return cv2.boxPoints(rect).astype(np.float32)
    (cx, cy), r = cv2.minEnclosingCircle(c)
    if a / (np.pi * r * r + 1e-9) >= 0.94 and r > 3 * eps_px:      # an octagon (0.90) stays an octagon
        t = np.linspace(0, 2 * np.pi, max(24, int(r)), endpoint=False)
        return np.stack([cx + r * np.cos(t), cy + r * np.sin(t)], 1).astype(np.float32)
    p = cv2.approxPolyDP(c, eps_px, True).reshape(-1, 2).astype(np.float32)
    return p


def draw_polys(img, polys, fill, rim=None, rim_w=1.0, shift=4):
    """img float HxWx3 (or HxW); polys from simplify(); fill + rim (anti-aliased) drawn at sub-pixel precision"""
    H, W = img.shape[:2]
    m = np.zeros((H, W), np.uint8)
    for poly, holes in polys:
        cv2.fillPoly(m, [np.round(poly * (1 << shift)).astype(np.int32)], 255, cv2.LINE_AA, shift)
        for h in holes:
            cv2.fillPoly(m, [np.round(h * (1 << shift)).astype(np.int32)], 0, cv2.LINE_AA, shift)
    blend(img, m, fill)
    if rim is not None:
        r = np.zeros((H, W), np.uint8)
        t = max(1, int(round(rim_w)))
        for poly, holes in polys:
            for p in [poly] + holes:
                cv2.polylines(r, [np.round(p * (1 << shift)).astype(np.int32)], True, 255, t, cv2.LINE_AA, shift)
        blend(img, r, rim)
    return m


def blend(img, alpha_u8, col):
    a = (alpha_u8.astype(np.float32) / 255.0)
    col = np.asarray(col, np.float32)
    if img.ndim == 3:
        img[:] = img * (1 - a[..., None]) + col * a[..., None]
    else:
        img[:] = img * (1 - a) + col * a


def box_px(job, size_m, pos, rot):
    """square collision box (com_inv_col_N) -> polygon in pixels"""
    from mm_core import qmat
    R = qmat(*rot)
    h = size_m / 2.0
    corners = np.array([[-h, 0, -h], [h, 0, -h], [h, 0, h], [-h, 0, h]]) * np.array([-1, 1, 1])
    w = corners @ R.T + np.array(pos)
    return job.to_px(w[:, 0], w[:, 2]).astype(np.float32)


GAP_M = 5.0


def play_area(job, building_masks, col_polys, seeds, max_slope=None):
    """ground you can walk to from the CPs: flood fill of the free ground, blocked by buildings and collision walls"""
    N = job.N
    block = np.zeros((N, N), np.uint8)
    for m in building_masks:
        block |= m
    for p in col_polys:
        cv2.fillPoly(block, [np.round(p).astype(np.int32)], 255)
    if max_slope is not None and job.H is not None:
        gy, gx = np.gradient(job.H, 2 * job.E / N)
        steep = (np.hypot(gx, gy) > max_slope).astype(np.uint8) * 255
        steep = cv2.morphologyEx(steep, cv2.MORPH_OPEN, np.ones((3, 3), np.uint8))
        block |= steep
    # close small gaps between buildings (doorways, columns) so the flood stays in the plaza
    gap = max(1, int(round(GAP_M / (2 * job.E / N))))
    k = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (2 * gap + 1, 2 * gap + 1))
    wall = cv2.dilate(block, k)
    free = (wall == 0).astype(np.uint8)
    n, lab = cv2.connectedComponents(free, connectivity=4)
    keep = set()
    dist = None
    for s in seeds:
        x, y = int(round(s[0])), int(round(s[1]))
        if not (0 <= x < N and 0 <= y < N):
            continue
        if lab[y, x] == 0:
            if dist is None:
                dist, idx = cv2.distanceTransformWithLabels((free == 0).astype(np.uint8), cv2.DIST_L2, 5, labelType=cv2.DIST_LABEL_PIXEL)
                ys, xs = np.nonzero(free == 1)
                lut = np.zeros(idx.max() + 1, np.int64)
                lut[idx[ys, xs]] = ys * N + xs
            j = lut[idx[y, x]]
            y, x = divmod(int(j), N)
        if lab[y, x]:
            keep.add(lab[y, x])
    area = np.isin(lab, list(keep)).astype(np.uint8) * 255
    # give back the gap margin, but never into buildings
    area = cv2.dilate(area, k) & ~block
    # area touching the box edge means the flood leaked out: report it
    leaked = area[0].any() or area[-1].any() or area[:, 0].any() or area[:, -1].any()
    return area, leaked


def town_block(masks, seeds, close_px, N):
    """the town block the stock artist shades: everything built, closed across streets, holes filled, the part holding the CPs"""
    m = np.zeros((N, N), np.uint8)
    for x in masks:
        m |= x
    k = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (2 * close_px + 1,) * 2)
    m = cv2.morphologyEx(m, cv2.MORPH_CLOSE, k)
    ff = np.pad(m, 1).copy()
    cv2.floodFill(ff, None, (0, 0), 128)
    m = np.where(ff[1:-1, 1:-1] == 128, 0, 255).astype(np.uint8)
    n, lab = cv2.connectedComponents(m)
    keep = {lab[int(round(s[1])), int(round(s[0]))] for s in seeds if 0 <= s[0] < N and 0 <= s[1] < N} - {0}
    return np.isin(lab, list(keep)).astype(np.uint8) * 255 if keep else m


def piece_polys(job, p, eps_px, min_area_px, close_px=1, solid=True):
    """one placed object -> simplified polygons in full-canvas pixels (traced in a small local window)"""
    px = p["px"]
    x0 = int(np.floor(px[..., 0].min())) - 4; y0 = int(np.floor(px[..., 1].min())) - 4
    x1 = int(np.ceil(px[..., 0].max())) + 5; y1 = int(np.ceil(px[..., 1].max())) + 5
    if x1 - x0 <= 2 or y1 - y0 <= 2:
        return []
    m = tri_mask(px - np.array([x0, y0]), (y1 - y0, x1 - x0))
    if close_px > 0:
        k = np.ones((2 * close_px + 1, 2 * close_px + 1), np.uint8)
        m = cv2.morphologyEx(m, cv2.MORPH_CLOSE, k)
    polys = simplify(m, eps_px, min_area_px)
    if solid:
        # small openings in a roof are filled; a big open middle (a walled yard open to the sky - tat2 arena)
        # stays open, as stock draws those as a ring of wall
        polys = [(pp, [h for h in hh if cv2.contourArea(h) >= 0.2 * cv2.contourArea(pp)]) for pp, hh in polys]
    off = np.array([x0, y0], np.float32)
    return [(pp + off, [h + off for h in hh]) for pp, hh in polys]


class LMask:
    """a polygon mask kept only over its own bounding box (full-canvas masks for hundreds of pieces ran out of memory)"""
    def __init__(self, polys, N, pad=2):
        pts = np.concatenate([pp for pp, hh in polys]) if polys else np.zeros((1, 2), np.float32)
        self.x0 = max(0, int(np.floor(pts[:, 0].min())) - pad); self.y0 = max(0, int(np.floor(pts[:, 1].min())) - pad)
        self.x1 = min(N, int(np.ceil(pts[:, 0].max())) + pad + 1); self.y1 = min(N, int(np.ceil(pts[:, 1].max())) + pad + 1)
        w, h = max(1, self.x1 - self.x0), max(1, self.y1 - self.y0)
        m = np.zeros((h, w), np.uint8)
        off = np.array([self.x0, self.y0], np.float32)
        draw_polys(m, [(pp - off, [hh_ - off for hh_ in hh]) for pp, hh in polys], 255)
        self.m = m > 127
        self.area = int(self.m.sum())

    def inter(self, o):
        x0, y0, x1, y1 = max(self.x0, o.x0), max(self.y0, o.y0), min(self.x1, o.x1), min(self.y1, o.y1)
        if x0 >= x1 or y0 >= y1:
            return 0
        a = self.m[y0 - self.y0:y1 - self.y0, x0 - self.x0:x1 - self.x0]
        b = o.m[y0 - o.y0:y1 - o.y0, x0 - o.x0:x1 - o.x0]
        return int((a & b).sum())

    def sample(self, full):
        """overlap with a full-canvas boolean mask"""
        return int((self.m & full[self.y0:self.y1, self.x0:self.x1][:self.m.shape[0], :self.m.shape[1]]).sum())

    def paint(self, full):
        full[self.y0:self.y1, self.x0:self.x1] |= self.m


def union_full(lms, N):
    u = np.zeros((N, N), bool)
    for l in lms:
        l.paint(u)
    return u


def pmask(polys, N):
    m = np.zeros((N, N), np.uint8)
    draw_polys(m, polys, 255)
    return m > 127


def plaza_level(job):
    """the level people walk on: the median CP height (a CP down in a pit or up on a deck doesn't move it)"""
    return float(np.median([c["pos"][1] for c in job.cps])) if job.cps else 0.0


def is_floor(job, layer):
    """a paved floor at plaza level with nothing built above head height (bespin2 courtyard): stock shows it as
    part of the plaza - navy with a light edge - not as a building"""
    if not job.cps:
        return False
    from mm_render import footprint
    yw = plaza_level(job)
    ps = [p.get("orig", p) for p in layer[2]]
    return all(p["top"] <= yw + 1.0 or (p["top"] <= yw + 4.0 and p["ymin"] >= yw - 12.0) for p in ps) and \
        sum(footprint(p) for p in ps) >= 400


def roofless(job, layer):
    """walls with no roof over them (RhenVar2 citadel rooms, ruins): stock draws these as outlines on the ground.
    Judged by how much of the footprint has an upward face above head height."""
    if not job.cps:
        return False
    from mm_render import footprint
    yw = plaza_level(job)
    tot = cov = 0.0
    for p in layer[2]:
        p = p.get("orig", p)
        if "roof" not in p:
            t = p["tri"]
            n_ = np.cross(t[:, 1] - t[:, 0], t[:, 2] - t[:, 0])
            a = 0.5 * np.linalg.norm(n_, axis=1)
            up = (np.abs(n_[:, 1]) > 0.7 * (2 * a + 1e-9)) & (t[..., 1].mean(1) > yw + 3.0)
            p["roof"] = float(a[up].sum()) / max(1.0, footprint(p))
        tot += 1
        cov += p["roof"]
    return tot > 0 and cov / tot < 0.3 and all(p.get("orig", p)["top"] > yw + 2.0 for p in layer[2])


def upper_part(job, p):
    """what stock draws of a building standing on the plaza: the part built up above head height.
    Its paving at ground level (the walk of a colonnade, a porch) is plaza, so stays navy.
    Low things (fountains, steps) and pieces rising from below the plaza (canal sides) are drawn whole."""
    if not job.cps:
        return p
    yw = plaza_level(job)
    t = p["tri"]
    if t[..., 1].min() < yw - 2.0 or t[..., 1].max() < yw + 4.0:
        return p
    sel = t[..., 1].min(1) > yw + 4.0       # wholly above head height: roofs and upper walls, not posts
    if sel.all() or not sel.any():
        return p
    q = dict(p)
    q["tri"] = t[sel]
    q["px"] = p["px"][sel]
    q["orig"] = p
    return q


def buildings(job, pieces, N, min_foot=20.0):
    """placed buildings -> [(top height, polygons)], lowest first, clutter left out.
    Each piece is traced on its own and simplified to straight edges; pieces of one class that overlap a lot
    are one shape, and wedges laid round a centre (a rotunda) become one round building."""
    mpp = 2 * job.E / N
    from mm_render import footprint
    bld = []
    for p in pieces:
        if CLUTTER.search(p["cls"]):
            continue
        if DARK.search(p["cls"]):
            # water / reflection planes stand upright: take their outline seen from above
            h = cv2.convexHull(p["px"].reshape(-1, 2).astype(np.float32)).reshape(-1, 2)
            if cv2.contourArea(h) > 1:
                bld.append((p, [(h, [])]))
            continue
        if footprint(p) < min_foot:
            continue
        p = upper_part(job, p)
        polys = piece_polys(job, p, eps_px=0.6 / mpp, min_area_px=6 / mpp ** 2, close_px=2)
        if polys:
            bld.append((p, polys))
    groups = {}
    for p, polys in bld:
        groups.setdefault(re.sub(r"_?\d+$", "", p["cls"].lower()), []).append((p, polys))
    layers = []
    for cls, items in groups.items():
        ms = [LMask(polys, N) for p, polys in items]
        par = list(range(len(items)))

        def f(i):
            while par[i] != i:
                i = par[i]
            return i
        if len(items) <= 60:
            for i in range(len(items)):
                for j in range(i + 1, len(items)):
                    inter = ms[i].inter(ms[j])
                    if inter and inter > 0.15 * min(ms[i].area, ms[j].area):
                        par[f(i)] = f(j)
        if len(items) >= 4:
            u = union_full(ms, N).astype(np.uint8)
            u = cv2.morphologyEx(u * 255, cv2.MORPH_CLOSE, np.ones((9, 9), np.uint8))
            cs, _ = cv2.findContours(u, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
            c = max(cs, key=cv2.contourArea) if cs else None
            r = cv2.minEnclosingCircle(c)[1] if c is not None else 0
            if len(cs) == 1 and cv2.contourArea(c) / (np.pi * r * r + 1e-9) > 0.8:
                for i in range(len(items)):
                    par[f(i)] = f(0)
        sets = {}
        for i in range(len(items)):
            sets.setdefault(f(i), []).append(i)
        for idx in sets.values():
            if len(idx) == 1:
                layers.append((items[idx[0]][0]["top"], items[idx[0]][1], [items[idx[0]][0]]))
                continue
            m = union_full([ms[i] for i in idx], N)
            polys = simplify(m.astype(np.uint8) * 255, 0.6 / mpp, 6 / mpp ** 2)
            layers.append((max(items[i][0]["top"] for i in idx), polys, [items[i][0] for i in idx]))
    layers.sort(key=lambda l: l[0])
    # small add-ons sitting mostly on a taller building (the palace's corner bulges) are part of it: stock never shows them
    masks = [LMask(l[1], N) for l in layers]
    keep = []
    hid = {}
    for i, l in enumerate(layers):
        a = masks[i].area
        hidden = False
        if is_dark(l):
            keep.append(i)
            continue
        for j, l2 in enumerate(layers):
            if j != i and masks[j].area > 8 * a and masks[i].inter(masks[j]) > 0.4 * a:
                hidden = True
                break
        if not hidden:
            keep.append(i)
        else:
            hid.setdefault(j, []).append(i)
    out = []
    for i, l in enumerate(layers):
        if i not in keep:
            continue
        if i in hid:
            m = union_full([masks[i]] + [masks[k] for k in hid[i]], N)
            pts = np.argwhere(m)[:, ::-1].astype(np.float32)
            hull = cv2.convexHull(pts)
            (cx, cy), r = cv2.minEnclosingCircle(hull)
            if cv2.contourArea(hull) / (np.pi * r * r + 1e-9) > 0.8:
                # the hull of the building with its add-ons; an octagon stays an octagon (yavin1 temple)
                l = (l[0], [(shape_poly(hull, cv2.contourArea(hull), max(1.0, 0.02 * r), 0.97), [])], l[2])
        out.append(l)
    return out


def town(job, layers, N, close_m=14.0):
    """the area the stock artist shades under a town with no walls (naboo2): the buildings closed across the
    streets, plazas filled in, the part holding the CPs; ground sunk well below the CPs (canals, pits) left out.
    Returns (area mask, area polygons)."""
    mpp = 2 * job.E / N
    union = np.zeros((N, N), np.uint8)
    for top, polys, _ in layers:
        union |= pmask(polys, N).astype(np.uint8) * 255
    union = cv2.morphologyEx(union, cv2.MORPH_OPEN, np.ones((3, 3), np.uint8))
    seeds = [job.to_px(c["pos"][0], c["pos"][2]) for c in job.cps]
    area = town_block([union], seeds, int(round(close_m / mpp)), N)
    # a big building holding no CP (the naboo2 palace) is a landmark beside the plaza, not plaza
    cut = np.zeros((N, N), np.uint8)
    for top, polys, ps in layers:
        m = pmask(polys, N)
        if m.sum() > 0.02 * N * N and not any(m[int(np.clip(s_[1], 0, N - 1)), int(np.clip(s_[0], 0, N - 1))] for s_ in seeds):
            r = max(1, int(10 / mpp))
            cut |= cv2.dilate(m.astype(np.uint8), cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (2 * r + 1,) * 2))
    area[cut > 0] = 0
    k10 = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (max(3, int(10 / mpp)),) * 2)
    area = cv2.morphologyEx(area, cv2.MORPH_OPEN, k10)
    if job.H is not None and job.cps:
        # the plaza level: the median CP (a CP down on a sunken floor must not drag it down - BF2 naboo2 rotunda)
        lo = float(np.median([c["pos"][1] for c in job.cps])) - 5
        pit = ((job.H < lo) & (union == 0)).astype(np.uint8) * 255
        pit = cv2.morphologyEx(pit, cv2.MORPH_OPEN, np.ones((5, 5), np.uint8))
        r = max(1, int(8 / mpp))
        area[cv2.dilate(pit, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (2 * r + 1,) * 2)) > 0] = 0
        area = cv2.morphologyEx(area, cv2.MORPH_OPEN, k10)
        n, lab = cv2.connectedComponents(area)
        keep = {lab[int(s_[1]), int(s_[0])] for s_ in seeds if 0 <= s_[0] < N and 0 <= s_[1] < N} - {0}
        if keep:
            area = np.isin(lab, list(keep)).astype(np.uint8) * 255
    k20 = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (max(3, int(16 / mpp)) | 1,) * 2)
    area = cv2.morphologyEx(area, cv2.MORPH_CLOSE, k20)
    area[cut > 0] = 0
    area = cv2.morphologyEx(area, cv2.MORPH_OPEN, k10)
    n, lab = cv2.connectedComponents(area)
    keep = {lab[int(s_[1]), int(s_[0])] for s_ in seeds if 0 <= s_[0] < N and 0 <= s_[1] < N} - {0}
    if keep:
        area = np.isin(lab, list(keep)).astype(np.uint8) * 255
    polys = simplify(area, 3.0 / mpp, 400 / mpp ** 2, snap_rect=0.97)
    area = pmask(polys, N).astype(np.uint8) * 255
    return area, polys


def near_pieces(job, pieces, area, N, near_m=6.0):
    """pieces on or beside the play area; pieces well below the CPs and off the area (canal walls) left out"""
    mpp = 2 * job.E / N
    r = int(max(near_m, 12.0) / mpp)
    near = cv2.dilate(area, np.ones((2 * r + 1,) * 2, np.uint8))
    from mm_render import footprint, walk_area
    cps = [job.to_px(c["pos"][0], c["pos"][2]) for c in job.cps]
    out = []
    for p in pieces:
        c = p["px"].reshape(-1, 2).mean(0).astype(int).clip(0, N - 1)
        pts = p["px"].reshape(-1, 2).astype(int).clip(0, N - 1)
        if not near[pts[:, 1], pts[:, 0]].any():
            continue
        inside = area[pts[:, 1], pts[:, 0]].mean()
        hull = cv2.convexHull(p["px"].reshape(-1, 2).astype(np.float32))
        on_cp = any(cv2.pointPolygonTest(hull, (float(q[0]), float(q[1])), True) >= -10.0 / mpp for q in cps)
        deck = footprint(p) >= 300 and walk_area(p) >= 0.5 * footprint(p)
        landmark = footprint(p) >= 3000          # a palace beside the plaza
        if not (on_cp or inside >= 0.3 or deck or landmark):
            continue                  # stock leaves out what stands off the play area (towers along the canal)
        out.append(p)
    return out


def keep_near(job, layers, area, N, near_m=6.0):
    """buildings on or next to the play area; low walls down in canals beside it left out"""
    mpp = 2 * job.E / N
    r = int(near_m / mpp)
    near = cv2.dilate(area, np.ones((2 * r + 1,) * 2, np.uint8))
    lo = min(c["pos"][1] for c in job.cps) - 5 if job.cps else -1e9
    out = []
    for top, polys, ps in layers:
        pts = np.concatenate([pp for pp, hh in polys]).astype(int).clip(0, N - 1)
        if not near[pts[:, 1], pts[:, 0]].any():
            continue
        if not area[pts[:, 1], pts[:, 0]].any() and top < lo:
            continue
        out.append((top, polys, ps))
    return out


def keep_layers(job, layers, area, N, near_m=6.0):
    """layers with any piece passing near_pieces (so a palace keeps the add-ons merged into it)"""
    ok = {id(p) for p in near_pieces(job, [p for l in layers for p in l[2]], area, N, near_m)}
    keep = [any(id(p) in ok for p in l[2]) for l in layers]
    # what stands up out of a sunken part, joined to the kept town (the naboo2 rotunda on its bridge), is kept too;
    # low canal walls (no higher than the plaza) are not
    if job.cps:
        mpp = 2 * job.E / N
        plaza = float(np.median([c["pos"][1] for c in job.cps]))
        r = max(1, int(round(2.0 / mpp)))
        k = np.ones((2 * r + 1,) * 2, np.uint8)
        masks = [LMask(l[1], N) for l in layers]
        for _ in range(4):
            u = union_full([m for m, kp in zip(masks, keep) if kp], N).astype(np.uint8)
            u = cv2.dilate(u, k) > 0
            grew = False
            for i, l in enumerate(layers):
                if not keep[i] and l[0] >= plaza + 2.0 and masks[i].sample(u):
                    keep[i] = grew = True
            if not grew:
                break
    return [l for l, kp in zip(layers, keep) if kp]


def open_under(job, p, N):
    """a roof on pillars (naboo2 colonnade): at no height near the ground do its walls close round its footprint.
    Slices of the upright faces are taken 0-12 m above walking level (the lowest CP); each is closed 1 m and its
    holes filled; the best enclosed share of the footprint decides it."""
    if "open" in p:
        return p["open"]
    t = p["tri"]
    yw = min(c["pos"][1] for c in job.cps) - 1.0 if job.cps else t[..., 1].min()
    y0 = max(t[..., 1].min(), yw)
    mpp = 2 * job.E / N
    px = p["px"]
    x0 = int(np.floor(px[..., 0].min())) - 4; yy0 = int(np.floor(px[..., 1].min())) - 4
    x1 = int(np.ceil(px[..., 0].max())) + 5; yy1 = int(np.ceil(px[..., 1].max())) + 5
    off = np.array([x0, yy0])
    full = tri_mask(px - off, (yy1 - yy0, x1 - x0))
    full = cv2.morphologyEx(full, cv2.MORPH_CLOSE, np.ones((3, 3), np.uint8))
    nf = max(1, (full > 0).sum())
    n_ = np.cross(t[:, 1] - t[:, 0], t[:, 2] - t[:, 0])
    upright = np.abs(n_[:, 1]) < 0.5 * (np.linalg.norm(n_, axis=1) + 1e-9)
    k = max(1, int(round(1.5 / mpp)))       # closes doorways
    best = 0.0
    for lv in (0.5, 2.5, 4.5, 6.5, 8.5, 10.5):
        sel = (t[..., 1].min(1) < y0 + lv + 2.5) & (t[..., 1].max(1) > y0 + lv) & upright
        if not sel.any():
            continue
        gp = job.to_px(t[sel][..., 0], t[sel][..., 2]) - off
        m = np.zeros(full.shape, np.uint8)
        for tri in np.round(gp * 16).astype(np.int32):
            cv2.polylines(m, [tri], True, 255, 1, cv2.LINE_8, 4)
        m = cv2.morphologyEx(m, cv2.MORPH_CLOSE, np.ones((2 * k + 1,) * 2, np.uint8))
        # walls on three sides (an open shop front) still make a building: measure the hull of the walls
        pts = np.argwhere(m > 0)[:, ::-1].astype(np.float32)
        if len(pts) >= 3:
            hm = np.zeros_like(m)
            cv2.fillPoly(hm, [cv2.convexHull(pts).astype(np.int32)], 255)
            best = max(best, ((hm > 0) & (full > 0)).sum() / nf)
    p["ratio"] = best
    # only pieces standing at walking level (canal pieces rise from far below and are solid walls)
    p["open"] = bool(best < 0.35 and t[..., 1].min() > yw - 2.0)
    return p["open"]


def is_open(job, layer, N):
    return all(open_under(job, p, N) for p in layer[2])
