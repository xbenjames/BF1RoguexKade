// BF1GameExt - sprint.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/sprint/; file names in the comments and log lines follow the new names. Nothing else changed.
// 21j-r17: the MODS page's SPRINT row (menus.cpp): SprintMenuState / SprintMenuSet switch the boost ([Sprint] TestBoost)
// on / off at once and save it; with it off there is no boost and no stamina bar (SprintHudState answers 0), the stamina still
// refills. L3 stays sprint only (L3SprintOnly as it was set at game start). Nothing else changed.
// 20g-r5: SprintHudState / SprintStamina01 answer players 3 and 4 too (their stamina bars, stamina_hud.cpp).
// 20e-r2: players 3 and 4 (with [Splitscreen] Players=3 / 4 and PadsP34=1; nothing changes for 2 players): L3
// sprint and stamina as players 1 and 2 have them (their pads from players34.cpp - no stamina bar yet, their HUD
// is a later phase), and kept in third person as player 2 is (14b, the same P2ThirdPerson setting).
// Phase 14a (2020 build): sprint scan + test boost. [sprint] lines.
// Phase 14b: forward-only boost (14a log: pushing forward gives a POSITIVE
// forward input, both players); L3 is sprint only; player 2 stays in third
// person. [sprint] and [view] lines.
// Phase 14c: stamina, with Battlefront II's own defaults (BF2 GOG exe, the
// EntitySoldierClass constructor: EnergyDrainSprint 20, EnergyRestore 10,
// EnergyRestoreIdle 15, SprintAccelerateTime 0.5; energy bar 100). Sprinting
// drains it; at 0 the sprint stops and the button must be let go; it starts
// again only with at least StaminaMinSprint. The boost ramps up over
// SprintAccelTime. No HUD yet (stamina_hud.cpp scans the HUD this round).
// Phase 14d: SprintHudState() feeds the green stamina bar (stamina_hud.cpp).
// Phase 14e: animation scan (log-only). While sprinting the soldier slides in a
// standing pose instead of playing runforward. The soldier object is 0xB50
// bytes (EntitySoldier allocation, GOG 1.3 0x0050899B: push 0B50h; call new).
// Per player, once: two snapshots of the whole soldier 0.4 s apart while
// running normally (standing state, 90..105% of MaxSpeed, no boost), then one
// after 0.6 s of sprinting (boosted, over 130% of MaxSpeed). Every 4-byte
// field that stayed the same while running but is different while sprinting
// is logged ([anim] lines) - the animation state the sprint knocks over
// should be among them.
// 14e-r1 result (Steam, player 1): only 6 fields. The soldier holds 3
// animation channels of 0x12C bytes at +0x4A4 (constructor: push 0x12C, 3
// ...). Channel 0's animation pointers (+0x4BC/+0x4C0/+0x4D0/+0x4D4/+0x4D8)
// moved to other entries, and channel 1's state (+0x5E0) went 3 -> 0.
// 14e-r2: WRITE WATCH (log-only). The soldier code never addresses these
// fields directly, so to find who writes them: once player 1 has been on foot
// for a second, a helper thread puts two CPU data breakpoints (debug
// registers DR0 / DR1, write, 4 bytes) on player 1's +0x5E0 and +0x4BC; a
// vectored exception handler (first in line, it only takes these two
// breakpoints and lets the game go on at once) records each writing
// instruction's address, its caller (ebp chain) and the value written. They
// are logged as RVAs every 5 s and taken off after 25 s, or at once if
// player 1 dies or the soldier changes. Channel state changes are logged
// with the speed at that moment ([anim] lines).
// 14e-r2 result: the watch works (no hitch, no crash), but the fields are NOT
// animation. The writers (Steam rva 0xA805C / 0xA805F / 0xA666C = GOG 1.3
// 0x004A3C88 / 0x004A3C8C / 0x004A2298) append pointers to, or clear, three
// lists of up to 70 entries (count byte at +0x10): contact lists that change
// with speed. The run / sprint differences were list noise.
// 14e-r3: READ watch on the soldier's velocity instead (+0x8CC x, +0x8D4 z;
// RW = read or write): the animation must read the speed to choose walk /
// run, so every distinct place that touches the velocity is listed (up to 32
// per field, with caller and count) for 15 s.
// 14e-r4: r3 CRASHED the moment it armed: the exception handler read the
// watched value itself ("value then"), which with a READ watch fires the
// breakpoint again inside the handler, over and over, until the stack runs
// out. Now the handler never touches the watched memory (no value is
// recorded), only counts places inside the exe (our own reads of the
// velocity are let through, not listed), and the watch takes itself off after
// 200,000 hits. INI [Sprint] AnimWatch=0 switches it off.
// r4 results: the 15 places that read the velocity are physics, Move itself,
// footstep / foley sound, a moving-or-not sound picker (GOG 0x00615A00), the
// minimap, aim spread (GOG 0x0050BAE0) and sound - NOT the animation. And
// the r4 watch crashed when it was taken off by the helper thread.
// Tests: the run animation plays at SpeedMult 1.2 (7.8 m/s) and breaks to the
// standing pose at 1.4 (9.1 m/s) and 1.5 (9.75 m/s).
// 14e-r5: DEEP animation scan (log-only, no debug registers). The run / stand
// choice is not stored in the soldier itself (r1 / r2 diffs), so the snapshot
// now also copies every block the soldier points at (up to 160 blocks of
// 0x400 bytes, heap only), and the same "steady while running, different
// while sprinting" test runs inside those blocks: a stored speed (6.5 ->
// 9.x) or an animation choice should show up there. Player 1, once.
// AnimWatch now defaults to 0 (its data is in).
// r5 result: soldier +0xA70 points at the soldier's animation controller; its
// +0x94 (the move animation chosen) was 7 while running and 0 (stand) while
// sprinting. The controller update (GOG 0x00611830) measures the soldier's
// forward / strafe speed from how far he moved this frame (+0x4C, divided by
// dt - which is why no velocity read was the animation) and asks the move
// selector (GOG 0x00613680) for the animation. Its first test:
//     if (forward^2 + strafe^2 > [0x007D9E18]) return 0;   // 64.0 = 8 m/s
// so above 8 m/s the soldier gets the standing pose (1.2 = 7.8 m/s ran,
// 1.4 = 9.05 m/s stood). Below the cap the run animation's playback rate
// already follows the speed (rate = speed / the animation's own speed,
// eased 5% toward 1), so the legs speed up with the sprint by themselves.
//
// Phase 14f: the cap. The selector's "comiss xmm1, [0x007D9E18]" reads the
// constant through a 4-byte absolute address (the constant is shared with 4
// other places, so it is never changed); that address is pointed at
// BF1GameExt's own value instead: AnimSpeedLimit squared (default 14 m/s ->
// 196). Checked first: one pattern hit (the selector's 46-byte start) and the
// old address must hold 64.0. INI [Sprint] AnimSpeedLimit (8 or less = leave
// the game's cap). The r1 - r5 animation scans stay in the DLL, off unless
// [Sprint] AnimScan=1 / AnimWatch=1.
//
// EntitySoldier::Move (GOG 1.3 0x0050C810; found by a 97-byte prologue
// pattern, exactly 1 hit on GOG 1.3) - thiscall, ecx = soldier, ret 14h:
//   arguments  dt, forward input, strafe input, turn input, brake
//   class = [soldier+830h] (EntitySoldierClass, ODF reader at 0x00512350)
//     +1630h acceleration (default 70)
//     +1634h MaxSpeed (default 9)       +1638h MaxStrafeSpeed (default 7)
//   forward: target = MaxSpeed * forward input; the forward part of the
//            velocity moves toward it by acceleration * 1.2 * dt
//   strafe:  the same with MaxStrafeSpeed
//   The forward input is used nowhere else in Move (checked to its ret 14h).
//   soldier+8CCh velocity (x, y, z)     soldier+0A64h movement state
// The soldier update (0x00514B60, a switch on the state) calls Move once per
// frame from 10 places: state 0 = standing passes the inputs as they are,
// 1 = crouch (forward x0.7, strafe x0.6), 2 = prone (x0.3, x0.2), 7 = jet.
// The class values are shared by every soldier of the class (AI too), so they
// are never written here: only one soldier's forward input is scaled.
//
// Player k's soldier: the character's unit (character + 0xB4) is the
// controllable the PlayerController drives; for a soldier that is the
// sub-object at soldier + 0x250 (the soldier update reads its inputs there).
//
// Hook (checked on GOG 1.3: no jump or call lands inside the 6 bytes):
//   Move+0: 53 8B DC 83 EC 08   push ebx; mov ebx, esp; sub esp, 8
//   ->      E9 <stub> 90
// The stub (DLL memory) - disassembled before use:
//   60             pushad
//   9C             pushfd
//   8D 44 24 24    lea eax, [esp+24h]       ; Move's entry esp
//   50             push eax
//   51             push ecx                 ; soldier
//   E8 <rel32>     call SprintOnMove        ; stdcall, 2 arguments
//   9D             popfd
//   61             popad
//   53 8B DC 83 EC 08                       ; the 6 original bytes
//   E9 <rel32>     jmp Move+6
// SprintOnMove(soldier, entry) reads / changes the arguments in place:
//   entry[0] return address, [1] dt, [2] forward, [3] strafe, [4] turn, [5] brake
//
// 14b - L3 sprint only. The 2020 pad layer (GOG 1.3, all static data):
//   0x006BA800  XInput buttons -> the game's pad bits (L3 -> bit 1)
//   0x008372A8  16 entries: input action i fires when pad bit [i] is down
//               (read every frame by PlayerInput::Update 0x00463AF1:
//                mov ecx, [edi+table]; mov eax, 1; shl eax, cl; cdq; and ...)
//   0x008370D8  3 pad layouts x 32: input action -> game control, copied into
//               every PlayerInput (+0x12C). All three: Back map, D-pad squad
//               commands, L3 VIEW, R3 zoom, X reload, B crouch, Y use, A jump,
//               LT secondary fire, RT primary fire, LB / RB next weapon.
//   Action 6 is L3 (bit 1) -> control 5 "view" (controllable +0x46, the 14a
//   log saw it go down with every L3 press). Action 6's entry becomes bit 30,
//   which no pad ever sets: L3 reaches nothing in the game, for both players,
//   at once (the table is shared and read every frame). The sprint code reads
//   L3 from the pad object itself. Keyboard view (the profile binding) stays.
// 14b - player 2 in third person. EntitySoldier view toggle (GOG 1.3
//   0x00508BBA, inside the soldier's control update):
//     if (gate [0x008382F8] && gate [0x008C4F31] && view button pressed)
//       view = [soldier+44Ch+4]
//       SetFirstPerson(view, controllable's character index, !IsFirstPerson(view))
//   IsFirstPerson (0x00494B70): the camera's mode 1 forces third person, 2
//   forces first person, otherwise the view's own flag (+0x14).
//   SetFirstPerson (0x006240D0, thiscall, ret 8): skipped while the mode is 1
//   or 2, else flag = value and the camera is told.
//   For player 2's soldier, whenever IsFirstPerson is true, the game's own
//   SetFirstPerson(view, index, 0) is called (at most once per second while it
//   does not take), and the outcome is logged.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "game_build.hpp"
#include "log.hpp"
#include "pads.hpp"
#include "players34.hpp"
#include "game_check.hpp"
#include "spawn_manager.hpp"
#include "sprint.hpp"
#include "stamina_hud.hpp"

namespace bf1 {
namespace {

const char* const kFileBuild = "p23a-r1";

// EntitySoldier::Move: prologue + its 5 argument checks + the first call.
const char* const kMovePattern =
    "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC E8 00 00 00 56 57 "
    "8D 43 08 8B F9 6A 02 50 E8 ?? ?? ?? ?? 8D 43 0C 6A 02 50 E8 ?? ?? ?? ?? 8D 43 10 6A 02 50 "
    "E8 ?? ?? ?? ?? 8D 43 14 6A 02 50 E8 ?? ?? ?? ?? 8D 43 18 6A 02 50 E8 ?? ?? ?? ?? 83 C4 28 "
    "8D 8F 34 08 00 00 E8";
const unsigned char kEntryBytes[6] = { 0x53, 0x8B, 0xDC, 0x83, 0xEC, 0x08 };
constexpr uint32_t kBodyWindow = 0x8E0; // Move is 0x8CE bytes on GOG 1.3

// 14b: PlayerInput::Update's pad bit -> action loop (the table is its disp32).
const char* const kPadActionLoop =
    "8B 8F ?? ?? ?? ?? B8 01 00 00 00 D3 E0 99 23 45 E8 23 55 DC 0B C2 74";
constexpr uint32_t kOffPadActionTable = 2;
const uint32_t kExpectedPadActions[16] = { 3, 0, 7, 5, 4, 6, 1, 2, 15, 13, 12, 14, 10, 11, 8, 9 };
constexpr int kActionL3 = 6;
constexpr uint32_t kUnusedPadBit = 30; // pad bits are 0..15

// 14b: the soldier's view toggle (see the top of this file).
const char* const kViewToggle =
    "57 E8 ?? ?? ?? ?? 84 C0 74 60 E8 ?? ?? ?? ?? 84 C0 74 57 8D 8E 96 02 00 00 E8 ?? ?? ?? ?? 84 C0 "
    "74 48 8D 8E 4C 04 00 00 E8 ?? ?? ?? ?? 85 C0 74 39 8D 8E 4C 04 00 00 E8 ?? ?? ?? ?? 8B C8 E8 "
    "?? ?? ?? ?? 84 C0 8D 8E 50 02 00 00 0F 94 C0 0F B6 C0 50 E8 ?? ?? ?? ?? 50 8D 8E 4C 04 00 00 "
    "E8 ?? ?? ?? ?? 8B C8 E8";
constexpr uint32_t kOffGateA = 1;
constexpr uint32_t kOffGateB = 10;
constexpr uint32_t kOffViewHolder = 40;
constexpr uint32_t kOffIsFirstPerson = 62;
constexpr uint32_t kOffCharIndex = 82;
constexpr uint32_t kOffSetFirstPerson = 101;
const char* const kGateBody = "A0 ?? ?? ?? ?? C3";
const char* const kViewHolderBody = "8B 41 04 C3";
const char* const kCharIndexBody = "8B 81 D4 00 00 00 C3";
const char* const kIsFirstPersonBody =
    "56 8B F1 E8 ?? ?? ?? ?? 85 C0 74 2E 8B CE E8 ?? ?? ?? ?? 8B C8 8B 10 FF 52 24 8B C8 8B 10 FF "
    "52 58 8B C8 E8 ?? ?? ?? ?? 83 E8 01 74 09 83 E8 01 75 08 B0 01 5E C3 32 C0 5E C3 8A 46 14 5E C3";
const char* const kSetFirstPersonBody =
    "55 8B EC 56 8B F1 83 7E 10 00 74 23 E8 ?? ?? ?? ?? 8B C8 8B 10 FF 52 24 8B C8 8B 10 FF 52 58 "
    "8B C8 E8 ?? ?? ?? ?? 85 C0 74 05 83 F8 02 76 17 8B 4E 10 8B 55 0C 88 56 14 85 C9 74 0A 8B 01 "
    "5E 89 55 0C 5D FF 60 34 5E 5D C2 08 00";
constexpr uint32_t kViewHolder = 0x44C;   // soldier -> view holder; view = [holder + 4]
constexpr uint32_t kViewCamera = 0x10;    // view -> camera
constexpr uint32_t kViewFlag = 0x14;      // view -> first-person flag
constexpr uint32_t kCharIndex = 0xD4;     // controllable -> character index
constexpr uint32_t kProfileSize = 0x458;
constexpr uint32_t kProfileFirstPerson = 0x2E;
constexpr DWORD kViewRetryMs = 1000;

typedef bool(__fastcall* IsFirstPerson_t)(void* view, void* edx);
typedef void(__fastcall* SetFirstPerson_t)(void* view, void* edx, int index, int firstPerson);

struct BodyCheck
{
    const char* what;
    const char* hex;
    int minCount;
    int maxCount;
};
const BodyCheck kBodyChecks[] = {
    { "class pointer   mov eax, [edi+830h]", "8B 87 30 08 00 00", 3, 99 },
    { "MaxSpeed        movss xmm2, [eax+1634h]", "F3 0F 10 90 34 16 00 00", 2, 2 },
    { "MaxStrafeSpeed  movss xmm3, [eax+1638h]", "F3 0F 10 98 38 16 00 00", 1, 1 },
    { "MaxStrafeSpeed  movss xmm0, [eax+1638h]", "F3 0F 10 80 38 16 00 00", 1, 1 },
    { "acceleration    movss xmm0, [eax+1630h]", "F3 0F 10 80 30 16 00 00", 1, 99 },
    { "velocity        lea esi, [edi+8CCh]", "8D B7 CC 08 00 00", 1, 99 },
    { "state           cmp dword [edi+0A64h], 7", "83 BF 64 0A 00 00 07", 1, 99 },
    { "end             mov esp, ebx; pop ebx; ret 14h", "8B E3 5B C2 14 00", 1, 1 },
};

// Soldier / class / character offsets (GOG 1.3, the same code as Steam).
constexpr uint32_t kControllable = 0x250;
constexpr uint32_t kClass = 0x830;
constexpr uint32_t kVelocity = 0x8CC;
constexpr uint32_t kState = 0xA64;
constexpr uint32_t kAccel = 0x1630;
constexpr uint32_t kMaxSpeed = 0x1634;
constexpr uint32_t kMaxStrafe = 0x1638;
constexpr uint32_t kUnit = 0xB4;        // character -> unit (controllable)
constexpr uint32_t kUnitButtons = 0x38; // controllable buttons, 2 bytes each
constexpr int kUnitButtonCount = 14;    // +0x38 .. +0x53
const char* const kUnitButtonName[kUnitButtonCount] = {
    "+0x38 (bit0)",  "+0x3A (bit1)",  "+0x3C (bit6)",  "+0x3E (bit2)", "+0x40 (bit3)",
    "+0x42 (bit7)",  "+0x44 (bit4)",  "+0x46 (bit5)",  "+0x48 (bit10)", "+0x4A (bit11)",
    "+0x4C (bit9)",  "+0x4E (bit8)",  "+0x50 (axis)",  "+0x52 (axis)",
};
constexpr int32_t kStateStand = 0;

// Pad object (the game's and pad 2): XINPUT_STATE at +0xC0.
constexpr uint32_t kPadConnected = 0x09;
constexpr uint32_t kPadButtons = 0xC4;
constexpr uint32_t kPadLY = 0xCA;
constexpr WORD kPadL3 = 0x0040;

constexpr float kMinInput = 0.3f;    // |forward| needed before the boost applies
constexpr int kWatchCalls = 30;      // frames to watch the game's buttons after a press
constexpr DWORD kSampleMs = 400;
constexpr DWORD kSummaryMs = 5000;
constexpr int kMaxLines = 500;
constexpr int kMaxSampleLines = 300;
constexpr int kMaxSummaryLines = 80;

bool g_testBoost = true;
bool g_hookOn = false;            // 21j-r17: Move's hook went in (the MODS page's SPRINT row can switch it)
char g_iniPath[MAX_PATH + 32] = ""; // 21j-r17: BF1GameExt.ini (the SPRINT row saves TestBoost there)
float g_mult = 1.5f;
bool g_l3SprintOnly = true;
bool g_p2Third = true;
bool g_stamina = true;
bool g_animWatchOn = false;       // 14e-r4: INI [Sprint] AnimWatch (r5: default 0)
bool g_animScanOn = false;        // 14f: INI [Sprint] AnimScan (the r1 / r5 snapshot scans)
float g_animSpeedLimit = 14.0f;   // 14f: INI [Sprint] AnimSpeedLimit (m/s)
float g_animLimitSq = 196.0f;     // the selector's comiss reads this once patched
float g_staminaMax = 100.0f;
float g_staminaDrain = 20.0f;     // per second while sprinting (BF2 EnergyDrainSprint)
float g_staminaRegen = 10.0f;     // per second while moving (BF2 EnergyRestore)
float g_staminaRegenIdle = 15.0f; // per second standing still (BF2 EnergyRestoreIdle)
float g_staminaMin = 20.0f;       // needed to start a sprint
float g_accelTime = 0.5f;         // boost ramp (BF2 SprintAccelerateTime)

// 14b: view (player 2 third person)
IsFirstPerson_t g_isFirstPerson = nullptr;
SetFirstPerson_t g_setFirstPerson = nullptr;
uintptr_t g_gateA = 0;
uintptr_t g_gateB = 0;
bool g_viewReady = false;
uintptr_t g_p2ViewSoldier = 0;
int g_p2ViewState = -1; // -1 unknown, 0 third, 1 first
DWORD g_p2LastTry = 0;
uintptr_t g_nViewSoldier[4] = {}; // 20e-r2: players 3 and 4 (slots 2 / 3)
int g_nViewState[4] = { -1, -1, -1, -1 };
DWORD g_nLastTry[4] = {};
int g_viewLines = 0;
constexpr int kMaxViewLines = 120;

// 14b view functions (defined below the Move hook's code)
uintptr_t SoldierView(uintptr_t soldier);
void LogView(int p, uintptr_t soldier, uintptr_t view);
void P2ViewFrame(uintptr_t soldier);
void ViewFrameN(int p, uintptr_t soldier); // 20e-r2: players 3 and 4

int g_lines = 0;
int g_sampleLines = 0;
int g_summaryLines = 0;

struct Player
{
    uintptr_t soldier;
    int32_t state;
    bool held;
    const char* heldBy;
    DWORD heldSince;
    long heldBoosted;
    float topHeld;
    float topNormal;
    float maxSpeed;
    DWORD lastSample;
    int watchCalls;
    bool conflictLogged;
    bool signLogged;
    int8_t prevCounter[kUnitButtonCount];
    int8_t baseCounter[kUnitButtonCount];
    long calls5s;
    long boosted5s;
    // 14c stamina
    float stamina;
    bool sprinting;
    bool needRelease; // emptied: the button must be let go before the next sprint
    float sprintTime;
    float lowest;     // lowest stamina during the current button hold
    DWORD lastMove;   // GetTickCount of this player's last Move call (on foot)
};

Player g_player[4]; // 20e-r2: 4 (players 3 and 4)

// 14e: animation scan
constexpr uint32_t kSoldierSize = 0xB50;
constexpr int kSoldierDwords = static_cast<int>(kSoldierSize / 4);
constexpr DWORD kAnimSteadyMs = 600;
constexpr DWORD kAnimGapMs = 400;
constexpr int kMaxAnimLines = 420;
struct AnimScan
{
    int stage;        // 0 wait run, 1 run snapshot 1 taken, 2 wait sprint, 3 done
    DWORD since;      // start of the current steady stretch (0 = none)
    DWORD shot1;
    uintptr_t soldier;
    uint32_t run1[kSoldierDwords];
    uint32_t run2[kSoldierDwords];
};
AnimScan g_anim[4];
int g_animLines = 0;

// 14e-r5: blocks the soldier points at (player 1 only)
constexpr int kMaxBlocks = 160;
constexpr uint32_t kBlockSize = 0x400;
constexpr int kBlockDwords = static_cast<int>(kBlockSize / 4);
struct DeepShot
{
    int count;
    uint32_t off[kMaxBlocks];   // soldier offset holding the pointer
    uint32_t ptr[kMaxBlocks];
    uint32_t data[kMaxBlocks][kBlockDwords];
};
DeepShot g_deep[3]; // run 1, run 2, sprint
uintptr_t g_unit[4] = {};
DWORD g_unitTick = 0;
bool g_unitInit = false;
long g_otherCalls5s = 0;
long g_totalCalls = 0;
DWORD g_lastSummary = 0;

bool Budget()
{
    if (g_lines >= kMaxLines)
        return false;
    ++g_lines;
    if (g_lines == kMaxLines)
        Log("[sprint] line limit reached, events going quiet (samples / summaries go on)");
    return true;
}

uint32_t ReadU32(uintptr_t va)
{
    uint32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

float ReadF32(uintptr_t va)
{
    float v = 0.0f;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

bool Readable(uintptr_t start, size_t len)
{
    MEMORY_BASIC_INFORMATION mbi;
    if (!start || !VirtualQuery(reinterpret_cast<LPCVOID>(start), &mbi, sizeof(mbi)))
        return false;
    if (mbi.State != MEM_COMMIT || (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)))
        return false;
    const DWORD prot = mbi.Protect & 0xFF;
    if (prot == 0 || prot == PAGE_EXECUTE)
        return false;
    return start + len <= reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
}

int HexNibble(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

// "8B 87 30" -> bytes (no wildcards). Returns the length or -1.
int ParseHex(const char* hex, unsigned char* out, int max)
{
    int n = 0;
    while (*hex)
    {
        if (*hex == ' ')
        {
            ++hex;
            continue;
        }
        const int hi = HexNibble(hex[0]);
        const int lo = hex[1] ? HexNibble(hex[1]) : -1;
        if (hi < 0 || lo < 0 || n >= max)
            return -1;
        out[n++] = static_cast<unsigned char>((hi << 4) | lo);
        hex += 2;
    }
    return n;
}

// How often 'hex' occurs in [start, start + len). firstOff = offset of the first hit.
int CountInWindow(uintptr_t start, uint32_t len, const char* hex, uint32_t* firstOff)
{
    unsigned char want[32];
    const int n = ParseHex(hex, want, static_cast<int>(sizeof(want)));
    if (n <= 0 || static_cast<uint32_t>(n) > len)
        return -1;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(start);
    int count = 0;
    for (uint32_t i = 0; i + static_cast<uint32_t>(n) <= len; ++i)
    {
        if (memcmp(p + i, want, static_cast<size_t>(n)) != 0)
            continue;
        if (count == 0 && firstOff)
            *firstOff = i;
        ++count;
    }
    return count;
}

float IniFloat(const char* ini, const char* key, double def, double lo, double hi)
{
    char def_[32];
    snprintf(def_, sizeof(def_), "%g", def);
    char buf[32] = {};
    GetPrivateProfileStringA("Sprint", key, def_, buf, sizeof(buf), ini);
    char* end = nullptr;
    double v = strtod(buf, &end);
    if (end == buf || !(v >= lo))
        v = end == buf ? def : lo;
    if (v > hi)
        v = hi;
    return static_cast<float>(v);
}

void ReadIni()
{
    char exe[MAX_PATH] = {};
    if (!GetModuleFileNameA(nullptr, exe, MAX_PATH))
        return;
    char* slash = strrchr(exe, '\\');
    if (!slash)
        return;
    *slash = 0;
    char ini[MAX_PATH + 32];
    snprintf(ini, sizeof(ini), "%s\\BF1GameExt.ini", exe);
    snprintf(g_iniPath, sizeof(g_iniPath), "%s", ini); // 21j-r17
    g_testBoost = GetPrivateProfileIntA("Sprint", "TestBoost", 0, ini) != 0; // v6.4: default off (dinput8_bf1sprint.dll does sprint)
    char buf[32] = {};
    GetPrivateProfileStringA("Sprint", "SpeedMult", "1.5", buf, sizeof(buf), ini);
    double m = strtod(buf, nullptr);
    if (!(m >= 1.0))
        m = 1.0;
    if (m > 3.0)
        m = 3.0;
    g_mult = static_cast<float>(m);
    g_l3SprintOnly = GetPrivateProfileIntA("Sprint", "L3SprintOnly", 1, ini) != 0;
    g_stamina = GetPrivateProfileIntA("Sprint", "Stamina", 1, ini) != 0;
    g_staminaMax = IniFloat(ini, "StaminaMax", 100.0, 1.0, 1000.0);
    g_staminaDrain = IniFloat(ini, "StaminaDrain", 20.0, 0.0, 1000.0);
    g_staminaRegen = IniFloat(ini, "StaminaRegen", 10.0, 0.0, 1000.0);
    g_staminaRegenIdle = IniFloat(ini, "StaminaRegenIdle", 15.0, 0.0, 1000.0);
    g_staminaMin = IniFloat(ini, "StaminaMinSprint", 20.0, 0.0, 1000.0);
    if (g_staminaMin > g_staminaMax)
        g_staminaMin = g_staminaMax;
    g_accelTime = IniFloat(ini, "SprintAccelTime", 0.5, 0.0, 10.0);
    g_animWatchOn = GetPrivateProfileIntA("Sprint", "AnimWatch", 0, ini) != 0;
    g_animScanOn = GetPrivateProfileIntA("Sprint", "AnimScan", 0, ini) != 0;
    g_animSpeedLimit = IniFloat(ini, "AnimSpeedLimit", 14.0, 0.0, 100.0);
    g_animLimitSq = g_animSpeedLimit * g_animSpeedLimit;
    g_p2Third = GetPrivateProfileIntA("Splitscreen", "P2ThirdPerson", 1, ini) != 0;
}

uint32_t LocalPlayers()
{
    const BuildInfo* b = CurrentBuild();
    if (!b || !b->localPlayerCountRva)
        return 1;
    return ReadU32(Rva(b->localPlayerCountRva));
}

// Player 1's and player 2's units, at most once per GetTickCount step (~16 ms).
void RefreshUnits()
{
    const DWORD t = GetTickCount();
    if (g_unitInit && t == g_unitTick)
        return;
    g_unitInit = true;
    g_unitTick = t;
    const int players = PlayerNPadPlayers(); // 20e-r2: 4 while pads 3 and 4 are on, else 2
    for (int k = 0; k < 4; ++k)
    {
        uintptr_t u = 0;
        if (k == 0 || (k == 1 && LocalPlayers() >= 2) || (k >= 2 && k < players && LocalPlayers() >= 2))
        {
            const uintptr_t ch = SpawnScanCharacter(k);
            // 20e-r2: players 3 / 4 by their character's player index (+9Ch), as the 20c characters are made
            if (ch && (k < 2 || static_cast<int32_t>(ReadU32(ch + 0x9C)) == k))
                u = ReadU32(ch + kUnit);
        }
        g_unit[k] = u;
    }
}

const char* StateName(int32_t s)
{
    switch (s)
    {
    case 0: return "standing";
    case 1: return "crouched";
    case 2: return "prone";
    case 7: return "jet";
    default: return "other";
    }
}

WORD PadButtons(uintptr_t pad)
{
    if (!pad || !*reinterpret_cast<const volatile uint8_t*>(pad + kPadConnected))
        return 0;
    return *reinterpret_cast<const volatile WORD*>(pad + kPadButtons);
}

float PadStickY(uintptr_t pad)
{
    if (!pad || !*reinterpret_cast<const volatile uint8_t*>(pad + kPadConnected))
        return 0.0f;
    const int16_t y = *reinterpret_cast<const volatile int16_t*>(pad + kPadLY);
    return static_cast<float>(y) / 32767.0f;
}

uintptr_t PlayerPad(int p)
{
    if (p == 0)
        return GamePadAddress();
    if (p >= 2)
        return PlayerNPadAddress(p); // 20e-r2: pad 3 / 4 (0 if not connected)
    return Pad2IsConnected() ? Pad2Address() : 0;
}

bool GameWindowInFront()
{
    const HWND fg = GetForegroundWindow();
    if (!fg)
        return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(fg, &pid);
    return pid == GetCurrentProcessId();
}

bool KeyDown(int vk)
{
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

// Player p's sprint button: L3 on the player's pad; Left Shift for player 1.
bool SprintHeld(int p, const char** by)
{
    if (PadButtons(PlayerPad(p)) & kPadL3)
    {
        *by = "L3";
        return true;
    }
    if (p == 0 && KeyDown(VK_LSHIFT) && GameWindowInFront())
    {
        *by = "Left Shift";
        return true;
    }
    *by = "";
    return false;
}

float HorizontalSpeed(uintptr_t soldier)
{
    const float x = ReadF32(soldier + kVelocity);
    const float z = ReadF32(soldier + kVelocity + 8);
    return std::sqrt(x * x + z * z);
}

void ReadCounters(uintptr_t soldier, int8_t (&out)[kUnitButtonCount])
{
    const unsigned char* b =
        reinterpret_cast<const unsigned char*>(soldier + kControllable + kUnitButtons);
    for (int i = 0; i < kUnitButtonCount; ++i)
        out[i] = static_cast<int8_t>(b[i * 2]);
}

void NewSoldier(int p, uintptr_t soldier)
{
    Player& pl = g_player[p];
    pl.soldier = soldier;
    pl.state = -999;
    pl.topNormal = 0.0f;
    pl.topHeld = 0.0f;
    pl.maxSpeed = 0.0f;
    pl.signLogged = false;
    pl.stamina = g_staminaMax;
    pl.sprinting = false;
    pl.needRelease = false;
    pl.sprintTime = 0.0f;
    ReadCounters(soldier, pl.prevCounter);

    const uintptr_t cls = ReadU32(soldier + kClass);
    if (cls && Readable(cls + kAccel, 12))
    {
        pl.maxSpeed = ReadF32(cls + kMaxSpeed);
        if (Budget())
            Log("[sprint] %lu ms: player %d on foot: soldier 0x%08X (its +0x250 is the character's "
                "unit), class 0x%08X: MaxSpeed %.2f, MaxStrafeSpeed %.2f, acceleration %.1f",
                ScanMsSinceAttach(), p + 1, static_cast<unsigned>(soldier),
                static_cast<unsigned>(cls), static_cast<double>(pl.maxSpeed),
                static_cast<double>(ReadF32(cls + kMaxStrafe)),
                static_cast<double>(ReadF32(cls + kAccel)));
    }
    else if (Budget())
    {
        Log("[sprint] %lu ms: player %d on foot: soldier 0x%08X, class pointer 0x%08X NOT readable",
            ScanMsSinceAttach(), p + 1, static_cast<unsigned>(soldier), static_cast<unsigned>(cls));
    }
}

void ButtonDown(int p, uintptr_t soldier, const char* by, int32_t state, float speed, DWORD now)
{
    Player& pl = g_player[p];
    pl.heldBy = by;
    pl.heldSince = now;
    pl.heldBoosted = 0;
    pl.topHeld = 0.0f;
    pl.watchCalls = kWatchCalls;
    pl.conflictLogged = false;
    pl.lowest = pl.stamina;
    memcpy(pl.baseCounter, pl.prevCounter, sizeof(pl.baseCounter));
    (void)soldier;
    if (Budget())
        Log("[sprint] %lu ms: player %d sprint button DOWN (%s) - state %ld (%s), speed %.2f m/s, "
            "stamina %.0f%s",
            ScanMsSinceAttach(), p + 1, by, static_cast<long>(state), StateName(state),
            static_cast<double>(speed), static_cast<double>(pl.stamina),
            g_stamina && pl.stamina < g_staminaMin ? " (below the minimum: no sprint yet)" : "");
}

void ButtonUp(int p, DWORD now)
{
    Player& pl = g_player[p];
    if (Budget())
        Log("[sprint] %lu ms: player %d sprint button up (%s) after %lu ms: %ld boosted frames, top "
            "speed while held %.2f m/s | top standing speed without it %.2f m/s | MaxSpeed %.2f | "
            "stamina %.0f (lowest %.0f)",
            ScanMsSinceAttach(), p + 1, pl.heldBy, static_cast<unsigned long>(now - pl.heldSince),
            pl.heldBoosted, static_cast<double>(pl.topHeld), static_cast<double>(pl.topNormal),
            static_cast<double>(pl.maxSpeed), static_cast<double>(pl.stamina),
            static_cast<double>(pl.lowest));
}

// After a press: did any of the game's own soldier buttons go down with it?
void CheckConflict(int p, const int8_t (&now)[kUnitButtonCount])
{
    Player& pl = g_player[p];
    if (pl.conflictLogged)
        return;
    for (int i = 0; i < kUnitButtonCount; ++i)
    {
        if (pl.baseCounter[i] <= 0 && now[i] > 0)
        {
            pl.conflictLogged = true;
            if (Budget())
                Log("[sprint] %lu ms: player %d: the game's soldier button %s went down together "
                    "with the sprint button (%s) - the game uses that button too",
                    ScanMsSinceAttach(), p + 1, kUnitButtonName[i], pl.heldBy);
            return;
        }
    }
}


// ---------------------------------------------------------------------------
// 14e: animation scan
// ---------------------------------------------------------------------------
bool AnimBudget()
{
    if (g_animLines >= kMaxAnimLines)
        return false;
    ++g_animLines;
    if (g_animLines == kMaxAnimLines)
        Log("[anim] line limit reached, going quiet");
    return true;
}

void Snapshot(uintptr_t soldier, uint32_t (&out)[kSoldierDwords])
{
    memcpy(out, reinterpret_cast<const void*>(soldier), sizeof(out));
}


bool HeapPointer(uint32_t v)
{
    if (v < 0x10000 || (v & 3) != 0)
        return false;
    const BuildInfo* b = CurrentBuild();
    const uintptr_t lo = ExeBase();
    const uintptr_t hi = lo + (b ? b->sizeOfImage : 0);
    if (v >= lo && v < hi)
        return false; // the exe image: vtables, globals, code
    return Readable(v, kBlockSize);
}

void DeepSnapshot(const uint32_t (&soldierWords)[kSoldierDwords], DeepShot& d)
{
    d.count = 0;
    for (int i = 0; i < kSoldierDwords && d.count < kMaxBlocks; ++i)
    {
        const uint32_t v = soldierWords[i];
        if (!HeapPointer(v))
            continue;
        d.off[d.count] = static_cast<uint32_t>(i * 4);
        d.ptr[d.count] = v;
        memcpy(d.data[d.count], reinterpret_cast<const void*>(v), kBlockSize);
        ++d.count;
    }
}

int FindBlock(const DeepShot& d, uint32_t off, uint32_t ptr)
{
    for (int i = 0; i < d.count; ++i)
        if (d.off[i] == off && d.ptr[i] == ptr)
            return i;
    return -1;
}

void DeepCompare(int p)
{
    const DeepShot& r1 = g_deep[0];
    const DeepShot& r2 = g_deep[1];
    const DeepShot& sp = g_deep[2];
    int blocks = 0;
    int listed = 0;
    if (AnimBudget())
        Log("[anim] player %d: deep scan - %d / %d / %d blocks the soldier points at (run 1 / run 2 / "
            "sprint); fields steady while running but different while sprinting:",
            p + 1, r1.count, r2.count, sp.count);
    for (int b = 0; b < r1.count; ++b)
    {
        const int b2 = FindBlock(r2, r1.off[b], r1.ptr[b]);
        const int b3 = FindBlock(sp, r1.off[b], r1.ptr[b]);
        if (b2 < 0 || b3 < 0)
            continue; // the pointer itself changed: not the same object
        ++blocks;
        for (int k = 0; k < kBlockDwords; ++k)
        {
            const uint32_t a = r1.data[b][k];
            if (a != r2.data[b2][k] || sp.data[b3][k] == a)
                continue;
            ++listed;
            if (!AnimBudget())
                return;
            float fa = 0.0f;
            float fs = 0.0f;
            memcpy(&fa, &a, sizeof(fa));
            memcpy(&fs, &sp.data[b3][k], sizeof(fs));
            Log("[anim]   soldier+0x%03X -> 0x%08X +0x%03X  running %08X (%g)  sprinting %08X (%g)",
                static_cast<unsigned>(r1.off[b]), static_cast<unsigned>(r1.ptr[b]),
                static_cast<unsigned>(k * 4), static_cast<unsigned>(a), static_cast<double>(fa),
                static_cast<unsigned>(sp.data[b3][k]), static_cast<double>(fs));
        }
    }
    if (AnimBudget())
        Log("[anim] player %d: deep scan done - %d blocks compared, %d fields listed", p + 1, blocks, listed);
}

void AnimCompare(int p, uintptr_t soldier, float speed, float maxSpeed)
{
    AnimScan& a = g_anim[p];
    uint32_t now[kSoldierDwords];
    Snapshot(soldier, now);
    int steady = 0;
    int listed = 0;
    for (int i = 0; i < kSoldierDwords; ++i)
        if (a.run1[i] == a.run2[i])
            ++steady;
    if (AnimBudget())
        Log("[anim] %lu ms: player %d soldier 0x%08X: sprinting at %.2f m/s (MaxSpeed %.2f) - fields that "
            "stayed the same while running (%d of %d) but are different now:",
            ScanMsSinceAttach(), p + 1, static_cast<unsigned>(soldier), static_cast<double>(speed),
            static_cast<double>(maxSpeed), steady, kSoldierDwords);
    for (int i = 0; i < kSoldierDwords; ++i)
    {
        if (a.run1[i] != a.run2[i] || now[i] == a.run1[i])
            continue;
        ++listed;
        if (!AnimBudget())
            return;
        float fr = 0.0f;
        float fs = 0.0f;
        memcpy(&fr, &a.run1[i], sizeof(fr));
        memcpy(&fs, &now[i], sizeof(fs));
        Log("[anim]   +0x%03X  running %08X (%g)  sprinting %08X (%g)", static_cast<unsigned>(i * 4),
            static_cast<unsigned>(a.run1[i]), static_cast<double>(fr), static_cast<unsigned>(now[i]),
            static_cast<double>(fs));
    }
    if (AnimBudget())
        Log("[anim] %lu ms: player %d: %d fields listed", ScanMsSinceAttach(), p + 1, listed);
}

// Every frame for player p's soldier (inside Move's hook), after the boost decision.
void AnimScanFrame(int p, uintptr_t soldier, int32_t state, bool boosted, float speed, float maxSpeed,
                   DWORD now)
{
    AnimScan& a = g_anim[p];
    if (!g_animScanOn || a.stage == 3 || maxSpeed <= 0.0f)
        return;
    if (soldier != a.soldier)
    {
        a.soldier = soldier;
        a.stage = 0;
        a.since = 0;
    }
    const bool running = state == kStateStand && !boosted && speed >= maxSpeed * 0.9f &&
                         speed <= maxSpeed * 1.05f;
    const bool sprinting = state == kStateStand && boosted && speed >= maxSpeed * 1.3f;
    if (a.stage < 2)
    {
        if (!running)
        {
            a.stage = 0;
            a.since = 0;
            return;
        }
        if (!a.since)
            a.since = now;
        if (a.stage == 0 && now - a.since >= kAnimSteadyMs)
        {
            Snapshot(soldier, a.run1);
            if (p == 0)
                DeepSnapshot(a.run1, g_deep[0]);
            a.shot1 = now;
            a.stage = 1;
        }
        else if (a.stage == 1 && now - a.shot1 >= kAnimGapMs)
        {
            Snapshot(soldier, a.run2);
            if (p == 0)
                DeepSnapshot(a.run2, g_deep[1]);
            a.stage = 2;
            a.since = 0;
            if (AnimBudget())
                Log("[anim] %lu ms: player %d: running snapshots taken (%.2f m/s) - now sprint for a "
                    "second",
                    ScanMsSinceAttach(), p + 1, static_cast<double>(speed));
        }
        return;
    }
    if (!sprinting)
    {
        a.since = 0;
        return;
    }
    if (!a.since)
        a.since = now;
    if (now - a.since >= kAnimSteadyMs)
    {
        AnimCompare(p, soldier, speed, maxSpeed);
        if (p == 0)
        {
            uint32_t words[kSoldierDwords];
            Snapshot(soldier, words);
            DeepSnapshot(words, g_deep[2]);
            DeepCompare(p);
        }
        a.stage = 3;
    }
}


// ---------------------------------------------------------------------------
// 14e-r2: write watch on player 1's animation channel fields
// ---------------------------------------------------------------------------
constexpr uint32_t kWatchOff[2] = { 0x8CC, 0x8D4 };
const char* const kWatchName[2] = { "velocity x +0x8CC", "velocity z +0x8D4" };
constexpr DWORD kWatchArmAfterMs = 1000;
constexpr DWORD kWatchForMs = 15000;
constexpr DWORD kWatchFlushMs = 5000;
constexpr int kMaxHits = 32;

struct WatchHit
{
    uint32_t eip;
    uint32_t caller;
    uint32_t value;
    LONG count;
    bool logged;
};

struct DrRequest
{
    DWORD tid;
    uintptr_t addr[2];
    bool arm;
};

WatchHit g_hits[2][kMaxHits];
int g_hitCount[2] = {};
long g_hitOverflow[2] = {};
PVOID g_veh = nullptr;
DrRequest g_drReq = {};
volatile LONG g_drBusy = 0;
volatile LONG g_drResult = 0; // 0 none, 1 ok, 2 failed
int g_watchStage = 0;         // 0 waiting, 1 arming, 2 armed, 3 disarming, 4 done
uintptr_t g_watchSoldier = 0;
DWORD g_watchSince = 0;
DWORD g_watchArmedAt = 0;
DWORD g_watchFlush = 0;
DWORD g_gameTid = 0;
uintptr_t g_stackTop = 0; // end of the game thread's committed stack (for the caller read)
uintptr_t g_exeLo = 0;
uintptr_t g_exeHi = 0;
long g_totalHits = 0;
constexpr long kMaxTotalHits = 200000;

void RecordHit(int w, const CONTEXT* c)
{
    // Never read the watched memory here: with a read watch that would fire
    // the breakpoint again inside this handler (r3 crash).
    ++g_totalHits;
    if (c->Eip < g_exeLo || c->Eip >= g_exeHi)
        return; // our own code (the sprint hook reads the velocity too)
    uint32_t caller = 0;
    const uintptr_t ebp = c->Ebp;
    if (g_stackTop && ebp >= c->Esp && ebp + 8 <= g_stackTop)
        memcpy(&caller, reinterpret_cast<const void*>(ebp + 4), sizeof(caller));
    for (int i = 0; i < g_hitCount[w]; ++i)
    {
        if (g_hits[w][i].eip == c->Eip)
        {
            ++g_hits[w][i].count;
            return;
        }
    }
    if (g_hitCount[w] >= kMaxHits)
    {
        ++g_hitOverflow[w];
        return;
    }
    WatchHit& h = g_hits[w][g_hitCount[w]++];
    h.eip = c->Eip;
    h.caller = caller;
    h.value = 0;
    h.count = 1;
    h.logged = false;
}

LONG CALLBACK WatchVeh(EXCEPTION_POINTERS* ep)
{
    if (ep->ExceptionRecord->ExceptionCode != EXCEPTION_SINGLE_STEP)
        return EXCEPTION_CONTINUE_SEARCH;
    CONTEXT* c = ep->ContextRecord;
    const DWORD fired = c->Dr6 & 3;
    // Any stage once armed (the helper thread may set or clear the registers a
    // frame before this thread sees the new stage): our breakpoint must never
    // reach the crash logger or the system.
    if (!fired || g_watchStage < 1 || GetCurrentThreadId() != g_gameTid)
        return EXCEPTION_CONTINUE_SEARCH;
    for (int w = 0; w < 2; ++w)
        if (fired & (1u << w))
            RecordHit(w, c);
    c->Dr6 = 0;
    return EXCEPTION_CONTINUE_EXECUTION;
}

// Helper thread: suspends the game thread, sets or clears DR0 / DR1, resumes.
DWORD WINAPI DrThread(LPVOID)
{
    LONG result = 2;
    const HANDLE h = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME,
                                FALSE, g_drReq.tid);
    if (h)
    {
        if (SuspendThread(h) != static_cast<DWORD>(-1))
        {
            CONTEXT c;
            memset(&c, 0, sizeof(c));
            c.ContextFlags = CONTEXT_DEBUG_REGISTERS;
            if (GetThreadContext(h, &c))
            {
                c.Dr7 &= ~static_cast<DWORD>(0x00FF000F);
                if (g_drReq.arm)
                {
                    c.Dr0 = static_cast<DWORD>(g_drReq.addr[0]);
                    c.Dr1 = static_cast<DWORD>(g_drReq.addr[1]);
                    // L0, L1; RW0 = RW1 = 11 (read or write); LEN0 = LEN1 = 11 (4 bytes)
                    c.Dr7 |= 0x1u | 0x4u | (3u << 16) | (3u << 18) | (3u << 20) | (3u << 22);
                }
                else
                {
                    c.Dr0 = 0;
                    c.Dr1 = 0;
                }
                c.Dr6 = 0;
                if (SetThreadContext(h, &c))
                    result = 1;
            }
            ResumeThread(h);
        }
        CloseHandle(h);
    }
    InterlockedExchange(&g_drResult, result);
    InterlockedExchange(&g_drBusy, 0);
    return 0;
}

bool StartDr(bool arm)
{
    if (InterlockedCompareExchange(&g_drBusy, 1, 0) != 0)
        return false;
    g_drReq.arm = arm;
    g_drReq.tid = g_gameTid;
    InterlockedExchange(&g_drResult, 0);
    const HANDLE t = CreateThread(nullptr, 0, DrThread, nullptr, 0, nullptr);
    if (!t)
    {
        InterlockedExchange(&g_drBusy, 0);
        return false;
    }
    CloseHandle(t);
    return true;
}

void FlushHits(bool final)
{
    for (int w = 0; w < 2; ++w)
    {
        for (int i = 0; i < g_hitCount[w]; ++i)
        {
            WatchHit& h = g_hits[w][i];
            if (h.logged && !final)
                continue;
            if (!AnimBudget())
                return;
            const uint32_t base = static_cast<uint32_t>(ExeBase());
            Log("[anim] %lu ms: %s touched by code at rva 0x%08X (caller rva 0x%08X) - %ld times%s",
                ScanMsSinceAttach(), kWatchName[w], h.eip - base,
                h.caller >= base ? h.caller - base : 0, static_cast<long>(h.count),
                final ? " [final]" : "");
            h.logged = true;
        }
        if (final && g_hitOverflow[w] && AnimBudget())
            Log("[anim] %s: %ld more hits from places not listed (32-place limit)", kWatchName[w],
                g_hitOverflow[w]);
    }
}

// Player 1's soldier, every frame (inside Move's hook).
void WatchFrame(uintptr_t soldier, float speed, bool boosted, DWORD now)
{
    (void)speed;
    (void)boosted;
    if (g_watchStage == 4)
        return;
    if (soldier != g_watchSoldier)
    {
        if (g_watchStage == 2)
        {
            g_watchStage = 3; // the soldier changed: take the breakpoints off
        }
        else if (g_watchStage == 0)
        {
            g_watchSoldier = soldier;
            g_watchSince = now;
        }
    }
    if (g_watchStage == 0 && !g_animWatchOn)
    {
        g_watchStage = 4;
        if (AnimBudget())
            Log("[anim] BF1GameExt.ini [Sprint] AnimWatch=0: no velocity watch");
        return;
    }
    if (g_watchStage == 0 && now - g_watchSince >= kWatchArmAfterMs)
    {
        const BuildInfo* b = CurrentBuild();
        g_exeLo = ExeBase();
        g_exeHi = ExeBase() + (b ? b->sizeOfImage : 0);
        if (!g_veh)
            g_veh = AddVectoredExceptionHandler(1, WatchVeh);
        g_gameTid = GetCurrentThreadId();
        MEMORY_BASIC_INFORMATION mbi;
        const int here = 0;
        if (VirtualQuery(&here, &mbi, sizeof(mbi)))
            g_stackTop = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
        g_drReq.addr[0] = soldier + kWatchOff[0];
        g_drReq.addr[1] = soldier + kWatchOff[1];
        if (g_veh && StartDr(true))
        {
            g_watchStage = 1;
            if (AnimBudget())
                Log("[anim] %lu ms: arming the velocity read watch on player 1's soldier 0x%08X (+0x8CC, +0x8D4)",
                    ScanMsSinceAttach(), static_cast<unsigned>(soldier));
        }
        else
        {
            g_watchStage = 4;
            if (AnimBudget())
                Log("[anim] write watch could not start (handler %p) - skipped", g_veh);
        }
        return;
    }
    if (g_watchStage == 1 && !g_drBusy)
    {
        if (g_drResult == 1)
        {
            g_watchStage = 2;
            g_watchArmedAt = now;
            g_watchFlush = now;
            if (AnimBudget())
                Log("[anim] %lu ms: velocity watch ARMED - walk, run, sprint, stand, for %lu s",
                    ScanMsSinceAttach(), static_cast<unsigned long>(kWatchForMs / 1000));
        }
        else
        {
            g_watchStage = 4;
            if (AnimBudget())
                Log("[anim] write watch: setting the debug registers failed - skipped");
        }
        return;
    }
    if (g_watchStage == 2)
    {
        if (now - g_watchFlush >= kWatchFlushMs)
        {
            g_watchFlush = now;
            FlushHits(false);
        }
        if (now - g_watchArmedAt >= kWatchForMs)
            g_watchStage = 3;
        if (g_totalHits >= kMaxTotalHits)
        {
            g_watchStage = 3;
            if (AnimBudget())
                Log("[anim] %lu ms: %ld hits - taking the watch off early", ScanMsSinceAttach(),
                    g_totalHits);
        }
    }
    if (g_watchStage == 3 && StartDr(false))
    {
        g_watchStage = 4;
        FlushHits(true);
        if (AnimBudget())
            Log("[anim] %lu ms: velocity watch taken off (%ld hits in all)", ScanMsSinceAttach(),
                g_totalHits);
    }
}

// Every Move call (any soldier): take the watch off if player 1 is gone.
void WatchIdleCheck(DWORD now)
{
    if (g_watchStage == 2 && now - g_player[0].lastMove > 300)
        g_watchStage = 3;
    if (g_watchStage == 3 && StartDr(false))
    {
        g_watchStage = 4;
        FlushHits(true);
        if (AnimBudget())
            Log("[anim] %lu ms: write watch taken off (player 1 not on foot)", ScanMsSinceAttach());
    }
}

void PlayerMove(int p, uintptr_t soldier, uint32_t* entry)
{
    Player& pl = g_player[p];
    const DWORD now = GetTickCount();
    ++pl.calls5s;
    pl.lastMove = now;

    if (soldier != pl.soldier)
    {
        NewSoldier(p, soldier);
        if (p == 0 && g_viewReady)
        {
            const uintptr_t view = SoldierView(soldier);
            if (view)
                LogView(0, soldier, view);
        }
    }
    if (p == 1)
        P2ViewFrame(soldier);
    else if (p >= 2)
        ViewFrameN(p, soldier); // 20e-r2: players 3 and 4 in third person too
    StaminaHudFrame(p, soldier); // 14c: HUD scan (stamina_hud.cpp)

    const int32_t state = static_cast<int32_t>(ReadU32(soldier + kState));
    if (state != pl.state)
    {
        if (Budget())
            Log("[sprint] %lu ms: player %d state %ld (%s)", ScanMsSinceAttach(), p + 1,
                static_cast<long>(state), StateName(state));
        pl.state = state;
    }

    float fwd = 0.0f;
    float strafe = 0.0f;
    memcpy(&fwd, entry + 2, sizeof(fwd));
    memcpy(&strafe, entry + 3, sizeof(strafe));
    const float speed = HorizontalSpeed(soldier);

    int8_t counters[kUnitButtonCount];
    ReadCounters(soldier, counters);

    const char* by = "";
    const bool held = SprintHeld(p, &by);
    if (held && !pl.held)
        ButtonDown(p, soldier, by, state, speed, now);
    else if (!held && pl.held)
        ButtonUp(p, now);
    pl.held = held;

    if (pl.watchCalls > 0)
    {
        --pl.watchCalls;
        CheckConflict(p, counters);
    }
    memcpy(pl.prevCounter, counters, sizeof(counters));

    float dt = 0.0f;
    memcpy(&dt, entry + 1, sizeof(dt));
    if (!(dt > 0.0f))
        dt = 0.0f;
    if (dt > 0.1f)
        dt = 0.1f;

    const bool want = g_testBoost && held && state == kStateStand && fwd > kMinInput;
    bool boost = false;
    if (!held)
        pl.needRelease = false;
    if (!g_stamina)
    {
        boost = want;
        if (boost)
            pl.sprintTime += dt;
        else
            pl.sprintTime = 0.0f;
    }
    else if (want && !pl.needRelease && (pl.sprinting || pl.stamina >= g_staminaMin))
    {
        pl.sprinting = true;
        pl.sprintTime += dt;
        pl.stamina -= g_staminaDrain * dt;
        boost = true;
        if (pl.stamina <= 0.0f)
        {
            pl.stamina = 0.0f;
            pl.sprinting = false;
            pl.needRelease = true;
            pl.sprintTime = 0.0f;
            if (Budget())
                Log("[sprint] %lu ms: player %d out of stamina - sprint stops (let go of the button to "
                    "sprint again once it is back to %.0f)",
                    ScanMsSinceAttach(), p + 1, static_cast<double>(g_staminaMin));
        }
    }
    else
    {
        pl.sprinting = false;
        pl.sprintTime = 0.0f;
        const bool movingNow = std::fabs(fwd) > 0.05f || std::fabs(strafe) > 0.05f;
        pl.stamina += (movingNow ? g_staminaRegen : g_staminaRegenIdle) * dt;
        if (pl.stamina > g_staminaMax)
            pl.stamina = g_staminaMax;
    }
    if (held && pl.stamina < pl.lowest)
        pl.lowest = pl.stamina;

    float boostedFwd = fwd;
    if (boost)
    {
        const float ramp = g_accelTime > 0.0f && pl.sprintTime < g_accelTime
                               ? pl.sprintTime / g_accelTime
                               : 1.0f;
        boostedFwd = fwd * (1.0f + (g_mult - 1.0f) * ramp);
        memcpy(entry + 2, &boostedFwd, sizeof(boostedFwd));
        ++pl.boosted5s;
        ++pl.heldBoosted;
    }

    AnimScanFrame(p, soldier, state, boost, speed, pl.maxSpeed, now); // 14e
    if (p == 0)
        WatchFrame(soldier, speed, boost, now); // 14e-r2
    if (held)
    {
        if (speed > pl.topHeld)
            pl.topHeld = speed;
    }
    else if (state == kStateStand && speed > pl.topNormal)
    {
        pl.topNormal = speed;
    }

    const float stickY = PadStickY(PlayerPad(p));
    const bool keyW = p == 0 && KeyDown('W') && GameWindowInFront();
    const bool keyS = p == 0 && KeyDown('S') && GameWindowInFront();

    // Which sign is "forward": the first time the stick or W/S clearly agrees.
    if (!pl.signLogged && std::fabs(fwd) > 0.5f &&
        (std::fabs(stickY) > 0.5f || keyW != keyS) && Budget())
    {
        pl.signLogged = true;
        const bool pushedForward = std::fabs(stickY) > 0.5f ? stickY > 0.0f : keyW;
        Log("[sprint] %lu ms: player %d forward-input sign: %s %s gives forward input %+.2f -> "
            "\"forward\" is %s",
            ScanMsSinceAttach(), p + 1, std::fabs(stickY) > 0.5f ? "left stick" : "key",
            pushedForward ? "pushed forward (up / W)" : "pulled back (down / S)",
            static_cast<double>(fwd),
            (fwd > 0.0f) == pushedForward ? "POSITIVE" : "NEGATIVE");
    }

    const bool moving = std::fabs(fwd) > 0.05f || std::fabs(strafe) > 0.05f || speed > 0.3f;
    if (moving && now - pl.lastSample >= kSampleMs && g_sampleLines < kMaxSampleLines)
    {
        pl.lastSample = now;
        ++g_sampleLines;
        char boostText[48] = "";
        if (boost)
            snprintf(boostText, sizeof(boostText), " -> %+.2f BOOSTED", static_cast<double>(boostedFwd));
        Log("[sprint] %lu ms: P%d %s | forward %+.2f%s, strafe %+.2f | stick Y %+.2f%s | speed %.2f "
            "m/s (MaxSpeed %.2f, vertical %+.2f) | sprint %s | stamina %.0f",
            ScanMsSinceAttach(), p + 1, StateName(state), static_cast<double>(fwd), boostText,
            static_cast<double>(strafe), static_cast<double>(stickY),
            p == 0 ? (keyW ? ", W" : (keyS ? ", S" : "")) : "", static_cast<double>(speed),
            static_cast<double>(pl.maxSpeed),
            static_cast<double>(ReadF32(soldier + kVelocity + 4)), held ? by : "-",
            static_cast<double>(pl.stamina));
        if (g_sampleLines == kMaxSampleLines)
            Log("[sprint] sample limit reached, samples going quiet");
    }
}

void MaybeSummary()
{
    const DWORD now = GetTickCount();
    if (now - g_lastSummary < kSummaryMs)
        return;
    g_lastSummary = now;
    const long p1 = g_player[0].calls5s;
    const long p2 = g_player[1].calls5s;
    if ((p1 || p2 || g_otherCalls5s) && g_summaryLines < kMaxSummaryLines)
    {
        ++g_summaryLines;
        const char* note = "";
        if (g_unit[0] && !p1)
            note = " - player 1 has a unit but no Move call was his (in a vehicle, or the "
                   "soldier+0x250 link is wrong)";
        Log("[sprint] %lu ms: last 5 s: Move calls player 1 %ld, player 2 %ld, other soldiers %ld | "
            "boosted frames player 1 %ld, player 2 %ld | units 0x%08X / 0x%08X%s",
            ScanMsSinceAttach(), p1, p2, g_otherCalls5s, g_player[0].boosted5s,
            g_player[1].boosted5s, static_cast<unsigned>(g_unit[0]),
            static_cast<unsigned>(g_unit[1]), note);
        if (g_summaryLines == kMaxSummaryLines)
            Log("[sprint] summary limit reached, summaries going quiet");
    }
    g_player[0].calls5s = g_player[1].calls5s = 0;
    g_player[0].boosted5s = g_player[1].boosted5s = 0;
    g_otherCalls5s = 0;
}

// Called by the stub at Move's entry, on the game thread, for every soldier.
void __stdcall SprintOnMove(uintptr_t soldier, uint32_t* entry)
{
    ++g_totalCalls;
    if (g_totalCalls == 1)
        Log("[sprint] %lu ms: first Move call through the hook (soldier 0x%08X)",
            ScanMsSinceAttach(), static_cast<unsigned>(soldier));

    RefreshUnits();
    const uintptr_t ctl = soldier + kControllable;
    int p = -1;
    if (g_unit[0] && ctl == g_unit[0])
        p = 0;
    else if (g_unit[1] && ctl == g_unit[1])
        p = 1;
    else if (g_unit[2] && ctl == g_unit[2]) // 20e-r2
        p = 2;
    else if (g_unit[3] && ctl == g_unit[3])
        p = 3;

    if (p >= 0)
        PlayerMove(p, soldier, entry);
    else
        ++g_otherCalls5s;
    WatchIdleCheck(GetTickCount()); // 14e-r2
    MaybeSummary();
}

void PutRel32(unsigned char* s, size_t& i, uintptr_t target)
{
    const int32_t rel = static_cast<int32_t>(target - (reinterpret_cast<uintptr_t>(s + i) + 4));
    memcpy(s + i, &rel, sizeof(rel));
    i += 4;
}

// The stub described at the top of this file. Returns its length (26 bytes).
size_t BuildStub(unsigned char* s, uintptr_t site)
{
    size_t i = 0;
    s[i++] = 0x60;                                          // pushad
    s[i++] = 0x9C;                                          // pushfd
    s[i++] = 0x8D; s[i++] = 0x44; s[i++] = 0x24; s[i++] = 0x24; // lea eax, [esp+24h]
    s[i++] = 0x50;                                          // push eax
    s[i++] = 0x51;                                          // push ecx
    s[i++] = 0xE8;                                          // call SprintOnMove
    PutRel32(s, i, reinterpret_cast<uintptr_t>(&SprintOnMove));
    s[i++] = 0x9D;                                          // popfd
    s[i++] = 0x61;                                          // popad
    memcpy(s + i, kEntryBytes, sizeof(kEntryBytes));         // push ebx; mov ebx, esp; sub esp, 8
    i += sizeof(kEntryBytes);
    s[i++] = 0xE9;                                          // jmp Move+6
    PutRel32(s, i, site + sizeof(kEntryBytes));
    return i;
}

bool InstallHook(uintptr_t site)
{
    unsigned char* stub = static_cast<unsigned char*>(
        VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!stub)
    {
        Log("[sprint] VirtualAlloc failed (error %lu) - NOT installed", GetLastError());
        return false;
    }
    const size_t n = BuildStub(stub, site);
    FlushInstructionCache(GetCurrentProcess(), stub, n);

    unsigned char patch[sizeof(kEntryBytes)];
    patch[0] = 0xE9;
    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(stub) - (site + 5));
    memcpy(patch + 1, &rel, sizeof(rel));
    patch[5] = 0x90;

    DWORD oldProt = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(site), sizeof(patch), PAGE_EXECUTE_READWRITE,
                        &oldProt))
    {
        Log("[sprint] VirtualProtect failed (error %lu) - NOT installed", GetLastError());
        return false;
    }
    memcpy(reinterpret_cast<void*>(site), patch, sizeof(patch));
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(site), sizeof(patch), oldProt, &ignored);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<LPCVOID>(site), sizeof(patch));

    Log("[sprint] hook installed at Move's entry: 6 bytes -> jmp 0x%08X (stub, %u bytes)",
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(stub)), static_cast<unsigned>(n));
    return true;
}


// ---------------------------------------------------------------------------
// 14b: helpers for code found through call sites
// ---------------------------------------------------------------------------
int32_t ReadI32(uintptr_t va)
{
    int32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

// Target RVA of the E8 at rva, following incremental-link E9 thunks (0 = none).
uint32_t CallTarget(uint32_t rva)
{
    uint32_t textRva = 0;
    uint32_t textSize = 0;
    if (!CodeSection(&textRva, &textSize))
        return 0;
    if (*reinterpret_cast<const unsigned char*>(Rva(rva)) != 0xE8)
        return 0;
    uint32_t t = static_cast<uint32_t>(static_cast<int64_t>(rva) + 5 + ReadI32(Rva(rva + 1)));
    for (int hop = 0; hop < 8; ++hop)
    {
        if (t < textRva || t >= textRva + textSize)
            return 0;
        if (*reinterpret_cast<const unsigned char*>(Rva(t)) != 0xE9)
            return t;
        t = static_cast<uint32_t>(static_cast<int64_t>(t) + 5 + ReadI32(Rva(t + 1)));
    }
    return 0;
}

bool CheckBody(const char* what, uint32_t rva, const char* body)
{
    const bool ok = rva && MatchPatternAt(rva, body);
    Log("[view]   %-18s rva 0x%08X  %s", what, rva, ok ? "OK" : "does NOT match");
    return ok;
}

bool InExe(uintptr_t va, size_t len)
{
    const BuildInfo* b = CurrentBuild();
    return b && va >= ExeBase() && va + len <= ExeBase() + b->sizeOfImage;
}

// ---------------------------------------------------------------------------
// 14b: L3 sprint only
// ---------------------------------------------------------------------------
void L3SprintOnly()
{
    if (!g_l3SprintOnly)
    {
        Log("[sprint] BF1GameExt.ini [Sprint] L3SprintOnly=0: L3 keeps the game's view toggle");
        return;
    }
    uint32_t hits[4] = {};
    const int n = FindPattern(kPadActionLoop, hits, 4);
    if (n != 1)
    {
        Log("[sprint] pad bit -> action loop: %d pattern hits (need 1) - L3 still toggles the view", n);
        return;
    }
    const uintptr_t table = ReadU32(Rva(hits[0] + kOffPadActionTable));
    if (!InExe(table, 16 * 4) || !Readable(table, 16 * 4))
    {
        Log("[sprint] pad bit -> action table at 0x%08X is not in the exe - L3 still toggles the view",
            static_cast<unsigned>(table));
        return;
    }
    for (int i = 0; i < 16; ++i)
    {
        const uint32_t v = ReadU32(table + static_cast<uint32_t>(i) * 4);
        const bool ok = v == kExpectedPadActions[i] || (i == kActionL3 && v == kUnusedPadBit);
        if (!ok)
        {
            Log("[sprint] pad bit -> action table entry %d is %u, expected %u - not changed (L3 still "
                "toggles the view)",
                i, static_cast<unsigned>(v), static_cast<unsigned>(kExpectedPadActions[i]));
            return;
        }
    }
    const uintptr_t slot = table + kActionL3 * 4;
    DWORD oldProt = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(slot), 4, PAGE_READWRITE, &oldProt))
    {
        Log("[sprint] VirtualProtect on the pad table failed (error %lu) - L3 still toggles the view",
            GetLastError());
        return;
    }
    const uint32_t unused = kUnusedPadBit;
    memcpy(reinterpret_cast<void*>(slot), &unused, sizeof(unused));
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(slot), 4, oldProt, &ignored);
    Log("[sprint] L3 sprint only: pad bit -> action table at rva 0x%08X (loop at rva 0x%08X, 16 "
        "entries as expected): action 6 (L3, the game's \"view\") now reads pad bit %u, which no "
        "pad sets - both players",
        static_cast<unsigned>(table - ExeBase()), hits[0], static_cast<unsigned>(kUnusedPadBit));
}

// ---------------------------------------------------------------------------
// 14b: player 2 in third person
// ---------------------------------------------------------------------------
bool ViewBudget()
{
    if (g_viewLines >= kMaxViewLines)
        return false;
    ++g_viewLines;
    if (g_viewLines == kMaxViewLines)
        Log("[view] line limit reached, going quiet");
    return true;
}

typedef uintptr_t(__fastcall* VGet_t)(void* self, void* edx);

// The camera mode IsFirstPerson looks at (1 = forced third, 2 = forced first,
// anything else = the view's own flag), or -1 if there is no camera.
int CameraMode(uintptr_t view)
{
    const uintptr_t cam = ReadU32(view + kViewCamera);
    if (!cam || !Readable(cam, 4))
        return -1;
    const uintptr_t vt = ReadU32(cam);
    if (!InExe(vt + 0x24, 4))
        return -1;
    const uintptr_t a = reinterpret_cast<VGet_t>(ReadU32(vt + 0x24))(reinterpret_cast<void*>(cam), nullptr);
    if (!a || !Readable(a, 4))
        return -1;
    const uintptr_t vt2 = ReadU32(a);
    if (!InExe(vt2 + 0x58, 4))
        return -1;
    const uintptr_t b = reinterpret_cast<VGet_t>(ReadU32(vt2 + 0x58))(reinterpret_cast<void*>(a), nullptr);
    if (!b || !Readable(b + 0x14C, 4))
        return -1;
    return static_cast<int>(ReadU32(b + 0x14C));
}

unsigned ProfileFirstPerson(int k)
{
    const BuildInfo* b = CurrentBuild();
    if (!b || !b->profileArrayRva)
        return 0xFF;
    const uintptr_t p = Rva(b->profileArrayRva) + static_cast<uint32_t>(k) * kProfileSize;
    if (!Readable(p + kProfileFirstPerson, 1))
        return 0xFF;
    return *reinterpret_cast<const uint8_t*>(p + kProfileFirstPerson);
}

void LogView(int p, uintptr_t soldier, uintptr_t view)
{
    if (!ViewBudget())
        return;
    const bool first = g_isFirstPerson(reinterpret_cast<void*>(view), nullptr);
    Log("[view] %lu ms: player %d's view 0x%08X (soldier 0x%08X): %s | flag +0x14 = %u, camera 0x%08X, "
        "camera mode %d | character index %ld | gates %u / %u | profile first-person byte: profile 1 "
        "= %u, profile 2 = %u",
        ScanMsSinceAttach(), p + 1, static_cast<unsigned>(view), static_cast<unsigned>(soldier),
        first ? "FIRST person" : "third person",
        static_cast<unsigned>(*reinterpret_cast<const uint8_t*>(view + kViewFlag)),
        static_cast<unsigned>(ReadU32(view + kViewCamera)), CameraMode(view),
        static_cast<long>(static_cast<int32_t>(ReadU32(soldier + kControllable + kCharIndex))),
        g_gateA ? static_cast<unsigned>(*reinterpret_cast<const uint8_t*>(g_gateA)) : 0xFFu,
        g_gateB ? static_cast<unsigned>(*reinterpret_cast<const uint8_t*>(g_gateB)) : 0xFFu,
        ProfileFirstPerson(0), ProfileFirstPerson(1));
}

uintptr_t SoldierView(uintptr_t soldier)
{
    const uintptr_t view = ReadU32(soldier + kViewHolder + 4);
    return view && Readable(view, kViewFlag + 1) ? view : 0;
}

// Player 2's soldier, every frame (inside Move's hook, on the game thread).
void P2ViewFrame(uintptr_t soldier)
{
    if (!g_viewReady)
        return;
    const uintptr_t view = SoldierView(soldier);
    if (soldier != g_p2ViewSoldier)
    {
        g_p2ViewSoldier = soldier;
        g_p2ViewState = -1;
        g_p2LastTry = 0;
        if (view)
            LogView(1, soldier, view);
        else if (ViewBudget())
            Log("[view] %lu ms: player 2's soldier 0x%08X has no view object", ScanMsSinceAttach(),
                static_cast<unsigned>(soldier));
    }
    if (!view)
        return;

    const bool first = g_isFirstPerson(reinterpret_cast<void*>(view), nullptr);
    if (!first)
    {
        g_p2ViewState = 0;
        return;
    }
    const DWORD now = GetTickCount();
    if (g_p2ViewState == 1 && now - g_p2LastTry < kViewRetryMs)
        return;
    g_p2LastTry = now;

    const int index = static_cast<int>(ReadU32(soldier + kControllable + kCharIndex));
    const unsigned flagBefore = *reinterpret_cast<const uint8_t*>(view + kViewFlag);
    g_setFirstPerson(reinterpret_cast<void*>(view), nullptr, index, 0);
    const bool still = g_isFirstPerson(reinterpret_cast<void*>(view), nullptr);
    if (ViewBudget())
        Log("[view] %lu ms: player 2 was in first person (flag %u, camera mode %d) - "
            "SetFirstPerson(view, %d, 0): %s",
            ScanMsSinceAttach(), flagBefore, CameraMode(view), index,
            still ? "STILL first person (retrying once a second)" : "now third person");
    g_p2ViewState = still ? 1 : 0;
}

// 20e-r2: player 3 / 4's soldier, every frame - the same as P2ViewFrame, with their own state.
void ViewFrameN(int p, uintptr_t soldier)
{
    if (!g_viewReady || p < 2 || p > 3)
        return;
    const uintptr_t view = SoldierView(soldier);
    if (soldier != g_nViewSoldier[p])
    {
        g_nViewSoldier[p] = soldier;
        g_nViewState[p] = -1;
        g_nLastTry[p] = 0;
        if (view)
            LogView(p, soldier, view);
    }
    if (!view)
        return;
    const bool first = g_isFirstPerson(reinterpret_cast<void*>(view), nullptr);
    if (!first)
    {
        g_nViewState[p] = 0;
        return;
    }
    const DWORD now = GetTickCount();
    if (g_nViewState[p] == 1 && now - g_nLastTry[p] < kViewRetryMs)
        return;
    g_nLastTry[p] = now;
    const int index = static_cast<int>(ReadU32(soldier + kControllable + kCharIndex));
    g_setFirstPerson(reinterpret_cast<void*>(view), nullptr, index, 0);
    const bool still = g_isFirstPerson(reinterpret_cast<void*>(view), nullptr);
    if (ViewBudget())
        Log("[view] %lu ms: player %d was in first person - SetFirstPerson(view, %d, 0): %s", ScanMsSinceAttach(),
            p + 1, index, still ? "STILL first person (retrying once a second)" : "now third person");
    g_nViewState[p] = still ? 1 : 0;
}

void ResolveView()
{
    if (!g_p2Third)
    {
        Log("[view] BF1GameExt.ini [Splitscreen] P2ThirdPerson=0: player 2's view is left alone");
        return;
    }
    uint32_t hits[4] = {};
    const int n = FindPattern(kViewToggle, hits, 4);
    if (n != 1)
    {
        Log("[view] soldier view toggle: %d pattern hits (need 1) - player 2 third person disabled", n);
        return;
    }
    const uint32_t site = hits[0];
    Log("[view] soldier view toggle at rva 0x%08X (soldier +0x296 view button, +0x44C view holder)", site);
    const uint32_t gateA = CallTarget(site + kOffGateA);
    const uint32_t gateB = CallTarget(site + kOffGateB);
    const uint32_t holder = CallTarget(site + kOffViewHolder);
    const uint32_t isFp = CallTarget(site + kOffIsFirstPerson);
    const uint32_t index = CallTarget(site + kOffCharIndex);
    const uint32_t setFp = CallTarget(site + kOffSetFirstPerson);
    const bool okA = CheckBody("gate A", gateA, kGateBody);
    const bool okB = CheckBody("gate B", gateB, kGateBody);
    const bool okH = CheckBody("view holder", holder, kViewHolderBody);
    const bool okI = CheckBody("IsFirstPerson", isFp, kIsFirstPersonBody);
    const bool okC = CheckBody("character index", index, kCharIndexBody);
    const bool okS = CheckBody("SetFirstPerson", setFp, kSetFirstPersonBody);
    if (!(okA && okB && okH && okI && okC && okS))
    {
        Log("[view] code does not match the GOG 1.3 analysis - player 2 third person disabled");
        return;
    }
    g_gateA = ReadU32(Rva(gateA + 1));
    g_gateB = ReadU32(Rva(gateB + 1));
    if (!InExe(g_gateA, 1) || !InExe(g_gateB, 1))
    {
        g_gateA = g_gateB = 0;
        Log("[view] gate addresses are not in the exe - logged as 0xFF");
    }
    g_isFirstPerson = reinterpret_cast<IsFirstPerson_t>(Rva(isFp));
    g_setFirstPerson = reinterpret_cast<SetFirstPerson_t>(Rva(setFp));
    g_viewReady = true;
    Log("[view] player 2 third person ON: whenever player 2's soldier is in first person, the game's "
        "SetFirstPerson(view, index, 0) switches it back");
}


// ---------------------------------------------------------------------------
// 14f: the move selector's 8 m/s cap
// ---------------------------------------------------------------------------
const char* const kMoveSelectStart =
    "55 8B EC F3 0F 10 55 08 83 EC 18 F3 0F 10 5D 0C 0F 28 CA 0F 28 C3 F3 0F 59 CA F3 0F 59 C3 56 "
    "8B F1 F3 0F 58 C8 0F 2F 0D ?? ?? ?? ?? 0F 87";
constexpr uint32_t kOffCapOperand = 40; // the comiss's absolute address

void RaiseAnimSpeedCap()
{
    if (g_animSpeedLimit <= 8.0f)
    {
        Log("[sprint] BF1GameExt.ini [Sprint] AnimSpeedLimit=%.1f: the game's 8 m/s run-animation cap is "
            "left as it is",
            static_cast<double>(g_animSpeedLimit));
        return;
    }
    uint32_t hits[4] = {};
    const int n = FindPattern(kMoveSelectStart, hits, 4);
    if (n != 1)
    {
        Log("[sprint] move animation selector: %d pattern hits (need 1) - the 8 m/s cap stays (standing "
            "pose above it)",
            n);
        return;
    }
    const uintptr_t operand = Rva(hits[0] + kOffCapOperand);
    const uintptr_t oldAddr = ReadU32(operand);
    const BuildInfo* b = CurrentBuild();
    if (!b || oldAddr < ExeBase() || oldAddr + 4 > ExeBase() + b->sizeOfImage || !Readable(oldAddr, 4) ||
        ReadF32(oldAddr) != 64.0f)
    {
        Log("[sprint] move animation selector at rva 0x%08X: its cap does not read 64.0 (address 0x%08X) "
            "- not changed",
            hits[0], static_cast<unsigned>(oldAddr));
        return;
    }
    const uint32_t ours = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&g_animLimitSq));
    DWORD oldProt = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(operand), 4, PAGE_EXECUTE_READWRITE, &oldProt))
    {
        Log("[sprint] VirtualProtect on the move selector failed (error %lu) - the 8 m/s cap stays",
            GetLastError());
        return;
    }
    memcpy(reinterpret_cast<void*>(operand), &ours, sizeof(ours));
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(operand), 4, oldProt, &ignored);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<LPCVOID>(operand), 4);
    Log("[sprint] run-animation cap raised: move animation selector at rva 0x%08X compared forward^2 + "
        "strafe^2 with 64.0 (8 m/s, the game's constant at rva 0x%08X, left untouched); it now reads "
        "BF1GameExt's %.1f (%.1f m/s) - above it the soldier still gets the standing pose",
        hits[0], static_cast<unsigned>(oldAddr - ExeBase()), static_cast<double>(g_animLimitSq),
        static_cast<double>(g_animSpeedLimit));
}

} // namespace

int SprintHudState(int p, float* fraction)
{
    if (p < 0 || p > 3 || !g_stamina || g_staminaMax <= 0.0f) // 20g-r5: players 3 and 4 too
        return 0;
    if (!g_testBoost)
        return 0; // 21j-r17: sprint switched off (the MODS page / TestBoost=0): no stamina bar
    const Player& pl = g_player[p];
    if (!pl.soldier || GetTickCount() - pl.lastMove > 250)
        return 0;
    float f = pl.stamina / g_staminaMax;
    if (f < 0.0f)
        f = 0.0f;
    if (f > 1.0f)
        f = 1.0f;
    *fraction = f;
    return pl.needRelease || (!pl.sprinting && pl.stamina < g_staminaMin) ? 2 : 1;
}

float SprintStamina01(int p)
{
    if (p < 0 || p > 3 || !g_stamina || g_staminaMax <= 0.0f)
        return 1.0f;
    return g_player[p].stamina / g_staminaMax;
}

void SprintStart(bool onGameThread)
{
    Log("[sprint] sprint.cpp file build %s", kFileBuild);
    const BuildInfo* build = CurrentBuild();
    if (!build || build->loadsMemExt)
    {
        Log("[sprint] not used on this build");
        return;
    }
    ReadIni();

    uint32_t hits[4] = {};
    const int n = FindPattern(kMovePattern, hits, 4);
    if (n != 1)
    {
        Log("[sprint] EntitySoldier::Move: %d pattern hits (need exactly 1) - sprint disabled", n);
        return;
    }
    const uintptr_t move = Rva(hits[0]);
    Log("[sprint] EntitySoldier::Move at rva 0x%08X (1 pattern hit)", hits[0]);

    if (memcmp(reinterpret_cast<const void*>(move), kEntryBytes, sizeof(kEntryBytes)) != 0)
    {
        Log("[sprint] Move's first 6 bytes are not push ebx / mov ebx, esp / sub esp, 8 - sprint "
            "disabled");
        return;
    }
    if (!Readable(move, kBodyWindow))
    {
        Log("[sprint] Move's body is not readable - sprint disabled");
        return;
    }

    bool ok = true;
    for (const BodyCheck& c : kBodyChecks)
    {
        uint32_t first = 0;
        const int count = CountInWindow(move, kBodyWindow, c.hex, &first);
        const bool good = count >= c.minCount && count <= c.maxCount;
        if (good)
            Log("[sprint]   %-48s x%d (first at Move+0x%03X)  OK", c.what, count,
                static_cast<unsigned>(first));
        else
            Log("[sprint]   %-48s x%d (need %d..%d)  MISMATCH", c.what, count, c.minCount,
                c.maxCount);
        ok = ok && good;
    }
    if (!ok)
    {
        Log("[sprint] Move's body does not match the GOG 1.3 analysis - sprint disabled");
        return;
    }

    Log("[sprint] game pad (player 1) at 0x%08X, pad 2 at 0x%08X; sprint button: L3 on the "
        "player's pad, Left Shift for player 1",
        static_cast<unsigned>(GamePadAddress()), static_cast<unsigned>(Pad2Address()));
    if (g_testBoost)
        Log("[sprint] BF1GameExt.ini [Sprint] TestBoost=1: while the button is held, standing and "
            "moving forward (forward input > %.1f), the forward input x %.2f",
            static_cast<double>(kMinInput), static_cast<double>(g_mult));
    else
        Log("[sprint] BF1GameExt.ini [Sprint] TestBoost=0: log only, no boost");
    if (g_stamina)
        Log("[sprint] stamina ON: max %.0f, drain %.1f/s sprinting, regen %.1f/s moving, %.1f/s still, "
            "%.0f needed to start, boost ramps up over %.2f s",
            static_cast<double>(g_staminaMax), static_cast<double>(g_staminaDrain),
            static_cast<double>(g_staminaRegen), static_cast<double>(g_staminaRegenIdle),
            static_cast<double>(g_staminaMin), static_cast<double>(g_accelTime));
    else
        Log("[sprint] BF1GameExt.ini [Sprint] Stamina=0: unlimited sprint");

    if (!onGameThread)
    {
        Log("[sprint] the scan did not run on the game thread - hook NOT installed (not safe)");
        return;
    }
    L3SprintOnly();
    ResolveView();
    RaiseAnimSpeedCap(); // 14f
    StaminaHudStart();
    g_hookOn = InstallHook(move);
}

// 21j-r17: the MODS page's SPRINT row
int SprintMenuState()
{
    if (!g_hookOn)
        return -1;
    return g_testBoost ? 1 : 0;
}

int SprintMenuSet(bool on)
{
    if (!g_hookOn)
        return -1;
    const bool was = g_testBoost;
    g_testBoost = on;
    const BOOL saved = g_iniPath[0] ? WritePrivateProfileStringA("Sprint", "TestBoost", on ? "1" : "0", g_iniPath) : FALSE;
    Log("[sprint] %lu ms: 21j-r17: the MODS page: SPRINT %s (was %s) - %s; saved in BF1GameExt.ini [Sprint] TestBoost=%d%s",
        ScanMsSinceAttach(), on ? "ON" : "OFF", was ? "ON" : "OFF",
        on ? "the boost while the button is held, the stamina bar back" : "no boost, no stamina bar (L3 stays sprint only)",
        on ? 1 : 0, saved ? "" : " - NOT SAVED (the write failed)");
    return g_testBoost ? 1 : 0;
}

} // namespace bf1
