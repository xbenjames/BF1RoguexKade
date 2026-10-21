// BF1GameExt - ragdoll_body.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/ragdoll/; file names in the comments and log lines follow the new names. Nothing else changed.
// 22c: THE PHYSICS WORLD'S POOLS SMALLER, AND IT CAN BE MADE AT START (Prepare). 22b's crash: the session's first ragdoll
//   made the world, and Bullet's default collision set-up asked for its two 4096-entry pools at once - the contact pool
//   alone 4096 x 880 bytes = 3.6 MB - in a game whose address space had no 3.6 MB left (EGM's maps): malloc gave 0 and
//   Bullet's default allocator cleared it anyway. Now 512 contacts and 512 collision algorithms (Bullet takes single blocks
//   when a pool runs out, as before - 12 ragdolls need far fewer), and the game side (ragdoll.cpp) makes the world at the
//   game's DirectInput call, while memory is plentiful. The physics itself is p17e-r1's, unchanged.
// Phase 15c: the ragdoll physics core (see ragdoll_body.hpp). Bullet 3.25,
// zlib license (bullet3\LICENSE.txt ships with the release).
// Phase 15d: the body parts collide with each other (self-collision), and the
//   joints hold the death pose with "muscle tone" that fades out over 0.4 s.
//   Self-collision: every pair of parts collides except the 10 jointed pairs
//   and 4 pairs that meet at a shared joint (thigh / spine at the hip, upper
//   arm / head at the shoulder). A pair already overlapping (or touching) in
//   the death pose - an arm across the chest holding a rifle - is left out
//   until it is 2 cm apart, then collides; so a pose can never push itself
//   apart. Muscle tone: each joint's motor pulls it back to its angle at
//   death, with a strength (child mass x 0.06 N s per substep) that fades as
//   (1 - t / 0.4 s)^2 - the soldier sags and buckles instead of dropping like
//   a sack in the first frame.
// Phase 15e: rolling friction 0.15 (15d log: bodies on 20-30 deg slopes rolled
//   like logs up to 18 m/s and 5 m; native test: they now come to rest on a
//   25 deg slope; spinning friction was tried too and dropped - it let a capsule
//   sink 25 cm into bumpy ground); every setting in a Tuning (SetTuning);
//   Step's time runs x timeScale (slow motion); Create takes a whole-body
//   spin for thrown soldiers.
// Phase 15f: muscles follow an animation (SetAnimTarget), with a floor of
//   animKeep; Hit() puts an impulse on the nearest part. FIX: the cone-twist
//   joints' muscle target is now set in constraint space - 15d / 15e passed
//   identity to setMotorTarget(), which Bullet treats as a BODY-space target
//   (it converts with the frames: Bframe^-1 x q x Aframe), so hips, shoulders,
//   waist and neck were pulled toward a slightly different pose than the one
//   at death (knees / elbows use the hinge motor and were right).
// Phase 15g: FileBuild(), GetParts() and PartName() - read-only, for the log
//   and the ragdoll lab page (this file compiled to WebAssembly, so the lab
//   runs exactly this physics). Nothing in the physics changed.
// Phase 15h (tried in the ragdoll lab first): "too many twists, the legs
//   jitter". Measured in the lab: the ragdoll's hips rolled up to ~145 deg,
//   the neck ~114, the waist ~65, while BF1's own death animations stay under
//   68 / 39 / 20 - Bullet's cone-twist joints do not hold their twist limit,
//   and the limits were centred on the pose at death, not on the body.
//   - waist, neck, hips, shoulders: btGeneric6DofSpring2Constraint (XYZ),
//     each axis limited on its own. Frame B = the child bone, frame A = the
//     child bone's REST orientation on the parent bone (skeleton rest data),
//     so 0 = the rest pose and the limits are anatomical. The limits are the
//     ranges of BF1's 24 human_rifle clips (deaths, thrown, landing, getting
//     up, running; mod tools .msh), measured the same way, left / right
//     merged mirrored, plus 8 deg. JointScale scales them about their middle.
//   - muscles: per-axis servo motors toward the animation's angles (the same
//     strength per kg); once the animation has stopped moving for 0.3 s the
//     AnimKeep floor fades out over 0.5 s (no fight with the ground after).
//   - capsule radii fitted to imp_inf_stormtrooper.msh (its median thickness
//     around each bone): pelvis 0.16, spine 0.18, head 0.14, upper arm 0.065,
//     forearm 0.055 (were 0.13 / 0.14 / 0.11 / 0.05 / 0.045). Masses unchanged.
//     r3: the legs back at 0.075 / 0.055 - fitted (0.095 / 0.07) the thighs and
//     calves rubbed while settling: over 10 lab deaths the legs turned 18 deg/s
//     on average while settling (peaks 640), back at the old size 8 (peak 170;
//     15g: 20, peak 610).
//   - drawn: bone_a_spine turns half way between the pelvis and the spine, so
//     a skinned body bends over two bones at the waist instead of one.
//   - r2, shoulders: r1 kept the full range of the clips (arm roll ~+-140 deg
//     from the T-pose) and the upper arms still wrung. Now measured from the
//     arm's middle position (the average over the clips), with the roll as
//     the joint's middle axis (YXZ): roll held to the clips' 5-95 % range
//     (about +-60 deg) + 10, swings to their 5-95 % range + 20.
//   - r2, rolls: the waist held to the clips' 5-95 % roll + 10 deg (-34..31,
//     was -54..41), the hips + 20 (-46..75, was -46..88: + 10 tripled the leg
//     twitches - the legs rested against the limit), the neck +-55.
//   - r3, the gun: ReleaseWeapon - the gun becomes a 4 kg box (0.06 x 0.11 x
//     0.8 m along hp_weapons' z) that falls from where the hand is, with the
//     hand's speed, and lands on the terrain patch (not on the body); hp_weapons
//     follows it. Before that it rides the forearm as before.
//   - r3, resting damping: once every part has been slower than 0.8 m/s for
//     0.25 s, the parts get linear damping 0.6 / angular 0.97 (Bullet's sleep
//     then takes them): the last twitches of a settling body (legs lying across
//     each other) die out. A hit, a push or faster than 2 m/s restores the
//     tuning's damping.
//   - 17e, walls: AddWall - static slabs (1 x 2 x 0.16 m) next to the body,
//     placed by the game side from the game's own ray test (walls, railings,
//     pillars, ceilings), in the ground's collision group. Otherwise the
//     physics is p15h-r3's, unchanged.
//   - knees, elbows: hinges as before.
//   - solver iterations 30 (were 20): about half the leg twitches while a body
//     settles, in the lab.
//
// Built from the soldier's pose at death (world bone matrices):
//   segment       from                         to                          radius  kg
//   pelvis        between the thighs           bone_b_spine                0.13    15
//   spine         bone_b_spine                 bone_neck                   0.14    20
//   head          neck..head at 25%            bone_head                   0.11     5
//   thigh  L / R  bone_?_thigh                 bone_?_calf                 0.075    9
//   calf   L / R  bone_?_calf                  bone_?_foot                 0.055    5
//   upper arm     bone_?_upperarm              bone_?_forearm              0.05     3
//   forearm       bone_?_forearm               bone_?_hand + 8 cm          0.045    2
// Capsule axis = the segment, centred on it. Joints at the shared bone:
//   waist (pelvis-spine), neck (spine-head): cone-twist
//   hips (pelvis-thigh), shoulders (spine-upper arm): cone-twist, centred on
//     the pose at death (twist axis = the limb)
//   knees, elbows: hinges about the upper bone's z axis (the skeleton bends
//     them about z: rest rotations are pure z rotations). The bend direction
//     is the sign of the lower bone's rest rotation (calf +15 deg, forearm
//     -29 deg on the human skeleton); limits: knee +2 .. 135 deg, elbow
//     +6 .. 140 deg, counted from the bend at death. (Native test, 48 runs:
//     knees stay inside; elbows overshoot to about -15 deg on the hardest
//     hits - more solver iterations did not help.)
//   Bullet's hinge angle reads -phi when the second body turns +phi about the
//   hinge axis (btHingeConstraint::getHingeAngle), so the limits are
//   (bend at death - max bend) .. (bend at death - min bend).
// Bodies collide with their own ragdoll's terrain patch only (collision
// groups), so a pose with an arm across the chest cannot push itself apart at
// the start, and bodies lying close together do not test each other's patch
// (that cut the cost of 8 ragdolls 3 m apart from 3.4 to 1.3 ms per frame in
// the native test; Bullet there runs the 15a box test in 0.27 ms, the game PC
// in 0.28 ms).
// Fixed 1/120 s substeps. A body faster than 40 m/s or a NaN freezes that
// ragdoll at its last good pose. When the pelvis gets 2.5 m from the terrain
// patch's centre, the patch is sampled again under it.

#include <cmath>
#include <cstdio>
#include <cstring>

#if defined(_MSC_VER)
#pragma warning(push, 0)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <BulletDynamics/ConstraintSolver/btGeneric6DofSpring2Constraint.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#include "ragdoll_body.hpp"

namespace bf1 {
namespace ragdoll {
namespace {

const char* const kRoleNames[kRoleCount] = {
    "bone_pelvis",     "bone_b_spine",   "bone_neck",   "bone_head",       "bone_l_thigh",   "bone_l_calf",
    "bone_l_foot",     "bone_r_thigh",   "bone_r_calf", "bone_r_foot",     "bone_l_upperarm", "bone_l_forearm",
    "bone_l_hand",     "bone_r_upperarm", "bone_r_forearm", "bone_r_hand",
};

enum Seg
{
    kSegPelvis,
    kSegSpine,
    kSegHead,
    kSegThighL,
    kSegCalfL,
    kSegThighR,
    kSegCalfR,
    kSegUpperArmL,
    kSegForearmL,
    kSegUpperArmR,
    kSegForearmR,
    kSegCount
};

struct SegDef
{
    float radius;
    float mass;
    int primaryRole; // this bone and the bones under it (until another segment) ride on the segment
};

const SegDef kSegs[kSegCount] = {
    { 0.16f, 15.0f, kRolePelvis },    { 0.18f, 20.0f, kRoleSpine },     { 0.14f, 5.0f, kRoleNeck },
    { 0.075f, 9.0f, kRoleThighL },    { 0.055f, 5.0f, kRoleCalfL },     { 0.075f, 9.0f, kRoleThighR },
    { 0.055f, 5.0f, kRoleCalfR },      { 0.065f, 3.0f, kRoleUpperArmL }, { 0.055f, 2.0f, kRoleForearmL },
    { 0.065f, 3.0f, kRoleUpperArmR }, { 0.055f, 2.0f, kRoleForearmR },
};

struct JointDef
{
    int parentSeg;
    int childSeg;
    int pivotRole;
    bool hinge;
    float swing1; // cone-twist
    float swing2;
    float twist;
    float bendMin;     // hinge: least bend (radians, 0 = straight)
    float bendMax;     // hinge: most bend
    int axisRole;      // hinge: bend axis = this bone's z axis
    int restRole;      // hinge: bend direction = the sign of this bone's rest z rotation
    float defaultSign; // hinge: if that rest rotation is (almost) zero
    int frameParent;   // 15h 6-axis: the parent bone (a role, or kFrameRibcage)
    int frameChild;    // 15h 6-axis: the child bone (a role)
    float lo[3];       // 15h 6-axis: angle limits x y z (degrees, 0 = rest pose or the centre below)
    float hi[3];
    int order;         // 15h 6-axis: Bullet's RotateOrder (the middle axis must stay inside +-90)
    float centre[4];   // 15h 6-axis: 0 = angles from the rest pose; else from this child-on-parent
                       //   orientation (quaternion of the row-form rotation)
};
constexpr int kFrameRibcage = -2; // the neck bone's parent (bone_ribcage)

constexpr int kJointCount = 10;
const JointDef kJoints[kJointCount] = {
    // 15h limits (degrees; x = the roll about the child bone): BF1's human_rifle clips. Swings: their
    // full range + 8. Rolls (r2): their 5-95 % range + 10 (waist) / + 20 (hips), the neck +-55 (a body's head
    // turns further than the clips turn it), shoulders: see r2 below
    { kSegPelvis, kSegSpine, kRoleSpine, false, 0.5f, 0.5f, 0.4f, 0.0f, 0.0f, 0, 0, 0.0f, kRolePelvis, kRoleSpine,
      { -34.0f, -32.0f, -74.0f }, { 31.0f, 28.0f, 37.0f }, RO_XYZ, { 0, 0, 0, 0 } }, // waist
    { kSegSpine, kSegHead, kRoleNeck, false, 0.7f, 0.7f, 0.7f, 0.0f, 0.0f, 0, 0, 0.0f, kFrameRibcage, kRoleNeck,
      { -55.0f, -28.0f, -41.0f }, { 55.0f, 40.0f, 47.0f }, RO_XYZ, { 0, 0, 0, 0 } }, // neck
    { kSegPelvis, kSegThighL, kRoleThighL, false, 1.1f, 1.1f, 0.4f, 0.0f, 0.0f, 0, 0, 0.0f, kRolePelvis, kRoleThighL,
      { -46.0f, -49.0f, -46.0f }, { 75.0f, 66.0f, 143.0f }, RO_XYZ, { 0, 0, 0, 0 } }, // hip L
    { kSegThighL, kSegCalfL, kRoleCalfL, true, 0.0f, 0.0f, 0.0f, 0.03f, 2.35f, kRoleThighL, kRoleCalfL, 1.0f, 0, 0,
      { 0, 0, 0 }, { 0, 0, 0 }, RO_XYZ, { 0, 0, 0, 0 } },
    { kSegPelvis, kSegThighR, kRoleThighR, false, 1.1f, 1.1f, 0.4f, 0.0f, 0.0f, 0, 0, 0.0f, kRolePelvis, kRoleThighR,
      { -75.0f, -66.0f, -46.0f }, { 46.0f, 49.0f, 143.0f }, RO_XYZ, { 0, 0, 0, 0 } }, // hip R (mirrored)
    { kSegThighR, kSegCalfR, kRoleCalfR, true, 0.0f, 0.0f, 0.0f, 0.03f, 2.35f, kRoleThighR, kRoleCalfR, 1.0f, 0, 0,
      { 0, 0, 0 }, { 0, 0, 0 }, RO_XYZ, { 0, 0, 0, 0 } },
    { kSegSpine, kSegUpperArmL, kRoleUpperArmL, false, 1.4f, 1.4f, 0.8f, 0.0f, 0.0f, 0, 0, 0.0f, kFrameRibcage,
      kRoleUpperArmL, { -75.0f, -80.0f, -93.0f }, { 60.0f, 96.0f, 96.0f }, RO_YXZ,
      { 0.462552f, 0.024040f, 0.883029f, -0.075672f } }, // shoulder L (r2: from the arm's middle, twist = x)
    { kSegUpperArmL, kSegForearmL, kRoleForearmL, true, 0.0f, 0.0f, 0.0f, 0.10f, 2.45f, kRoleUpperArmL,
      kRoleForearmL, -1.0f, 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, RO_XYZ, { 0, 0, 0, 0 } },
    { kSegSpine, kSegUpperArmR, kRoleUpperArmR, false, 1.4f, 1.4f, 0.8f, 0.0f, 0.0f, 0, 0, 0.0f, kFrameRibcage,
      kRoleUpperArmR, { -64.0f, -116.0f, -100.0f }, { 61.0f, 75.0f, 102.0f }, RO_YXZ,
      { -0.550903f, 0.037526f, 0.796702f, -0.245689f } }, // shoulder R (r2)
    { kSegUpperArmR, kSegForearmR, kRoleForearmR, true, 0.0f, 0.0f, 0.0f, 0.10f, 2.45f, kRoleUpperArmR,
      kRoleForearmR, -1.0f, 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, RO_XYZ, { 0, 0, 0, 0 } },
};
constexpr float kServoSpeed = 8.0f;   // 15h: muscles turn a joint toward the animation at most this fast (rad/s)
constexpr float kAnimHold = 0.3f;     // 15h: the animation still this long ...
constexpr float kAnimFade = 0.5f;     // 15h: ... then the AnimKeep floor fades out over this
constexpr float kAnimMoved = 0.0087f; // 15h: a target change above 0.5 deg counts as moving
constexpr float kCalmSpeed = 0.8f;    // 15h r3: every part slower than this ...
constexpr float kCalmTime = 0.25f;    // ... this long: resting damping
constexpr float kWakeSpeed = 2.0f;    // 15h r3: faster than this: the tuning's damping again
constexpr float kRestLinDamp = 0.6f;
constexpr float kRestAngDamp = 0.97f;
const btVector3 kGunHalf(0.03f, 0.055f, 0.4f); // 15h r3: the dropped gun's box (half sizes)
const btVector3 kGunCentre(0.0f, -0.02f, 0.17f); // its centre in hp_weapons' axes
constexpr float kGunMass = 4.0f;

constexpr int kPatchN = 25;          // terrain samples per side
constexpr float kPatchStep = 0.5f;   // metres between samples (12 m patch)
constexpr float kPatchMove = 2.5f;   // pelvis this far from the patch centre: re-centre the patch
constexpr float kMaxSpeed = 40.0f;   // m/s: faster = unstable, freeze
constexpr float kSubStep = 1.0f / 120.0f;
constexpr float kSeparate = 0.02f;   // an overlapping pair collides again once this far apart
constexpr float kTouching = 0.005f;  // a pair this close at the start counts as overlapping
Tuning g_tuning = { 0.06f, 0.4f, 0.9f, 0.15f, 0.05f, 0.85f, 1.0f, 1.0f, 0.1f };
constexpr int kMaxSubSteps = 8;
// Collision groups: ragdoll i's bodies (bit i) meet ragdoll i's terrain patch
// (bit 16 + i) and each other (15d) - never another ragdoll's patch.
int BodyGroup(int slot)
{
    return static_cast<int>(1u << slot);
}

int GroundGroup(int slot)
{
    return static_cast<int>(1u << (16 + slot));
}

struct Ragdoll
{
    bool used;
    int slot;
    bool frozen;
    const char* frozenWhy;
    int count;
    int seg[kMaxBones];
    btTransform offset[kMaxBones]; // bone = segment x offset (Bullet order)
    btRigidBody* body[kSegCount];
    btCollisionShape* shape[kSegCount];
    btDefaultMotionState* motion[kSegCount];
    btTransform lastGood[kSegCount];
    float radius[kSegCount];
    float halfLen[kSegCount];
    btTypedConstraint* joint[kJointCount];
    btRigidBody* wall[kMaxWalls]; // 17e: wall slabs
    btDefaultMotionState* wallMotion[kMaxWalls];
    btVector3 wallPoint[kMaxWalls];
    btVector3 wallNormal[kMaxWalls];
    int wallNext;
    int walls;
    btRigidBody* ground;
    btHeightfieldTerrainShape* groundShape;
    btDefaultMotionState* groundMotion;
    float* heights;
    float cx;
    float cz;
    GroundFn fn;
    void* ctx;
    float speedMax;
    int recentres;
    bool joined[kSegCount][kSegCount];  // jointed or sharing a joint: never collide
    bool ignored[kSegCount][kSegCount]; // overlapping at the start: not yet colliding
    int ignoredCount;
    float age;
    float muscle;      // 1 .. 0
    float muscleTime;  // the tuning's value when built
    int role[kRoleCount];                 // 15f: the skeleton's role bones (for SetAnimTarget)
    float servo[kJointCount][3];          // 15h: 6-axis joint targets (angles x y z)
    float animMovedAge;                   // 15h: age when the animation last moved
    int aSpine;                           // 15h: bone_a_spine (-1: none) - drawn half way to the spine
    btTransform aSpineAlt;                // 15h: bone_a_spine on the spine segment
    float hingeTarget[kJointCount];       // 15f: hinge targets (angle)
    bool animTargets;                     // 15f: an animation is being followed
    float penetration; // deepest self-overlap of colliding parts seen so far
    float calm;        // 15h r3: seconds every part has been slow
    bool resting;      // 15h r3: resting damping on
    int gunBone;       // 15h r3: hp_weapons (-1 none)
    int gunState;      // 0 in the hand, 1 letting go (gunTimer), 2 falling / lying
    float gunTimer;
    btRigidBody* gun;
    btBoxShape* gunShape;
    btDefaultMotionState* gunMotion;
    btTransform gunOffset; // hp_weapons = gun body x gunOffset
};

Ragdoll g_rag[kMaxRagdolls];
btDefaultCollisionConfiguration* g_config = nullptr;
btCollisionDispatcher* g_dispatcher = nullptr;
btBroadphaseInterface* g_broadphase = nullptr;
btSequentialImpulseConstraintSolver* g_solver = nullptr;
btDiscreteDynamicsWorld* g_world = nullptr;

// 22c: Bullet's defaults are 4096 + 4096 (4.1 MB, the contact pool alone 3.6 MB in one block - 22c native test: 505 KB
// now, the biggest block 450 KB); a full pool falls back to single blocks (tested: 46786 contacts at once, no crash)
constexpr int kPoolContacts = 512;
constexpr int kPoolAlgorithms = 512;

bool EnsureWorld()
{
    if (g_world)
        return true;
    btDefaultCollisionConstructionInfo ci;
    ci.m_defaultMaxPersistentManifoldPoolSize = kPoolContacts;
    ci.m_defaultMaxCollisionAlgorithmPoolSize = kPoolAlgorithms;
    g_config = new btDefaultCollisionConfiguration(ci);
    g_dispatcher = new btCollisionDispatcher(g_config);
    g_broadphase = new btDbvtBroadphase();
    g_solver = new btSequentialImpulseConstraintSolver();
    g_world = new btDiscreteDynamicsWorld(g_dispatcher, g_broadphase, g_solver, g_config);
    g_world->setGravity(btVector3(0, btScalar(-9.81), 0));
    g_world->getSolverInfo().m_numIterations = 30; // 15h: was 20 (lab: half the settling twitches)
    return true;
}

// Direct3D row-vector matrix -> Bullet transform (basis columns = D3D rows).
btTransform FromD3D(const float* m)
{
    const btMatrix3x3 b(m[0], m[4], m[8], m[1], m[5], m[9], m[2], m[6], m[10]);
    return btTransform(b, btVector3(m[12], m[13], m[14]));
}

void ToD3D(const btTransform& t, float* m)
{
    const btMatrix3x3& b = t.getBasis();
    for (int r = 0; r < 3; ++r)
    {
        m[r * 4 + 0] = b[0][r];
        m[r * 4 + 1] = b[1][r];
        m[r * 4 + 2] = b[2][r];
        m[r * 4 + 3] = 0.0f;
    }
    m[12] = t.getOrigin().x();
    m[13] = t.getOrigin().y();
    m[14] = t.getOrigin().z();
    m[15] = 1.0f;
}

btVector3 BonePos(const float* boneWorld, int bone)
{
    const float* m = boneWorld + bone * 16;
    return btVector3(m[12], m[13], m[14]);
}

btVector3 BoneAxis(const float* boneWorld, int bone, int row)
{
    const float* m = boneWorld + bone * 16 + row * 4;
    return btVector3(m[0], m[1], m[2]);
}

bool Finite(const btVector3& v)
{
    return std::isfinite(v.x()) && std::isfinite(v.y()) && std::isfinite(v.z());
}

// The segment's two ends from the bone positions.
void SegEnds(int seg, const btVector3* p, btVector3* a, btVector3* b)
{
    switch (seg)
    {
    case kSegPelvis:
        *a = (p[kRoleThighL] + p[kRoleThighR]) * btScalar(0.5);
        *b = p[kRoleSpine];
        break;
    case kSegSpine:
        *a = p[kRoleSpine];
        *b = p[kRoleNeck];
        break;
    case kSegHead:
        *a = p[kRoleNeck].lerp(p[kRoleHead], btScalar(0.25));
        *b = p[kRoleHead];
        break;
    case kSegThighL:
        *a = p[kRoleThighL];
        *b = p[kRoleCalfL];
        break;
    case kSegCalfL:
        *a = p[kRoleCalfL];
        *b = p[kRoleFootL];
        break;
    case kSegThighR:
        *a = p[kRoleThighR];
        *b = p[kRoleCalfR];
        break;
    case kSegCalfR:
        *a = p[kRoleCalfR];
        *b = p[kRoleFootR];
        break;
    case kSegUpperArmL:
        *a = p[kRoleUpperArmL];
        *b = p[kRoleForearmL];
        break;
    case kSegForearmL:
    {
        *a = p[kRoleForearmL];
        btVector3 d = p[kRoleHandL] - p[kRoleForearmL];
        *b = p[kRoleHandL] + (d.length() > btScalar(0.01) ? d.normalized() * btScalar(0.08) : btVector3(0, 0, 0));
        break;
    }
    case kSegUpperArmR:
        *a = p[kRoleUpperArmR];
        *b = p[kRoleForearmR];
        break;
    default: // kSegForearmR
    {
        *a = p[kRoleForearmR];
        btVector3 d = p[kRoleHandR] - p[kRoleForearmR];
        *b = p[kRoleHandR] + (d.length() > btScalar(0.01) ? d.normalized() * btScalar(0.08) : btVector3(0, 0, 0));
        break;
    }
    }
}

// Orthonormal basis with y = dir; x from 'ref' where possible.
btMatrix3x3 BasisY(const btVector3& dir, const btVector3& ref)
{
    const btVector3 y = dir.normalized();
    btVector3 x = y.cross(ref);
    if (x.length2() < btScalar(0.01))
    {
        btVector3 p;
        btVector3 q;
        btPlaneSpace1(y, p, q);
        x = p;
    }
    x.normalize();
    const btVector3 z = x.cross(y);
    return btMatrix3x3(x.x(), y.x(), z.x(), x.y(), y.y(), z.y(), x.z(), y.z(), z.z());
}

// Columns x, y, z -> basis.
btMatrix3x3 BasisCols(const btVector3& x, const btVector3& y, const btVector3& z)
{
    return btMatrix3x3(x.x(), y.x(), z.x(), x.y(), y.y(), z.y(), x.z(), y.z(), z.z());
}

btTransform SegWorld(const Ragdoll& r, int seg)
{
    if (r.frozen)
        return r.lastGood[seg];
    btTransform t;
    r.motion[seg]->getWorldTransform(t);
    return t;
}

float Lowest(const Ragdoll& r)
{
    float lowest = 1e9f;
    for (int s = 0; s < kSegCount; ++s)
    {
        const btTransform t = SegWorld(r, s);
        const btVector3 c = t.getOrigin();
        const btVector3 axis = t.getBasis().getColumn(1);
        const float bottom = c.y() - r.halfLen[s] * std::fabs(axis.y()) - r.radius[s];
        float h = 0.0f;
        if (r.fn && r.fn(r.ctx, c.x(), c.z(), &h) && bottom - h < lowest)
            lowest = bottom - h;
    }
    return lowest;
}

void Freeze(Ragdoll& r, const char* why)
{
    r.frozen = true;
    r.frozenWhy = why;
    for (int s = 0; s < kSegCount; ++s)
    {
        if (!r.body[s])
            continue;
        r.body[s]->setLinearVelocity(btVector3(0, 0, 0));
        r.body[s]->setAngularVelocity(btVector3(0, 0, 0));
        r.body[s]->setWorldTransform(r.lastGood[s]);
        r.body[s]->forceActivationState(DISABLE_SIMULATION);
    }
}


bool Fail(char* why, size_t whyLen, const char* text)
{
    if (why && whyLen)
        snprintf(why, whyLen, "%s", text);
    return false;
}

int FailHandle(char* why, size_t whyLen, const char* text)
{
    Fail(why, whyLen, text);
    return -1;
}

void FreeGround(Ragdoll& r)
{
    if (r.ground)
    {
        g_world->removeRigidBody(r.ground);
        delete r.ground;
    }
    delete r.groundMotion;
    delete r.groundShape;
    delete[] r.heights;
    r.ground = nullptr;
    r.groundMotion = nullptr;
    r.groundShape = nullptr;
    r.heights = nullptr;
}

// (Re)builds the terrain patch centred under 'centre'.
bool BuildGround(Ragdoll& r, const btVector3& centre, GroundFn ground, void* ctx, Built* built, char* why,
                 size_t whyLen)
{
    FreeGround(r);
    r.heights = new float[kPatchN * kPatchN];
    r.cx = centre.x();
    r.cz = centre.z();
    const int half = kPatchN / 2;
    float lo = 1e30f;
    float hi = -1e30f;
    for (int j = 0; j < kPatchN; ++j)
    {
        for (int i = 0; i < kPatchN; ++i)
        {
            const float x = r.cx + static_cast<float>(i - half) * kPatchStep;
            const float z = r.cz + static_cast<float>(j - half) * kPatchStep;
            float h = 0.0f;
            if (!ground(ctx, x, z, &h) || !std::isfinite(h))
                return Fail(why, whyLen, "no terrain height under part of the body");
            r.heights[j * kPatchN + i] = h;
            if (h < lo)
                lo = h;
            if (h > hi)
                hi = h;
        }
    }
    if (hi - lo < 0.02f)
    {
        lo -= 0.01f;
        hi += 0.01f;
    }
    if (built)
    {
        built->groundMin = lo;
        built->groundMax = hi;
    }
    r.groundShape = new btHeightfieldTerrainShape(kPatchN, kPatchN, r.heights, lo, hi, 1, false);
    r.groundShape->setLocalScaling(btVector3(kPatchStep, 1, kPatchStep));
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(r.cx, (lo + hi) * 0.5f, r.cz));
    r.groundMotion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo ci(0, r.groundMotion, r.groundShape, btVector3(0, 0, 0));
    ci.m_friction = btScalar(0.9);
    ci.m_restitution = 0;
    r.ground = new btRigidBody(ci);
    g_world->addRigidBody(r.ground, GroundGroup(r.slot), BodyGroup(r.slot));
    return true;
}

void Free(Ragdoll& r)
{
    for (int j = 0; j < kJointCount; ++j)
    {
        if (r.joint[j])
        {
            g_world->removeConstraint(r.joint[j]);
            delete r.joint[j];
        }
    }
    for (int s = 0; s < kSegCount; ++s)
    {
        if (r.body[s])
        {
            g_world->removeRigidBody(r.body[s]);
            delete r.body[s];
        }
        delete r.motion[s];
        delete r.shape[s];
    }
    if (r.gun)
    {
        g_world->removeRigidBody(r.gun);
        delete r.gun;
    }
    delete r.gunMotion;
    delete r.gunShape;
    for (int w = 0; w < kMaxWalls; ++w)
    {
        if (r.wall[w])
        {
            g_world->removeRigidBody(r.wall[w]);
            delete r.wall[w];
        }
        delete r.wallMotion[w];
    }
    FreeGround(r);
    r = Ragdoll();
}

// 15h r3: damping for every part (and the gun): the tuning's, or resting
void SetDampingAll(Ragdoll& r, bool resting)
{
    r.resting = resting;
    const btScalar lin = resting ? btScalar(kRestLinDamp) : btScalar(g_tuning.linearDamping);
    const btScalar ang = resting ? btScalar(kRestAngDamp) : btScalar(g_tuning.angularDamping);
    for (int g = 0; g < kSegCount; ++g)
        if (r.body[g])
            r.body[g]->setDamping(lin, ang);
    if (r.gun)
        r.gun->setDamping(lin, ang);
}

// 15h r3: the hand lets go - the gun becomes a box at hp_weapons, with the hand's motion
void MakeGun(Ragdoll& r)
{
    r.gunState = 2;
    const int seg = r.seg[r.gunBone];
    const btTransform bone = SegWorld(r, seg) * r.offset[r.gunBone];
    const btTransform body(bone.getBasis(), bone(kGunCentre));
    r.gunOffset = body.inverse() * bone;
    r.gunShape = new btBoxShape(kGunHalf);
    btVector3 inertia(0, 0, 0);
    r.gunShape->calculateLocalInertia(kGunMass, inertia);
    r.gunMotion = new btDefaultMotionState(body);
    btRigidBody::btRigidBodyConstructionInfo ci(kGunMass, r.gunMotion, r.gunShape, inertia);
    ci.m_linearDamping = g_tuning.linearDamping;
    ci.m_angularDamping = g_tuning.angularDamping;
    ci.m_friction = g_tuning.friction;
    ci.m_rollingFriction = g_tuning.rollingFriction;
    ci.m_restitution = 0;
    r.gun = new btRigidBody(ci);
    r.gun->setDeactivationTime(btScalar(0.8));
    r.gun->setSleepingThresholds(btScalar(1.6), btScalar(2.5));
    r.gun->setCcdMotionThreshold(btScalar(0.03));
    r.gun->setCcdSweptSphereRadius(btScalar(0.03));
    btRigidBody* hand = r.body[seg];
    r.gun->setLinearVelocity(hand->getVelocityInLocalPoint(body.getOrigin() - hand->getCenterOfMassPosition()));
    r.gun->setAngularVelocity(hand->getAngularVelocity());
    g_world->addRigidBody(r.gun, BodyGroup(r.slot), GroundGroup(r.slot)); // the terrain patch only
}

// Closest distance between segments p1-q1 and p2-q2 (Ericson, RTCD 5.1.9).
float SegSegDist(const btVector3& p1, const btVector3& q1, const btVector3& p2, const btVector3& q2)
{
    const btVector3 d1 = q1 - p1;
    const btVector3 d2 = q2 - p2;
    const btVector3 r = p1 - p2;
    const float a = d1.dot(d1);
    const float e = d2.dot(d2);
    const float f = d2.dot(r);
    float s = 0.0f;
    float t = 0.0f;
    if (a <= 1e-8f && e <= 1e-8f)
        return (p1 - p2).length();
    if (a <= 1e-8f)
    {
        t = btClamped(f / e, 0.0f, 1.0f);
    }
    else
    {
        const float c = d1.dot(r);
        if (e <= 1e-8f)
        {
            s = btClamped(-c / a, 0.0f, 1.0f);
        }
        else
        {
            const float b = d1.dot(d2);
            const float denom = a * e - b * b;
            s = denom != 0.0f ? btClamped((b * f - c * e) / denom, 0.0f, 1.0f) : 0.0f;
            t = (b * s + f) / e;
            if (t < 0.0f)
            {
                t = 0.0f;
                s = btClamped(-c / a, 0.0f, 1.0f);
            }
            else if (t > 1.0f)
            {
                t = 1.0f;
                s = btClamped((b - c) / a, 0.0f, 1.0f);
            }
        }
    }
    return ((p1 + d1 * s) - (p2 + d2 * t)).length();
}

// Gap between two of the ragdoll's capsules (negative = overlapping).
float CapsuleGap(const Ragdoll& r, int a, int b)
{
    const btTransform ta = r.body[a]->getWorldTransform();
    const btTransform tb = r.body[b]->getWorldTransform();
    const btVector3 ya = ta.getBasis().getColumn(1) * r.halfLen[a];
    const btVector3 yb = tb.getBasis().getColumn(1) * r.halfLen[b];
    const float d = SegSegDist(ta.getOrigin() - ya, ta.getOrigin() + ya, tb.getOrigin() - yb, tb.getOrigin() + yb);
    return d - r.radius[a] - r.radius[b];
}

void SetIgnore(Ragdoll& r, int a, int b, bool ignore)
{
    r.body[a]->setIgnoreCollisionCheck(r.body[b], ignore);
    r.body[b]->setIgnoreCollisionCheck(r.body[a], ignore);
    r.ignored[a][b] = ignore;
    r.ignored[b][a] = ignore;
    r.ignoredCount += ignore ? 1 : -1;
}

// Muscle tone: motors pull every joint toward its target - its angle at death,
// or (15f) its angle in the animation being followed.
void SetMuscle(Ragdoll& r, float strength, float dt)
{
    r.muscle = strength;
    for (int j = 0; j < kJointCount; ++j)
    {
        const float imp = g_tuning.muscleStrength * kSegs[kJoints[j].childSeg].mass * strength;
        if (kJoints[j].hinge)
        {
            btHingeConstraint* h = static_cast<btHingeConstraint*>(r.joint[j]);
            if (strength > 0.0f && dt > 0.0f)
            {
                h->enableMotor(true);
                h->setMaxMotorImpulse(imp);
                h->setMotorTarget(r.hingeTarget[j], dt);
            }
            else
            {
                h->enableMotor(false);
            }
        }
        else
        {
            // 15h: a servo per axis toward the animation's angles; the same impulse per substep
            btGeneric6DofSpring2Constraint* c = static_cast<btGeneric6DofSpring2Constraint*>(r.joint[j]);
            for (int a = 0; a < 3; ++a)
            {
                const int idx = 3 + a;
                if (strength > 0.0f)
                {
                    c->enableMotor(idx, true);
                    c->setServo(idx, true);
                    c->setServoTarget(idx, r.servo[j][a]);
                    c->setTargetVelocity(idx, kServoSpeed);
                    c->setMaxMotorForce(idx, imp / kSubStep);
                }
                else
                {
                    c->enableMotor(idx, false);
                }
            }
        }
    }
}

// 15h: a bone's rotation (rows = its axes, made unit length) from a Direct3D matrix
btMatrix3x3 BoneRows(const float* boneWorld, int bone)
{
    btVector3 r[3];
    for (int k = 0; k < 3; ++k)
    {
        r[k] = BoneAxis(boneWorld, bone, k);
        const btScalar len = r[k].length();
        r[k] = len > btScalar(1e-6) ? r[k] / len : btVector3(k == 0, k == 1, k == 2);
    }
    return btMatrix3x3(r[0].x(), r[0].y(), r[0].z(), r[1].x(), r[1].y(), r[1].z(), r[2].x(), r[2].y(), r[2].z());
}

int FrameBone(const Skeleton& s, int sel)
{
    if (sel == kFrameRibcage)
    {
        const int neck = s.role[kRoleNeck];
        const int rib = neck >= 0 ? s.parent[neck] : -1;
        return rib >= 0 ? rib : s.role[kRoleSpine];
    }
    return s.role[sel];
}

btTypedConstraint* MakeJoint(Ragdoll& r, const JointDef& d, const Skeleton& s, const float* boneWorld,
                             const btVector3* p, const btVector3* segDir, float* bendOut, const btMatrix3x3* restW)
{
    btRigidBody* a = r.body[d.parentSeg];
    btRigidBody* b = r.body[d.childSeg];
    btTransform f;
    f.setIdentity();
    f.setOrigin(p[d.pivotRole]);
    if (!d.hinge)
    {
        // 15h: frame B = the child bone now; frame A = where the child bone would
        // be at its rest orientation on the parent bone now -> angle 0 = rest
        const int pb = FrameBone(s, d.frameParent);
        const int cb = FrameBone(s, d.frameChild);
        const bool centred = d.centre[0] != 0.0f || d.centre[1] != 0.0f || d.centre[2] != 0.0f || d.centre[3] != 0.0f;
        const btMatrix3x3 restRel = centred ? btMatrix3x3(btQuaternion(d.centre[0], d.centre[1], d.centre[2], d.centre[3]))
                                            : restW[cb] * restW[pb].transpose(); // rows: child = rel x parent
        const btMatrix3x3 fA = (restRel * BoneRows(boneWorld, pb)).transpose(); // Bullet: columns = axes
        const btMatrix3x3 fB = BoneRows(boneWorld, cb).transpose();
        const btTransform fa = a->getCenterOfMassTransform().inverse() * btTransform(fA, p[d.pivotRole]);
        const btTransform fb = b->getCenterOfMassTransform().inverse() * btTransform(fB, p[d.pivotRole]);
        btGeneric6DofSpring2Constraint* c =
            new btGeneric6DofSpring2Constraint(*a, *b, fa, fb, static_cast<RotateOrder>(d.order));
        c->setLinearLowerLimit(btVector3(0, 0, 0));
        c->setLinearUpperLimit(btVector3(0, 0, 0));
        const float k = g_tuning.jointScale;
        const float d2r = 0.01745329f;
        btVector3 lo;
        btVector3 hi;
        for (int i = 0; i < 3; ++i)
        {
            const float mid = (d.lo[i] + d.hi[i]) * 0.5f;
            const float half = (d.hi[i] - d.lo[i]) * 0.5f * k;
            float l = mid - half;
            float h = mid + half;
            const int middle = (d.order == RO_XYZ || d.order == RO_ZYX) ? 1 : (d.order == RO_YXZ || d.order == RO_ZXY) ? 0 : 2;
            const float cap = i == middle ? 85.0f : 175.0f; // the middle axis must stay inside +-90
            l = l < -cap ? -cap : l;
            h = h > cap ? cap : h;
            lo[i] = l * d2r;
            hi[i] = h * d2r;
        }
        c->setAngularLowerLimit(lo);
        c->setAngularUpperLimit(hi);
        for (int i = 3; i < 6; ++i)
            c->setBounce(i, 0);
        return c;
    }
    // Hinge: bend axis = the upper bone's z axis; x = the upper segment.
    btVector3 z = BoneAxis(boneWorld, s.role[d.axisRole], 2);
    if (z.length2() < btScalar(1e-6))
        z = btVector3(0, 0, 1);
    z.normalize();
    const btVector3 pd = segDir[d.parentSeg];
    const btVector3 cd = segDir[d.childSeg];
    btVector3 x = pd - z * pd.dot(z);
    if (x.length2() < btScalar(1e-6))
    {
        btVector3 q;
        btPlaneSpace1(z, x, q);
    }
    x.normalize();
    const btVector3 y = z.cross(x);
    f.setBasis(BasisCols(x, y, z));
    // bend now, about z, from the upper segment to the lower one
    const float theta = std::atan2(pd.cross(cd).dot(z), pd.dot(cd));
    const float* q = s.restRot[s.role[d.restRole]];
    const float zw = q[2] * q[3];
    const float sign = std::fabs(q[2]) > 0.02f ? (zw >= 0.0f ? 1.0f : -1.0f) : d.defaultSign;
    const float lo = sign > 0.0f ? d.bendMin : -d.bendMax;
    const float hi = sign > 0.0f ? d.bendMax : -d.bendMin;
    // hinge angle = theta at death - theta now
    float low = theta - hi;
    float high = theta - lo;
    if (low > -0.01f)
        low = -0.01f;
    if (high < 0.01f)
        high = 0.01f;
    *bendOut = theta * sign;
    const btTransform fa = a->getCenterOfMassTransform().inverse() * f;
    const btTransform fb = b->getCenterOfMassTransform().inverse() * f;
    btHingeConstraint* h = new btHingeConstraint(*a, *b, fa, fb);
    h->setLimit(low, high);
    return h;
}

} // namespace

Tuning DefaultTuning()
{
    const Tuning t = { 0.06f, 0.4f, 0.9f, 0.15f, 0.05f, 0.85f, 1.0f, 1.0f, 0.1f };
    return t;
}

void SetTuning(const Tuning& t)
{
    g_tuning = t;
}

const Tuning& CurrentTuning()
{
    return g_tuning;
}

const char* RoleBoneName(int role)
{
    return role >= 0 && role < kRoleCount ? kRoleNames[role] : "?";
}

int Create(const Skeleton& s, const float* boneWorld, const float velocity[3], GroundFn ground, void* ctx,
           Built* built, char* why, size_t whyLen, const float* spin, float gravity)
{
    Built dummy;
    if (!built)
        built = &dummy;
    memset(built, 0, sizeof(*built));
    if (s.count < 1 || s.count > kMaxBones || !boneWorld || !ground)
        return FailHandle(why, whyLen, "bad skeleton");
    for (int k = 0; k < kRoleCount; ++k)
        if (s.role[k] < 0 || s.role[k] >= s.count)
            return FailHandle(why, whyLen, "a needed bone is missing");
    int slot = -1;
    for (int i = 0; i < kMaxRagdolls; ++i)
    {
        if (!g_rag[i].used)
        {
            slot = i;
            break;
        }
    }
    if (slot < 0)
        return FailHandle(why, whyLen, "no free ragdoll slot");
    EnsureWorld();

    btVector3 p[kRoleCount];
    for (int k = 0; k < kRoleCount; ++k)
    {
        p[k] = BonePos(boneWorld, s.role[k]);
        if (!Finite(p[k]))
            return FailHandle(why, whyLen, "pose has a NaN");
    }
    btVector3 segA[kSegCount];
    btVector3 segB[kSegCount];
    btVector3 segDir[kSegCount];
    float segLen[kSegCount];
    for (int g = 0; g < kSegCount; ++g)
    {
        SegEnds(g, p, &segA[g], &segB[g]);
        segLen[g] = (segB[g] - segA[g]).length();
        if (segLen[g] < 0.05f || segLen[g] > 1.5f)
            return FailHandle(why, whyLen, "a body part has a strange length");
        segDir[g] = (segB[g] - segA[g]) / segLen[g];
    }

    Ragdoll& r = g_rag[slot];
    r = Ragdoll();
    r.used = true;
    r.slot = slot;
    r.muscleTime = g_tuning.muscleTime;
    for (int k = 0; k < kRoleCount; ++k)
        r.role[k] = s.role[k];
    for (int j = 0; j < kJointCount; ++j)
    {
        r.servo[j][0] = r.servo[j][1] = r.servo[j][2] = 0.0f; // 15h: set from the joint below
        r.hingeTarget[j] = 0.0f;
    }
    r.count = s.count;
    r.fn = ground;
    r.ctx = ctx;
    if (!BuildGround(r, p[kRolePelvis], ground, ctx, built, why, whyLen))
    {
        Free(r);
        return -1;
    }

    const btVector3 vel(velocity ? velocity[0] : 0.0f, velocity ? velocity[1] : 0.0f, velocity ? velocity[2] : 0.0f);
    for (int g = 0; g < kSegCount; ++g)
    {
        const SegDef& d = kSegs[g];
        btTransform t;
        t.setOrigin((segA[g] + segB[g]) * btScalar(0.5));
        t.setBasis(BasisY(segDir[g], BoneAxis(boneWorld, s.role[d.primaryRole], 2)));
        r.radius[g] = d.radius;
        r.halfLen[g] = segLen[g] * 0.5f;
        r.shape[g] = new btCapsuleShape(d.radius, segLen[g]);
        btVector3 inertia(0, 0, 0);
        r.shape[g]->calculateLocalInertia(d.mass, inertia);
        r.motion[g] = new btDefaultMotionState(t);
        btRigidBody::btRigidBodyConstructionInfo ci(d.mass, r.motion[g], r.shape[g], inertia);
        ci.m_linearDamping = g_tuning.linearDamping;
        ci.m_angularDamping = g_tuning.angularDamping;
        ci.m_friction = g_tuning.friction;
        ci.m_rollingFriction = g_tuning.rollingFriction;
        ci.m_restitution = 0;
        r.body[g] = new btRigidBody(ci);
        r.body[g]->setDeactivationTime(btScalar(0.8));
        r.body[g]->setSleepingThresholds(btScalar(1.6), btScalar(2.5));
        r.body[g]->setCcdMotionThreshold(d.radius * 0.5f);
        r.body[g]->setCcdSweptSphereRadius(d.radius * 0.8f);
        r.body[g]->setLinearVelocity(vel);
        r.body[g]->setAnisotropicFriction(r.shape[g]->getAnisotropicRollingFrictionDirection(),
                                          btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);
        r.lastGood[g] = t;
        g_world->addRigidBody(r.body[g], BodyGroup(r.slot), GroundGroup(r.slot) | BodyGroup(r.slot));
        r.body[g]->setGravity(btVector3(0, -(gravity > 0.0f ? gravity : btScalar(9.81)), 0)); // after adding
        built->massKg += d.mass;
    }

    // 15e: whole-body spin (thrown soldiers): v = v0 + w x (centre - centre of mass)
    if (spin && (spin[0] != 0.0f || spin[1] != 0.0f || spin[2] != 0.0f))
    {
        const btVector3 w(spin[0], spin[1], spin[2]);
        btVector3 com(0, 0, 0);
        float mass = 0.0f;
        for (int g = 0; g < kSegCount; ++g)
        {
            com += r.lastGood[g].getOrigin() * kSegs[g].mass;
            mass += kSegs[g].mass;
        }
        com /= mass;
        for (int g = 0; g < kSegCount; ++g)
        {
            r.body[g]->setLinearVelocity(vel + w.cross(r.lastGood[g].getOrigin() - com));
            r.body[g]->setAngularVelocity(w);
        }
    }

    // Which segment each bone rides on: its own, or its nearest ancestor's.
    for (int i = 0; i < s.count; ++i)
    {
        int seg = kSegPelvis;
        int j = i;
        for (int hop = 0; hop <= kMaxBones && j >= 0 && j < s.count; ++hop)
        {
            int found = -1;
            for (int g = 0; g < kSegCount; ++g)
                if (s.role[kSegs[g].primaryRole] == j)
                    found = g;
            if (found >= 0)
            {
                seg = found;
                break;
            }
            j = s.parent[j];
        }
        r.seg[i] = seg;
        const btTransform bone = FromD3D(boneWorld + i * 16);
        r.offset[i] = r.lastGood[seg].inverse() * bone;
    }

    // 15h: rest orientations (rows) of every bone, from the skeleton's rest rotations
    btMatrix3x3 restW[kMaxBones];
    for (int i = 0; i < s.count; ++i)
    {
        const float* q = s.restRot[i];
        const float x = q[0], y = q[1], z = q[2], w = q[3];
        const btMatrix3x3 L(1 - 2 * (y * y + z * z), 2 * (x * y + z * w), 2 * (x * z - y * w), 2 * (x * y - z * w),
                            1 - 2 * (x * x + z * z), 2 * (y * z + x * w), 2 * (x * z + y * w), 2 * (y * z - x * w),
                            1 - 2 * (x * x + y * y));
        const int par = s.parent[i];
        restW[i] = (par >= 0 && par < i) ? L * restW[par] : L;
    }
    // 15h: bone_a_spine is drawn half way between the pelvis and the spine
    r.gunBone = (s.weaponBone > 0 && s.weaponBone < s.count) ? s.weaponBone : -1; // 15h r3
    r.gunState = 0;
    r.calm = 0.0f;
    r.resting = false;
    r.aSpine = s.parent[s.role[kRoleSpine]];
    if (r.aSpine >= 0 && r.aSpine < s.count && r.seg[r.aSpine] != kSegSpine)
        r.aSpineAlt = r.lastGood[kSegSpine].inverse() * FromD3D(boneWorld + r.aSpine * 16);
    else
        r.aSpine = -1;
    r.animMovedAge = 0.0f;
    for (int j = 0; j < kJointCount; ++j)
    {
        float bend = 0.0f;
        r.joint[j] = MakeJoint(r, kJoints[j], s, boneWorld, p, segDir, &bend, restW);
        g_world->addConstraint(r.joint[j], true);
        if (!kJoints[j].hinge)
        {
            // the muscles hold the angles at death until an animation is given
            btGeneric6DofSpring2Constraint* c = static_cast<btGeneric6DofSpring2Constraint*>(r.joint[j]);
            c->calculateTransforms();
            for (int a = 0; a < 3; ++a)
            {
                const float v = c->getAngle(a);
                r.servo[j][a] = std::isfinite(v) ? v : 0.0f;
            }
        }
        if (kJoints[j].hinge && kJoints[j].pivotRole == kRoleCalfL)
            built->bendKnee[0] = bend;
        else if (kJoints[j].hinge && kJoints[j].pivotRole == kRoleCalfR)
            built->bendKnee[1] = bend;
        else if (kJoints[j].hinge && kJoints[j].pivotRole == kRoleForearmL)
            built->bendElbow[0] = bend;
        else if (kJoints[j].hinge && kJoints[j].pivotRole == kRoleForearmR)
            built->bendElbow[1] = bend;
        r.joined[kJoints[j].parentSeg][kJoints[j].childSeg] = true;
        r.joined[kJoints[j].childSeg][kJoints[j].parentSeg] = true;
    }
    // 15d: pairs meeting at a shared joint never collide either
    const int shared[4][2] = { { kSegThighL, kSegSpine }, { kSegThighR, kSegSpine }, { kSegUpperArmL, kSegHead },
                               { kSegUpperArmR, kSegHead } };
    for (const auto& pr : shared)
    {
        r.joined[pr[0]][pr[1]] = true;
        r.joined[pr[1]][pr[0]] = true;
        r.body[pr[0]]->setIgnoreCollisionCheck(r.body[pr[1]], true);
        r.body[pr[1]]->setIgnoreCollisionCheck(r.body[pr[0]], true);
    }
    // 15d: parts overlapping in the death pose wait until they are apart
    for (int a = 0; a < kSegCount; ++a)
        for (int b = a + 1; b < kSegCount; ++b)
            if (!r.joined[a][b] && CapsuleGap(r, a, b) < kTouching)
                SetIgnore(r, a, b, true);
    built->overlapsAtStart = r.ignoredCount;
    SetMuscle(r, 1.0f, kSubStep);
    built->lowestAbove = Lowest(r);
    return slot;
}

void Push(int handle, const float dv[3], const float* spin)
{
    if (handle < 0 || handle >= kMaxRagdolls || !g_rag[handle].used || g_rag[handle].frozen || !dv)
        return;
    Ragdoll& r = g_rag[handle];
    const btVector3 d(dv[0], dv[1], dv[2]);
    btVector3 w(0, 0, 0);
    btVector3 com(0, 0, 0);
    if (spin)
    {
        w = btVector3(spin[0], spin[1], spin[2]);
        float mass = 0.0f;
        for (int g = 0; g < kSegCount; ++g)
        {
            com += r.body[g]->getWorldTransform().getOrigin() * kSegs[g].mass;
            mass += kSegs[g].mass;
        }
        com /= mass;
    }
    if (r.resting)
        SetDampingAll(r, false); // 15h r3: a blast wakes the body fully
    r.calm = 0.0f;
    for (int g = 0; g < kSegCount; ++g)
    {
        btRigidBody* b = r.body[g];
        b->activate(true);
        b->setLinearVelocity(b->getLinearVelocity() + d + w.cross(b->getWorldTransform().getOrigin() - com));
        b->setAngularVelocity(b->getAngularVelocity() + w);
    }
}

// 15f: the segment transforms of a pose, exactly as Create builds them.
bool SegTransforms(const int* role, const float* boneWorld, btTransform* out)
{
    btVector3 p[kRoleCount];
    for (int k = 0; k < kRoleCount; ++k)
    {
        p[k] = BonePos(boneWorld, role[k]);
        if (!Finite(p[k]))
            return false;
    }
    for (int g = 0; g < kSegCount; ++g)
    {
        btVector3 a;
        btVector3 b;
        SegEnds(g, p, &a, &b);
        const float len = (b - a).length();
        if (len < 0.01f)
            return false;
        out[g].setOrigin((a + b) * btScalar(0.5));
        out[g].setBasis(BasisY((b - a) / len, BoneAxis(boneWorld, role[kSegs[g].primaryRole], 2)));
    }
    return true;
}

bool SetAnimTarget(int handle, const float* boneWorld)
{
    if (handle < 0 || handle >= kMaxRagdolls || !g_rag[handle].used || g_rag[handle].frozen || !boneWorld)
        return false;
    Ragdoll& r = g_rag[handle];
    btTransform t[kSegCount];
    if (!SegTransforms(r.role, boneWorld, t))
        return false;
    float moved = 0.0f; // 15h: the largest change of a target since the last call
    for (int j = 0; j < kJointCount; ++j)
    {
        const JointDef& d = kJoints[j];
        if (d.hinge)
        {
            btHingeConstraint* h = static_cast<btHingeConstraint*>(r.joint[j]);
            const float a = h->getHingeAngle(t[d.parentSeg], t[d.childSeg]);
            if (std::isfinite(a) && std::fabs(a - r.hingeTarget[j]) > moved)
                moved = std::fabs(a - r.hingeTarget[j]);
            r.hingeTarget[j] = std::isfinite(a) ? a : 0.0f;
        }
        else
        {
            // 15h: the joint's angles in the animated pose, measured by the joint itself
            btGeneric6DofSpring2Constraint* c = static_cast<btGeneric6DofSpring2Constraint*>(r.joint[j]);
            c->calculateTransforms(t[d.parentSeg], t[d.childSeg]);
            for (int a = 0; a < 3; ++a)
            {
                const float v = c->getAngle(a);
                if (!std::isfinite(v))
                    continue;
                if (std::fabs(v - r.servo[j][a]) > moved)
                    moved = std::fabs(v - r.servo[j][a]);
                r.servo[j][a] = v;
            }
            c->calculateTransforms(); // back to the bodies as they are
        }
    }
    if (moved > kAnimMoved || !r.animTargets)
        r.animMovedAge = r.age; // 15h: the animation is still moving
    r.animTargets = true;
    return true;
}

const char* const kSegNames[kSegCount] = { "pelvis", "spine", "head", "left thigh", "left calf", "right thigh",
                                           "right calf", "left upper arm", "left forearm", "right upper arm",
                                           "right forearm" };

const char* Hit(int handle, const float point[3], const float dir[3], float impulse)
{
    if (handle < 0 || handle >= kMaxRagdolls || !g_rag[handle].used || g_rag[handle].frozen || !point || !dir)
        return nullptr;
    Ragdoll& r = g_rag[handle];
    const btVector3 pt(point[0], point[1], point[2]);
    const btVector3 d(dir[0], dir[1], dir[2]);
    if (!Finite(pt) || !Finite(d) || d.length2() < btScalar(1e-6))
        return nullptr;
    int best = -1;
    float bestDist = 1e9f;
    for (int g = 0; g < kSegCount; ++g)
    {
        const btTransform t = r.body[g]->getWorldTransform();
        const btVector3 y = t.getBasis().getColumn(1) * r.halfLen[g];
        const float dist = SegSegDist(pt, pt, t.getOrigin() - y, t.getOrigin() + y) - r.radius[g];
        if (dist < bestDist)
        {
            bestDist = dist;
            best = g;
        }
    }
    if (best < 0 || bestDist > 1.0f)
        return nullptr;
    if (r.resting)
        SetDampingAll(r, false); // 15h r3: a hit wakes the body fully
    r.calm = 0.0f;
    btRigidBody* b = r.body[best];
    b->activate(true);
    b->applyImpulse(d.normalized() * impulse, pt - b->getCenterOfMassPosition());
    return kSegNames[best];
}

void Step(float dt)
{
    dt *= g_tuning.timeScale;
    if (!g_world || !(dt > 0.0f))
        return;
    if (dt > 0.1f)
        dt = 0.1f;
    for (Ragdoll& r : g_rag)
    {
        if (!r.used || r.frozen)
            continue;
        r.age += dt;
        const float left = r.muscleTime > 0.0f ? 1.0f - r.age / r.muscleTime : 0.0f;
        float strength = left > 0.0f ? left * left : 0.0f;
        if (r.animTargets && strength < g_tuning.animKeep)
        {
            // 15f: a little of the animation stays to the end - 15h: while it moves;
            // once it has been still for kAnimHold it fades out over kAnimFade
            float keep = g_tuning.animKeep;
            const float still = r.age - r.animMovedAge - kAnimHold;
            if (still > 0.0f)
                keep *= still < kAnimFade ? 1.0f - still / kAnimFade : 0.0f;
            if (strength < keep)
                strength = keep;
        }
        if (strength > 0.0f || r.muscle > 0.0f)
            SetMuscle(r, strength, dt);
    }
    for (Ragdoll& r : g_rag)
    {
        if (r.used && !r.frozen && r.gunState == 1)
        {
            r.gunTimer -= dt;
            if (r.gunTimer <= 0.0f)
                MakeGun(r); // 15h r3
        }
    }
    g_world->stepSimulation(dt, kMaxSubSteps, kSubStep);
    for (Ragdoll& r : g_rag)
    {
        if (!r.used || r.frozen)
            continue;
        for (int a = 0; a < kSegCount; ++a)
        {
            for (int b = a + 1; b < kSegCount; ++b)
            {
                if (r.joined[a][b])
                    continue;
                const float gap = CapsuleGap(r, a, b);
                if (r.ignored[a][b])
                {
                    if (gap > kSeparate)
                        SetIgnore(r, a, b, false);
                }
                else if (-gap > r.penetration)
                {
                    r.penetration = -gap;
                }
            }
        }
        const char* bad = nullptr;
        float fastest = 0.0f;
        for (int g = 0; g < kSegCount && !bad; ++g)
        {
            const btTransform t = r.body[g]->getWorldTransform();
            const btVector3 v = r.body[g]->getLinearVelocity();
            if (!Finite(t.getOrigin()) || !Finite(v) || !Finite(r.body[g]->getAngularVelocity()))
                bad = "NaN in the physics";
            else if (v.length() > kMaxSpeed)
                bad = "a body went faster than 40 m/s";
            else if (v.length() > fastest)
                fastest = v.length();
        }
        if (!bad)
        {
            const btVector3 c = r.body[kSegPelvis]->getWorldTransform().getOrigin();
            const float dx = c.x() - r.cx;
            const float dz = c.z() - r.cz;
            if (dx * dx + dz * dz > kPatchMove * kPatchMove)
            {
                ++r.recentres;
                if (!BuildGround(r, c, r.fn, r.ctx, nullptr, nullptr, 0))
                    bad = "no terrain height under the body's new place";
            }
        }
        if (!bad && r.gun)
        {
            const btVector3 gv = r.gun->getLinearVelocity();
            if (!Finite(r.gun->getWorldTransform().getOrigin()) || !Finite(gv) || gv.length() > kMaxSpeed)
            {
                // 15h r3: a gun gone wrong goes back to the hand (the body is fine)
                g_world->removeRigidBody(r.gun);
                delete r.gun;
                delete r.gunMotion;
                delete r.gunShape;
                r.gun = nullptr;
                r.gunMotion = nullptr;
                r.gunShape = nullptr;
                r.gunState = 0;
            }
        }
        if (bad)
        {
            Freeze(r, bad);
            continue;
        }
        if (fastest > r.speedMax)
            r.speedMax = fastest;
        // 15h r3: resting damping once the whole body is slow
        r.calm = fastest < kCalmSpeed ? r.calm + dt : 0.0f;
        if (!r.resting && r.calm > kCalmTime)
            SetDampingAll(r, true);
        else if (r.resting && fastest > kWakeSpeed)
            SetDampingAll(r, false);
        for (int g = 0; g < kSegCount; ++g)
            r.lastGood[g] = r.body[g]->getWorldTransform();
    }
}

bool GetPose(int handle, float* boneWorld)
{
    if (handle < 0 || handle >= kMaxRagdolls || !g_rag[handle].used || !boneWorld)
        return false;
    const Ragdoll& r = g_rag[handle];
    btTransform segNow[kSegCount];
    for (int g = 0; g < kSegCount; ++g)
        segNow[g] = SegWorld(r, g);
    for (int i = 0; i < r.count; ++i)
        ToD3D(segNow[r.seg[i]] * r.offset[i], boneWorld + i * 16);
    if (r.gunState == 2 && r.gun)
    {
        // 15h r3: hp_weapons follows the dropped gun
        btTransform g;
        r.gunMotion->getWorldTransform(g);
        ToD3D(g * r.gunOffset, boneWorld + r.gunBone * 16);
    }
    if (r.aSpine >= 0)
    {
        // 15h: bone_a_spine turns half way between the pelvis and the spine
        const btTransform onPelvis = segNow[r.seg[r.aSpine]] * r.offset[r.aSpine];
        const btTransform onSpine = segNow[kSegSpine] * r.aSpineAlt;
        const btQuaternion q = onPelvis.getRotation().slerp(onSpine.getRotation(), btScalar(0.5));
        ToD3D(btTransform(q, onPelvis.getOrigin()), boneWorld + r.aSpine * 16);
    }
    return true;
}

bool GetStatus(int handle, Status* out)
{
    if (handle < 0 || handle >= kMaxRagdolls || !g_rag[handle].used || !out)
        return false;
    const Ragdoll& r = g_rag[handle];
    const btVector3 c = SegWorld(r, kSegPelvis).getOrigin();
    out->pelvis[0] = c.x();
    out->pelvis[1] = c.y();
    out->pelvis[2] = c.z();
    float now = 0.0f;
    bool asleep = true;
    for (int g = 0; g < kSegCount; ++g)
    {
        const float v = r.body[g]->getLinearVelocity().length();
        if (v > now)
            now = v;
        if (r.body[g]->isActive())
            asleep = false;
    }
    out->speed = r.frozen ? 0.0f : now;
    out->speedMax = r.speedMax;
    out->lowestAbove = Lowest(r);
    out->asleep = asleep && !r.frozen;
    out->frozen = r.frozen;
    out->frozenWhy = r.frozenWhy ? r.frozenWhy : "";
    out->recentres = r.recentres;
    out->overlapsWaiting = r.ignoredCount;
    out->selfPenetration = r.penetration;
    out->muscle = r.muscle;
    return true;
}

void Destroy(int handle)
{
    if (handle < 0 || handle >= kMaxRagdolls || !g_rag[handle].used)
        return;
    Free(g_rag[handle]);
}

// 17e: one shared slab shape for every wall
btBoxShape* WallShape()
{
    static btBoxShape* shape = nullptr;
    if (!shape)
        shape = new btBoxShape(btVector3(btScalar(0.5), btScalar(1.0), btScalar(0.08)));
    return shape;
}

bool AddWall(int handle, const float point[3], const float normal[3])
{
    if (handle < 0 || handle >= kMaxRagdolls || !g_rag[handle].used || !g_world)
        return false;
    Ragdoll& r = g_rag[handle];
    if (r.frozen)
        return false;
    btVector3 n(normal[0], normal[1], normal[2]);
    const btVector3 p(point[0], point[1], point[2]);
    if (!Finite(n) || !Finite(p) || n.length2() < 0.5f)
        return false;
    n.normalize();
    // already covered: a slab within 0.4 m facing nearly the same way
    for (int w = 0; w < kMaxWalls; ++w)
        if (r.wall[w] && (r.wallPoint[w] - p).length2() < 0.16f && r.wallNormal[w].dot(n) > 0.9f)
            return false;
    // the slab's thin axis (z) along the normal, its tall axis (y) as close to up as the wall allows
    btVector3 up(0, 1, 0);
    if (std::fabs(n.dot(up)) > 0.9f)
        up = btVector3(1, 0, 0); // a ceiling / floor-like face: any axis across it
    btVector3 x = up.cross(n);
    x.normalize();
    const btVector3 y = n.cross(x);
    btMatrix3x3 basis(x.x(), y.x(), n.x(), x.y(), y.y(), n.y(), x.z(), y.z(), n.z());
    btTransform t(basis, p - n * btScalar(0.08)); // just behind the surface
    const int w = r.wallNext;
    r.wallNext = (r.wallNext + 1) % kMaxWalls;
    if (r.wall[w])
    {
        r.wall[w]->setWorldTransform(t);
        r.wallMotion[w]->setWorldTransform(t);
        g_world->updateSingleAabb(r.wall[w]);
    }
    else
    {
        r.wallMotion[w] = new btDefaultMotionState(t);
        btRigidBody::btRigidBodyConstructionInfo ci(0, r.wallMotion[w], WallShape(), btVector3(0, 0, 0));
        ci.m_friction = btScalar(0.6);
        ci.m_restitution = 0;
        r.wall[w] = new btRigidBody(ci);
        g_world->addRigidBody(r.wall[w], GroundGroup(r.slot), BodyGroup(r.slot));
        ++r.walls;
    }
    r.wallPoint[w] = p;
    r.wallNormal[w] = n;
    // wake the body: it may already be resting against where the wall is
    for (int g = 0; g < kSegCount; ++g)
        if (r.body[g])
            r.body[g]->activate(true);
    return true;
}

int WallCount(int handle)
{
    if (handle < 0 || handle >= kMaxRagdolls || !g_rag[handle].used)
        return 0;
    return g_rag[handle].walls;
}

void ReleaseWeapon(int handle, float delay)
{
    if (handle < 0 || handle >= kMaxRagdolls || !g_rag[handle].used)
        return;
    Ragdoll& r = g_rag[handle];
    if (r.gunBone < 0 || r.gunState != 0 || r.frozen)
        return;
    r.gunState = 1;
    r.gunTimer = delay > 0.0f ? delay : 0.0f;
}

const char* FileBuild()
{
    return "p23a-r1";
}

bool Prepare()
{
    return EnsureWorld();
}

const char* PartName(int part)
{
    return part >= 0 && part < kSegCount ? kSegNames[part] : "?";
}

int GetParts(int handle, float* out, int maxParts)
{
    if (handle < 0 || handle >= kMaxRagdolls || !g_rag[handle].used || !out)
        return 0;
    const Ragdoll& r = g_rag[handle];
    const int n = maxParts < kSegCount ? maxParts : kSegCount;
    for (int g = 0; g < n; ++g)
    {
        float* o = out + g * 18;
        ToD3D(SegWorld(r, g), o);
        o[16] = r.radius[g];
        o[17] = r.halfLen[g];
    }
    return n > 0 ? n : 0;
}

int ActiveCount()
{
    int n = 0;
    for (const Ragdoll& r : g_rag)
        n += r.used ? 1 : 0;
    return n;
}

} // namespace ragdoll
} // namespace bf1
