// BF1GameExt - pads.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/splitscreen/ (was pad2.hpp); file names in the comments and log lines follow the new names. Nothing else changed.
// 20e: PadBuild / PadPoint / PadUpdate ... - more pads of the game's class (players 3 and 4).
// Pad 2 for player 2 (2020 build: GOG 1.3 / Steam).
//
// The 2020 exe has one XInput pad object. This builds a second one in DLL
// memory with the game's own pad code (constructor, SetIndex, probe, update)
// and points it at a second connected XInput pad.
//
// It is driven one of two ways:
//   - from the game's frame (phase 6b): player2_input.cpp calls Pad2Tick()
//     inside the game's input update, on the game thread
//   - from its own thread (fallback, the 6a behaviour) if the frame hook could
//     not be installed
// Either way it logs what pad 2 sees as [pad2] lines.
//
// Safe beside the game: the pad code writes nothing outside the pad object
// except its own XInput calls (checked in the GOG 1.3 code).
#pragma once

#include <cstdint>

namespace bf1 {

// Finds the pad code by pattern and builds pad 2 (not pointed at a pad yet).
// Returns false (and logs why) if anything does not match. Safe to call twice.
bool Pad2Init();

// One update: picks / re-picks pad 2's XInput index (at most once a second),
// runs the game's pad update on it, and logs changes. Call from one thread only.
void Pad2Tick(float dt);

// Fallback: drive Pad2Tick from a thread every 16 ms.
void Pad2StartThread();

// Address of pad 2 (for player 2's input object), and whether it is connected.
uintptr_t Pad2Address();
bool Pad2IsConnected();

// Phase 14a: address of the game's own pad (player 1's), or 0 if Pad2Init did
// not find it. Same layout as pad 2: +0x09 connected, +0xC4 XInput buttons,
// +0xC8 / +0xCA raw left stick X / Y.
uintptr_t GamePadAddress();

// 20e: pads 3 and 4 (players34.cpp). The game's pad class in the caller's memory (kPadBytes, 16-aligned),
// with the code Pad2Init found: PadBuild = its constructor, PadPoint = SetIndex(XInput index) + Probe, PadUpdate =
// the game's per-frame pad update. All false / no-ops if Pad2Init did not succeed. Call from the game thread.
constexpr uint32_t kPadBytes = 0xD8;
bool PadBuild(void* mem);
void PadPoint(void* pad, int index);
void PadUpdate(void* pad, float dt);
bool PadConnected(const void* pad);
uint16_t PadButtons(const void* pad); // XInput buttons (+0xC4)
// The XInput index of the game's pad / pad 2 (-1: none yet), and whether an XInput pad answers at an index.
int GamePadXInputIndex();
int Pad2XInputIndex();
bool XInputPadPresent(int index);

} // namespace bf1
