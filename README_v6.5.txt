BF1GameExt v6.5 (build p25a-r1) - v6.4 (p24a-r1) + Ben James's side screen change, Oct 25 2026.

WHAT CHANGED
- The class wheel's CHOOSE YOUR SIDE screen (src\mr_rogue\spawn\class_wheel.cpp, SideUpdate, marked "v6.5") can offer up
  to 7 sides. It asks dinput8_BF1Modes.dll (the game modes add-on) through its export BF1Modes_SideTeams(int* teams, int max):
  in a Mashup Conquest match that returns every playable team (Republic, CIS, Alliance, Empire and the map's locals, up to 7).
  The sides then stand in a row across the screen (smaller as there are more), each name under its soldier, left / right
  step through them, the mouse picks the one under it, A / Enter / a click joins it (the spawn screen's own SelectTeam).
- Any other match (or without dinput8_BF1Modes.dll): two sides, exactly as v6.4. Split screen keeps its two-side layouts.
- The build string: p25a-r1.
