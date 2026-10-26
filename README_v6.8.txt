BF1GameExt v6.8 (build p25d-r1) - v6.7 (p25c-r1) + Ben James's fixes, Oct 26 2026.

WHAT CHANGED
- The console spawn flow (class wheel) now runs for teams 1 - 7 (class_wheel.cpp ClassWheelDriverFrame, marked "v6.8").
  It stopped for anything but teams 1 / 2, so joining a Mashup side 3 - 7 (e.g. Geonosis' OUTLAWS) left the side screen
  on the screen over the spawn point screen with no input. The once-per-match class scan (split_spawn_menus.cpp) also
  runs for teams 3 - 7 now.
- Two-row CHOOSE YOUR SIDE (more than 4 sides): the names right under the soldiers' feet (top row 47.5%, bottom 83.5%
  of the screen height; v6.7 had them about 8% lower).
- The build string: p25d-r1.
