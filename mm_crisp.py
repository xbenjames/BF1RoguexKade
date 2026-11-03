"""Crisp vector drawing for the minimaps (the stock maps are drawn by hand: straight edges, few shapes).
- every building is traced to a polygon and simplified (straight edges, true rectangles snapped)
- the play area is the ground you can actually walk to from the CPs, walls and buildings block it
- clutter (props, plants, lamps, crates, tiny bits) is left out"""
import re
import numpy as np
import cv2

CLUTTER = re.compile(r"planter|lamp|lampost|post|tree|leaf|flower|plant|bush|shrub|grass|fern|vine|bench|chair|table|crate|barrel|"
                     r"box|sign|banner|flag|light|torch|debris|rubble|rock|stone|pebble|weed|fx|mist|cloud|sky|dust|god|decal|"
                     r"billboard|reflection|turret|gun|console|terminal|panel|droid|speeder|ship|fighter|vehicle|spawn|recharge|item|holo", re.I)


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
    if a / (np.pi * r * r + 1e-9) >= 0.88 and r > 3 * eps_px:
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
        polys = [(pp, []) for pp, hh in polys]
    off = np.array([x0, y0], np.float32)
    return [(pp + off, [h + off for h in hh]) for pp, hh in polys]


def pmask(polys, N):
    m = np.zeros((N, N), np.uint8)
    draw_polys(m, polys, 255)
    return m > 127


def buildings(job, pieces, N, min_foot=20.0):
    """placed buildings -> [(top height, polygons)], lowest first, clutter left out.
    Each piece is traced on its own and simplified to straight edges; pieces of one class that overlap a lot
    are one shape, and wedges laid round a centre (a rotunda) become one round building."""
    mpp = 2 * job.E / N
    from mm_render import footprint
    bld = []
    for p in pieces:
        if CLUTTER.search(p["cls"]) or footprint(p) < min_foot:
            continue
        polys = piece_polys(job, p, eps_px=0.6 / mpp, min_area_px=6 / mpp ** 2, close_px=2)
        if polys:
            bld.append((p, polys))
    groups = {}
    for p, polys in bld:
        groups.setdefault(re.sub(r"_?\d+$", "", p["cls"].lower()), []).append((p, polys))
    layers = []
    for cls, items in groups.items():
        ms = [pmask(polys, N) for p, polys in items]
        par = list(range(len(items)))

        def f(i):
            while par[i] != i:
                i = par[i]
            return i
        if len(items) <= 60:
            for i in range(len(items)):
                for j in range(i + 1, len(items)):
                    inter = (ms[i] & ms[j]).sum()
                    if inter and inter > 0.15 * min(ms[i].sum(), ms[j].sum()):
                        par[f(i)] = f(j)
        if len(items) >= 4:
            u = np.zeros((N, N), np.uint8)
            for m_ in ms:
                u |= m_.astype(np.uint8)
            u = cv2.morphologyEx(u * 255, cv2.MORPH_CLOSE, np.ones((9, 9), np.uint8))
            cs, _ = cv2.findContours(u, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
            c = max(cs, key=cv2.contourArea)
            r = cv2.minEnclosingCircle(c)[1]
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
            m = np.zeros((N, N), bool)
            for i in idx:
                m |= ms[i]
            polys = simplify(m.astype(np.uint8) * 255, 0.6 / mpp, 6 / mpp ** 2)
            layers.append((max(items[i][0]["top"] for i in idx), [(pp, []) for pp, hh in polys], [items[i][0] for i in idx]))
    layers.sort(key=lambda l: l[0])
    return layers


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
    k10 = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (max(3, int(10 / mpp)),) * 2)
    area = cv2.morphologyEx(area, cv2.MORPH_OPEN, k10)
    if job.H is not None and job.cps:
        lo = min(c["pos"][1] for c in job.cps) - 5
        area[(job.H < lo) & (union == 0)] = 0
        area = cv2.morphologyEx(area, cv2.MORPH_OPEN, k10)
    return area, simplify(area, 1.5 / mpp, 400 / mpp ** 2, snap_rect=0.97)


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
