// BF1GameExt - render_hooks.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/core/ (was frame_capture.cpp); file names in the comments and log lines follow the new names. Nothing else changed.
// 22c: THE DIRECT3D CREATES FOR THE MEMORY LOG (log only): the device's CreateTexture / CreateVolumeTexture /
// CreateCubeTexture / CreateVertexBuffer / CreateIndexBuffer (slots 23 - 27) wrapped with the other methods - each one the
// game makes is counted with its bytes and pool (mem_log.cpp's [mem] lines: a managed one keeps a copy in the game's address
// space), each failed one logged with its size, format, pool and caller. The call itself goes on unchanged. Nothing else
// changed.
// 21j-r35: THE WATER SHOTS, WIDER (r34's shot: the four views' reflection pictures, each right for its own camera - the
// odd-looking one is view 2's, zoomed and looking up; the water's second pass, which draws its ripples into the shared
// 2048 x 2048 screen texture #3 and reads them back, was missed: r34 picked the water's draws by their caller, and the stack
// walk gave the water draw's and the second pass's draws different callers). A shot now takes, per view, the first draw
// INTO each screen texture (logged: state, every bound stage, the shaders' constants) and the first draw SAMPLING each one
// (logged, and its picture saved at that moment), whatever code draws it; each shader met is saved once as
// shader_vs_<address>.bin / shader_ps_<address>.bin (its own instructions: how it makes the coordinates it reads a texture
// with); pictures 512 wide at most. Nothing else changed.
// 21j-r34: THE WATER PICTURE SCAN (log only; [water34] shot lines; the block above WtCopy). r33's scan found every view's
// water reading its own view's pictures of the same frame, at the right viewports - so what is wrong in views 3 / 4 is in
// the pictures themselves or in where the water's shaders read them. A shot (the capture key - Insert, Home or LB+RB+Y -
// and one by itself, players34.cpp's FrameWaterShotArm; 4 at most) saves, for one whole frame, every render target
// texture the water's draws sample (the water draw's own, and the draws of the same patch-drawing code: the water's second
// pass and its ripple picture) the first time each view's water samples it - what that view's water reads at that moment -
// as a .bmp in BF1GameExt_water\ next to the exe (colour left, alpha right as grey, 1024 wide at most), and logs that
// draw's viewport, render target, sampler, coordinates, scissor, shaders, vertex shader c0 - c63 and pixel shader c0 - c15.
// The draw wrappers and Present call it first; they still call the originals exactly as before. Nothing else changed.
// 21j-r33: THE WATER SCAN's device side (log only; [water34] lines - the block above the frame-rate scan): players34.cpp opens
// a phase around the game's reflection pass, water draw and refraction copy for each view (FrameWaterBegin / End). In the
// reflection pass and the copy, each draw's render target and the copy's target are noted as written by that view this frame;
// in the water draw, each draw's textures (stages 0 - 7) are looked up there and counted - written in its own view this frame,
// in ANOTHER view this frame, or in an earlier frame only (FrameWaterStale). FrameWaterDetail(view) logs that view's next water
// draw's first call in full: each stage's texture, size and who wrote it, Direct3D's viewport and render target, the vertex
// shader constants c0 - c31. The StretchRect and draw wrappers call it right after the scope trace; they still call the
// originals exactly as before. Nothing else changed.
// 21j-r15: MenuViewportBeginRegion / MenuViewportBeginOwn (spawn_ui.cpp's menu layer): a 16:9 box in a region of the render
// target - player 1's top left quarter in the 3-player layout (his popups and option screens were drawn across the whole top
// half), player 2's own half in the 2-player split (his were stretched over all of it) - only while a split view's viewport is
// set. Nothing else changed.
// 21j-r14: THE GLYPH LEAK FIXED ([Display] GlyphLeakFix, default 1; in the [glyphs] lines - see the block above Hook_Present).
// r13's log: no text scrambled (the pool ran low at 392 s and got a block), but the chunks in use never came back down (611 at
// the main menu at start, 1645 back at the sign-in screen after the match). The game's text destructor drops a text's chunks
// with the arrays that hold them (RedGlyphCacheElement's cleanup) - they are never returned to the pool. Its single jump thunk
// goes through BF1GameExt, which runs the game's own ReleaseGlyphs on the element first (what SetFont does). The 30 s [glyphs]
// line counts the texts deleted and the chunks given back. GlyphPoolGrow stays as the safety net. Nothing else changed.
// 21j-r13: THE SCRAMBLED TEXT FIXED ([Display] GlyphPoolGrow, default 1; [glyphs] lines - see the block above Hook_Present).
// r12's scan found the vertex buffers clean; the bad letters come from the game's text glyph pool (1280 chunks of 8 letters for
// every text at once, made at start): with 4 quarters of HUDs, windows and menus it runs dry, and a text laid out then keeps
// its letter count with no letters - its draw shows whatever an earlier text left in the vertex buffer. The pool's allocator
// (its single jump thunk) goes through BF1GameExt, which adds 1024 chunks of its own whenever the pool runs low (at most 16
// times). r12's text garble scan stays in, off by default (TextGarbleScan=0). Nothing else changed.
// 21j-r12: THE TEXT GARBLE SCAN ([Display] TextGarbleScan, default 1; [vbscan] lines; log only - see the block above
// Hook_Present). r11's 4-player video: player 2's spawn windows, then the stats screen, drew other texts' letters (slices of
// the objectives line in his VEHICLES box and on a title bar, smeared letters), steady frame after frame - draws reading
// vertices that are not theirs. The game's vertex buffer Lock (its single jump thunk moved here) watches for a dynamic buffer
// running out of room mid-frame (WRAP), positions running past what the pool handed out (AHEAD), failed locks, locks on a freed
// wrapper (DEAD) and on other threads; each DrawPrimitive / DrawIndexedPrimitive compares the device's stream 0 with the
// buffer of the wrapper in the game's stream cache (STREAM); a summary every 10 s with the wrappers' reference counts (a
// count past 16383 wraps to 0 and frees a wrapper still in use); End writes a MARK line. The Lock calls the game's own at
// once; nothing in the game changes. Nothing else changed.
// 21j-r11: THE FRAME-RATE SCAN ([Display] PerfScan, default 1; PerfSpikeMs, default 25; [perf] lines; log only - nothing in the
// game changes). r10's log (4 players, 60 Hz with vsync) dropped to 55 - 57 fps in the busiest fights. To find which part of a
// frame is slow, the game's thread (the one that calls Present) keeps a stack of parts - the game's update, its render frame,
// its soldiers' poses, its HUD / menu layers, Direct3D's draws / other calls / Present, and BF1GameExt's own parts (FrameWrap's
// players 3 / 4 work, player 2's input, the spawn screen layer, the chooser, players 3 / 4's menus, the ragdolls, these
// wrappers, the log) - timed with the CPU's time stamp counter, each tick to the part on top. A frame ends when the real
// Present returns. Every 5 s: frames, fps, frame / work times, the frames over 17.5 / 20 / 25 / 33.4 ms, each part's average and
// longest, the same for the frames whose work went over 16.7 ms, the game thread's CPU share (QueryThreadCycleTime); a SLOW
// FRAME line with every part for each frame longer than PerfSpikeMs. The other files enter their parts with FramePerfEnter /
// FramePerfLeave. Every wrapper here still calls the original exactly as before.
// 21j-r10: THE CRASH DURING AN AUTOMATIC CAPTURE (r9's log, 1 player, 4K: an access violation reading the address right past
// the render thread's stack, 0x02800000, at "mov esi, [ebp + esi*4 - 14h]" with esi 2C0h). TypeOnStack - the third stack
// walker, which every captured draw runs (MaybeDump / NoteDraw) - looked 768 entries up the stack with no end check; 18y gave
// GameCallers and DeepCallers theirs (the thread's stack top, the TEB's StackBase) but not this one. A draw made with less
// than 3 KB of stack above it (here 704 entries) read past the stack. It stops at the stack top now, as the other two.
// Nothing else changed.
// 21j-r6: the scope blur's trace (FrameScopeTraceArm / Note, [scope] trace lines; the block above Hook_Present): log only.
// 21j-r1: FrameCapturePresents - the Present count (the grass scan's frame number). Nothing else changed.
// 21h-r8: MenuViewportBeginFull - the menu layer over the whole screen (the end screens with 3 / 4 players). Nothing else changed.
// 21h-r3: MenuViewportBeginP2 - the menu layer in player 2's box in 3 / 4-player matches (the top right quarter: right of
// player 4's quarter - or of the middle with player 3's half - and above the bottom row), for player 2's popups and option
// screens (spawn_ui.cpp, while P2SpawnMenuOwner says so). MenuViewportBegin unchanged.
// 20g-r7: MenuViewportBegin in 4-player: when the viewport left by the last view is the bottom right quarter (player 4's),
// player 1's box is the top left quarter (everything left of AND above it) - it was the left half's 16:9 box, half of it
// in player 3's quarter (player 1's side / class screens were drawn a quarter of the height too low and cut off).
// Nothing else changed.
// 18y: the stack walkers (GameCallers / DeepCallers) stop at the thread's own stack top (the TEB's
// StackBase) - DeepCallers read past the end of the stack during a capture (an access violation at
// the stack's top page in 18x: its VirtualQuery region reached beyond the stack). Nothing else changed.
// 18s: MenuViewportRead (read only: the current viewport and the target's size, for the player 2
// spawn scan). Nothing else changed.
// Phase 11a (2020 build): log-only capture of one whole rendered frame.
//
// The device: every screen-copy site gets it from GetDevice ("mov eax,
// [<global>] ; ret", GOG 1.3 0x00675A60). Pattern on the blur copy site
// (proven on GOG 1.3; 2 hits - blur and scope blur - that must agree):
//   call GetDevice ; push 0 ; lea edx, [ebp-28h] ; push edx ; push [ebp-8] ;
//   mov ecx, [eax] ; lea edx, [ebp-38h] ; push edx ; push [ebp-4] ; push eax ;
//   call [ecx+88h]                                   ; StretchRect
// IDirect3DDevice9 vtable slots wrapped: 17 Present, 34 StretchRect,
// 37 SetRenderTarget, 43 Clear, 47 SetViewport, 75 SetScissorRect, and (r3)
// 81 DrawPrimitive, 82 DrawIndexedPrimitive, 83 DrawPrimitiveUP,
// 84 DrawIndexedPrimitiveUP. r3 also names the game functions behind each call:
// the two nearest return addresses into the exe's code found on the stack (a
// value counts if the bytes before it are a call instruction), and between
// logged events it counts the draws and who made them.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "render_hooks.hpp"
#include "game_build.hpp"
#include "log.hpp"
#include "mem_log.hpp"
#include "game_check.hpp"
#include "spawn_manager.hpp"

#ifdef _MSC_VER
extern "C" void* _ReturnAddress(void);
#pragma intrinsic(_ReturnAddress)
#define BF1_RETURN_ADDRESS() _ReturnAddress()
#else
#define BF1_RETURN_ADDRESS() __builtin_return_address(0)
#endif
#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h> // 21j-r11: __rdtsc (the frame-rate scan), as ragdoll.cpp has it
#endif

namespace bf1 {
namespace {

const char* const kCopySite =
    "E8 ?? ?? ?? ?? 6A 00 8D 55 D8 52 FF 75 F8 8B 08 8D 55 C8 52 FF 75 FC 50 FF 91 88 00 00 00";
const char* const kGetDeviceBody = "A1 ?? ?? ?? ?? C3";

constexpr int kSlotPresent = 17;
constexpr int kSlotStretchRect = 34;
constexpr int kSlotSetRenderTarget = 37;
constexpr int kSlotClear = 43;
constexpr int kSlotSetViewport = 47;
constexpr int kSlotSetRenderState = 57;
constexpr int kSlotSetVertexShader = 92;
constexpr int kSlotSetVertexShaderConstantF = 94;
constexpr int kSlotSetPixelShader = 107;
constexpr int kSlotSetPixelShaderConstantF = 109;
constexpr int kSlotSetTexture = 65;
constexpr int kSlotSetScissorRect = 75;
constexpr int kSlotDrawPrimitive = 81;
constexpr int kSlotDrawIndexedPrimitive = 82;
constexpr int kSlotDrawPrimitiveUP = 83;
constexpr int kSlotDrawIndexedPrimitiveUP = 84;
constexpr int kMaxLines = 5000;
// r2: automatic captures, this many ms after player 1 first appears in the world
const DWORD kAutoCaptureMs[] = { 15000, 45000, 75000 };
constexpr int kAutoCaptures = static_cast<int>(sizeof(kAutoCaptureMs) / sizeof(kAutoCaptureMs[0]));

struct Viewport9
{
    DWORD X, Y, Width, Height;
    float MinZ, MaxZ;
};
struct SurfaceDesc9
{
    DWORD Format, Type, Usage, Pool, MultiSampleType, MultiSampleQuality, Width, Height;
};

typedef long(__stdcall* Present_t)(void*, const RECT*, const RECT*, HWND, const void*);
typedef long(__stdcall* StretchRect_t)(void*, void*, const RECT*, void*, const RECT*, unsigned);
typedef long(__stdcall* SetRenderTarget_t)(void*, DWORD, void*);
typedef long(__stdcall* Clear_t)(void*, DWORD, const void*, DWORD, DWORD, float, DWORD);
typedef long(__stdcall* SetViewport_t)(void*, const Viewport9*);
typedef long(__stdcall* SetScissorRect_t)(void*, const RECT*);
typedef long(__stdcall* GetDesc_t)(void*, SurfaceDesc9*);
typedef long(__stdcall* SetTexture_t)(void*, DWORD, void*);
typedef long(__stdcall* SetRenderState_t)(void*, DWORD, DWORD);
typedef long(__stdcall* SetShader_t)(void*, void*);
typedef long(__stdcall* SetShaderConstantF_t)(void*, unsigned, const float*, unsigned);
typedef long(__stdcall* GetContainer_t)(void*, const GUID&, void**);
typedef unsigned long(__stdcall* Release_t)(void*);
typedef long(__stdcall* DrawPrimitive_t)(void*, unsigned, unsigned, unsigned);
typedef long(__stdcall* DrawIndexedPrimitive_t)(void*, unsigned, int, unsigned, unsigned, unsigned,
                                                unsigned);
typedef long(__stdcall* DrawPrimitiveUP_t)(void*, unsigned, unsigned, const void*, unsigned);
typedef long(__stdcall* DrawIndexedPrimitiveUP_t)(void*, unsigned, unsigned, unsigned, unsigned,
                                                  const void*, unsigned, const void*, unsigned);

Present_t g_present = nullptr;
StretchRect_t g_stretchRect = nullptr;
SetRenderTarget_t g_setRenderTarget = nullptr;
Clear_t g_clear = nullptr;
SetViewport_t g_setViewport = nullptr;
SetScissorRect_t g_setScissorRect = nullptr;
SetTexture_t g_setTexture = nullptr;
SetRenderState_t g_setRenderState = nullptr;
SetShader_t g_setVertexShader = nullptr;
SetShader_t g_setPixelShader = nullptr;
SetShaderConstantF_t g_setVsConstF = nullptr;
SetShaderConstantF_t g_setPsConstF = nullptr;

// 12b shader-constant dump. Both players' views run the same passes (12a), so
// the difference must be in the numbers: this keeps a copy of the vertex and
// pixel shader constants, the bound shaders, the render target size and which
// player's view is current, and during a capture dumps them at every shadow /
// blur draw and at the first ordinary draw of each view.
constexpr unsigned kConstRegs = 256;
float g_vsConst[kConstRegs][4];
float g_psConst[kConstRegs][4];
void* g_vs = nullptr;
void* g_ps = nullptr;
unsigned g_rtW = 0;
unsigned g_rtH = 0;
int g_view = -1;            // 0 = player 1's view, 1 = player 2's (from the split viewport)
DWORD g_vpX = 0, g_vpY = 0, g_vpW = 0, g_vpH = 0;
bool g_firstDumped[2] = {};
int g_dumpsThisCapture = 0;
// 12g: dump each (view, game function, stencil on/off) group once per capture
struct DumpedKey
{
    int view;
    uint32_t caller;
    bool stencil;
};
DumpedKey g_dumped[64];
int g_dumpedCount = 0;

// 12c hook health: how many calls each wrapper receives, and whether the
// device's method table still points at the wrappers.
enum HookIndex
{
    kHPresent, kHStretchRect, kHSetRenderTarget, kHClear, kHSetViewport, kHSetScissorRect,
    kHSetRenderState, kHSetTexture, kHSetVertexShader, kHSetVsConstF, kHSetPixelShader,
    kHSetPsConstF, kHDrawPrimitive, kHDrawIndexedPrimitive, kHDrawPrimitiveUP,
    kHDrawIndexedPrimitiveUP, kHookCount
};
volatile LONG g_calls[kHookCount] = {};
void** g_vtable = nullptr;

// 12a shadow scan. The game's SetRenderState wrapper (GOG 1.3 0x00676D40) keeps
// its own copy of every render state and SKIPS the call when the copy already
// holds the value. A state set on the device directly (bypassing the wrapper)
// leaves the copy stale - then e.g. "colour writes off" for the stencil shadow
// volumes is skipped and the volumes are drawn visibly. Found by pattern (the
// whole 97-byte function, unique on GOG 1.3); the copy's address is read from
// it, and the wrapper's own device call returns to wrapper + 94.
const char* const kSetRenderStateWrapper =
    "55 8B EC 8B 55 08 56 8B 75 0C 8B 0C 95 ?? ?? ?? ?? 3B F1 74 49 83 3D ?? ?? ?? ?? 00 74 29 80 BA ?? "
    "?? ?? ?? 00 75 20 A1 ?? ?? ?? ?? C6 82 ?? ?? ?? ?? 01 89 14 C5 ?? ?? ?? ?? 89 0C C5 ?? ?? ?? ?? 40 "
    "A3 ?? ?? ?? ?? A1 ?? ?? ?? ?? 56 89 34 95 ?? ?? ?? ?? 52 8B 08 50 FF 91 E4 00 00 00 5E 5D C3";
constexpr uint32_t kWrapperCacheOperand = 13;
constexpr uint32_t kWrapperCacheOperand2 = 80;
constexpr uint32_t kWrapperDeviceCallReturn = 94;
constexpr DWORD kRsZWrite = 14;
constexpr DWORD kRsAlphaBlend = 27;
constexpr DWORD kRsStencilEnable = 52;
constexpr DWORD kRsColorWrite = 168;
uintptr_t g_rsWrapperReturn = 0; // the wrapper's own device call returns here
uintptr_t g_rsCopy = 0;          // the game's copy: DWORD per render state
DWORD g_rsDevice[256];           // what Direct3D actually has (as far as we saw)
bool g_rsKnown[256];
volatile LONG g_cacheSync = 0;   // Num /: keep the game's copy in sync with direct sets
struct DirectSet
{
    uint32_t caller;
    DWORD state;
};
DirectSet g_directSeen[48];
int g_directSeenCount = 0;
unsigned g_directTotal = 0;
int g_mismatchLogged = 0;

// 11f screen-copy tracking: textures that receive StretchRect copies of the
// screen, or that are used as render targets, are "screen textures". A draw
// with one bound (any of stages 0-7) samples the screen. Num * switches every
// such draw off at once.
const GUID kIID_IDirect3DTexture9 = { 0x85c31227, 0x3de5, 0x4f00,
                                      { 0x9b, 0x3a, 0xf1, 0x1a, 0xc3, 0x8c, 0x18, 0xb5 } };
struct ScreenTex
{
    void* tex;
    unsigned w;
    unsigned h;
    bool copyTarget;   // receives StretchRect copies
    bool renderTarget; // used as a render target
};
constexpr int kMaxScreenTex = 24;
ScreenTex g_screenTex[kMaxScreenTex];
int g_screenTexCount = 0;
void* g_bound[8] = {};
volatile LONG g_skipScreenSampling = 0;
DrawPrimitive_t g_drawPrimitive = nullptr;
DrawIndexedPrimitive_t g_drawIndexedPrimitive = nullptr;
DrawPrimitiveUP_t g_drawPrimitiveUP = nullptr;
DrawIndexedPrimitiveUP_t g_drawIndexedPrimitiveUP = nullptr;

// Draws since the last logged event, grouped by the pair of game functions
// that made them.
struct DrawGroup
{
    uint32_t a; // nearest game return address (rva)
    uint32_t b; // the next one up
    int type;   // effect type on the stack (11d), or -1
    int screen; // screen texture it samples (11f), or -1
    DWORD stencil;   // STENCILENABLE on the device (12a)
    DWORD colorMask; // COLORWRITEENABLE on the device (12a)
    DWORD zWrite;    // ZWRITEENABLE on the device (12a)
    unsigned count;
};
constexpr int kMaxGroups = 12;
DrawGroup g_groups[kMaxGroups];
int g_groupCount = 0;
unsigned g_drawsOther = 0;

// 11d effect scanner. Effect types are found by their RTTI type names at
// runtime (no hard-coded addresses: works on GOG 1.3 and Steam). A draw belongs
// to a type if one of that type's virtual methods is on the stack when it is
// made. Methods shared by more than one type (common base classes) are left
// out, so a label always means exactly one effect type. During a capture every
// group of draws is labelled; Page Down / LB+RB+X switches off one whole type
// at a time (its draws are skipped), then everything back on.
struct EffectType
{
    const char* label;
    const char* classes[8];
};
const EffectType kTypes[] = {
    { "motion blur", { "MotionBlurEffect", nullptr } },
    { "blur", { "BlurEffect", "ScopeBlurEffect", nullptr } },
    { "god rays", { "GodrayManager", "GodrayEffectObject", "GodrayClass", nullptr } },
    { "light beams",
      { "LightBeam", "LightBeamEffect", "ManagedLightBeam", "pcLightBeamShader", nullptr } },
    { "sun / lens flare",
      { "SunFlare", "SunFlareEffect", "LensFlare", "LensFlareEffect", "pcFlareShader", "SkySun",
        "SunSkyRenderable", nullptr } },
    { "command post / hologram",
      { "CommandPost", "CommandPostClass", "EntityHologram", "EntityHologramClass", nullptr } },
    { "heat shimmer / refraction", { "HeatShimmerEffect", "pcRefractionShader", nullptr } },
    { "screen filter", { "ScreenFilterEffect", nullptr } },
    { "shadows",
      { "RedStencilShadow", "pcStencilShadowShader", "pcShadowQuadShader", "pcShadowQuadRenderable",
        "RedShadowProxy", "RedShadowBlob", "WorldShadowMapEffect", nullptr } },
};
constexpr int kTypeCount = static_cast<int>(sizeof(kTypes) / sizeof(kTypes[0]));

struct MethodRange
{
    uintptr_t start;
    uintptr_t end;
    int type;
};
constexpr int kMaxRanges = 1024;
MethodRange g_ranges[kMaxRanges];
int g_rangeCount = 0;
int g_methodsPerType[kTypeCount] = {};
volatile LONG g_skipMask = 0; // bit t set: effect type t is switched off (11d, kept)
LONG g_cycle = -1;             // LB+RB+X: the group it last switched off alone

// 11e draw-group switches. Effects that build their shapes earlier in the frame
// and leave the drawing to the generic renderer are not on the stack when the
// draw happens, so the 11d type switches cannot reach them. These switches
// instead match the game function that makes the draw (the second return
// address, as the 11a-r3 captures named them on Steam). Num 1-9 flip one
// group off / on each, Num 0 turns everything back on.
struct DrawSource
{
    uint32_t rva;
    const char* what;
};
const DrawSource kSources[] = {
    { 0x002D3423, "big end-of-view group A (58-64 user-pointer draws: particles / sprites / beams?)" },
    { 0x002D33B8, "big end-of-view group B (46-52 user-pointer draws: particles / sprites / beams?)" },
    { 0x002DC1A2, "small group, also into the effects layer" },
    { 0x002DDA63, "small group into the effects layer" },
    { 0x002DB982, "sun / lens flare draws" },
    { 0x002D59A7, "1 draw in each view's scene" },
    { 0x002D47CF, "5-16 draws in each view's scene" },
    { 0x002D63AE, "1-2 draws in each view's scene" },
    { 0x002E3F38, "1 draw in player 1's scene" },
};
constexpr int kSourceCount = static_cast<int>(sizeof(kSources) / sizeof(kSources[0]));
volatile LONG g_sourceMask = 0; // bit i set: draws from kSources[i] are skipped
bool g_sourcesReady = false;    // Steam only (the addresses come from Steam captures)
bool g_scannerReady = false;

uintptr_t g_deviceGlobal = 0;
volatile LONG g_armed = 0;
volatile LONG g_capturing = 0;
int g_lines = 0;
int g_frames = 0;
volatile LONG g_presentSeen = 0;

bool Budget()
{
    if (g_lines >= kMaxLines)
        return false;
    ++g_lines;
    if (g_lines == kMaxLines)
        Log("[frame] line limit reached, going quiet");
    return true;
}

uint32_t ReadU32(uintptr_t va)
{
    uint32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

void SurfaceSize(void* surf, char* out, size_t outSize)
{
    if (!surf)
    {
        snprintf(out, outSize, "null");
        return;
    }
    void** vt = *reinterpret_cast<void***>(surf);
    GetDesc_t getDesc = reinterpret_cast<GetDesc_t>(vt[12]);
    SurfaceDesc9 d;
    memset(&d, 0, sizeof(d));
    if (getDesc(surf, &d) >= 0)
        snprintf(out, outSize, "0x%08X (%lux%lu)", static_cast<unsigned>(reinterpret_cast<uintptr_t>(surf)),
                 static_cast<unsigned long>(d.Width), static_cast<unsigned long>(d.Height));
    else
        snprintf(out, outSize, "0x%08X", static_cast<unsigned>(reinterpret_cast<uintptr_t>(surf)));
}

void RectText(const RECT* r, char* out, size_t outSize)
{
    if (!r)
        snprintf(out, outSize, "(whole)");
    else
        snprintf(out, outSize, "(%ld,%ld)-(%ld,%ld)", static_cast<long>(r->left),
                 static_cast<long>(r->top), static_cast<long>(r->right), static_cast<long>(r->bottom));
}

bool InExeCode(uintptr_t v)
{
    uint32_t rva = 0;
    uint32_t size = 0;
    return v > 8 && CodeSection(&rva, &size) && v >= Rva(rva) + 8 && v < Rva(rva) + size;
}

// Does v look like a return address: is the instruction just before it a call?
bool AfterCall(uintptr_t v)
{
    const unsigned char* p = reinterpret_cast<const unsigned char*>(v);
    if (p[-5] == 0xE8)
        return true;                                   // call rel32
    if (p[-2] == 0xFF && (p[-1] & 0xF8) == 0xD0)
        return true;                                   // call reg
    if (p[-3] == 0xFF && (p[-2] & 0xF8) == 0x50)
        return true;                                   // call [reg+disp8]
    if (p[-6] == 0xFF && ((p[-5] & 0xF8) == 0x90 || p[-5] == 0x15))
        return true;                                   // call [reg+disp32] / [abs]
    if (p[-3] == 0xFF && p[-2] == 0x14)
        return true;                                   // call [sib]
    return false;
}

// The nearest game return addresses on the stack above this point.
int GameCallers(uint32_t* out, int max)
{
    volatile uintptr_t marker = 0;
    const uintptr_t* sp = reinterpret_cast<const uintptr_t*>(const_cast<uintptr_t*>(&marker));
    const uintptr_t top = reinterpret_cast<uintptr_t>(reinterpret_cast<const NT_TIB*>(NtCurrentTeb())->StackBase); // 18y
    int n = 0;
    for (int i = 0; i < 96 && n < max; ++i)
    {
        if (reinterpret_cast<uintptr_t>(sp + i) + sizeof(uintptr_t) > top)
            break;
        const uintptr_t v = sp[i];
        if (InExeCode(v) && AfterCall(v))
            out[n++] = static_cast<uint32_t>(v - ExeBase());
    }
    return n;
}

// 13e: a deep call chain (up to 'max' game return addresses), scanning up to the
// stack's real top: the end of the committed region holding the current stack
// address (VirtualQuery), so it never reads past the stack
int DeepCallers(uint32_t* out, int max)
{
    volatile uintptr_t marker = 0;
    const uintptr_t* sp = reinterpret_cast<const uintptr_t*>(const_cast<uintptr_t*>(&marker));
    MEMORY_BASIC_INFORMATION mbi;
    if (!VirtualQuery(reinterpret_cast<LPCVOID>(reinterpret_cast<uintptr_t>(sp)), &mbi, sizeof(mbi)))
        return 0;
    uintptr_t top = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
    const uintptr_t stackBase = reinterpret_cast<uintptr_t>(reinterpret_cast<const NT_TIB*>(NtCurrentTeb())->StackBase); // 18y: the thread's stack top
    if (stackBase > reinterpret_cast<uintptr_t>(sp) && stackBase < top)
        top = stackBase;
    int n = 0;
    for (int i = 0; i < 4096 && n < max; ++i)
    {
        if (reinterpret_cast<uintptr_t>(sp + i) + sizeof(uintptr_t) > top)
            break;
        const uintptr_t v = sp[i];
        if (InExeCode(v) && AfterCall(v))
            out[n++] = static_cast<uint32_t>(v - ExeBase());
    }
    return n;
}

// 13e: the draws after the captured frame's LAST viewport change (player 2's
// HUD and the menu screens in split screen), one entry per distinct deep chain.
// Reset at every SetViewport while capturing; printed at Present.
struct SegChain
{
    uint32_t c[12];
    int n;
    unsigned count;
};
constexpr int kMaxSegChains = 48;
SegChain g_seg[kMaxSegChains];
int g_segCount = 0;
unsigned g_segOther = 0;
unsigned g_segDraws = 0;

void SegReset()
{
    g_segCount = 0;
    g_segOther = 0;
    g_segDraws = 0;
}

void SegNote()
{
    uint32_t c[12] = {};
    const int n = DeepCallers(c, 12);
    ++g_segDraws;
    for (int i = 0; i < g_segCount; ++i)
    {
        if (g_seg[i].n == n && memcmp(g_seg[i].c, c, static_cast<size_t>(n) * sizeof(uint32_t)) == 0)
        {
            ++g_seg[i].count;
            return;
        }
    }
    if (g_segCount < kMaxSegChains)
    {
        memcpy(g_seg[g_segCount].c, c, sizeof(c));
        g_seg[g_segCount].n = n;
        g_seg[g_segCount].count = 1;
        ++g_segCount;
        return;
    }
    ++g_segOther;
}

void SegPrint()
{
    Log("[frame] final stretch (draws after the frame's last viewport change): %u draw(s), %d distinct call "
        "chain(s)%s",
        g_segDraws, g_segCount, g_segOther ? " (+ more not listed)" : "");
    for (int i = 0; i < g_segCount; ++i)
    {
        char line[256];
        size_t len = 0;
        line[0] = 0;
        for (int k = 0; k < g_seg[i].n && len + 16 < sizeof(line); ++k)
        {
            const int w = snprintf(line + len, sizeof(line) - len, "%s0x%08X", k ? " < " : "",
                                   static_cast<unsigned>(g_seg[i].c[k]));
            if (w > 0)
                len += static_cast<size_t>(w);
        }
        Log("[frame]   chain %d, %u draw(s): %s", i + 1, g_seg[i].count,
            g_seg[i].n ? line : "(no game caller found)");
    }
}

void ChainText(char* out, size_t outSize)
{
    uint32_t c[3] = {};
    const int n = GameCallers(c, 3);
    if (n == 0)
        snprintf(out, outSize, "(no game caller found)");
    else if (n == 1)
        snprintf(out, outSize, "rva 0x%08X", static_cast<unsigned>(c[0]));
    else if (n == 2)
        snprintf(out, outSize, "rva 0x%08X < 0x%08X", static_cast<unsigned>(c[0]),
                 static_cast<unsigned>(c[1]));
    else
        snprintf(out, outSize, "rva 0x%08X < 0x%08X < 0x%08X", static_cast<unsigned>(c[0]),
                 static_cast<unsigned>(c[1]), static_cast<unsigned>(c[2]));
}

// The texture a surface belongs to (null for the back buffer and plain surfaces).
void* TextureOf(void* surf)
{
    if (!surf)
        return nullptr;
    void** vt = *reinterpret_cast<void***>(surf);
    GetContainer_t getContainer = reinterpret_cast<GetContainer_t>(vt[11]);
    void* tex = nullptr;
    if (getContainer(surf, kIID_IDirect3DTexture9, &tex) < 0 || !tex)
        return nullptr;
    void** tvt = *reinterpret_cast<void***>(tex);
    reinterpret_cast<Release_t>(tvt[2])(tex); // GetContainer added a reference
    return tex;
}

void MarkScreenTexture(void* surf, bool copyTarget)
{
    void* tex = TextureOf(surf);
    if (!tex)
        return;
    for (int i = 0; i < g_screenTexCount; ++i)
    {
        if (g_screenTex[i].tex == tex)
        {
            if (copyTarget)
                g_screenTex[i].copyTarget = true;
            else
                g_screenTex[i].renderTarget = true;
            return;
        }
    }
    if (g_screenTexCount >= kMaxScreenTex)
        return;
    SurfaceDesc9 d;
    memset(&d, 0, sizeof(d));
    void** vt = *reinterpret_cast<void***>(surf);
    reinterpret_cast<GetDesc_t>(vt[12])(surf, &d);
    ScreenTex& t = g_screenTex[g_screenTexCount];
    t.tex = tex;
    t.w = d.Width;
    t.h = d.Height;
    t.copyTarget = copyTarget;
    t.renderTarget = !copyTarget;
    ++g_screenTexCount;
    Log("[frame] screen texture #%d: 0x%08X (%ux%u), first seen as %s", g_screenTexCount,
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(tex)), t.w, t.h,
        copyTarget ? "a screen-copy destination" : "a render target");
}

// The screen texture (index) bound for this draw, or -1.
int ScreenTextureBound()
{
    for (int s = 0; s < 8; ++s)
    {
        if (!g_bound[s])
            continue;
        for (int i = 0; i < g_screenTexCount; ++i)
        {
            if (g_screenTex[i].tex == g_bound[s])
                return i;
        }
    }
    return -1;
}

// The effect type whose (unshared) method is nearest up the stack, or -1.
int TypeOnStack()
{
    if (!g_rangeCount)
        return -1;
    volatile uintptr_t marker = 0;
    const uintptr_t* sp = reinterpret_cast<const uintptr_t*>(const_cast<uintptr_t*>(&marker));
    const uintptr_t top = reinterpret_cast<uintptr_t>(reinterpret_cast<const NT_TIB*>(NtCurrentTeb())->StackBase); // 21j-r10
    for (int i = 0; i < 768; ++i)
    {
        if (reinterpret_cast<uintptr_t>(sp + i) + sizeof(uintptr_t) > top) // 21j-r10: never past the thread's stack top
            break;
        const uintptr_t v = sp[i];
        if (!InExeCode(v) || !AfterCall(v))
            continue;
        // last range starting at or before v
        int lo = 0;
        int hi = g_rangeCount - 1;
        int found = -1;
        while (lo <= hi)
        {
            const int mid = (lo + hi) / 2;
            if (g_ranges[mid].start <= v)
            {
                found = mid;
                lo = mid + 1;
            }
            else
            {
                hi = mid - 1;
            }
        }
        if (found >= 0 && v < g_ranges[found].end)
            return g_ranges[found].type;
    }
    return -1;
}

// True if this draw belongs to an effect type that is switched off.
bool SkipThisDraw()
{
    if (g_skipScreenSampling && ScreenTextureBound() >= 0)
        return true;
    const LONG sources = g_sourceMask;
    if (sources)
    {
        uint32_t c[2] = {};
        GameCallers(c, 2);
        for (int i = 0; i < kSourceCount; ++i)
        {
            if ((sources & (1L << i)) && (c[1] == kSources[i].rva || c[0] == kSources[i].rva))
                return true;
        }
    }
    const LONG mask = g_skipMask;
    if (!mask)
        return false;
    const int t = TypeOnStack();
    return t >= 0 && (mask & (1L << t)) != 0;
}

void Feedback(bool switchedOff)
{
    // high beep = something switched off, low beep = something back on
    Beep(switchedOff ? 1200 : 500, 90);
}

void NoteDraw()
{
    SegNote();
    uint32_t c[2] = {};
    GameCallers(c, 2);
    const int type = TypeOnStack();
    const int screen = ScreenTextureBound();
    const DWORD stencil = g_rsKnown[kRsStencilEnable] ? g_rsDevice[kRsStencilEnable] : 0;
    const DWORD colorMask = g_rsKnown[kRsColorWrite] ? g_rsDevice[kRsColorWrite] : 0xF;
    const DWORD zWrite = g_rsKnown[kRsZWrite] ? g_rsDevice[kRsZWrite] : 1;
    for (int i = 0; i < g_groupCount; ++i)
    {
        if (g_groups[i].a == c[0] && g_groups[i].b == c[1] && g_groups[i].type == type &&
            g_groups[i].screen == screen && g_groups[i].stencil == stencil &&
            g_groups[i].colorMask == colorMask && g_groups[i].zWrite == zWrite)
        {
            ++g_groups[i].count;
            return;
        }
    }
    if (g_groupCount < kMaxGroups)
    {
        g_groups[g_groupCount].a = c[0];
        g_groups[g_groupCount].b = c[1];
        g_groups[g_groupCount].type = type;
        g_groups[g_groupCount].screen = screen;
        g_groups[g_groupCount].stencil = stencil;
        g_groups[g_groupCount].colorMask = colorMask;
        g_groups[g_groupCount].zWrite = zWrite;
        g_groups[g_groupCount].count = 1;
        ++g_groupCount;
        return;
    }
    ++g_drawsOther;
}

// Log the draws made since the last logged event, then start counting again.
void FlushDraws()
{
    if (!g_groupCount && !g_drawsOther)
        return;
    for (int i = 0; i < g_groupCount; ++i)
    {
        if (!Budget())
            continue;
        char screen[64] = "";
        if (g_groups[i].screen >= 0)
            snprintf(screen, sizeof(screen), "  <samples screen texture #%d (%ux%u)>",
                     g_groups[i].screen + 1, g_screenTex[g_groups[i].screen].w,
                     g_screenTex[g_groups[i].screen].h);
        char stencil[64] = "";
        if (g_groups[i].stencil)
            snprintf(stencil, sizeof(stencil), "  {stencil ON, colour mask 0x%lX, z-write %lu}",
                     static_cast<unsigned long>(g_groups[i].colorMask),
                     static_cast<unsigned long>(g_groups[i].zWrite));
        Log("[frame]     %u draw(s) by rva 0x%08X < 0x%08X%s%s%s%s%s", g_groups[i].count,
            static_cast<unsigned>(g_groups[i].a), static_cast<unsigned>(g_groups[i].b),
            g_groups[i].type >= 0 ? "  [" : "", g_groups[i].type >= 0 ? kTypes[g_groups[i].type].label : "",
            g_groups[i].type >= 0 ? "]" : "", screen, stencil);
    }
    if (g_drawsOther && Budget())
        Log("[frame]     %u more draw(s) by other callers", g_drawsOther);
    g_groupCount = 0;
    g_drawsOther = 0;
}

void CaptureWrapBegin(void* dev);
void CaptureWrapEnd();

// 12i frame-start clear fix. The game clears the whole screen (colour, depth,
// stencil) at the start of each frame with Clear(0 rects) - which clears only
// the CURRENT viewport. In split screen that is still player 2's half from the
// end of the previous frame, so player 1's half never gets its colour cleared:
// the sky / clouds blend onto stale pixels and moving things leave trails (and
// the stencil volumes' colour was painted into that stale area). Right after
// each Present, in split screen, the viewport goes back to the whole render
// target - as in single player - so the game's own frame-start clear covers
// both halves. The game sets each player's viewport itself afterwards.
bool g_frameClearFix = true;
DWORD g_lastClearFixLog = 0;
typedef long(__stdcall* GetRenderTarget_t)(void*, DWORD, void**);
constexpr int kSlotGetRenderTarget = 38;
typedef long(__stdcall* GetViewport_t)(void*, Viewport9*);
constexpr int kSlotGetViewport = 48;

uint32_t LocalPlayersNow()
{
    const BuildInfo* b = CurrentBuild();
    if (!b || !b->localPlayerCountRva)
        return 0;
    return ReadU32(Rva(b->localPlayerCountRva));
}

void FullViewportForNextFrame(void* dev)
{
    void** vt = *reinterpret_cast<void***>(dev);
    void* rt = nullptr;
    if (reinterpret_cast<GetRenderTarget_t>(vt[kSlotGetRenderTarget])(dev, 0, &rt) < 0 || !rt)
        return;
    SurfaceDesc9 d;
    memset(&d, 0, sizeof(d));
    void** svt = *reinterpret_cast<void***>(rt);
    const bool ok = reinterpret_cast<GetDesc_t>(svt[12])(rt, &d) >= 0;
    reinterpret_cast<Release_t>(svt[2])(rt);
    if (!ok || !d.Width || !d.Height)
        return;
    Viewport9 vp;
    vp.X = 0;
    vp.Y = 0;
    vp.Width = d.Width;
    vp.Height = d.Height;
    vp.MinZ = 0.0f;
    vp.MaxZ = 1.0f;
    g_setViewport(dev, &vp); // Direct3D's own SetViewport (not our logging wrapper)
    if (!g_lastClearFixLog)
    {
        g_lastClearFixLog = GetTickCount();
        Log("[clearfix] %lu ms: split screen - viewport reset to the whole %lux%lu target after each frame, so "
            "the game's frame-start clear covers both halves",
            ScanMsSinceAttach(), static_cast<unsigned long>(d.Width), static_cast<unsigned long>(d.Height));
    }
}

// ---- 21j-r6: the scope blur's trace ([scope] trace lines) -------------------
// players34.cpp arms it right before the scope blur's draw for a view (the first times each view's viewport has a
// new size). From then until the frame's Present: the draw's screen copy (StretchRect: its source rectangle, the
// texture it goes to), render target and viewport changes and clears, and the draws that sample what the trace saw
// made - a draw sampling the screen copy (the blur pass: drawn into which target, with which viewport) and a draw
// sampling that target (the last pass over the view) - each with the viewport and render target Direct3D has at the
// draw, the textures on stages 0 - 3 and stage 1's address modes. What it answers: the blur pass draws a quad in clip
// space over the top left (256 / target width) x (256 / target height) of ... the player's viewport (the blur picture
// in the view's own place) or the whole target (at its top left corner)? The last pass samples it at (the view's share
// of the screen) x (256 / target size) - right only for a view at the screen's top left.
typedef long(__stdcall* StGetTexture_t)(void*, DWORD, void**);
typedef long(__stdcall* StGetSampler_t)(void*, DWORD, DWORD, DWORD*);
typedef long(__stdcall* StGetLevelDesc_t)(void*, unsigned, SurfaceDesc9*);
typedef unsigned(__stdcall* StGetType_t)(void*);
constexpr int kStSlotGetTexture = 64;
constexpr int kStSlotGetSampler = 68;
constexpr int kStEach = 30;   // detail lines per trace
constexpr int kStMax = 420;   // trace lines in all
bool g_stOn = false;          // a trace runs (until the frame's Present)
int g_stLeft = 0;             // its detail lines left
int g_stLines = 0;
int g_stId = 0;
int g_stView = -1;
int g_stPlayer = 0;
void* g_stCopyTex = nullptr;  // the texture the trace's first screen copy went to
void* g_stPassRt = nullptr;   // the render target the first draw sampling it drew into (the blur picture)
void* g_stPassTex = nullptr;  // that target's texture
int g_stDraws = 0;            // draws while the trace ran
int g_stCopyDraws = 0;        // ... sampling the screen copy
int g_stPassDraws = 0;        // ... sampling the blur picture

bool StLine()
{
    if (!g_stOn || g_stLeft <= 0 || g_stLines >= kStMax)
        return false;
    --g_stLeft;
    ++g_stLines;
    return true;
}

void StTexText(void* tex, char* out, size_t n)
{
    if (!tex)
    {
        snprintf(out, n, "none");
        return;
    }
    void** vt = *reinterpret_cast<void***>(tex);
    const unsigned type = reinterpret_cast<StGetType_t>(vt[10])(tex);
    SurfaceDesc9 d;
    memset(&d, 0, sizeof(d));
    if (type == 3 && reinterpret_cast<StGetLevelDesc_t>(vt[17])(tex, 0, &d) >= 0)
        snprintf(out, n, "0x%08X (%lux%lu)%s%s", static_cast<unsigned>(reinterpret_cast<uintptr_t>(tex)),
                 static_cast<unsigned long>(d.Width), static_cast<unsigned long>(d.Height),
                 tex == g_stCopyTex ? " =the screen copy" : "", (g_stPassTex && tex == g_stPassTex) ? " =the blur picture" : "");
    else
        snprintf(out, n, "0x%08X (type %u)", static_cast<unsigned>(reinterpret_cast<uintptr_t>(tex)), type);
}

// the viewport and render target Direct3D has now
void StState(void* dev, char* out, size_t n, void** rtOut)
{
    void** vt = *reinterpret_cast<void***>(dev);
    Viewport9 vp;
    memset(&vp, 0, sizeof(vp));
    const bool vpOk = reinterpret_cast<GetViewport_t>(vt[kSlotGetViewport])(dev, &vp) >= 0;
    void* rt = nullptr;
    char rts[48] = "?";
    char rtt[64] = "none";
    if (reinterpret_cast<GetRenderTarget_t>(vt[kSlotGetRenderTarget])(dev, 0, &rt) >= 0 && rt)
    {
        SurfaceSize(rt, rts, sizeof(rts));
        StTexText(TextureOf(rt), rtt, sizeof(rtt));
        void** svt = *reinterpret_cast<void***>(rt);
        reinterpret_cast<Release_t>(svt[2])(rt); // (the device keeps its own reference)
    }
    if (rtOut)
        *rtOut = rt;
    if (vpOk)
        snprintf(out, n, "viewport x %lu y %lu w %lu h %lu, render target %s (its texture %s)", static_cast<unsigned long>(vp.X),
                 static_cast<unsigned long>(vp.Y), static_cast<unsigned long>(vp.Width), static_cast<unsigned long>(vp.Height), rts,
                 rtt);
    else
        snprintf(out, n, "viewport ?, render target %s (its texture %s)", rts, rtt);
}

void StCopy(void* dev, void* src, const RECT* sr, void* dst, const RECT* dr)
{
    if (!g_stOn)
        return;
    void* const tex = TextureOf(dst);
    if (!g_stCopyTex && tex)
        g_stCopyTex = tex;
    if (!StLine())
        return;
    char s[48], d[48], srt[64], drt[64], st[200], tt[80];
    SurfaceSize(src, s, sizeof(s));
    SurfaceSize(dst, d, sizeof(d));
    RectText(sr, srt, sizeof(srt));
    RectText(dr, drt, sizeof(drt));
    StTexText(tex, tt, sizeof(tt));
    StState(dev, st, sizeof(st), nullptr);
    Log("[scope] trace %d: screen copy (StretchRect) %s %s -> %s %s (its texture %s); %s", g_stId, s, srt, d, drt, tt, st);
}

void StDraw(void* dev, const char* what, unsigned type, unsigned count)
{
    if (!g_stOn)
        return;
    ++g_stDraws;
    void** vt = *reinterpret_cast<void***>(dev);
    void* tex[4] = {};
    int copyAt = -1;
    int passAt = -1;
    for (DWORD s = 0; s < 4; ++s)
    {
        void* t = nullptr;
        if (reinterpret_cast<StGetTexture_t>(vt[kStSlotGetTexture])(dev, s, &t) >= 0 && t)
        {
            tex[s] = t;
            reinterpret_cast<Release_t>((*reinterpret_cast<void***>(t))[2])(t); // (bound: the device keeps it)
        }
        if (tex[s] && g_stCopyTex && tex[s] == g_stCopyTex && copyAt < 0)
            copyAt = static_cast<int>(s);
        if (tex[s] && g_stPassTex && tex[s] == g_stPassTex && passAt < 0)
            passAt = static_cast<int>(s);
    }
    if (copyAt < 0 && passAt < 0)
        return;
    void* rt = nullptr;
    char st[200];
    StState(dev, st, sizeof(st), &rt);
    if (copyAt >= 0)
    {
        ++g_stCopyDraws;
        if (!g_stPassRt && rt)
        {
            g_stPassRt = rt;
            g_stPassTex = TextureOf(rt);
        }
    }
    else
        ++g_stPassDraws;
    if (!StLine())
        return;
    char t[4][80];
    for (int s = 0; s < 4; ++s)
        StTexText(tex[s], t[s], sizeof(t[s]));
    DWORD au = 0;
    DWORD av = 0;
    reinterpret_cast<StGetSampler_t>(vt[kStSlotGetSampler])(dev, 1, 1, &au); // D3DSAMP_ADDRESSU / V of stage 1
    reinterpret_cast<StGetSampler_t>(vt[kStSlotGetSampler])(dev, 1, 2, &av);
    char c[80];
    ChainText(c, sizeof(c));
    Log("[scope] trace %d: %s (type %u, %u primitives) %s: %s; stages 0 - 3: %s | %s | %s | %s; stage 1 address u %lu v %lu (1 wrap, "
        "3 clamp); from %s",
        g_stId, what, type, count,
        copyAt >= 0 ? "SAMPLES THE SCREEN COPY (the blur pass)" : "SAMPLES THE BLUR PICTURE (the last pass over the view)", st, t[0], t[1],
        t[2], t[3], static_cast<unsigned long>(au), static_cast<unsigned long>(av), c);
}

void StEnd(const char* why)
{
    if (!g_stOn)
        return;
    if (g_stLines < kStMax)
    {
        ++g_stLines;
        Log("[scope] trace %d ends (%s): %d draw(s) while it ran, %d sampled the screen copy, %d sampled the blur picture%s", g_stId,
            why, g_stDraws, g_stCopyDraws, g_stPassDraws, g_stCopyTex ? "" : " (no screen copy to a texture seen)");
    }
    g_stOn = false;
}

// ---- 21j-r33: the water scan's device side ([water34] lines; render_hooks.hpp has the plan) -------------------------------
constexpr int kWtTexMax = 48;
constexpr int kWtSlotGetVsConstF = 95;
constexpr int kWtMaxLines = 700;
typedef long(__stdcall* WtGetVsConst_t)(void*, unsigned, float*, unsigned);
struct WtTex
{
    void* tex;
    int view;
    int phase;
    long frame;
};
WtTex g_wtTex[kWtTexMax] = {};
int g_wtTexCount = 0;
int g_wtPhase = 0;          // 0 none, 1 the reflection pass, 2 the water draw, 3 the refraction copy
int g_wtView = -1;
bool g_wtRtDirty = true;    // the render target may have changed since it was last looked at
int g_wtNotedView = -2;     // the view, phase and frame the last noted render target was drawn in
int g_wtNotedPhase = -1;
long g_wtNotedFrame = -1;
bool g_wtDetail[4] = {};    // the next water draw's first call of this view in full
bool g_wtSegFirst = false;  // the open water draw's first call not seen yet
volatile LONG g_wtStale[4][3] = {};
int g_wtLines = 0;
const char* const kWtPhaseName[4] = { "other drawing", "reflection pass", "water draw", "refraction copy" };
const volatile int* g_wtViewPtr = nullptr; // the renderer's current view (set when the scan is on): outside the phases

bool WtLine()
{
    if (g_wtLines >= kWtMaxLines)
        return false;
    if (++g_wtLines == kWtMaxLines)
        Log("[water34] %d device lines logged, going quiet (the counts go on)", kWtMaxLines);
    return true;
}

WtTex* WtFind(void* tex, bool add)
{
    for (int i = 0; i < g_wtTexCount; ++i)
        if (g_wtTex[i].tex == tex)
            return &g_wtTex[i];
    if (!add || g_wtTexCount >= kWtTexMax)
        return nullptr;
    WtTex& t = g_wtTex[g_wtTexCount++];
    t.tex = tex;
    t.view = -1;
    t.phase = 0;
    t.frame = -1;
    return &t;
}

void WtNoteWritten(void* tex)
{
    if (!tex)
        return;
    WtTex* const t = WtFind(tex, true);
    if (!t)
        return;
    t->view = g_wtView;
    t->phase = g_wtPhase;
    t->frame = g_calls[kHPresent];
}

// "written in view 3's reflection pass this frame" (or older / never)
void WtWrittenText(void* tex, char* out, size_t n)
{
    const WtTex* const t = tex ? WtFind(tex, false) : nullptr;
    if (!t || t->frame < 0)
    {
        snprintf(out, n, "never seen drawn into or copied to (not a render target the scan saw)");
        return;
    }
    const long ago = g_calls[kHPresent] - t->frame;
    if (ago == 0)
        snprintf(out, n, "written in view %d's %s THIS frame%s", t->view + 1, kWtPhaseName[t->phase & 3],
                 t->view == g_wtView ? " (its own view)" : " - ANOTHER VIEW'S");
    else
        snprintf(out, n, "written in view %d's %s %ld frame(s) ago - NOT this frame", t->view + 1, kWtPhaseName[t->phase & 3], ago);
}

// ---- 21j-r34 / r35: THE WATER PICTURE SCAN ([water34] shot lines; render_hooks.hpp has the plan) ------------------------
// 21j-r35: not only the water's draws - per view, the first draw into and the first draw sampling each screen texture (any
// code), with the shaders saved (WsDraw below); r34's text follows.
// One whole frame: every render target texture the water's draws sample (the reflection pass's picture, the ripple
// picture of the water's second pass, the refraction copy) saved as a .bmp the first time each view's water samples it -
// what that view's water reads, at the moment it reads it - in BF1GameExt_water\ next to the exe (left: its colour,
// right: its alpha as grey; 1024 wide at most), with that draw's state and shader constants in the log.
constexpr int kWsMaxShots = 4;
constexpr int kWsPerView = 8;
constexpr unsigned kWsMaxSide = 512;   // 21j-r35: 512 wide at most (r34: 1024)
constexpr int kWsMaxLines = 1400;
constexpr int kWsMaxShaders = 48;
constexpr int kWsMaxConstSeen = 64;
constexpr int kWsSlotGetRenderTargetData = 32;
constexpr int kWsSlotCreateOffscreenPlain = 36;
constexpr int kWsSlotGetTransform = 45;
constexpr int kWsSlotGetTextureStageState = 66;
constexpr int kWsSlotGetScissorRect = 76;
constexpr int kWsSlotGetPixelShaderConstantF = 110;
constexpr int kWsSlotGetVertexShader = 93;
constexpr int kWsSlotGetPixelShader = 108;
constexpr int kWsSlotGetRenderState = 58;
typedef long(__stdcall* WsGetSurfaceLevel_t)(void*, unsigned, void**);
typedef long(__stdcall* WsCreateOffscreen_t)(void*, unsigned, unsigned, DWORD, DWORD, void**, void*);
typedef long(__stdcall* WsGetRtData_t)(void*, void*, void*);
typedef long(__stdcall* WsGetTransform_t)(void*, DWORD, float*);
typedef long(__stdcall* WsGetTss_t)(void*, DWORD, DWORD, DWORD*);
typedef long(__stdcall* WsGetScissor_t)(void*, RECT*);
typedef long(__stdcall* WsGetShader_t)(void*, void**);
typedef long(__stdcall* WsGetConst_t)(void*, unsigned, float*, unsigned);
typedef long(__stdcall* WsGetRs_t)(void*, DWORD, DWORD*);
typedef long(__stdcall* WsGetSampler_t)(void*, DWORD, DWORD, DWORD*);
struct WsLocked
{
    int Pitch;
    void* pBits;
};
typedef long(__stdcall* WsLockRect_t)(void*, WsLocked*, const RECT*, DWORD);
typedef long(__stdcall* WsUnlockRect_t)(void*);

int g_wsShots = 0;               // shots taken this session
volatile LONG g_wsArmed = 0;     // the next frame is a shot
bool g_wsOn = false;             // this frame is one
const char* g_wsWhy = "";
void* g_wsSaved[4][kWsPerView] = {}; // the screen textures each view's draws have sampled this shot (saved at the first)
int g_wsSavedCount[4] = {};
void* g_wsInto[4][kWsPerView] = {};  // 21j-r35: the screen textures each view's draws have drawn into this shot
int g_wsIntoCount[4] = {};
struct WsConstSeen
{
    void* vs;
    void* ps;
    int view;
};
WsConstSeen g_wsConstSeen[kWsMaxConstSeen] = {}; // 21j-r35: (view, shaders) whose constants this shot has logged
int g_wsConstSeenCount = 0;
void* g_wsDumped[kWsMaxShaders] = {};             // 21j-r35: shaders saved this session
int g_wsDumpedCount = 0;
typedef long(__stdcall* WsGetFunction_t)(void*, void*, unsigned*);
int g_wsFiles = 0;
int g_wsLines = 0;
long g_wsWaterDraws[4] = {};
char g_wsDir[MAX_PATH] = {};

bool WsLine()
{
    if (g_wsLines >= kWsMaxLines)
        return false;
    ++g_wsLines;
    return true;
}

void WsArm(const char* why)
{
    if (!g_wtViewPtr || g_wsShots >= kWsMaxShots)
        return;
    g_wsWhy = why ? why : "";
    InterlockedExchange(&g_wsArmed, 1);
}

// at each Present: a shot frame ends, an armed one begins
void WsPresent()
{
    if (g_wsOn)
    {
        g_wsOn = false;
        if (WsLine())
            Log("[water34] shot %d ends: %d picture(s) saved in %s; water draws seen per view: %ld / %ld / %ld / %ld", g_wsShots, g_wsFiles,
                g_wsDir[0] ? g_wsDir : "(no folder)", g_wsWaterDraws[0], g_wsWaterDraws[1], g_wsWaterDraws[2], g_wsWaterDraws[3]);
    }
    if (!InterlockedExchange(&g_wsArmed, 0) || g_wsShots >= kWsMaxShots)
        return;
    if (!g_wsDir[0])
    {
        char exe[MAX_PATH] = {};
        if (GetModuleFileNameA(nullptr, exe, MAX_PATH))
        {
            char* slash = strrchr(exe, '\\');
            if (slash)
                *slash = 0;
            snprintf(g_wsDir, sizeof(g_wsDir), "%s\\BF1GameExt_water", exe);
            CreateDirectoryA(g_wsDir, nullptr);
        }
    }
    ++g_wsShots;
    g_wsFiles = 0;
    memset(g_wsSaved, 0, sizeof(g_wsSaved));
    memset(g_wsSavedCount, 0, sizeof(g_wsSavedCount));
    memset(g_wsInto, 0, sizeof(g_wsInto));
    memset(g_wsIntoCount, 0, sizeof(g_wsIntoCount));
    memset(g_wsConstSeen, 0, sizeof(g_wsConstSeen));
    g_wsConstSeenCount = 0;
    memset(g_wsWaterDraws, 0, sizeof(g_wsWaterDraws));
    g_wsOn = true;
    if (WsLine())
        Log("[water34] %lu ms: shot %d of %d begins (%s): per view, the first draw into each screen texture and the first draw "
            "sampling each one (its picture saved then: what that draw reads), with their state, constants and shaders -> %s",
            ScanMsSinceAttach(), g_wsShots, kWsMaxShots, g_wsWhy, g_wsDir[0] ? g_wsDir : "(no folder)");
}

// a 24-bit .bmp: the picture's colour on the left, its alpha (as grey) on the right, every step-th pixel
bool WsWriteBmp(const char* path, const unsigned char* bits, int pitch, unsigned w, unsigned h, unsigned step)
{
    const unsigned ow = (w + step - 1) / step;
    const unsigned oh = (h + step - 1) / step;
    const unsigned rowBytes = (ow * 2 * 3 + 3) & ~3u;
    if (ow == 0 || oh == 0 || rowBytes > 2 * kWsMaxSide * 3 + 8)
        return false;
    const unsigned image = rowBytes * oh;
    unsigned char hdr[54] = {};
    const unsigned fileSize = 54 + image;
    const unsigned owTotal = ow * 2;
    hdr[0] = 'B';
    hdr[1] = 'M';
    memcpy(hdr + 2, &fileSize, 4);
    hdr[10] = 54;
    hdr[14] = 40;
    memcpy(hdr + 18, &owTotal, 4);
    memcpy(hdr + 22, &oh, 4);
    hdr[26] = 1;
    hdr[28] = 24;
    memcpy(hdr + 34, &image, 4);
    HANDLE f = CreateFileA(path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE)
        return false;
    DWORD wrote = 0;
    bool ok = WriteFile(f, hdr, sizeof(hdr), &wrote, nullptr) && wrote == sizeof(hdr);
    static unsigned char row[2 * kWsMaxSide * 3 + 8];
    for (unsigned y = 0; ok && y < oh; ++y)
    {
        memset(row, 0, rowBytes);
        const unsigned sy = (oh - 1 - y) * step; // (a .bmp's rows go from the bottom up)
        const unsigned char* src = bits + static_cast<size_t>(sy) * static_cast<size_t>(pitch);
        for (unsigned x = 0; x < ow; ++x)
        {
            const unsigned char* px = src + static_cast<size_t>(x) * step * 4;
            row[x * 3 + 0] = px[0];
            row[x * 3 + 1] = px[1];
            row[x * 3 + 2] = px[2];
            row[(ow + x) * 3 + 0] = px[3];
            row[(ow + x) * 3 + 1] = px[3];
            row[(ow + x) * 3 + 2] = px[3];
        }
        ok = WriteFile(f, row, rowBytes, &wrote, nullptr) && wrote == rowBytes;
    }
    CloseHandle(f);
    return ok;
}

// a render target texture's level 0 as the GPU has it now -> a .bmp; "" in out if it could not be read
bool WsSaveTexture(void* dev, void* tex, const char* path, char* why, size_t whyLen, unsigned* step)
{
    void** dvt = *reinterpret_cast<void***>(dev);
    void** tvt = *reinterpret_cast<void***>(tex);
    void* surf = nullptr;
    if (reinterpret_cast<WsGetSurfaceLevel_t>(tvt[18])(tex, 0, &surf) < 0 || !surf)
    {
        snprintf(why, whyLen, "its surface could not be had");
        return false;
    }
    void** svt = *reinterpret_cast<void***>(surf);
    SurfaceDesc9 d;
    memset(&d, 0, sizeof(d));
    reinterpret_cast<GetDesc_t>(svt[12])(surf, &d);
    bool ok = false;
    if (d.Format != 21 && d.Format != 22) // A8R8G8B8 / X8R8G8B8
        snprintf(why, whyLen, "format %lu (only A8R8G8B8 / X8R8G8B8 are saved)", static_cast<unsigned long>(d.Format));
    else if (d.MultiSampleType)
        snprintf(why, whyLen, "multisampled");
    else
    {
        void* sys = nullptr;
        long hr = reinterpret_cast<WsCreateOffscreen_t>(dvt[kWsSlotCreateOffscreenPlain])(dev, d.Width, d.Height, d.Format, 2, &sys, nullptr);
        if (hr < 0 || !sys)
            snprintf(why, whyLen, "no system memory copy (0x%08lX)", static_cast<unsigned long>(hr));
        else
        {
            void** yvt = *reinterpret_cast<void***>(sys);
            hr = reinterpret_cast<WsGetRtData_t>(dvt[kWsSlotGetRenderTargetData])(dev, surf, sys);
            WsLocked lr = {};
            if (hr < 0)
                snprintf(why, whyLen, "GetRenderTargetData failed (0x%08lX)", static_cast<unsigned long>(hr));
            else if (reinterpret_cast<WsLockRect_t>(yvt[13])(sys, &lr, nullptr, 0x10) < 0 || !lr.pBits)
                snprintf(why, whyLen, "its copy could not be locked");
            else
            {
                const unsigned big = d.Width > d.Height ? d.Width : d.Height;
                *step = (big + kWsMaxSide - 1) / kWsMaxSide;
                if (*step < 1)
                    *step = 1;
                ok = WsWriteBmp(path, reinterpret_cast<const unsigned char*>(lr.pBits), lr.Pitch, d.Width, d.Height, *step);
                if (!ok)
                    snprintf(why, whyLen, "the file could not be written");
                reinterpret_cast<WsUnlockRect_t>(yvt[14])(sys);
            }
            reinterpret_cast<Release_t>(yvt[2])(sys);
        }
    }
    reinterpret_cast<Release_t>(svt[2])(surf);
    return ok;
}

// 21j-r35: a shader's bytecode (IDirect3DVertexShader9 / IDirect3DPixelShader9 GetFunction, vtable +4) saved once a session as
// shader_vs_<address>.bin / shader_ps_<address>.bin - its own instructions, to read how it makes its texture coordinates
void WsDumpShader(void* sh, const char* kind)
{
    if (!sh || !g_wsDir[0])
        return;
    for (int i = 0; i < g_wsDumpedCount; ++i)
        if (g_wsDumped[i] == sh)
            return;
    if (g_wsDumpedCount >= kWsMaxShaders)
        return;
    g_wsDumped[g_wsDumpedCount++] = sh;
    void** svt = *reinterpret_cast<void***>(sh);
    unsigned size = 0;
    if (reinterpret_cast<WsGetFunction_t>(svt[4])(sh, nullptr, &size) < 0 || size < 4 || size > 0x40000)
    {
        if (WsLine())
            Log("[water34]   %s shader 0x%08X: its code could not be had (size %u)", kind, static_cast<unsigned>(reinterpret_cast<uintptr_t>(sh)),
                size);
        return;
    }
    unsigned char* const buf = static_cast<unsigned char*>(HeapAlloc(GetProcessHeap(), 0, size));
    if (!buf)
        return;
    bool ok = reinterpret_cast<WsGetFunction_t>(svt[4])(sh, buf, &size) >= 0;
    char path[MAX_PATH + 64];
    snprintf(path, sizeof(path), "%s\\shader_%s_%08X.bin", g_wsDir, kind, static_cast<unsigned>(reinterpret_cast<uintptr_t>(sh)));
    uint32_t version = 0;
    memcpy(&version, buf, 4);
    if (ok)
    {
        HANDLE f = CreateFileA(path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        DWORD wrote = 0;
        ok = f != INVALID_HANDLE_VALUE && WriteFile(f, buf, size, &wrote, nullptr) && wrote == size;
        if (f != INVALID_HANDLE_VALUE)
            CloseHandle(f);
    }
    HeapFree(GetProcessHeap(), 0, buf);
    if (ok)
        ++g_wsFiles;
    if (WsLine())
        Log("[water34]   %s shader 0x%08X: %u bytes (version 0x%08X) %s%s", kind, static_cast<unsigned>(reinterpret_cast<uintptr_t>(sh)), size,
            version, ok ? "saved as " : "NOT saved", ok ? strrchr(path, '\\') + 1 : "");
}

// the screen texture index of a texture (g_screenTex: render targets and copy targets), -1 if not one
int WsScreenId(void* t)
{
    for (int i = 0; t && i < g_screenTexCount; ++i)
        if (g_screenTex[i].tex == t)
            return i;
    return -1;
}

// true the first time t is put in this view's list (false if it was there, or the list is full)
bool WsFirst(void* (&list)[kWsPerView], int* count, void* t)
{
    for (int i = 0; i < *count; ++i)
        if (list[i] == t)
            return false;
    if (*count >= kWsPerView)
        return false;
    list[(*count)++] = t;
    return true;
}

// a draw's state: viewport, render target, every bound stage (its texture, coordinates and addressing), the shaders (saved),
// scissor; the vertex shader's c0 - c95 and the pixel shader's c0 - c7 (once a shot for each view's pair of shaders)
void WsDrawState(void* dev, int v, int stage)
{
    void** vt = *reinterpret_cast<void***>(dev);
    char st[200];
    StState(dev, st, sizeof(st), nullptr);
    char stages[600];
    size_t at = 0;
    stages[0] = 0;
    for (DWORD s = 0; s < 8 && at < sizeof(stages) - 96; ++s)
    {
        void* t = nullptr;
        if (reinterpret_cast<StGetTexture_t>(vt[kStSlotGetTexture])(dev, s, &t) < 0 || !t)
            continue;
        reinterpret_cast<Release_t>((*reinterpret_cast<void***>(t))[2])(t);
        DWORD au = 0, av = 0, tci = 0, ttf = 0;
        reinterpret_cast<WsGetSampler_t>(vt[kStSlotGetSampler])(dev, s, 1, &au);
        reinterpret_cast<WsGetSampler_t>(vt[kStSlotGetSampler])(dev, s, 2, &av);
        reinterpret_cast<WsGetTss_t>(vt[kWsSlotGetTextureStageState])(dev, s, 11, &tci);
        reinterpret_cast<WsGetTss_t>(vt[kWsSlotGetTextureStageState])(dev, s, 24, &ttf);
        const int id = WsScreenId(t);
        char name[24];
        if (id >= 0)
            snprintf(name, sizeof(name), "#%d", id + 1);
        else
            snprintf(name, sizeof(name), "0x%08X", static_cast<unsigned>(reinterpret_cast<uintptr_t>(t)));
        const int w = snprintf(stages + at, sizeof(stages) - at, "%s%lu %s (coords 0x%lX, flags %lu, address %lu/%lu)", at ? "; " : "",
                               static_cast<unsigned long>(s), name, static_cast<unsigned long>(tci), static_cast<unsigned long>(ttf),
                               static_cast<unsigned long>(au), static_cast<unsigned long>(av));
        if (w > 0)
            at += static_cast<size_t>(w);
    }
    DWORD sc = 0;
    reinterpret_cast<WsGetRs_t>(vt[kWsSlotGetRenderState])(dev, 174, &sc);
    RECT sr = {};
    reinterpret_cast<WsGetScissor_t>(vt[kWsSlotGetScissorRect])(dev, &sr);
    void* vs = nullptr;
    void* ps = nullptr;
    reinterpret_cast<WsGetShader_t>(vt[kWsSlotGetVertexShader])(dev, &vs);
    reinterpret_cast<WsGetShader_t>(vt[kWsSlotGetPixelShader])(dev, &ps);
    if (vs)
        reinterpret_cast<Release_t>((*reinterpret_cast<void***>(vs))[2])(vs); // (bound: the device keeps it)
    if (ps)
        reinterpret_cast<Release_t>((*reinterpret_cast<void***>(ps))[2])(ps);
    if (WsLine())
        Log("[water34]   view %d's draw: %s; stages: %s; vertex shader 0x%08X, pixel shader 0x%08X; scissor %s (%ld,%ld)-(%ld,%ld)", v + 1, st,
            stages[0] ? stages : "none", static_cast<unsigned>(reinterpret_cast<uintptr_t>(vs)), static_cast<unsigned>(reinterpret_cast<uintptr_t>(ps)),
            sc ? "ON" : "off", static_cast<long>(sr.left), static_cast<long>(sr.top), static_cast<long>(sr.right), static_cast<long>(sr.bottom));
    WsDumpShader(vs, "vs");
    WsDumpShader(ps, "ps");
    if (!vs && stage >= 0)
    {
        float m[16] = {};
        if (reinterpret_cast<WsGetTransform_t>(vt[kWsSlotGetTransform])(dev, 16 + static_cast<DWORD>(stage), m) >= 0 && WsLine())
            Log("[water34]   view %d's stage %d texture transform (no vertex shader): %.4g %.4g %.4g %.4g | %.4g %.4g %.4g %.4g | %.4g %.4g %.4g "
                "%.4g | %.4g %.4g %.4g %.4g",
                v + 1, stage, static_cast<double>(m[0]), static_cast<double>(m[1]), static_cast<double>(m[2]), static_cast<double>(m[3]),
                static_cast<double>(m[4]), static_cast<double>(m[5]), static_cast<double>(m[6]), static_cast<double>(m[7]),
                static_cast<double>(m[8]), static_cast<double>(m[9]), static_cast<double>(m[10]), static_cast<double>(m[11]),
                static_cast<double>(m[12]), static_cast<double>(m[13]), static_cast<double>(m[14]), static_cast<double>(m[15]));
    }
    for (int i = 0; i < g_wsConstSeenCount; ++i)
    {
        if (g_wsConstSeen[i].vs == vs && g_wsConstSeen[i].ps == ps && g_wsConstSeen[i].view == v)
        {
            if (WsLine())
                Log("[water34]   view %d: (these shaders' constants: as logged above for view %d)", v + 1, v + 1);
            return;
        }
    }
    if (g_wsConstSeenCount < kWsMaxConstSeen)
        g_wsConstSeen[g_wsConstSeenCount++] = { vs, ps, v };
    for (int kind = 0; kind < 2; ++kind)
    {
        float c[96][4] = {};
        const unsigned regs = kind == 0 ? 96u : 8u;
        const int slot = kind == 0 ? kWtSlotGetVsConstF : kWsSlotGetPixelShaderConstantF;
        if (reinterpret_cast<WsGetConst_t>(vt[slot])(dev, 0, &c[0][0], regs) < 0)
            continue;
        for (unsigned row = 0; row < regs; row += 8)
        {
            if (!WsLine())
                return;
            char line[520];
            size_t pos = 0;
            line[0] = 0;
            for (unsigned r = row; r < row + 8 && pos < sizeof(line) - 64; ++r)
            {
                const int w = snprintf(line + pos, sizeof(line) - pos, "%sc%u %.4g %.4g %.4g %.4g", r == row ? "" : " | ", r,
                                       static_cast<double>(c[r][0]), static_cast<double>(c[r][1]), static_cast<double>(c[r][2]),
                                       static_cast<double>(c[r][3]));
                if (w > 0)
                    pos += static_cast<size_t>(w);
            }
            Log("[water34]   view %d %s %s", v + 1, kind == 0 ? "vs" : "ps", line);
        }
    }
}

// 21j-r35: each draw while a shot runs (r34 picked the water's draws by their caller, and that missed the water's second pass):
// per view, the first draw INTO each screen texture (logged) and the first draw SAMPLING each one (its picture saved then)
void WsDraw(void* dev)
{
    if (!g_wsOn || !g_wtViewPtr)
        return;
    const int v = g_wtPhase ? g_wtView : *g_wtViewPtr;
    if (v < 0 || v > 3)
        return;
    if (g_wtPhase == 2)
        ++g_wsWaterDraws[v];
    void** vt = *reinterpret_cast<void***>(dev);
    const char* const what = kWtPhaseName[g_wtPhase & 3];
    void* rt = nullptr;
    if (reinterpret_cast<GetRenderTarget_t>(vt[kSlotGetRenderTarget])(dev, 0, &rt) >= 0 && rt)
    {
        void* const rtex = TextureOf(rt);
        reinterpret_cast<Release_t>((*reinterpret_cast<void***>(rt))[2])(rt); // (the device keeps its own reference)
        const int id = WsScreenId(rtex);
        if (id >= 0 && WsFirst(g_wsInto[v], &g_wsIntoCount[v], rtex))
        {
            uint32_t c[2] = {};
            const int n = GameCallers(c, 2);
            if (WsLine())
                Log("[water34] shot %d, view %d (%s): the first draw INTO screen texture #%d (%ux%u) (by rva 0x%08X < 0x%08X)", g_wsShots, v + 1,
                    what, id + 1, g_screenTex[id].w, g_screenTex[id].h, n >= 1 ? c[0] : 0u, n >= 2 ? c[1] : 0u);
            WsDrawState(dev, v, -1);
        }
    }
    for (DWORD s = 0; s < 8; ++s)
    {
        void* t = nullptr;
        if (reinterpret_cast<StGetTexture_t>(vt[kStSlotGetTexture])(dev, s, &t) < 0 || !t)
            continue;
        reinterpret_cast<Release_t>((*reinterpret_cast<void***>(t))[2])(t); // (bound: the device keeps it)
        const int id = WsScreenId(t);
        if (id < 0 || !WsFirst(g_wsSaved[v], &g_wsSavedCount[v], t))
            continue;
        char path[MAX_PATH + 80];
        snprintf(path, sizeof(path), "%s\\shot%d_view%d_tex%d_%ux%u.bmp", g_wsDir, g_wsShots, v + 1, id + 1, g_screenTex[id].w,
                 g_screenTex[id].h);
        char why[96] = "";
        unsigned step = 1;
        const bool saved = g_wsDir[0] && WsSaveTexture(dev, t, path, why, sizeof(why), &step);
        if (saved)
            ++g_wsFiles;
        char wr[120];
        WtWrittenText(t, wr, sizeof(wr));
        uint32_t c[2] = {};
        const int n = GameCallers(c, 2);
        if (WsLine())
            Log("[water34] shot %d, view %d (%s): the first draw SAMPLING screen texture #%d (%ux%u), stage %lu - %s - %s%s%s (by rva 0x%08X < 0x%08X)",
                g_wsShots, v + 1, what, id + 1, g_screenTex[id].w, g_screenTex[id].h, static_cast<unsigned long>(s), wr,
                saved ? "saved as " : "NOT saved: ", saved ? strrchr(path, '\\') + 1 : why, saved ? (step > 1 ? " (shrunk)" : "") : "",
                n >= 1 ? c[0] : 0u, n >= 2 ? c[1] : 0u);
        WsDrawState(dev, v, static_cast<int>(s));
    }
}

void WtCopy(void* dst)
{
    if (!g_wtViewPtr || g_wtPhase == 2)
        return;
    if (!g_wtPhase)
        g_wtView = *g_wtViewPtr;
    WtNoteWritten(TextureOf(dst));
}

void WtDraw(void* dev)
{
    if (!g_wtViewPtr)
        return;
    WsDraw(dev); // 21j-r34: the water picture scan (only while a shot runs)
    void** vt = *reinterpret_cast<void***>(dev);
    if (g_wtPhase != 2)
    {
        if (!g_wtPhase) // outside the phases: the view whose turn it is (the renderer's current view)
            g_wtView = *g_wtViewPtr;
        const long frame = g_calls[kHPresent];
        if (!g_wtRtDirty && g_wtView == g_wtNotedView && g_wtPhase == g_wtNotedPhase && frame == g_wtNotedFrame)
            return; // (the same render target, view, phase and frame as the last draw noted)
        g_wtRtDirty = false;
        g_wtNotedView = g_wtView;
        g_wtNotedPhase = g_wtPhase;
        g_wtNotedFrame = frame;
        void* rt = nullptr;
        if (reinterpret_cast<GetRenderTarget_t>(vt[kSlotGetRenderTarget])(dev, 0, &rt) >= 0 && rt)
        {
            WtNoteWritten(TextureOf(rt));
            reinterpret_cast<Release_t>((*reinterpret_cast<void***>(rt))[2])(rt); // (the device keeps its own reference)
        }
        return;
    }
    // the water draw: its textures against what the reflection passes / copies wrote
    const long frame = g_calls[kHPresent];
    void* tex[8] = {};
    int other = 0;
    int old = 0;
    int own = 0;
    for (DWORD s = 0; s < 8; ++s)
    {
        void* t = nullptr;
        if (reinterpret_cast<StGetTexture_t>(vt[kStSlotGetTexture])(dev, s, &t) >= 0 && t)
        {
            tex[s] = t;
            reinterpret_cast<Release_t>((*reinterpret_cast<void***>(t))[2])(t); // (bound: the device keeps it)
            const WtTex* const w = WtFind(t, false);
            if (w && w->frame >= 0)
            {
                if (w->frame != frame)
                    ++old;
                else if (w->view == g_wtView)
                    ++own;
                else
                    ++other;
            }
        }
    }
    const int v = g_wtView;
    if (v >= 0 && v < 4)
    {
        if (other)
            InterlockedIncrement(&g_wtStale[v][0]);
        else if (old)
            InterlockedIncrement(&g_wtStale[v][1]);
        if (own)
            InterlockedIncrement(&g_wtStale[v][2]);
    }
    const bool first = g_wtSegFirst;
    g_wtSegFirst = false;
    if (!first || v < 0 || v > 3 || !g_wtDetail[v])
        return;
    g_wtDetail[v] = false;
    if (!WtLine())
        return;
    char st[200];
    StState(dev, st, sizeof(st), nullptr);
    Log("[water34] view %d's water draw, frame %ld - its first call: %s", v + 1, frame, st);
    for (int s = 0; s < 8; ++s)
    {
        if (!tex[s] || !WtLine())
            continue;
        char tt[80];
        char wr[120];
        StTexText(tex[s], tt, sizeof(tt));
        WtWrittenText(tex[s], wr, sizeof(wr));
        Log("[water34]   view %d stage %d: %s - %s", v + 1, s, tt, wr);
    }
    float c[32][4] = {};
    if (reinterpret_cast<WtGetVsConst_t>(vt[kWtSlotGetVsConstF])(dev, 0, &c[0][0], 32) >= 0)
    {
        for (int row = 0; row < 32; row += 8)
        {
            if (!WtLine())
                break;
            char line[520];
            size_t at = 0;
            line[0] = 0;
            for (int r = row; r < row + 8 && at < sizeof(line) - 64; ++r)
            {
                const int w = snprintf(line + at, sizeof(line) - at, "%sc%d %.4g %.4g %.4g %.4g", r == row ? "" : " | ", r,
                                       static_cast<double>(c[r][0]), static_cast<double>(c[r][1]),
                                       static_cast<double>(c[r][2]), static_cast<double>(c[r][3]));
                if (w > 0)
                    at += static_cast<size_t>(w);
            }
            Log("[water34]   view %d vs %s", v + 1, line);
        }
    }
}

// ---- 21j-r11: THE FRAME-RATE SCAN ([Display] PerfScan; [perf] lines; see render_hooks.hpp) -------------------------------
// The game's thread (the one that calls Present) keeps a stack of parts; every tick of the time stamp counter between two
// marks goes to the part on top (kPerfGame at the bottom), so a frame's parts add up to the frame. Only that thread touches
// these, so nothing here is locked. A frame ends right after the real Present returns (PerfFrameEnd).
bool g_perfWanted = true;       // [Display] PerfScan
DWORD g_perfSpikeMs = 25;       // [Display] PerfSpikeMs: a SLOW FRAME line for each frame longer than this
volatile bool g_perfOn = false; // set on the game's thread at its first Present
DWORD g_perfThread = 0;
constexpr int kPerfDepth = 24;
int g_pDepth = 0;
uint8_t g_pStack[kPerfDepth] = {};
uint64_t g_pMark = 0;
uint64_t g_pAcc[kPerfSlots] = {};
uint32_t g_pCalls[kPerfSlots] = {};
uint32_t g_pPairs = 0;          // parts entered this frame (the scan's own cost)
uint64_t g_pFrameStart = 0;     // the counter when the last Present returned
ULONG64 g_pCycLast = 0;         // the thread's CPU cycles then
uint64_t g_pPairTicks = 0;      // counter ticks one enter + leave costs (measured at the start)
double g_tscPerMs = 0.0;        // from the last window (0 until the first 250 ms are measured)
uint64_t g_calTsc = 0;
LARGE_INTEGER g_calQpc = {};
LARGE_INTEGER g_qpcFreq = {};
int g_perfSpikeLines = 0;
constexpr int kPerfSpikeMax = 400;  // SLOW FRAME lines in all
constexpr int kPerfSpikeWin = 6;    // ... in one 5-s window
constexpr DWORD kPerfWinMs = 5000;
bool g_perfPresentInRenderLogged = false;
volatile LONG g_perfOther = 0;      // Presents from another thread (this window)
DWORD g_perfLastOwn = 0;            // when the game's thread last presented
int g_perfSwitches = 0;

const char* const kPerfNames[kPerfSlots] = {
    "update and the rest", "render", "soldier poses", "HUD layers", "menu layer", "draws", "targets / viewports / clears / copies",
    "Present", "players 3 / 4's frame work", "player 2's input", "spawn screen layer", "class chooser", "players 3 / 4's menus",
    "ragdolls", "Direct3D wrappers", "the log",
};

struct PerfWin
{
    DWORD startTick;
    LARGE_INTEGER startQpc;
    uint64_t startTsc;
    uint32_t frames;
    uint64_t wallSum, wallMax, workSum, workMax, oursSum, oursMax;
    uint64_t sum[kPerfSlots];
    uint64_t max[kPerfSlots];
    uint64_t calls[kPerfSlots];
    uint32_t over[4];               // frames longer than 17.5 / 20 / 25 / 33.4 ms
    uint32_t slow;                  // frames whose work (outside Present) went over 16.7 ms
    uint64_t slowSum[kPerfSlots];
    uint64_t slowCalls[kPerfSlots];
    uint64_t slowWork, slowOurs;
    ULONG64 cpuOut, cpuIn;          // the thread's CPU cycles outside / inside Present
    uint64_t tickOut, tickIn;
    uint64_t pairs;
    uint32_t playersMin, playersMax;
    uint32_t captures;              // frames with a frame capture in them
    int spikes;
};
PerfWin g_pw = {};

#if defined(_MSC_VER) && !defined(__clang__)
inline uint64_t PerfTsc() { return __rdtsc(); }
#else
inline uint64_t PerfTsc() { return __builtin_ia32_rdtsc(); }
#endif

inline bool PerfEnterI(int slot)
{
    if (!g_perfOn || GetCurrentThreadId() != g_perfThread || g_pDepth >= kPerfDepth - 1 || slot < 0 || slot >= kPerfSlots)
        return false;
    const uint64_t t = PerfTsc();
    g_pAcc[g_pStack[g_pDepth]] += t - g_pMark;
    g_pMark = t;
    g_pStack[++g_pDepth] = static_cast<uint8_t>(slot);
    ++g_pCalls[slot];
    ++g_pPairs;
    return true;
}

inline void PerfLeaveI(bool entered)
{
    if (!entered || g_pDepth <= 0 || GetCurrentThreadId() != g_perfThread)
        return;
    const uint64_t t = PerfTsc();
    g_pAcc[g_pStack[g_pDepth]] += t - g_pMark;
    g_pMark = t;
    --g_pDepth;
}

// a part for the length of a scope (this file's wrappers)
struct PerfPart
{
    bool on;
    explicit PerfPart(int slot) : on(PerfEnterI(slot)) {}
    ~PerfPart() { PerfLeaveI(on); }
    PerfPart(const PerfPart&) = delete;
    PerfPart& operator=(const PerfPart&) = delete;
};

bool PerfOnStack(int slot)
{
    for (int i = 1; i <= g_pDepth; ++i)
        if (g_pStack[i] == slot)
            return true;
    return false;
}

void PerfWinReset(uint64_t tsc)
{
    memset(&g_pw, 0, sizeof(g_pw));
    g_pw.startTick = GetTickCount();
    QueryPerformanceCounter(&g_pw.startQpc);
    g_pw.startTsc = tsc;
    g_pw.playersMin = 99;
}

// The frame's state from scratch on this thread (the scan's start, or the Presents moved to another thread).
void PerfThreadReset()
{
    memset(g_pAcc, 0, sizeof(g_pAcc));
    memset(g_pCalls, 0, sizeof(g_pCalls));
    g_pPairs = 0;
    g_pDepth = 0;
    g_pStack[0] = kPerfGame;
    g_calTsc = PerfTsc();
    QueryPerformanceCounter(&g_calQpc);
    g_pMark = g_calTsc;
    g_pFrameStart = g_calTsc;
    g_perfLastOwn = GetTickCount();
    QueryThreadCycleTime(GetCurrentThread(), &g_pCycLast);
    PerfWinReset(g_calTsc);
}

// A Present on another thread while the game's thread has not presented for 2 s (a loading screen drawn by another
// thread, say): that thread is the one that presents now.
void PerfSwitch()
{
    const DWORD was = g_perfThread;
    g_perfThread = GetCurrentThreadId();
    PerfThreadReset();
    if (++g_perfSwitches <= 40)
        Log("[perf] %lu ms: the Presents come from thread %lu now (thread %lu has not presented for 2 s) - the scan follows it%s",
            ScanMsSinceAttach(), static_cast<unsigned long>(g_perfThread), static_cast<unsigned long>(was),
            g_perfSwitches == 40 ? " (the last such line)" : "");
}

// The first Present on the game's thread: the scan starts here (its own cost measured first).
void PerfStart()
{
    g_perfThread = GetCurrentThreadId();
    QueryPerformanceFrequency(&g_qpcFreq);
    // the scan's own cost: one enter + leave, measured on a scratch stack
    g_perfOn = true;
    g_pDepth = 0;
    g_pStack[0] = kPerfGame;
    g_pMark = PerfTsc();
    const uint64_t t0 = PerfTsc();
    for (int i = 0; i < 2048; ++i)
    {
        const bool e = PerfEnterI(kPerfOurD3d);
        PerfLeaveI(e);
    }
    const uint64_t t1 = PerfTsc();
    g_pPairTicks = (t1 - t0) / 2048;
    PerfThreadReset();
    Log("[perf] %lu ms: 21j-r11 the frame-rate scan is ON on the game's thread (thread %lu): every 5 s [perf] lines - the frame "
        "rate, each frame's time and where it went (the game, Direct3D, BF1GameExt's parts); a SLOW FRAME line for each frame "
        "longer than %lu ms (PerfSpikeMs). One enter + leave costs %llu counter ticks. Log only.",
        ScanMsSinceAttach(), static_cast<unsigned long>(g_perfThread), static_cast<unsigned long>(g_perfSpikeMs),
        static_cast<unsigned long long>(g_pPairTicks));
}

double PerfMs(uint64_t ticks)
{
    return g_tscPerMs > 0.0 ? static_cast<double>(ticks) / g_tscPerMs : 0.0;
}

// "1.23 (4.5)": a part's average per frame and its longest
void PerfAvgMax(char* out, size_t size, uint64_t sum, uint64_t max, uint32_t frames)
{
    snprintf(out, size, "%.2f (%.2f)", frames ? PerfMs(sum) / frames : 0.0, PerfMs(max));
}

void PerfWinLog(uint64_t now)
{
    PerfWin& w = g_pw;
    LARGE_INTEGER q;
    QueryPerformanceCounter(&q);
    const double secs = g_qpcFreq.QuadPart ? static_cast<double>(q.QuadPart - w.startQpc.QuadPart) / static_cast<double>(g_qpcFreq.QuadPart) : 0.0;
    if (secs > 0.5 && now > w.startTsc)
        g_tscPerMs = static_cast<double>(now - w.startTsc) / (secs * 1000.0); // the counter's rate, from this window
    const uint32_t n = w.frames;
    if (!n)
        return;
    char g[kPerfSlots][32];
    for (int i = 0; i < kPerfSlots; ++i)
        PerfAvgMax(g[i], sizeof(g[i]), w.sum[i], w.max[i], n);
    const double fn = static_cast<double>(n);
    const LONG other = InterlockedExchange(&g_perfOther, 0);
    char otherText[64] = "";
    if (other)
        snprintf(otherText, sizeof(otherText), " (and %ld Present(s) from another thread)", static_cast<long>(other));
    char players[32];
    if (w.playersMin == w.playersMax)
        snprintf(players, sizeof(players), "%lu", static_cast<unsigned long>(w.playersMax));
    else
        snprintf(players, sizeof(players), "%lu - %lu", static_cast<unsigned long>(w.playersMin), static_cast<unsigned long>(w.playersMax));
    Log("[perf] %lu ms, last %.1f s: %s player(s), %lu frames (%.1f fps)%s%s; a frame %.2f ms on average (the longest %.1f); "
        "longer than 17.5 ms: %lu, 20 ms: %lu, 25 ms: %lu, 33.4 ms: %lu",
        ScanMsSinceAttach(), secs, players, static_cast<unsigned long>(n), secs > 0.0 ? fn / secs : 0.0, otherText, w.captures ? " (a frame capture in it)" : "",
        PerfMs(w.wallSum) / fn, PerfMs(w.wallMax), static_cast<unsigned long>(w.over[0]), static_cast<unsigned long>(w.over[1]),
        static_cast<unsigned long>(w.over[2]), static_cast<unsigned long>(w.over[3]));
    Log("[perf]   work (the frame outside Present) %.2f ms on average (the longest %.1f), over 16.7 ms (the 60 Hz budget) in %lu "
        "frame(s); waiting in Present %.2f ms on average; the game's thread on the CPU %.0f%% of its work time, %.0f%% of its "
        "time in Present (thread cycles against the counter)",
        PerfMs(w.workSum) / fn, PerfMs(w.workMax), static_cast<unsigned long>(w.slow), PerfMs(w.sum[kPerfPresentWait]) / fn,
        w.tickOut ? 100.0 * static_cast<double>(w.cpuOut) / static_cast<double>(w.tickOut) : 0.0,
        w.tickIn ? 100.0 * static_cast<double>(w.cpuIn) / static_cast<double>(w.tickIn) : 0.0);
    Log("[perf]   the game per frame, ms on average (the longest): %s %s, %s %s, %s %s [%.1f], %s %s, %s %s; Direct3D: %s %s "
        "[%.0f], %s %s [%.0f], %s %s",
        kPerfNames[kPerfGame], g[kPerfGame], kPerfNames[kPerfRender], g[kPerfRender], kPerfNames[kPerfPoses], g[kPerfPoses],
        static_cast<double>(w.calls[kPerfPoses]) / fn, kPerfNames[kPerfHudDraw], g[kPerfHudDraw], kPerfNames[kPerfMenuDraw],
        g[kPerfMenuDraw], kPerfNames[kPerfD3dDraw], g[kPerfD3dDraw], static_cast<double>(w.calls[kPerfD3dDraw]) / fn,
        kPerfNames[kPerfD3dOther], g[kPerfD3dOther], static_cast<double>(w.calls[kPerfD3dOther]) / fn,
        kPerfNames[kPerfPresentWait], g[kPerfPresentWait]);
    Log("[perf]   BF1GameExt per frame %.2f ms on average (the longest %.1f): %s %s, %s %s, %s %s, %s %s, %s %s, %s %s, %s %s, "
        "%s %s [%.1f lines]; the scan itself about %.2f ms",
        PerfMs(w.oursSum) / fn, PerfMs(w.oursMax), kPerfNames[kPerfOurFrame], g[kPerfOurFrame], kPerfNames[kPerfOurInput],
        g[kPerfOurInput], kPerfNames[kPerfOurMenu], g[kPerfOurMenu], kPerfNames[kPerfOurChooser], g[kPerfOurChooser],
        kPerfNames[kPerfOurMenus34], g[kPerfOurMenus34], kPerfNames[kPerfOurRagdoll], g[kPerfOurRagdoll],
        kPerfNames[kPerfOurD3d], g[kPerfOurD3d], kPerfNames[kPerfLog], g[kPerfLog],
        static_cast<double>(w.calls[kPerfLog]) / fn, PerfMs(w.pairs * g_pPairTicks) / fn);
    if (w.slow)
    {
        const double sn = static_cast<double>(w.slow);
        char line[700];
        int pos = snprintf(line, sizeof(line), "[perf]   the %lu frame(s) over 16.7 ms of work, ms on average: work %.2f, BF1GameExt %.2f;",
                           static_cast<unsigned long>(w.slow), PerfMs(w.slowWork) / sn, PerfMs(w.slowOurs) / sn);
        for (int i = 0; i < kPerfSlots && pos > 0 && pos < static_cast<int>(sizeof(line)) - 80; ++i)
        {
            if (i == kPerfPresentWait)
                continue;
            pos += snprintf(line + pos, sizeof(line) - static_cast<size_t>(pos), " %s %.2f", kPerfNames[i], PerfMs(w.slowSum[i]) / sn);
            if ((i == kPerfPoses || i == kPerfD3dDraw) && pos > 0 && pos < static_cast<int>(sizeof(line)) - 24)
                pos += snprintf(line + pos, sizeof(line) - static_cast<size_t>(pos), " [%.1f]",
                                static_cast<double>(w.slowCalls[i]) / sn);
            if (i + 1 < kPerfSlots && pos > 0 && pos < static_cast<int>(sizeof(line)) - 2)
                pos += snprintf(line + pos, sizeof(line) - static_cast<size_t>(pos), ",");
        }
        Log("%s", line);
    }
}

// Right after the real Present returned (the game's thread, inside Hook_Present): the frame since the last one is done.
void PerfFrameEnd(ULONG64 cycBefore, bool captured)
{
    const uint64_t now = PerfTsc();
    g_pAcc[g_pStack[g_pDepth]] += now - g_pMark;
    g_pMark = now;
    ULONG64 cycAfter = 0;
    QueryThreadCycleTime(GetCurrentThread(), &cycAfter);
    const uint64_t wall = now - g_pFrameStart;
    g_pFrameStart = now;
    g_perfLastOwn = GetTickCount();
    if (g_tscPerMs <= 0.0 && g_qpcFreq.QuadPart)
    {
        // the first 250 ms: the counter's rate, then the first window starts
        LARGE_INTEGER q;
        QueryPerformanceCounter(&q);
        const double secs = static_cast<double>(q.QuadPart - g_calQpc.QuadPart) / static_cast<double>(g_qpcFreq.QuadPart);
        if (secs >= 0.25 && now > g_calTsc)
        {
            g_tscPerMs = static_cast<double>(now - g_calTsc) / (secs * 1000.0);
            PerfWinReset(now);
        }
    }
    else if (g_tscPerMs > 0.0)
    {
        PerfWin& w = g_pw;
        const uint64_t present = g_pAcc[kPerfPresentWait];
        const uint64_t work = wall > present ? wall - present : 0;
        uint64_t ours = 0;
        for (int i = kPerfOurFrame; i < kPerfSlots; ++i)
            ours += g_pAcc[i];
        ++w.frames;
        w.wallSum += wall;
        w.workSum += work;
        w.oursSum += ours;
        if (wall > w.wallMax)
            w.wallMax = wall;
        if (work > w.workMax)
            w.workMax = work;
        if (ours > w.oursMax)
            w.oursMax = ours;
        for (int i = 0; i < kPerfSlots; ++i)
        {
            w.sum[i] += g_pAcc[i];
            w.calls[i] += g_pCalls[i];
            if (g_pAcc[i] > w.max[i])
                w.max[i] = g_pAcc[i];
        }
        const double r = g_tscPerMs;
        const double bins[4] = { 17.5, 20.0, 25.0, 33.4 };
        for (int b = 0; b < 4; ++b)
            if (static_cast<double>(wall) > bins[b] * r)
                ++w.over[b];
        if (static_cast<double>(work) > 16.7 * r)
        {
            ++w.slow;
            w.slowWork += work;
            w.slowOurs += ours;
            for (int i = 0; i < kPerfSlots; ++i)
            {
                w.slowSum[i] += g_pAcc[i];
                w.slowCalls[i] += g_pCalls[i];
            }
        }
        w.cpuOut += cycBefore > g_pCycLast ? cycBefore - g_pCycLast : 0;
        w.cpuIn += cycAfter > cycBefore ? cycAfter - cycBefore : 0;
        w.tickOut += work;
        w.tickIn += present;
        w.pairs += g_pPairs;
        const uint32_t players = LocalPlayersNow();
        if (players < w.playersMin)
            w.playersMin = players;
        if (players > w.playersMax)
            w.playersMax = players;
        if (captured)
            ++w.captures;
        // a SLOW FRAME line (logged below, after the frame's numbers are taken: its own time goes to the next frame)
        const bool spike = static_cast<double>(wall) > static_cast<double>(g_perfSpikeMs) * r && w.spikes < kPerfSpikeWin &&
                           g_perfSpikeLines < kPerfSpikeMax;
        uint64_t acc[kPerfSlots];
        uint32_t calls[kPerfSlots];
        if (spike)
        {
            memcpy(acc, g_pAcc, sizeof(acc));
            memcpy(calls, g_pCalls, sizeof(calls));
        }
        memset(g_pAcc, 0, sizeof(g_pAcc));
        memset(g_pCalls, 0, sizeof(g_pCalls));
        g_pPairs = 0;
        g_pCycLast = cycAfter;
        if (spike)
        {
            ++w.spikes;
            ++g_perfSpikeLines;
            char line[900];
            int pos = snprintf(line, sizeof(line), "[perf] %lu ms: SLOW FRAME %.1f ms (work %.1f, Present %.1f; %lu player(s)%s):",
                               ScanMsSinceAttach(), PerfMs(wall), PerfMs(work), PerfMs(present), static_cast<unsigned long>(players),
                               captured ? "; A FRAME CAPTURE" : "");
            for (int i = 0; i < kPerfSlots && pos > 0 && pos < static_cast<int>(sizeof(line)) - 80; ++i)
            {
                if (i == kPerfPresentWait)
                    continue;
                pos += snprintf(line + pos, sizeof(line) - static_cast<size_t>(pos), " %s %.2f", kPerfNames[i], PerfMs(acc[i]));
                if ((i == kPerfPoses || i == kPerfD3dDraw || i == kPerfD3dOther || i == kPerfLog) && pos > 0 &&
                    pos < static_cast<int>(sizeof(line)) - 16)
                    pos += snprintf(line + pos, sizeof(line) - static_cast<size_t>(pos), " [%lu]", static_cast<unsigned long>(calls[i]));
                if (i + 1 < kPerfSlots && pos > 0 && pos < static_cast<int>(sizeof(line)) - 2)
                    pos += snprintf(line + pos, sizeof(line) - static_cast<size_t>(pos), ",");
            }
            Log("%s%s", line, g_perfSpikeLines == kPerfSpikeMax ? " (the last SLOW FRAME line)" : "");
        }
        if (GetTickCount() - w.startTick >= kPerfWinMs)
        {
            PerfWinLog(now);
            PerfWinReset(PerfTsc());
        }
        return;
    }
    memset(g_pAcc, 0, sizeof(g_pAcc));
    memset(g_pCalls, 0, sizeof(g_pCalls));
    g_pPairs = 0;
    g_pCycLast = cycAfter;
}

// ---- 21j-r12: THE TEXT GARBLE SCAN ([Display] TextGarbleScan, default 1; [vbscan] lines; log only) --------------------
// r11's video: player 2's spawn windows (and at the end the stats screen) showed other texts' letters - slices of "CAPTURE
// ENEMY COMMAND POSTS" in his VEHICLES box and on a title bar, smeared letters - the same frame after frame. The letters are
// cut at any vertex, not at whole letters: the draws read vertices that are not theirs. The game draws its texts, bars and
// pictures from dynamic vertex buffers - "wrappers" (GOG 1.3 constructor 0x00287320, 0x4C bytes: +04 a reference count word
// (count << 2, so 16383 at most), +08 the stride, +0C the write position, +10 the size in vertices, +18 the bytes handed out
// this frame, +1C static, +1D shared, +1E discard at the next lock, +38 the Direct3D buffer) - handed out per draw (the pool,
// GOG 1.3 0x002879F0) and locked (GOG 1.3 0x002881B0: no room left = discard and start at 0 again); its stream source is set
// through a cache (GOG 1.3 0x00276FF0: the last wrapper set per stream; setting the same wrapper again is skipped). Three ways
// for the garble, each watched here:
//  (1) a dynamic wrapper runs out of room in the middle of a frame (its lock discards while this frame's draws still wait in
//      the game's queue) - the Lock is ours (its single jump thunk): [vbscan] WRAP lines, and AHEAD lines when a shared
//      wrapper's position runs past what the pool handed out this frame (what makes it run out);
//  (2) a draw reads another buffer than the one the game set (its stream cache stale): each draw compares the device's
//      stream 0 (GetStreamSource) with the cached wrapper's buffer: [vbscan] STREAM lines;
//  (3) a wrapper freed while still in use (its reference count past 16383 wraps to 0): each lock checks the object is still
//      a wrapper (its vtable); every 10 s the wrappers with the highest reference counts: [vbscan] DEAD lines, the summary.
// End on the keyboard writes a MARK line with the numbers so far (press it when the text goes wrong). Nothing in the game
// changes: the Lock calls the game's own at once, the draws are only read.
bool g_vbWanted = false;         // [Display] TextGarbleScan (21j-r13: off by default - its job is done, see the glyph pool below)
volatile bool g_vbOn = false;    // the Lock is ours (the patterns matched)
DWORD g_vbThread = 0;            // the game's thread (the first Present's)
uintptr_t g_vbWrapperVt = 0;     // the wrappers' vtable
uintptr_t g_vbStreamCache = 0;   // the stream cache: the last wrapper set, per stream
typedef void*(__fastcall* VbLock_t)(uintptr_t w, void* edx, uint32_t count, uint32_t* out);
VbLock_t g_vbLockOrig = nullptr;
// the Lock's whole start (GOG 1.3 0x002881B0): the buffer +38h, static +1Ch, position +0Ch, discard +1Eh, size +10h, the
// discard flag 2000h - every field read here is in it
const char* const kVbLockBody = "55 8B EC 51 56 8B F1 C7 45 FC 00 00 00 00 8B 56 38 85 D2 74 78 80 7E 1C 00 53 57 8D 7E 0C 75 2C 80 "
                                "7E 1E 00 75 0C 8B 45 08 8B 1F 03 C3 3B 46 10 76 13 C6 46 1E 00 B8 00 20 00 00 C7 07 00 00 00 00";
// the constructor (GOG 1.3 0x00287358): its vtable, static +1Ch, shared +1Dh, size +10h, handed out +18h, position +0Ch, stride +08h
const char* const kVbCtorBody = "C7 06 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8A 45 10 0F 57 C0 8B 4D 0C 8B 55 08 88 46 1C 8A 45 14 88 46 1D 0F B6 "
                                "45 10 89 56 10 C7 46 18 00 00 00 00 C7 46 0C 00 00 00 00 89 4E 08";
constexpr uint32_t kVbCtorVt = 2;
// the cached SetStreamSource (GOG 1.3 0x00276FF0): cmp [table + stream * 4], wrapper; je (skip); ... device+190h; store
const char* const kVbStreamBody = "55 8B EC 8B 45 08 53 8B 5D 0C 39 1C 85 ?? ?? ?? ?? 74 32 56 8B 35 ?? ?? ?? ?? 8B CB 57 8B 3E E8 ?? ?? ?? "
                                  "?? 50 6A 00 8B CB E8 ?? ?? ?? ?? 50 FF 75 08 56 FF 97 90 01 00 00 8B 45 08 5F 5E 89 1C 85";
constexpr uint32_t kVbStreamTable = 0x0D;
// the reference count's release (GOG 1.3 0x0005C9A0): the word at +4, the count above its 2 low bits
const char* const kVbReleaseBody = "0F B7 51 04 66 8B C2 83 E2 03 66 C1 E8 02 66 48 66 C1 E0 02 66 0B C2";
constexpr uint32_t kVbRefWord = 0x04;
constexpr uint32_t kVbStride = 0x08;
constexpr uint32_t kVbPos = 0x0C;
constexpr uint32_t kVbCap = 0x10;
constexpr uint32_t kVbUsed = 0x18;
constexpr uint32_t kVbStatic = 0x1C;
constexpr uint32_t kVbShared = 0x1D;
constexpr uint32_t kVbDiscard = 0x1E;
constexpr uint32_t kVbBuffer = 0x38;
constexpr int kSlotGetStreamSource = 101;
typedef long(__stdcall* GetStreamSource_t)(void*, unsigned, void**, unsigned*, unsigned*);
constexpr DWORD kVbWinMs = 10000;

struct VbRec
{
    uintptr_t w;
    uint32_t stride, cap, buffer;
    uint8_t isStatic, shared;
    bool dead;
    uint16_t rcLast, rcLo, rcHi, rcWinStart;
    uint32_t locks, verts, wraps, ahead, maxPos;
};
constexpr int kVbRecs = 512;
VbRec g_vbRec[kVbRecs];
int g_vbRecUsed = 0;
bool g_vbRecFull = false;

struct VbWin
{
    DWORD start;
    uint32_t frames, locks, verts, frameDiscards, wrapsShared, wrapsPrivate, ahead, fails, deadLocks;
    uint32_t draws, mismatch, misOther, misEmpty, misStride, misDead, upDraws;
};
VbWin g_vbWin = {};
volatile LONG g_vbOtherThread = 0;
int g_vbEvents[8] = {};          // lines per kind
constexpr int kVbEventMax = 12;
enum VbEvent { kVbEvWrap, kVbEvAhead, kVbEvFail, kVbEvDead, kVbEvStream, kVbEvThread, kVbEvTable };
bool g_vbEndDown = false;
int g_vbMarks = 0;

bool VbEventBudget(int kind)
{
    return g_vbEvents[kind]++ < kVbEventMax;
}

uint16_t VbRefCount(uintptr_t w)
{
    return static_cast<uint16_t>(*reinterpret_cast<const uint16_t*>(w + kVbRefWord) >> 2);
}

// this wrapper's record (made the first time it is seen); null when the table is full
VbRec* VbRecFor(uintptr_t w)
{
    uint32_t i = static_cast<uint32_t>((w >> 4) ^ (w >> 13)) & (kVbRecs - 1);
    for (int n = 0; n < kVbRecs; ++n, i = (i + 1) & (kVbRecs - 1))
    {
        VbRec& r = g_vbRec[i];
        if (r.w == w)
            return &r;
        if (!r.w)
        {
            if (g_vbRecUsed >= kVbRecs - 8)
                break;
            memset(&r, 0, sizeof(r));
            r.w = w;
            const uint16_t rc = VbRefCount(w);
            r.rcLast = r.rcLo = r.rcHi = r.rcWinStart = rc;
            ++g_vbRecUsed;
            return &r;
        }
    }
    if (!g_vbRecFull && VbEventBudget(kVbEvTable))
        Log("[vbscan] %lu ms: the wrapper table is full (%d) - wrappers seen from now on are not followed one by one", ScanMsSinceAttach(),
            g_vbRecUsed);
    g_vbRecFull = true;
    return nullptr;
}

void VbCallers(char* out, size_t size)
{
    uint32_t c[3] = {};
    const int n = GameCallers(c, 3);
    snprintf(out, size, "rva 0x%08X < 0x%08X < 0x%08X", n > 0 ? c[0] : 0u, n > 1 ? c[1] : 0u, n > 2 ? c[2] : 0u);
}

// the game's Lock(count, &start) on wrapper w (thiscall, ret 8)
void* __fastcall VbLockWrap(uintptr_t w, void* edx, uint32_t count, uint32_t* out)
{
    if (!g_vbOn || !w)
        return g_vbLockOrig(w, edx, count, out);
    if (GetCurrentThreadId() != g_vbThread)
    {
        if (g_vbThread && InterlockedIncrement(&g_vbOtherThread) <= 4 && VbEventBudget(kVbEvThread))
            Log("[vbscan] %lu ms: a lock on ANOTHER thread (%lu, the game's is %lu): wrapper 0x%08X, %lu vertices", ScanMsSinceAttach(),
                static_cast<unsigned long>(GetCurrentThreadId()), static_cast<unsigned long>(g_vbThread), static_cast<unsigned>(w),
                static_cast<unsigned long>(count));
        return g_vbLockOrig(w, edx, count, out);
    }
    const bool dead = *reinterpret_cast<const uint32_t*>(w) != g_vbWrapperVt;
    const uint32_t stride = *reinterpret_cast<const uint32_t*>(w + kVbStride);
    const uint32_t pos = *reinterpret_cast<const uint32_t*>(w + kVbPos);
    const uint32_t cap = *reinterpret_cast<const uint32_t*>(w + kVbCap);
    const uint8_t isStatic = *reinterpret_cast<const uint8_t*>(w + kVbStatic);
    const uint8_t shared = *reinterpret_cast<const uint8_t*>(w + kVbShared);
    const uint8_t discard = *reinterpret_cast<const uint8_t*>(w + kVbDiscard);
    const bool wrap = !isStatic && !discard && pos + count > cap;
    ++g_vbWin.locks;
    g_vbWin.verts += count;
    if (!isStatic && discard)
        ++g_vbWin.frameDiscards;
    if (dead)
    {
        ++g_vbWin.deadLocks;
        if (VbEventBudget(kVbEvDead))
        {
            char c[80];
            VbCallers(c, sizeof(c));
            Log("[vbscan] %lu ms: DEAD - a lock on 0x%08X, which is NOT a vertex buffer wrapper any more (vtable 0x%08X, reference word "
                "0x%04X): %lu vertices, from %s",
                ScanMsSinceAttach(), static_cast<unsigned>(w), *reinterpret_cast<const uint32_t*>(w),
                *reinterpret_cast<const uint16_t*>(w + kVbRefWord), static_cast<unsigned long>(count), c);
        }
    }
    if (wrap)
    {
        if (shared)
            ++g_vbWin.wrapsShared;
        else
            ++g_vbWin.wrapsPrivate;
        if (VbEventBudget(kVbEvWrap))
        {
            char c[80];
            VbCallers(c, sizeof(c));
            Log("[vbscan] %lu ms: WRAP - %s dynamic wrapper 0x%08X (stride %lu, %lu vertices) runs out of room in the middle of frame %ld: "
                "at %lu, %lu more asked, %lu bytes handed out this frame - it discards and starts at 0 again (the draws of this frame "
                "still queued from it get other vertices); from %s",
                ScanMsSinceAttach(), shared ? "a SHARED" : "a private", static_cast<unsigned>(w), static_cast<unsigned long>(stride),
                static_cast<unsigned long>(cap), static_cast<long>(g_calls[kHPresent]), static_cast<unsigned long>(pos),
                static_cast<unsigned long>(count), static_cast<unsigned long>(*reinterpret_cast<const uint32_t*>(w + kVbUsed)), c);
        }
    }
    void* const r = g_vbLockOrig(w, edx, count, out);
    if (!r)
    {
        ++g_vbWin.fails;
        if (VbEventBudget(kVbEvFail))
        {
            char c[80];
            VbCallers(c, sizeof(c));
            Log("[vbscan] %lu ms: FAILED - the lock of wrapper 0x%08X (stride %lu, %lu vertices, %s) gave no memory: at %lu, %lu asked; "
                "from %s",
                ScanMsSinceAttach(), static_cast<unsigned>(w), static_cast<unsigned long>(stride), static_cast<unsigned long>(cap),
                isStatic ? "static" : "dynamic", static_cast<unsigned long>(pos), static_cast<unsigned long>(count), c);
        }
    }
    const uint32_t posAfter = *reinterpret_cast<const uint32_t*>(w + kVbPos);
    const uint32_t used = *reinterpret_cast<const uint32_t*>(w + kVbUsed);
    const bool ahead = !dead && !isStatic && shared && stride && static_cast<uint64_t>(posAfter) * stride > used;
    if (ahead)
    {
        ++g_vbWin.ahead;
        if (VbEventBudget(kVbEvAhead))
        {
            char c[80];
            VbCallers(c, sizeof(c));
            Log("[vbscan] %lu ms: AHEAD - shared wrapper 0x%08X (stride %lu, %lu vertices): its position is %lu after this lock of %lu, "
                "but the pool handed out only %lu bytes (%lu vertices) this frame - a lock the pool did not count; from %s",
                ScanMsSinceAttach(), static_cast<unsigned>(w), static_cast<unsigned long>(stride), static_cast<unsigned long>(cap),
                static_cast<unsigned long>(posAfter), static_cast<unsigned long>(count), static_cast<unsigned long>(used),
                static_cast<unsigned long>(used / stride), c);
        }
    }
    VbRec* const rec = VbRecFor(w);
    if (rec && rec->dead && !dead)
    {
        // a new wrapper where a freed one was (its record from scratch)
        const uint16_t now = VbRefCount(w);
        rec->dead = false;
        rec->rcLast = rec->rcLo = rec->rcHi = rec->rcWinStart = now;
    }
    if (rec)
    {
        const uint16_t rc = dead ? rec->rcLast : VbRefCount(w);
        rec->stride = stride;
        rec->cap = cap;
        rec->isStatic = isStatic;
        rec->shared = shared;
        rec->buffer = dead ? rec->buffer : *reinterpret_cast<const uint32_t*>(w + kVbBuffer);
        rec->dead = rec->dead || dead;
        rec->rcLast = rc;
        if (rc < rec->rcLo)
            rec->rcLo = rc;
        if (rc > rec->rcHi)
            rec->rcHi = rc;
        ++rec->locks;
        rec->verts += count;
        rec->wraps += wrap ? 1 : 0;
        rec->ahead += ahead ? 1 : 0;
        if (posAfter > rec->maxPos && !isStatic)
            rec->maxPos = posAfter;
    }
    return r;
}

// a draw (DrawPrimitive / DrawIndexedPrimitive, the game's thread): is stream 0 the buffer of the wrapper the game set?
void VbStreamCheck(void* dev)
{
    if (!g_vbOn || !g_vbStreamCache || GetCurrentThreadId() != g_vbThread)
        return;
    ++g_vbWin.draws;
    void** vt = *reinterpret_cast<void***>(dev);
    void* vb = nullptr;
    unsigned off = 0;
    unsigned stride = 0;
    if (reinterpret_cast<GetStreamSource_t>(vt[kSlotGetStreamSource])(dev, 0, &vb, &off, &stride) < 0)
        vb = nullptr;
    if (vb)
        reinterpret_cast<Release_t>((*reinterpret_cast<void***>(vb))[2])(vb); // (GetStreamSource added a reference)
    const uintptr_t want = *reinterpret_cast<const uint32_t*>(g_vbStreamCache);
    if (!want)
        return;
    const bool dead = *reinterpret_cast<const uint32_t*>(want) != g_vbWrapperVt;
    const uint32_t wantVb = dead ? 0u : *reinterpret_cast<const uint32_t*>(want + kVbBuffer);
    const uint32_t wantStride = dead ? 0u : *reinterpret_cast<const uint32_t*>(want + kVbStride);
    const uint32_t haveVb = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(vb));
    const char* what = nullptr;
    if (dead)
    {
        ++g_vbWin.misDead;
        what = "the cached wrapper is NOT a wrapper any more (freed)";
    }
    else if (!vb)
    {
        ++g_vbWin.misEmpty;
        what = "stream 0 is EMPTY on the device (a ...PrimitiveUP draw clears it) while the game's cache still has a wrapper";
    }
    else if (haveVb != wantVb)
    {
        ++g_vbWin.misOther;
        what = "the device has ANOTHER buffer on stream 0 than the cached wrapper's";
    }
    else if (stride != wantStride || off != 0)
    {
        ++g_vbWin.misStride;
        what = "the right buffer with ANOTHER stride / offset";
    }
    if (!what)
        return;
    ++g_vbWin.mismatch;
    if (VbEventBudget(kVbEvStream))
    {
        char c[80];
        VbCallers(c, sizeof(c));
        Log("[vbscan] %lu ms: STREAM - %s: cached wrapper 0x%08X (its buffer 0x%08X, stride %lu), the device's stream 0 buffer 0x%08X "
            "(stride %u, offset %u); the draw from %s",
            ScanMsSinceAttach(), what, static_cast<unsigned>(want), wantVb, static_cast<unsigned long>(wantStride), haveVb, stride, off, c);
    }
}

void VbSummary(const char* why)
{
    VbWin& w = g_vbWin;
    int shared = 0;
    int priv = 0;
    int stat = 0;
    int deadN = 0;
    for (const VbRec& r : g_vbRec)
    {
        if (!r.w || !r.locks)
            continue;
        if (r.isStatic)
            ++stat;
        else if (r.shared)
            ++shared;
        else
            ++priv;
        deadN += r.dead ? 1 : 0;
    }
    const double secs = static_cast<double>(GetTickCount() - w.start) / 1000.0;
    Log("[vbscan] %lu ms, %s %.1f s: %lu frames; %lu locks (%lu vertices) on %d shared / %d private dynamic and %d static wrappers "
        "(%d of them dead); frame-start discards %lu; MID-FRAME WRAPS %lu shared / %lu private; AHEAD %lu; failed locks %lu; locks on a "
        "dead wrapper %lu; on other threads %ld so far | draws checked %lu: STREAM MISMATCHES %lu (another buffer %lu, empty %lu, "
        "another stride / offset %lu, a dead cached wrapper %lu); ...PrimitiveUP draws %lu",
        ScanMsSinceAttach(), why, secs, static_cast<unsigned long>(w.frames), static_cast<unsigned long>(w.locks),
        static_cast<unsigned long>(w.verts), shared, priv, stat, deadN, static_cast<unsigned long>(w.frameDiscards),
        static_cast<unsigned long>(w.wrapsShared), static_cast<unsigned long>(w.wrapsPrivate), static_cast<unsigned long>(w.ahead),
        static_cast<unsigned long>(w.fails), static_cast<unsigned long>(w.deadLocks), static_cast<long>(g_vbOtherThread),
        static_cast<unsigned long>(w.draws), static_cast<unsigned long>(w.mismatch), static_cast<unsigned long>(w.misOther),
        static_cast<unsigned long>(w.misEmpty), static_cast<unsigned long>(w.misStride), static_cast<unsigned long>(w.misDead),
        static_cast<unsigned long>(w.upDraws));
    // the wrappers with the highest reference counts (a count that only climbs reaches 16383 and wraps to 0: freed in use), the
    // dead ones, and any that wrapped or ran ahead
    const VbRec* pick[8] = {};
    int np = 0;
    bool taken[kVbRecs] = {};
    for (int k = 0; k < 8; ++k)
    {
        int best = -1;
        for (int i = 0; i < kVbRecs; ++i)
        {
            const VbRec& r = g_vbRec[i];
            if (!r.w || taken[i] || !r.locks)
                continue;
            const int score = (r.dead ? 100000 : 0) + (r.wraps || r.ahead ? 50000 : 0) + r.rcHi;
            const int bestScore = best < 0 ? -1
                                           : (g_vbRec[best].dead ? 100000 : 0) + (g_vbRec[best].wraps || g_vbRec[best].ahead ? 50000 : 0) +
                                                 g_vbRec[best].rcHi;
            if (score > bestScore)
                best = i;
        }
        if (best < 0)
            break;
        taken[best] = true;
        pick[np++] = &g_vbRec[best];
    }
    for (int k = 0; k < np; ++k)
    {
        const VbRec& r = *pick[k];
        Log("[vbscan]   wrapper 0x%08X%s: stride %lu, %lu vertices, %s; reference count %u (%u .. %u, %+d in this window); %lu locks, "
            "%lu vertices; highest position %lu; wraps %lu, ahead %lu; Direct3D buffer 0x%08X",
            static_cast<unsigned>(r.w), r.dead ? " (DEAD)" : "", static_cast<unsigned long>(r.stride), static_cast<unsigned long>(r.cap),
            r.isStatic ? "static" : (r.shared ? "shared dynamic" : "private dynamic"), r.rcLast, r.rcLo, r.rcHi,
            static_cast<int>(r.rcLast) - static_cast<int>(r.rcWinStart), static_cast<unsigned long>(r.locks),
            static_cast<unsigned long>(r.verts), static_cast<unsigned long>(r.maxPos), static_cast<unsigned long>(r.wraps),
            static_cast<unsigned long>(r.ahead), static_cast<unsigned>(r.buffer));
    }
}

void VbWinReset()
{
    memset(&g_vbWin, 0, sizeof(g_vbWin));
    g_vbWin.start = GetTickCount();
    for (VbRec& r : g_vbRec)
    {
        if (!r.w)
            continue;
        r.locks = r.verts = r.wraps = r.ahead = r.maxPos = 0;
        r.rcLo = r.rcHi = r.rcWinStart = r.rcLast;
    }
}

// the game's Present (its thread): a frame ends; every 10 s the summary; End = MARK
void VbFrame()
{
    if (!g_vbOn)
        return;
    if (!g_vbThread)
    {
        g_vbThread = GetCurrentThreadId();
        VbWinReset();
        Log("[vbscan] %lu ms: the game's thread is %lu (the first Present) - the scan runs", ScanMsSinceAttach(),
            static_cast<unsigned long>(g_vbThread));
        return;
    }
    if (GetCurrentThreadId() != g_vbThread)
        return;
    ++g_vbWin.frames;
    const bool end = (GetAsyncKeyState(VK_END) & 0x8000) != 0;
    if (end && !g_vbEndDown && g_vbMarks < 40)
    {
        ++g_vbMarks;
        Log("[vbscan] %lu ms: MARK %d (End pressed - the text looks wrong now)", ScanMsSinceAttach(), g_vbMarks);
        VbSummary("MARK - this window so far,");
    }
    g_vbEndDown = end;
    if (GetTickCount() - g_vbWin.start >= kVbWinMs)
    {
        VbSummary("last");
        VbWinReset();
    }
}

// the Lock's single jump thunk moved to VbLockWrap (FrameCaptureStart, the code unlocked)
void VbScanInstall()
{
    if (!g_vbWanted)
    {
        Log("[vbscan] [Display] TextGarbleScan=0 - the text garble scan is off");
        return;
    }
    uint32_t hits[4] = {};
    const int nl = FindPattern(kVbLockBody, hits, 4);
    const uint32_t lock = nl == 1 ? hits[0] : 0;
    const int nc = FindPattern(kVbCtorBody, hits, 4);
    const uint32_t ctor = nc == 1 ? hits[0] : 0;
    const int ns = FindPattern(kVbStreamBody, hits, 4);
    const uint32_t stream = ns == 1 ? hits[0] : 0;
    const int nr = FindPattern(kVbReleaseBody, hits, 4);
    g_vbWrapperVt = ctor ? ReadU32(Rva(ctor + kVbCtorVt)) : 0;
    g_vbStreamCache = stream ? ReadU32(Rva(stream + kVbStreamTable)) : 0;
    // the Lock's jump thunk (an E9 in the link table) and any direct calls
    uint32_t thunk = 0;
    int thunks = 0;
    int direct = 0;
    uint32_t codeRva = 0;
    uint32_t codeSize = 0;
    if (lock && CodeSection(&codeRva, &codeSize))
    {
        const unsigned char* p = reinterpret_cast<const unsigned char*>(Rva(codeRva));
        for (uint32_t i = 0; i + 5 <= codeSize; ++i)
        {
            if (p[i] != 0xE9 && p[i] != 0xE8)
                continue;
            int32_t rel = 0;
            memcpy(&rel, p + i + 1, 4);
            if (static_cast<uint32_t>(static_cast<int64_t>(codeRva + i) + 5 + rel) != lock)
                continue;
            if (p[i] == 0xE8)
            {
                ++direct;
                continue;
            }
            const bool table = (i >= 5 && p[i - 5] == 0xE9) || (i + 10 <= codeSize && p[i + 5] == 0xE9);
            if (table)
            {
                ++thunks;
                thunk = codeRva + i;
            }
        }
    }
    const bool vtOk = g_vbWrapperVt && InExeCode(ReadU32(g_vbWrapperVt));
    if (!lock || !vtOk || !g_vbStreamCache || nr != 1 || thunks != 1)
    {
        Log("[vbscan] the text garble scan NOT installed: Lock %d hit(s) 0x%08X, wrapper constructor %d hit(s) (vtable 0x%08X %s), the "
            "stream cache setter %d hit(s) (table 0x%08X), the reference count's release %d hit(s), the Lock's jump thunks %d (need 1), "
            "direct calls %d",
            nl, lock, nc, static_cast<unsigned>(g_vbWrapperVt), vtOk ? "OK" : "not code", ns, static_cast<unsigned>(g_vbStreamCache), nr,
            thunks, direct);
        return;
    }
    unsigned char* const at = reinterpret_cast<unsigned char*>(Rva(thunk));
    DWORD old = 0;
    if (!VirtualProtect(at + 1, 4, PAGE_EXECUTE_READWRITE, &old))
    {
        Log("[vbscan] the text garble scan NOT installed: VirtualProtect failed (error %lu)", GetLastError());
        return;
    }
    g_vbLockOrig = reinterpret_cast<VbLock_t>(Rva(lock));
    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(&VbLockWrap) - (reinterpret_cast<uintptr_t>(at) + 5));
    g_vbOn = true; // (before the jump moves: the wrapper calls the game's Lock either way)
    memcpy(at + 1, &rel, 4);
    DWORD ignored = 0;
    VirtualProtect(at + 1, 4, old, &ignored);
    FlushInstructionCache(GetCurrentProcess(), at, 5);
    int32_t back = 0;
    memcpy(&back, at + 1, 4);
    Log("[vbscan] 21j-r12: the TEXT GARBLE SCAN is ON (log only): the vertex buffer Lock rva 0x%08X now goes through BF1GameExt (its "
        "jump thunk rva 0x%08X%s; %d direct calls), the wrappers' vtable 0x%08X, the stream cache at 0x%08X; each draw's stream 0 "
        "compared with the cached wrapper's buffer. [vbscan] lines: WRAP / AHEAD / FAILED / DEAD / STREAM when they happen, a "
        "summary every 10 s, MARK when End is pressed",
        lock, thunk, back == rel ? "" : " - READ BACK FAILED", direct, static_cast<unsigned>(g_vbWrapperVt),
        static_cast<unsigned>(g_vbStreamCache));
}

// ---- 21j-r13: THE GLYPH POOL GROWS ([Display] GlyphPoolGrow, default 1; [glyphs] lines) ---------------------------------
// THE SCRAMBLED TEXT (r11 / r12, 4 players). r12's scan found the vertex buffers clean (no wraps, no stream mismatches, no
// freed buffers) - the bad vertices come from before them. Every text (RedTextElement) keeps its letters' vertices in
// "glyph chunks" (0x3C8 bytes: 48 vertices = 8 letters, +3C0h their count, +3C4h the next) taken from ONE pool the game makes
// at start (GOG 1.3 0x0029E370: 10240 letters - 1280 chunks; 8000 in one mode), its pointer in a global; a text takes whole
// chunks, at least one per font page, however short it is. When the pool is empty the layout (GOG 1.3 0x0029E040) gets an
// empty chunk list (the allocator, 0x0029DD60, gives back what it took) but keeps the text's letter count: its draw then
// reserves that many letters in the vertex buffer, copies nothing and draws whatever an earlier text left there - other texts'
// letters, cut anywhere, the same frame after frame; with nothing left there, nothing (r12's empty stats screen). Every text
// laid out after the pool ran dry is wrong (player 2's windows made late, the stats screen), the ones laid out before are fine.
// The game's four quarters - four HUDs, our windows, players 3 / 4's menus - need more than 1280 chunks at once.
// Fix: the allocator's single jump thunk goes through BF1GameExt; before each allocation, if the pool's free list is shorter
// than the text needs plus a reserve, BF1GameExt adds 1024 chunks of its own to it (pushed as the game's own free does: count
// 0, next = the old head) - the game's code takes and gives them back like its own (they are never handed to the game's heap).
// At most 16 such blocks (16384 chunks, about 15.5 MB). [glyphs] lines: each block added, and the pool every 30 s.
bool g_gpWanted = true;          // [Display] GlyphPoolGrow
volatile bool g_gpOn = false;
uintptr_t g_gpPoolGlobal = 0;    // the global holding the pool's pointer
typedef void*(__fastcall* GpAlloc_t)(uintptr_t pool, void* edx, void* out, uint32_t letters);
GpAlloc_t g_gpAllocOrig = nullptr;
// the allocator (GOG 1.3 0x0029DD60): thiscall (the pool), (out, letters), ret 8; pops a chunk per 8 letters, gives them all
// back if one is missing
const char* const kGpAllocBody = "55 8B EC 83 EC 08 53 56 8B 75 0C 8B D9 57 85 F6 0F 84 7E 00 00 00 E8 ?? ?? ?? ?? 8B F8 85 FF 74 73 57 8D "
                                 "4D F8 E8 ?? ?? ?? ?? 83 FE 08 76 1C 90 8B CB E8 ?? ?? ?? ?? 89 87 C4 03 00 00 85 C0 74 37 83 EE 08 8B F8 "
                                 "83 FE 08 77 E5";
// its pop (GOG 1.3 0x0029DE40): the free list's head at the pool +4, a chunk's next at +3C4h, its count set to 48 at +3C0h
const char* const kGpPopBody = "8B D1 8B 42 04 85 C0 75 01 C3 8B 88 C4 03 00 00 89 4A 04 C7 80 C0 03 00 00 30 00 00 00 C3";
// its free (GOG 1.3 0x0029E330): count 0, next = the head, the head = the chunk - what BF1GameExt does with its own
const char* const kGpFreeBody = "55 8B EC 8B 55 08 C7 82 C0 03 00 00 00 00 00 00 8B 41 04 89 82 C4 03 00 00 89 51 04 5D C2 04 00";
// the pool's making (GOG 1.3 0x0029E370): new(8), the pool's constructor, mov [global], eax
const char* const kGpInitBody = "55 8B EC 51 6A 08 C7 05 ?? ?? ?? ?? 00 00 00 00 C6 05 ?? ?? ?? ?? 00 E8 ?? ?? ?? ?? 83 C4 04 89 45 FC 85 C0 "
                                "74 13 FF 75 08 8B C8 E8 ?? ?? ?? ?? A3 ?? ?? ?? ?? 8B E5 5D C3";
constexpr uint32_t kGpInitGlobal = 0x31;
constexpr uint32_t kGpHead = 0x04;      // the pool: its free list's head
constexpr uint32_t kGpCount = 0x3C0;    // a chunk: its vertex count
constexpr uint32_t kGpNext = 0x3C4;     // a chunk: the next
constexpr uint32_t kGpChunk = 0x3C8;    // a chunk's size
constexpr uint32_t kGpBlockChunks = 1024;
constexpr int kGpMaxBlocks = 16;
constexpr uint32_t kGpReserve = 64;     // chunks kept free beyond what the text asks for
int g_gpBlocks = 0;
volatile LONG g_gpAllocs = 0;           // allocations seen
volatile LONG g_gpSaved = 0;            // allocations the pool alone could not have served
uint32_t g_gpLowest = 0xFFFFFFFFu;      // the fewest free chunks seen at an allocation (before a block)
DWORD g_gpLastLog = 0;
bool g_gpCapLogged = false;
uintptr_t g_gpLoggedPool = 0;

// the pool's free chunks, counted up to `most`
uint32_t GpFree(uintptr_t pool, uint32_t most)
{
    uint32_t n = 0;
    for (uintptr_t c = *reinterpret_cast<const uint32_t*>(pool + kGpHead); c && n < most;
         c = *reinterpret_cast<const uint32_t*>(c + kGpNext))
        ++n;
    return n;
}

// a block of chunks of BF1GameExt's own pushed on the pool's free list (as the game's own free); false at the cap / no memory
bool GpTopUp(uintptr_t pool, uint32_t freeNow, uint32_t need)
{
    if (g_gpBlocks >= kGpMaxBlocks)
    {
        if (!g_gpCapLogged)
        {
            g_gpCapLogged = true;
            Log("[glyphs] %lu ms: the glyph pool ran low AGAIN after %d blocks of BF1GameExt's own (%lu chunks) - no more are added; "
                "something keeps texts alive (a leak) - the scrambled text can come back",
                ScanMsSinceAttach(), g_gpBlocks, static_cast<unsigned long>(g_gpBlocks * kGpBlockChunks));
        }
        return false;
    }
    unsigned char* const mem =
        static_cast<unsigned char*>(VirtualAlloc(nullptr, kGpBlockChunks * kGpChunk, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    if (!mem)
    {
        Log("[glyphs] %lu ms: the glyph pool is low (%lu free) but VirtualAlloc failed (error %lu) - nothing added", ScanMsSinceAttach(),
            static_cast<unsigned long>(freeNow), GetLastError());
        g_gpBlocks = kGpMaxBlocks; // (no more tries)
        return false;
    }
    uint32_t* const head = reinterpret_cast<uint32_t*>(pool + kGpHead);
    for (uint32_t i = 0; i < kGpBlockChunks; ++i)
    {
        unsigned char* const c = mem + static_cast<size_t>(i) * kGpChunk;
        *reinterpret_cast<uint32_t*>(c + kGpCount) = 0;
        *reinterpret_cast<uint32_t*>(c + kGpNext) = *head;
        *head = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(c));
    }
    ++g_gpBlocks;
    Log("[glyphs] %lu ms: the game's glyph pool ran low - %lu chunk(s) free, a text needs %lu (+%lu kept free): %lu chunks (%lu letters) "
        "of BF1GameExt's own added (block %d of at most %d, %lu chunks in all)%s",
        ScanMsSinceAttach(), static_cast<unsigned long>(freeNow), static_cast<unsigned long>(need), static_cast<unsigned long>(kGpReserve),
        static_cast<unsigned long>(kGpBlockChunks), static_cast<unsigned long>(kGpBlockChunks * 8), g_gpBlocks, kGpMaxBlocks,
        static_cast<unsigned long>(g_gpBlocks * kGpBlockChunks),
        freeNow < need ? " - THE POOL WAS DRY: without them this text would have shown other texts' letters" : "");
    return true;
}

// the game's allocation of a text's chunk list (letters on one font page): room first
void* __fastcall GpAllocWrap(uintptr_t pool, void* edx, void* out, uint32_t letters)
{
    if (g_gpOn && pool && letters)
    {
        InterlockedIncrement(&g_gpAllocs);
        const uint32_t need = (letters + 7) / 8;
        const uint32_t freeN = GpFree(pool, need + kGpReserve);
        if (freeN < need && freeN < g_gpLowest)
            g_gpLowest = freeN;
        if (freeN < need + kGpReserve)
        {
            if (freeN < need)
                InterlockedIncrement(&g_gpSaved);
            GpTopUp(pool, freeN, need);
        }
    }
    return g_gpAllocOrig(pool, edx, out, letters);
}

// ---- 21j-r14: THE GLYPH LEAK FIXED ([Display] GlyphLeakFix, default 1; in the [glyphs] lines) -----------------------------
// r13's log (4 players on Bespin Platforms to the stats screen, the main menu, then a 2-player match): the pool ran low at
// 392 s (block 1 added, no text scrambled) - and its chunks in use NEVER came back down: 611 in use at the main menu at start,
// 1645 at the sign-in screen after the match, 1738 in the 2-player match after it; each 3 / 4-player spawn screen switch left
// some more in use. Why (GOG 1.3): a text's chunks go back to the pool only through ReleaseGlyphs (0x0029DFB0: each font page's
// list to the pool's free-list function 0x0029E300, the page's count and list 0) - the text's SetFont (0x0029E800) and its
// SetText("") call it; its DESTRUCTOR DOES NOT: RedTextElement's (and FlashyTextElement's) destructor runs
// RedGlyphCacheElement's (0x0029DAF0), whose cleanup (0x0029DEA0) gives the heap the arrays that hold the lists (+8Ch the
// counts, +90h the lists, +94h the materials) and drops the chunks in them - out of the pool for good. Every text deleted with
// letters in it leaks them (the game's own screens at every match / shell change; with 3 / 4 players the windows and texts
// BF1GameExt makes and deletes each time a spawn screen comes up). r13's blocks only put the dry pool off.
// Fix: the cleanup's single jump thunk goes through BF1GameExt, which first runs the game's own ReleaseGlyphs on the element
// when it still holds chunks (what SetFont does before it drops a font's arrays), then the game's cleanup as before.
bool g_glWanted = true;          // [Display] GlyphLeakFix
volatile bool g_glOn = false;
uintptr_t g_glPoolGlobal = 0;    // the pool's global, as ReleaseGlyphs reads it
typedef void(__fastcall* GlNoArg_t)(uintptr_t self, void* edx);
typedef uint8_t(__fastcall* GlPages_t)(uintptr_t font, void* edx);
GlNoArg_t g_glRelease = nullptr;     // the game's ReleaseGlyphs
GlNoArg_t g_glCleanupOrig = nullptr; // RedGlyphCacheElement's cleanup (the destructor's)
volatile LONG g_glDeleted = 0;   // glyph cache elements (texts) deleted
volatile LONG g_glHolding = 0;   // ... of them still holding chunks
volatile LONG g_glReturned = 0;  // the chunks they held, back in the pool
bool g_glFirstLogged = false;
// ReleaseGlyphs (GOG 1.3 0x0029DFB0): thiscall (the element); for each page of its font (+80h, vfunc 4: the page count): the
// count 0, the list (if any) to the pool's free-list function (mov ecx, [the pool's global]; push list; call), the list 0
const char* const kGlReleaseBody = "57 8B F9 8B 8F 80 00 00 00 85 C9 74 4D 8B 01 53 56 8B 40 04 FF D0 33 F6 0F B6 D8 84 C0 74 39 90 8B 87 8C "
                                   "00 00 00 C7 04 B0 00 00 00 00 8B 87 90 00 00 00 8B 04 B0 85 C0 74 19 8B 0D ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 8B "
                                   "87 90 00 00 00 C7 04 B0 00 00 00 00 46 3B F3 72 C8 5E 5B 5F C3";
constexpr uint32_t kGlReleaseGlobal = 0x3C;   // its mov ecx, [global]'s operand
constexpr uint32_t kGlReleaseFreeCall = 0x41; // its call of the free-list function
// the free-list function (GOG 1.3 0x0029E300): thiscall (the pool), (list): an iterator on the list, then each chunk freed
const char* const kGlFreeListBody = "55 8B EC 83 EC 08 56 FF 75 08 8B F1 8D 4D F8 E8 ?? ?? ?? ?? 8D 45 F8 8B CE 50 E8 ?? ?? ?? ?? 5E 8B E5 5D "
                                    "C2 04 00";
// RedGlyphCacheElement's cleanup (GOG 1.3 0x0029DEA0): thiscall (the element), ret; +8Ch and +90h to the heap (no chunk freed),
// then +94h's materials
const char* const kGlCleanupBody = "56 8B F1 8B 86 8C 00 00 00 85 C0 74 19 50 FF 35 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 08 C7 86 8C 00 00 00 00 "
                                   "00 00 00 8B 86 90 00 00 00 85 C0 74 19 50 FF 35 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 08 C7 86 90 00 00 00 00 "
                                   "00 00 00 8B 86 94 00 00 00 85 C0 74 6E 8B 8E 80 00 00 00 85 C9 74 4B";
constexpr uint32_t kGlFont = 0x80;   // the element: its font
constexpr uint32_t kGlCounts = 0x8C; // ... each page's letter count
constexpr uint32_t kGlLists = 0x90;  // ... each page's chunk list

// the chunks the element still holds (each page of its font)
uint32_t GlHeld(uintptr_t e)
{
    const uintptr_t font = ReadU32(e + kGlFont);
    const uintptr_t lists = ReadU32(e + kGlLists);
    if (!font || !lists || !ReadU32(e + kGlCounts))
        return 0;
    const uintptr_t vt = ReadU32(font);
    const uintptr_t pagesFn = vt ? ReadU32(vt + 4) : 0;
    if (!pagesFn)
        return 0;
    const uint32_t pages = reinterpret_cast<GlPages_t>(pagesFn)(font, nullptr);
    uint32_t n = 0;
    for (uint32_t p = 0; p < pages; ++p)
        for (uintptr_t c = ReadU32(lists + 4 * p); c && n < 200000; c = ReadU32(c + kGpNext))
            ++n;
    return n;
}

// the destructor's cleanup: the element's chunks back in the pool first (the game's own ReleaseGlyphs), then the game's cleanup
void __fastcall GlCleanupWrap(uintptr_t e, void* edx)
{
    if (g_glOn && e)
    {
        InterlockedIncrement(&g_glDeleted);
        const uint32_t held = ReadU32(g_glPoolGlobal) ? GlHeld(e) : 0;
        if (held)
        {
            InterlockedIncrement(&g_glHolding);
            InterlockedExchangeAdd(&g_glReturned, static_cast<LONG>(held));
            g_glRelease(e, nullptr);
            if (!g_glFirstLogged)
            {
                g_glFirstLogged = true;
                Log("[glyphs] %lu ms: 21j-r14: the first text deleted with letters in it - its %lu chunk(s) back in the pool (the game's "
                    "destructor would have dropped them)",
                    ScanMsSinceAttach(), static_cast<unsigned long>(held));
            }
        }
    }
    g_glCleanupOrig(e, edx);
}

// every 30 s (Present, the game's thread): the pool's free chunks
void GpFrame()
{
    const uintptr_t global = g_gpPoolGlobal ? g_gpPoolGlobal : g_glPoolGlobal;
    if ((!g_gpOn && !g_glOn) || !global)
        return;
    const DWORD now = GetTickCount();
    if (now - g_gpLastLog < 30000)
        return;
    g_gpLastLog = now;
    const uintptr_t pool = *reinterpret_cast<const uint32_t*>(global);
    if (!pool)
        return;
    const uint32_t freeN = GpFree(pool, 200000);
    char leak[200] = "";
    if (g_glOn)
        snprintf(leak, sizeof(leak), "; texts deleted %ld, %ld of them holding letters - %ld chunk(s) back in the pool (21j-r14)",
                 static_cast<long>(g_glDeleted), static_cast<long>(g_glHolding), static_cast<long>(g_glReturned));
    else
        snprintf(leak, sizeof(leak), "; GlyphLeakFix off: deleted texts keep their chunks out of the pool");
    Log("[glyphs] %lu ms: the glyph pool 0x%08X: %lu chunk(s) free (%lu letters); %ld text layouts so far, %ld of them found it dry; "
        "BF1GameExt's own blocks %d (%lu chunks)%s%s",
        ScanMsSinceAttach(), static_cast<unsigned>(pool), static_cast<unsigned long>(freeN), static_cast<unsigned long>(freeN * 8),
        static_cast<long>(g_gpAllocs), static_cast<long>(g_gpSaved), g_gpBlocks, static_cast<unsigned long>(g_gpBlocks * kGpBlockChunks),
        leak, pool != g_gpLoggedPool && g_gpLoggedPool ? " - A NEW POOL (the game made it again)" : "");
    g_gpLoggedPool = pool;
}

// fn's single jump thunk in the incremental link table (and how many direct calls it has); 0 unless exactly one
uint32_t LinkThunkOf(uint32_t fn, int* thunksOut, int* directOut)
{
    uint32_t thunk = 0;
    int thunks = 0;
    int direct = 0;
    uint32_t codeRva = 0;
    uint32_t codeSize = 0;
    if (fn && CodeSection(&codeRva, &codeSize))
    {
        const unsigned char* p = reinterpret_cast<const unsigned char*>(Rva(codeRva));
        for (uint32_t i = 0; i + 5 <= codeSize; ++i)
        {
            if (p[i] != 0xE9 && p[i] != 0xE8)
                continue;
            int32_t rel = 0;
            memcpy(&rel, p + i + 1, 4);
            if (static_cast<uint32_t>(static_cast<int64_t>(codeRva + i) + 5 + rel) != fn)
                continue;
            if (p[i] == 0xE8)
            {
                ++direct;
                continue;
            }
            if ((i >= 5 && p[i - 5] == 0xE9) || (i + 10 <= codeSize && p[i + 5] == 0xE9))
            {
                ++thunks;
                thunk = codeRva + i;
            }
        }
    }
    *thunksOut = thunks;
    *directOut = direct;
    return thunks == 1 ? thunk : 0;
}

// the jump thunk at thunkRva now jumps to `to`; false if it could not be written
bool LinkThunkMove(uint32_t thunkRva, const void* to)
{
    unsigned char* const at = reinterpret_cast<unsigned char*>(Rva(thunkRva));
    DWORD old = 0;
    if (!VirtualProtect(at + 1, 4, PAGE_EXECUTE_READWRITE, &old))
        return false;
    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(to) - (reinterpret_cast<uintptr_t>(at) + 5));
    memcpy(at + 1, &rel, 4);
    DWORD ignored = 0;
    VirtualProtect(at + 1, 4, old, &ignored);
    FlushInstructionCache(GetCurrentProcess(), at, 5);
    int32_t back = 0;
    memcpy(&back, at + 1, 4);
    return back == rel;
}

void GlyphPoolInstall()
{
    if (!g_gpWanted)
    {
        Log("[glyphs] [Display] GlyphPoolGrow=0 - the game's glyph pool keeps its own size (the scrambled text with 3 / 4 players can "
            "come back)");
        return;
    }
    uint32_t hits[4] = {};
    const int na = FindPattern(kGpAllocBody, hits, 4);
    const uint32_t alloc = na == 1 ? hits[0] : 0;
    const int np = FindPattern(kGpPopBody, hits, 4);
    const int nf = FindPattern(kGpFreeBody, hits, 4);
    const int ni = FindPattern(kGpInitBody, hits, 4);
    g_gpPoolGlobal = ni == 1 ? ReadU32(Rva(hits[0] + kGpInitGlobal)) : 0;
    int thunks = 0;
    int direct = 0;
    const uint32_t thunk = LinkThunkOf(alloc, &thunks, &direct);
    if (!alloc || np != 1 || nf != 1 || !g_gpPoolGlobal || !thunk || direct != 0)
    {
        Log("[glyphs] the glyph pool fix NOT installed: allocator %d hit(s) 0x%08X, pop %d, free %d, the pool's making %d (global 0x%08X), "
            "the allocator's jump thunks %d (need 1), direct calls %d (need 0) - the scrambled text with 3 / 4 players can come back",
            na, alloc, np, nf, ni, static_cast<unsigned>(g_gpPoolGlobal), thunks, direct);
        return;
    }
    g_gpAllocOrig = reinterpret_cast<GpAlloc_t>(Rva(alloc));
    g_gpOn = true; // (before the jump moves)
    if (!LinkThunkMove(thunk, reinterpret_cast<const void*>(&GpAllocWrap)))
    {
        g_gpOn = false;
        Log("[glyphs] the glyph pool fix NOT installed: its jump thunk rva 0x%08X could not be written", thunk);
        return;
    }
    const uintptr_t pool = ReadU32(g_gpPoolGlobal);
    Log("[glyphs] 21j-r13: THE GLYPH POOL GROWS (GlyphPoolGrow=1): the text chunk allocator rva 0x%08X goes through BF1GameExt (its jump "
        "thunk rva 0x%08X); the pool's global 0x%08X (pool 0x%08X%s, %lu chunk(s) free now) - when it runs low BF1GameExt adds %lu chunks "
        "(%lu letters) of its own, at most %d times; [glyphs] lines when it does and every 30 s",
        alloc, thunk, static_cast<unsigned>(g_gpPoolGlobal), static_cast<unsigned>(pool), pool ? "" : " - not made yet",
        static_cast<unsigned long>(pool ? GpFree(pool, 200000) : 0), static_cast<unsigned long>(kGpBlockChunks),
        static_cast<unsigned long>(kGpBlockChunks * 8), kGpMaxBlocks);
}

uintptr_t FollowJumps(uintptr_t f); // (below)

// 21j-r14: the glyph leak fix - RedGlyphCacheElement's cleanup through BF1GameExt (its single jump thunk), ReleaseGlyphs first
void GlyphLeakInstall()
{
    if (!g_glWanted)
    {
        Log("[glyphs] [Display] GlyphLeakFix=0 - a deleted text keeps its letters' chunks out of the glyph pool (the game's own "
            "destructor; the pool runs dry sooner or later)");
        return;
    }
    uint32_t hits[4] = {};
    const int nr = FindPattern(kGlReleaseBody, hits, 4);
    const uint32_t release = nr == 1 ? hits[0] : 0;
    const int nc = FindPattern(kGlCleanupBody, hits, 4);
    const uint32_t cleanup = nc == 1 ? hits[0] : 0;
    g_glPoolGlobal = release ? ReadU32(Rva(release + kGlReleaseGlobal)) : 0;
    // ReleaseGlyphs' call goes to the free-list function (through its thunk); its global is the one the pool's making writes
    const uintptr_t callAt = release ? Rva(release + kGlReleaseFreeCall) : 0;
    int32_t callRel = 0;
    if (callAt)
        memcpy(&callRel, reinterpret_cast<const void*>(callAt + 1), sizeof(callRel));
    const bool callOk = callAt && *reinterpret_cast<const unsigned char*>(callAt) == 0xE8;
    const uintptr_t freeList = callOk ? FollowJumps(static_cast<uintptr_t>(static_cast<intptr_t>(callAt) + 5 + callRel)) : 0;
    const bool freeListOk = freeList && MatchPatternAt(static_cast<uint32_t>(freeList - ExeBase()), kGlFreeListBody);
    const bool globalOk = g_glPoolGlobal && (!g_gpPoolGlobal || g_glPoolGlobal == g_gpPoolGlobal);
    int thunks = 0;
    int direct = 0;
    const uint32_t thunk = LinkThunkOf(cleanup, &thunks, &direct);
    if (!release || !cleanup || !freeListOk || !globalOk || !thunk || direct != 0)
    {
        Log("[glyphs] the glyph leak fix NOT installed: ReleaseGlyphs %d hit(s) 0x%08X (its free-list call %s), the destructor's cleanup "
            "%d hit(s) 0x%08X, the pool's global 0x%08X %s, the cleanup's jump thunks %d (need 1), direct calls %d (need 0) - deleted "
            "texts keep leaking their chunks",
            nr, release, freeListOk ? "OK" : "does NOT match", nc, cleanup, static_cast<unsigned>(g_glPoolGlobal),
            globalOk ? "OK" : "NOT the pool's making's", thunks, direct);
        g_glPoolGlobal = 0;
        return;
    }
    g_glRelease = reinterpret_cast<GlNoArg_t>(Rva(release));
    g_glCleanupOrig = reinterpret_cast<GlNoArg_t>(Rva(cleanup));
    g_glOn = true; // (before the jump moves)
    if (!LinkThunkMove(thunk, reinterpret_cast<const void*>(&GlCleanupWrap)))
    {
        g_glOn = false;
        Log("[glyphs] the glyph leak fix NOT installed: the cleanup's jump thunk rva 0x%08X could not be written", thunk);
        return;
    }
    Log("[glyphs] 21j-r14: THE GLYPH LEAK FIXED (GlyphLeakFix=1): a deleted text's cleanup (RedGlyphCacheElement rva 0x%08X, its jump "
        "thunk rva 0x%08X) goes through BF1GameExt - the game's own ReleaseGlyphs (rva 0x%08X, the free-list function rva 0x%08X, the "
        "pool's global 0x%08X) gives its chunks back to the pool first (the game's destructor dropped them); counted in the 30 s lines",
        cleanup, thunk, release, static_cast<unsigned>(freeList - ExeBase()), static_cast<unsigned>(g_glPoolGlobal));
}

long __stdcall Hook_Present(void* dev, const RECT* a, const RECT* b, HWND w, const void* r)
{
    InterlockedIncrement(&g_calls[kHPresent]);
    VbFrame(); // 21j-r12: the text garble scan (a frame ends; its summary every 10 s; End = MARK)
    GpFrame(); // 21j-r13: the glyph pool every 30 s
    if (g_perfWanted) // 21j-r11: the frame-rate scan starts on the thread of the first Present (and follows the Presents)
    {
        if (!g_perfOn)
            PerfStart();
        else if (GetCurrentThreadId() != g_perfThread)
        {
            InterlockedIncrement(&g_perfOther);
            if (GetTickCount() - g_perfLastOwn >= 2000)
                PerfSwitch();
        }
    }
    const bool perfOurs = PerfEnterI(kPerfOurD3d); // 21j-r11
    const bool captured = g_capturing != 0;         // 21j-r11: the frame that ends here was captured (its lines go out here)
    StEnd("the frame's Present"); // 21j-r6
    if (!InterlockedExchange(&g_presentSeen, 1))
        Log("[frame] %lu ms: Present wrapper fires (frame boundaries OK)", ScanMsSinceAttach());
    if (g_capturing)
    {
        FlushDraws();
        InterlockedExchange(&g_capturing, 0);
        CaptureWrapEnd();
        SegPrint();
        Log("[frame] ---- Present: end of the captured frame ----");
    }
    else if (InterlockedExchange(&g_armed, 0))
    {
        ++g_frames;
        Log("[frame] ---- capture %d begins (next frame) ----", g_frames);
        g_firstDumped[0] = false;
        g_firstDumped[1] = false;
        g_dumpsThisCapture = 0;
        g_dumpedCount = 0;
        SegReset();
        CaptureWrapBegin(dev);
        InterlockedExchange(&g_capturing, 1);
    }
    ULONG64 cycBefore = 0;
    if (perfOurs)
    {
        if (!g_perfPresentInRenderLogged && g_tscPerMs > 0.0 && LocalPlayersNow() >= 3) // 21j-r11: once, in a 3 / 4-player match
        {
            g_perfPresentInRenderLogged = true;
            char open[200];
            int pos = 0;
            open[0] = 0;
            for (int i = 0; i <= g_pDepth && pos >= 0 && pos < static_cast<int>(sizeof(open)) - 48; ++i)
                pos += snprintf(open + pos, sizeof(open) - static_cast<size_t>(pos), "%s%s", i ? " > " : "", kPerfNames[g_pStack[i]]);
            Log("[perf] %lu ms: Present is called %s the game's render frame (the parts open at it: %s)", ScanMsSinceAttach(),
                PerfOnStack(kPerfRender) ? "INSIDE" : "after", open);
        }
        QueryThreadCycleTime(GetCurrentThread(), &cycBefore);
    }
    WsPresent(); // 21j-r34: the water picture scan (a shot frame ends / begins)
    const bool perfWait = PerfEnterI(kPerfPresentWait); // 21j-r11
    const long result = g_present(dev, a, b, w, r);
    PerfLeaveI(perfWait);
    if (perfOurs)
        PerfFrameEnd(cycBefore, captured); // 21j-r11: the frame is done
    if (g_frameClearFix && LocalPlayersNow() >= 2)
        FullViewportForNextFrame(dev);
    PerfLeaveI(perfOurs);
    return result;
}

long __stdcall Hook_StretchRect(void* dev, void* src, const RECT* sr, void* dst, const RECT* dr,
                                unsigned filter)
{
    InterlockedIncrement(&g_calls[kHStretchRect]);
    PerfPart perfOurs(kPerfOurD3d); // 21j-r11: this wrapper's own code
    MarkScreenTexture(dst, true);
    StCopy(dev, src, sr, dst, dr); // 21j-r6
    WtCopy(dst);                   // 21j-r33: the water scan (a refraction copy's target)
    if (g_capturing && Budget())
    {
        FlushDraws();
        char c[80], s[48], d[48], srt[64], drt[64];
        ChainText(c, sizeof(c));
        SurfaceSize(src, s, sizeof(s));
        SurfaceSize(dst, d, sizeof(d));
        RectText(sr, srt, sizeof(srt));
        RectText(dr, drt, sizeof(drt));
        Log("[frame] StretchRect %s %s -> %s %s filter %u  from %s", s, srt, d, drt, filter, c);
    }
    const bool perfIn = PerfEnterI(kPerfD3dOther); // 21j-r11: Direct3D's own work
    const long hr = g_stretchRect(dev, src, sr, dst, dr, filter);
    PerfLeaveI(perfIn);
    return hr;
}

long __stdcall Hook_SetRenderTarget(void* dev, DWORD index, void* surf)
{
    InterlockedIncrement(&g_calls[kHSetRenderTarget]);
    PerfPart perfOurs(kPerfOurD3d); // 21j-r11: this wrapper's own code
    MarkScreenTexture(surf, false);
    if (index == 0)
        g_wtRtDirty = true; // 21j-r33: the water scan looks at the render target again at the next draw
    if (index == 0 && surf)
    {
        SurfaceDesc9 d;
        memset(&d, 0, sizeof(d));
        void** svt = *reinterpret_cast<void***>(surf);
        if (reinterpret_cast<GetDesc_t>(svt[12])(surf, &d) >= 0)
        {
            g_rtW = d.Width;
            g_rtH = d.Height;
        }
    }
    if (g_capturing && Budget())
    {
        FlushDraws();
        char c[80], s[48];
        ChainText(c, sizeof(c));
        SurfaceSize(surf, s, sizeof(s));
        Log("[frame] SetRenderTarget %lu = %s  from %s", static_cast<unsigned long>(index), s, c);
    }
    if (g_stOn && StLine()) // 21j-r6
    {
        char s[48], t[80], c[80];
        SurfaceSize(surf, s, sizeof(s));
        StTexText(TextureOf(surf), t, sizeof(t));
        ChainText(c, sizeof(c));
        Log("[scope] trace %d: SetRenderTarget %lu = %s (its texture %s) from %s", g_stId, static_cast<unsigned long>(index), s, t, c);
    }
    const bool perfIn = PerfEnterI(kPerfD3dOther); // 21j-r11: Direct3D's own work
    const long hr = g_setRenderTarget(dev, index, surf);
    PerfLeaveI(perfIn);
    return hr;
}

long __stdcall Hook_Clear(void* dev, DWORD count, const void* rects, DWORD flags, DWORD color,
                          float z, DWORD stencil)
{
    InterlockedIncrement(&g_calls[kHClear]);
    PerfPart perfOurs(kPerfOurD3d); // 21j-r11: this wrapper's own code
    if (g_capturing && Budget())
    {
        FlushDraws();
        char c[80];
        ChainText(c, sizeof(c));
        Log("[frame] Clear %lu rect(s) flags 0x%lX color 0x%08lX  from %s",
            static_cast<unsigned long>(count), static_cast<unsigned long>(flags),
            static_cast<unsigned long>(color), c);
    }
    if (g_stOn && StLine()) // 21j-r6
    {
        char st[200];
        StState(dev, st, sizeof(st), nullptr);
        Log("[scope] trace %d: Clear %lu rect(s) flags 0x%lX colour 0x%08lX; %s", g_stId, static_cast<unsigned long>(count),
            static_cast<unsigned long>(flags), static_cast<unsigned long>(color), st);
    }
    const bool perfIn = PerfEnterI(kPerfD3dOther); // 21j-r11: Direct3D's own work
    const long hr = g_clear(dev, count, rects, flags, color, z, stencil);
    PerfLeaveI(perfIn);
    return hr;
}

long __stdcall Hook_SetViewport(void* dev, const Viewport9* vp)
{
    InterlockedIncrement(&g_calls[kHSetViewport]);
    PerfPart perfOurs(kPerfOurD3d); // 21j-r11: this wrapper's own code
    if (vp)
    {
        g_vpX = vp->X;
        g_vpY = vp->Y;
        g_vpW = vp->Width;
        g_vpH = vp->Height;
        // a split-screen viewport (clearly smaller than the target in one direction)
        if (g_rtW && g_rtH && (vp->Width * 4 < g_rtW * 3 || vp->Height * 4 < g_rtH * 3) &&
            vp->Width * 3 > g_rtW / 4 && vp->Height * 3 > g_rtH / 4)
            g_view = (vp->X > 0 || vp->Y > 0) ? 1 : 0;
    }
    if (g_capturing && vp)
        SegReset(); // 13e: only the draws after the frame's last viewport change are kept
    if (g_capturing && vp && Budget())
    {
        FlushDraws();
        char c[80];
        ChainText(c, sizeof(c));
        Log("[frame] SetViewport x %lu y %lu w %lu h %lu  from %s", static_cast<unsigned long>(vp->X),
            static_cast<unsigned long>(vp->Y), static_cast<unsigned long>(vp->Width),
            static_cast<unsigned long>(vp->Height), c);
    }
    if (g_stOn && vp && StLine()) // 21j-r6
    {
        char c[80];
        ChainText(c, sizeof(c));
        Log("[scope] trace %d: SetViewport x %lu y %lu w %lu h %lu from %s", g_stId, static_cast<unsigned long>(vp->X),
            static_cast<unsigned long>(vp->Y), static_cast<unsigned long>(vp->Width), static_cast<unsigned long>(vp->Height), c);
    }
    const bool perfIn = PerfEnterI(kPerfD3dOther); // 21j-r11: Direct3D's own work
    const long hr = g_setViewport(dev, vp);
    PerfLeaveI(perfIn);
    return hr;
}

long __stdcall Hook_SetScissorRect(void* dev, const RECT* r)
{
    InterlockedIncrement(&g_calls[kHSetScissorRect]);
    PerfPart perfOurs(kPerfOurD3d); // 21j-r11: this wrapper's own code
    if (g_capturing && Budget())
    {
        FlushDraws();
        char c[80], rt[64];
        ChainText(c, sizeof(c));
        RectText(r, rt, sizeof(rt));
        Log("[frame] SetScissorRect %s  from %s", rt, c);
    }
    const bool perfIn = PerfEnterI(kPerfD3dOther); // 21j-r11: Direct3D's own work
    const long hr = g_setScissorRect(dev, r);
    PerfLeaveI(perfIn);
    return hr;
}

// At a draw with the stencil on (a shadow draw): does the game's copy of the
// shadow-critical states still match Direct3D? Logged continuously (30 max).
void CheckStateCopy()
{
    if (!g_rsCopy || g_mismatchLogged >= 30)
        return;
    if (!g_rsKnown[kRsStencilEnable] || !g_rsDevice[kRsStencilEnable])
        return;
    const DWORD states[] = { kRsColorWrite, kRsStencilEnable, kRsZWrite, kRsAlphaBlend };
    for (DWORD st : states)
    {
        if (!g_rsKnown[st])
            continue;
        const DWORD copy = ReadU32(g_rsCopy + st * 4);
        if (copy == g_rsDevice[st])
            continue;
        uint32_t c[2] = {};
        GameCallers(c, 2);
        ++g_mismatchLogged;
        Log("[shadow] %lu ms: OUT OF SYNC at a stencil draw by rva 0x%08X < 0x%08X: render state %lu - "
            "game's copy 0x%lX, Direct3D 0x%lX%s",
            ScanMsSinceAttach(), static_cast<unsigned>(c[0]), static_cast<unsigned>(c[1]),
            static_cast<unsigned long>(st), static_cast<unsigned long>(copy),
            static_cast<unsigned long>(g_rsDevice[st]),
            st == kRsColorWrite && g_rsDevice[st] ? " (shadow volume drawn WITH colour)" : "");
        if (g_mismatchLogged == 30)
            Log("[shadow] 30 out-of-sync draws logged, going quiet");
        return;
    }
}

long __stdcall Hook_SetRenderState(void* dev, DWORD state, DWORD value)
{
    InterlockedIncrement(&g_calls[kHSetRenderState]);
    if (state < 256)
    {
        g_rsDevice[state] = value;
        g_rsKnown[state] = true;
    }
    const uintptr_t ra = reinterpret_cast<uintptr_t>(BF1_RETURN_ADDRESS());
    if (g_rsWrapperReturn && ra != g_rsWrapperReturn)
    {
        // set on the device directly: the game's copy does not know about it
        ++g_directTotal;
        const uint32_t caller = InExeCode(ra) ? static_cast<uint32_t>(ra - ExeBase()) : 0;
        bool seen = false;
        for (int i = 0; i < g_directSeenCount; ++i)
        {
            if (g_directSeen[i].caller == caller && g_directSeen[i].state == state)
            {
                seen = true;
                break;
            }
        }
        if (!seen && g_directSeenCount < 48)
        {
            g_directSeen[g_directSeenCount].caller = caller;
            g_directSeen[g_directSeenCount].state = state;
            ++g_directSeenCount;
            Log("[shadow] %lu ms: render state %lu = 0x%lX set on Direct3D DIRECTLY (not through the "
                "game's copy) from %s 0x%08X%s",
                ScanMsSinceAttach(), static_cast<unsigned long>(state),
                static_cast<unsigned long>(value), caller ? "rva" : "outside the exe at",
                caller ? static_cast<unsigned>(caller) : static_cast<unsigned>(ra),
                state == kRsColorWrite ? "  <- colour writes" : "");
        }
        if (g_cacheSync && g_rsCopy && state < 256)
        {
            uint32_t v = value;
            memcpy(reinterpret_cast<void*>(g_rsCopy + state * 4), &v, sizeof(v));
        }
    }
    return g_setRenderState(dev, state, value);
}

void DumpRegs(const char* which, float (*regs)[4], unsigned count)
{
    char line[600];
    int pos = 0;
    int inLine = 0;
    for (unsigned r = 0; r < count; ++r)
    {
        const float* v = regs[r];
        if (v[0] == 0.0f && v[1] == 0.0f && v[2] == 0.0f && v[3] == 0.0f)
            continue;
        if (inLine == 0)
            pos = snprintf(line, sizeof(line), "[cdump]       %s", which);
        pos += snprintf(line + pos, sizeof(line) - static_cast<size_t>(pos), " c%u(%.4g %.4g %.4g %.4g)", r,
                        static_cast<double>(v[0]), static_cast<double>(v[1]), static_cast<double>(v[2]),
                        static_cast<double>(v[3]));
        if (++inLine == 3 || pos > 480)
        {
            if (Budget())
                Log("%s", line);
            inLine = 0;
        }
    }
    if (inLine && Budget())
        Log("%s", line);
}

// Everything the GPU uses for this draw: who, which view, target, viewport,
// shaders, textures, blend state and the constants.
void DumpDraw(const char* why)
{
    if (g_dumpsThisCapture >= 48 || !Budget())
        return;
    ++g_dumpsThisCapture;
    uint32_t c[2] = {};
    GameCallers(c, 2);
    const int type = TypeOnStack();
    const int screen = ScreenTextureBound();
    Log("[cdump] %s: %s view, draw by rva 0x%08X < 0x%08X%s%s%s - target %ux%u, viewport %lu,%lu %lux%lu, "
        "vs 0x%08X ps 0x%08X, tex0 0x%08X tex1 0x%08X tex2 0x%08X%s, blend %lu src %lu dst %lu, colour "
        "mask 0x%lX, z %lu/%lu",
        why, g_view == 0 ? "PLAYER 1" : g_view == 1 ? "PLAYER 2" : "no", static_cast<unsigned>(c[0]),
        static_cast<unsigned>(c[1]), type >= 0 ? " [" : "", type >= 0 ? kTypes[type].label : "",
        type >= 0 ? "]" : "", g_rtW, g_rtH, static_cast<unsigned long>(g_vpX),
        static_cast<unsigned long>(g_vpY), static_cast<unsigned long>(g_vpW),
        static_cast<unsigned long>(g_vpH), static_cast<unsigned>(reinterpret_cast<uintptr_t>(g_vs)),
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(g_ps)),
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(g_bound[0])),
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(g_bound[1])),
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(g_bound[2])),
        screen >= 0 ? " (a screen texture)" : "",
        static_cast<unsigned long>(g_rsKnown[kRsAlphaBlend] ? g_rsDevice[kRsAlphaBlend] : 0),
        static_cast<unsigned long>(g_rsKnown[19] ? g_rsDevice[19] : 0),
        static_cast<unsigned long>(g_rsKnown[20] ? g_rsDevice[20] : 0),
        static_cast<unsigned long>(g_rsKnown[kRsColorWrite] ? g_rsDevice[kRsColorWrite] : 0xF),
        static_cast<unsigned long>(g_rsKnown[7] ? g_rsDevice[7] : 1),
        static_cast<unsigned long>(g_rsKnown[kRsZWrite] ? g_rsDevice[kRsZWrite] : 1));
    if (g_rsKnown[kRsStencilEnable] && g_rsDevice[kRsStencilEnable] && Budget())
        Log("[cdump]       stencil: func %lu ref 0x%lX mask 0x%lX writemask 0x%lX, fail %lu zfail %lu pass %lu, "
            "two-sided %lu (ccw fail %lu zfail %lu pass %lu func %lu), cull %lu, zfunc %lu",
            static_cast<unsigned long>(g_rsDevice[56]), static_cast<unsigned long>(g_rsDevice[57]),
            static_cast<unsigned long>(g_rsDevice[58]), static_cast<unsigned long>(g_rsDevice[59]),
            static_cast<unsigned long>(g_rsDevice[53]), static_cast<unsigned long>(g_rsDevice[54]),
            static_cast<unsigned long>(g_rsDevice[55]), static_cast<unsigned long>(g_rsDevice[185]),
            static_cast<unsigned long>(g_rsDevice[186]), static_cast<unsigned long>(g_rsDevice[187]),
            static_cast<unsigned long>(g_rsDevice[188]), static_cast<unsigned long>(g_rsDevice[189]),
            static_cast<unsigned long>(g_rsDevice[22]), static_cast<unsigned long>(g_rsDevice[23]));
    DumpRegs("vs", g_vsConst, 32);
    DumpRegs("ps", g_psConst, 16);
}

// During a capture: dump shadow and blur draws, and the first ordinary draw of
// each player's view (for its camera numbers).
void MaybeDump()
{
    if (!g_capturing)
        return;
    // 12g: one dump per (view, game function, stencil on/off): every stencil-on
    // group, every shadow / blur group, in both views - no repeats eating the budget
    const bool stencil = g_rsKnown[kRsStencilEnable] && g_rsDevice[kRsStencilEnable];
    const int type = TypeOnStack();
    const bool labelled = type >= 0 && (strcmp(kTypes[type].label, "shadows") == 0 ||
                                        strcmp(kTypes[type].label, "blur") == 0);
    if (stencil || labelled)
    {
        uint32_t c[2] = {};
        GameCallers(c, 2);
        for (int i = 0; i < g_dumpedCount; ++i)
        {
            if (g_dumped[i].view == g_view && g_dumped[i].caller == c[1] && g_dumped[i].stencil == stencil)
                return;
        }
        if (g_dumpedCount < 64)
        {
            g_dumped[g_dumpedCount].view = g_view;
            g_dumped[g_dumpedCount].caller = c[1];
            g_dumped[g_dumpedCount].stencil = stencil;
            ++g_dumpedCount;
        }
        DumpDraw(stencil ? (labelled ? "STENCIL + label" : "STENCIL") : kTypes[type].label);
        return;
    }
    if (g_view >= 0 && g_view <= 1 && !g_firstDumped[g_view] && g_vpW < g_rtW)
    {
        g_firstDumped[g_view] = true;
        DumpDraw("first ordinary draw");
    }
}

long __stdcall Hook_SetVertexShader(void* dev, void* shader)
{
    InterlockedIncrement(&g_calls[kHSetVertexShader]);
    g_vs = shader;
    return g_setVertexShader(dev, shader);
}

long __stdcall Hook_SetPixelShader(void* dev, void* shader)
{
    InterlockedIncrement(&g_calls[kHSetPixelShader]);
    g_ps = shader;
    return g_setPixelShader(dev, shader);
}

long __stdcall Hook_SetVsConstF(void* dev, unsigned start, const float* data, unsigned count)
{
    InterlockedIncrement(&g_calls[kHSetVsConstF]);
    if (data && start < kConstRegs)
    {
        const unsigned n = (start + count > kConstRegs) ? kConstRegs - start : count;
        memcpy(g_vsConst[start], data, n * 4 * sizeof(float));
    }
    return g_setVsConstF(dev, start, data, count);
}

long __stdcall Hook_SetPsConstF(void* dev, unsigned start, const float* data, unsigned count)
{
    InterlockedIncrement(&g_calls[kHSetPsConstF]);
    if (data && start < kConstRegs)
    {
        const unsigned n = (start + count > kConstRegs) ? kConstRegs - start : count;
        memcpy(g_psConst[start], data, n * 4 * sizeof(float));
    }
    return g_setPsConstF(dev, start, data, count);
}

long __stdcall Hook_SetTexture(void* dev, DWORD stage, void* tex)
{
    InterlockedIncrement(&g_calls[kHSetTexture]);
    if (stage < 8)
        g_bound[stage] = tex;
    return g_setTexture(dev, stage, tex);
}

long __stdcall Hook_DrawPrimitive(void* dev, unsigned type, unsigned start, unsigned count)
{
    InterlockedIncrement(&g_calls[kHDrawPrimitive]);
    PerfPart perfOurs(kPerfOurD3d); // 21j-r11: this wrapper's own code
    VbStreamCheck(dev);             // 21j-r12: stream 0 against the game's stream cache
    StDraw(dev, "DrawPrimitive", type, count); // 21j-r6
    WtDraw(dev);                   // 21j-r33: the water scan
    CheckStateCopy();
    MaybeDump();
    if (SkipThisDraw())
        return 0;
    if (g_capturing)
        NoteDraw();
    const bool perfIn = PerfEnterI(kPerfD3dDraw); // 21j-r11: Direct3D's own work
    const long hr = g_drawPrimitive(dev, type, start, count);
    PerfLeaveI(perfIn);
    return hr;
}

long __stdcall Hook_DrawIndexedPrimitive(void* dev, unsigned type, int baseVertex, unsigned minIndex,
                                         unsigned numVertices, unsigned startIndex, unsigned count)
{
    InterlockedIncrement(&g_calls[kHDrawIndexedPrimitive]);
    PerfPart perfOurs(kPerfOurD3d); // 21j-r11: this wrapper's own code
    VbStreamCheck(dev);             // 21j-r12: stream 0 against the game's stream cache
    StDraw(dev, "DrawIndexedPrimitive", type, count); // 21j-r6
    WtDraw(dev);                   // 21j-r33: the water scan
    CheckStateCopy();
    MaybeDump();
    if (SkipThisDraw())
        return 0;
    if (g_capturing)
        NoteDraw();
    const bool perfIn = PerfEnterI(kPerfD3dDraw); // 21j-r11: Direct3D's own work
    const long hr = g_drawIndexedPrimitive(dev, type, baseVertex, minIndex, numVertices, startIndex, count);
    PerfLeaveI(perfIn);
    return hr;
}

long __stdcall Hook_DrawPrimitiveUP(void* dev, unsigned type, unsigned count, const void* data,
                                    unsigned stride)
{
    InterlockedIncrement(&g_calls[kHDrawPrimitiveUP]);
    PerfPart perfOurs(kPerfOurD3d); // 21j-r11: this wrapper's own code
    if (g_vbOn && GetCurrentThreadId() == g_vbThread) // 21j-r12: (Direct3D empties stream 0 after it)
        ++g_vbWin.upDraws;
    StDraw(dev, "DrawPrimitiveUP", type, count); // 21j-r6
    WtDraw(dev);                   // 21j-r33: the water scan
    CheckStateCopy();
    MaybeDump();
    if (SkipThisDraw())
        return 0;
    if (g_capturing)
        NoteDraw();
    const bool perfIn = PerfEnterI(kPerfD3dDraw); // 21j-r11: Direct3D's own work
    const long hr = g_drawPrimitiveUP(dev, type, count, data, stride);
    PerfLeaveI(perfIn);
    return hr;
}

long __stdcall Hook_DrawIndexedPrimitiveUP(void* dev, unsigned type, unsigned minIndex,
                                           unsigned numVertices, unsigned count,
                                           const void* indexData, unsigned indexFormat,
                                           const void* vertexData, unsigned stride)
{
    InterlockedIncrement(&g_calls[kHDrawIndexedPrimitiveUP]);
    PerfPart perfOurs(kPerfOurD3d); // 21j-r11: this wrapper's own code
    if (g_vbOn && GetCurrentThreadId() == g_vbThread) // 21j-r12: (Direct3D empties stream 0 after it)
        ++g_vbWin.upDraws;
    StDraw(dev, "DrawIndexedPrimitiveUP", type, count); // 21j-r6
    WtDraw(dev);                   // 21j-r33: the water scan
    CheckStateCopy();
    MaybeDump();
    if (SkipThisDraw())
        return 0;
    if (g_capturing)
        NoteDraw();
    const bool perfIn = PerfEnterI(kPerfD3dDraw); // 21j-r11: Direct3D's own work
    const long hr = g_drawIndexedPrimitiveUP(dev, type, minIndex, numVertices, count, indexData, indexFormat, vertexData,
                                             stride);
    PerfLeaveI(perfIn);
    return hr;
}

// ---- RTTI: type name -> vtables -> virtual method ranges -------------------
struct Span
{
    uintptr_t start;
    uint32_t size;
};

int DataSpans(Span* out, int max)
{
    uint32_t codeRva = 0;
    uint32_t codeSize = 0;
    CodeSection(&codeRva, &codeSize);
    const uintptr_t base = ExeBase();
    const IMAGE_DOS_HEADER* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    const IMAGE_NT_HEADERS32* nt =
        reinterpret_cast<const IMAGE_NT_HEADERS32*>(base + static_cast<uintptr_t>(dos->e_lfanew));
    const IMAGE_SECTION_HEADER* sec = IMAGE_FIRST_SECTION(nt);
    int n = 0;
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections && n < max; ++i)
    {
        if (!(sec[i].Characteristics & IMAGE_SCN_MEM_READ) || sec[i].VirtualAddress == codeRva)
            continue;
        uint32_t len = sec[i].Misc.VirtualSize;
        if (sec[i].SizeOfRawData && sec[i].SizeOfRawData < len)
            len = sec[i].SizeOfRawData;
        if (len < 16)
            continue;
        MEMORY_BASIC_INFORMATION mbi;
        const uintptr_t start = base + sec[i].VirtualAddress;
        if (!VirtualQuery(reinterpret_cast<LPCVOID>(start), &mbi, sizeof(mbi)) ||
            mbi.State != MEM_COMMIT || (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)))
            continue;
        out[n].start = start;
        out[n].size = len;
        ++n;
    }
    return n;
}

uintptr_t FindTypeDescriptor(const Span* spans, int n, const char* cls)
{
    char name[96];
    snprintf(name, sizeof(name), ".?AV%s@@", cls);
    const size_t len = strlen(name) + 1; // with its terminating zero
    for (int s = 0; s < n; ++s)
    {
        const unsigned char* p = reinterpret_cast<const unsigned char*>(spans[s].start);
        for (uint32_t off = 0; off + len <= spans[s].size; ++off)
        {
            if (p[off] == '.' && memcmp(p + off, name, len) == 0 && off >= 8)
                return spans[s].start + off - 8;
        }
    }
    return 0;
}

int FindVtables(const Span* spans, int n, uintptr_t td, uintptr_t* out, int max)
{
    int found = 0;
    for (int s = 0; s < n && found < max; ++s)
    {
        for (uint32_t off = 0; off + 20 <= spans[s].size && found < max; off += 4)
        {
            const uintptr_t col = spans[s].start + off;
            if (ReadU32(col + 12) != td || ReadU32(col) != 0)
                continue;
            // the vtable sits right after a pointer to its complete-object locator
            for (int s2 = 0; s2 < n && found < max; ++s2)
            {
                for (uint32_t o2 = 0; o2 + 8 <= spans[s2].size; o2 += 4)
                {
                    if (ReadU32(spans[s2].start + o2) == static_cast<uint32_t>(col))
                    {
                        out[found++] = spans[s2].start + o2 + 4;
                        break;
                    }
                }
            }
        }
    }
    return found;
}

uintptr_t FollowJumps(uintptr_t f)
{
    for (int hop = 0; hop < 8 && InExeCode(f + 8) && *reinterpret_cast<const unsigned char*>(f) == 0xE9;
         ++hop)
    {
        int32_t rel = 0;
        memcpy(&rel, reinterpret_cast<const void*>(f + 1), sizeof(rel));
        f = static_cast<uintptr_t>(static_cast<intptr_t>(f) + 5 + rel);
    }
    return f;
}

// A function runs until the int3 padding before the next one (capped).
uintptr_t FunctionEnd(uintptr_t start)
{
    const unsigned char* p = reinterpret_cast<const unsigned char*>(start);
    for (uint32_t i = 1; i < 0x4000; ++i)
    {
        if (!InExeCode(start + i + 8))
            return start + i;
        if (p[i] == 0xCC && p[i + 1] == 0xCC)
            return start + i;
    }
    return start + 0x4000;
}

void BuildEffectRanges()
{
    Span spans[16];
    const int n = DataSpans(spans, 16);
    MethodRange tmp[kMaxRanges];
    int count = 0;
    for (int t = 0; t < kTypeCount; ++t)
    {
        int classes = 0;
        for (int c = 0; c < 8 && kTypes[t].classes[c]; ++c)
        {
            const uintptr_t td = FindTypeDescriptor(spans, n, kTypes[t].classes[c]);
            if (!td)
                continue;
            uintptr_t vts[8] = {};
            const int nv = FindVtables(spans, n, td, vts, 8);
            if (nv)
                ++classes;
            for (int v = 0; v < nv; ++v)
            {
                for (int e = 0; e < 96 && count < kMaxRanges; ++e)
                {
                    const uintptr_t fn = ReadU32(vts[v] + static_cast<uintptr_t>(e) * 4);
                    if (!InExeCode(fn + 8))
                        break;
                    const uintptr_t start = FollowJumps(fn);
                    if (!InExeCode(start + 8))
                        continue;
                    tmp[count].start = start;
                    tmp[count].end = FunctionEnd(start);
                    tmp[count].type = t;
                    ++count;
                }
            }
        }
        Log("[frame] effect type \"%s\": %d of its classes found", kTypes[t].label, classes);
    }
    // keep each method once, and only if exactly one type uses it
    g_rangeCount = 0;
    for (int i = 0; i < count; ++i)
    {
        bool shared = false;
        bool dup = false;
        for (int j = 0; j < count; ++j)
        {
            if (j == i || tmp[j].start != tmp[i].start)
                continue;
            if (tmp[j].type != tmp[i].type)
                shared = true;
            else if (j < i)
                dup = true;
        }
        if (shared || dup || g_rangeCount >= kMaxRanges)
            continue;
        g_ranges[g_rangeCount++] = tmp[i];
    }
    // sort by start (insertion sort: a few hundred entries, once)
    for (int i = 1; i < g_rangeCount; ++i)
    {
        const MethodRange r = g_ranges[i];
        int j = i - 1;
        while (j >= 0 && g_ranges[j].start > r.start)
        {
            g_ranges[j + 1] = g_ranges[j];
            --j;
        }
        g_ranges[j + 1] = r;
    }
    for (int t = 0; t < kTypeCount; ++t)
        g_methodsPerType[t] = 0;
    for (int i = 0; i < g_rangeCount; ++i)
        ++g_methodsPerType[g_ranges[i].type];
    char line[400];
    int pos = snprintf(line, sizeof(line), "[frame] effect scanner: %d methods (unshared) -", g_rangeCount);
    for (int t = 0; t < kTypeCount && pos > 0 && pos < static_cast<int>(sizeof(line)) - 48; ++t)
        pos += snprintf(line + pos, sizeof(line) - static_cast<size_t>(pos), " %s %d;", kTypes[t].label,
                        g_methodsPerType[t]);
    Log("%s", line);
}

// ---- 12f: wrap the six state setters only around a captured frame ---------
typedef long(__stdcall* GetRenderState_t)(void*, DWORD, DWORD*);
typedef long(__stdcall* GetTexture_t)(void*, DWORD, void**);
typedef long(__stdcall* GetShader_t)(void*, void**);
typedef long(__stdcall* GetShaderConstantF_t)(void*, unsigned, float*, unsigned);
constexpr int kSlotGetRenderState = 58;
constexpr int kSlotGetTexture = 64;
constexpr int kSlotGetVertexShader = 93;
constexpr int kSlotGetVertexShaderConstantF = 95;
constexpr int kSlotGetPixelShader = 108;
constexpr int kSlotGetPixelShaderConstantF = 110;

struct CapSlot
{
    int slot;
    void* hook;
    void* saved;
    bool wrapped;
};
CapSlot g_capSlots[6];
void** g_capTable = nullptr;
int g_capReads = 0;

void ReleaseCom(void* obj)
{
    if (obj)
        reinterpret_cast<Release_t>((*reinterpret_cast<void***>(obj))[2])(obj);
}

void SetSlot(void** vt, int slot, void* value)
{
    MEMORY_BASIC_INFORMATION mbi;
    const DWORD writable = PAGE_READWRITE | PAGE_EXECUTE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_WRITECOPY;
    const bool needProtect =
        VirtualQuery(&vt[slot], &mbi, sizeof(mbi)) && !(mbi.Protect & writable);
    DWORD old = 0;
    if (needProtect)
        VirtualProtect(&vt[slot], sizeof(void*), PAGE_READWRITE, &old);
    InterlockedExchangePointer(&vt[slot], value);
    if (needProtect)
    {
        DWORD ignored = 0;
        VirtualProtect(&vt[slot], sizeof(void*), old, &ignored);
    }
}

// Read what the device has right now (Get* methods), so the captured frame
// starts from the true state rather than whatever was last seen.
void ReadDeviceState(void* dev)
{
    void** vt = *reinterpret_cast<void***>(dev);
    const GetRenderState_t getRs = reinterpret_cast<GetRenderState_t>(vt[kSlotGetRenderState]);
    const DWORD states[] = { 7, 14, 15, 19, 20, 22, 23, 27, 52, 53, 54, 55, 56, 57, 58, 59, 168, 185, 186, 187,
                             188, 189 };
    int rsOk = 0;
    for (DWORD st : states)
    {
        DWORD v = 0;
        if (getRs(dev, st, &v) >= 0)
        {
            g_rsDevice[st] = v;
            g_rsKnown[st] = true;
            ++rsOk;
        }
    }
    const bool vsOk = reinterpret_cast<GetShaderConstantF_t>(vt[kSlotGetVertexShaderConstantF])(
                          dev, 0, &g_vsConst[0][0], 32) >= 0;
    const bool psOk = reinterpret_cast<GetShaderConstantF_t>(vt[kSlotGetPixelShaderConstantF])(
                          dev, 0, &g_psConst[0][0], 16) >= 0;
    int texOk = 0;
    for (DWORD stage = 0; stage < 8; ++stage)
    {
        void* t = nullptr;
        if (reinterpret_cast<GetTexture_t>(vt[kSlotGetTexture])(dev, stage, &t) >= 0)
        {
            g_bound[stage] = t;
            ReleaseCom(t);
            ++texOk;
        }
    }
    void* sh = nullptr;
    if (reinterpret_cast<GetShader_t>(vt[kSlotGetVertexShader])(dev, &sh) >= 0)
    {
        g_vs = sh;
        ReleaseCom(sh);
    }
    sh = nullptr;
    if (reinterpret_cast<GetShader_t>(vt[kSlotGetPixelShader])(dev, &sh) >= 0)
    {
        g_ps = sh;
        ReleaseCom(sh);
    }
    if (g_capReads++ < 3)
        Log("[frame] device state read at capture start: %d of %u render states, vs constants %s, ps "
            "constants %s, %d of 8 texture stages",
            rsOk, static_cast<unsigned>(sizeof(states) / sizeof(states[0])), vsOk ? "OK" : "FAILED",
            psOk ? "OK" : "FAILED", texOk);
}

// Whatever Direct3D has in each state slot right now becomes the original, and
// our wrapper goes in - for this one frame.
void CaptureWrapBegin(void* dev)
{
    ReadDeviceState(dev);
    void** vt = *reinterpret_cast<void***>(dev);
    g_capTable = vt;
    const int slots[6] = { kSlotSetRenderState, kSlotSetTexture, kSlotSetVertexShader,
                           kSlotSetVertexShaderConstantF, kSlotSetPixelShader, kSlotSetPixelShaderConstantF };
    void* const hooks[6] = { reinterpret_cast<void*>(&Hook_SetRenderState),
                             reinterpret_cast<void*>(&Hook_SetTexture),
                             reinterpret_cast<void*>(&Hook_SetVertexShader),
                             reinterpret_cast<void*>(&Hook_SetVsConstF),
                             reinterpret_cast<void*>(&Hook_SetPixelShader),
                             reinterpret_cast<void*>(&Hook_SetPsConstF) };
    for (int i = 0; i < 6; ++i)
    {
        g_capSlots[i].slot = slots[i];
        g_capSlots[i].hook = hooks[i];
        g_capSlots[i].wrapped = false;
        void* cur = vt[slots[i]];
        if (!cur || cur == hooks[i])
            continue;
        g_capSlots[i].saved = cur;
        switch (i)
        {
        case 0: g_setRenderState = reinterpret_cast<SetRenderState_t>(cur); break;
        case 1: g_setTexture = reinterpret_cast<SetTexture_t>(cur); break;
        case 2: g_setVertexShader = reinterpret_cast<SetShader_t>(cur); break;
        case 3: g_setVsConstF = reinterpret_cast<SetShaderConstantF_t>(cur); break;
        case 4: g_setPixelShader = reinterpret_cast<SetShader_t>(cur); break;
        default: g_setPsConstF = reinterpret_cast<SetShaderConstantF_t>(cur); break;
        }
        SetSlot(vt, slots[i], hooks[i]);
        g_capSlots[i].wrapped = true;
    }
}

// Put Direct3D's own functions back (unless Direct3D already changed a slot).
void CaptureWrapEnd()
{
    if (!g_capTable)
        return;
    for (int i = 0; i < 6; ++i)
    {
        if (!g_capSlots[i].wrapped)
            continue;
        if (g_capTable[g_capSlots[i].slot] == g_capSlots[i].hook)
            SetSlot(g_capTable, g_capSlots[i].slot, g_capSlots[i].saved);
        g_capSlots[i].wrapped = false;
    }
}

// "d3d9.dll" etc. for an address, or "?" if it is in no module.
void ModuleOf(const void* addr, char* out, size_t outSize)
{
    HMODULE m = nullptr;
    char path[MAX_PATH] = {};
    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           static_cast<LPCSTR>(addr), &m) &&
        m && GetModuleFileNameA(m, path, MAX_PATH))
    {
        const char* slash = strrchr(path, '\\');
        snprintf(out, outSize, "%s", slash ? slash + 1 : path);
    }
    else
    {
        snprintf(out, outSize, "? (no module)");
    }
}

struct WatchedSlot
{
    int slot;
    int counter;
    const char* name;
    void* hook;
    void* lastSeen;
};
WatchedSlot g_watched[kHookCount];
int g_watchedCount = 0;

void Watch(int slot, int counter, const char* name, void* hook)
{
    if (g_watchedCount >= kHookCount)
        return;
    g_watched[g_watchedCount].slot = slot;
    g_watched[g_watchedCount].counter = counter;
    g_watched[g_watchedCount].name = name;
    g_watched[g_watchedCount].hook = hook;
    g_watched[g_watchedCount].lastSeen = hook;
    ++g_watchedCount;
}

DWORD WINAPI HookHealthThread(LPVOID)
{
    LONG last[kHookCount] = {};
    for (int round = 1;; ++round)
    {
        Sleep(5000);
        if (!g_vtable)
            continue;
        for (int i = 0; i < g_watchedCount; ++i)
        {
            void* cur = g_vtable[g_watched[i].slot];
            if (cur != g_watched[i].lastSeen)
            {
                char mod[MAX_PATH];
                ModuleOf(cur, mod, sizeof(mod));
                Log("[hooks] %lu ms: device slot %d (%s) now points to 0x%08X in %s - %s",
                    ScanMsSinceAttach(), g_watched[i].slot, g_watched[i].name,
                    static_cast<unsigned>(reinterpret_cast<uintptr_t>(cur)), mod,
                    cur == g_watched[i].hook ? "our wrapper again" : "SOMETHING REPLACED OUR WRAPPER");
                g_watched[i].lastSeen = cur;
            }
        }
        if (round <= 24 || round % 12 == 0)
        {
            LONG now[kHookCount];
            for (int i = 0; i < kHookCount; ++i)
                now[i] = g_calls[i];
            Log("[hooks] %lu ms: calls in the last 5 s - Present %ld, draws %ld, SetRenderTarget %ld, "
                "SetViewport %ld, StretchRect %ld, SetRenderState %ld, SetTexture %ld, SetVertexShader %ld, "
                "VS constants %ld, SetPixelShader %ld, PS constants %ld",
                ScanMsSinceAttach(), static_cast<long>(now[kHPresent] - last[kHPresent]),
                static_cast<long>(now[kHDrawPrimitive] + now[kHDrawIndexedPrimitive] +
                                  now[kHDrawPrimitiveUP] + now[kHDrawIndexedPrimitiveUP] -
                                  last[kHDrawPrimitive] - last[kHDrawIndexedPrimitive] -
                                  last[kHDrawPrimitiveUP] - last[kHDrawIndexedPrimitiveUP]),
                static_cast<long>(now[kHSetRenderTarget] - last[kHSetRenderTarget]),
                static_cast<long>(now[kHSetViewport] - last[kHSetViewport]),
                static_cast<long>(now[kHStretchRect] - last[kHStretchRect]),
                static_cast<long>(now[kHSetRenderState] - last[kHSetRenderState]),
                static_cast<long>(now[kHSetTexture] - last[kHSetTexture]),
                static_cast<long>(now[kHSetVertexShader] - last[kHSetVertexShader]),
                static_cast<long>(now[kHSetVsConstF] - last[kHSetVsConstF]),
                static_cast<long>(now[kHSetPixelShader] - last[kHSetPixelShader]),
                static_cast<long>(now[kHSetPsConstF] - last[kHSetPsConstF]));
            for (int i = 0; i < kHookCount; ++i)
                last[i] = now[i];
        }
    }
}

// 22c: the creates (slots 23 - 27), for the memory log
constexpr int kSlotCreateTexture = 23;
constexpr int kSlotCreateVolumeTexture = 24;
constexpr int kSlotCreateCubeTexture = 25;
constexpr int kSlotCreateVertexBuffer = 26;
constexpr int kSlotCreateIndexBuffer = 27;
typedef long(__stdcall* CreateTexture_t)(void*, unsigned, unsigned, unsigned, DWORD, DWORD, DWORD, void**, HANDLE*);
typedef long(__stdcall* CreateVolumeTexture_t)(void*, unsigned, unsigned, unsigned, unsigned, DWORD, DWORD, DWORD, void**,
                                               HANDLE*);
typedef long(__stdcall* CreateCubeTexture_t)(void*, unsigned, unsigned, DWORD, DWORD, DWORD, void**, HANDLE*);
typedef long(__stdcall* CreateBuffer_t)(void*, unsigned, DWORD, DWORD, DWORD, void**, HANDLE*);
CreateTexture_t g_createTexture = nullptr;
CreateVolumeTexture_t g_createVolumeTexture = nullptr;
CreateCubeTexture_t g_createCubeTexture = nullptr;
CreateBuffer_t g_createVertexBuffer = nullptr;
CreateBuffer_t g_createIndexBuffer = nullptr;

// 22c: a texture's bytes, all its levels (0 = the whole chain); the common formats, 4 bytes a texel for any other
unsigned long long TexBytes(unsigned w, unsigned h, unsigned d, unsigned levels, DWORD fmt)
{
    unsigned block = 0; // DXTn: bytes per 4 x 4 block
    unsigned bpp = 4;
    switch (fmt)
    {
    case 0x31545844u: block = 8; break; // DXT1
    case 0x32545844u: case 0x33545844u: case 0x34545844u: case 0x35545844u: block = 16; break; // DXT2 - DXT5
    case 20: bpp = 3; break; // R8G8B8
    case 23: case 24: case 25: case 26: case 29: case 30: case 40: case 51: case 60: case 80: case 111: bpp = 2; break;
    case 27: case 28: case 41: case 50: case 52: bpp = 1; break;
    case 113: bpp = 8; break;  // A16B16G16R16F
    case 116: bpp = 16; break; // A32B32G32R32F
    default: break;
    }
    unsigned long long total = 0;
    unsigned lw = w ? w : 1;
    unsigned lh = h ? h : 1;
    unsigned ld = d ? d : 1;
    for (unsigned l = 0; l < 16; ++l)
    {
        if (block)
            total += static_cast<unsigned long long>((lw + 3) / 4) * ((lh + 3) / 4) * block * ld;
        else
            total += static_cast<unsigned long long>(lw) * lh * ld * bpp;
        if ((levels && l + 1 >= levels) || (lw == 1 && lh == 1 && ld == 1))
            break;
        lw = lw > 1 ? lw / 2 : 1;
        lh = lh > 1 ? lh / 2 : 1;
        ld = ld > 1 ? ld / 2 : 1;
    }
    return total;
}

long __stdcall Hook_CreateTexture(void* dev, unsigned w, unsigned h, unsigned levels, DWORD usage, DWORD fmt, DWORD pool,
                                  void** out, HANDLE* shared)
{
    const long hr = g_createTexture(dev, w, h, levels, usage, fmt, pool, out, shared);
    if (hr < 0)
        MemLogD3dFailed(0, hr, w, h, levels, usage, fmt, pool, BF1_RETURN_ADDRESS());
    else
        MemLogD3dMade(0, pool, TexBytes(w, h, 1, levels, fmt));
    return hr;
}

long __stdcall Hook_CreateVolumeTexture(void* dev, unsigned w, unsigned h, unsigned d, unsigned levels, DWORD usage, DWORD fmt,
                                        DWORD pool, void** out, HANDLE* shared)
{
    const long hr = g_createVolumeTexture(dev, w, h, d, levels, usage, fmt, pool, out, shared);
    if (hr < 0)
        MemLogD3dFailed(1, hr, w, h, d, usage, fmt, pool, BF1_RETURN_ADDRESS());
    else
        MemLogD3dMade(1, pool, TexBytes(w, h, d, levels, fmt));
    return hr;
}

long __stdcall Hook_CreateCubeTexture(void* dev, unsigned edge, unsigned levels, DWORD usage, DWORD fmt, DWORD pool, void** out,
                                      HANDLE* shared)
{
    const long hr = g_createCubeTexture(dev, edge, levels, usage, fmt, pool, out, shared);
    if (hr < 0)
        MemLogD3dFailed(2, hr, edge, edge, levels, usage, fmt, pool, BF1_RETURN_ADDRESS());
    else
        MemLogD3dMade(2, pool, 6ull * TexBytes(edge, edge, 1, levels, fmt));
    return hr;
}

long __stdcall Hook_CreateVertexBuffer(void* dev, unsigned length, DWORD usage, DWORD fvf, DWORD pool, void** out, HANDLE* shared)
{
    const long hr = g_createVertexBuffer(dev, length, usage, fvf, pool, out, shared);
    if (hr < 0)
        MemLogD3dFailed(3, hr, length, 0, 0, usage, fvf, pool, BF1_RETURN_ADDRESS());
    else
        MemLogD3dMade(3, pool, length);
    return hr;
}

long __stdcall Hook_CreateIndexBuffer(void* dev, unsigned length, DWORD usage, DWORD fmt, DWORD pool, void** out, HANDLE* shared)
{
    const long hr = g_createIndexBuffer(dev, length, usage, fmt, pool, out, shared);
    if (hr < 0)
        MemLogD3dFailed(4, hr, length, 0, 0, usage, fmt, pool, BF1_RETURN_ADDRESS());
    else
        MemLogD3dMade(4, pool, length);
    return hr;
}

bool InstallHooks(void* device)
{
    void** vt = *reinterpret_cast<void***>(device);
    if (!vt)
        return false;
    DWORD oldProt = 0;
    const SIZE_T span = static_cast<SIZE_T>((kSlotSetPixelShaderConstantF + 1) * sizeof(void*));
    if (!VirtualProtect(vt, span, PAGE_READWRITE, &oldProt))
    {
        Log("[frame] VirtualProtect on the device vtable failed (error %lu) - capture disabled",
            GetLastError());
        return false;
    }
    g_present = reinterpret_cast<Present_t>(vt[kSlotPresent]);
    g_stretchRect = reinterpret_cast<StretchRect_t>(vt[kSlotStretchRect]);
    g_setRenderTarget = reinterpret_cast<SetRenderTarget_t>(vt[kSlotSetRenderTarget]);
    g_clear = reinterpret_cast<Clear_t>(vt[kSlotClear]);
    g_setViewport = reinterpret_cast<SetViewport_t>(vt[kSlotSetViewport]);
    g_setScissorRect = reinterpret_cast<SetScissorRect_t>(vt[kSlotSetScissorRect]);
    g_setTexture = reinterpret_cast<SetTexture_t>(vt[kSlotSetTexture]);
    g_setRenderState = reinterpret_cast<SetRenderState_t>(vt[kSlotSetRenderState]);
    g_setVertexShader = reinterpret_cast<SetShader_t>(vt[kSlotSetVertexShader]);
    g_setPixelShader = reinterpret_cast<SetShader_t>(vt[kSlotSetPixelShader]);
    g_setVsConstF = reinterpret_cast<SetShaderConstantF_t>(vt[kSlotSetVertexShaderConstantF]);
    g_setPsConstF = reinterpret_cast<SetShaderConstantF_t>(vt[kSlotSetPixelShaderConstantF]);
    g_drawPrimitive = reinterpret_cast<DrawPrimitive_t>(vt[kSlotDrawPrimitive]);
    g_drawIndexedPrimitive = reinterpret_cast<DrawIndexedPrimitive_t>(vt[kSlotDrawIndexedPrimitive]);
    g_drawPrimitiveUP = reinterpret_cast<DrawPrimitiveUP_t>(vt[kSlotDrawPrimitiveUP]);
    g_drawIndexedPrimitiveUP =
        reinterpret_cast<DrawIndexedPrimitiveUP_t>(vt[kSlotDrawIndexedPrimitiveUP]);
    InterlockedExchangePointer(&vt[kSlotPresent], reinterpret_cast<void*>(&Hook_Present));
    InterlockedExchangePointer(&vt[kSlotStretchRect], reinterpret_cast<void*>(&Hook_StretchRect));
    InterlockedExchangePointer(&vt[kSlotSetRenderTarget],
                               reinterpret_cast<void*>(&Hook_SetRenderTarget));
    InterlockedExchangePointer(&vt[kSlotClear], reinterpret_cast<void*>(&Hook_Clear));
    InterlockedExchangePointer(&vt[kSlotSetViewport], reinterpret_cast<void*>(&Hook_SetViewport));
    InterlockedExchangePointer(&vt[kSlotSetScissorRect],
                               reinterpret_cast<void*>(&Hook_SetScissorRect));
    // 12f: the six state setters (SetTexture, SetRenderState, SetVertexShader,
    // SetPixelShader, the two constant setters) are NOT wrapped here any more:
    // Direct3D swaps its own versions into this per-device table at runtime.
    // They are wrapped only around a captured frame (CaptureWrapBegin / End).
    InterlockedExchangePointer(&vt[kSlotDrawPrimitive], reinterpret_cast<void*>(&Hook_DrawPrimitive));
    InterlockedExchangePointer(&vt[kSlotDrawIndexedPrimitive],
                               reinterpret_cast<void*>(&Hook_DrawIndexedPrimitive));
    InterlockedExchangePointer(&vt[kSlotDrawPrimitiveUP],
                               reinterpret_cast<void*>(&Hook_DrawPrimitiveUP));
    InterlockedExchangePointer(&vt[kSlotDrawIndexedPrimitiveUP],
                               reinterpret_cast<void*>(&Hook_DrawIndexedPrimitiveUP));
    // 22c: the creates, for the memory log (once: a slot already ours is left as it is)
    if (vt[kSlotCreateTexture] != reinterpret_cast<void*>(&Hook_CreateTexture))
    {
        g_createTexture = reinterpret_cast<CreateTexture_t>(vt[kSlotCreateTexture]);
        g_createVolumeTexture = reinterpret_cast<CreateVolumeTexture_t>(vt[kSlotCreateVolumeTexture]);
        g_createCubeTexture = reinterpret_cast<CreateCubeTexture_t>(vt[kSlotCreateCubeTexture]);
        g_createVertexBuffer = reinterpret_cast<CreateBuffer_t>(vt[kSlotCreateVertexBuffer]);
        g_createIndexBuffer = reinterpret_cast<CreateBuffer_t>(vt[kSlotCreateIndexBuffer]);
        InterlockedExchangePointer(&vt[kSlotCreateTexture], reinterpret_cast<void*>(&Hook_CreateTexture));
        InterlockedExchangePointer(&vt[kSlotCreateVolumeTexture], reinterpret_cast<void*>(&Hook_CreateVolumeTexture));
        InterlockedExchangePointer(&vt[kSlotCreateCubeTexture], reinterpret_cast<void*>(&Hook_CreateCubeTexture));
        InterlockedExchangePointer(&vt[kSlotCreateVertexBuffer], reinterpret_cast<void*>(&Hook_CreateVertexBuffer));
        InterlockedExchangePointer(&vt[kSlotCreateIndexBuffer], reinterpret_cast<void*>(&Hook_CreateIndexBuffer));
    }
    DWORD ignored = 0;
    VirtualProtect(vt, span, oldProt, &ignored);

    // 12c: where the device, its method table and the original methods live
    g_vtable = vt;
    {
        char modDev[MAX_PATH], modVt[MAX_PATH], modRs[MAX_PATH], modVs[MAX_PATH], modTex[MAX_PATH],
            modDraw[MAX_PATH];
        ModuleOf(device, modDev, sizeof(modDev));
        ModuleOf(vt, modVt, sizeof(modVt));
        ModuleOf(reinterpret_cast<void*>(g_setRenderState), modRs, sizeof(modRs));
        ModuleOf(reinterpret_cast<void*>(g_setVsConstF), modVs, sizeof(modVs));
        ModuleOf(reinterpret_cast<void*>(g_setTexture), modTex, sizeof(modTex));
        ModuleOf(reinterpret_cast<void*>(g_drawIndexedPrimitive), modDraw, sizeof(modDraw));
        Log("[hooks] device object in %s, its method table in %s; original SetRenderState in %s, VS "
            "constants in %s, SetTexture in %s, DrawIndexedPrimitive in %s",
            modDev, modVt, modRs, modVs, modTex, modDraw);
    }
    Watch(kSlotPresent, kHPresent, "Present", reinterpret_cast<void*>(&Hook_Present));
    Watch(kSlotStretchRect, kHStretchRect, "StretchRect", reinterpret_cast<void*>(&Hook_StretchRect));
    Watch(kSlotSetRenderTarget, kHSetRenderTarget, "SetRenderTarget",
          reinterpret_cast<void*>(&Hook_SetRenderTarget));
    Watch(kSlotSetViewport, kHSetViewport, "SetViewport", reinterpret_cast<void*>(&Hook_SetViewport));
    Watch(kSlotDrawIndexedPrimitive, kHDrawIndexedPrimitive, "DrawIndexedPrimitive",
          reinterpret_cast<void*>(&Hook_DrawIndexedPrimitive));
    Watch(kSlotDrawPrimitiveUP, kHDrawPrimitiveUP, "DrawPrimitiveUP",
          reinterpret_cast<void*>(&Hook_DrawPrimitiveUP));
    const HANDLE health = CreateThread(nullptr, 0, HookHealthThread, nullptr, 0, nullptr);
    if (health)
        CloseHandle(health);
    return true;
}

// After the device is wrapped: arm the automatic captures once player 1 is in
// the world (SpawnManager character 0 has a unit).
void AutoCaptureLoop()
{
    DWORD inWorldSince = 0;
    int next = 0;
    while (next < kAutoCaptures)
    {
        Sleep(250);
        const uintptr_t ch = SpawnScanCharacter(0);
        const bool inWorld = ch && ReadU32(ch + 0xB4) != 0;
        if (!inWorldSince)
        {
            if (inWorld)
            {
                inWorldSince = GetTickCount();
                Log("[frame] %lu ms: player 1 is in the world - automatic captures in %lu, %lu and "
                    "%lu s",
                    ScanMsSinceAttach(), static_cast<unsigned long>(kAutoCaptureMs[0] / 1000),
                    static_cast<unsigned long>(kAutoCaptureMs[1] / 1000),
                    static_cast<unsigned long>(kAutoCaptureMs[2] / 1000));
            }
            continue;
        }
        if (GetTickCount() - inWorldSince >= kAutoCaptureMs[next])
        {
            if (!g_capturing)
            {
                InterlockedExchange(&g_armed, 1);
                Log("[frame] %lu ms: automatic capture %d of %d armed", ScanMsSinceAttach(), next + 1,
                    kAutoCaptures);
            }
            ++next;
        }
    }
}

DWORD WINAPI WaitForDeviceThread(LPVOID)
{
    for (int tries = 0; tries < 1200; ++tries) // up to 10 minutes
    {
        const uintptr_t dev = ReadU32(g_deviceGlobal);
        if (dev)
        {
            Sleep(500); // let the game finish setting the device up
            if (InstallHooks(reinterpret_cast<void*>(ReadU32(g_deviceGlobal))))
            {
                BuildEffectRanges();
                g_scannerReady = g_rangeCount > 0;
                {
                    const BuildInfo* b = CurrentBuild();
                    g_sourcesReady = b && b->name && strncmp(b->name, "Steam", 5) == 0;
                    Log("[frame] draw-group switches: %s",
                        g_sourcesReady ? "ready - Num 1-9 flip one draw group off / on, Num 0 all on"
                                       : "calibrated for the Steam build only - off");
                }
                Log("[frame] effect scanner: %s",
                    g_scannerReady ? "ready - captured frames label each group of draws with its "
                                     "effect type"
                                   : "no effect methods found - off");
                Log("[frame] %lu ms: device 0x%08X found, 16 methods wrapped (log-only; 22c: and its 5 creates, for the "
                    "memory log). Three frames "
                    "are captured by themselves once player 1 is in the world; Insert, Home or "
                    "LB+RB+Y captures one more.",
                    ScanMsSinceAttach(), static_cast<unsigned>(ReadU32(g_deviceGlobal)));
                AutoCaptureLoop();
            }
            return 0;
        }
        Sleep(500);
    }
    Log("[frame] the game's device never appeared - capture disabled");
    return 0;
}

} // namespace

void FrameCaptureStart()
{
    const BuildInfo* build = CurrentBuild();
    if (!build || build->loadsMemExt)
    {
        Log("[frame] not used on this build");
        return;
    }
    {
        char exe[MAX_PATH] = {};
        if (GetModuleFileNameA(nullptr, exe, MAX_PATH))
        {
            char* slash = strrchr(exe, '\\');
            if (slash)
            {
                *slash = 0;
                char ini[MAX_PATH + 32];
                snprintf(ini, sizeof(ini), "%s\\BF1GameExt.ini", exe);
                g_frameClearFix = GetPrivateProfileIntA("Display", "SplitFrameClearFix", 1, ini) != 0;
                g_perfWanted = GetPrivateProfileIntA("Display", "PerfScan", 1, ini) != 0; // 21j-r11
                const UINT spike = GetPrivateProfileIntA("Display", "PerfSpikeMs", 25, ini);
                g_perfSpikeMs = spike < 17 ? 17 : spike > 1000 ? 1000 : spike;
                g_vbWanted = GetPrivateProfileIntA("Display", "TextGarbleScan", 0, ini) != 0; // 21j-r12 (21j-r13: default 0)
                g_gpWanted = GetPrivateProfileIntA("Display", "GlyphPoolGrow", 1, ini) != 0;  // 21j-r13
                g_glWanted = GetPrivateProfileIntA("Display", "GlyphLeakFix", 1, ini) != 0;   // 21j-r14
            }
        }
        Log("[perf] BF1GameExt.ini [Display] PerfScan=%d PerfSpikeMs=%lu%s", g_perfWanted ? 1 : 0,
            static_cast<unsigned long>(g_perfSpikeMs),
            g_perfWanted ? ": 21j-r11 the frame-rate scan starts at the first Present ([perf] lines every 5 s, log only)"
                         : ": the frame-rate scan is off");
        Log("[clearfix] BF1GameExt.ini [Display] SplitFrameClearFix=%d%s", g_frameClearFix ? 1 : 0,
            g_frameClearFix ? ": in split screen the frame-start clear will cover both halves"
                            : ": off (player 1's half keeps missing the frame-start clear)");
    }
    uint32_t hits[4] = {};
    const int n = FindPattern(kCopySite, hits, 4);
    if (n < 1 || n > 4)
    {
        Log("[frame] screen-copy site: %d pattern hits - capture disabled", n);
        return;
    }
    uintptr_t global = 0;
    for (int i = 0; i < n; ++i)
    {
        const uintptr_t call = Rva(hits[i]);
        int32_t rel = 0;
        memcpy(&rel, reinterpret_cast<const void*>(call + 1), sizeof(rel));
        uintptr_t t = static_cast<uintptr_t>(static_cast<intptr_t>(call) + 5 + rel);
        for (int hop = 0; hop < 8 && *reinterpret_cast<const unsigned char*>(t) == 0xE9; ++hop)
        {
            memcpy(&rel, reinterpret_cast<const void*>(t + 1), sizeof(rel));
            t = static_cast<uintptr_t>(static_cast<intptr_t>(t) + 5 + rel);
        }
        if (!MatchPatternAt(static_cast<uint32_t>(t - ExeBase()), kGetDeviceBody))
        {
            Log("[frame] GetDevice at rva 0x%08X does not match - capture disabled",
                static_cast<unsigned>(t - ExeBase()));
            return;
        }
        const uintptr_t g = ReadU32(t + 1);
        if (global && g != global)
        {
            Log("[frame] the screen-copy sites disagree on the device pointer - capture disabled");
            return;
        }
        global = g;
    }
    if (global < ExeBase() || global >= ExeBase() + build->sizeOfImage)
    {
        Log("[frame] device pointer outside the exe - capture disabled");
        return;
    }
    g_deviceGlobal = global;

    // 12a: the game's SetRenderState wrapper and its copy of the render states
    {
        uint32_t w[4] = {};
        const int nw = FindPattern(kSetRenderStateWrapper, w, 4);
        if (nw == 1)
        {
            const uintptr_t wv = Rva(w[0]);
            const uint32_t copy1 = ReadU32(wv + kWrapperCacheOperand);
            const uint32_t copy2 = ReadU32(wv + kWrapperCacheOperand2);
            if (copy1 == copy2 && copy1 >= ExeBase() && copy1 < ExeBase() + build->sizeOfImage)
            {
                g_rsCopy = copy1;
                g_rsWrapperReturn = wv + kWrapperDeviceCallReturn;
                Log("[shadow] SetRenderState wrapper at rva 0x%08X, the game's state copy at rva 0x%08X - "
                    "watching for states set on Direct3D directly; Num / keeps the copy in sync",
                    w[0], static_cast<unsigned>(copy1 - ExeBase()));
            }
            else
            {
                Log("[shadow] SetRenderState wrapper: the two copy addresses disagree - scan off");
            }
        }
        else
        {
            Log("[shadow] SetRenderState wrapper: %d pattern hits (need exactly 1) - scan off", nw);
        }
    }
    VbScanInstall(); // 21j-r12: the text garble scan (the vertex buffer Lock's jump thunk)
    GlyphPoolInstall(); // 21j-r13: the glyph pool grows (the text chunk allocator's jump thunk)
    GlyphLeakInstall(); // 21j-r14: a deleted text's chunks back in the pool (the destructor's cleanup's jump thunk)
    Log("[frame] device pointer at rva 0x%08X (%d copy sites agree); waiting for the device",
        static_cast<unsigned>(global - ExeBase()), n);
    const HANDLE t = CreateThread(nullptr, 0, WaitForDeviceThread, nullptr, 0, nullptr);
    if (t)
        CloseHandle(t);
}

void FrameCaptureToggleSource(int index)
{
    if (!g_sourcesReady || index < 0 || index >= kSourceCount)
    {
        Log("[frame] draw-group switch: not available on this run");
        return;
    }
    const LONG bit = 1L << index;
    const LONG now = InterlockedXor(&g_sourceMask, bit) ^ bit;
    const bool off = (now & bit) != 0;
    char list[200];
    list[0] = 0;
    size_t used = 0;
    for (int i = 0; i < kSourceCount; ++i)
    {
        if (!(now & (1L << i)))
            continue;
        const int w = snprintf(list + used, sizeof(list) - used, "%s%d", used ? " " : "", i + 1);
        if (w < 0 || static_cast<size_t>(w) >= sizeof(list) - used)
            break;
        used += static_cast<size_t>(w);
    }
    if (!used)
        snprintf(list, sizeof(list), "none");
    Log("[frame] %lu ms: Num %d: draw group %d (rva 0x%08X, %s) %s - groups off now: %s",
        ScanMsSinceAttach(), index + 1, index + 1, static_cast<unsigned>(kSources[index].rva),
        kSources[index].what, off ? "OFF" : "back ON", list);
    Feedback(off);
}

void FrameCaptureToggleScreenSampling()
{
    const LONG now = InterlockedXor(&g_skipScreenSampling, 1) ^ 1;
    Log("[frame] %lu ms: Num *: every draw that samples a screen texture %s (%d screen textures known)",
        ScanMsSinceAttach(), now ? "OFF" : "back ON", g_screenTexCount);
    Feedback(now != 0);
}

void FrameCaptureToggleCacheSync()
{
    if (!g_rsCopy)
    {
        Log("[shadow] Num /: the state-copy scan is not available on this run");
        return;
    }
    const LONG now = InterlockedXor(&g_cacheSync, 1) ^ 1;
    Log("[shadow] %lu ms: Num /: keeping the game's state copy in sync %s (%u direct state changes so "
        "far)",
        ScanMsSinceAttach(), now ? "ON" : "OFF", g_directTotal);
    Feedback(now != 0);
}

void FrameCaptureToggleBlur()
{
    if (!g_scannerReady)
    {
        Log("[frame] Num -: the effect scanner is not available on this run");
        return;
    }
    const LONG bits = (1L << 0) | (1L << 1); // motion blur, blur
    const LONG now = InterlockedXor(&g_skipMask, bits) ^ bits;
    const bool off = (now & bits) != 0;
    Log("[frame] %lu ms: Num -: blur and motion blur %s", ScanMsSinceAttach(), off ? "OFF" : "back ON");
    Feedback(off);
}

void FrameCaptureAllOn()
{
    InterlockedExchange(&g_sourceMask, 0);
    InterlockedExchange(&g_skipMask, 0);
    InterlockedExchange(&g_skipScreenSampling, 0);
    InterlockedExchange(&g_cacheSync, 0);
    g_cycle = -1;
    Log("[frame] %lu ms: Num 0: every draw group back ON", ScanMsSinceAttach());
    Feedback(false);
}

void FrameCaptureCycleSkip()
{
    if (!g_sourcesReady)
    {
        Log("[frame] draw-group switch: not available on this run");
        return;
    }
    g_cycle = (g_cycle + 1 >= kSourceCount) ? -1 : g_cycle + 1;
    InterlockedExchange(&g_sourceMask, g_cycle < 0 ? 0 : (1L << g_cycle));
    if (g_cycle < 0)
        Log("[frame] %lu ms: LB+RB+X: every draw group back ON", ScanMsSinceAttach());
    else
        Log("[frame] %lu ms: LB+RB+X: only draw group %ld OFF (rva 0x%08X, %s)", ScanMsSinceAttach(),
            static_cast<long>(g_cycle + 1), static_cast<unsigned>(kSources[g_cycle].rva),
            kSources[g_cycle].what);
    Feedback(g_cycle >= 0);
}

void FrameCaptureArm()
{
    if (!g_present)
    {
        Log("[frame] capture key: the capture is not installed (yet)");
        return;
    }
    InterlockedExchange(&g_armed, 1);
    Log("[frame] %lu ms: capture key: the next whole frame will be captured", ScanMsSinceAttach());
    WsArm("the capture key"); // 21j-r34: and, with the water scan on, a water shot of it
}

// 13f: the menu layer in split screen. The game draws its menu screens after
// the last view, with the viewport still on player 2's half. This saves the
// current viewport and sets player 1's box instead: mode 1 = the target's own
// shape (16:9) as large as fits, centred in player 1's half (the single-player
// layout at half size); mode 2 = player 1's whole half (squeezed sideways).
// Uses Direct3D's own GetViewport / SetViewport. Changes nothing (returns false)
// unless the current viewport is clearly player 2's split-screen viewport - or,
// with player1Own (13g: player 1's HUD layer), player 1's own viewport.
bool MenuViewportBegin(int mode, bool player1Own, MenuViewportSave* save)
{
    if (!save || !g_setViewport || !g_deviceGlobal)
        return false;
    void* dev = *reinterpret_cast<void* const*>(g_deviceGlobal);
    if (!dev)
        return false;
    void** vt = *reinterpret_cast<void***>(dev);
    Viewport9 cur;
    memset(&cur, 0, sizeof(cur));
    if (reinterpret_cast<GetViewport_t>(vt[kSlotGetViewport])(dev, &cur) < 0)
        return false;
    void* rt = nullptr;
    if (reinterpret_cast<GetRenderTarget_t>(vt[kSlotGetRenderTarget])(dev, 0, &rt) < 0 || !rt)
        return false;
    SurfaceDesc9 d;
    memset(&d, 0, sizeof(d));
    void** svt = *reinterpret_cast<void***>(rt);
    const bool ok = reinterpret_cast<GetDesc_t>(svt[12])(rt, &d) >= 0;
    reinterpret_cast<Release_t>(svt[2])(rt);
    if (!ok || !d.Width || !d.Height)
        return false;
    // player 1's region
    DWORD rw = d.Width;
    DWORD rh = d.Height;
    if (player1Own)
    {
        // player 1's own split viewport: top-left corner, smaller than the target
        if (cur.X != 0 || cur.Y != 0 || (cur.Width >= d.Width && cur.Height >= d.Height) || !cur.Width ||
            !cur.Height)
            return false;
        rw = cur.Width;
        rh = cur.Height;
    }
    else if (cur.X > 0 && cur.X < d.Width && cur.Width < d.Width)
    {
        rw = cur.X; // side by side: everything left of player 2
        if (cur.Y > 0 && cur.Y < d.Height && cur.Height < d.Height)
            rh = cur.Y; // 20g-r7: 4 quarters (the last view drawn is the bottom right one): player 1's top left quarter
    }
    else if (cur.Y > 0 && cur.Y < d.Height && cur.Height < d.Height)
        rh = cur.Y; // top / bottom: everything above player 2
    else
        return false; // not player 2's split viewport - leave it alone
    Viewport9 vp;
    vp.MinZ = cur.MinZ;
    vp.MaxZ = cur.MaxZ;
    if (mode == 2)
    {
        vp.X = 0;
        vp.Y = 0;
        vp.Width = rw;
        vp.Height = rh;
    }
    else
    {
        DWORD bw = rw;
        DWORD bh = static_cast<DWORD>(static_cast<unsigned long long>(rw) * d.Height / d.Width);
        if (bh > rh)
        {
            bh = rh;
            bw = static_cast<DWORD>(static_cast<unsigned long long>(rh) * d.Width / d.Height);
        }
        vp.X = (rw - bw) / 2;
        vp.Y = (rh - bh) / 2;
        vp.Width = bw;
        vp.Height = bh;
    }
    save->x = cur.X;
    save->y = cur.Y;
    save->w = cur.Width;
    save->h = cur.Height;
    save->minZ = cur.MinZ;
    save->maxZ = cur.MaxZ;
    save->boxX = vp.X;
    save->boxY = vp.Y;
    save->boxW = vp.Width;
    save->boxH = vp.Height;
    g_setViewport(dev, &vp);
    return true;
}

// 21h-r3: the menu layer in player 2's box (3 / 4 players: the views' last one is the bottom row - player 4's quarter or
// player 3's half). His region: right of player 4's quarter (or of the middle) and above the bottom row; mode 1 = the target's
// shape as large as fits, centred in it; mode 2 = all of it. Changes nothing (false) unless the current viewport is a bottom
// row view.
bool MenuViewportBeginP2(int mode, MenuViewportSave* save)
{
    if (!save || !g_setViewport || !g_deviceGlobal)
        return false;
    void* dev = *reinterpret_cast<void* const*>(g_deviceGlobal);
    if (!dev)
        return false;
    void** vt = *reinterpret_cast<void***>(dev);
    Viewport9 cur;
    memset(&cur, 0, sizeof(cur));
    if (reinterpret_cast<GetViewport_t>(vt[kSlotGetViewport])(dev, &cur) < 0)
        return false;
    void* rt = nullptr;
    if (reinterpret_cast<GetRenderTarget_t>(vt[kSlotGetRenderTarget])(dev, 0, &rt) < 0 || !rt)
        return false;
    SurfaceDesc9 d;
    memset(&d, 0, sizeof(d));
    void** svt = *reinterpret_cast<void***>(rt);
    const bool ok = reinterpret_cast<GetDesc_t>(svt[12])(rt, &d) >= 0;
    reinterpret_cast<Release_t>(svt[2])(rt);
    if (!ok || !d.Width || !d.Height)
        return false;
    if (!(cur.Y > 0 && cur.Y < d.Height && cur.Height < d.Height))
        return false; // not a bottom row view - leave it alone
    const DWORD x0 = (cur.X > 0 && cur.X < d.Width) ? cur.X : d.Width / 2;
    const DWORD rw = d.Width - x0;
    const DWORD rh = cur.Y;
    if (!rw || !rh)
        return false;
    Viewport9 vp;
    vp.MinZ = cur.MinZ;
    vp.MaxZ = cur.MaxZ;
    if (mode == 2)
    {
        vp.X = x0;
        vp.Y = 0;
        vp.Width = rw;
        vp.Height = rh;
    }
    else
    {
        DWORD bw = rw;
        DWORD bh = static_cast<DWORD>(static_cast<unsigned long long>(rw) * d.Height / d.Width);
        if (bh > rh)
        {
            bh = rh;
            bw = static_cast<DWORD>(static_cast<unsigned long long>(rh) * d.Width / d.Height);
        }
        vp.X = x0 + (rw - bw) / 2;
        vp.Y = (rh - bh) / 2;
        vp.Width = bw;
        vp.Height = bh;
    }
    save->x = cur.X;
    save->y = cur.Y;
    save->w = cur.Width;
    save->h = cur.Height;
    save->minZ = cur.MinZ;
    save->maxZ = cur.MaxZ;
    save->boxX = vp.X;
    save->boxY = vp.Y;
    save->boxW = vp.Width;
    save->boxH = vp.Height;
    g_setViewport(dev, &vp);
    return true;
}

// 21h-r8: the menu layer over the whole screen (the end screens with 3 / 4 players): the render target's full size (mode 1: a
// 16:9 box in it, centred; 2: all of it) - whatever view's viewport the game left set
bool MenuViewportBeginFull(int mode, MenuViewportSave* save)
{
    if (!save || !g_setViewport || !g_deviceGlobal)
        return false;
    void* dev = *reinterpret_cast<void* const*>(g_deviceGlobal);
    if (!dev)
        return false;
    void** vt = *reinterpret_cast<void***>(dev);
    Viewport9 cur;
    memset(&cur, 0, sizeof(cur));
    if (reinterpret_cast<GetViewport_t>(vt[kSlotGetViewport])(dev, &cur) < 0)
        return false;
    void* rt = nullptr;
    if (reinterpret_cast<GetRenderTarget_t>(vt[kSlotGetRenderTarget])(dev, 0, &rt) < 0 || !rt)
        return false;
    SurfaceDesc9 d;
    memset(&d, 0, sizeof(d));
    void** svt = *reinterpret_cast<void***>(rt);
    const bool ok = reinterpret_cast<GetDesc_t>(svt[12])(rt, &d) >= 0;
    reinterpret_cast<Release_t>(svt[2])(rt);
    if (!ok || !d.Width || !d.Height)
        return false;
    Viewport9 vp;
    vp.MinZ = cur.MinZ;
    vp.MaxZ = cur.MaxZ;
    if (mode == 2)
    {
        vp.X = 0;
        vp.Y = 0;
        vp.Width = d.Width;
        vp.Height = d.Height;
    }
    else
    {
        DWORD bw = d.Width;
        DWORD bh = static_cast<DWORD>(static_cast<unsigned long long>(d.Width) * 9 / 16);
        if (bh > d.Height)
        {
            bh = d.Height;
            bw = static_cast<DWORD>(static_cast<unsigned long long>(d.Height) * 16 / 9);
        }
        vp.X = (d.Width - bw) / 2;
        vp.Y = (d.Height - bh) / 2;
        vp.Width = bw;
        vp.Height = bh;
    }
    save->x = cur.X;
    save->y = cur.Y;
    save->w = cur.Width;
    save->h = cur.Height;
    save->minZ = cur.MinZ;
    save->maxZ = cur.MaxZ;
    save->boxX = vp.X;
    save->boxY = vp.Y;
    save->boxW = vp.Width;
    save->boxH = vp.Height;
    g_setViewport(dev, &vp);
    return true;
}

// 21j-r15: the menu layer in a region of the render target (fractions fx0 .. fx1, fy0 .. fy1; mode 1: the target's shape as
// large as fits, centred in it; 2: all of it) - only while a split view's viewport is set (smaller than the target: a match
// in split screen), else nothing changes (false). ownView: the region is the current viewport itself (fx / fy unused).
static bool MenuViewportBeginIn(int mode, bool ownView, float fx0, float fy0, float fx1, float fy1, MenuViewportSave* save)
{
    if (!save || !g_setViewport || !g_deviceGlobal)
        return false;
    void* dev = *reinterpret_cast<void* const*>(g_deviceGlobal);
    if (!dev)
        return false;
    void** vt = *reinterpret_cast<void***>(dev);
    Viewport9 cur;
    memset(&cur, 0, sizeof(cur));
    if (reinterpret_cast<GetViewport_t>(vt[kSlotGetViewport])(dev, &cur) < 0)
        return false;
    void* rt = nullptr;
    if (reinterpret_cast<GetRenderTarget_t>(vt[kSlotGetRenderTarget])(dev, 0, &rt) < 0 || !rt)
        return false;
    SurfaceDesc9 d;
    memset(&d, 0, sizeof(d));
    void** svt = *reinterpret_cast<void***>(rt);
    const bool ok = reinterpret_cast<GetDesc_t>(svt[12])(rt, &d) >= 0;
    reinterpret_cast<Release_t>(svt[2])(rt);
    if (!ok || !d.Width || !d.Height || !cur.Width || !cur.Height)
        return false;
    if (cur.Width >= d.Width && cur.Height >= d.Height)
        return false; // the whole target: not a split view - leave it alone
    if (cur.X + cur.Width > d.Width || cur.Y + cur.Height > d.Height)
        return false;
    DWORD x0 = cur.X;
    DWORD y0 = cur.Y;
    DWORD rw = cur.Width;
    DWORD rh = cur.Height;
    if (!ownView)
    {
        if (!(fx0 >= 0.0f && fx1 <= 1.0f && fx1 > fx0 && fy0 >= 0.0f && fy1 <= 1.0f && fy1 > fy0))
            return false;
        x0 = static_cast<DWORD>(fx0 * static_cast<float>(d.Width) + 0.5f);
        y0 = static_cast<DWORD>(fy0 * static_cast<float>(d.Height) + 0.5f);
        const DWORD x1 = static_cast<DWORD>(fx1 * static_cast<float>(d.Width) + 0.5f);
        const DWORD y1 = static_cast<DWORD>(fy1 * static_cast<float>(d.Height) + 0.5f);
        if (x1 <= x0 || y1 <= y0 || x1 > d.Width || y1 > d.Height)
            return false;
        rw = x1 - x0;
        rh = y1 - y0;
    }
    Viewport9 vp;
    vp.MinZ = cur.MinZ;
    vp.MaxZ = cur.MaxZ;
    if (mode == 2)
    {
        vp.X = x0;
        vp.Y = y0;
        vp.Width = rw;
        vp.Height = rh;
    }
    else
    {
        DWORD bw = rw;
        DWORD bh = static_cast<DWORD>(static_cast<unsigned long long>(rw) * d.Height / d.Width);
        if (bh > rh)
        {
            bh = rh;
            bw = static_cast<DWORD>(static_cast<unsigned long long>(rh) * d.Width / d.Height);
        }
        vp.X = x0 + (rw - bw) / 2;
        vp.Y = y0 + (rh - bh) / 2;
        vp.Width = bw;
        vp.Height = bh;
    }
    save->x = cur.X;
    save->y = cur.Y;
    save->w = cur.Width;
    save->h = cur.Height;
    save->minZ = cur.MinZ;
    save->maxZ = cur.MaxZ;
    save->boxX = vp.X;
    save->boxY = vp.Y;
    save->boxW = vp.Width;
    save->boxH = vp.Height;
    g_setViewport(dev, &vp);
    return true;
}

bool MenuViewportBeginRegion(int mode, float fx0, float fy0, float fx1, float fy1, MenuViewportSave* save)
{
    return MenuViewportBeginIn(mode, false, fx0, fy0, fx1, fy1, save);
}

bool MenuViewportBeginOwn(int mode, MenuViewportSave* save)
{
    return MenuViewportBeginIn(mode, true, 0.0f, 0.0f, 1.0f, 1.0f, save);
}

// 18s: read only - the current viewport and the render target's size
// 21j-r11: the frame-rate scan's parts (see render_hooks.hpp)
bool FramePerfEnter(int slot)
{
    return PerfEnterI(slot);
}

void FramePerfLeave(bool entered)
{
    PerfLeaveI(entered);
}

// 21j-r1: frames presented so far (0 until the Present wrapper is in)
long FrameCapturePresents()
{
    return g_calls[kHPresent];
}

// 21j-r6: the scope blur's trace (see the block above Hook_Present)
bool FrameScopeTraceArm(int view, int player)
{
    if (g_stOn || g_stLines >= kStMax - 4 || !g_present || !g_deviceGlobal)
        return false;
    void* dev = *reinterpret_cast<void* const*>(g_deviceGlobal);
    if (!dev)
        return false;
    g_stOn = true;
    g_stLeft = kStEach;
    ++g_stId;
    g_stView = view;
    g_stPlayer = player;
    g_stCopyTex = nullptr;
    g_stPassRt = nullptr;
    g_stPassTex = nullptr;
    g_stDraws = 0;
    g_stCopyDraws = 0;
    g_stPassDraws = 0;
    char st[200];
    StState(dev, st, sizeof(st), nullptr);
    ++g_stLines;
    Log("[scope] trace %d: view %d (player %d)'s scope blur draw begins - %s", g_stId, view, player, st);
    return true;
}

// 21j-r33: the water scan (render_hooks.hpp)
void FrameWaterScanOn(const volatile int* currentView)
{
    g_wtViewPtr = currentView;
}

void FrameWaterBegin(int phase, int view)
{
    g_wtPhase = (phase >= 1 && phase <= 3) ? phase : 0;
    g_wtView = view;
    g_wtRtDirty = true;
    g_wtSegFirst = phase == 2;
}

void FrameWaterEnd()
{
    g_wtPhase = 0;
    g_wtRtDirty = true;
    g_wtSegFirst = false;
}

void FrameWaterShotArm(const char* why)
{
    WsArm(why);
}

void FrameWaterDetail(int view)
{
    if (view >= 0 && view < 4)
        g_wtDetail[view] = true;
}

long FrameDrawCalls()
{
    return g_calls[kHDrawPrimitive] + g_calls[kHDrawIndexedPrimitive] + g_calls[kHDrawPrimitiveUP] +
           g_calls[kHDrawIndexedPrimitiveUP];
}

void FrameWaterStale(int view, long* otherView, long* oldFrame, long* ownView)
{
    const bool ok = view >= 0 && view < 4;
    if (otherView)
        *otherView = ok ? g_wtStale[view][0] : 0;
    if (oldFrame)
        *oldFrame = ok ? g_wtStale[view][1] : 0;
    if (ownView)
        *ownView = ok ? g_wtStale[view][2] : 0;
}

void FrameScopeTraceNote(const char* what)
{
    if (!g_stOn || g_stLines >= kStMax)
        return;
    ++g_stLines;
    Log("[scope] trace %d: %s (%d draw(s) so far: %d sampled the screen copy, %d the blur picture)", g_stId, what ? what : "", g_stDraws,
        g_stCopyDraws, g_stPassDraws);
}

bool MenuViewportRead(MenuViewportSave* out)
{
    if (!out || !g_deviceGlobal)
        return false;
    void* dev = *reinterpret_cast<void* const*>(g_deviceGlobal);
    if (!dev)
        return false;
    void** vt = *reinterpret_cast<void***>(dev);
    Viewport9 cur;
    memset(&cur, 0, sizeof(cur));
    if (reinterpret_cast<GetViewport_t>(vt[kSlotGetViewport])(dev, &cur) < 0)
        return false;
    void* rt = nullptr;
    if (reinterpret_cast<GetRenderTarget_t>(vt[kSlotGetRenderTarget])(dev, 0, &rt) < 0 || !rt)
        return false;
    SurfaceDesc9 d;
    memset(&d, 0, sizeof(d));
    void** svt = *reinterpret_cast<void***>(rt);
    const bool ok = reinterpret_cast<GetDesc_t>(svt[12])(rt, &d) >= 0;
    reinterpret_cast<Release_t>(svt[2])(rt);
    if (!ok)
        return false;
    out->x = cur.X;
    out->y = cur.Y;
    out->w = cur.Width;
    out->h = cur.Height;
    out->minZ = cur.MinZ;
    out->maxZ = cur.MaxZ;
    out->boxX = 0;
    out->boxY = 0;
    out->boxW = d.Width;
    out->boxH = d.Height;
    return true;
}

void MenuViewportEnd(const MenuViewportSave* save)
{
    if (!save || !g_setViewport || !g_deviceGlobal)
        return;
    void* dev = *reinterpret_cast<void* const*>(g_deviceGlobal);
    if (!dev)
        return;
    Viewport9 vp;
    vp.X = save->x;
    vp.Y = save->y;
    vp.Width = save->w;
    vp.Height = save->h;
    vp.MinZ = save->minZ;
    vp.MaxZ = save->maxZ;
    g_setViewport(dev, &vp);
}

} // namespace bf1
