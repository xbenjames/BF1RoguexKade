"""Battlefront Minimap Maker - the window."""
import os, sys, threading, queue, traceback, json
import tkinter as tk
from tkinter import ttk, filedialog, messagebox
from PIL import Image, ImageTk, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mm_core as mc
import mm_render as mr

VERSION = "8"
TITLE = "Battlefront Minimap Maker v" + VERSION
CFG = os.path.join(os.path.expanduser("~"), ".bf_minimap_maker.json")


class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title(TITLE)
        self.geometry("1320x880")
        self.minsize(1100, 720)
        self.folders = []
        self.ix = None
        self.worlds = []
        self.world = None
        self.opt = None
        self.layer_vars = {}
        self.cp_vars = {}
        self.class_rows = {}
        self.result = None
        self.q = queue.Queue()
        self.cfg = self.load_cfg()
        self.build()
        self.after(100, self.poll)

    # ------------------------------------------------------------ settings file
    def load_cfg(self):
        try:
            return json.load(open(CFG))
        except Exception:
            return {}

    def save_cfg(self):
        try:
            self.cfg["folders"] = self.folders
            self.cfg["out"] = self.out_var.get()
            json.dump(self.cfg, open(CFG, "w"))
        except Exception:
            pass

    # ------------------------------------------------------------ layout
    def build(self):
        st = ttk.Style(self)
        try:
            st.theme_use("vista")
        except Exception:
            pass
        left = ttk.Frame(self, padding=8)
        left.pack(side="left", fill="y")
        right = ttk.Frame(self, padding=8)
        right.pack(side="left", fill="both", expand=True)

        # 1 folders
        f1 = ttk.LabelFrame(left, text="1 · Folders (world folder + any folders holding its msh / odf / mission lua)", padding=6)
        f1.pack(fill="x")
        self.folder_list = tk.Listbox(f1, height=4, width=70)
        self.folder_list.pack(fill="x")
        b = ttk.Frame(f1); b.pack(fill="x", pady=(4, 0))
        ttk.Button(b, text="Add folder…", command=self.add_folder).pack(side="left")
        ttk.Button(b, text="Remove", command=self.remove_folder).pack(side="left", padx=4)
        ttk.Button(b, text="Reload", command=self.reload).pack(side="left")

        # 2 world
        f2 = ttk.LabelFrame(left, text="2 · World and look", padding=6)
        f2.pack(fill="x", pady=6)
        g = ttk.Frame(f2); g.pack(fill="x")
        ttk.Label(g, text="World").grid(row=0, column=0, sticky="w")
        self.world_cb = ttk.Combobox(g, state="readonly", width=48)
        self.world_cb.grid(row=0, column=1, columnspan=3, sticky="we", pady=2)
        self.world_cb.bind("<<ComboboxSelected>>", lambda e: self.load_world(self.world_cb.current()))
        ttk.Label(g, text="Game mode").grid(row=1, column=0, sticky="w")
        self.mode_cb = ttk.Combobox(g, state="readonly", width=16)
        self.mode_cb.grid(row=1, column=1, sticky="w", pady=2)
        self.mode_cb.bind("<<ComboboxSelected>>", lambda e: self.refresh_layers(reset=True))
        ttk.Label(g, text="Made for").grid(row=1, column=2, sticky="e")
        self.game_cb = ttk.Combobox(g, state="readonly", width=10, values=["Auto", "BF1", "BF2"])
        self.game_cb.current(0); self.game_cb.grid(row=1, column=3, sticky="w", padx=4)
        self.game_cb.bind("<<ComboboxSelected>>", lambda e: self.update_box())
        ttk.Label(g, text="Style").grid(row=2, column=0, sticky="w")
        self.style_cb = ttk.Combobox(g, state="readonly", width=16, values=["Auto (same as game)", "BF1 (blue)", "BF2 (grey)"])
        self.style_cb.current(0); self.style_cb.grid(row=2, column=1, sticky="w", pady=2)
        ttk.Label(g, text="Size").grid(row=2, column=2, sticky="e")
        self.size_cb = ttk.Combobox(g, state="readonly", width=10, values=["Auto", "1024", "512", "256"])
        self.size_cb.current(0); self.size_cb.grid(row=2, column=3, sticky="w", padx=4)
        ttk.Label(g, text="Ground").grid(row=3, column=0, sticky="w")
        self.ground_cb = ttk.Combobox(g, state="readonly", width=16, values=["Auto", "Draw", "Hide"])
        self.ground_cb.current(0); self.ground_cb.grid(row=3, column=1, sticky="w", pady=2)
        ttk.Label(g, text="BF1 height lines").grid(row=3, column=2, sticky="e")
        self.cont_cb = ttk.Combobox(g, state="readonly", width=10, values=["10 m", "5 m", "20 m", "Off"])
        self.cont_cb.current(0); self.cont_cb.grid(row=3, column=3, sticky="w", padx=4)
        ttk.Label(g, text="BF2 turn 180°").grid(row=4, column=0, sticky="w")
        self.rot_cb = ttk.Combobox(g, state="readonly", width=16, values=["Auto (from lua)", "Yes", "No"])
        self.rot_cb.current(0); self.rot_cb.grid(row=4, column=1, sticky="w", pady=2)
        ttk.Label(g, text="Smallest m²").grid(row=4, column=2, sticky="e")
        self.minarea = tk.StringVar(value="25")
        ttk.Entry(g, textvariable=self.minarea, width=8).grid(row=4, column=3, sticky="w", padx=4)

        # box
        f3 = ttk.LabelFrame(left, text="3 · Map box (BF1: CPs extents + 100 m · BF2: the mapbounds region)", padding=6)
        f3.pack(fill="x")
        self.boxE, self.boxX, self.boxZ = tk.StringVar(), tk.StringVar(), tk.StringVar()
        bb = ttk.Frame(f3); bb.pack(fill="x")
        for i, (t, v) in enumerate((("Half size E", self.boxE), ("Centre X", self.boxX), ("Centre Z", self.boxZ))):
            ttk.Label(bb, text=t).grid(row=0, column=2 * i, sticky="w")
            ttk.Entry(bb, textvariable=v, width=10).grid(row=0, column=2 * i + 1, padx=(2, 10))
        self.box_over = tk.BooleanVar(value=False)
        ttk.Checkbutton(f3, text="Use these numbers instead of the rule", variable=self.box_over).pack(anchor="w")

        # tabs: layers / cps / objects
        nb = ttk.Notebook(left)
        nb.pack(fill="both", expand=True, pady=6)
        self.tab_layers = self.scroll_tab(nb, "Layers")
        self.tab_cps = self.scroll_tab(nb, "Command posts")
        self.tab_cls = self.scroll_tab(nb, "Objects")

        # right: preview + actions + log
        top = ttk.Frame(right); top.pack(fill="x")
        self.go_btn = ttk.Button(top, text="Generate map", command=self.generate)
        self.go_btn.pack(side="left")
        ttk.Label(top, text="  Save to").pack(side="left")
        self.out_var = tk.StringVar(value=self.cfg.get("out", ""))
        ttk.Entry(top, textvariable=self.out_var, width=46).pack(side="left", padx=4)
        ttk.Button(top, text="…", width=3, command=self.pick_out).pack(side="left")
        self.save_btn = ttk.Button(top, text="Save .tga + .option", command=self.save, state="disabled")
        self.save_btn.pack(side="left", padx=6)
        self.show_cp = tk.BooleanVar(value=True)
        ttk.Checkbutton(top, text="CPs on preview", variable=self.show_cp, command=self.show_preview).pack(side="left")
        self.canvas = tk.Canvas(right, width=640, height=640, bg="#111", highlightthickness=0)
        self.canvas.pack(pady=6)
        self.status = ttk.Label(right, text="Add a folder to start.")
        self.status.pack(anchor="w")
        self.logbox = tk.Text(right, height=10, wrap="word", font=("Consolas", 9))
        self.logbox.pack(fill="both", expand=True)
        for f in self.cfg.get("folders", []):
            if os.path.isdir(f):
                self.folders.append(f); self.folder_list.insert("end", f)
        if self.folders:
            self.after(200, self.reload)

    def scroll_tab(self, nb, title):
        outer = ttk.Frame(nb)
        nb.add(outer, text=title)
        cv = tk.Canvas(outer, highlightthickness=0, height=220)
        sb = ttk.Scrollbar(outer, orient="vertical", command=cv.yview)
        inner = ttk.Frame(cv)
        inner.bind("<Configure>", lambda e: cv.configure(scrollregion=cv.bbox("all")))
        cv.create_window((0, 0), window=inner, anchor="nw")
        cv.configure(yscrollcommand=sb.set)
        cv.pack(side="left", fill="both", expand=True); sb.pack(side="right", fill="y")
        cv.bind("<Enter>", lambda e: cv.bind_all("<MouseWheel>", lambda ev: cv.yview_scroll(int(-ev.delta / 120), "units")))
        cv.bind("<Leave>", lambda e: cv.unbind_all("<MouseWheel>"))
        return inner

    # ------------------------------------------------------------ folders / worlds
    def log(self, msg):
        self.q.put(("log", msg))

    def poll(self):
        try:
            while True:
                k, v = self.q.get_nowait()
                if k == "log":
                    self.logbox.insert("end", v + "\n"); self.logbox.see("end")
                elif k == "done":
                    self.on_done(*v)
                elif k == "error":
                    self.logbox.insert("end", "ERROR: " + v + "\n"); self.logbox.see("end")
                    self.go_btn.config(state="normal"); self.status.config(text="Not generated.")
        except queue.Empty:
            pass
        self.after(100, self.poll)

    def add_folder(self):
        d = filedialog.askdirectory(title="Pick a world folder (or the folder above it)")
        if d:
            self.folders.append(d); self.folder_list.insert("end", d); self.reload()

    def remove_folder(self):
        s = self.folder_list.curselection()
        if s:
            self.folders.pop(s[0]); self.folder_list.delete(s[0]); self.reload()

    def pick_out(self):
        d = filedialog.askdirectory(title="Where to save the map files")
        if d:
            self.out_var.set(d)

    def reload(self):
        self.save_cfg()
        self.ix = mc.FileIndex()
        for f in self.folders:
            self.ix.add_folder(f)
        self.worlds = sorted(self.ix.worlds())
        nm = sum(1 for _, _, n in self.ix.files if n.endswith(".msh"))
        no = sum(1 for _, _, n in self.ix.files if n.endswith(".odf"))
        self.status.config(text="%d files · %d worlds · %d models · %d ODFs" % (len(self.ix.files), len(self.worlds), nm, no))
        self.world_cb["values"] = [os.path.relpath(w, os.path.commonpath(self.folders)) if self.folders else w for w in self.worlds]
        if self.worlds:
            self.world_cb.current(0); self.load_world(0)

    def load_world(self, i):
        try:
            self.world = mc.World(self.ix, self.worlds[i])
        except Exception as e:
            messagebox.showerror(TITLE, "Couldn't read the world:\n%s" % e); return
        modes = self.world.mode_names()
        self.mode_cb["values"] = modes or ["(none)"]
        dm = self.world.default_mode()
        self.mode_cb.set(dm if dm else "(none)")
        self.opt = mr.Options()
        self.refresh_layers(reset=True)

    def refresh_layers(self, reset=False):
        w = self.world
        if not w:
            return
        for c in self.tab_layers.winfo_children():
            c.destroy()
        mode = self.mode_cb.get()
        mode = None if mode in ("", "(none)") else mode
        on = w.default_layers(mode)
        self.layer_vars = {}
        for key in ["__base"] + w.lyrs:
            v = tk.BooleanVar(value=key in on)
            lab = os.path.basename(w.path) + " (base world)" if key == "__base" else os.path.basename(key)
            ttk.Checkbutton(self.tab_layers, text=lab, variable=v, command=self.refresh_cps).pack(anchor="w")
            self.layer_vars[key] = v
        if reset:
            self.opt.cp_off = None
        self.refresh_cps()

    def cur_layers(self):
        return {k for k, v in self.layer_vars.items() if v.get()}

    def refresh_cps(self):
        w = self.world
        for c in self.tab_cps.winfo_children():
            c.destroy()
        objs = w.objects(self.cur_layers())
        cps = [o for o in objs if w.is_cp(o)]
        if self.opt.cp_off is None:
            self.opt.cp_off = {mr.cp_key(c) for c in cps if mc.OTHER_MODE.search(c["name"]) or mc.OTHER_MODE.search(c["cls"])}
        self.cp_vars = {}
        if not cps:
            ttk.Label(self.tab_cps, text="No command posts in the ticked layers.").pack(anchor="w")
        for c in cps:
            k = mr.cp_key(c)
            v = tk.BooleanVar(value=k not in self.opt.cp_off)
            ttk.Checkbutton(self.tab_cps, text="%-18s %-30s x %6.0f  z %6.0f" % (c["name"][:18], c["cls"][:30], c["pos"][0], c["pos"][2]),
                            variable=v, command=self.cp_changed).pack(anchor="w")
            self.cp_vars[k] = v
        self.refresh_classes(objs)
        self.update_box()

    def cp_changed(self):
        self.opt.cp_off = {k for k, v in self.cp_vars.items() if not v.get()}
        self.update_box()

    def refresh_classes(self, objs):
        for c in self.tab_cls.winfo_children():
            c.destroy()
        cnt = {}
        for o in objs:
            cnt[o["cls"]] = cnt.get(o["cls"], 0) + 1
        names = {"struct": "Building", "rock": "Dark ground", "out": "Leave out"}
        back = {v: k for k, v in names.items()}
        self.class_rows = {}
        for i, cls in enumerate(sorted(cnt, key=str.lower)):
            cur = self.opt.class_modes.get(cls.lower()) or self.world.class_mode(cls)
            ttk.Label(self.tab_cls, text=cls, width=40).grid(row=i, column=0, sticky="w")
            ttk.Label(self.tab_cls, text="×%d" % cnt[cls], width=6).grid(row=i, column=1)
            cb = ttk.Combobox(self.tab_cls, state="readonly", width=12, values=list(names.values()))
            cb.set(names[cur]); cb.grid(row=i, column=2, pady=1)
            cb.bind("<<ComboboxSelected>>", lambda e, c=cls, b=cb: self.opt.class_modes.__setitem__(c.lower(), back[b.get()]))

    def gather(self):
        o = self.opt
        o.game = {"Auto": "auto", "BF1": "bf1", "BF2": "bf2"}[self.game_cb.get()]
        o.style = ["auto", "bf1", "bf2"][self.style_cb.current()]
        o.mode = None if self.mode_cb.get() in ("", "(none)") else self.mode_cb.get()
        o.layers = self.cur_layers()
        o.ground = {"Auto": "auto", "Draw": "on", "Hide": "off"}[self.ground_cb.get()]
        o.contours = {"10 m": 10, "5 m": 5, "20 m": 20, "Off": 0}[self.cont_cb.get()]
        o.size = None if self.size_cb.get() == "Auto" else int(self.size_cb.get())
        o.rotate180 = ["auto", "yes", "no"][self.rot_cb.current()]
        try:
            o.min_area = float(self.minarea.get())
        except ValueError:
            o.min_area = 25.0
        o.box = None
        if self.box_over.get():
            try:
                o.box = (float(self.boxE.get()), float(self.boxX.get()), float(self.boxZ.get()))
            except ValueError:
                pass
        return o

    def update_box(self):
        if not self.world or self.box_over.get():
            return
        try:
            job = mr.Job(self.world, self.gather(), log=lambda *a: None)
            b = job.box()
            self.boxE.set("%.2f" % b["E"]); self.boxX.set("%.2f" % b["CX"]); self.boxZ.set("%.2f" % b["CZ"])
        except Exception:
            pass

    # ------------------------------------------------------------ run
    def generate(self):
        if not self.world:
            return
        self.logbox.delete("1.0", "end")
        self.go_btn.config(state="disabled"); self.save_btn.config(state="disabled")
        self.status.config(text="Working…")
        opt = self.gather()
        w = mc.World(self.ix, self.world.path)       # fresh caches each run

        def work():
            try:
                img, job = mr.render(w, opt, log=self.log)
                self.q.put(("done", (img, job)))
            except Exception as e:
                self.q.put(("error", "%s\n%s" % (e, traceback.format_exc(limit=3))))
        threading.Thread(target=work, daemon=True).start()

    def on_done(self, img, job):
        self.result = (img, job)
        self.go_btn.config(state="normal"); self.save_btn.config(state="normal")
        self.status.config(text="%s_map.tga · %d×%d · %s style" % (job.w.name, img.size[0], img.size[1], job.style.upper()))
        self.show_preview()

    def show_preview(self):
        if not self.result:
            return
        img, job = self.result
        S = 640
        pv = img.convert("RGB").resize((S, S), Image.LANCZOS)
        if self.show_cp.get():
            d = ImageDraw.Draw(pv)
            col = {1: (60, 224, 106), 2: (255, 74, 74)}
            for c in job.cps:
                p = job.to_px(c["pos"][0], c["pos"][2]) / job.N * S
                x, y = float(p[0]), float(p[1])
                d.ellipse([x - 8, y - 8, x + 8, y + 8], outline=(0, 0, 0), width=4)
                d.ellipse([x - 8, y - 8, x + 8, y + 8], outline=col.get(c["team"], (230, 230, 230)), width=2)
                d.text((x + 11, y - 6), c["name"], fill=(255, 255, 255), stroke_width=2, stroke_fill=(0, 0, 0))
        self._tk = ImageTk.PhotoImage(pv)
        self.canvas.delete("all"); self.canvas.create_image(0, 0, image=self._tk, anchor="nw")

    def save(self):
        if not self.result:
            return
        out = self.out_var.get().strip() or filedialog.askdirectory(title="Where to save the map files")
        if not out:
            return
        self.out_var.set(out); self.save_cfg()
        img, job = self.result
        p = mr.save_outputs(img, job, out)
        self.log("Saved %s (+ .option and preview). Copy both map files next to the .wld and munge." % p)


