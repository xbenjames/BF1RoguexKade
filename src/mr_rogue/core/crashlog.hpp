// BF1GameExt - crashlog.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/core/; file names in the comments and log lines follow the new names. Nothing else changed.
// Logs access violations and other fatal-type exceptions to the log, with the
// address, registers, code bytes and top of stack. It never handles them: the
// game (or Windows) still decides what happens next.
#pragma once

namespace bf1 {

void CrashLogInstall();

} // namespace bf1
