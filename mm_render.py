"""Battlefront minimap maker - drawing part.
style "bf1": stock BF1 look (blue, scanline background, ring), as worked out from bes1/end1/hoth/kamino.
style "bf2": stock BF2 look (grey floor plan the HUD tints), as worked out from the 19 stock BF2 maps."""
import os, re, sys, time
import numpy as np
import mm_crisp as cr
from PIL import Image, ImageDraw
from scipy import ndimage as ndi
import mm_core as mc


def res_path(name):
    base = getattr(sys, "_MEIPASS", os.path.dirname(os.path.abspath(__file__)))
    return os.path.join(base, "assets", name)


# ---------------------------------------------------------------- raster helpers
SQ = np.ones((3, 3), bool)
CROSS = ndi.generate_binary_structure(2, 1)


def morph(m, r, grow):
    """octagon kernel radius r (alternate 3x3 square / cross), outside counts as empty"""
    m = m.astype(bool)
    for k in range(int(r)):
        st = SQ if k % 2 == 0 else CROSS
        m = ndi.binary_dilation(m, st) if grow else ndi.binary_erosion(m, st, border_value=0)
    return m


def erode_sq(m, r):
    if r <= 0:
        return m.astype(bool)
    return ndi.minimum_filter(m.astype(np.uint8), size=2 * r + 1, mode="constant", cval=0).astype(bool)


def dilate_sq(m, r):
    if r <= 0:
        return m.astype(bool)
    return ndi.maximum_filter(m.astype(np.uint8), size=2 * r + 1, mode="constant", cval=0).astype(bool)


def drop_small(m, min_pix, val):
    """val=1: remove specks smaller than min_pix; val=0: fill holes smaller than min_pix not touching the edge"""
    m = m.astype(bool)
    tgt = m if val else ~m
    lab, n = ndi.label(tgt)
    if n == 0:
        return m
    size = np.bincount(lab.ravel())
    kill = size < min_pix
    kill[0] = False
    if not val:
        edge = np.unique(np.r_[lab[0], lab[-1], lab[:, 0], lab[:, -1]])
        kill[edge] = False
    k = kill[lab]
    out = m.copy()
    out[k] = not val
    return out


def box_blur(f, r):
    r = max(1, int(round(r)))
    f = f.astype(np.float32)
    for _ in range(3):
        f = ndi.uniform_filter(f, 2 * r + 1, mode="nearest")
    return f


def smooth_mask(m, r):
    return box_blur(m.astype(np.float32), r) > 0.5


def clean(m, closeR, openR, minPix, holePix, smooth):
    m = morph(morph(m, closeR, True), closeR, False)
    m = morph(morph(m, openR, False), openR, True)
    m = drop_small(m, minPix, 1)
    m = drop_small(m, holePix, 0)
    return smooth_mask(m, smooth)


def fill_tris(N, tris_px, mode="L", value=255, img=None):
    """tris_px: (k,3,2) pixel coords"""
    im = img if img is not None else Image.new(mode, (N, N), 0)
    d = ImageDraw.Draw(im)
    a = tris_px
    area = np.abs((a[:, 1, 0] - a[:, 0, 0]) * (a[:, 2, 1] - a[:, 0, 1]) - (a[:, 2, 0] - a[:, 0, 0]) * (a[:, 1, 1] - a[:, 0, 1]))
    for t in a[area > 1e-3]:
        d.polygon([(t[0, 0], t[0, 1]), (t[1, 0], t[1, 1]), (t[2, 0], t[2, 1])], fill=value)
    return im


def mix(a, b, t):
    return np.array(a, np.float32) * (1 - t) + np.array(b, np.float32) * t


def write_tga(path, rgb):
    """24-bit uncompressed, bottom-left origin (as stock)"""
    a = np.asarray(rgb.convert("RGB"))
    h, w = a.shape[:2]
    hdr = bytes([0, 0, 2] + [0] * 9) + w.to_bytes(2, "little") + h.to_bytes(2, "little") + bytes([24, 0])
    with open(path, "wb") as f:
        f.write(hdr + a[::-1, :, ::-1].tobytes())


# ---------------------------------------------------------------- settings / preparation
class Options:
    def __init__(self, **kw):
        self.style = "auto"          # auto | bf1 | bf2   (look)
        self.game = "auto"           # auto | bf1 | bf2   (whose box rule: BF1 = CPs, BF2 = mapbounds)
        self.mode = None             # BF2 game mode from the .ldx (None = Conquest)
        self.layers = None           # set of layer keys (None = default for the mode)
        self.cp_off = None           # set of cp keys left out of the box
        self.class_modes = {}        # lower class -> struct | rock | out
        self.ground = "auto"         # auto | on | off
        self.contours = 10           # BF1 height lines (0 = off)
        self.size = None             # texture size (None: BF1 1024, BF2 512)
        self.min_area = 25.0
        self.box = None              # (E, CX, CZ) override
        self.rotate180 = "auto"      # auto | yes | no  (BF2 SetMapNorthAngle(180))
        self.bf1_look = "auto"       # auto | floating | town | open | ground
        self.cp_rings = False        # draw each CP's capture region (stock BF2 geo1 does)
        self.__dict__.update(kw)


def cp_key(c):
    return "%s|%s|%s" % (c["layer"], c["name"], ",".join("%.1f" % v for v in c["pos"]))


def rule_box(cps):
    xs = [c["pos"][0] for c in cps]
    zs = [c["pos"][2] for c in cps]
    return dict(CX=(min(xs) + max(xs)) / 2, CZ=(min(zs) + max(zs)) / 2,
                E=max(max(xs) - min(xs), max(zs) - min(zs)) / 2 + 100)


def find_north180(world):
    """BF2: SetMapNorthAngle(180) without the second argument rotates the map texture 180 deg (tan1).
    Looks for the world's mission scripts among the loaded files."""
    stem = re.sub(r"\d+$", "", world.lname)[:3]
    num = re.findall(r"\d+", world.lname)
    pre = stem + (num[0] if num else "")
    for p, lo, n in world.ix.files:
        if n.endswith(".lua") and n.startswith(pre):
            t = mc.read_text(p)
            m = re.search(r"SetMapNorthAngle\(\s*([-\d.]+)\s*(,\s*(\d+))?\s*\)", t)
            if m:
                ang = float(m.group(1))
                return abs(ang - 180) < 1 and not m.group(3), os.path.basename(p), m.group(0)
    return None, None, None


class Job:
    """Everything about one world + options that both styles need."""
    def __init__(self, world, opt, log=print):
        self.w, self.o, self.log = world, opt, log
        w = world
        self.game = opt.game if opt.game != "auto" else w.game
        self.style = opt.style if opt.style != "auto" else self.game
        self.layers = opt.layers if opt.layers is not None else w.default_layers(opt.mode)
        self.objs = w.objects(self.layers)
        self.all_cps = [o for o in self.objs if w.is_cp(o)]
        if opt.cp_off is None:
            opt.cp_off = {cp_key(c) for c in self.all_cps if mc.OTHER_MODE.search(c["name"]) or mc.OTHER_MODE.search(c["cls"])}
        self.cps = [c for c in self.all_cps if cp_key(c) not in opt.cp_off]
        self.high_outside = False

    def object_mode(self, ob):
        """user's choice for the class > what the layer says > what the class name / ODF says"""
        c = ob["cls"].lower()
        if c in self.o.class_modes:
            return self.o.class_modes[c]
        base = self.w.class_mode(ob["cls"])
        if base == "out" and (self.w.odf(ob["cls"]) or {}).get("label") in mc.SKIP_LABELS:
            return "out"                        # CPs, doors, spawns, soldiers stay out whatever layer they're in
        hint = mc.layer_hint(ob["layer"])
        if hint == "struct" and base == "out" and not re.search(r"tree|bush|plant|leaf|grass|fern|flower|vine|mist|light|bamboo|weed", c):
            return "struct"
        if hint in ("rock", "out") and base == "struct":
            return hint
        return base

    def class_mode(self, cls):
        return self.o.class_modes.get(cls.lower()) or self.w.class_mode(cls)

    def box(self):
        o, w = self.o, self.w
        if o.box:
            E, CX, CZ = o.box
            return dict(E=E, CX=CX, CZ=CZ, src="manual box")
        if self.game == "bf2":
            mb = w.mapbounds()
            if mb:
                return dict(E=max(mb["size"][0], mb["size"][2]), CX=mb["pos"][0], CZ=mb["pos"][2],
                            src="BF2 mapbounds region (%s)" % mb["file"])
        if not self.cps:
            raise ValueError("No command posts found, so the map box can't be worked out. Tick the layer with the CPs, or enter the box by hand.")
        b = rule_box(self.cps)
        b["src"] = ("BF2" if self.game == "bf2" else "BF1") + " CP rule"
        return b

    def prepare(self, N):
        """box, terrain, triangles in pixel space"""
        w, o, log = self.w, self.o, self.log
        b = self.box()
        E, CX, CZ = b["E"], b["CX"], b["CZ"]
        self.E, self.CX, self.CZ, self.N = E, CX, CZ, N
        log("World %s (made for %s, %s style): %s, %d CPs -> box E %.1f centre (%.1f, %.1f)" %
            (w.name, self.game.upper(), self.style.upper(), b["src"], len(self.cps), E, CX, CZ))
        rot = False
        if self.game == "bf2":
            if o.rotate180 == "yes":
                rot = True
            elif o.rotate180 == "auto":
                r, f, call = find_north180(w)
                if r:
                    rot = True
                    log("%s in %s: texture turned 180 degrees (like stock tan1)." % (call, f))
        self.rot = rot
        c = self.cps
        if len(c) > 3:
            mx = np.mean([p["pos"][0] for p in c]); mz = np.mean([p["pos"][2] for p in c])
            d = [np.hypot(p["pos"][0] - mx, p["pos"][2] - mz) for p in c]
            med = sorted(d)[len(d) // 2]
            for p, dd in zip(c, d):
                if dd > 2.5 * med and dd > 150:
                    log('CP "%s" is far from the others and stretches the map. If it belongs to another game mode, untick it.' % (p["name"] or p["cls"]))
        self.ter = None
        if w.ter_path:
            try:
                self.ter = w.terrain(w.objects(list(w.objs.keys())))   # every layer: trees and rocks pin the height grid best
                t = self.ter
                log("Terrain %s: %dx%d grid, %g m cells%s" % (os.path.basename(w.ter_path), t.n, t.n, t.gs,
                    "" if t.ho == t.hdr else " (heights found at byte %d, not %d)" % (t.ho, t.hdr)))
            except Exception as e:
                log("Terrain skipped: %s" % e)
        elif w.ter_name:
            log("Terrain %s not found; drawing without ground." % w.ter_name)
        # objects -> pixel triangles
        missing = set()
        self.pieces, self.rocks = [], []
        near = allc = 0
        for ob in self.objs:
            r = w.odf(ob["cls"])
            if self.ter is not None and not (r and r["label"] == "commandpost"):
                allc += 1
                if abs(ob["pos"][1] - float(self.ter.sample(ob["pos"][0], ob["pos"][2]))) < 2.5:
                    near += 1
            mode = self.object_mode(ob)
            if mode == "out":
                continue
            tri, geo = w.object_tris(ob)
            if tri is None:
                missing.add(geo)
                continue
            if not len(tri):
                continue
            x0, x1 = tri[..., 0].min(), tri[..., 0].max()
            z0, z1 = tri[..., 2].min(), tri[..., 2].max()
            if (x1 - x0) * (z1 - z0) < o.min_area:
                continue
            if (x1 - x0) * (z1 - z0) > 0.5 * (2 * E) ** 2 and len(self.objs) > 5:
                # a whole-world shell (mus1 lava fields, skyboxes): stock never draws these
                log("Left out %s: it covers most of the map (world shell / backdrop)." % ob["cls"])
                continue
            item = dict(tri=tri, px=self.to_px(tri[..., 0], tri[..., 2]), top=float(tri[..., 1].max()),
                        ymin=float(tri[..., 1].min()), cls=ob["cls"], pos=ob["pos"])
            (self.rocks if mode == "rock" else self.pieces).append(item)
        if missing:
            m = sorted(missing)
            log("%d model%s not found (add the folder holding them if they matter): %s%s" %
                (len(m), "s" if len(m) > 1 else "", ", ".join(m[:12]), " ..." if len(m) > 12 else ""))
        log("%d buildings/structures, %d rock/plant objects" % (len(self.pieces), len(self.rocks)))
        g = o.ground
        if g == "auto":
            nc = 0
            if self.ter is not None:
                for cp in self.cps:
                    if abs(cp["pos"][1] - float(self.ter.sample(cp["pos"][0], cp["pos"][2]))) < 5:
                        nc += 1
            frac = nc / len(self.cps) if self.cps else 0
            ofrac = near / allc if allc else 0
            isl = False
            if self.ter is not None and getattr(self.ter, "water_h", None) is not None and self.ter.h is not None:
                a = (self.ter.h > self.ter.water_h).mean()
                isl = 0.05 <= a <= 0.95 and allc and near / allc >= 0.1   # islands in a sea (kas2): the land shows even if the CPs stand on platforms
            g = "on" if (self.ter is not None and (frac >= 0.34 or ofrac >= 0.25 or isl)) else "off"
            log("Ground: %s (%d of %d CPs and %d%% of objects stand on the terrain%s)" %
                ("drawn" if g == "on" else "hidden", nc, len(self.cps), round(ofrac * 100), "" if self.ter is not None else "; no terrain loaded"))
        self.ground = g
        # world coordinates of every pixel centre (for terrain sampling)
        u = (np.arange(N) + 0.5) / N - 0.5
        U, V = np.meshgrid(u, -u)
        if self.rot:
            U, V = -U, -V
        self.WX = CX + U * 2 * E
        self.WZ = CZ + V * 2 * E
        self.H = self.ter.sample(self.WX, self.WZ).astype(np.float32) if self.ter is not None else None

    def to_px(self, x, z):
        u = (np.asarray(x) - self.CX) / (2 * self.E)
        v = (np.asarray(z) - self.CZ) / (2 * self.E)
        if self.rot:
            u, v = -u, -v
        return np.stack([(u + 0.5) * self.N, (0.5 - v) * self.N], -1)

    def land_mask(self, U):
        """playable land: union of boundary paths, else terrain above its lowest level; sea removed"""
        N, w, log = self.N, self.w, self.log
        land = np.zeros((N, N), bool)
        polys = w.boundary
        log("Playable area: %d boundary path%s" % (len(polys), "" if len(polys) == 1 else "s"))
        if polys:
            im = Image.new("L", (N, N), 0); d = ImageDraw.Draw(im)
            for p in polys:
                q = self.to_px([a[0] for a in p], [a[2] for a in p])
                d.polygon([tuple(v) for v in q], fill=255)
            land = box_blur(np.asarray(im) / 255.0, max(1, round(3 * U))) > 0.5
            if self.style == "bf1" and self.H is not None and (~land).any():
                # BF1 hoth: the canyon walls outside the boundary are drawn too (as darker high ground);
                # BF1 end1: flat forest outside the boundary is not. Tell them apart by how high the outside is.
                out_h = float(np.median(self.H[~land])) - self.play_height()
                if out_h > 29:
                    land[:] = True
                    self.high_outside = True
                    log("The ground outside the boundary is high (%.0f m up): drawn as high ground, like stock BF1 Hoth." % out_h)
        elif self.H is not None and np.percentile(self.H, 95) - np.percentile(self.H, 5) > 40 and self.cps:
            # mountain maps without a boundary (BF1 RhenVar): the land is the plateau the CPs stand on
            band = np.abs(box_blur(self.H, max(1, round(2 * U))) - self.play_height()) < 12
            band = morph(morph(band, round(3 * U), False), round(3 * U), True)
            lab, n = ndi.label(band)
            keep = set()
            for c in self.cps:
                q = self.to_px(c["pos"][0], c["pos"][2]).astype(int)
                if 0 <= q[0] < N and 0 <= q[1] < N and lab[q[1], q[0]]:
                    keep.add(lab[q[1], q[0]])
            land = np.isin(lab, list(keep)) if keep else band
            land = drop_small(land, (30 * U) ** 2, 0)
            log("No boundary; land = the plateau around the CPs (within 12 m of their height).")
        elif self.H is not None:
            lo = self.H.min()
            land = box_blur((self.H > lo + 2).astype(np.float32), max(1, round(U))) > 0.5
            log("No boundary found; land = terrain above its lowest level.")
        else:
            land[:] = True
            log("No boundary or terrain; the whole box counts as ground.")
        self.water = np.zeros((N, N), bool)
        wh = getattr(self.ter, "water_h", None) if self.ter is not None else None
        if self.H is not None and wh is not None and 0.05 <= (self.H < wh).mean() < 0.95:
            # the terrain's own water layer (kashyyyk1 lake, kas2 sea): land is what stands above it
            keep = box_blur((self.H > wh).astype(np.float32), max(1, round(1.5 * U))) > 0.5
            self.water = ~keep
            # the islands are drawn whole, beyond the boundary too (stock kas2, kashyyyk1)
            land = keep.copy()
            self.island = True
            log("Water layer at %.1f m: %d%% of the box is under water." % (wh, round((~keep).mean() * 100)))
        elif self.H is not None:
            lo = self.H.min()
            flat = (self.H < lo + 1).mean()
            if 0.15 <= flat < 0.9:
                keep = box_blur((self.H > lo + 1).astype(np.float32), max(1, round(1.5 * U))) > 0.5
                self.water = ~keep
                land &= keep
                log("Water: %d%% of the map sits at the lowest level, drawn as sea." % round(flat * 100))
        return land

    def object_spread(self, R):
        import cv2
        N = self.N
        mpp = 2 * self.E / N
        m = np.zeros((N, N), np.uint8)
        r = max(1, int(round(R / mpp)))
        for o in self.objs:
            q = self.to_px(o["pos"][0], o["pos"][2])
            if -r <= q[0] < N + r and -r <= q[1] < N + r:
                cv2.circle(m, (int(q[0]), int(q[1])), r, 255, -1)
        k = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (2 * r + 1,) * 2)
        m = cv2.morphologyEx(m, cv2.MORPH_CLOSE, k)
        ff = np.pad(m, 1).copy()
        cv2.floodFill(ff, None, (0, 0), 128)
        m = np.where(ff[1:-1, 1:-1] == 128, 0, 255).astype(np.uint8)
        m = drop_small(m > 0, (8 * r) ** 2, 1)
        return box_blur(m.astype(np.float32), max(1, int(1.5 * r))) > 0.5

    def play_height(self):
        ys = sorted(c["pos"][1] for c in self.cps)
        return ys[len(ys) // 2] if ys else 0.0


# ---------------------------------------------------------------- BF1 style
def glow_bg(N):
    """BF1 open-ground background (geo1, tat1, kashyyyk1, naboo1): a soft blue glow, no scanlines"""
    u = (np.arange(N) + 0.5) / N - 0.5
    r = np.hypot(u[None, :], u[:, None]) / 0.7071
    t = np.clip(r / 0.85, 0, 1)[..., None] ** 0.9
    c0, c1 = np.array([38, 98, 182], np.float32), np.array([11, 40, 86], np.float32)
    return c0 * (1 - t) + c1 * t


def badges(job, img, pieces, U, clip=None):
    """BF1 floating-map pieces (Kamino / Bespin): light rim, blue band, navy middle; thin pieces as light bars.
    Painted lowest first so decks sit on their bridges."""
    N = job.N
    NAVY = [17, 58, 121]
    KRIM, KBAND, KLINE, KLIGHT = [129, 170, 236], [34, 84, 161], [75, 122, 198], [91, 139, 217]
    band, thin = round(6 * U), round(3 * U)
    rim, line = max(1, round(U)), max(1, round(U * 0.8))
    drawn = 0
    for p in sorted(pieces, key=lambda p: p["top"]):
        px = p["px"]
        x0 = int(np.floor(px[..., 0].min() - band - 6)); y0 = int(np.floor(px[..., 1].min() - band - 6))
        x1 = int(np.ceil(px[..., 0].max() + band + 6)); y1 = int(np.ceil(px[..., 1].max() + band + 6))
        w_, h_ = x1 - x0, y1 - y0
        if w_ <= 0 or h_ <= 0 or x1 < 0 or y1 < 0 or x0 > N or y0 > N:
            continue
        im = Image.new("L", (w_, h_), 0)
        m = np.asarray(fill_tris(0, px - np.array([x0, y0]), img=im)) > 127
        m = morph(morph(m, round(U), True), round(U), False)
        m = morph(morph(m, round(U * 0.5), False), round(U * 0.5), True)
        m = drop_small(m, (3 * U) ** 2, 1); m = drop_small(m, (6 * U) ** 2, 0)
        m = smooth_mask(m, max(1, round(U * 0.6)))
        if m.sum() < (3 * U) ** 2:
            continue
        r1 = morph(m, rim, False); core = morph(m, band, False)
        is_thin = not morph(m, thin, False).any()
        core2 = morph(core, line, False) if core.any() else core
        col = np.zeros((h_, w_, 3), np.float32)
        col[:] = NAVY
        col[~core2] = KLINE
        col[~core] = KBAND
        if is_thin:
            col[:] = KLIGHT
        col[~r1] = KRIM
        ys, xs = np.nonzero(m)
        Y, X = ys + y0, xs + x0
        ok = (X >= 0) & (X < N) & (Y >= 0) & (Y < N)
        if clip is not None:
            ok[ok] &= clip[Y[ok], X[ok]]
        img[Y[ok], X[ok]] = col[ys[ok], xs[ok]]
        drawn += 1
    return drawn


def render_bf1(job):
    """Stock BF1 has four looks, picked here the way the stock maps use them:
       floating  - no ground (bes1, kamino1): every piece a badge on the scanlines
       town      - buildings cover the play area (bespin2, naboo2, tat2): scanlines, the walled area dark navy, buildings as badges
       open      - wide open ground, few buildings (geo1, tat1, naboo1, kashyyyk1): blue glow instead of scanlines,
                   no land fill (except a coastline), objects as badges
       ground    - (hoth, end1, yavin1, RhenVar): scanlines, the play land filled, height lines, buildings merged and light"""
    o, log = job.o, job.log
    OUT = o.size or 1024
    SS = 2
    N = OUT * SS
    U = N / 512.0
    job.prepare(N)
    FILL, RIM, NAVY, EDGE = [50, 110, 201], [137, 178, 242], [17, 58, 121], [166, 200, 255]
    DARK, DARKRIM, BLD = [25, 71, 141], [77, 124, 199], [89, 156, 255]
    look = "floating"
    land = np.zeros((N, N), bool)
    if job.ground == "on":
        land = job.land_mask(U)
        big = [p for p in job.pieces if walk_area(p) >= 150]
        foot = np.asarray(fill_tris(N, np.concatenate([p["px"] for p in big]))) > 127 if big else np.zeros((N, N), bool)
        cover = (foot & land).sum() / max(1, land.sum())
        if o.bf1_look and o.bf1_look != "auto":
            look = o.bf1_look
        elif land.mean() < 0.02:
            look = "floating"          # no usable ground under the buildings (myg1, tan1, uta1 interiors)
        elif cover >= 0.3:
            look = "town"
        elif getattr(job, "island", False) or ((land.mean() >= 0.65 or (job.water.any() and land.mean() >= 0.3)) and cover < 0.15 and not job.high_outside):
            look = "open"
        else:
            look = "ground"
        log("BF1 look: %s (land %d%% of the box, buildings cover %d%% of it)" % (look, round(land.mean() * 100), round(cover * 100)))
    if look == "open":
        img = glow_bg(N)
    else:
        bg = Image.open(res_path("bf1_bg.png")).convert("RGB").resize((N, N), Image.NEAREST)
        img = np.asarray(bg).astype(np.float32).copy()
    drawn = 0
    if look == "ground" and job.w.boundary and max(len(b) for b in job.w.boundary) <= 6 and not job.high_outside and not job.water.any():
        # stock BF1 draws the land the level was built on: where things were placed (end1's forest island,
        # a boundary of a few points drawn loosely round it). The loose boundary and empty terrain beyond it are left out.
        spread = job.object_spread(8.0)
        if spread.any() and (spread & land).sum() < 0.85 * land.sum():
            land &= spread
            log("Land cut to where objects were placed (%d%% of the box)." % round(land.mean() * 100))
    if look == "ground":
        base = FILL if job.high_outside else DARK      # hoth: light snow field; jungle/forest/rock maps: darker land
        img[land] = base
        step = o.contours
        if step and step > 0 and job.H is not None:
            H = box_blur(job.H, max(1, round(2 * U)))
            lev = np.floor(H / step).astype(np.int32)
            playH = job.play_height()
            HIGH = [25, 71, 141] if job.high_outside else NAVY
            hm = land & (H > playH + 20)
            hm = morph(hm, round(2 * U), False); hm = morph(hm, round(2 * U), True)
            hm = drop_small(hm, (10 * U) ** 2, 1)
            bandv = np.floor(np.abs(H - playH) / step)
            sel = land & ~hm & (bandv > 0)
            t = np.minimum(0.3, bandv * 0.1)[sel][:, None]
            img[sel] = np.array(base, np.float32) * (1 - t) + np.array(NAVY, np.float32) * t
            img[land & hm] = HIGH
            hin = morph(hm, max(1, round(U)), False)
            img[hm & ~hin & land] = mix(base, RIM, 0.6)
            ed = np.zeros((N, N), bool)
            ed[:-1, :-1] = (lev[:-1, :-1] != lev[:-1, 1:]) | (lev[:-1, :-1] != lev[1:, :-1])
            ed = dilate_sq(ed, max(0, round(U / 2) - 1))
            img[ed & land & ~hm] = mix(base, RIM, 0.45)
        if job.rocks:
            m = np.asarray(fill_tris(N, np.concatenate([r["px"] for r in job.rocks]))) > 127
            m = clean(m, round(3 * U), round(1.5 * U), (8 * U) ** 2, (10 * U) ** 2, U)
            inner = erode_sq(m, max(1, round(U * 0.8)))
            img[m & land & inner] = NAVY
            img[m & land & ~inner] = DARKRIM
        if job.pieces:
            layers = cr.buildings(job, job.pieces, N)
            if job.w.boundary:
                # stock leaves out buildings beyond the play area (tat2): keep what stands mostly on the land
                lm = land
                layers = [l for l in layers if cr.LMask(l[1], N).sample(lm) >= 0.5 * max(1, cr.LMask(l[1], N).area)]
            for top, polys, _ in layers:
                cr.draw_polys(img, polys, BLD, EDGE, rim_w=U)
            drawn = len(layers)
        img[land & ~erode_sq(land, max(1, round(U)))] = EDGE
    elif look == "town":
        if job.w.boundary:
            img[land] = NAVY
            img[land & ~erode_sq(land, max(1, round(U)))] = EDGE
            # only what stands mostly inside the walls: the towers of the skyline beyond them are scenery (bespin2)
            lm = land.astype(np.uint8)
            def inside(l):
                m = cr.LMask(l[1], N)
                return m.sample(lm > 0) >= 0.4 * max(1, m.area)
            layers = [l for l in cr.buildings(job, job.pieces, N) if inside(l)]
            for l in sorted(layers, key=lambda l: (not cr.is_floor(job, l), cr.is_dark(l))):
                if cr.is_dark(l):
                    col = [34, 84, 161]
                elif cr.is_floor(job, l):
                    col = NAVY
                else:
                    col = FILL
                cr.draw_polys(img, l[1], col, RIM, rim_w=U)
            drawn = len(layers)
        else:
            # no walls (naboo2): the town block (buildings + the plazas they enclose, holding the CPs) dark navy,
            # each building a crisp light shape with a pale rim, clutter (planters, trees, lamps) left out
            layers = cr.buildings(job, job.pieces, N)
            area, apoly = cr.town(job, layers, N)
            cr.draw_polys(img, apoly, NAVY, RIM, rim_w=U)
            layers = cr.keep_layers(job, layers, area, N)
            for l in sorted(layers, key=lambda l: (not cr.is_floor(job, l), cr.is_dark(l))):
                if cr.is_dark(l):
                    col = [34, 84, 161]
                elif cr.is_floor(job, l):
                    col = NAVY
                else:
                    col = FILL
                cr.draw_polys(img, l[1], col, RIM, rim_w=U)
            drawn = len(layers)
    elif look == "open":
        if job.water.any():
            img[land] = FILL
            img[land & ~erode_sq(land, max(1, round(U)))] = EDGE
        if job.rocks:
            m = np.asarray(fill_tris(N, np.concatenate([r["px"] for r in job.rocks]))) > 127
            m = clean(m, round(3 * U), round(1.5 * U), (8 * U) ** 2, (10 * U) ** 2, U)
            inner = erode_sq(m, max(1, round(U * 0.8)))
            img[m & inner] = NAVY
            img[m & ~inner] = RIM
        drawn = badges(job, img, [p for p in job.pieces if walk_area(p) >= 40], U)
    else:  # floating
        if job.rocks:
            m = np.asarray(fill_tris(N, np.concatenate([r["px"] for r in job.rocks]))) > 127
            m = clean(m, round(3 * U), round(1.5 * U), (8 * U) ** 2, (10 * U) ** 2, U)
            inner = erode_sq(m, max(1, round(U * 0.8)))
            img[m & inner] = DARK
            img[m & ~inner] = DARKRIM
        drawn = badges(job, img, job.pieces, U)
    small = img.reshape(OUT, SS, OUT, SS, 3).mean((1, 3))
    out = Image.fromarray(np.clip(small + 0.5, 0, 255).astype(np.uint8))
    ring = Image.open(res_path("bf1_ring.png")).convert("RGBA").resize((OUT, OUT), Image.BICUBIC)
    out = Image.alpha_composite(out.convert("RGBA"), ring).convert("RGB")
    log("%d structures drawn." % drawn)
    return out


# ---------------------------------------------------------------- BF2 style
# Stock BF2 maps (cor1 dag1 dea1 end1 fel1 geo1 hoth kamino1 kas2 mus1 myg1 naboo2 pol1 tan1 tat2 tat3 uta1 yavin1):
#  * grey only - the HUD tints it; 512x512 24-bit, "-8bit -maps 1"
#  * outside the play area: flat dark grey (~44) on outdoor maps, black on interior / space maps
#  * the walkable ground: mid grey (~125-150), the edge of the play area a thin light line
#  * low ground / water / pits darker (~60-75), cliffs and walls you can't climb are left out (background)
#  * buildings drawn as floor plans: walkable floors flat light grey, higher decks lighter,
#    every step in height outlined with a light 1-2 px line, pillars/props as small light blocks
BG_OUT, BG_IN = 44.0, 3.0


def floor_raster(job, N):
    """per-pixel walkable floor height (NaN = none): the highest upward-facing surface that is at most
    STEP m above the lowest one there - so floors show, roofs and ceilings don't."""
    STEP = 3.5
    tris = [p["tri"] for p in job.pieces]
    if not tris:
        return np.full((N, N), np.nan, np.float32), np.zeros((N, N), bool)
    T = np.concatenate(tris)
    nrm = np.cross(T[:, 1] - T[:, 0], T[:, 2] - T[:, 0])
    ln = np.linalg.norm(nrm, axis=1) + 1e-9
    ny = nrm[:, 1] / ln
    # the mesh is mirrored in x (that's how the engine places it), so winding flips: accept both signs
    up = np.abs(ny) > 0.6
    px = job.to_px(T[..., 0], T[..., 2])
    hgt = T[..., 1].mean(1)
    # any geometry at all (walls included) -> footprint
    foot = np.asarray(fill_tris(N, px)) > 127
    sel = np.nonzero(up)[0]
    if not len(sel):
        return np.full((N, N), np.nan, np.float32), foot
    order = sel[np.argsort(-hgt[sel])]               # draw high first, lowest ends on top
    low = Image.new("F", (N, N), float("nan"))
    fill_by_value(low, px[order], hgt[order])
    low = np.asarray(low, np.float32)
    top = low.copy()
    # height bands, ascending: a band paints where it is within STEP of the lowest floor
    lo_h, hi_h = np.nanmin(hgt[sel]), np.nanmax(hgt[sel])
    edges_ = np.arange(np.floor(lo_h), np.ceil(hi_h) + 1.0, 1.0)
    for b0 in edges_:
        bs = sel[(hgt[sel] >= b0) & (hgt[sel] < b0 + 1)]
        if not len(bs):
            continue
        bs = bs[np.argsort(hgt[bs])]
        im = Image.new("F", (N, N), float("nan"))
        fill_by_value(im, px[bs], hgt[bs])
        a = np.asarray(im, np.float32)
        ok = ~np.isnan(a) & (a <= low + STEP) & ~(a < top)
        top[ok] = a[ok]
    return top, foot


def fill_by_value(im, px, vals):
    d = ImageDraw.Draw(im)
    area = np.abs((px[:, 1, 0] - px[:, 0, 0]) * (px[:, 2, 1] - px[:, 0, 1]) - (px[:, 2, 0] - px[:, 0, 0]) * (px[:, 1, 1] - px[:, 0, 1]))
    for t, v in zip(px, vals):
        d.polygon([(t[0, 0], t[0, 1]), (t[1, 0], t[1, 1]), (t[2, 0], t[2, 1])], fill=float(v))


def footprint(p):
    t = p["tri"]
    return (t[..., 0].max() - t[..., 0].min()) * (t[..., 2].max() - t[..., 2].min())


def walk_area(p):
    """m² of upward-facing surface (decks, floors, roofs) - plants and poles have almost none"""
    if "walk" not in p:
        t = p["tri"]
        n = np.cross(t[:, 1] - t[:, 0], t[:, 2] - t[:, 0])
        a = 0.5 * np.linalg.norm(n, axis=1)
        ny = np.abs(n[:, 1]) / (2 * a + 1e-9)
        p["walk"] = float(a[ny > 0.6].sum())
    return p["walk"]


def cp_ring_mask(job, U):
    """(fill, ring) masks of every CP's capture region, from the CaptureRegion named on the CP in the .lyr"""
    N = job.N
    fill = np.zeros((N, N), bool)
    regs = {g["name"].lower(): g for g in job.w.regions}
    yy, xx = np.mgrid[0:N, 0:N]
    for c in job.cps:
        g = regs.get((c.get("capture") or "").lower())
        rad = max(g["size"][0], g["size"][2]) if g else 8.0
        cx, cz = (g["pos"][0], g["pos"][2]) if g else (c["pos"][0], c["pos"][2])
        p = job.to_px(cx, cz)
        rp = rad / (2 * job.E) * N
        fill |= (xx - p[0]) ** 2 + (yy - p[1]) ** 2 <= rp * rp
    ring = fill & ~erode_sq(fill, max(1, round(0.8 * U)))
    return fill & ~ring, ring


def town_area(job, foot):
    """the town: buildings plus the plazas they enclose, kept near the CPs"""
    N = job.N
    r = max(2, round(12.0 / (2 * job.E / N)))
    area = morph(morph(foot, r, True), r, False)
    area = ndi.binary_fill_holes(area)
    q = [tuple(job.to_px(c["pos"][0], c["pos"][2])) for c in job.cps]
    if len(q) >= 3:
        from math import atan2
        cx_, cy_ = np.mean([v[0] for v in q]), np.mean([v[1] for v in q])
        q.sort(key=lambda v: atan2(v[1] - cy_, v[0] - cx_))
        hull = Image.new("L", (N, N), 0)
        ImageDraw.Draw(hull).polygon(q, fill=255)
        area &= dilate_sq(np.asarray(hull) > 0, max(2, round(45.0 / (2 * job.E / N))))
    U = N / 512.0
    return clean(area, round(2 * U), round(2 * U), (20 * U) ** 2, (20 * U) ** 2, U)


def render_bf2(job):
    o, log = job.o, job.log
    OUT = o.size or 512
    SS = 3
    N = OUT * SS
    U = N / 512.0
    job.prepare(N)
    outdoor = job.ground == "on"
    big = [p for p in job.pieces if walk_area(p) >= 150]
    if outdoor:
        land0 = job.land_mask(U)
        foot = np.asarray(fill_tris(N, np.concatenate([p["px"] for p in big]))) > 127 if big else np.zeros((N, N), bool)
        cover = (foot & land0).sum() / max(1, land0.sum())
        if cover >= 0.3:
            # a town / temple / station covers the ground (cor1, naboo2, pol1): stock draws only the floor plan
            outdoor = False
            log("Buildings cover %d%% of the ground: floor plan only, no ground drawn." % round(cover * 100))
    if outdoor:
        # natural maps: small props and plants are left off (stock fel1, dag1, end1 show only the ground)
        job.pieces = big
    img = np.full((N, N), BG_OUT if outdoor else BG_IN, np.float32)
    playH = job.play_height()
    land = np.zeros((N, N), bool)
    if outdoor:
        land = land0
        H = box_blur(job.H, max(1, round(1.5 * U))) if job.H is not None else None
        g = np.full((N, N), 138.0, np.float32)
        if H is not None:
            # lower ground darker, a little lighter up the slopes (stock fel1 / hoth / dag1 / kas2)
            # stock keeps the play ground one flat grey; only pits / river beds well below it go dark (fel1, dag1)
            # "well below" is judged against the ground around it (~40 m), so a whole valley stays light
            r = max(2, round(60.0 / (2 * job.E / N)))
            local = ndi.uniform_filter(H, 2 * r + 1, mode="nearest")
            g = np.where(H < local - 5, 82, 138).astype(np.float32)
        img[land] = g[land]
        if job.water.any():
            img[job.water & ~land] = 68
        # cliffs too steep to walk: drawn like the outside (stock geo1 / hoth canyon walls)
        if H is not None:
            gy, gx = np.gradient(job.H, 2 * job.E / N)
            steep = np.hypot(gx, gy) > 1.4
            steep = morph(morph(steep, round(U), False), round(1.5 * U), True)
            img[land & steep] = np.minimum(img[land & steep], 92)
    # rock: dark ground, on outdoor maps only
    if job.rocks and outdoor:
        m = np.asarray(fill_tris(N, np.concatenate([r["px"] for r in job.rocks]))) > 127
        m = clean(m, round(2 * U), round(U), (6 * U) ** 2, (8 * U) ** 2, U)
        img[m & land] = np.minimum(img[m & land], 96)
    # buildings as floor plans: few flat tones (stock maps use 1-3 greys per map, not a height ramp)
    top, foot = floor_raster(job, N)
    has = ~np.isnan(top)
    cy = [c["pos"][1] for c in job.cps] or [playH]
    has &= top < max(cy) + 14          # roofs / decks far above every CP are left off (naboo2 roofs, kamino pod tops)
    has &= top > min(cy) - 18          # and ground far below every CP (cor1 under-city)
    if outdoor and job.w.boundary:
        has &= dilate_sq(land, round(4 * U))   # buildings beyond the play area are left off (tat2)
    crisp_town = not outdoor and not job.w.boundary and job.cps and job.pieces
    if crisp_town:
        # town with no walls (naboo2): stock BF2 naboo2 is the BF1 art in grey - the plaza 69 with a light edge,
        # every building a crisp 125 shape with a light rim, the rotunda's water band 97
        img[:] = 0.0
        layers = cr.buildings(job, job.pieces, N)
        area, apoly = cr.town(job, layers, N)
        cr.draw_polys(img, apoly, 69.0, 190.0, rim_w=U)
        layers = cr.keep_layers(job, layers, area, N)
        for l in sorted(layers, key=lambda l: (not cr.is_floor(job, l), cr.is_dark(l))):
            cr.draw_polys(img, l[1], 97.0 if cr.is_dark(l) else (69.0 if cr.is_floor(job, l) else 125.0), 190.0, rim_w=U)
        has[:] = False
    if has.any():
        rel = np.where(has, top - playH, 0.0)
        tone = np.select([rel < -9, rel < -3, rel <= 3, rel <= 9], [105, 128, 150, 178], 200).astype(np.float32)
        m = clean(has, round(U), round(0.75 * U), (3 * U) ** 2, (4 * U) ** 2, 0.6 * U)
        # one tone per patch: majority filter removes stair-steps and small clutter
        t = np.where(m, tone, 0)
        t = ndi.median_filter(t, size=max(3, int(round(1.5 * U)) | 1))
        t = np.where(m & (t == 0), 150, t)
        img[m] = t[m]
        # pillars and solid blocks standing inside the floor (no floor on top of them): light squares, like cor1
        solid = foot & ~has & dilate_sq(m, round(U))
        solid = morph(solid, round(0.5 * U), False)
        solid = drop_small(solid, (1.5 * U) ** 2, 1)
        img[solid] = 215
        # light line where the tone changes, and round the outside
        e = np.zeros_like(m)
        e[:, 1:] |= (t[:, 1:] != t[:, :-1]) & m[:, 1:] & m[:, :-1]
        e[1:, :] |= (t[1:, :] != t[:-1, :]) & m[1:, :] & m[:-1, :]
        e = dilate_sq(e, max(0, round(U / 3)))
        img[e] = np.maximum(img[e], 190)
        outer = m & ~erode_sq(m, max(1, round(0.7 * U)))
        img[outer] = 205
    if outdoor:
        rim = land & ~erode_sq(land, max(1, round(0.8 * U)))
        img[rim] = np.maximum(img[rim], 180)
    if o.cp_rings:
        rings = cp_ring_mask(job, U)
        img[rings[0]] = 30
        img[rings[1]] = 215
    small = img.reshape(OUT, SS, OUT, SS).mean((1, 3))
    g8 = np.clip(small + 0.5, 0, 255).astype(np.uint8)
    log("%d structures drawn as floor plans." % len(job.pieces))
    return Image.fromarray(np.stack([g8] * 3, -1))


def render(world, opt, log=print):
    job = Job(world, opt, log)
    t0 = time.time()
    img = (render_bf2 if job.style == "bf2" else render_bf1)(job)
    log("Done in %.1f s." % (time.time() - t0))
    return img, job


def save_outputs(img, job, folder):
    os.makedirs(folder, exist_ok=True)
    name = job.w.name + "_map"
    tga = os.path.join(folder, name + ".tga")
    write_tga(tga, img)
    with open(tga + ".option", "w") as f:
        f.write("-8bit -maps 1 ")
    img.save(os.path.join(folder, name + "_preview.png"))
    return tga
