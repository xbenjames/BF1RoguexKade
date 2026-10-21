// BF1GameExt - player2_input.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/splitscreen/; file names in the comments and log lines follow the new names. Nothing else changed.
// 20e: PlayerInputBuild / SetConnected / Update, ControlStateFill (players 3 and 4, players34.cpp).
// 18v: Player2RedirectActive().
// Phase 6b: player 2's input object (2020 build: GOG 1.3 / Steam).
//
// The 2020 input manager builds its per-player arrays with ONE slot each:
// player-input (0x718 bytes), buffer A (0x14), a 0x34 object, buffer B (0x140).
// This builds player 2's player-input object and buffers A and B in DLL
// memory with the game's own constructors, wires them the way the game wires
// player 1 (connected flag, flag +0x4B4, buffer A at +0x4B8, buffer B at
// +0x4B0), gives it pad 2 and no keyboard or mouse, and updates it every frame.
//
// Frame hook: the manager's per-frame "call PlayerInput::Update" for player 1
// is redirected to BF1GameExt, which calls the original for player 1 first,
// then updates pad 2 and player 2's input object on the game thread.
//
// Phase 6c: the input manager's virtual getters GetPlayerInput(i), GetBufA(i)
// and GetBufB(i) (vtable +0x18/+0x1C/+0x20) are redirected so that index 1
// returns player 2's objects instead of the manager's own members (which is
// what the one-slot arrays gave for index 1). Index 0 and everything else go
// to the game's original getters. Every first request for a player-2 index
// from a new place in the game's code is logged as [p2redirect].
// F11 toggles the redirect on/off live (it starts ON).
#pragma once

#include <cstdint>

namespace bf1 {

// Call after the code is unlocked. onGameThread must be true for the frame
// hook to be installed (patching the call site while the game thread could be
// running it is not safe); otherwise pad 2 falls back to its own thread.
void Player2InputStart(bool onGameThread);

// F11: flips the 6c redirect on/off. Logs the new state. No effect if the
// redirect was never installed.
void Player2RedirectToggle();

// 18v: true while the 6c redirect is installed and ON (the input manager's getters give player 2's
// objects for index 1) - split_spawn_menus.cpp lets the interface read player 2's input only then.
bool Player2RedirectActive();

// 20e: players 3 and 4 (players34.cpp), with the game's code found for player 2 (false / no-op before that).
// PlayerInputBuild: a PlayerInput (kPlayerInputBytes) and its buffers A / B (kBufABytes / kBufBBytes) in the
// caller's memory, built and wired as player 2's (the given pad in the pad slot, no keyboard, no mouse, not
// connected yet). PlayerInputUpdate: the game's PlayerInput::Update (reads the pad, fills buffer A).
// ControlStateFill: buffer A -> the control state of player id `index` (local games only), as 7c does for 1.
constexpr uint32_t kPlayerInputBytes = 0x718;
constexpr uint32_t kBufABytes = 0x14;
constexpr uint32_t kBufBBytes = 0x140;
bool PlayerInputBuild(void* input, void* bufA, void* bufB, uintptr_t pad);
void PlayerInputSetConnected(void* input, bool connected);
void PlayerInputUpdate(void* input, float dt);
bool ControlStateFill(int index, const void* bufA);

} // namespace bf1
