BF1GameExt v7.1 (build p25g-r1) - v7.0 (p25f-r1) + the side carousel, Oct 27 2026.

- CHOOSE YOUR SIDE with more than two sides (Mashup Conquest) is a carousel, like the class wheel:
  - the picked side stands in front with its name under it;
  - the other sides stand round it, smaller, darker and higher the further back;
  - left / right turn it round (defeated sides are skipped), and A joins.
  - Mouse (1 player): click a side to turn it to the front, then click the front side (or its name) to join.
  - With exactly two sides, the stock side-by-side screen is unchanged.
- Split screen (2 players, vertical and horizontal): both players get every Mashup side (v7.0 had teams 1 / 2 only there).
  Player 2's flow follows teams 1 - 7: his spawn point screen, his map's posts, and the game moving him to another side.
- 4-player split screen still offers the two stock sides.
- Code: class_wheel.cpp (SideUpdate's carousel; ClassWheelSideTeams / SideOut / SideRingFrame / SideRingClear, rings 4 - 7
  = players 1 - 4's side carousels) and split_spawn_menus.cpp (SideCarouselFrame, P2Sides; the team 1 / 2 checks widened).
- Log: [wheel] "click on side N - the carousel turns to it", [p2spawn] "side carousel (N sides) in ...".
- The build string: p25g-r1.
