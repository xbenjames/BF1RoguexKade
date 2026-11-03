"""Battlefront minimap maker - file reading part.
Reads a world folder (BF1 or BF2): .wld/.lyr objects, .ldx game modes, .rgn mapbounds,
.pth/.bnd boundary, .ter heights, .odf classes and .msh meshes, and gives world-space geometry."""
import os, re, struct
import numpy as np

DEF_IGNORE = ("gas_cell, tur_, bush, shrub, hedge, foliage, leaf, leaves, flora, veg_, vegetation, frond, reed, vine, "
              "moss, ivy, flower, cactus, kelp, coral, plant, palm, torch, grave, fence, dust, coffin, vase, arrow, "
              "fire_cup, crane, door, cart, egg, godray, terraincutter, cutter, sarlacc, skeleton, turret, _col_, wings, "
              "snd_, sound, light, effect, item, recharge, spawn, flag, decal, grass, fern, weed, debris, sky_, "
              "cloud, controlzone, controlpanel, commandpost, holo, mist, fog, smoke, steam, spray, splash, star, water, ocean, sea_")
DEF_ROCK = "boulder, rock, spire, cliff, tree, log, stump, canopy, statue, stonegiant, stonehead, asteroid, mountain"
SKIP_LABELS = {"commandpost", "vehiclespawn", "powerupstation", "powerupitem", "soldier", "hover", "walker", "flyer",
               "door", "armedbuilding", "animatedprop", "beacon", "sound", "light", "effect", "claimable"}
OTHER_MODE = re.compile(r"ctf|1flag|2flag|flag_|hunt|rush|assault|elim|_eli|tdm|hero|_xl|space|campaign|tutorial|_h\.", re.I)
NOT_PLAY_LAYER = re.compile(r"sound|rain|rumble|shadow|reflect|death|light", re.I)


# what a layer's name says about the objects in it (the mappers sorted them themselves)
LAYER_OUT = re.compile(r"sound|light|shadow|reflect|death|rumble|collision|invis|design|effect|_fx|mist|cloud|sky|prop|droid|vehicle|turret|spawn", re.I)
LAYER_ROCK = re.compile(r"tree|rock|rubble|foliage|plant|jungle|forest|cliff|mountain|bush|grass|roid", re.I)
LAYER_BLDG = re.compile(r"build|bldg|platform|temple|pen|object|structure|base|wall|bridge|walk", re.I)


def layer_hint(layer_key):
    """'out' / 'rock' / 'struct' / None from a .lyr file name"""
    if not layer_key or layer_key == "__base":
        return None
    n = re.sub(r"\.lyr$", "", os.path.basename(layer_key), flags=re.I)
    n = n.split("_", 1)[1] if "_" in n else n          # drop the world prefix (myg1_Rubble -> Rubble)
    if LAYER_BLDG.search(n):
        return "struct"
    if LAYER_ROCK.search(n):
        return "rock"
    if LAYER_OUT.search(n):
        return "out"
    return None


def kwlist(s):
    return [t.strip().lower() for t in re.split(r"[,\n]", s) if t.strip()]


class FileIndex:
    """Every file under the folders the user picked, found by lower-case name."""
    def __init__(self):
        self.files = []            # (path, lower path, lower name)
        self.by_name = {}

    def add_folder(self, root):
        for dp, dn, fn in os.walk(root):
            for f in fn:
                p = os.path.join(dp, f)
                lo = p.replace("\\", "/").lower()
                name = f.lower()
                self.files.append((p, lo, name))
                self.by_name.setdefault(name, []).append(p)

    def find(self, name):
        l = self.by_name.get(name.lower())
        return l[0] if l else None

    def worlds(self):
        return [p for p, lo, n in self.files if n.endswith(".wld")]

    def in_dir(self, d):
        d = os.path.normcase(os.path.abspath(d))
        return [p for p, lo, n in self.files if os.path.normcase(os.path.dirname(os.path.abspath(p))) == d]


def read_text(p):
    with open(p, "rb") as f:
        return f.read().decode("latin-1")


OBJ_RE = re.compile(r'Object\("([^"]*)",\s*"([^"]*)",\s*-?\d+\)\s*\{(.*?)\n\}', re.S)


def parse_objects(s, layer):
    out = []
    for m in OBJ_RE.finditer(s):
        b = m.group(3)
        pos = re.search(r"ChildPosition\(([^)]*)\)", b)
        if not pos:
            continue
        rot = re.search(r"ChildRotation\(([^)]*)\)", b)
        tm = re.search(r"Team\((-?\d+)\)", b)
        cap = re.search(r'CaptureRegion\("([^"]*)"\)', b)
        out.append(dict(name=m.group(1), cls=m.group(2), pos=[float(v) for v in pos.group(1).split(",")],
                        rot=[float(v) for v in rot.group(1).split(",")] if rot else [1, 0, 0, 0],
                        team=int(tm.group(1)) if tm else 0, layer=layer, capture=cap.group(1) if cap else ""))
    return out


def parse_paths(s):
    out = {}
    for m in re.finditer(r'Path\("([^"]+)"\)\s*\{(.*?)\n\}', s, re.S):
        out[m.group(1)] = [[float(v) for v in x.split(",")] for x in re.findall(r"Position\(([^)]*)\)", m.group(2))]
    return out


def parse_regions(s):
    out = []
    for m in re.finditer(r'Region\("([^"]*)"[^)]*\)\s*\{(.*?)\n\s*\}', s, re.S):
        b = m.group(2)
        g = lambda k: re.search(k + r"\(([^)]*)\)", b)
        p, z, r = g("Position"), g("Size"), g("Rotation")
        if p and z:
            out.append(dict(name=m.group(1), pos=[float(v) for v in p.group(1).split(",")],
                            size=[float(v) for v in z.group(1).split(",")],
                            rot=[float(v) for v in r.group(1).split(",")] if r else [1, 0, 0, 0]))
    return out


def qmat(w, x, y, z):
    return np.array([[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
                     [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
                     [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]])


# ---------------------------------------------------------------- msh
def parse_msh(path):
    d = open(path, "rb").read()
    models = []

    def chunks(o, end):
        while o + 8 <= end:
            t = d[o:o + 4]
            n = struct.unpack_from("<I", d, o + 4)[0]
            yield t, o + 8, min(o + 8 + n, end)
            o += 8 + n

    def walk(o, end):
        for t, a, b in chunks(o, end):
            if t in (b"HEDR", b"MSH2"):
                walk(a, b)
            elif t == b"MODL":
                m = dict(name="", parent="", flags=0, tran=None, tris=[])
                for t2, a2, b2 in chunks(a, b):
                    if t2 == b"NAME":
                        m["name"] = d[a2:b2].split(b"\0")[0].decode("latin-1")
                    elif t2 == b"PRNT":
                        m["parent"] = d[a2:b2].split(b"\0")[0].decode("latin-1")
                    elif t2 == b"FLGS":
                        m["flags"] = struct.unpack_from("<I", d, a2)[0]
                    elif t2 == b"TRAN":
                        v = struct.unpack_from("<10f", d, a2)
                        m["tran"] = (np.array(v[0:3]), qmat(v[6], v[3], v[4], v[5]), np.array(v[7:10]))
                    elif t2 == b"GEOM":
                        for t3, a3, b3 in chunks(a2, b2):
                            if t3 != b"SEGM":
                                continue
                            pos = strip = ndxl = None
                            for t4, a4, b4 in chunks(a3, b3):
                                if t4 == b"POSL":
                                    n = struct.unpack_from("<I", d, a4)[0]
                                    pos = np.frombuffer(d, "<f4", n * 3, a4 + 4).reshape(n, 3)
                                elif t4 == b"STRP":
                                    n = struct.unpack_from("<I", d, a4)[0]
                                    strip = np.frombuffer(d, "<u2", n, a4 + 4).astype(np.int64)
                                elif t4 == b"NDXL":
                                    n = struct.unpack_from("<I", d, a4)[0]
                                    o4 = a4 + 4
                                    ndxl = []
                                    for _ in range(n):
                                        if o4 + 2 > b4:
                                            break
                                        c = struct.unpack_from("<H", d, o4)[0]
                                        ndxl.append(np.frombuffer(d, "<u2", c, o4 + 2))
                                        o4 += 2 + 2 * c
                            if pos is None:
                                continue
                            tri = []
                            if strip is not None:
                                s = strip
                                i = 0
                                L = len(s)
                                while i < L - 2:
                                    if s[i] & 0x8000 and s[i + 1] & 0x8000:
                                        j = i + 2
                                        tri.append((s[i] & 0x7fff, s[i + 1] & 0x7fff, s[j] & 0x7fff))
                                        j += 1
                                        while j < L and not (s[j] & 0x8000):
                                            tri.append((s[j - 2] & 0x7fff, s[j - 1] & 0x7fff, s[j]))
                                            j += 1
                                        i = j
                                    else:
                                        i += 1
                            elif ndxl is not None:
                                for p in ndxl:
                                    for q in range(1, len(p) - 1):
                                        tri.append((p[0], p[q], p[q + 1]))
                            if tri:
                                t_ = np.array(tri, dtype=np.int64)
                                t_ = t_[(t_ < len(pos)).all(1)]
                                if len(t_):
                                    m["tris"].append(pos[t_])
                models.append(m)
    walk(0, len(d))
    by = {m["name"]: m for m in models}
    out = []
    for m in models:
        nm = m["name"].lower()
        if not m["tris"] or (m["flags"] & 1) or re.search(r"shadow|collision|lowrez|lowres|_lod", nm) or re.match(r"(p_|c_|sv_)", nm):
            continue
        M = np.eye(3)
        t = np.zeros(3)
        cur, guard = m, 0
        while cur is not None and guard < 32:
            guard += 1
            if cur["tran"] is not None:
                sc, R, tr = cur["tran"]
                M = R @ np.diag(sc) @ M
                t = R @ (sc * t) + tr
            cur = by.get(cur["parent"]) if cur["parent"] and cur["parent"] != cur["name"] else None
        P = np.concatenate(m["tris"]).reshape(-1, 3) @ M.T + t
        out.append(P.reshape(-1, 3, 3))
    return np.concatenate(out) if out else np.zeros((0, 3, 3))


# ---------------------------------------------------------------- terrain
class Terrain:
    def __init__(self, path, pts=None):
        d = open(path, "rb").read()
        if d[:4] != b"TERR":
            raise ValueError("not a terrain file")
        ver = struct.unpack_from("<I", d, 4)[0]
        if ver not in (21, 22):
            raise ValueError("terrain version %d (BF1 is 21, BF2 is 22)" % ver)
        hdr = 2820 if ver == 21 else 2821
        hs, gs = struct.unpack_from("<2f", d, 164)
        n = struct.unpack_from("<I", d, 176)[0]
        if not (0 < n <= 4096 and gs > 0):
            raise ValueError("unexpected terrain header")

        def grid(o):
            return np.frombuffer(d, "<i2", n * n, o).reshape(n, n).astype(np.float32) * hs if o + 2 * n * n <= len(d) else None

        ho = hdr
        if pts is not None and len(pts) >= 20:
            P = np.asarray(pts, float)
            c = np.round(P[:, 0] / gs + n / 2).astype(int)
            r = np.round(P[:, 2] / gs + n / 2).astype(int)
            ok = (c >= 0) & (r >= 0) & (c < n) & (r < n)
            idx = r[ok] * n + c[ok]
            y = P[ok, 1]
            raw = np.frombuffer(d, np.uint8)

            def fit(o):
                if o + 2 * n * n > len(d) or o < 0:
                    return 0
                b0 = raw[o + 2 * idx].astype(np.int32)
                b1 = raw[o + 2 * idx + 1].astype(np.int32)
                v = ((b1 << 8) | b0).astype(np.int16).astype(np.float32) * hs
                return float((np.abs(v - y) < 2.5).sum()) / len(P)
            base = best = fit(hdr)
            for o in range(hdr - 128, hdr + 257):
                f = fit(o)
                if f > best:
                    best, ho = f, o
            if not (best > base + 0.15 and best > 0.4):
                ho = hdr
        self.ver, self.n, self.gs, self.hs, self.ho, self.hdr = ver, n, gs, hs, ho, hdr
        # water layer 1 (the sea / lake): height at byte 1276 in the header (layers are 68 bytes from 1208)
        wo = 1276 if ver == 21 else 1277
        self.water_h = None
        if len(d) > wo + 4:
            wh = struct.unpack_from("<f", d, wo)[0]
            if np.isfinite(wh) and 0.5 <= abs(wh) < 5000:
                self.water_h = float(wh)
        self.h = grid(ho)

    def sample(self, x, z):
        """bilinear height at world x,z (arrays ok)"""
        n, gs = self.n, self.gs
        c = np.asarray(x, float) / gs + n / 2
        r = np.asarray(z, float) / gs + n / 2
        c0 = np.clip(np.floor(c), 0, n - 2).astype(int)
        r0 = np.clip(np.floor(r), 0, n - 2).astype(int)
        fc = np.clip(c - c0, 0, 1)
        fr = np.clip(r - r0, 0, 1)
        h = self.h
        return ((h[r0, c0] * (1 - fc) + h[r0, c0 + 1] * fc) * (1 - fr) +
                (h[r0 + 1, c0] * (1 - fc) + h[r0 + 1, c0 + 1] * fc) * fr)


# ---------------------------------------------------------------- world
class World:
    def __init__(self, index, wld_path):
        self.ix = index
        self.path = wld_path
        self.dir = os.path.dirname(wld_path)
        self.name = os.path.splitext(os.path.basename(wld_path))[0]
        self.lname = self.name.lower()
        self.indir = index.in_dir(self.dir)
        self.odf_cache = {}
        self.msh_cache = {}
        wtxt = read_text(wld_path)
        self.objs = {"__base": parse_objects(wtxt, "__base")}
        self.lyrs = [p for p in self.indir if p.lower().endswith(".lyr")]
        for l in self.lyrs:
            self.objs[l] = parse_objects(read_text(l), l)
        m = re.search(r'TerrainName\("([^"]+)"\)', wtxt)
        self.ter_name = m.group(1) if m else None
        # .ldx layers and game modes (BF2)
        self.layer_num, self.modes = {}, {}
        ldx = [p for p in self.indir if p.lower().endswith(".ldx")]
        ldx = sorted(ldx, key=lambda p: os.path.basename(p).lower() != self.lname + ".ldx")
        if ldx:
            t = read_text(ldx[0])
            for mm in re.finditer(r'Layer\("([^"]+)",\s*(\d+)', t):
                self.layer_num[int(mm.group(2))] = mm.group(1)
            for mm in re.finditer(r'GameMode\("([^"]+)"\)\s*\{(.*?)\}', t, re.S):
                self.modes[mm.group(1)] = [int(x) for x in re.findall(r"Layer\((\d+)\)", mm.group(2))]
        self.ter_path = None
        if self.ter_name:
            cand = [p for p in self.indir if os.path.basename(p).lower() == self.ter_name.lower()]
            self.ter_path = cand[0] if cand else index.find(self.ter_name)
        self.ter_ver = 0
        if self.ter_path:
            try:
                self.ter_ver = struct.unpack_from("<I", open(self.ter_path, "rb").read(8), 4)[0]
            except Exception:
                pass
        # regions (mapbounds), paths, boundary
        self.regions = []
        for r in self.indir:
            if r.lower().endswith(".rgn"):
                for g in parse_regions(read_text(r)):
                    g["file"] = os.path.basename(r)
                    self.regions.append(g)
        self.paths = {}
        for p in self.indir:
            if p.lower().endswith(".pth"):
                self.paths.update(parse_paths(read_text(p)))
        self.boundary = []
        bnd = [p for p in self.indir if p.lower().endswith(".bnd")]
        if bnd:
            names = re.findall(r'Path\("([^"]+)"\)', read_text(bnd[0]))
            self.boundary = [self.paths[n] for n in names if n in self.paths and len(self.paths[n]) > 2]
        self.game = "bf2" if self.ter_ver == 22 else "bf1"

    # ---- layers
    def mode_names(self):
        return [m for m in self.modes if m.lower() != "common"]

    def default_mode(self):
        mn = self.mode_names()
        return next((m for m in mn if m.lower().startswith("con")), mn[0] if mn else "")

    def lyr_for(self, lname):
        if lname == "[Base]":
            return "__base"
        want = (self.lname + "_" + lname + ".lyr").lower()
        return next((l for l in self.lyrs if os.path.basename(l).lower() == want), None)

    def default_layers(self, mode=None):
        mode = self.default_mode() if mode is None else mode
        on = {"__base"}
        if mode and mode in self.modes:
            for k in ("common", mode.lower()):
                for mk, lst in self.modes.items():
                    if mk.lower() == k:
                        for n in lst:
                            i = self.lyr_for(self.layer_num.get(n, ""))
                            if i:
                                on.add(i)
            return {k for k in on if k == "__base" or not re.search("sound", os.path.basename(k), re.I)}
        for l in self.lyrs:
            b = os.path.basename(l)
            if not (OTHER_MODE.search(b) or NOT_PLAY_LAYER.search(b)):
                on.add(l)
        return on

    def objects(self, layers):
        return [o for k in self.objs if k in layers for o in self.objs[k]]

    # ---- odf / msh
    def odf(self, cls):
        k = cls.lower()
        if k in self.odf_cache:
            return self.odf_cache[k]
        self.odf_cache[k] = None
        p = self.ix.find(k + ".odf")
        r = None
        if p:
            props = {}
            for line in read_text(p).splitlines():
                mm = re.match(r'^\s*([A-Za-z0-9_]+)\s*=\s*"?([^"\r\n]*?)"?\s*(//.*)?$', line)
                if mm and mm.group(1).lower() not in props:
                    props[mm.group(1).lower()] = mm.group(2).strip()
            try:
                sc = float(props.get("geometryscale", "nan"))
            except ValueError:
                sc = float("nan")
            r = dict(label=props.get("classlabel", "").lower(), parent=props.get("classparent", ""),
                     geo=re.sub(r"\.msh$", "", props.get("geometryname", ""), flags=re.I), scale=sc)
            self.odf_cache[k] = r
            if r["parent"] and r["parent"].lower() != k:
                pr = self.odf(r["parent"])
                if pr:
                    r["label"] = r["label"] or pr["label"]
                    r["geo"] = r["geo"] or pr["geo"]
                    if np.isnan(r["scale"]):
                        r["scale"] = pr["scale"]
        self.odf_cache[k] = r
        return r

    def msh(self, name):
        k = name.lower()
        if k not in self.msh_cache:
            p = self.ix.find(k + ".msh")
            try:
                self.msh_cache[k] = parse_msh(p) if p else None
            except Exception:
                self.msh_cache[k] = None
        return self.msh_cache[k]

    def is_cp(self, o):
        r = self.odf(o["cls"])
        if r and r["label"]:
            return r["label"] == "commandpost"
        return bool(re.search("controlzone|controlpanel|commandpost", o["cls"], re.I))

    def class_mode(self, cls, ign=None, rock=None):
        ign = kwlist(DEF_IGNORE) if ign is None else ign
        rock = kwlist(DEF_ROCK) if rock is None else rock
        c = cls.lower()
        r = self.odf(cls)
        if r and r["label"] in SKIP_LABELS:
            return "out"
        if any(k in c for k in ign) or c.endswith("_col"):
            return "out"
        if any(k in c for k in rock):
            return "rock"
        return "struct"

    def mapbounds(self):
        mb = [g for g in self.regions if g["name"].lower() == "mapbounds"]
        if not mb:
            return None
        # prefer the main world's .rgn over layer ones
        mb.sort(key=lambda g: g["file"].lower() != self.lname + ".rgn")
        return mb[0]

    def terrain(self, objs=None):
        if not self.ter_path:
            return None
        return Terrain(self.ter_path, [o["pos"] for o in objs] if objs else None)

    def object_tris(self, o):
        """world-space triangles (k,3,3) of a placed object, or None if the mesh is missing"""
        r = self.odf(o["cls"])
        geo = (r and r["geo"]) or o["cls"]
        sc = r["scale"] if (r and not np.isnan(r["scale"])) else 1.0
        tri = self.msh(geo)
        if tri is None:
            return None, geo
        if not len(tri):
            return tri, geo
        R = qmat(*o["rot"])
        v = tri.reshape(-1, 3) * sc
        v = v * np.array([-1.0, 1.0, 1.0])
        w = v @ R.T + np.array(o["pos"])
        return w.reshape(-1, 3, 3), geo
