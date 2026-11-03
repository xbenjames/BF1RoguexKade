# Battlefront Minimap Maker

Makes stock-looking minimap textures (`<world>_map.tga` + `.option`) for Star Wars Battlefront (2004) and Battlefront II (2005) worlds from the world files (.wld/.lyr/.ldx/.rgn/.pth/.bnd/.ter + .odf/.msh).

- Window: run `MinimapMaker.exe`, add the world folder (+ any folder holding its msh/odf and mission lua), pick the world, Generate, Save.
- Batch: `MinimapMaker.exe --folders "D:\a;D:\b" --world tan1 --style bf2 --out D:\maps`
- Style: BF1 (blue, scanlines, ring) or BF2 (grey floor plan the HUD tints). Made for: BF1 box = CP extents + 100 m; BF2 box = `mapbounds` region (square, half size = max(Size.x, Size.z)); BF2 `SetMapNorthAngle(180)` without the 2nd argument turns the texture 180°.
- Built by GitHub Actions on push to `minimap-maker`; the exe goes to the `minimap-builds` branch.
