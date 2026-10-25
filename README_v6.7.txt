BF1GameExt v6.7 (build p25c-r1) - v6.6 (p25b-r1) + Ben James's two-row side screen, Oct 25 2026.

WHAT CHANGED
- CHOOSE YOUR SIDE (src\mr_rogue\spawn\class_wheel.cpp, marked "v6.7") with more than 4 sides (Mashup Conquest with
  dinput8_BF1Modes.dll, e.g. Geonosis with 7 sides): two rows - 4 sides on the top row, the rest centred below, the
  soldiers smaller (kTwoRowSize 0.58 of the 4-side size), each name under its soldier. Left / right still step
  through the sides in order (the top row, then the bottom row); the mouse picks the side under it.
  Places: top row feet at 49% of the screen height (names 52.5%), bottom row feet at 85% (names 88.5%).
- 2 - 4 sides: one row, as v6.6.
- The build string: p25c-r1.
