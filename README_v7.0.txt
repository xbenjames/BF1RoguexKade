BF1GameExt v7.0 (build p25f-r1) - v6.9 (p25e-r1) + Ben James's Mashup release, Oct 26 2026.

WHAT CHANGED
- CHOOSE YOUR SIDE (class_wheel.cpp, marked "v7.0") has one layout per side count:
    2 sides  the stock pair
    3 sides  one row of 3
    4 sides  one row of 4
    5 sides  3 on top, 2 below
    6 sides  3 on top, 3 below
    7 sides  4 on top, 3 below
  (v6.7 - v6.9 put 4 on top for 5 and 6 sides too: 4 + 1 and 4 + 2.) The side count comes from BF1Modes.dll's
  BF1Modes_SideTeams (Mashup Conquest: every team with units); any other mission keeps the stock 2-side screen.
  SideTopCount(n) picks the top row; the rows use the top row's spacing, 3-per-row soldiers are a little bigger.
- The build string: p25f-r1.
