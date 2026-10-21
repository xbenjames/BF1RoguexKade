// BF1GameExt - render_hooks.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/core/ (was frame_capture.hpp); file names in the comments and log lines follow the new names. Nothing else changed.
// 21j-r34: FrameWaterShotArm (the water picture scan: the next frame's water pictures saved, BF1GameExt_water\). Nothing else
// changed.
// 21j-r33: the water scan's device side (FrameWaterBegin / FrameWaterEnd / FrameWaterDetail / FrameDrawCalls). Log only.
// 21j-r15: MenuViewportBeginRegion / MenuViewportBeginOwn (the menu layer in player 1's 3-player quarter, in player 2's own
// 2-player half). Nothing else changed.
// 21j-r11: the frame-rate scan (PerfSlot, FramePerfEnter / FramePerfLeave, FramePerfScope). Nothing else changed.
// 21j-r6: FrameScopeTraceArm / FrameScopeTraceNote. Nothing else changed.
// 21j-r1: FrameCapturePresents. Nothing else changed.
// 21h-r8: MenuViewportBeginFull.
// 21h-r3: MenuViewportBeginP2 - the menu layer in player 2's box (3 / 4 players). Nothing else changed.
// 18s: MenuViewportRead - the current Direct3D viewport and target size, read only (for the player
// 2 spawn scan's menu layer line). Nothing else changed.
// Phase 11a: one-frame capture of the renderer's screen copies (2020 build).
//
// Player 1's half shows red wedges / smears around engine heat haze, command
// post holograms and other distortion: screen-space effects that copy part of
// the frame (IDirect3DDevice9::StretchRect) and draw it back warped. To see
// which rectangle each effect copies for each player's view, this wraps six of
// the device's methods - Present, StretchRect, SetRenderTarget, Clear,
// SetViewport, SetScissorRect - (log-only: every wrapper calls the original)
// and logs every call of ONE whole frame with its rectangles, the surfaces'
// sizes and the game code that made it. Three frames are captured by
// themselves (15, 45 and 75 s after player 1 is first in the world); Insert,
// Home or LB+RB+Y on pad 1 captures one more. [frame] lines.
// Phase 13e: at the end of each captured frame, the draws made after the
// frame's LAST viewport change (in split screen: player 2's HUD and any menu
// screen) are listed by their deep call chain (up to 12 game callers each), to
// find where menu drawing starts.
#pragma once

namespace bf1 {

// Finds the game's device pointer by pattern and starts a thread that wraps
// the device's methods once the device exists. Call after the code is unlocked.
// 12i: also resets the viewport to the whole target after each Present in
// split screen (INI [Display] SplitFrameClearFix, default 1), so the game's
// frame-start clear covers player 1's half too.
void FrameCaptureStart();

// Insert / Home / LB+RB+Y: capture the next whole frame.
void FrameCaptureArm();

// 13f: the split-screen menu layer's viewport (see render_hooks.cpp).
struct MenuViewportSave
{
    unsigned long x, y, w, h;
    float minZ, maxZ;
    unsigned long boxX, boxY, boxW, boxH;
};
// mode 1 = a 16:9 box centred in player 1's half, 2 = player 1's whole half.
// Returns false (nothing changed) unless the current viewport is player 2's -
// or, with player1Own (13g, player 1's HUD layer), player 1's own viewport.
bool MenuViewportBegin(int mode, bool player1Own, MenuViewportSave* save);
// 21h-r3: the same in player 2's box (3 / 4 players: the top right quarter, above the bottom row the last view is in).
// Returns false (nothing changed) unless the current viewport is a bottom row view. MenuViewportEnd puts it back.
bool MenuViewportBeginP2(int mode, MenuViewportSave* save);
// 21h-r8: the menu layer over the whole render target (mode 1: a centred 16:9 box, 2: all of it), whatever viewport was set.
bool MenuViewportBeginFull(int mode, MenuViewportSave* save);
// 21j-r15: the menu layer in a region of the render target, in fractions of it (mode 1: a 16:9 box centred in it, 2: all of
// it) - the 3-player layout's top left quarter for player 1. Returns false (nothing changed) unless a split view's viewport
// (smaller than the target) is set. MenuViewportEnd puts it back.
bool MenuViewportBeginRegion(int mode, float fx0, float fy0, float fx1, float fy1, MenuViewportSave* save);
// 21j-r15: the same in the viewport set now (the last view drawn: player 2's half in the 2-player split) - his own box.
bool MenuViewportBeginOwn(int mode, MenuViewportSave* save);
void MenuViewportEnd(const MenuViewportSave* save);

// 18s: the current viewport (x, y, w, h, minZ, maxZ) and the render target's size (boxW, boxH;
// boxX / boxY 0), read with Direct3D's own getters. Nothing is changed. False if the device is not
// known yet or a getter fails.
bool MenuViewportRead(MenuViewportSave* out);

// 21j-r1: the number of frames presented so far (the Present wrapper's count; 0 until it is in). Read only.
long FrameCapturePresents();

// 21j-r6: the scope blur's trace (players34.cpp's scope scan). Armed right before the scope blur's draw for a view, it
// logs that draw's screen copy and what Direct3D does with it until the frame's Present ([scope] trace lines): render
// target and viewport changes, clears, and the draws that sample the screen copy (the blur pass) or the target that pass
// drew into (the last pass over the view), with the viewport, render target and textures Direct3D has at each. Log only.
// False (nothing armed) while another trace runs, before the device is wrapped, or once its lines are used up.
bool FrameScopeTraceArm(int view, int player);
// a line in the running trace (nothing if none runs)
void FrameScopeTraceNote(const char* what);

// 21j-r33: THE WATER SCAN's device side (players34.cpp's [water34] scan, [Splitscreen] WaterScan; log only). players34.cpp
// opens a phase around three of the game's per-view water functions: 1 = the reflection pass (the scene mirrored into the
// reflection target), 2 = the water's draw (the water patches), 3 = the refraction copy (the view's part of the screen copied
// into a texture). Outside 2, each render target drawn into and each screen copy's target is noted as "written in view V
// (phase - 0 = the view's other drawing - and frame)"; while 2 is open each draw's textures (stages 0 - 7) are looked up there: a texture written in another view
// this frame, or in an earlier frame, is counted (FrameWaterStale). With FrameWaterDetail armed for a view, its next water
// draw's first call is logged in full ([water34] lines: each stage's texture, its size and who wrote it, the viewport and the
// render target Direct3D has, and the vertex shader constants c0 - c31). Nothing in the game changes.
// the scan on: currentView is the renderer's current view (read while no phase is open: a render target / copy target written
// then is noted as that view's "other drawing")
void FrameWaterScanOn(const volatile int* currentView);
void FrameWaterBegin(int phase, int view);
void FrameWaterEnd();
void FrameWaterDetail(int view);
// 21j-r34: the next whole frame is a water shot (see render_hooks.cpp): every render target texture each view's water samples
// saved as a .bmp in BF1GameExt_water\ next to the exe, with that draw's state in [water34] shot lines. The capture key arms
// one too. 4 shots a session at most; nothing happens unless the water scan is on.
void FrameWaterShotArm(const char* why);
// draw calls made through the wrappers so far (all kinds) - for the scan's per-phase counts
long FrameDrawCalls();
// per view (0 - 3): water draw calls that sampled a texture written in another view this frame / in an earlier frame (only),
// and those that sampled one written in their own view this frame - counted since the start
void FrameWaterStale(int view, long* otherView, long* oldFrame, long* ownView);

// 21j-r11: THE FRAME-RATE SCAN (INI [Display] PerfScan, default 1; [perf] lines). Log only: nothing in the game changes.
// Where each frame's time goes on the game's thread (the thread that calls Present): the parts below are entered and left
// around the code they name (exclusive: a part entered inside another is not counted in the outer one), timed with the
// CPU's time stamp counter; the time outside every part is the game's own code (kPerfGame). A frame ends when Present
// returns, so a frame's parts add up to the whole time since the last Present returned. Every 5 s: the frame rate, how long
// the frames took, and each part's average and longest per frame (and their averages in the frames whose work went over
// the 60 Hz budget); a SLOW FRAME line for each frame longer than [Display] PerfSpikeMs (default 25), with its parts.
enum PerfSlot : int
{
    kPerfGame = 0,      // the game: its code outside every part below (its update - AI, physics, Lua, sound - and the rest)
    kPerfRender,        // the game: its render frame (the view loop, players34.cpp's FrameWrap) outside the parts below
    kPerfPoses,         // the game: its soldiers' pose updates (the bone matrices; ragdoll.cpp's pose hook)
    kPerfHudDraw,       // the game: its HUD layer draws (each view's; spawn_ui.cpp's HUD layer hook)
    kPerfMenuDraw,      // the game: its menu layer draw (spawn_ui.cpp's menu layer hook)
    kPerfD3dDraw,       // Direct3D: the draw calls (the driver's work for them)
    kPerfD3dOther,      // Direct3D: render targets, viewports, clears, scissors, screen copies
    kPerfPresentWait,   // Direct3D: Present (the wait for the display / the graphics card)
    kPerfOurFrame,      // ours: FrameWrap's work before the game's render (players 3 / 4: cameras, spawn screens, HUD)
    kPerfOurInput,      // ours: player 2's input each frame (players 3 / 4's pads too)
    kPerfOurMenu,       // ours: the spawn screens' menu layer work (P2SpawnMenuLayer, the menu / HUD viewport moves)
    kPerfOurChooser,    // ours: the spawn screen chooser hook (the class wheel's console driver)
    kPerfOurMenus34,    // ours: players 3 / 4's own menu managers (their pause screens' input and tick, the sound)
    kPerfOurRagdoll,    // ours: the ragdolls (the pose hook: physics, bones, floors, scans)
    kPerfOurD3d,        // ours: the Direct3D wrappers' own code (this file)
    kPerfLog,           // ours: writing the log on the game's thread
    kPerfSlots
};
// True when the part was entered (pass it to FramePerfLeave): the scan is on and this is the game's thread.
bool FramePerfEnter(int slot);
void FramePerfLeave(bool entered);
struct FramePerfScope
{
    bool on;
    explicit FramePerfScope(int slot) : on(FramePerfEnter(slot)) {}
    ~FramePerfScope() { FramePerfLeave(on); }
    FramePerfScope(const FramePerfScope&) = delete;
    FramePerfScope& operator=(const FramePerfScope&) = delete;
};

// Draw-group switches (phase 11e, Steam): skip the draws made by one game
// function. Each beeps: high = switched off, low = back on.
//   Num 1..9  FrameCaptureToggleSource(0..8): flip one draw group off / on
//   Num 0     FrameCaptureAllOn(): everything back on
//   LB+RB+X   FrameCaptureCycleSkip(): one group off at a time, then all on
// (11d's effect-type labels stay in the captures; the type switches could not
// reach effects drawn later by the generic renderer, so they are retired.)
void FrameCaptureToggleSource(int index);

// Num * (phase 11f): switch off / on every draw that samples a screen texture
// (a texture that receives screen copies or is used as a render target).
void FrameCaptureToggleScreenSampling();

// Num / (phase 12a shadow scan): keep the game's render-state copy in sync when
// a state is set on Direct3D directly (candidate fix for visible shadow volumes).
void FrameCaptureToggleCacheSync();

// Num - (phase 12a motion scan): switch blur and motion blur off / on.
void FrameCaptureToggleBlur();
void FrameCaptureAllOn();
void FrameCaptureCycleSkip();

} // namespace bf1
