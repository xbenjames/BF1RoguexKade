"""Battlefront Minimap Maker - desktop program (BF1 and BF2 style minimaps from world files).
Run MinimapMaker.exe for the window, or with --folders/--out for batch use."""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mm_core as mc
import mm_render as mr


def cli(argv):
    """MinimapMaker.exe --world <name> --folders a;b --style bf1|bf2 --out <dir>"""
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument("--world"); ap.add_argument("--folders", required=True)
    ap.add_argument("--style", default="auto"); ap.add_argument("--game", default="auto")
    ap.add_argument("--out", required=True); ap.add_argument("--size", type=int)
    a = ap.parse_args(argv)
    ix = mc.FileIndex()
    for d in a.folders.split(";"):
        ix.add_folder(d)
    ws = [p for p in ix.worlds() if not a.world or os.path.basename(p).lower() == a.world.lower() + ".wld"]
    for wp in ws:
        w = mc.World(ix, wp)
        img, job = mr.render(w, mr.Options(style=a.style, game=a.game, size=a.size))
        print(mr.save_outputs(img, job, a.out))


if __name__ == "__main__":
    if len(sys.argv) > 1:
        cli(sys.argv[1:])
    else:
        import mm_gui
        mm_gui.App().mainloop()
