// BF1GameExt - player2_input.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/splitscreen/; file names in the comments and log lines follow the new names. Nothing else changed.
// 21j-r11: the frame-rate scan's part around Player2Frame (render_hooks.cpp, kPerfOurInput; [perf] lines). Log only.
// 20e: players 3 and 4 (players34.cpp; nothing here changes for 2 players): PlayerInputBuild / Update /
// ControlStateFill give players34.cpp the game's PlayerInput code for their objects; the redirected getters
// (6c) answer indexes 2 and 3 with their objects (PlayerNInputObject - nullptr while 20e is off, then the game's
// own getter as before); and player 2's frame (after player 1's input update) ends with PlayerNInputFrame.
// 18v: Player2RedirectActive() - whether index 1 gets player 2's objects right now (split_spawn_menus.cpp
// lets the interface read player 2's input only then). Nothing else changed.
// Phase 6b (2020 build): builds player 2's input object and updates it from the
// game's frame; [p2input] lines log its output.
// Phase 6c: redirects the input manager's per-player getters for index 1 to
// player 2's objects (vtable +0x18/+0x1C/+0x20); [p2redirect] lines.
// Phase 7c: player 2's controls. Each frame the game copies player 1's control
// packet (buffer A: move/look floats + button bits) into control state 0, the
// struct player 1's PlayerController reads - but in a local game ONLY player
// 1's (the network path fills every player's). This does the same copy for
// player 2: buffer A 2 -> control state 1, with the game's own copy. [p2ctl]
//
// Found by pattern (proven on GOG 1.3, the same code as Steam):
//   input manager constructor: builds 4 per-player arrays with count 1
//     +0x1C  PlayerInput  0x718  (element constructor pushed at offset 9)
//     +0x734 buffer A     0x14   (offset 36)
//     +0x748 0x34 object         (offset 57, not needed here)
//     +0x77C buffer B     0x140  (offset 78)
//   input init wiring for player 1 (ecx = player 1's PlayerInput):
//     SetConnected(+0x4B5), SetFlag(+0x4B4) = 1, SetBufA(+0x4B8), SetBufB(+0x4B0)
//   manager frame update: lea ecx, [edi+1Ch]; ...; call PlayerInput::Update
//     (thiscall, float dt, ret 4) - this call is redirected (6b frame hook)
// PlayerInput device slots: +0x4BC pad, +0x4C0 keyboard, +0x4C4 mouse.
// The update null-checks all three; player 2 gets pad 2 and no keyboard/mouse.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
// Return address of the current function (who called the getter). Declared
// directly instead of including <intrin.h>, which clashes with some headers.
#ifdef _MSC_VER
extern "C" void* _ReturnAddress(void);
#pragma intrinsic(_ReturnAddress)
#define BF1_RETURN_ADDRESS() _ReturnAddress()
#else
#define BF1_RETURN_ADDRESS() __builtin_return_address(0)
#endif

#include "autospawn.hpp"
#include "render_hooks.hpp"
#include "game_build.hpp"
#include "log.hpp"
#include "pads.hpp"
#include "player2_input.hpp"
#include "players34.hpp"
#include "prone_log.hpp"
#include "game_check.hpp"

namespace bf1 {
namespace {

const char* const kManagerCtor =
    "56 8B F1 E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? 6A 01 68 18 07 00 00 8D 46 1C C7 06 ?? ?? ?? ?? 50 E8 "
    "?? ?? ?? ?? 68 ?? ?? ?? ?? 6A 01 6A 14 8D 86 34 07 00 00 50 E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? 6A "
    "01 6A 34 8D 86 48 07 00 00 50 E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? 6A 01 68 40 01 00 00 8D 86 7C 07 "
    "00 00 50 E8";
constexpr uint32_t kOffManagerVtable = 25; // mov dword ptr [esi], <vtable>
constexpr uint32_t kOffPlayerInputCtor = 9;
constexpr uint32_t kOffBufACtor = 36;
constexpr uint32_t kOffBufBCtor = 78;

const char* const kInitWiring =
    "B9 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 6A 01 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? B9 ?? ?? ?? "
    "?? E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? B9 ?? ?? ?? ?? E8 ?? ?? ?? ??";

const char* const kFrameSite =
    "F3 0F 10 45 08 51 8D 4F 1C F3 0F 11 04 24 E8 ?? ?? ?? ?? F3 0F 10 45 08 51 8D 8F 48 07 00 00 "
    "F3 0F 11 04 24 E8";
constexpr uint32_t kOffFrameCall = 14;

const char* const kPlayerInputCtorBody =
    "53 8B D9 0F 57 C0 56 57 B9 60 00 00 00 BE ?? ?? ?? ?? C7 03 ?? ?? ?? ??";
const char* const kPlayerInputUpdateBody =
    "55 8B EC 83 EC 30 56 57 8D 41 0C 89 4D F0 8D B9 8C 00 00 00 89 45 F4 68 80 00 00 00";
const char* const kBufACtorBody = "0F 57 C0 8B C1 0F 11 01 C7 41 10 00 00 00 80 C3";
const char* const kBufBCtorBody =
    "56 6A 68 8B F1 6A 00 56 E8 ?? ?? ?? ?? 6A 68 8D 86 D0 00 00 00 6A 00 50";
const char* const kSetConnectedBody = "55 8B EC 8A 45 08 88 81 B5 04 00 00 5D C2 04 00";
const char* const kSetFlagBody = "55 8B EC 8A 45 08 88 81 B4 04 00 00 5D C2 04 00";
const char* const kSetBufABody = "55 8B EC 8B 45 08 89 81 B8 04 00 00 5D C2 04 00";
const char* const kSetBufBBody = "55 8B EC 8B 45 08 89 81 B0 04 00 00 5D C2 04 00";

// Input manager virtual getters (thiscall, one index argument, ret 4)
//   +0x18 GetPlayerInput(i) = this + 0x1C  + i * 0x718
//   +0x1C GetBufA(i)        = this + 0x734 + i * 0x14
//   +0x20 GetBufB(i)        = this + 0x77C + i * 0x140
const char* const kGetPlayerInputBody =
    "55 8B EC 69 45 08 18 07 00 00 83 C0 1C 03 C1 5D C2 04 00";
const char* const kGetBufABody =
    "55 8B EC 8B 45 08 8D 04 80 8D 80 CD 01 00 00 8D 04 81 5D C2 04 00";
const char* const kGetBufBBody =
    "55 8B EC 8B 45 08 81 C1 7C 07 00 00 8D 04 80 C1 E0 06 03 C1 5D C2 04 00";
// Local controls copy (in the per-frame "collect controls" step):
//   cmp byte [<network flag>], 0 ; push ebx ; push esi ; push edi ; jne <network path>
//   call GetInputManager ; push 0 ; mov ecx, eax ; mov edx, [eax] ; call [edx+1Ch]
//   push eax ; push [<control state table>] ; call CopyPacket
const char* const kLocalControls =
    "80 3D ?? ?? ?? ?? 00 53 56 57 75 ?? E8 ?? ?? ?? ?? 6A 00 8B C8 8B 10 FF 52 1C 50 FF 35 ?? ?? "
    "?? ?? E8 ?? ?? ?? ??";
constexpr uint32_t kOffNetFlag = 2;
constexpr uint32_t kOffControlTable = 29;
constexpr uint32_t kOffCopyCall = 33;
const char* const kCopyPacketBody =
    "55 8B EC 8B 45 0C 8B 4D 08 0F 10 00 0F 11 01 8B 40 10 89 41 10 5D C3";
constexpr int kVtGetPlayerInput = 6;
constexpr int kVtGetBufA = 7;
constexpr int kVtGetBufB = 8;

constexpr uint32_t kPlayerInputSize = 0x718;
constexpr uint32_t kBufASize = 0x14;
constexpr uint32_t kBufBSize = 0x140;
constexpr uint32_t kPadSlot = 0x4BC;
constexpr uint32_t kKeyboardSlot = 0x4C0;
constexpr uint32_t kMouseSlot = 0x4C4;
constexpr uint32_t kActionsOffset = 0x0C; // 32 floats, rebuilt every update
constexpr int kActionCount = 32;
constexpr int kMaxP2Lines = 300;
constexpr int kMaxP1Lines = 120;

typedef void*(__fastcall* Ctor_t)(void* self, void* edx);
typedef void(__fastcall* SetByte_t)(void* self, void* edx, uint32_t value);
typedef void(__fastcall* SetPtr_t)(void* self, void* edx, void* value);
typedef void(__fastcall* Update_t)(void* self, void* edx, float dt);
typedef void*(__fastcall* Getter_t)(void* self, void* edx, uint32_t index);
typedef void(__cdecl* CopyPacket_t)(void* dst, const void* src);

Ctor_t g_piCtor = nullptr;
Ctor_t g_bufACtor = nullptr;
Ctor_t g_bufBCtor = nullptr;
SetByte_t g_setConnected = nullptr;
SetByte_t g_setFlag = nullptr;
SetPtr_t g_setBufA = nullptr;
SetPtr_t g_setBufB = nullptr;
Update_t g_origUpdate = nullptr;

uintptr_t g_player1Input = 0;
uint32_t g_frameCallRva = 0;

// 6c
uintptr_t g_manager = 0;      // the input manager instance (player 1's object - 0x1C)
uintptr_t g_managerVtable = 0;
Getter_t g_origGetPlayerInput = nullptr;
Getter_t g_origGetBufA = nullptr;
Getter_t g_origGetBufB = nullptr;
volatile LONG g_redirectOn = 0;
bool g_redirectInstalled = false;
volatile LONG g_asked[3] = {};   // requests for index 1, per getter, since the last summary
DWORD g_lastSummary = 0;

// 7c
uintptr_t g_netFlag = 0;        // byte: non-zero in a network game
uintptr_t g_controlTable = 0;   // pointers to each player's control state (0x14 bytes)
CopyPacket_t g_copyPacket = nullptr;
unsigned long g_copies = 0;
uint32_t g_lastP2Bits = 0;
bool g_p2Moving = false;
int g_ctlLines = 0;
constexpr int kMaxCtlLines = 150;

alignas(16) unsigned char g_p2Input[kPlayerInputSize];
alignas(16) unsigned char g_p2BufA[kBufASize];
alignas(16) unsigned char g_p2BufB[kBufBSize];

bool g_p2Ready = false;
bool g_lastP2Connected = false;
float g_lastP2Actions[kActionCount] = {};
float g_lastP1Actions[kActionCount] = {};
int g_p2Lines = 0;
int g_p1Lines = 0;
unsigned long g_frames = 0;

uint32_t ReadU32(uintptr_t va)
{
    uint32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

int32_t ReadI32At(uint32_t rva)
{
    int32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(Rva(rva)), sizeof(v));
    return v;
}

uint32_t FollowThunks(uint32_t t)
{
    for (int hop = 0; hop < 8 && *reinterpret_cast<const unsigned char*>(Rva(t)) == 0xE9; ++hop)
        t = static_cast<uint32_t>(static_cast<int64_t>(t) + 5 + ReadI32At(t + 1));
    return t;
}

uint32_t CallTarget(uint32_t rva)
{
    return FollowThunks(static_cast<uint32_t>(static_cast<int64_t>(rva) + 5 + ReadI32At(rva + 1)));
}

// Absolute code pointer stored at rva (a push imm32 operand) -> RVA of the function.
uint32_t CodePointerAt(uint32_t rva)
{
    const uintptr_t v = ReadU32(Rva(rva));
    if (v < ExeBase())
        return 0;
    return FollowThunks(static_cast<uint32_t>(v - ExeBase()));
}

bool FindOne(const char* what, const char* pattern, uint32_t* rva)
{
    uint32_t hits[4] = {};
    const int n = FindPattern(pattern, hits, 4);
    if (n != 1)
    {
        Log("[p2input] %s: %d pattern hits (need exactly 1) - player 2 input disabled", what, n);
        return false;
    }
    *rva = hits[0];
    return true;
}

bool Check(const char* what, uint32_t rva, const char* body)
{
    if (rva && MatchPatternAt(rva, body))
    {
        Log("[p2input] %-22s rva 0x%08X  OK", what, rva);
        return true;
    }
    Log("[p2input] %-22s rva 0x%08X  code does NOT match - player 2 input disabled", what, rva);
    return false;
}

bool FindAll()
{
    uint32_t mgr = 0;
    uint32_t wiring = 0;
    uint32_t site = 0;
    if (!FindOne("input manager constructor", kManagerCtor, &mgr) ||
        !FindOne("player 1 input wiring", kInitWiring, &wiring) ||
        !FindOne("per-frame input update", kFrameSite, &site))
        return false;

    const uintptr_t vtable = ReadU32(Rva(mgr + kOffManagerVtable));
    const uint32_t piCtor = CodePointerAt(mgr + kOffPlayerInputCtor);
    const uint32_t bufACtor = CodePointerAt(mgr + kOffBufACtor);
    const uint32_t bufBCtor = CodePointerAt(mgr + kOffBufBCtor);
    const uint32_t setConnected = CallTarget(wiring + 6);
    const uint32_t setFlag = CallTarget(wiring + 18);
    const uint32_t setBufA = CallTarget(wiring + 33);
    const uint32_t setBufB = CallTarget(wiring + 48);
    const uint32_t update = CallTarget(site + kOffFrameCall);

    if (!Check("PlayerInput constructor", piCtor, kPlayerInputCtorBody) ||
        !Check("buffer A constructor", bufACtor, kBufACtorBody) ||
        !Check("buffer B constructor", bufBCtor, kBufBCtorBody) ||
        !Check("SetConnected (+0x4B5)", setConnected, kSetConnectedBody) ||
        !Check("SetFlag (+0x4B4)", setFlag, kSetFlagBody) ||
        !Check("SetBufA (+0x4B8)", setBufA, kSetBufABody) ||
        !Check("SetBufB (+0x4B0)", setBufB, kSetBufBBody) ||
        !Check("PlayerInput::Update", update, kPlayerInputUpdateBody))
        return false;

    const uintptr_t p1 = ReadU32(Rva(wiring + 1));
    const BuildInfo* b = CurrentBuild();
    if (p1 < ExeBase() || p1 >= ExeBase() + b->sizeOfImage)
    {
        Log("[p2input] player 1's input object address is outside the exe - disabled");
        return false;
    }

    g_piCtor = reinterpret_cast<Ctor_t>(Rva(piCtor));
    g_bufACtor = reinterpret_cast<Ctor_t>(Rva(bufACtor));
    g_bufBCtor = reinterpret_cast<Ctor_t>(Rva(bufBCtor));
    g_setConnected = reinterpret_cast<SetByte_t>(Rva(setConnected));
    g_setFlag = reinterpret_cast<SetByte_t>(Rva(setFlag));
    g_setBufA = reinterpret_cast<SetPtr_t>(Rva(setBufA));
    g_setBufB = reinterpret_cast<SetPtr_t>(Rva(setBufB));
    g_origUpdate = reinterpret_cast<Update_t>(Rva(update));
    g_player1Input = p1;
    g_frameCallRva = site + kOffFrameCall;
    g_manager = p1 - 0x1C;
    g_managerVtable = (vtable >= ExeBase() && vtable < ExeBase() + b->sizeOfImage) ? vtable : 0;

    Log("[p2input] player 1's input object at rva 0x%08X; per-frame update call at rva 0x%08X",
        static_cast<unsigned>(p1 - ExeBase()), g_frameCallRva);
    return true;
}

void BuildPlayer2()
{
    g_piCtor(g_p2Input, nullptr);
    g_bufACtor(g_p2BufA, nullptr);
    g_bufBCtor(g_p2BufB, nullptr);

    g_setConnected(g_p2Input, nullptr, Pad2IsConnected() ? 1u : 0u);
    g_setFlag(g_p2Input, nullptr, 1u);
    g_setBufA(g_p2Input, nullptr, g_p2BufA);
    g_setBufB(g_p2Input, nullptr, g_p2BufB);

    const uint32_t pad = static_cast<uint32_t>(Pad2Address());
    const uint32_t none = 0;
    memcpy(g_p2Input + kPadSlot, &pad, sizeof(pad));
    memcpy(g_p2Input + kKeyboardSlot, &none, sizeof(none));
    memcpy(g_p2Input + kMouseSlot, &none, sizeof(none));

    uint32_t vtable = 0;
    memcpy(&vtable, g_p2Input, sizeof(vtable));
    Log("[p2input] player 2's input object built (vtable 0x%08X): pad 2 in the pad slot, no "
        "keyboard, no mouse, own buffers A and B",
        static_cast<unsigned>(vtable));
    g_p2Ready = true;
}

void ReadActions(const unsigned char* obj, float (&out)[kActionCount])
{
    memcpy(out, obj + kActionsOffset, sizeof(out));
}

// One line listing the non-zero actions, e.g. "0=1.00 3=1.00 30=-0.62".
void FormatActions(const float (&a)[kActionCount], char* out, size_t outSize)
{
    out[0] = 0;
    size_t used = 0;
    for (int i = 0; i < kActionCount; ++i)
    {
        if (a[i] > -0.05f && a[i] < 0.05f)
            continue;
        const int w = snprintf(out + used, outSize - used, "%s%d=%.2f", used ? " " : "", i,
                               static_cast<double>(a[i]));
        if (w < 0 || static_cast<size_t>(w) >= outSize - used)
            break;
        used += static_cast<size_t>(w);
    }
    if (!used)
        snprintf(out, outSize, "none");
}

bool ActionsChanged(const float (&a)[kActionCount], const float (&b)[kActionCount])
{
    for (int i = 0; i < kActionCount; ++i)
    {
        const float d = a[i] - b[i];
        if (d > 0.25f || d < -0.25f)
            return true;
        const bool az = a[i] > -0.05f && a[i] < 0.05f;
        const bool bz = b[i] > -0.05f && b[i] < 0.05f;
        if (az != bz)
            return true;
    }
    return false;
}

// 7c: player 2's control packet (buffer A 2, just filled from pad 2 by player
// 2's input update) -> control state 1, the struct player 2's PlayerController
// reads. Local games only (a network game fills every player's itself).
void Player2Controls()
{
    if (!g_copyPacket || !g_controlTable || !g_netFlag)
        return;
    if (*reinterpret_cast<const volatile uint8_t*>(g_netFlag))
        return;
    const BuildInfo* b = CurrentBuild();
    if (!b || !b->localPlayerCountRva || ReadU32(Rva(b->localPlayerCountRva)) < 2)
        return;
    const uintptr_t state1 = ReadU32(g_controlTable + 4);
    if (!state1)
        return;

    g_copyPacket(reinterpret_cast<void*>(state1), g_p2BufA);
    ++g_copies;
    if (g_copies == 1 && g_ctlLines < kMaxCtlLines)
    {
        ++g_ctlLines;
        Log("[p2ctl] %lu ms: first copy - player 2's control packet now goes into control state 1 "
            "(0x%08X) every frame (control state 0 is 0x%08X)",
            ScanMsSinceAttach(), static_cast<unsigned>(state1),
            static_cast<unsigned>(ReadU32(g_controlTable)));
    }

    float f[4];
    uint32_t bits = 0;
    memcpy(f, g_p2BufA, sizeof(f));
    memcpy(&bits, g_p2BufA + 0x10, sizeof(bits));
    bool moving = false;
    for (float v : f)
        if (v > 0.5f || v < -0.5f)
            moving = true;
    if ((bits != g_lastP2Bits || moving != g_p2Moving) && g_ctlLines < kMaxCtlLines)
    {
        ++g_ctlLines;
        Log("[p2ctl] %lu ms: player 2 control packet: %.2f %.2f %.2f %.2f, buttons 0x%08X",
            ScanMsSinceAttach(), static_cast<double>(f[0]), static_cast<double>(f[1]),
            static_cast<double>(f[2]), static_cast<double>(f[3]), static_cast<unsigned>(bits));
    }
    g_lastP2Bits = bits;
    g_p2Moving = moving;
}

void Player2Frame(float dt)
{
    ++g_frames;
    Pad2Tick(dt);
    AutoSpawnFrame(); // 7b: player 2's Spawn button, on the game thread
    ProneScanFrame(); // 10a: read-only watch of player 1's buttons

    const bool conn = Pad2IsConnected();
    if (conn != g_lastP2Connected)
    {
        g_setConnected(g_p2Input, nullptr, conn ? 1u : 0u);
        if (g_p2Lines < kMaxP2Lines)
        {
            ++g_p2Lines;
            Log("[p2input] %lu ms: player 2's input object: pad %s", ScanMsSinceAttach(),
                conn ? "connected" : "not connected");
        }
        g_lastP2Connected = conn;
    }

    g_origUpdate(g_p2Input, nullptr, dt);
    Player2Controls(); // 7c

    float now[kActionCount];
    ReadActions(g_p2Input, now);
    if (ActionsChanged(now, g_lastP2Actions) && g_p2Lines < kMaxP2Lines)
    {
        ++g_p2Lines;
        char text[512];
        FormatActions(now, text, sizeof(text));
        Log("[p2input] %lu ms: PLAYER 2 actions: %s", ScanMsSinceAttach(), text);
    }
    memcpy(g_lastP2Actions, now, sizeof(now));

    // Player 1, for comparison (read-only).
    float p1[kActionCount];
    ReadActions(reinterpret_cast<const unsigned char*>(g_player1Input), p1);
    if (ActionsChanged(p1, g_lastP1Actions) && g_p1Lines < kMaxP1Lines)
    {
        ++g_p1Lines;
        char text[512];
        FormatActions(p1, text, sizeof(text));
        Log("[p2input] %lu ms: player 1 actions: %s", ScanMsSinceAttach(), text);
    }
    memcpy(g_lastP1Actions, p1, sizeof(p1));

    if (g_frames == 1)
        Log("[p2input] %lu ms: first frame through the hook - player 1 updated by the game's code, "
            "then pad 2 and player 2",
            ScanMsSinceAttach());

    PlayerNInputFrame(dt); // 20e: pads 3 and 4, players 3 and 4 (does nothing unless 20e is on)

    // 6c: every 5 s, how often the game asked for player 2's input objects.
    const DWORD tick = GetTickCount();
    if (g_redirectInstalled && tick - g_lastSummary >= 5000)
    {
        g_lastSummary = tick;
        const LONG a = InterlockedExchange(&g_asked[0], 0);
        const LONG b = InterlockedExchange(&g_asked[1], 0);
        const LONG c = InterlockedExchange(&g_asked[2], 0);
        if ((a || b || c) && g_p2Lines < kMaxP2Lines)
        {
            ++g_p2Lines;
            Log("[p2redirect] %lu ms: last 5 s the game asked for player 2's GetPlayerInput x%ld, "
                "GetBufA x%ld, GetBufB x%ld (redirect %s)",
                ScanMsSinceAttach(), static_cast<long>(a), static_cast<long>(b),
                static_cast<long>(c), g_redirectOn ? "ON" : "OFF");
        }
    }
}

// ---------------------------------------------------------------------------
// 6c: redirected getters
// ---------------------------------------------------------------------------
constexpr int kMaxCallers = 48;
uint32_t g_callers[3][kMaxCallers] = {};
volatile LONG g_callerCount[3] = {};
const char* const kGetterNames[3] = { "GetPlayerInput", "GetBufA", "GetBufB" };

// Logs the first request from each place in the game's code for an index
// other than 0. Safe from any thread (the log is locked; the table is only
// appended to, and a rare duplicate line is harmless).
void NoteRequest(int getter, uint32_t index, const void* returnAddress)
{
    if (index == 1)
        InterlockedIncrement(&g_asked[getter]);
    const uintptr_t ra = reinterpret_cast<uintptr_t>(returnAddress);
    const uint32_t rva = (ra >= ExeBase()) ? static_cast<uint32_t>(ra - ExeBase()) : 0;
    const LONG n = g_callerCount[getter];
    for (LONG i = 0; i < n && i < kMaxCallers; ++i)
        if (g_callers[getter][i] == rva)
            return;
    const LONG slot = InterlockedIncrement(&g_callerCount[getter]) - 1;
    if (slot >= kMaxCallers)
        return;
    g_callers[getter][slot] = rva;
    Log("[p2redirect] %lu ms: %s(%lu) asked by game code at rva 0x%08X (first time from there) -> %s",
        ScanMsSinceAttach(), kGetterNames[getter], static_cast<unsigned long>(index),
        static_cast<unsigned>(rva),
        (index == 1 && g_redirectOn)                                                   ? "player 2's object"
        : ((index == 2 || index == 3) && g_redirectOn && PlayerNInputObject(static_cast<int>(index), getter))
            ? "player 3 / 4's object (20e)"
            : "the game's original (manager memory)");
}

void* __fastcall Hook_GetPlayerInput(void* self, void* edx, uint32_t index)
{
    if (index != 0)
    {
        NoteRequest(0, index, BF1_RETURN_ADDRESS());
        if ((index == 2 || index == 3) && g_redirectOn && reinterpret_cast<uintptr_t>(self) == g_manager)
        {
            void* const mine = PlayerNInputObject(static_cast<int>(index), 0); // 20e
            if (mine)
                return mine;
        }
        if (index == 1 && g_redirectOn && reinterpret_cast<uintptr_t>(self) == g_manager)
            return g_p2Input;
    }
    return g_origGetPlayerInput(self, edx, index);
}

void* __fastcall Hook_GetBufA(void* self, void* edx, uint32_t index)
{
    if (index != 0)
    {
        NoteRequest(1, index, BF1_RETURN_ADDRESS());
        if ((index == 2 || index == 3) && g_redirectOn && reinterpret_cast<uintptr_t>(self) == g_manager)
        {
            void* const mine = PlayerNInputObject(static_cast<int>(index), 1); // 20e
            if (mine)
                return mine;
        }
        if (index == 1 && g_redirectOn && reinterpret_cast<uintptr_t>(self) == g_manager)
            return g_p2BufA;
    }
    return g_origGetBufA(self, edx, index);
}

void* __fastcall Hook_GetBufB(void* self, void* edx, uint32_t index)
{
    if (index != 0)
    {
        NoteRequest(2, index, BF1_RETURN_ADDRESS());
        if ((index == 2 || index == 3) && g_redirectOn && reinterpret_cast<uintptr_t>(self) == g_manager)
        {
            void* const mine = PlayerNInputObject(static_cast<int>(index), 2); // 20e
            if (mine)
                return mine;
        }
        if (index == 1 && g_redirectOn && reinterpret_cast<uintptr_t>(self) == g_manager)
            return g_p2BufB;
    }
    return g_origGetBufB(self, edx, index);
}

uintptr_t VtableEntry(int slot)
{
    return g_managerVtable + static_cast<uintptr_t>(slot) * 4;
}

bool CheckVtableEntry(int slot, const char* name, const char* body, Getter_t* original)
{
    const uintptr_t p = ReadU32(VtableEntry(slot));
    if (p < ExeBase())
    {
        Log("[p2redirect] vtable +0x%02X (%s): points outside the exe - redirect NOT installed",
            slot * 4, name);
        return false;
    }
    const uint32_t target = FollowThunks(static_cast<uint32_t>(p - ExeBase()));
    if (!MatchPatternAt(target, body))
    {
        Log("[p2redirect] vtable +0x%02X (%s): rva 0x%08X does NOT match - redirect NOT installed",
            slot * 4, name, target);
        return false;
    }
    *original = reinterpret_cast<Getter_t>(Rva(target));
    Log("[p2redirect] vtable +0x%02X %-14s -> rva 0x%08X  OK", slot * 4, name, target);
    return true;
}

bool InstallRedirect()
{
    if (!g_managerVtable)
    {
        Log("[p2redirect] input manager vtable not found - redirect NOT installed");
        return false;
    }
    Log("[p2redirect] input manager at rva 0x%08X, vtable at rva 0x%08X",
        static_cast<unsigned>(g_manager - ExeBase()),
        static_cast<unsigned>(g_managerVtable - ExeBase()));
    if (!CheckVtableEntry(kVtGetPlayerInput, "GetPlayerInput", kGetPlayerInputBody,
                          &g_origGetPlayerInput) ||
        !CheckVtableEntry(kVtGetBufA, "GetBufA", kGetBufABody, &g_origGetBufA) ||
        !CheckVtableEntry(kVtGetBufB, "GetBufB", kGetBufBBody, &g_origGetBufB))
        return false;

    const uintptr_t first = VtableEntry(kVtGetPlayerInput);
    DWORD oldProt = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(first), 12, PAGE_READWRITE, &oldProt))
    {
        Log("[p2redirect] VirtualProtect failed (error %lu) - redirect NOT installed",
            GetLastError());
        return false;
    }
    InterlockedExchange(&g_redirectOn, 1);
    const uint32_t hooks[3] = {
        static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&Hook_GetPlayerInput)),
        static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&Hook_GetBufA)),
        static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&Hook_GetBufB)),
    };
    for (int i = 0; i < 3; ++i)
        InterlockedExchange(reinterpret_cast<volatile LONG*>(first + i * 4),
                            static_cast<LONG>(hooks[i]));
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(first), 12, oldProt, &ignored);

    g_redirectInstalled = true;
    Log("[p2redirect] redirect installed and ON: index 1 now gets player 2's input object and "
        "buffers. F11 turns it off/on.");
    return true;
}

// Replaces the manager's call to PlayerInput::Update for player 1. Same call
// shape as the original (thiscall: ECX = this, float on the stack, ret 4).
void __fastcall FrameHook(void* self, void* edx, float dt)
{
    g_origUpdate(self, edx, dt);
    if (g_p2Ready && reinterpret_cast<uintptr_t>(self) == g_player1Input)
    {
        const FramePerfScope perf(kPerfOurInput); // 21j-r11: the frame-rate scan
        Player2Frame(dt);
    }
}

bool InstallFrameHook()
{
    // The whole site was matched by FindOne; check again right before writing.
    if (!MatchPatternAt(g_frameCallRva - kOffFrameCall, kFrameSite) ||
        Rva(CallTarget(g_frameCallRva)) != reinterpret_cast<uintptr_t>(g_origUpdate))
    {
        Log("[p2input] frame hook: site changed since it was found - NOT installed");
        return false;
    }

    const uintptr_t at = Rva(g_frameCallRva);
    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(&FrameHook) - (at + 5));
    DWORD oldProt = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(at + 1), 4, PAGE_EXECUTE_READWRITE, &oldProt))
    {
        Log("[p2input] frame hook: VirtualProtect failed (error %lu) - NOT installed",
            GetLastError());
        return false;
    }
    memcpy(reinterpret_cast<void*>(at + 1), &rel, sizeof(rel));
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(at + 1), 4, oldProt, &ignored);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<LPCVOID>(at), 5);

    Log("[p2input] frame hook installed at rva 0x%08X: player 1's input update now goes through "
        "BF1GameExt (original first, then player 2)",
        g_frameCallRva);
    return true;
}

bool FindControlCopy()
{
    uint32_t hits[4] = {};
    const int n = FindPattern(kLocalControls, hits, 4);
    if (n != 1)
    {
        Log("[p2ctl] local controls copy: %d pattern hits (need exactly 1) - player 2 controls "
            "disabled",
            n);
        return false;
    }
    const uint32_t site = hits[0];
    const BuildInfo* b = CurrentBuild();
    const uintptr_t net = ReadU32(Rva(site + kOffNetFlag));
    const uintptr_t table = ReadU32(Rva(site + kOffControlTable));
    const uint32_t copy = CallTarget(site + kOffCopyCall);
    const uintptr_t lo = ExeBase();
    const uintptr_t hi = ExeBase() + b->sizeOfImage;
    if (net < lo || net >= hi || table < lo || table >= hi)
    {
        Log("[p2ctl] network flag or control state table outside the exe - player 2 controls "
            "disabled");
        return false;
    }
    if (!MatchPatternAt(copy, kCopyPacketBody))
    {
        Log("[p2ctl] CopyPacket at rva 0x%08X does NOT match - player 2 controls disabled", copy);
        return false;
    }
    g_netFlag = net;
    g_controlTable = table;
    g_copyPacket = reinterpret_cast<CopyPacket_t>(Rva(copy));
    Log("[p2ctl] local controls copy at rva 0x%08X OK: control state table at rva 0x%08X, network "
        "flag at rva 0x%08X, CopyPacket at rva 0x%08X OK",
        site, static_cast<unsigned>(table - lo), static_cast<unsigned>(net - lo), copy);
    return true;
}

} // namespace

void Player2InputStart(bool onGameThread)
{
    const BuildInfo* build = CurrentBuild();
    if (!build || build->loadsMemExt)
    {
        Log("[p2input] not used on this build");
        return;
    }
    if (!Pad2Init())
        return;
    if (!FindAll())
    {
        Pad2StartThread();
        return;
    }
    if (!onGameThread)
    {
        Log("[p2input] the scan did not run on the game thread, so the frame hook is NOT installed "
            "(not safe); pad 2 falls back to its own thread");
        Pad2StartThread();
        return;
    }

    BuildPlayer2();
    if (!InstallFrameHook())
    {
        g_p2Ready = false;
        Pad2StartThread();
        return;
    }
    InstallRedirect();
    FindControlCopy(); // 7c
}

void Player2RedirectToggle()
{
    if (!g_redirectInstalled)
    {
        Log("[p2redirect] F11: redirect is not installed on this run");
        return;
    }
    const LONG was = InterlockedExchange(&g_redirectOn, g_redirectOn ? 0 : 1);
    Log("[p2redirect] %lu ms: F11: redirect %s", ScanMsSinceAttach(), was ? "OFF" : "ON");
}

bool Player2RedirectActive()
{
    return g_redirectInstalled && g_redirectOn != 0;
}

// 20e
bool PlayerInputBuild(void* input, void* bufA, void* bufB, uintptr_t pad)
{
    if (!g_p2Ready || !g_piCtor || !input || !bufA || !bufB)
        return false;
    g_piCtor(input, nullptr);
    g_bufACtor(bufA, nullptr);
    g_bufBCtor(bufB, nullptr);
    g_setConnected(input, nullptr, 0u);
    g_setFlag(input, nullptr, 1u);
    g_setBufA(input, nullptr, bufA);
    g_setBufB(input, nullptr, bufB);
    const uint32_t p = static_cast<uint32_t>(pad);
    const uint32_t none = 0;
    unsigned char* const o = static_cast<unsigned char*>(input);
    memcpy(o + kPadSlot, &p, sizeof(p));
    memcpy(o + kKeyboardSlot, &none, sizeof(none));
    memcpy(o + kMouseSlot, &none, sizeof(none));
    return true;
}

void PlayerInputSetConnected(void* input, bool connected)
{
    if (g_setConnected && input)
        g_setConnected(input, nullptr, connected ? 1u : 0u);
}

void PlayerInputUpdate(void* input, float dt)
{
    if (g_origUpdate && input)
        g_origUpdate(input, nullptr, dt);
}

bool ControlStateFill(int index, const void* bufA)
{
    if (!g_copyPacket || !g_controlTable || !g_netFlag || index < 0 || index >= 50 || !bufA)
        return false;
    if (*reinterpret_cast<const volatile uint8_t*>(g_netFlag))
        return false;
    const uintptr_t state = ReadU32(g_controlTable + static_cast<uintptr_t>(index) * 4);
    if (!state)
        return false;
    g_copyPacket(reinterpret_cast<void*>(state), bufA);
    return true;
}

} // namespace bf1
