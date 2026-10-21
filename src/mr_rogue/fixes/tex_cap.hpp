// BF1GameExt - tex_cap.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/fixes/; file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 22d: the texture cap (BF1GameExt.ini [Limits] MaxTextureSize, TextureSafetyNet). See tex_cap.cpp.
#pragma once

#include <cstddef>

namespace bf1 {

// At the game's DirectInput call: finds the texture loader's High-detail branch (the one that loads every mip level) and
// sends it through BF1GameExt, which picks how many top levels the game's own skip leaves out: textures over
// MaxTextureSize, and (TextureSafetyNet) one that would not fit in the address space any more.
void TexCapStart(bool onGameThread);

// For the [mem] lines: "textures at High detail: ..." (empty when the cap is not in).
const char* TexCapText(char* out, size_t n);

} // namespace bf1
