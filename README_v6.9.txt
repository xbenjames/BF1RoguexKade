BF1GameExt v6.9 (build p25e-r1) - v6.8 (p25d-r1) + Ben James's fixes, Oct 26 2026.

WHAT CHANGED
- The log (core/log.cpp) is written by its own thread. Ben's log showed freezes of 1.5 - 4.8 s at the start of a match,
  all spent in Log's WriteFile (4840 ms over 26 lines in one frame, the game's thread on the CPU 7% of the time - the
  write waited on the disk). Log now copies the line into a 4 MB memory buffer; a writer thread writes it every 100 ms
  (sooner once 64 KB wait). A full buffer drops lines and says how many. The crash handler calls LogFlush after each
  report, and the process-exit line writes what waits first, so crash logs are still complete.
- CHOOSE YOUR SIDE (class_wheel.cpp, marked "v6.9"): a side that is out of the match (BF1Modes.dll's new export
  BF1Modes_SideOut: Mashup Conquest, reinforcements gone) is drawn darker than an unpicked side (soldier and name), is
  skipped by left / right and the cursor, and A or a click on it does nothing. If his own side is out, the screen opens
  on the first side still in. Without BF1Modes (or an older one) no side is greyed out.
- The build string: p25e-r1.
