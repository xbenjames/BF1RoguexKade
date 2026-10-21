BF1GameExt v6.4 (build p24a-r1) - Mr Rogue's v6.3 (p23a-r1) + Ben James's changes, Oct 21 2026.
Everything of Ben's is in src\ben\ (addons.cpp / .hpp, bfx_addon.h, the Lua in mods_page.lua / mods_build.lua is a copy of
what went into menus.cpp's kModsLua) plus small edits marked "v6.4" in dllmain.cpp, menus.cpp, sprint.cpp, stamina_hud.cpp,
the .vcxproj / .filters and config\BF1GameExt.ini.

WHAT CHANGED
- The add-on loader: DllMain loads every GameData\dinput8_*.dll (name order). At the game's DirectInput8Create call, before
  BF1GameExt's own hooks, each add-on's BFX_Init runs (the code is unlocked then); the IDirectInput8 object is handed to
  BFX_DirectInput. [Addons] LoadAddons=1 in BF1GameExt.ini. [addon] lines in the log.
- The MODS page (OPTIONS > MODS) is generic: one row per mod from the registry (every add-on's BFX_ModOptions table, plus
  BF1GameExt's own sprint and ragdolls). A on a row opens the mod's page (ifs_bfx_modpage): ON / OFF rows and choices,
  left / right / A turn them; a row marked * needs a game restart. Each mod saves to its own ini. "BFXQ: mods rows",
  "mods page K", "mods turn K I D".
- BF1GameExt's own files live in GameData\BF1GameExt\ (made at start if missing): console.lvl, BF1GameExt_split.lua,
  BF1GameExt_ctrlpage.lua, BF1GameExt_pause34.lua, the logs (BF1GameExt_split.log / .prev.log), BF1GameExt_IAOptions.ini.
  BF1GameExt.ini stays next to Battlefront.exe.
- [Sprint] TestBoost and StaminaBar default to 0 (Ben's dinput8_bf1sprint.dll does sprint; if it is loaded first, BF1GameExt's
  sprint hook fails its byte-check anyway and logs a MISMATCH).

BUILD: Visual Studio 2022 (v143), Release | Win32 - or push to GitHub: .github\workflows\build.yml builds it on a Windows
runner (msbuild) and uploads dinput8.dll as an artifact. third_party\bullet3 is needed (BF1GameExt_v6.3_part2_bullet.zip).
