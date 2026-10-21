// BF1GameExt - ragdoll_body.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/ragdoll/; file names in the comments and log lines follow the new names. Nothing else changed.
// 22c: Prepare() - the physics world made now (else the first Create makes it), with smaller pools (512 + 512, not
// Bullet's 4096 + 4096). Otherwise p17e-r1's.
// Phase 15c: the ragdoll physics core (Bullet 3.25). No game addresses in here:
// it takes a soldier's bone matrices at death and gives bone matrices back
// every frame. ragdoll.cpp is the game side (hooks, which deaths, writing the
// matrices into the animation controller).
//
// Matrices are Direct3D 4x4 (16 floats, row vectors: rows 0-2 = the bone's
// x / y / z axes, row 3 = its position), in WORLD space (metres, y up).
//
// Each ragdoll: 11 capsules (pelvis, spine, head, upper / lower legs, upper /
// lower arms) built along the soldier's own bones, 10 joints (cone-twist at
// the waist, neck, hips and shoulders; hinges at the knees and elbows, with
// their bend limits measured from the pose at death), and a 12 x 12 m terrain
// patch sampled from the game's terrain height under the body (sampled again
// whenever the body slides 2.5 m from its centre). 15d: the parts collide with
// each other too (not with other ragdolls), and the joints hold the death
// pose with muscle tone that fades out over 0.4 s.
// 15e: rolling friction (round capsules no longer roll downhill like logs),
// every setting in Tuning (INI [Ragdoll], reloadable in game), slow motion,
// and a launch spin (whole-body tumble) for thrown soldiers.
// 15f: the muscles can follow an animation (SetAnimTarget: the game's death /
// thrown animation keeps playing through the ragdoll's joints, fading to
// Tuning.animKeep), and hits (Hit: an impulse at a point, e.g. a headshot).
// 15g: FileBuild() and GetParts() / PartName() (read-only: for the log and the
// ragdoll lab page, which runs this same file compiled to WebAssembly). The
// physics did not change.
// 15h r3: ReleaseWeapon (the gun falls as its own physics body from the hand),
// and resting damping (a body that is nearly still is calmed: no twitching).
// 15h (lab first): 6-axis joints with limits from BF1's own animations, capsules
// fitted to the stormtrooper, muscles that let go once the animation stops,
// the waist's turn spread over the spine bones. Same API.
// Every other bone (hands, feet, toes, clavicles, root...) rides rigidly on
// the capsule of its nearest ancestor that has one.
#pragma once

#include <cstddef>

namespace bf1 {
namespace ragdoll {

constexpr int kMaxBones = 32;    // the controller holds 32 bone matrices
constexpr int kMaxRagdolls = 16;

// The bones the ragdoll is built from (all must exist in the skeleton).
enum Role
{
    kRolePelvis,
    kRoleSpine, // bone_b_spine
    kRoleNeck,
    kRoleHead,
    kRoleThighL,
    kRoleCalfL,
    kRoleFootL,
    kRoleThighR,
    kRoleCalfR,
    kRoleFootR,
    kRoleUpperArmL,
    kRoleForearmL,
    kRoleHandL,
    kRoleUpperArmR,
    kRoleForearmR,
    kRoleHandR,
    kRoleCount
};

// "bone_pelvis", "bone_b_spine", ... for each role.
const char* RoleBoneName(int role);

struct Skeleton
{
    int count;                      // bones (1..kMaxBones)
    int parent[kMaxBones];          // -1 = none
    float restRot[kMaxBones][4];    // rest rotation quaternion (x y z w), for the bend directions
    int role[kRoleCount];           // bone index of each role
    int weaponBone;                 // 15h: hp_weapons (<= 0 = none): a gun that can be dropped (ReleaseWeapon)
};

// 15h: this physics core has Skeleton.weaponBone and ReleaseWeapon (the lab checks it)
#define BF1_RAGDOLL_WEAPON 1

// Terrain height under (x, z). false = no terrain there.
typedef bool (*GroundFn)(void* ctx, float x, float z, float* height);

// Filled by Create (for the log).
struct Built
{
    float bendKnee[2];   // current knee bend at death, radians (0 = straight)
    float bendElbow[2];
    float groundMin;     // the terrain patch's height range
    float groundMax;
    float lowestAbove;   // lowest capsule point above the terrain at the start
    float massKg;
    int overlapsAtStart; // 15d: part pairs overlapping in the death pose (collide once apart)
};

// Live state (for the log).
struct Status
{
    float pelvis[3];
    float speed;         // fastest body now, m/s
    float speedMax;      // fastest body so far
    float lowestAbove;   // lowest capsule point above the terrain now
    bool asleep;         // Bullet put every body to sleep (settled)
    bool frozen;         // stopped: see frozenWhy
    const char* frozenWhy;
    int recentres;       // times the terrain patch was sampled again under the body
    int overlapsWaiting; // 15d: part pairs still overlapping since the start (not colliding yet)
    float selfPenetration; // 15d: deepest overlap of two colliding parts so far (m)
    float muscle;        // 15d: muscle tone left, 1 .. 0
};

// 15e: every setting the ragdolls use (defaults = the tested 15d values plus
// rolling friction). New ragdolls take the values in force when they are
// built; timeScale applies at once.
struct Tuning
{
    float muscleStrength;  // motor impulse per substep per kg of the child part (0.06)
    float muscleTime;      // seconds for the muscle tone to fade out (0.4)
    float friction;        // body parts on terrain (0.9)
    float rollingFriction; // resists capsules rolling (0.15)
    float linearDamping;   // (0.05)
    float angularDamping;  // (0.85)
    float jointScale;      // multiplies the cone joints' swing / twist limits (1.0)
    float timeScale;       // physics speed: 1 = real time, 0.25 = slow motion
    float animKeep;        // 15f: share of the muscle pull that stays to the end when
                           // following an animation (0.1); 0 = fully limp after muscleTime
};

Tuning DefaultTuning();
void SetTuning(const Tuning& t);
const Tuning& CurrentTuning();

// Builds a ragdoll from the soldier's bone matrices (world, skeleton order,
// s.count of them). velocity = the soldier's velocity; spin (15e, may be null)
// = a whole-body angular velocity (rad/s): each part also gets spin x (its
// centre - the body's centre of mass), so the body tumbles as one. Returns a
// handle >= 0, or -1 with the reason in why.
int Create(const Skeleton& s, const float* boneWorld, const float velocity[3], GroundFn ground, void* ctx,
           Built* built, char* why, size_t whyLen, const float* spin = nullptr, float gravity = 9.81f);

// 15e: a push on a running ragdoll (an explosion hitting a body): every part's
// velocity changes by dv, plus a whole-body spin (may be null). Wakes it up.
void Push(int handle, const float dv[3], const float* spin);

// 15f: the muscles' targets from an animated pose (world bone matrices, as for
// Create): every joint is driven toward its angle in that pose (instead of
// its angle at death), with the fading muscle strength. Call each frame.
bool SetAnimTarget(int handle, const float* boneWorld);

// 15h: the hand lets go of the gun after delay seconds: from then on hp_weapons
// (Skeleton.weaponBone) follows a 4 kg box that falls and lands on the terrain
// patch, starting with the hand's motion. Until then (or never) it rides the
// forearm as before.
void ReleaseWeapon(int handle, float delay);

// 17e: a wall next to the body - a solid slab (1 m wide, 2 m tall, 16 cm thick) just behind a surface at
// point, facing normal (unit, out of the wall). At most kMaxWalls per ragdoll: one close to an existing
// slab facing the same way is skipped, the oldest is replaced when full. Returns true if a slab was added.
constexpr int kMaxWalls = 12;
bool AddWall(int handle, const float point[3], const float normal[3]);
int WallCount(int handle);

// 15f: a hit - impulse (N s) along dir (unit) at a world point, on the body
// part nearest to it. Returns that part's name ("head", "spine"...) or null.
const char* Hit(int handle, const float point[3], const float dir[3], float impulse);

// Advances every ragdoll by dt seconds (fixed 1/120 s substeps, at most 8).
void Step(float dt);

// The ragdoll's bone matrices now (world, skeleton order, count x 16 floats).
bool GetPose(int handle, float* boneWorld);

bool GetStatus(int handle, Status* out);

void Destroy(int handle);

int ActiveCount();

// 22c: makes the physics world now (the game side calls it at start, while memory is plentiful); the first Create
// makes it otherwise. True once it exists.
bool Prepare();

// 15g: this file's build ("p15g-r1"): the DLL logs it and the ragdoll lab
// shows it, so both can be checked to run the same physics.
const char* FileBuild();

// 15g: the body parts now (read-only, for drawing them in the ragdoll lab):
// per part 18 floats - its world matrix (Direct3D, 16 floats; the capsule
// runs along the matrix's y axis), radius, half length (centre to the start of
// a rounded end). out must hold maxParts x 18 floats. Returns the number of
// parts written (11), 0 for a bad handle. PartName(i): "pelvis", "spine"...
int GetParts(int handle, float* out, int maxParts);
const char* PartName(int part);

} // namespace ragdoll
} // namespace bf1
