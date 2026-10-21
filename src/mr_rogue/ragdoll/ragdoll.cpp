// BF1GameExt - ragdoll.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/ragdoll/; file names in the comments and log lines follow the new names. Nothing else changed.
// 22c: THE OUT-OF-MEMORY CRASH FIXED. 22b on EGM's maps crashed the moment player 1 died: the session's first ragdoll made
// the physics world, Bullet asked Windows for 3.6 MB at once (its 4096-entry contact pool), got 0 - the game's address space
// was full - and its default allocator cleared the 0 anyway (DINPUT8.dll+0x10D67C). Now: Bullet's memory goes through
// BF1GameExt (RdAlloc: malloc; a failure gives back a 2 MB reserve, tries once more, logs OUT OF MEMORY and stops new
// ragdolls for 60 s); the physics world is made at the game's DirectInput call, while memory is plentiful (ragdoll_body.cpp
// p22c-r1: 512 + 512 pool entries, 4096 + 4096 before); and a ragdoll is built only while Windows can still give 1 MB in
// one piece ("no ragdoll - out of memory: ..." in the [ragdoll] lines). Nothing else changed.
// 21j-r18: ragdolls OFF by default - [Ragdoll] Enabled missing from both files reads 0 now (was 1); BF1GameExt.ini ships with
// Enabled=0. The MODS page (the main menu's OPTIONS only) turns them on. Nothing else changed.
// 21j-r17: the MODS page's RAGDOLL EFFECTS row (menus.cpp): RagdollMenuState / RagdollMenuSet switch the ragdolls on /
// off at once (off ends the running ones, as Enabled=0 while playing did) and save [Ragdoll] Enabled in BF1GameExt.ini (and in
// ragdoll_live.ini when that file has Enabled - its keys win). With [Splitscreen] ModsMenu=1 the hooks go in even when Enabled=0
// at game start (ragdolls off until switched on - the state Enabled=0 while playing always gave), so the row works both ways
// in every session. ModsMenu=0 and Enabled=0: no hooks, as before. Nothing else changed.
// 21j-r11: the frame-rate scan's parts in the pose hook (render_hooks.cpp, [perf] lines; log only): the game's own pose
// update (kPerfPoses) and ours after it - the ragdolls, their physics and the scans (kPerfOurRagdoll). Nothing else changed.
// 21g-r4: THE SKELETON TABLE NO LONGER FILLS UP. Every soldier's skeleton goes in a table of kMaxSkeletons (16) slots
// the first time he is drawn; the slots were never given back, and each level loads its models' skeletons at new
// addresses - after a few maps in one session every new skeleton found the table full, and every death on that map was
// "NO RAGDOLL: skeleton: not readable" (5cc74a87's log: Mos Eisley, the 7th level of the session - 0 ragdolls of 226
// deaths, all "skeleton"; the maps before it had them). A slot now keeps its skeleton's bone array, bone
// count and first / last bone names, checked each time it is found: a new level's skeleton at an old one's address is
// read again. With all slots taken, the one unused longest (none of its soldiers drawn for 5 s) is given to the new
// skeleton. [ragdoll] "skeletons:" lines when that happens; "skeleton: the table is full" if all 16 are in use at once.
// 20g-r6: FOUR PLAYERS. Players 3 and 4 (splitscreen Players=3 / 4) are local players here too: their soldiers are
// refreshed with players 1 and 2's (PlayerSoldier 0..3), so a death of theirs is "his own body (his camera watches
// it)" and every other death is measured from the nearest of the four (r5's log: "AI \"Player 3\"", 38 m from
// the nearest player - only players 1 and 2 counted). [Ragdoll] Range defaults to 150 m (was 60). Nothing else changed.
// 19b: RagdollOrigDeathFn() - the game's own death function behind the ragdoll hook (split_spawn_menus.cpp's
// Respawn). Nothing else changed.
// Phase 15a (2020 build): Bullet Physics inside BF1GameExt - integration test.
// Phase 15b (2020 build: GOG 1.3 / Steam): ragdoll scan, LOG-ONLY. Its logging
// is now off unless INI [Ragdoll] Scan=1; its three hooks carry 15c.
// Phase 15c: FIRST RAGDOLL (INI [Ragdoll] Enabled=1). [ragdoll] lines.
// Phase 15d: SELF-COLLISION + MUSCLE TONE (in ragdoll_body.cpp) and the HIT
// SCAN (log-only, INI [Ragdoll] HitScan=1, [hit] lines) for the next round's
// thrown ragdolls (players too), rocket push and headshot punch:
//   - the soldier's push (primary vtable slot 32, GOG 0x00508510: velocity +=
//     impulse x scale; upward speed over 2 m/s -> state 5 "thrown", else over
//     2 m/s -> state 8): impulse, scale, velocity and state before / after,
//     the caller
//   - the soldier's damage-interface slot 5 (GOG 0x005083A0, 5 arguments,
//     ret 14h, bool): all 5 arguments raw (hex, float, and what a pointer
//     points at), his head and pelvis at that moment, the caller - BF1 keeps a
//     "headShots" career stat (stats +0x10), so a hit part exists somewhere
//   - every pushed soldier for 8 s: each state / animation change (controller
//     +0x58 state, +0x94 move), velocity, height above the terrain, and a
//     death while thrown (the death function skips SetState(3) in state 5:
//     no ragdoll for those yet - counted as "died thrown")
//   Both are vtable slots (data only), each checked like 15b's.
// 15c result (Steam, split screen): ragdolls worked first time.
// 15d result (Steam): 18 ragdolls, self-collision fine (worst 6 cm), physics
//   0.04 - 0.2 ms per frame; bodies on 20-30 deg slopes rolled like logs (up
//   to 18 m/s, 5 m): rolling friction added in 15e. Damage hook NOT installed
//   on Steam (its check wanted 2 "ret 14h", Steam has 1 in the window).
//   Pushes: all from one place (Steam rva 0x10CC53, the explosions): velocity
//   += impulse (about unit length, up and away) x 5 or 10. Thrown sequence:
//   state 5 flying 1.3 - 1.9 s (game gravity on them ~12.5 - 14 m/s^2), 8
//   landing ~0.6 s, 9 getting up 0.2 - 0.7 s, 0 standing - and a soldier the
//   explosion killed is set DEAD in that same frame (fly, land, get up, die).
// Phase 15e: THROWN RAGDOLLS + LIVE TUNING:
//   - a soldier the game throws (push -> state 5) near a local player -
//     players included, since a thrown player sees himself - becomes a
//     ragdoll at once with his velocity after the push (at most ThrownMaxSpeed)
//     and a whole-body tumble (ThrownSpin, about push direction x up: feet go
//     with the blast), under ThrownGravity (13.5: the game's arc). The game's
//     states 5 / 8 / 9 run underneath, unseen. If he is then set dead, the
//     ragdoll simply carries on. If he survives, 0.2 s after he is back on his
//     feet the body blends from the ragdoll into his own animation over 0.4 s.
//   - an explosion pushing a body that already is a ragdoll pushes the
//     ragdoll (velocity change + tumble) - corpses get blown around
//   - every physics setting is in INI [Ragdoll]; F7 reloads them in game (new
//     ragdolls use them), F8 toggles slow motion for ragdolls (SlowMotion)
//   - damage hook check: 1 or 2 "ret 14h" (Steam has 1)
// 15e-r2: CRASH FIX. r1 crashed (Steam) the first time the damage hook ran -
//   player 2's soldier spawning - reading 3 floats at 0xFFFFFFFF (BF1GameExt
//   +0x71F19, mov eax, [edi] with edi = FFFFFFFF): one of the 5 arguments was
//   -1 ("no object"), and Readable(0xFFFFFFFF, 12) said yes, because start +
//   len wrapped past 4 GB to 11 and its loop never ran. Readable now refuses
//   addresses below 64 KB and any range that wraps; the damage log only peeks
//   at 4-byte aligned pointers.
// 15e-r2 result (Steam): no crash. Damage hook decoded (150 calls): arg 2 =
//   pointer to the shot direction (unit), arg 3 = pointer to the hit point,
//   arg 4 = hit zone: 1 body (0.03 - 1.57 m above the feet), 2 HEAD (1.60 -
//   1.78 m, 20 cm from the head bone; 7 of the 9 hits within 25 cm of it),
//   -1 none (splash); arg 1 = an object, arg 5 = -1 or 2. Thrown ragdolls
//   worked (players too; 3 were set dead mid-air and stayed ragdolls). One
//   body ended 0.68 m under the terrain (a terrain cut-out?).
// Phase 15f: ANIMATION BLEND + HITS:
//   - the ragdoll's muscles follow the game's own animation (death / thrown)
//     every frame (ragdoll_body.cpp SetAnimTarget), fading over MuscleTime to
//     AnimKeep (a little of the animation stays to the end); on screen the
//     body blends from the animation into the ragdoll over BlendIn seconds.
//     The game's animated pose is copied at the frame's first update, so the
//     second split screen view blends against the same pose (15e's survivor
//     blend re-blended its own output in the second view - fixed too)
//   - hits: the last damage call (point, direction, zone) and the last push
//     are kept per soldier for 0.5 s. A ragdoll built after a hit gets an
//     impulse at the hit point on the nearest part: HeadshotPunch for zone 2
//     (plus half of it on the chest, so the body goes back), HitPush for a
//     body hit; a recent push adds its tumble. Bullets hitting a ragdoll jolt
//     it (CorpseHit).
//   - a ragdoll that would start more than 0.3 m under the terrain (a
//     cut-out: bunker, tunnel) is not built: the death animation plays
//   - seat scan (log-only, HitScan=1): soldiers seen in states 12 / 13
//     (seated: turrets, vehicles) near a player, and their deaths
// 15f result (Steam, split screen): clean session, 37 ragdolls, 9 headshots, 9
//   corpse shots, physics ~0.2 ms per frame (max 0.8).
// Phase 17e: WALLS (no new hooks):
//   - walls, railings, pillars, ceilings from the game's own ray test: every
//     0.1 s per ragdoll, 8 rays out sideways and 1 up from the pelvis and the
//     head (1.6 m, mask 0x48); each non-floor hit becomes a solid slab (1 x 2
//     x 0.16 m, ragdoll_body.cpp p17e-r1 AddWall) facing the way the surface
//     does. An approximation until the real meshes are read (see "17e:
//     WALLS"). INI [Ragdoll] Walls
//   - scans (log only): the triangle tree of the collision meshes (the block
//     before the vertices, the nodes, what they point at); the droid scan now
//     also copies the soldier before the game's death code - what that code
//     changes for droids and not for others
// 17d result (Steam, 351 deaths): builds 1.1 ms on average (17c: 51.2), the
//   longest 2.1 ms, the worst physics frame 3.3 ms - the stutter is gone.
//   Droids (77 drawn after death vs 50 not): straight after the death code
//   +0x19C is 1 / 0 and +0xF8, +0x134, +0x138, +0x188 pointers / empty - the
//   game unhooks droid bodies from the soldier drawing at death. Meshes:
//   +08 the vertex count, +0C..+20 the box, +24 the vertices (x y z floats,
//   all readable), +28 a tree of 12-byte nodes (a pointer, 6 bytes of
//   quantised box, 2 more bytes) - the triangles are at its leaves.
// Phase 17d: THE STUTTER FIXED + SCANS (no new hooks):
//   - the stutter: 17c timed every build - 51 ms average, 50 of it "naming
//     hits": every floor ray's hit was named (its class from RTTI) and handed
//     to the wall scan, each a memory check (VirtualQuery) - and VirtualQuery
//     costs ~0.2 ms in the game. Now only the one ray that decides is named;
//     memory checks are remembered per thread for 0.25 s; the death tracker
//     reads the character list in one go (17c: ~114 memory checks a death -
//     a second hitch at every death, ragdoll or not). Builds should take ~2 ms.
//   - droid scan, part 2: the soldier's fields that differ between bodies
//     drawn after death and not (see "17d: DROID SCAN")
//   - wall scan: each CollisionMesh's layout (header, vertices, the +28 array)
// 17c result (Steam, Bespin Platforms + Cloud City, 495 deaths): builds 51.2
//   ms on average (rays 1.1, terrain 0.0, naming hits 50.2); 72 bodies drawn
//   just before death and never after - all kept their controller ("kept but
//   not updated") - nearly all droids ("UNIT" names, either team; the player
//   too when playing one): the soldier draw function stops before the update;
//   walls: EntityProp +0x48 its CollisionModel, +0xC0 its world matrix (props
//   0x190 bytes apart); CollisionModel +0x8 a CollisionBodyMesh (+0x10 a local
//   matrix, +0x60 its CollisionMesh); CollisionMesh +0x08 a count (338 on an
//   85 x 85 m platform), +0x0C..+0x20 its box, +0x24 / +0x28 two arrays.
// Phase 17c: SCANS + TWO FIXES (no new hooks):
//   - fix: deaths in the air (jet pack, jump, roll, off a ledge) get ragdolls
//     - the body falls onto the floor (17b skipped them "in the air", a rule
//     from when ragdolls only knew terrain); deaths in a vehicle / turret
//     seat (state 12 / 13) get their own reason ("seated") instead
//   - fix / measure, the stutter: 17b's builds took 20-30 ms (rays only 1.3
//     ms; the physics 0.15 ms in tests) - every part of a build is timed now
//     (rays, terrain calls, naming hits, the rest), and where there is no
//     terrain under the death (Bespin Platforms) the terrain calls are skipped
//   - droid scan (in the death tracker): 44 deaths near a player (mostly team
//     2 "UNIT" droids) were drawn in full detail just before death and never
//     after; each such death now says what became of his animation controller
//     - given to another soldier (and when), kept but not updated, released
//   - wall scan (log only, WallScan): the objects floor rays hit are walked
//     for their collision data (see "17c: WALL SCAN") - for 17d's walls
// 17b result (Steam, Bespin Platforms + a map with floors 17 m over terrain,
//   293 deaths): 93 ragdolls (32 %), all on real floors (EntityProp); builds
//   20-30 ms each (the stutter) though the rays took 1.3 ms; no ragdoll near
//   a player: 44 never drawn after death (droids), 3 player deaths in the air
//   (jet pack 7 m, roll 4 m, jump 1.5 m).
// Phase 17b: FLOORS (no new hooks): ragdolls get the real floor from the
// game's own ray test (17a) - platforms, walkways, interiors, ramps, stairs,
// terrain, whichever is first below - for the ground patch and for the "is he
// on the floor?" check (now at the build, in the pose update: rays are never
// cast from the death / push hooks). Nothing below (a platform's edge): the
// floor drops 500 m, the body falls off. See "17b: FLOORS". INI [Ragdoll]
// Floors, FloorStep; RayScan now off by default.
// 17a result (Steam, Bespin Platforms): no terrain anywhere there ("no
//   terrain height there" for every death, throw and ray hit): a headshot
//   "no terrain height here", a blast throw "+108 m from the terrain"; the
//   platforms are EntityProp ~100-115 m up. The ray test: arg 1 the start,
//   arg 2 a unit direction, arg 3 the length, -> the hit fraction (1 = none);
//   out 4 the object hit (EntityProp), out 5 the surface normal (0 1 0) when
//   given; mask 0x48 = the game's own floor checks (a ray from 109.26 down
//   found the platform at 107.89); about 2300 rays a second from the game.
// Phase 17a: RAY SCAN (log only): ragdolls only know the terrain, so on
// Bespin Platforms / Cloud City (platforms, walkways, interiors: objects, the
// terrain far below or none) every death is skipped "on an object or in the
// air", and a ragdoll there would fall through the floor. The game's own
// world ray test (what bullets, the camera, explosions and AI use - it knows
// every floor) is hooked as a pass-through that only logs, to learn its
// arguments, masks and results from real calls (see "17a: RAY SCAN" below);
// 17b will use it to give ragdolls the real floor. The death tracker (15i)
// shows the reasons on those maps. INI [Ragdoll] RayScan.
// 16b result (Steam): all four limits raised (main thread paused 0.14 ms for
//   21 writes); soldiers in full detail per frame 18.9 average, 42 most (15i:
//   5.2, 8); deaths never drawn in full detail 1 of 593 (15i: 24 of 148);
//   ragdolls 17 % of deaths (15i: 11 %); still 405 of 593 out of range.
// Phase 15j: PLAYER RAGDOLLS + TRACKER FIXES (no new hooks):
//   - a local player's own death counts as 0 m away: his own camera watches
//     his body. 15c-15i measured it from the OTHER local player - in single
//     player there is none (15h: "no other local player to see the body"),
//     or the 15-frame-old pointer to his previous soldier (15i: "39 m / 150 m
//     from the nearest player", out of range) - so players never got a death
//     ragdoll, only thrown ones.
//   - tracker: character names are UTF-16 (as the spawn scan reads them; 15i
//     read them as 8-bit and got ""); the damage call's first argument is not
//     an object with a vtable in the exe (15i: "vtable rva 0x00000000"): only
//     a real RTTI class name is shown.
// 15i result (Steam, single player, 9.5 min): 290 deaths, 29 throws. Players'
//   deaths: all measured from a stale pointer (out of range) - fixed above.
//   No ragdoll: 192 out of range (60 m), 45 not drawn in time, 1 on an object.
//   Drawn in full detail at the moment of death (AI): 74 % within 20 m, ~55 %
//   at 20-40 m (the rest off screen), 14-24 % at 40-100 m, 3-4 % beyond 100 m:
//   BF1 draws far soldiers without the pose update (low detail). At most 8
//   soldiers in full detail in any frame - below the stock limit of 10, so
//   that limit was not what stopped ragdolls here.
// Phase 15i: DEATH TRACKER (log only; no new hooks - the death, damage, push,
// pose and delete hooks already in place feed it; nothing in the game changes):
//   - every death and every throw gets a [deaths] line when it is over: who
//     (player / AI, his character's name and team - the spawn scan's list,
//     linked by the unit pointer as PlayerSoldier - and his class's ODF name,
//     probed once per class), his state, where (distance to the nearest local
//     player, height above the terrain, speed), what did it (the last damage
//     within 1.5 s: headshot / body shot / splash, the damaging object's C++
//     class from its RTTI, an explosion push), whether he got a ragdoll and if
//     not exactly why (every skip reason), and whether the game drew him in
//     full detail (through the pose hook) before and after he died, with how
//     many soldiers it drew in full detail that frame (15h's 10 s summaries
//     never showed more than 10 different soldiers: BF1's hi-res unit limit?)
//   - totals every 60 s and for the session every 5 minutes
//   - INI [Ragdoll] DeathTracker=1 (0 off)
// 15h result (Steam, single player): 38 ragdolls; no ragdoll: 460 out of range
//   (most of them throws far away), 42 not drawn in time, 40 on an object or
//   in the air, 23 with no other local player alive (player 1's own deaths,
//   and AI deaths while he was dead). Every 10 s summary: exactly 10 soldiers
//   drawn.
// Phase 15h: RAGDOLL JOINTS + GUN DROP + GETTING UP (no new hooks; no new game reads):
//   - the physics (ragdoll_body.cpp p15h-r3, tried in the ragdoll lab first):
//     waist / neck / hip / shoulder joints limited to BF1's own animations
//     (they hold their twist now), the gun as its own falling body, a body
//     that is nearly still is calmed (no leg twitching). See that file.
//   - WeaponDrop (INI, live): 0 the gun stays in the hand, 1 (default) the
//     hand lets go 0.1 s after death and the gun falls from wherever the
//     ragdoll's hand is (its own physics body; hp_weapons follows it - the
//     soldier render draws the weapon there, checked in 15g), 2 the death
//     animation's throw (15g's WeaponDrop=1). Thrown soldiers keep the gun
//     until the game sets them dead.
//   - thrown survivors: the blend back starts when the game starts getting
//     him up (state 9; or when he leaves 5 / 8 / 9 without it), over 0.5 s,
//     into the game's own getting-up animation, which is first moved and
//     turned onto where the ragdoll lies (pelvis, and its head direction) and
//     eased back to the game's own place over 1.6 s (longer if it is far: at
//     most ~2 m/s) - the bones go on being
//     written (that animation, moved) until then. 15e-15g blended from the
//     lying ragdoll straight into standing, 0.2 s after he was up.
//   - lab file: thrown ragdolls' animation is recorded for 5 s (the getting
//     up too), deaths' for 2.5 s as before.
// Phase 15g: LIVE TUNING + LAB FILE (nothing new is hooked; no new game reads):
//   - [Ragdoll] reloads by itself: BF1GameExt.ini and the live file are
//     checked every 0.5 s and re-read once a change has settled (0.5 s with
//     no further change). F7 still works.
//   - the live file: Documents\BF1GameExt\ragdoll_live.ini ([Ragdoll], written by
//     the ragdoll lab page - Chrome / Edge refuse to write into Program Files,
//     where the game is). A key in it wins over BF1GameExt.ini's. Read at
//     startup too. INI LiveFile=0 ignores it.
//   - live: every physics / hit setting, Range, MaxBodies, Debug, and Enabled
//     (0 = running ragdolls end at once, no new ones; 1 = back on - only if
//     ragdolls were on at game start). Startup only: BulletTest, Scan,
//     ScanRange, HitScan, LiveFile, LabFile.
//   - a reload logs only the keys that changed (old -> new).
//   - the lab file: Documents\BF1GameExt\ragdoll_lab.txt (INI LabFile=1),
//     rewritten each game start, for the lab page: the settings in force (T
//     lines, after every reload), each ragdoll skeleton (K / B lines: rest
//     data, the same bone records 15b dumped), and the start of each ragdoll
//     built (R lines, first 60: kind, model-space bone matrices, soldier
//     rotation, velocity, blast, hit) so the lab can replay real deaths.
//     Written from a buffer at the 0.5 s check, not at the death itself.
//   - fix: this file's marker in the log said p15c-r1 since 15c; it now logs
//     its own build and ragdoll_body.cpp's.
//   - the lab file also gets the game's animated pose (A lines: 20 a second
//     for 2.5 s) of the first 10 ragdolls, so the lab replays them with the
//     animation the game really played.
//   - WeaponDrop (INI, live, default 1): a death ragdoll's hp_weapons matrix is
//     left as the game's animation wrote it. BF1's death clips (mod tools .msh)
//     throw the gun 2-3 m; riding the ragdoll's hand it stayed glued to it.
//     The soldier render (GOG 0x00510801, render+0x1541) looks hp_weapons up
//     in the drawn pose right after drawing the body and draws the current
//     weapon ([soldier+8B4h + [soldier+8E0h]*4], vtable +5Ch) there. That site
//     is checked at startup (log-only) and each death ragdoll's weapon slot is
//     logged. Thrown ragdolls keep the gun in the hand.
//
// 15b results (Steam, 7.5 min, split screen): all hooks and 300 run-time
// checks passed. Skeletons of 25 / 26 bones, all names known; the renderer's
// pose points every bone at controller+0xC0 + i*0x40 (25 of 25, 26 of 26);
// matrices are Direct3D row vectors (head +1.717 m by p x W, +102 m by W x p);
// player origins 0.003 m from TerrainHeight. Deaths: 60, all state 0/1 -> 3
// from one caller; the death animation drops the body in ~1 s, slides it
// 1-3 m, then it lies still (pelvis +0.25 m) until it is last drawn at 3.9 -
// 4.0 s. Bodies are NEVER deleted (0 deletes): dead soldier objects are reused
// for new spawns 4.9 - 20 s after death (state 3 -> 0/1/12). The game still
// moves dead bodies (one was thrown 7 m 1.5 s after death). A dead player's
// own body is not drawn (0 frames). Off-screen bodies get no updates. Hook
// cost ~0.25 ms per frame (mostly the scan's own memory checks).
//
// 15c - what happens (ragdoll_body.cpp is the physics; this file is the game
// side). Logged with Debug=1 (bounded) plus a 10 s summary:
//   1. Death (the death hook, after the game's death function): the soldier
//      becomes a ragdoll candidate if the game set state 3, another local
//      player's soldier is within Range metres (a dead player cannot see his
//      own body), his origin is within 0.5 m of the terrain height (not on a
//      building / platform / bridge, not in the air), and fewer than
//      MaxBodies ragdolls are running or waiting.
//   2. First draw after death (the pose hook): if it comes within 0.3 s of
//      death, the ragdoll is built from that frame's bone matrices (world =
//      model x soldier matrix), every body starting with the soldier's
//      velocity at death (at most 12 m/s). The skeleton needs the 16 bones in
//      ragdoll_body.hpp (pelvis, b_spine, neck, head, thighs, calves, feet,
//      upper arms, forearms, hands) within its first 32. Otherwise, or later
//      than 0.3 s, the game's death animation plays as before.
//   3. Every frame (the first pose update of the frame): the physics steps by
//      the game's frame time.
//   4. After each controller update of a ragdoll soldier: his bone matrices
//      are replaced by the ragdoll's (model = world x the soldier matrix NOW,
//      so it stays where it is while the game slides the soldier's origin).
//      The second split screen view in the same frame gets the same pose.
//   5. End: the soldier leaves state 3 (reused), or is not drawn for 1.5 s,
//      or 4.5 s after death (the game stops drawing bodies at 4 s), or he is
//      deleted. Nothing is written for him after that.
// The only game memory written: those bone matrices (controller+0xC0, at most
// 32 x 64 bytes), only for ragdoll soldiers, only right after the game's own
// update rebuilt them.
//
// Bullet 3.25 (https://github.com/bulletphysics/bullet3, zlib license - keep
// bullet3\LICENSE.txt with the release) is compiled into the DLL from its
// three single-file builds: bullet3\src\btLinearMathAll.cpp,
// btBulletCollisionAll.cpp and btBulletDynamicsAll.cpp (the project's include
// path has bullet3\src).
//
// 15a test, once at startup (INI [Ragdoll] BulletTest=1): a Bullet world with
// gravity -9.81 m/s^2 on y, a static ground plane at y = 0 and a 1 x 1 x 1 m
// box (1 kg) dropped from y = 10 m; 3 s in 180 fixed steps of 1/60 s. Passed
// in 15a: rest at y = 0.500, 180 steps in 0.28 ms.
//
// ---------------------------------------------------------------------------
// 15b - what the static analysis found (GOG 1.3; Steam is the same code)
// ---------------------------------------------------------------------------
// Soldier render: EntitySoldier's render virtual (vtable of the +0x17C
// sub-object, slot 3; GOG 0x0050F2C0, frame 0x658 bytes) runs per soldier per
// view. It builds a pose on its stack (ebp-0x658), then calls the animation
// controller update (GOG 0x00611830, thiscall, 1 argument = that pose, ret 4)
// from two places (GOG 0x0050FBF0 / 0x0050FC28), then draws the model with
// the pose. Before that it makes sure the controller's owner [ctl+0x98] is
// this soldier (else it takes and initialises another controller).
//
// Controller (soldier+0xA70), from the update (0x00611830), the matrix build
// (0x006FDF70) and the pose copy (0x006A74C0):
//   +0x98    owner soldier
//   +0xB0    skeleton instance: [+0xB0] = skeleton, [+0xB4] = local pose,
//            +0xC0 + i*0x40 = bone i's matrix in the soldier's model space
//            (room for 32 bones, up to +0x8C0)
//   +0x1C20  local pose: [+0x1C20] = the instance (+0xB0), then 0x1C bytes
//            per bone (rotation quaternion + position)
//   +0x1FFC  frame number of the last full update. Once per frame: a second
//            call in the same frame (the other split screen view) only copies
//            the matrix pointers again. Matrices written after the update
//            stay until the next frame.
// Skeleton: +0 bone array (0x70 bytes per bone), +4 bone count. Bone record:
//   +0x00 a 4x4 matrix, +0x40 rest rotation (quaternion), +0x50 rest position,
//   +0x5C name hash, +0x60 parent index (signed byte, -1 = none), +0x61 first
//   child, +0x63 flags (bit 0: keep the rest position, bit 1: scale by +0x64)
// Matrix build: root = local(0) x identity, child = local x parent (Direct3D
//   row vectors: rows = x / y / z axes, row 3 = position, +0x3C = 1.0).
// The stack pose: +0 entry count, +4 a 0x80-slot hash table (keys = bone name
//   hashes, then 0x80 values = matrix pointers, probed downwards with
//   wrap-around), +0x404 the model. The copy puts &controller matrix i in it,
//   so the renderer reads controller+0xC0 + i*0x40 directly.
// Name hash (GOG 0x006F9FD0): CRC-32 (polynomial 0x04C11DB7, MSB first, table
//   at 0x0084E2C0) over the lowercased name, start ~0, result inverted.
//   bone_root 0x16E27226, bone_l_foot 0x1AF77461, bone_r_foot 0x2F06FA86.
// Soldier world matrix: soldier+0x1B0 (64 bytes, position at +0x1E0), what
//   vtable slot 61 returns in a local game. World bone = bone x soldier.
//
// Death: the +8 sub-object's vtable slot 1 (GOG 0x0050C3E0, thiscall, no
//   arguments, plain ret; only ever called through the vtable) ends with
//       if (state != 5) SetState(3)          ; 0x0050C6F6
//   SetState (0x00513970): refuses once dead (3); stores the old state at
//   +0xA68 and zeroes +0xA6C (time in the state). So state 3 = dead and
//   +0xA6C = time since death. The render fades a dead soldier out from
//   3 s to 4 s and stops drawing him at 4 s (0x0050F496).
// Delete: the primary vtable slot 11 (GOG 0x00508200, the scalar deleting
//   destructor: thiscall, flags, ret 4; its first call is the destructor
//   0x00507D70, which writes the EntitySoldier vtables back).
// Terrain: float __cdecl TerrainHeight(float x, float z) (GOG 0x0067E510,
//   15 callers): bilinear on the height grid, -1000 when no terrain loaded.
// Frame counter: the update's second call (A1 [frame] / C3); frame time: its
//   first call (D9 05 [dt] / C3).
//
// ---------------------------------------------------------------------------
// 15b - the hooks (each checked before it is installed)
// ---------------------------------------------------------------------------
// 1. Pose: the render's two E8 calls to the controller update get BF1GameExt's
//    function as their target (only the 4-byte call distance changes). It
//    calls the game's update first, then reads. Checks: the 14-byte site
//    pattern hits exactly twice, both inside the render function (found from
//    the +0x17C vtable, prologue checked incl. its 0x658 frame), both calls
//    lead to the same function, which matches the update's 59-byte start
//    (its +0x1FFC frame test and +0x1C20 copy); the frame / dt getters match.
//    The first 300 calls are fully checked at run time (controller, pose and
//    soldier readable, soldier's vtable and +0xA70 = the controller, skeleton
//    and bones readable); any failure switches the scan off.
// 2. Death: vtable slot (data only). Checks: RTTI finds EntitySoldier's +8
//    vtable; slot 1 leads to code matching the death function's 60-byte start
//    and "state != 5 -> SetState(3)" occurs exactly once in its 0x350 bytes.
// 3. Delete: vtable slot (data only). Checks: slot 11 matches the scalar
//    deleting destructor; its first call leads to the destructor, which
//    writes exactly the primary and +8 vtables RTTI found.
// Terrain height is called (not hooked) when one pattern hit is found.
//
// Logged ([ragdoll]): the hook checks; the controller layout (first call);
// each new skeleton (up to 6): every bone's name, parent, child, flags, rest
// position and model-space position, and how many bones the renderer's pose
// points at the controller's matrices; player 1's (and 2's) soldier after
// 2 s standing still: position against the terrain height, matrix rows, key
// bones in model and world space; the players' state changes; every death
// (state before / after, position, speed, distance, caller); bodies within
// INI [Ragdoll] ScanRange metres of a player (and the players' own): key bone
// positions and their height above the terrain at 0 - 3.9 s after death, and
// when each is removed; every 10 s: frames, pose updates per frame, soldiers
// drawn, the hook's cost, deaths and removals.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Bullet's own headers are third-party code: their warnings are not ours.
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
#if defined(_MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#include "render_hooks.hpp"
#include "game_build.hpp"
#include "log.hpp"
#include "mem_log.hpp"
#include "players34.hpp"
#include "ragdoll.hpp"
#include "ragdoll_body.hpp"
#include "game_check.hpp"
#include "spawn_manager.hpp"

#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h>
#define BF1_RETURN_ADDRESS() reinterpret_cast<uintptr_t>(_ReturnAddress())
#else
#define BF1_RETURN_ADDRESS() reinterpret_cast<uintptr_t>(__builtin_return_address(0))
#endif

namespace bf1 {
namespace {

const char* const kFileBuild = "p23a-r1";

// ---------------------------------------------------------------------------
// 15a: the Bullet test
// ---------------------------------------------------------------------------
constexpr int kSteps = 180;
constexpr btScalar kStep = btScalar(1.0) / btScalar(60.0);

bool g_bulletTest = true;
bool g_rayScanIni = true; // 17a: INI RayScan
bool g_floorsIni = true;   // 17b: INI Floors
bool g_wallScanIni = true; // 17c: INI WallScan (log only)
bool g_wallsIni = true;    // 17e: INI Walls (walls from the game's ray test)
float g_floorStep = 1.0f;  // 17b: INI FloorStep
// 17a / 17b: the game's world ray test (cdecl, 9 arguments, returns the hit fraction; 1 = nothing hit)
typedef float(__cdecl* Ray_t)(uint32_t a1, uint32_t a2, float len, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7,
                              uint32_t a8, uint32_t a9);
Ray_t g_rayFn = nullptr;   // 17b: the real function (found by InstallRayScan; called directly, never via its stub)
bool g_scanIni = false;  // 15b scan logging (INI [Ragdoll] Scan, default 0 from 15c)
float g_range = 40.0f;   // 15b: ScanRange
bool g_ragdollIni = true; // 15c: Enabled
float g_rdRange = 150.0f; // 15c: Range (20g-r6: 150, was 60)
int g_rdMax = 8;          // 15c: MaxBodies
bool g_rdDebug = true;    // 15c: Debug
bool g_hitScanIniValue = true; // 15d: HitScan (copied into g_hitScanIni at setup)
float g_thrownGravity = 13.5f;  // 15e: ThrownGravity
float g_thrownSpin = 4.0f;      // 15e: ThrownSpin (rad/s for a 12 m/s push)
float g_thrownMaxSpeed = 25.0f; // 15e: ThrownMaxSpeed
float g_gravity = 9.81f;        // 15e: Gravity (deaths)
float g_slowMotion = 0.25f;     // 15e: SlowMotion (F8)
float g_blendIn = 0.25f;        // 15f: BlendIn (s)
float g_hitPush = 20.0f;        // 15f: HitPush (N s)
float g_headshotPunch = 35.0f;  // 15f: HeadshotPunch (N s)
float g_corpseHit = 8.0f;       // 15f: CorpseHit (N s)
int g_weaponDrop = 1;          // 15g: WeaponDrop - 15h: 0 hand, 1 physics, 2 the death animation's
bool g_slowOn = false;
char g_iniPath[MAX_PATH + 32] = "";
// 15g: the live file and the lab file (Documents\BF1GameExt)
char g_liveDir[MAX_PATH + 16] = "";
char g_livePath[MAX_PATH + 48] = ""; // ...\ragdoll_live.ini, "" = not used
char g_labPath[MAX_PATH + 48] = "";  // ...\ragdoll_lab.txt, "" = not written
bool g_liveFile = true;              // 15g: LiveFile (read from BF1GameExt.ini only)
bool g_labFile = true;               // 15g: LabFile (read from BF1GameExt.ini only)
bool g_trackOn = true;               // 15i: DeathTracker (read from BF1GameExt.ini only)
bool g_rdLiveOn = false;             // 15g: Enabled as last read (live); new ragdolls need it (21j-r18: off by default)
bool g_modsMenu = true;              // 21j-r17: [Splitscreen] ModsMenu (the hooks go in for the RAGDOLL EFFECTS row)
bool g_rdReady = false;              // 21j-r17: the ragdolls are set up (hooks, terrain) - the row can switch them

// 15g: a [Ragdoll] value - BF1GameExt.ini's, unless the live file has the key.
void RagdollValue(const char* key, const char* def, char* out, DWORD len)
{
    GetPrivateProfileStringA("Ragdoll", key, def, out, len, g_iniPath);
    if (!g_liveFile || !g_livePath[0])
        return;
    char buf[64] = {};
    GetPrivateProfileStringA("Ragdoll", key, "\x01", buf, sizeof(buf), g_livePath);
    if (buf[0] != '\x01')
        snprintf(out, len, "%s", buf);
}

float IniFloatRagdoll(const char* key, double def, double lo, double hi)
{
    char defText[32];
    snprintf(defText, sizeof(defText), "%g", def);
    char buf[64] = {};
    RagdollValue(key, defText, buf, sizeof(buf));
    char* end = nullptr;
    double v = strtod(buf, &end);
    if (end == buf)
        v = def;
    if (!(v >= lo))
        v = lo;
    if (v > hi)
        v = hi;
    return static_cast<float>(v);
}

// 15g: a 0 / 1 switch (the leading number, as GetPrivateProfileInt reads it).
bool IniBoolRagdoll(const char* key, bool def)
{
    char buf[64] = {};
    RagdollValue(key, def ? "1" : "0", buf, sizeof(buf));
    char* end = nullptr;
    const long v = strtol(buf, &end, 10);
    return end == buf ? def : v != 0;
}

// 15e: the physics settings ([Ragdoll], defaults = ragdoll::DefaultTuning()).
void ReadTuning()
{
    const ragdoll::Tuning d = ragdoll::DefaultTuning();
    ragdoll::Tuning t = ragdoll::CurrentTuning();
    t.muscleStrength = IniFloatRagdoll("MuscleStrength", d.muscleStrength, 0.0, 2.0);
    t.muscleTime = IniFloatRagdoll("MuscleTime", d.muscleTime, 0.0, 5.0);
    t.friction = IniFloatRagdoll("Friction", d.friction, 0.0, 5.0);
    t.rollingFriction = IniFloatRagdoll("RollingFriction", d.rollingFriction, 0.0, 2.0);
    t.linearDamping = IniFloatRagdoll("LinearDamping", d.linearDamping, 0.0, 0.99);
    t.angularDamping = IniFloatRagdoll("AngularDamping", d.angularDamping, 0.0, 0.99);
    t.jointScale = IniFloatRagdoll("JointScale", d.jointScale, 0.1, 2.0);
    g_gravity = IniFloatRagdoll("Gravity", 9.81, 1.0, 50.0);
    g_thrownGravity = IniFloatRagdoll("ThrownGravity", 13.5, 1.0, 50.0);
    g_thrownSpin = IniFloatRagdoll("ThrownSpin", 4.0, 0.0, 20.0);
    g_thrownMaxSpeed = IniFloatRagdoll("ThrownMaxSpeed", 25.0, 1.0, 60.0);
    g_slowMotion = IniFloatRagdoll("SlowMotion", 0.25, 0.02, 1.0);
    t.animKeep = IniFloatRagdoll("AnimKeep", d.animKeep, 0.0, 1.0);
    g_blendIn = IniFloatRagdoll("BlendIn", 0.25, 0.0, 2.0);
    g_hitPush = IniFloatRagdoll("HitPush", 20.0, 0.0, 500.0);
    g_headshotPunch = IniFloatRagdoll("HeadshotPunch", 35.0, 0.0, 500.0);
    g_corpseHit = IniFloatRagdoll("CorpseHit", 8.0, 0.0, 500.0);
    t.timeScale = g_slowOn ? g_slowMotion : 1.0f;
    ragdoll::SetTuning(t);
}

void LogTuning(const char* why)
{
    const ragdoll::Tuning& t = ragdoll::CurrentTuning();
    Log("[ragdoll] %s: MuscleStrength %.3f, MuscleTime %.2f, Friction %.2f, RollingFriction %.2f, LinearDamping %.2f, "
        "AngularDamping %.2f, JointScale %.2f, Gravity %.2f, ThrownGravity %.2f, ThrownSpin %.1f, ThrownMaxSpeed %.0f, "
        "SlowMotion %.2f (%s), AnimKeep %.2f, BlendIn %.2f, HitPush %.0f, HeadshotPunch %.0f, CorpseHit %.0f",
        why, static_cast<double>(t.muscleStrength), static_cast<double>(t.muscleTime),
        static_cast<double>(t.friction), static_cast<double>(t.rollingFriction), static_cast<double>(t.linearDamping),
        static_cast<double>(t.angularDamping), static_cast<double>(t.jointScale), static_cast<double>(g_gravity),
        static_cast<double>(g_thrownGravity), static_cast<double>(g_thrownSpin),
        static_cast<double>(g_thrownMaxSpeed), static_cast<double>(g_slowMotion), g_slowOn ? "ON" : "off",
        static_cast<double>(t.animKeep), static_cast<double>(g_blendIn), static_cast<double>(g_hitPush),
        static_cast<double>(g_headshotPunch), static_cast<double>(g_corpseHit));
}

// 15g: Documents\BF1GameExt - the live file and the lab file. The folder is
// made if missing. Documents = the shell's folder (moved / OneDrive too), else
// %USERPROFILE%\Documents.
typedef HRESULT(WINAPI* GetFolderPath_t)(HWND, int, HANDLE, DWORD, LPSTR);

void FindLiveDir()
{
    g_liveDir[0] = 0;
    g_livePath[0] = 0;
    g_labPath[0] = 0;
    if (!g_liveFile && !g_labFile)
        return;
    char docs[MAX_PATH] = {};
    HMODULE shell = GetModuleHandleA("shell32.dll");
    if (!shell)
        shell = LoadLibraryA("shell32.dll");
    const FARPROC fp = shell ? GetProcAddress(shell, "SHGetFolderPathA") : nullptr;
    const GetFolderPath_t getFolder = reinterpret_cast<GetFolderPath_t>(reinterpret_cast<void*>(fp));
    const int kCsidlPersonal = 5; // CSIDL_PERSONAL: Documents
    if (!getFolder || getFolder(nullptr, kCsidlPersonal, nullptr, 0, docs) < 0 || !docs[0])
    {
        char profile[MAX_PATH - 16] = {};
        const DWORD n = GetEnvironmentVariableA("USERPROFILE", profile, sizeof(profile));
        if (n == 0 || n >= sizeof(profile))
            return;
        snprintf(docs, sizeof(docs), "%s\\Documents", profile);
    }
    snprintf(g_liveDir, sizeof(g_liveDir), "%s\\BF1GameExt", docs);
    CreateDirectoryA(g_liveDir, nullptr); // already there: fails harmlessly
    if (g_liveFile)
        snprintf(g_livePath, sizeof(g_livePath), "%s\\ragdoll_live.ini", g_liveDir);
    if (g_labFile)
        snprintf(g_labPath, sizeof(g_labPath), "%s\\ragdoll_lab.txt", g_liveDir);
}

// 15g: the [Ragdoll] values that change while playing (the physics, plus
// Range / MaxBodies / Debug / Enabled).
void ReadLiveValues()
{
    ReadTuning();
    g_rdRange = IniFloatRagdoll("Range", 150.0, 1.0, 1000.0);
    g_rdMax = static_cast<int>(IniFloatRagdoll("MaxBodies", 8.0, 1.0, static_cast<double>(ragdoll::kMaxRagdolls)));
    g_rdDebug = IniBoolRagdoll("Debug", true);
    g_rdLiveOn = IniBoolRagdoll("Enabled", false); // 21j-r18: off unless a file says 1
    g_weaponDrop = static_cast<int>(IniFloatRagdoll("WeaponDrop", 1.0, 0.0, 2.0)); // 15h: 0 / 1 / 2
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
    snprintf(g_iniPath, sizeof(g_iniPath), "%s\\BF1GameExt.ini", exe);
    // 15g: these two only from BF1GameExt.ini (they decide where the rest comes from)
    g_liveFile = GetPrivateProfileIntA("Ragdoll", "LiveFile", 1, g_iniPath) != 0;
    g_labFile = GetPrivateProfileIntA("Ragdoll", "LabFile", 1, g_iniPath) != 0;
    g_trackOn = GetPrivateProfileIntA("Ragdoll", "DeathTracker", 1, g_iniPath) != 0; // 15i
    FindLiveDir();
    g_bulletTest = IniBoolRagdoll("BulletTest", true);
    g_rayScanIni = IniBoolRagdoll("RayScan", false); // 17a (17b: off by default - its job is done)
    g_floorsIni = IniBoolRagdoll("Floors", true);     // 17b
    g_floorStep = static_cast<float>(IniFloatRagdoll("FloorStep", 1.0, 0.25, 2.0));
    g_wallScanIni = IniBoolRagdoll("WallScan", true); // 17c
    g_wallsIni = IniBoolRagdoll("Walls", true);       // 17e
    g_scanIni = IniBoolRagdoll("Scan", false);
    g_range = IniFloatRagdoll("ScanRange", 40.0, 1.0, 1000.0);
    g_hitScanIniValue = IniBoolRagdoll("HitScan", true);
    ReadLiveValues();
    g_modsMenu = GetPrivateProfileIntA("Splitscreen", "ModsMenu", 1, g_iniPath) != 0; // 21j-r17
    // Enabled at game start decides whether the hooks go in (21j-r17: or the MODS page, which can switch them on later)
    g_ragdollIni = g_rdLiveOn || g_modsMenu;
}

double NowMs()
{
    LARGE_INTEGER f;
    LARGE_INTEGER c;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return static_cast<double>(c.QuadPart) * 1000.0 / static_cast<double>(f.QuadPart);
}

void BulletDropTest()
{
    btDefaultCollisionConfiguration* config = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(config);
    btBroadphaseInterface* broadphase = new btDbvtBroadphase();
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
    btDiscreteDynamicsWorld* world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config);
    world->setGravity(btVector3(0, btScalar(-9.81), 0));

    // ground: static plane y = 0
    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
    btDefaultMotionState* groundMotion = new btDefaultMotionState(btTransform::getIdentity());
    btRigidBody::btRigidBodyConstructionInfo groundInfo(0, groundMotion, groundShape, btVector3(0, 0, 0));
    btRigidBody* ground = new btRigidBody(groundInfo);
    world->addRigidBody(ground);

    // box: 1 x 1 x 1 m, 1 kg, dropped from y = 10
    btCollisionShape* boxShape = new btBoxShape(btVector3(btScalar(0.5), btScalar(0.5), btScalar(0.5)));
    btVector3 inertia(0, 0, 0);
    boxShape->calculateLocalInertia(1, inertia);
    btTransform start;
    start.setIdentity();
    start.setOrigin(btVector3(0, 10, 0));
    btDefaultMotionState* boxMotion = new btDefaultMotionState(start);
    btRigidBody::btRigidBodyConstructionInfo boxInfo(1, boxMotion, boxShape, inertia);
    btRigidBody* box = new btRigidBody(boxInfo);
    world->addRigidBody(box);

    Log("[ragdoll] Bullet %d.%02d test: world created, ground plane at y = 0, 1 m box (1 kg) dropped from y = "
        "10 m, %d steps of 1/60 s",
        btGetVersion() / 100, btGetVersion() % 100, kSteps);

    const double t0 = NowMs();
    double stepMs = 0.0;
    for (int i = 1; i <= kSteps; ++i)
    {
        const double s0 = NowMs();
        world->stepSimulation(kStep, 1, kStep);
        stepMs += NowMs() - s0;
        if (i % 30 == 0)
        {
            btTransform t;
            box->getMotionState()->getWorldTransform(t);
            const btVector3 v = box->getLinearVelocity();
            Log("[ragdoll]   t = %.1f s: box centre at y = %.3f m, falling at %.2f m/s%s",
                static_cast<double>(i) * static_cast<double>(kStep), static_cast<double>(t.getOrigin().getY()),
                static_cast<double>(-v.getY()), box->isActive() ? "" : " (asleep: at rest)");
        }
    }
    btTransform t;
    box->getMotionState()->getWorldTransform(t);
    const double y = static_cast<double>(t.getOrigin().getY());
    const bool ok = std::fabs(y - 0.5) < 0.05;
    Log("[ragdoll] Bullet test %s: the box came to rest at y = %.3f m (expected 0.5: its centre, sitting on "
        "the ground) - %d steps in %.2f ms (%.3f ms per step), %.1f ms in all",
        ok ? "PASSED" : "FAILED", y, kSteps, stepMs, stepMs / kSteps, NowMs() - t0);

    world->removeRigidBody(box);
    world->removeRigidBody(ground);
    delete box;
    delete boxMotion;
    delete boxShape;
    delete ground;
    delete groundMotion;
    delete groundShape;
    delete world;
    delete solver;
    delete broadphase;
    delete dispatcher;
    delete config;
}

// ---------------------------------------------------------------------------
// 15b: patterns (GOG 1.3 bytes, "??" over call distances / absolute addresses)
// ---------------------------------------------------------------------------
// The soldier render's start (+0x17C vtable slot 3): note its 0x658 frame.
const char* const kRenderEntry =
    "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC 58 06 00 00 56 8B F1 57";
constexpr uint32_t kRenderWindow = 0x2000; // the render is 0x1FE4 bytes on GOG 1.3
// mov ecx, [edi+8F4h] (the controller); lea eax, [ebp-658h] (the pose); push eax; call
const char* const kUpdateCall = "8B 8F F4 08 00 00 8D 85 A8 F9 FF FF 50 E8";
constexpr uint32_t kUpdateCallE8 = 13;
// The controller update's start: frame time, frame number, the +1FFCh "already
// done this frame" test and the copy of +1C20h into the pose.
const char* const kUpdateBody =
    "55 8B EC 83 EC 40 57 8B F9 E8 ?? ?? ?? ?? D9 5D F0 E8 ?? ?? ?? ?? 8B 97 FC 1F 00 00 8B C8 3B D1 75 "
    "16 8B 4D 08 8D 87 20 1C 00 00 50 E8 ?? ?? ?? ?? 5F 8B E5 5D C2 04 00";
constexpr uint32_t kUpdateDtCall = 9;
constexpr uint32_t kUpdateFrameCall = 17;
const char* const kFldGetter = "D9 05 ?? ?? ?? ?? C3"; // fld dword [dt]; ret
const char* const kMovGetter = "A1 ?? ?? ?? ?? C3";    // mov eax, [frame]; ret
// The death function (+8 vtable slot 1): prologue, its two-pass loop setup.
const char* const kDeathBody =
    "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC B8 00 00 00 56 57 8B F9 C7 "
    "45 F8 02 00 00 00 89 7D F4 83 A7 A8 00 00 00 F7 8D 87 48 0A 00 00 89 45 FC";
// cmp dword [edi+0A5Ch], 5; je +0Ah; push 3; lea ecx, [edi-8]; call SetState
const char* const kDeathSetDead = "83 BF 5C 0A 00 00 05 74 0A 6A 03 8D 4F F8 E8";
constexpr uint32_t kDeathWindow = 0x350; // the death function is 0x344 bytes on GOG 1.3
// The scalar deleting destructor (primary vtable slot 11) and the destructor.
const char* const kDeleteBody = "55 8B EC 56 8B F1 E8 ?? ?? ?? ?? 8B 45 08 A8 01 74 22 A8 04 75 10 56 E8";
constexpr uint32_t kDeleteInnerCall = 6;
const char* const kDtorStart = "53 56 8B F1 57 C7 06 ?? ?? ?? ?? C7 46 08 ?? ?? ?? ??";
constexpr uint32_t kDtorVt0 = 7;  // mov [esi], primary vtable
constexpr uint32_t kDtorVt8 = 14; // mov [esi+8], the +8 vtable
// float __cdecl TerrainHeight(float x, float z)
const char* const kTerrainHeight =
    "55 8B EC 83 EC 0C A1 ?? ?? ?? ?? 48 53 56 57 66 0F 6E C0 83 EC 0C 0F 5B C0 F3 0F 59 05 ?? ?? ?? ?? "
    "F3 0F 11 44 24 08 F3 0F 11 45 FC F3 0F 10 05 ?? ?? ?? ?? F3 0F 58 45 08 C7 44 24 04 00 00 00 00 F3 "
    "0F 11 04 24 E8";

// EntitySoldier sub-objects and vtable slots
constexpr uint32_t kSubDamage = 0x8;
constexpr uint32_t kSubRender = 0x17C;
constexpr int kSlotDeath = 1;   // +8 vtable
constexpr int kSlotRender = 3;  // +0x17C vtable
constexpr int kSlotDelete = 11; // primary vtable

// Soldier (0xB50 bytes)
constexpr uint32_t kSoldierBytes = 0xB50;
constexpr uint32_t kSoldierMatrix = 0x1B0; // 64 bytes, position at +0x1E0
constexpr uint32_t kUnitOff = 0x250;       // the controllable a character drives
constexpr uint32_t kClass = 0x830;
constexpr uint32_t kVelocity = 0x8CC;
// 15g: what the soldier render checks before it draws the weapon at hp_weapons
// (GOG 0x005107CF..0x00510864): [+8E8h] == 12 with [+6ACh] == 0 -> none;
// [+8E0h] = current weapon slot (< 0: none); [+8B4h + slot * 4] = the weapon
constexpr uint32_t kWeaponSlot = 0x8E0;
constexpr uint32_t kWeapons = 0x8B4;
constexpr uint32_t kWeaponMode = 0x8E8;
constexpr uint32_t kWeaponModeFlag = 0x6AC;
const char* const kWeaponDraw = "68 99 00 96 2B 8D 8D A8 F9 FF FF E8"; // push hp_weapons hash; lea ecx, [ebp-658h]; call
constexpr uint32_t kState = 0xA64;
constexpr uint32_t kPrevState = 0xA68;
constexpr uint32_t kStateTime = 0xA6C;
constexpr uint32_t kCtl = 0xA70;
constexpr int32_t kStateStand = 0;
constexpr int32_t kStateDead = 3;
// Class
constexpr uint32_t kClassSkeleton = 0x1AF8;
// Character
constexpr uint32_t kCharUnit = 0xB4;
// Controller
constexpr uint32_t kCtlOwner = 0x98;
constexpr uint32_t kCtlInst = 0xB0;
constexpr uint32_t kCtlInstLocal = 0xB4;
constexpr uint32_t kCtlMatrices = 0xC0;
constexpr uint32_t kCtlMaxBones = 32;
constexpr uint32_t kCtlLocalPose = 0x1C20;
constexpr uint32_t kCtlFrame = 0x1FFC;
constexpr uint32_t kCtlBytes = 0x200C;
constexpr uint32_t kMatrixBytes = 0x40;
// Skeleton / bone record
constexpr uint32_t kSkelBones = 0x0;
constexpr uint32_t kSkelCount = 0x4;
constexpr uint32_t kBoneBytes = 0x70;
constexpr uint32_t kBoneRestRot = 0x40;
constexpr uint32_t kBoneRestPos = 0x50;
constexpr uint32_t kBoneHash = 0x5C;
constexpr uint32_t kBoneParent = 0x60;
constexpr uint32_t kBoneChild = 0x61;
constexpr uint32_t kBoneNext = 0x62;
constexpr uint32_t kBoneFlags = 0x63;
constexpr uint32_t kBoneScale = 0x64; // 15g: the scale used with flags bit 1
constexpr uint32_t kMaxSkelBones = 64; // the game's bone masks are 64 bits
// Stack pose
constexpr uint32_t kPoseKeys = 0x4;
constexpr uint32_t kPoseHalf = 0x80;
constexpr uint32_t kPoseModel = 0x404;
constexpr uint32_t kPoseBytes = 0x408;

// Limits
constexpr int kCheckCalls = 300;
constexpr int kMaxLines = 700;
constexpr int kMaxSkeletons = 16;
constexpr int kMaxDumps = 6;
constexpr int kMaxBodies = 8;
constexpr int kMaxBodySessions = 14;
constexpr int kMaxDeathLines = 60;
constexpr int kMaxStateLines = 80;
constexpr int kMaxDeleteLines = 40;
constexpr DWORD kStatsMs = 10000;
constexpr DWORD kBodyGiveUpMs = 30000;
constexpr float kStillTime = 2.0f;
constexpr float kStillSpeed = 0.3f;
const float kSampleTimes[] = { 0.0f, 0.25f, 0.5f, 1.0f, 1.5f, 2.0f, 3.0f, 3.5f, 3.9f };
// 15c
constexpr float kBuildWindow = 0.3f;  // first draw must come this soon after death
constexpr float kOnTerrain = 0.5f;    // origin within this of the terrain height
constexpr float kMaxStartSpeed = 12.0f;
constexpr DWORD kNotSeenMs = 1500;    // not drawn this long: end
constexpr DWORD kLifeMs = 4500;       // the game stops drawing bodies at 4 s
constexpr DWORD kPendingMs = 500;     // candidate not drawn in this time: dropped
constexpr DWORD kThrownLifeMs = 9000; // 15e: a thrown ragdoll that never died nor got up
constexpr DWORD kGetupBlendMs = 500;  // 15h: survivor: ragdoll -> the game's getting-up animation
constexpr DWORD kMatchMs = 1600;      // 15h: that animation, put onto where he lay, eases back over this
constexpr float kGunHoldSecs = 0.1f;  // 15h: WeaponDrop 1: the hand lets go this long after death
constexpr DWORD kLabAnimThrownMs = 5000; // 15h: a thrown ragdoll's animation is recorded this long
constexpr DWORD kLabAnimMs = 2500;    // 15g: the game's animation recorded this long after the build
constexpr DWORD kLabAnimStepMs = 50;  // 15g: ... 20 times a second
constexpr int kLabAnimRecords = 10;   // 15g: ... for the first 10 ragdolls
constexpr int kMaxRdLines = 600;
constexpr uint32_t kPlayerRefreshFrames = 15;
const float kRdSampleTimes[] = { 0.5f, 1.5f, 3.0f };
constexpr int kRdSampleCount = static_cast<int>(sizeof(kRdSampleTimes) / sizeof(kRdSampleTimes[0]));
constexpr int kSampleCount = static_cast<int>(sizeof(kSampleTimes) / sizeof(kSampleTimes[0]));

// Bone names the exe uses (and a few common ones), for the dumps.
const char* const kKnownNames[] = {
    "bone_root", "dummyroot", "root", "bone_pelvis", "bone_a_spine", "bone_b_spine", "bone_spine1",
    "bone_spine2", "bone_spine3", "bone_ribcage", "bone_neck", "bone_head", "bone_l_clavicle",
    "bone_l_upperarm", "bone_l_forearm", "bone_l_hand", "bone_r_clavicle", "bone_r_upperarm",
    "bone_r_forearm", "bone_r_hand", "bone_l_thigh", "bone_l_calf", "bone_l_foot", "bone_l_toe",
    "bone_r_thigh", "bone_r_calf", "bone_r_foot", "bone_r_toe", "hp_weapons", "hp_fire", "hp_camera",
    "hp_hologram", "hp_head", "hp_l_hand", "hp_r_hand", "hp_backpack", "hp_jetpack", "hp_active",
    "bone_jaw", "bone_tail", "bone_l_finger", "bone_r_finger", "bone_l_thumb", "bone_r_thumb",
};
constexpr int kKnownCount = static_cast<int>(sizeof(kKnownNames) / sizeof(kKnownNames[0]));

enum KeyBone { kRoot, kPelvis, kHead, kLHand, kRHand, kLFoot, kRFoot, kKeyCount };
const char* const kKeyNames[kKeyCount] = {
    "bone_root", "bone_pelvis", "bone_head", "bone_l_hand", "bone_r_hand", "bone_l_foot", "bone_r_foot",
};
const char* const kKeyShort[kKeyCount] = { "root", "pelvis", "head", "lhand", "rhand", "lfoot", "rfoot" };

typedef void(__fastcall* CtlUpdate_t)(void* ctl, void* edx, void* pose);
typedef void(__fastcall* Death_t)(void* self, void* edx);
typedef void*(__fastcall* Delete_t)(void* self, void* edx, unsigned flags);
typedef float(__cdecl* TerrainHeight_t)(float x, float z);

struct Vec3
{
    float x;
    float y;
    float z;
};

struct SkelInfo
{
    uintptr_t skel;
    uint32_t count;
    int key[kKeyCount];
    int named;
    ragdoll::Skeleton rs; // 15c
    bool rsOk;
    char missing[40];     // why not (a missing bone)
    int labId;            // 15g: its number in the lab file, -1 = not written
    int weaponBone;       // 15g: hp_weapons, -1 = none
    uintptr_t bones;      // 21g-r4: its bone array, bone count (above) and first / last bone names when read - still
    uint32_t firstHash;   //   the same skeleton while they match (a new level can put another one at its address)
    uint32_t lastHash;
    DWORD lastUse;        // 21g-r4: GetTickCount when a soldier of it was last drawn (the slot unused longest is reused)
};

struct Body
{
    bool active;
    int id;
    int player; // -1 AI, 0 / 1 = player 1 / 2
    uintptr_t soldier;
    DWORD tick0;
    int nextSample;
    int32_t lastState;
    float lastTimer;
    uint32_t lastFrame;
    uint32_t frames;
    bool poseChecked;
};

struct StatsWindow
{
    DWORD start;
    uint32_t frames;
    uint32_t calls;
    uint32_t deadCalls;
    double costMs;
    double maxFrameMs;
    uint32_t deaths;
    uint32_t removedDead;
    uint32_t removedAlive;
};

// ---------------------------------------------------------------------------
// 15b state
// ---------------------------------------------------------------------------
uint32_t g_crc[256];
uint32_t g_knownHash[kKnownCount];

CRITICAL_SECTION g_lock;
bool g_lockReady = false;

CtlUpdate_t g_origUpdate = nullptr;
Death_t g_origDeath = nullptr;
Delete_t g_origDelete = nullptr;
TerrainHeight_t g_terrainHeight = nullptr;
uintptr_t g_frameVa = 0;
uintptr_t g_dtVa = 0;
uint32_t g_vtSoldier = 0;

bool g_poseOn = false;   // pose hook installed
bool g_deathOn = false;  // death hook installed
bool g_deleteOn = false; // delete hook installed
bool g_disabled = false; // a run-time check failed: the scan stops reading
int g_checksLeft = kCheckCalls;
bool g_layoutLogged = false;
DWORD g_poseThread = 0;
bool g_deathThreadLogged = false;
bool g_deleteThreadLogged = false;

int g_lines = 0;
int g_deathLines = 0;
int g_stateLines = 0;
int g_deleteLines = 0;

uint32_t g_frame = 0;
bool g_frameSeen = false;
uint32_t g_playerFrames = kPlayerRefreshFrames; // refresh on the first frame
constexpr int kLocalMax = 4; // 20g-r6: players 1-4
uintptr_t g_player[kLocalMax] = {};
int32_t g_playerState[kLocalMax] = { -1, -1, -1, -1 };
uintptr_t g_playerStateSoldier[kLocalMax] = {};
bool g_playerChecked[kLocalMax] = {};

SkelInfo g_skels[kMaxSkeletons];
int g_skelCount = 0;
int g_dumps = 0;
// 21g-r4: the skeleton table's slots given back (reused, read again), and why the last skeleton found no slot
constexpr DWORD kSkelReuseMs = 5000;
constexpr int kMaxSkelLines = 40;
uint32_t g_skelReused = 0;
uint32_t g_skelReread = 0;
uint32_t g_skelFull = 0;
uint32_t g_skelLogged = 0xFFFFFFFFu; // reused + reread + full at the last summary line
int g_skelLines = 0;
const char* g_skelWhy = "not readable";

Body g_bodies[kMaxBodies];
int g_bodySessions = 0;

StatsWindow g_win;
double g_qpcToMs = 0.0;
double g_frameCostMs = 0.0;
uintptr_t g_seen[512];
uint32_t g_seenCount = 0;

// totals for the summary
uint32_t g_totalDeaths = 0;
uint32_t g_totalRemovedDead = 0;
float g_removedMin = 1e9f;
float g_removedMax = 0.0f;
double g_removedSum = 0.0;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
bool Budget()
{
    if (g_lines >= kMaxLines)
        return false;
    ++g_lines;
    if (g_lines == kMaxLines)
        Log("[ragdoll] line limit reached, events going quiet (10 s summaries go on)");
    return true;
}

uint32_t ReadU32(uintptr_t va)
{
    uint32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

int32_t ReadI32(uintptr_t va)
{
    int32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

float ReadF32(uintptr_t va)
{
    float v = 0.0f;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

uint8_t ReadU8(uintptr_t va)
{
    return *reinterpret_cast<const uint8_t*>(va);
}

int ReadI8(uintptr_t va)
{
    return static_cast<int>(*reinterpret_cast<const int8_t*>(va));
}

Vec3 ReadVec3(uintptr_t va)
{
    Vec3 v = { ReadF32(va), ReadF32(va + 4), ReadF32(va + 8) };
    return v;
}

// True if every page of [start, start + len) is committed and readable.
// 15e-r2: refuses the first 64 KB and any range that wraps past 4 GB (r1 said
// yes to 0xFFFFFFFF + 12: the end wrapped to 11 and the loop never ran).
// 17d: VirtualQuery costs about 0.2 ms inside the game (17c: 50 ms of a 51 ms ragdoll build was memory
// checks), so readable regions are remembered per thread for a quarter second
struct ReadRegion
{
    uintptr_t lo;
    uintptr_t hi;
    DWORD tick;
};
thread_local ReadRegion t_readCache[8];
thread_local int t_readNext = 0;

bool Readable(uintptr_t start, size_t len)
{
    if (start < 0x10000 || len == 0)
        return false;
    const uintptr_t end = start + len;
    if (end <= start)
        return false;
    const DWORD now = GetTickCount();
    for (const ReadRegion& r : t_readCache)
        if (r.hi && start >= r.lo && end <= r.hi && now - r.tick < 250)
            return true;
    uintptr_t p = start;
    while (p < end)
    {
        MEMORY_BASIC_INFORMATION mbi;
        if (!VirtualQuery(reinterpret_cast<LPCVOID>(p), &mbi, sizeof(mbi)))
            return false;
        if (mbi.State != MEM_COMMIT || (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)))
            return false;
        const DWORD prot = mbi.Protect & 0xFF;
        if (prot == 0 || prot == PAGE_EXECUTE)
            return false;
        const uintptr_t next = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
        if (next <= p)
            return false;
        ReadRegion& r = t_readCache[t_readNext]; // 17d: remember it
        t_readNext = (t_readNext + 1) & 7;
        r.lo = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
        r.hi = next;
        r.tick = now;
        p = next;
    }
    return true;
}

bool InExe(uintptr_t va, size_t len)
{
    const BuildInfo* b = CurrentBuild();
    return b && va >= ExeBase() && va + len <= ExeBase() + b->sizeOfImage;
}

uint32_t RvaOf(uintptr_t va)
{
    return InExe(va, 1) ? static_cast<uint32_t>(va - ExeBase()) : 0;
}

bool InCode(uintptr_t va)
{
    uint32_t textRva = 0;
    uint32_t textSize = 0;
    if (!CodeSection(&textRva, &textSize))
        return false;
    return va >= ExeBase() + textRva && va < ExeBase() + textRva + textSize;
}

// Follows incremental-link E9 thunks from va (code). 0 if it leaves the code.
uintptr_t FollowThunks(uintptr_t va)
{
    for (int hop = 0; hop < 8; ++hop)
    {
        if (!InCode(va))
            return 0;
        if (ReadU8(va) != 0xE9)
            return va;
        va = static_cast<uintptr_t>(static_cast<int64_t>(va) + 5 + ReadI32(va + 1));
    }
    return 0;
}

// Final target of the E8 at va (following thunks), or 0.
uintptr_t CallTargetVa(uintptr_t va)
{
    if (!InCode(va) || ReadU8(va) != 0xE8)
        return 0;
    return FollowThunks(static_cast<uintptr_t>(static_cast<int64_t>(va) + 5 + ReadI32(va + 1)));
}

bool MatchVa(uintptr_t va, const char* pattern)
{
    return va && InExe(va, 1) && MatchPatternAt(RvaOf(va), pattern);
}

int HexNibble(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
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

// How often 'hex' (no wildcards) occurs in [start, start + len).
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

// Start and length of an exe section's initialised part (false if missing).
bool Section(const char* name, uintptr_t* start, uint32_t* len)
{
    const uintptr_t base = ExeBase();
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE)
        return false;
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(base + static_cast<uintptr_t>(dos->e_lfanew));
    if (nt->Signature != IMAGE_NT_SIGNATURE)
        return false;
    const auto* sec = reinterpret_cast<const IMAGE_SECTION_HEADER*>(
        reinterpret_cast<uintptr_t>(&nt->OptionalHeader) + nt->FileHeader.SizeOfOptionalHeader);
    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i)
    {
        char nm[9] = {};
        memcpy(nm, sec[i].Name, 8);
        if (strcmp(nm, name) != 0)
            continue;
        uint32_t size = sec[i].Misc.VirtualSize;
        if (sec[i].SizeOfRawData && sec[i].SizeOfRawData < size)
            size = sec[i].SizeOfRawData;
        *start = base + sec[i].VirtualAddress;
        *len = size;
        return Readable(*start, size);
    }
    return false;
}

// Aligned dword search; returns the address or 0. 'from' continues a search.
uintptr_t FindDword(uintptr_t start, uint32_t len, uint32_t value, uintptr_t from)
{
    uintptr_t p = from ? from + 4 : start;
    for (; p + 4 <= start + len; p += 4)
        if (ReadU32(p) == value)
            return p;
    return 0;
}

// vtable of class 'cls' for the sub-object at 'offset' (its complete object
// locator says so), found through the RTTI name. 0 if not found.
uintptr_t VtableByRtti(const char* cls, uint32_t offset, uintptr_t data, uint32_t dataLen, uintptr_t rdata,
                       uint32_t rdataLen)
{
    char mangled[96];
    const int n = snprintf(mangled, sizeof(mangled), ".?AV%s@@", cls);
    if (n <= 0 || n >= static_cast<int>(sizeof(mangled)))
        return 0;
    const size_t want = static_cast<size_t>(n) + 1; // with the terminating 0
    const unsigned char* d = reinterpret_cast<const unsigned char*>(data);
    for (uint32_t i = 8; i + want <= dataLen; ++i)
    {
        if (d[i] != '.' || memcmp(d + i, mangled, want) != 0)
            continue;
        const uint32_t td = static_cast<uint32_t>(data + i - 8);
        uintptr_t ref = 0;
        while ((ref = FindDword(rdata, rdataLen, td, ref)) != 0)
        {
            const uintptr_t col = ref - 12;
            if (col < rdata || ReadU32(col) != 0 || ReadU32(col + 4) != offset)
                continue;
            const uintptr_t vref = FindDword(rdata, rdataLen, static_cast<uint32_t>(col), 0);
            if (vref)
                return vref + 4;
        }
    }
    return 0;
}

void InitHashes()
{
    for (uint32_t i = 0; i < 256; ++i)
    {
        uint32_t c = i << 24;
        for (int k = 0; k < 8; ++k)
            c = (c & 0x80000000u) ? ((c << 1) ^ 0x04C11DB7u) : (c << 1);
        g_crc[i] = c;
    }
    for (int i = 0; i < kKnownCount; ++i)
    {
        uint32_t h = 0xFFFFFFFFu;
        for (const char* s = kKnownNames[i]; *s; ++s)
        {
            uint32_t c = static_cast<unsigned char>(*s);
            if (c >= 'A' && c <= 'Z')
                c += 0x20;
            h = (h << 8) ^ g_crc[(c ^ (h >> 24)) & 0xFF];
        }
        g_knownHash[i] = ~h;
    }
}

uint32_t NameHash(const char* name)
{
    uint32_t h = 0xFFFFFFFFu;
    for (const char* p = name; *p; ++p)
    {
        uint32_t c = static_cast<unsigned char>(*p);
        if (c >= 'A' && c <= 'Z')
            c += 0x20;
        h = (h << 8) ^ g_crc[(c ^ (h >> 24)) & 0xFF];
    }
    return ~h;
}

const char* NameOf(uint32_t hash)
{
    for (int i = 0; i < kKnownCount; ++i)
        if (g_knownHash[i] == hash)
            return kKnownNames[i];
    return nullptr;
}

uint32_t HashOf(const char* name)
{
    for (int i = 0; i < kKnownCount; ++i)
        if (strcmp(kKnownNames[i], name) == 0)
            return g_knownHash[i];
    return 0;
}

// The stack pose's hash table, probed like the game's (GOG 0x006FA070).
uint32_t PoseLookup(uintptr_t pose, uint32_t hash)
{
    if (!hash)
        return 0;
    const uintptr_t keys = pose + kPoseKeys;
    uint32_t i = hash & (kPoseHalf - 1);
    for (uint32_t n = 0; n < kPoseHalf; ++n)
    {
        const uint32_t k = ReadU32(keys + i * 4);
        if (k == hash)
            return ReadU32(keys + (i + kPoseHalf) * 4);
        if (k == 0)
            return 0;
        i = i ? i - 1 : kPoseHalf - 1;
    }
    return 0;
}

uintptr_t BoneMatrix(uintptr_t ctl, int i)
{
    return ctl + kCtlMatrices + static_cast<uint32_t>(i) * kMatrixBytes;
}

Vec3 MatrixPos(uintptr_t m)
{
    return ReadVec3(m + 0x30);
}

// Row vector p x W (Direct3D order).
Vec3 ToWorld(const Vec3& p, const float* w)
{
    Vec3 r = { p.x * w[0] + p.y * w[4] + p.z * w[8] + w[12], p.x * w[1] + p.y * w[5] + p.z * w[9] + w[13],
               p.x * w[2] + p.y * w[6] + p.z * w[10] + w[14] };
    return r;
}

// Column order W x p (for comparison only).
Vec3 ToWorldColumn(const Vec3& p, const float* w)
{
    Vec3 r = { w[0] * p.x + w[1] * p.y + w[2] * p.z + w[3], w[4] * p.x + w[5] * p.y + w[6] * p.z + w[7],
               w[8] * p.x + w[9] * p.y + w[10] * p.z + w[11] };
    return r;
}

void ReadMatrix(uintptr_t va, float* out)
{
    memcpy(out, reinterpret_cast<const void*>(va), 16 * sizeof(float));
}

float Length3(float x, float y, float z)
{
    return std::sqrt(x * x + y * y + z * z);
}

float Dist(const Vec3& a, const Vec3& b)
{
    return Length3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3 SoldierPos(uintptr_t soldier)
{
    return ReadVec3(soldier + kSoldierMatrix + 0x30);
}

// Terrain height under (x, z), or false if the function is missing / no terrain.
bool Ground(float x, float z, float* h)
{
    if (!g_terrainHeight)
        return false;
    const float v = g_terrainHeight(x, z);
    if (!(v > -999.0f))
        return false;
    *h = v;
    return true;
}

// "+0.12" (above the terrain) or "n/a".
void AboveText(const Vec3& p, char* out, size_t len)
{
    float h = 0.0f;
    if (Ground(p.x, p.z, &h))
        snprintf(out, len, "%+.2f", static_cast<double>(p.y - h));
    else
        snprintf(out, len, "n/a");
}

const char* StateName(int32_t s)
{
    switch (s)
    {
    case 0: return "stand";
    case 1: return "crouch";
    case 2: return "prone";
    case 3: return "DEAD";
    case 4: return "jump";
    case 7: return "jet";
    case 10: return "roll";
    case 12: return "vehicle?";
    case 13: return "vehicle?";
    default: return "?";
    }
}

uint32_t LocalPlayers()
{
    const BuildInfo* b = CurrentBuild();
    if (!b || !b->localPlayerCountRva)
        return 1;
    return ReadU32(Rva(b->localPlayerCountRva));
}

// Player k's soldier (0 if not on foot / not spawned).
uintptr_t PlayerSoldier(int k)
{
    // 20g-r6: in split screen with players 3 / 4 the game's count stays 2 (players34.cpp answers its getter 3 / 4)
    uint32_t local = LocalPlayers();
    const int more = PlayerNLocalPlayers();
    if (local >= 2 && more > 2)
        local = static_cast<uint32_t>(more);
    if (k < 0 || k >= kLocalMax || (k >= 1 && local < static_cast<uint32_t>(k + 1)))
        return 0;
    const uintptr_t ch = SpawnScanCharacter(k);
    if (!ch)
        return 0;
    const uintptr_t unit = ReadU32(ch + kCharUnit);
    if (unit <= kUnitOff)
        return 0;
    const uintptr_t s = unit - kUnitOff;
    if (!Readable(s, kSoldierBytes) || ReadU32(s) != g_vtSoldier)
        return 0;
    return s;
}

int PlayerIndex(uintptr_t soldier)
{
    if (!soldier)
        return -1;
    for (int k = 0; k < kLocalMax; ++k)
        if (soldier == g_player[k])
            return k;
    return -1;
}

// 20g-r6: the local player whose soldier this is right now (the cached ones first, then each one read again), or -1
int PlayerIndexNow(uintptr_t soldier)
{
    int p = PlayerIndex(soldier);
    for (int k = 0; p < 0 && soldier && k < kLocalMax; ++k)
        if (soldier == PlayerSoldier(k))
            p = k;
    return p;
}

const char* WhoText(int p)
{
    return p == 0 ? "player 1" : (p == 1 ? "player 2" : (p == 2 ? "player 3" : (p == 3 ? "player 4" : "AI")));
}

// ---------------------------------------------------------------------------
// Skeletons
// ---------------------------------------------------------------------------
SkelInfo* FindSkel(uintptr_t skel)
{
    for (int i = 0; i < g_skelCount; ++i)
        if (g_skels[i].skel == skel)
            return &g_skels[i];
    return nullptr;
}

// 21g-r4: true while slot s still holds the skeleton at its address: the same bone array and count, the same first and
// last bone names (read from the live skeleton - the controller just used it - and from its bones only when the array
// is the one read before)
bool SkelStill(const SkelInfo& s)
{
    const uintptr_t bones = ReadU32(s.skel + kSkelBones);
    const uint32_t count = ReadU32(s.skel + kSkelCount);
    if (bones != s.bones || count != s.count || count == 0)
        return false;
    return ReadU32(bones + kBoneHash) == s.firstHash && ReadU32(bones + (count - 1) * kBoneBytes + kBoneHash) == s.lastHash;
}

bool SkelLog()
{
    if (g_skelLines >= kMaxSkelLines)
        return false;
    ++g_skelLines;
    if (g_skelLines == kMaxSkelLines)
        Log("[ragdoll] skeletons: line limit reached (the 10 s summaries go on)");
    return true;
}

void FillSkel(SkelInfo& s, uintptr_t skel, uintptr_t bones, uint32_t count); // 21g-r4 (below)

// Adds a skeleton (checked readable). Returns null when invalid or full (g_skelWhy says which). 21g-r4: with every slot
// taken, the one unused longest (kSkelReuseMs and more) is given to it.
SkelInfo* AddSkel(uintptr_t skel)
{
    g_skelWhy = "not readable";
    if (!Readable(skel, 8))
        return nullptr;
    const uintptr_t bones = ReadU32(skel + kSkelBones);
    const uint32_t count = ReadU32(skel + kSkelCount);
    if (count == 0 || count > kMaxSkelBones || !Readable(bones, count * kBoneBytes))
        return nullptr;
    const DWORD now = GetTickCount();
    int slot = -1;
    if (g_skelCount < kMaxSkeletons)
    {
        slot = g_skelCount++;
    }
    else
    {
        DWORD oldest = 0;
        for (int i = 0; i < g_skelCount; ++i)
        {
            const DWORD idle = now - g_skels[i].lastUse;
            if (idle >= kSkelReuseMs && (slot < 0 || idle > oldest))
            {
                slot = i;
                oldest = idle;
            }
        }
        if (slot < 0)
        {
            ++g_skelFull;
            g_skelWhy = "the table is full (16 skeletons in use)";
            return nullptr;
        }
        ++g_skelReused;
        if (SkelLog())
            Log("[ragdoll] %lu ms: skeletons: all %d slots taken - slot %d (skeleton 0x%08X, none of its soldiers drawn for "
                "%.1f s) given to skeleton 0x%08X (%u bones)",
                ScanMsSinceAttach(), kMaxSkeletons, slot, static_cast<unsigned>(g_skels[slot].skel),
                static_cast<double>(oldest) / 1000.0, static_cast<unsigned>(skel), static_cast<unsigned>(count));
    }
    SkelInfo& s = g_skels[slot];
    FillSkel(s, skel, bones, count);
    return &s;
}

// 21g-r4: (re)reads slot s from skeleton skel (bones / count checked readable by the caller)
void FillSkel(SkelInfo& s, uintptr_t skel, uintptr_t bones, uint32_t count)
{
    s.skel = skel;
    s.bones = bones;
    s.firstHash = ReadU32(bones + kBoneHash);
    s.lastHash = ReadU32(bones + (count - 1) * kBoneBytes + kBoneHash);
    s.lastUse = GetTickCount();
    s.count = count;
    s.named = 0;
    s.labId = -1;
    s.weaponBone = -1;
    for (int k = 0; k < kKeyCount; ++k)
        s.key[k] = -1;
    for (uint32_t i = 0; i < count; ++i)
    {
        const uint32_t h = ReadU32(bones + i * kBoneBytes + kBoneHash);
        if (NameOf(h))
            ++s.named;
        for (int k = 0; k < kKeyCount; ++k)
            if (s.key[k] < 0 && h == HashOf(kKeyNames[k]))
                s.key[k] = static_cast<int>(i);
    }
    // 15c: the ragdoll's view of it (first 32 bones: the controller's room)
    s.rsOk = false;
    s.missing[0] = 0;
    memset(&s.rs, 0, sizeof(s.rs));
    if (count > static_cast<uint32_t>(ragdoll::kMaxBones))
    {
        snprintf(s.missing, sizeof(s.missing), "more than %d bones", ragdoll::kMaxBones);
        return;
    }
    s.rs.count = static_cast<int>(count);
    const uint32_t weaponHash = NameHash("hp_weapons"); // 15g
    for (uint32_t i = 0; i < count; ++i)
        if (ReadU32(bones + i * kBoneBytes + kBoneHash) == weaponHash)
            s.weaponBone = static_cast<int>(i);
    for (uint32_t i = 0; i < count; ++i)
    {
        const uintptr_t b = bones + i * kBoneBytes;
        s.rs.parent[i] = ReadI8(b + kBoneParent);
        memcpy(s.rs.restRot[i], reinterpret_cast<const void*>(b + kBoneRestRot), 16);
    }
    for (int r = 0; r < ragdoll::kRoleCount; ++r)
    {
        const uint32_t want = NameHash(ragdoll::RoleBoneName(r));
        s.rs.role[r] = -1;
        for (uint32_t i = 0; i < count; ++i)
            if (ReadU32(bones + i * kBoneBytes + kBoneHash) == want)
                s.rs.role[r] = static_cast<int>(i);
        if (s.rs.role[r] < 0)
        {
            snprintf(s.missing, sizeof(s.missing), "no %s", ragdoll::RoleBoneName(r));
            return;
        }
    }
    s.rs.weaponBone = s.weaponBone > 0 ? s.weaponBone : -1; // 15h: the physics' gun (ReleaseWeapon)
    s.rsOk = true;
    return;
}

// How many of the skeleton's bones the renderer's pose maps to the
// controller's own matrices (ctl+0xC0 + i*0x40).
void PoseMapCheck(const SkelInfo& s, uintptr_t ctl, uintptr_t pose, const char* indent)
{
    const uintptr_t bones = ReadU32(s.skel + kSkelBones);
    const uint32_t n = s.count < kCtlMaxBones ? s.count : kCtlMaxBones;
    int same = 0;
    int other = 0;
    int missing = 0;
    for (uint32_t i = 0; i < n; ++i)
    {
        const uint32_t h = ReadU32(bones + i * kBoneBytes + kBoneHash);
        const uint32_t p = PoseLookup(pose, h);
        if (!p)
            ++missing;
        else if (p == BoneMatrix(ctl, static_cast<int>(i)))
            ++same;
        else
            ++other;
    }
    Log("[ragdoll] %spose map: the renderer's pose holds %u entries (model 0x%08X); of %u bones %d point at "
        "controller+C0h+i*40h, %d elsewhere, %d not in it%s",
        indent, static_cast<unsigned>(ReadU32(pose)), static_cast<unsigned>(ReadU32(pose + kPoseModel)),
        static_cast<unsigned>(n), same, other, missing,
        same == static_cast<int>(n) ? " - OK: the renderer draws the controller's matrices" : "");
}

void DumpSkeleton(const SkelInfo& s, uintptr_t ctl, uintptr_t pose, uintptr_t soldier)
{
    const uintptr_t bones = ReadU32(s.skel + kSkelBones);
    const int p = PlayerIndex(soldier);
    Log("[ragdoll] skeleton #%d at 0x%08X: %u bones, %d with a known name (first seen on %s soldier 0x%08X, "
        "class 0x%08X, state %d %s)",
        g_dumps + 1, static_cast<unsigned>(s.skel), static_cast<unsigned>(s.count), s.named, WhoText(p),
        static_cast<unsigned>(soldier), static_cast<unsigned>(ReadU32(soldier + kClass)),
        static_cast<int>(ReadI32(soldier + kState)), StateName(ReadI32(soldier + kState)));
    if (s.count > kCtlMaxBones)
        Log("[ragdoll]   WARNING: more than %u bones - the controller has room for %u matrices",
            static_cast<unsigned>(kCtlMaxBones), static_cast<unsigned>(kCtlMaxBones));
    Log("[ragdoll]    i name              par chl nxt flg  rest position (local)      model position now        "
        "rest rotation (x y z w)          +60..+6F");
    int sane = 0;
    for (uint32_t i = 0; i < s.count; ++i)
    {
        const uintptr_t b = bones + i * kBoneBytes;
        const uint32_t h = ReadU32(b + kBoneHash);
        char name[24];
        const char* known = NameOf(h);
        if (known)
            snprintf(name, sizeof(name), "%s", known);
        else
            snprintf(name, sizeof(name), "hash %08X", static_cast<unsigned>(h));
        const Vec3 rp = ReadVec3(b + kBoneRestPos);
        const Vec3 rq = ReadVec3(b + kBoneRestRot);
        const float rw = ReadF32(b + kBoneRestRot + 12);
        char now[40] = "(beyond the controller's room)";
        if (i < kCtlMaxBones)
        {
            const uintptr_t m = BoneMatrix(ctl, static_cast<int>(i));
            const Vec3 mp = MatrixPos(m);
            snprintf(now, sizeof(now), "(%7.3f %7.3f %7.3f)", static_cast<double>(mp.x), static_cast<double>(mp.y),
                     static_cast<double>(mp.z));
            if (ReadF32(m + 0x3C) == 1.0f && ReadF32(m + 0x0C) == 0.0f && ReadF32(m + 0x1C) == 0.0f &&
                ReadF32(m + 0x2C) == 0.0f)
                ++sane;
        }
        char raw[40];
        int pos = 0;
        for (uint32_t k = 0; k < 16; ++k)
            pos += snprintf(raw + pos, sizeof(raw) - static_cast<size_t>(pos), "%02X",
                            static_cast<unsigned>(ReadU8(b + 0x60 + k)));
        Log("[ragdoll]   %2u %-17s %3d %3d %3d  %02X  (%7.3f %7.3f %7.3f)  %s  (%6.3f %6.3f %6.3f %6.3f)  %s",
            static_cast<unsigned>(i), name, ReadI8(b + kBoneParent), ReadI8(b + kBoneChild), ReadI8(b + kBoneNext),
            static_cast<unsigned>(ReadU8(b + kBoneFlags)), static_cast<double>(rp.x), static_cast<double>(rp.y),
            static_cast<double>(rp.z), now, static_cast<double>(rq.x), static_cast<double>(rq.y),
            static_cast<double>(rq.z), static_cast<double>(rw), raw);
    }
    const uint32_t n = s.count < kCtlMaxBones ? s.count : kCtlMaxBones;
    Log("[ragdoll]   matrices: %d of %u have a plain last column (0, 0, 0, 1) - Direct3D layout%s", sane,
        static_cast<unsigned>(n), sane == static_cast<int>(n) ? " OK" : " NOT confirmed");
    PoseMapCheck(s, ctl, pose, "  ");
}

void LabSkeleton(SkelInfo& s); // 15g (below)

SkelInfo* SkeletonOf(uintptr_t ctl, uintptr_t pose, uintptr_t soldier)
{
    const uintptr_t skel = ReadU32(ctl + kCtlInst);
    SkelInfo* s = FindSkel(skel);
    if (s)
    {
        // 21g-r4: still the skeleton read into this slot? (else a new level's, at the old one's address: read it again)
        if (SkelStill(*s))
        {
            s->lastUse = GetTickCount();
            return s;
        }
        const uintptr_t bones = ReadU32(skel + kSkelBones);
        const uint32_t count = ReadU32(skel + kSkelCount);
        if (!Readable(skel, 8) || count == 0 || count > kMaxSkelBones || !Readable(bones, count * kBoneBytes))
        {
            g_skelWhy = "not readable";
            return nullptr;
        }
        ++g_skelReread;
        if (SkelLog())
            Log("[ragdoll] %lu ms: skeletons: slot %d's skeleton 0x%08X is another one now (%u bones, was %u) - read again",
                ScanMsSinceAttach(), static_cast<int>(s - g_skels), static_cast<unsigned>(skel),
                static_cast<unsigned>(count), static_cast<unsigned>(s->count));
        FillSkel(*s, skel, bones, count);
    }
    else
    {
        s = AddSkel(skel);
    }
    if (!s)
        return nullptr;
    LabSkeleton(*s); // 15g: the rest data for the lab page (ragdoll skeletons only)
    if (g_scanIni && g_dumps < kMaxDumps && Budget())
    {
        DumpSkeleton(*s, ctl, pose, soldier);
        ++g_dumps;
    }
    return s;
}

// ---------------------------------------------------------------------------
// Run-time checks (first calls) and the controller layout
// ---------------------------------------------------------------------------
bool FailCheck(const char* what, uintptr_t ctl, uintptr_t soldier)
{
    Log("[ragdoll] RUN-TIME CHECK FAILED after %d checked calls: %s (controller 0x%08X, soldier 0x%08X) - the "
        "scan stops reading (the game is not affected)",
        kCheckCalls - g_checksLeft, what, static_cast<unsigned>(ctl), static_cast<unsigned>(soldier));
    g_disabled = true;
    return false;
}

bool FullCheck(uintptr_t ctl, uintptr_t pose, uintptr_t soldier)
{
    if (!Readable(ctl, kCtlBytes))
        return FailCheck("controller not readable", ctl, soldier);
    if (!Readable(pose, kPoseBytes))
        return FailCheck("pose not readable", ctl, soldier);
    if (!Readable(soldier, kSoldierBytes))
        return FailCheck("controller+98h is not a readable soldier", ctl, soldier);
    if (ReadU32(soldier) != g_vtSoldier)
        return FailCheck("controller+98h has not got the EntitySoldier vtable", ctl, soldier);
    if (ReadU32(soldier + kCtl) != ctl)
        return FailCheck("soldier+A70h is not this controller", ctl, soldier);
    const uintptr_t skel = ReadU32(ctl + kCtlInst);
    if (!Readable(skel, 8))
        return FailCheck("controller+B0h skeleton not readable", ctl, soldier);
    const uint32_t count = ReadU32(skel + kSkelCount);
    if (count == 0 || count > kMaxSkelBones || !Readable(ReadU32(skel + kSkelBones), count * kBoneBytes))
        return FailCheck("skeleton bone array / count not sane", ctl, soldier);
    return true;
}

void LogLayout(uintptr_t ctl, uintptr_t pose, uintptr_t soldier)
{
    const uintptr_t cls = ReadU32(soldier + kClass);
    const uintptr_t skel = ReadU32(ctl + kCtlInst);
    const uintptr_t classSkel = Readable(cls + kClassSkeleton, 4) ? ReadU32(cls + kClassSkeleton) : 0;
    const uintptr_t local = ReadU32(ctl + kCtlInstLocal);
    const uintptr_t localInst = ReadU32(ctl + kCtlLocalPose);
    Log("[ragdoll] controller layout (first update, %s soldier 0x%08X, controller 0x%08X):",
        WhoText(PlayerIndex(soldier)), static_cast<unsigned>(soldier), static_cast<unsigned>(ctl));
    Log("[ragdoll]   +98h owner = the soldier: yes; soldier+A70h = this controller: yes (checked)");
    Log("[ragdoll]   +B0h skeleton 0x%08X, class 0x%08X +1AF8h skeleton 0x%08X: %s", static_cast<unsigned>(skel),
        static_cast<unsigned>(cls), static_cast<unsigned>(classSkel), skel == classSkel ? "same" : "DIFFERENT");
    Log("[ragdoll]   +B4h local pose 0x%08X (controller+1C20h = 0x%08X: %s); [+1C20h] = 0x%08X (controller+B0h: %s)",
        static_cast<unsigned>(local), static_cast<unsigned>(ctl + kCtlLocalPose),
        local == ctl + kCtlLocalPose ? "yes" : "NO", static_cast<unsigned>(localInst),
        localInst == ctl + kCtlInst ? "yes" : "NO");
    Log("[ragdoll]   bones %u (array 0x%08X); +1FFCh last full update at frame %u (frame now %u); pose entries %u",
        static_cast<unsigned>(ReadU32(skel + kSkelCount)), static_cast<unsigned>(ReadU32(skel + kSkelBones)),
        static_cast<unsigned>(ReadU32(ctl + kCtlFrame)), static_cast<unsigned>(ReadU32(g_frameVa)),
        static_cast<unsigned>(ReadU32(pose)));
}

// ---------------------------------------------------------------------------
// Players
// ---------------------------------------------------------------------------
void LogBoneLine(const char* label, uintptr_t m, const float* w)
{
    const Vec3 mp = MatrixPos(m);
    const Vec3 wp = ToWorld(mp, w);
    char above[24];
    AboveText(wp, above, sizeof(above));
    Log("[ragdoll]   %-12s model (%7.3f %7.3f %7.3f)  world (%9.3f %8.3f %9.3f)  above terrain %s", label,
        static_cast<double>(mp.x), static_cast<double>(mp.y), static_cast<double>(mp.z), static_cast<double>(wp.x),
        static_cast<double>(wp.y), static_cast<double>(wp.z), above);
}

void PlayerCheck(int p, uintptr_t soldier, uintptr_t ctl, uintptr_t pose, const SkelInfo* s)
{
    if (!Budget())
        return;
    float w[16];
    ReadMatrix(soldier + kSoldierMatrix, w);
    const Vec3 pos = { w[12], w[13], w[14] };
    float h = 0.0f;
    if (Ground(pos.x, pos.z, &h))
        Log("[ragdoll] %s check (standing still %.1f s): position (%.3f, %.3f, %.3f), terrain height %.3f -> the "
            "soldier's origin is %+.3f m from the terrain",
            WhoText(p), static_cast<double>(ReadF32(soldier + kStateTime)), static_cast<double>(pos.x),
            static_cast<double>(pos.y), static_cast<double>(pos.z), static_cast<double>(h),
            static_cast<double>(pos.y - h));
    else
        Log("[ragdoll] %s check (standing still %.1f s): position (%.3f, %.3f, %.3f), terrain height not available",
            WhoText(p), static_cast<double>(ReadF32(soldier + kStateTime)), static_cast<double>(pos.x),
            static_cast<double>(pos.y), static_cast<double>(pos.z));
    Log("[ragdoll]   soldier matrix rows: x (%.3f %.3f %.3f) |%.3f|, y (%.3f %.3f %.3f) |%.3f|, z (%.3f %.3f %.3f) "
        "|%.3f|, last column (%.2f %.2f %.2f %.2f)",
        static_cast<double>(w[0]), static_cast<double>(w[1]), static_cast<double>(w[2]),
        static_cast<double>(Length3(w[0], w[1], w[2])), static_cast<double>(w[4]), static_cast<double>(w[5]),
        static_cast<double>(w[6]), static_cast<double>(Length3(w[4], w[5], w[6])), static_cast<double>(w[8]),
        static_cast<double>(w[9]), static_cast<double>(w[10]), static_cast<double>(Length3(w[8], w[9], w[10])),
        static_cast<double>(w[3]), static_cast<double>(w[7]), static_cast<double>(w[11]), static_cast<double>(w[15]));
    if (!s)
    {
        Log("[ragdoll]   (skeleton not known - no bone lines)");
        return;
    }
    for (int k = 0; k < kKeyCount; ++k)
    {
        if (s->key[k] < 0 || s->key[k] >= static_cast<int>(kCtlMaxBones))
        {
            Log("[ragdoll]   %-12s not in this skeleton", kKeyNames[k]);
            continue;
        }
        LogBoneLine(kKeyNames[k], BoneMatrix(ctl, s->key[k]), w);
    }
    if (s->key[kHead] >= 0 && s->key[kHead] < static_cast<int>(kCtlMaxBones))
    {
        const Vec3 mp = MatrixPos(BoneMatrix(ctl, s->key[kHead]));
        const Vec3 row = ToWorld(mp, w);
        const Vec3 col = ToWorldColumn(mp, w);
        Log("[ragdoll]   head above the soldier's origin: %+.3f m in model space (expected about +1.5..+1.9 "
            "standing); world by p x W (%.3f %.3f %.3f) %+.3f, by W x p (%.3f %.3f %.3f) %+.3f",
            static_cast<double>(mp.y), static_cast<double>(row.x), static_cast<double>(row.y),
            static_cast<double>(row.z), static_cast<double>(row.y - pos.y), static_cast<double>(col.x),
            static_cast<double>(col.y), static_cast<double>(col.z), static_cast<double>(col.y - pos.y));
    }
    PoseMapCheck(*s, ctl, pose, "  ");
}

void PlayerFrame(int p, uintptr_t soldier, uintptr_t ctl, uintptr_t pose, int32_t state, const SkelInfo* s)
{
    if (g_playerStateSoldier[p] != soldier)
    {
        g_playerStateSoldier[p] = soldier;
        g_playerState[p] = state;
        if (g_stateLines < kMaxStateLines && Budget())
        {
            ++g_stateLines;
            Log("[ragdoll] %lu ms: %s has soldier 0x%08X (state %d %s, controller 0x%08X)", ScanMsSinceAttach(),
                WhoText(p), static_cast<unsigned>(soldier), static_cast<int>(state), StateName(state),
                static_cast<unsigned>(ctl));
        }
    }
    else if (state != g_playerState[p])
    {
        if (g_stateLines < kMaxStateLines && Budget())
        {
            ++g_stateLines;
            Log("[ragdoll] %lu ms: %s state %d %s -> %d %s (+A68h now %d)", ScanMsSinceAttach(), WhoText(p),
                static_cast<int>(g_playerState[p]), StateName(g_playerState[p]), static_cast<int>(state),
                StateName(state), static_cast<int>(ReadI32(soldier + kPrevState)));
        }
        g_playerState[p] = state;
    }
    if (g_playerChecked[p] || state != kStateStand)
        return;
    const Vec3 v = ReadVec3(soldier + kVelocity);
    if (ReadF32(soldier + kStateTime) < kStillTime || Length3(v.x, v.y, v.z) > kStillSpeed)
        return;
    g_playerChecked[p] = true;
    PlayerCheck(p, soldier, ctl, pose, s);
}

// ---------------------------------------------------------------------------
// Bodies after death
// ---------------------------------------------------------------------------
Body* FindBody(uintptr_t soldier)
{
    for (Body& b : g_bodies)
        if (b.active && b.soldier == soldier)
            return &b;
    return nullptr;
}

void StartBody(uintptr_t soldier, int p)
{
    if (g_bodySessions >= kMaxBodySessions || FindBody(soldier))
        return;
    for (Body& b : g_bodies)
    {
        if (b.active)
            continue;
        memset(&b, 0, sizeof(b));
        b.active = true;
        b.id = ++g_bodySessions;
        b.player = p;
        b.soldier = soldier;
        b.tick0 = GetTickCount();
        b.lastState = -1;
        b.lastTimer = -1.0f;
        if (Budget())
            Log("[ragdoll]   -> tracking it as body #%d (pose at 0 - 3.9 s after death)", b.id);
        return;
    }
}

void EndBody(Body& b, const char* why)
{
    if (Budget())
        Log("[ragdoll] body #%d (%s, soldier 0x%08X): %s - drawn in %u frames, last drawn %.2f s after death",
            b.id, WhoText(b.player), static_cast<unsigned>(b.soldier), why, static_cast<unsigned>(b.frames),
            static_cast<double>(b.lastTimer));
    b.active = false;
}

void SampleBody(Body& b, uintptr_t ctl, uintptr_t pose, const SkelInfo* s, float t, int32_t state)
{
    if (!Budget())
        return;
    float w[16];
    ReadMatrix(b.soldier + kSoldierMatrix, w);
    char line[900];
    int pos = snprintf(line, sizeof(line), "[ragdoll] body #%d t=%.2f s state %d %s, origin (%.2f %.2f %.2f):", b.id,
                       static_cast<double>(t), static_cast<int>(state), StateName(state),
                       static_cast<double>(w[12]), static_cast<double>(w[13]), static_cast<double>(w[14]));
    float lowest = 1e9f;
    const char* lowestName = "";
    for (int k = kPelvis; k < kKeyCount && s; ++k)
    {
        if (s->key[k] < 0 || s->key[k] >= static_cast<int>(kCtlMaxBones))
            continue;
        const Vec3 wp = ToWorld(MatrixPos(BoneMatrix(ctl, s->key[k])), w);
        float h = 0.0f;
        if (Ground(wp.x, wp.z, &h))
        {
            const float above = wp.y - h;
            if (above < lowest)
            {
                lowest = above;
                lowestName = kKeyShort[k];
            }
            pos += snprintf(line + pos, sizeof(line) - static_cast<size_t>(pos), " %s (%.2f %.2f %.2f) %+.2f",
                            kKeyShort[k], static_cast<double>(wp.x), static_cast<double>(wp.y),
                            static_cast<double>(wp.z), static_cast<double>(above));
        }
        else
        {
            pos += snprintf(line + pos, sizeof(line) - static_cast<size_t>(pos), " %s (%.2f %.2f %.2f)",
                            kKeyShort[k], static_cast<double>(wp.x), static_cast<double>(wp.y),
                            static_cast<double>(wp.z));
        }
        if (pos >= static_cast<int>(sizeof(line)) - 64)
            break;
    }
    if (lowest < 1e8f)
        snprintf(line + pos, sizeof(line) - static_cast<size_t>(pos), " | lowest %s %+.2f m", lowestName,
                 static_cast<double>(lowest));
    Log("%s", line);
    if (!b.poseChecked && s)
    {
        b.poseChecked = true;
        PoseMapCheck(*s, ctl, pose, "  ");
    }
}

void BodyFrame(uintptr_t soldier, uintptr_t ctl, uintptr_t pose, int32_t state, uint32_t frame, const SkelInfo* s)
{
    Body* b = FindBody(soldier);
    if (!b || b->lastFrame == frame)
        return;
    b->lastFrame = frame;
    ++b->frames;
    if (state != b->lastState)
    {
        if (b->lastState >= 0 && Budget())
            Log("[ragdoll] body #%d: state %d %s -> %d %s", b->id, static_cast<int>(b->lastState),
                StateName(b->lastState), static_cast<int>(state), StateName(state));
        b->lastState = state;
    }
    // Dead: the game's own time since death; otherwise (e.g. state 5) our clock
    // since the death call.
    const float t = state == kStateDead ? ReadF32(soldier + kStateTime)
                                        : static_cast<float>(GetTickCount() - b->tick0) / 1000.0f;
    b->lastTimer = t;
    if (b->nextSample < kSampleCount && t >= kSampleTimes[b->nextSample])
    {
        // one sample per frame at most: skip the times already passed
        while (b->nextSample + 1 < kSampleCount && t >= kSampleTimes[b->nextSample + 1])
            ++b->nextSample;
        SampleBody(*b, ctl, pose, s, t, state);
        ++b->nextSample;
    }
}

void AgeBodies(DWORD now)
{
    for (Body& b : g_bodies)
        if (b.active && now - b.tick0 > kBodyGiveUpMs)
            EndBody(b, "still there 30 s after death, no longer tracked");
}

// ---------------------------------------------------------------------------
// 15c: ragdolls (the game side; physics in ragdoll_body.cpp)
// ---------------------------------------------------------------------------
enum RagdollKind
{
    kKindDead,   // died: state 3
    kKindThrown  // 15e: thrown by an explosion (state 5), alive or not yet set dead
};

struct Pending
{
    bool used;
    uintptr_t soldier;
    DWORD tick;
    float vel[3];
    int player;
    int kind;     // 15e
    float spin[3]; // 15e: whole-body tumble (thrown)
    float v1[3];          // 15g: thrown: velocity after the push, before ThrownMaxSpeed (lab file)
    float pushAxis[3];    // 15g: thrown: tumble axis (unit; lab file)
    float pushStrength;   // 15g: thrown: the push, m/s (lab file)
    float pos[3];         // 17b: where he died / was pushed (the floor check at build)
};

struct Active
{
    bool used;
    int id;
    int handle;
    int player;
    uintptr_t soldier;
    uintptr_t skel;
    int count;
    DWORD tick0;
    DWORD lastSeen;
    uint32_t lastFrame;
    uint32_t frames;
    float start[3];
    int nextSample;
    int kind;        // 15e
    DWORD blendTick; // 15e: survivor: blending back to his animation since, 0 = not blending
    DWORD getupTick; // 15h: survivor: getting up since (0 = not yet)
    DWORD wallTick;  // 17e: last wall probe
    int wallRays;    // 17e: rays cast for walls
    int wallHits;
    int pelvisBone;  // 15h: for the getting-up matching
    int headBone;
    bool matchPending;   // 15h: the getting-up animation still to be put onto the ragdoll
    DWORD matchTick;     // 15h: ... done at (0 = no matching)
    float matchYaw;      // 15h: turned by this (radians, about world y through matchPivot) ...
    float matchOff[3];   // ... and moved by this (world), easing to 0 over kMatchMs
    float matchPivot[3];
    DWORD matchMs;       // 15h: the easing takes this long (longer for a long way: at most ~2 m/s)
    DWORD built;     // 15f: when built (blend-in)
    bool haveAnim;   // 15f: anim holds this frame's animated pose
    int keepBone;    // 15g: WeaponDrop - hp_weapons keeps the game's own matrix (-1 = none)
    bool animRec;    // 15g: its animation goes to the lab file (A lines)
    DWORD lastAnimRec;
    float anim[ragdoll::kMaxBones * 16]; // 15f: the game's animated pose (model space), this frame
};

// 15f: the last hit and push per soldier (kept 0.5 s).
struct HitRec
{
    uintptr_t soldier;
    DWORD tick;
    float pos[3];
    float dir[3];
    int zone; // 1 body, 2 head, -1 none
};

struct PushRec
{
    uintptr_t soldier;
    DWORD tick;
    float spin[3];
    float strength;
    float axis[3]; // 15g: the tumble axis (unit), for the lab file
};

constexpr int kRecs = 32;
constexpr DWORD kRecMs = 500;
HitRec g_hitRecs[kRecs];
int g_hitNext = 0;
PushRec g_pushRecs[kRecs];
int g_pushNext = 0;

const HitRec* RecentHit(uintptr_t soldier, DWORD now)
{
    const HitRec* best = nullptr;
    for (const HitRec& h : g_hitRecs)
        if (h.soldier == soldier && now - h.tick <= kRecMs && (!best || h.tick - best->tick < 0x80000000u))
            best = &h;
    return best;
}

const PushRec* RecentPush(uintptr_t soldier, DWORD now)
{
    const PushRec* best = nullptr;
    for (const PushRec& p : g_pushRecs)
        if (p.soldier == soldier && now - p.tick <= kRecMs && (!best || p.tick - best->tick < 0x80000000u))
            best = &p;
    return best;
}

enum SkipReason
{
    kSkipAlone,
    kSkipRange,
    kSkipTerrain,
    kSkipLimit,
    kSkipLate,
    kSkipSkeleton,
    kSkipFailed,
    kSkipThrown,
    kSkipSeated, // 17c: died in a vehicle / turret seat (state 12 / 13)
    kSkipCount
};

Pending g_pending[ragdoll::kMaxRagdolls];
Active g_active[ragdoll::kMaxRagdolls];
int g_rdIds = 0;
int g_rdLines = 0;
uint32_t g_rdBuilt = 0;
uint32_t g_rdEnded = 0;
uint32_t g_rdSkips[kSkipCount] = {};
uint32_t g_rdWinBuilt = 0;
double g_rdStepMs = 0.0;
double g_rdStepMax = 0.0;
double g_rdApplyMs = 0.0;
uint32_t g_rdWinFrames = 0;

bool RdLog()
{
    if (!g_rdDebug || g_rdLines >= kMaxRdLines)
        return false;
    ++g_rdLines;
    if (g_rdLines == kMaxRdLines)
        Log("[ragdoll] ragdoll line limit reached, going quiet (10 s summaries go on)");
    return true;
}

double QpcMs()
{
    LARGE_INTEGER q;
    QueryPerformanceCounter(&q);
    return static_cast<double>(q.QuadPart) * g_qpcToMs;
}

// ---------------------------------------------------------------------------
// 17b: FLOORS - the game's own world ray test (17a: it sees every platform,
// walkway and interior; a ray down with mask 0x48 found Bespin's EntityProp
// floors) gives the ragdoll its floor: platforms, walkways, interiors, ramps,
// stairs, terrain - whichever is first below. The ragdoll's ground patch
// (25 x 25 heights, 0.5 m apart, from the physics core's callback) is filled
// from rays cast on a coarser world-aligned grid (FloorStep, 1 m: 169 rays for
// a first patch, only the new edge when the patch follows the body), cached
// per ragdoll and blended between grid points. Where nothing is below, the
// floor drops 500 m: a body at a platform's edge falls off (into the clouds,
// or onto the level below). Rays are only cast from the pose update (never
// from the death / push hooks, which can run inside the game's own collision
// work). INI [Ragdoll] Floors, FloorStep.
// ---------------------------------------------------------------------------
const char* ObjClass(uint32_t p); // the C++ class of an object (17a, below)

constexpr float kFloorUp = 1.0f;        // rays start this far above the reference height
constexpr float kFloorDown = 40.0f;     // and look this far down
constexpr float kVoidDepth = 500.0f;    // nothing below: the floor this far down (bodies fall off edges)
constexpr uint32_t kFloorMask = 0x48;   // the game's own floor checks (17a)
constexpr float kOnFloor = 0.5f;        // a death within this of the floor gets a ragdoll
constexpr int kFloorCells = 1024;       // cached grid heights per ragdoll

struct FloorCell
{
    int ix;
    int iz;
    float h;
    float refY;
    bool used;
};

struct FloorCtx
{
    float refY;      // the pelvis height (last frame): rays start kFloorUp above it
    float step;      // grid spacing
    int rays;        // rays cast for this ragdoll
    int rayHits;
    int voids;       // grid points with nothing below
    double ms;       // time spent in rays
    bool noTerrain;  // 17c: no terrain where he died: no terrain calls
    int terrainCalls;
    double terrainMs; // 17c: time in the game's terrain height function
    double classMs;   // 17c: time naming what the rays hit
    int terrainBoth;  // 17c: rays that hit and had terrain below too
    int terrainMatched; // 17c: ... where the ray's hit was the terrain (within 5 cm)
    FloorCell cell[kFloorCells];
};

// 17c: build times with floors (10 s summaries)
int g_floorBuilds = 0;
double g_floorBuildMs = 0.0;
double g_floorBuildMax = 0.0;
double g_floorRayMs = 0.0;
double g_floorTerrainMs = 0.0;
double g_floorClassMs = 0.0;
void NoteWallProp(uint32_t obj); // 17c: the wall scan (below)

FloorCtx g_floorCtx[ragdoll::kMaxRagdolls];

bool FloorsOn()
{
    return g_floorsIni && g_rayFn;
}

// the first floor below (x, refY + kFloorUp, z): the game's ray (objects and terrain) and the terrain
// height, whichever is higher. what: "EntityProp", "terrain", ... (static text)
bool FloorAt(float x, float z, float refY, float* h, const char** what, FloorCtx* ctx, bool name = false)
{
    bool found = false;
    float best = 0.0f;
    *what = "nothing";
    if (g_rayFn && g_floorsIni)
    {
        const double t0 = QpcMs();
        float start[3] = { x, refY + kFloorUp, z };
        float dir[3] = { 0.0f, -1.0f, 0.0f };
        uint32_t hit = 0;
        const float f = g_rayFn(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(start)),
                                static_cast<uint32_t>(reinterpret_cast<uintptr_t>(dir)), kFloorDown,
                                static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&hit)), 0, 0, 0, kFloorMask, 0);
        if (ctx)
        {
            ctx->ms += QpcMs() - t0;
            ++ctx->rays;
        }
        if (std::isfinite(f) && f >= 0.0f && f < 1.0f)
        {
            best = start[1] - kFloorDown * f;
            found = true;
            const double c0 = QpcMs();
            *what = "an object";
            if (name && hit)
            {
                // 17d: only the one ray that decides (17c: naming every patch ray's hit was the stutter)
                *what = ObjClass(hit);
                NoteWallProp(hit); // 17c: its collision data, for walls (logged later, not here)
            }
            if (ctx)
            {
                ++ctx->rayHits;
                ctx->classMs += QpcMs() - c0;
            }
        }
    }
    float th = 0.0f;
    bool haveTerrain = false;
    if (!ctx || !ctx->noTerrain)
    {
        const double g0 = QpcMs();
        haveTerrain = Ground(x, z, &th);
        if (ctx)
        {
            ctx->terrainMs += QpcMs() - g0;
            ++ctx->terrainCalls;
        }
    }
    if (haveTerrain && found && ctx && th <= refY + kFloorUp + 0.01f)
    {
        ++ctx->terrainBoth;
        if (std::fabs(th - best) < 0.05f)
            ++ctx->terrainMatched; // 17c: the ray saw the terrain itself (then terrain calls are not needed)
    }
    if (haveTerrain && th <= refY + kFloorUp + 0.01f && (!found || th > best))
    {
        best = th;
        found = true;
        *what = "terrain";
    }
    if (found)
        *h = best;
    return found;
}

// one grid point's floor, cached (recast if the body has moved 2 m up or down since)
float FloorCellHeight(FloorCtx& c, int ix, int iz)
{
    const uint32_t mask = kFloorCells - 1;
    uint32_t i = (static_cast<uint32_t>(ix) * 73856093u ^ static_cast<uint32_t>(iz) * 19349663u) & mask;
    FloorCell* slot = nullptr;
    for (int n = 0; n < 32; ++n, i = (i + 1) & mask)
    {
        FloorCell& e = c.cell[i];
        if (e.used && e.ix == ix && e.iz == iz)
        {
            if (std::fabs(e.refY - c.refY) <= 2.0f)
                return e.h;
            slot = &e;
            break;
        }
        if (!e.used)
        {
            slot = &e;
            break;
        }
    }
    float h = 0.0f;
    const char* what = nullptr;
    if (!FloorAt(static_cast<float>(ix) * c.step, static_cast<float>(iz) * c.step, c.refY, &h, &what, &c))
    {
        h = c.refY - kVoidDepth;
        ++c.voids;
    }
    if (slot)
    {
        slot->used = true;
        slot->ix = ix;
        slot->iz = iz;
        slot->h = h;
        slot->refY = c.refY;
    }
    return h;
}

// 17e: WALLS (for now from the game's ray test; real meshes later): every 0.1 s per ragdoll, 8 rays out
// sideways (45 deg apart, turned 22.5 deg every other time) and 1 up from the pelvis and the head, 1.6 m
// long, mask 0x48 (props / buildings, not soldiers); each hit that is not floor-like (that is the ground
// patch's) becomes a solid slab just behind the surface, facing the way the surface does (the ray test's
// normal; back along the ray if it gave none) - walls, railings, pillars, ceilings.
constexpr DWORD kWallEveryMs = 100;
constexpr float kWallReach = 1.6f;
int g_wallRaysTotal = 0;
int g_wallSlabsTotal = 0;

void ProbeWallsFrom(Active& a, const float* from, int turn)
{
    for (int k = 0; k < 9; ++k)
    {
        float dir[3];
        if (k < 8)
        {
            const float ang = (static_cast<float>(k) * 45.0f + static_cast<float>(turn) * 22.5f) * 0.0174533f;
            dir[0] = std::cos(ang);
            dir[1] = 0.0f;
            dir[2] = std::sin(ang);
        }
        else
        {
            dir[0] = 0.0f;
            dir[1] = 1.0f; // a ceiling / overhang
            dir[2] = 0.0f;
        }
        float start[3] = { from[0], from[1], from[2] };
        uint32_t hit = 0;
        float n[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
        const float f = g_rayFn(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(start)),
                                static_cast<uint32_t>(reinterpret_cast<uintptr_t>(dir)), kWallReach,
                                static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&hit)),
                                static_cast<uint32_t>(reinterpret_cast<uintptr_t>(n)), 0, 0, kFloorMask, 0);
        ++a.wallRays;
        ++g_wallRaysTotal;
        if (!std::isfinite(f) || f < 0.0f || f >= 1.0f)
            continue;
        float normal[3] = { n[0], n[1], n[2] };
        const float len = std::sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
        if (!std::isfinite(len) || len < 0.9f || len > 1.1f || normal[0] * dir[0] + normal[1] * dir[1] + normal[2] * dir[2] > 0.0f)
        {
            normal[0] = -dir[0]; // no usable normal: facing back along the ray
            normal[1] = -dir[1];
            normal[2] = -dir[2];
        }
        if (normal[1] > 0.7f)
            continue; // floor-like: the ground patch has it
        const float point[3] = { start[0] + dir[0] * kWallReach * f, start[1] + dir[1] * kWallReach * f,
                                 start[2] + dir[2] * kWallReach * f };
        if (ragdoll::AddWall(a.handle, point, normal))
        {
            ++a.wallHits;
            ++g_wallSlabsTotal;
        }
    }
}

void ProbeWalls(Active& a, DWORD now)
{
    if (!g_wallsIni || !g_rayFn || !g_floorsIni || a.handle < 0 || now - a.wallTick < kWallEveryMs)
        return;
    const int turn = static_cast<int>((now / kWallEveryMs) & 1);
    a.wallTick = now;
    float bones[ragdoll::kMaxBones * 16];
    if (!ragdoll::GetPose(a.handle, bones))
        return;
    if (a.pelvisBone >= 0 && a.pelvisBone < a.count)
        ProbeWallsFrom(a, bones + a.pelvisBone * 16 + 12, turn);
    if (a.headBone >= 0 && a.headBone < a.count)
        ProbeWallsFrom(a, bones + a.headBone * 16 + 12, turn);
}

// the physics core's ground callback: ctx = a FloorCtx (17b floors) or null (terrain only)
bool GroundCb(void* ctx, float x, float z, float* h)
{
    if (!ctx)
        return Ground(x, z, h);
    FloorCtx& c = *static_cast<FloorCtx*>(ctx);
    const float gx = x / c.step;
    const float gz = z / c.step;
    const int ix = static_cast<int>(std::floor(gx));
    const int iz = static_cast<int>(std::floor(gz));
    const float fx = gx - static_cast<float>(ix);
    const float fz = gz - static_cast<float>(iz);
    const float h00 = FloorCellHeight(c, ix, iz);
    const float h10 = FloorCellHeight(c, ix + 1, iz);
    const float h01 = FloorCellHeight(c, ix, iz + 1);
    const float h11 = FloorCellHeight(c, ix + 1, iz + 1);
    *h = (h00 * (1.0f - fx) + h10 * fx) * (1.0f - fz) + (h01 * (1.0f - fx) + h11 * fx) * fz;
    return std::isfinite(*h);
}

// out = a x b (Direct3D 4x4, row vectors)
void Mul4(const float* a, const float* b, float* out)
{
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            out[r * 4 + c] = a[r * 4 + 0] * b[0 * 4 + c] + a[r * 4 + 1] * b[1 * 4 + c] + a[r * 4 + 2] * b[2 * 4 + c] +
                             a[r * 4 + 3] * b[3 * 4 + c];
}

// Inverse of a rotation + translation matrix (rows x / y / z, row 3 = position).
void RigidInverse(const float* w, float* out)
{
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
            out[r * 4 + c] = w[c * 4 + r];
        out[r * 4 + 3] = 0.0f;
    }
    for (int c = 0; c < 3; ++c)
        out[12 + c] = -(w[12] * w[c * 4 + 0] + w[13] * w[c * 4 + 1] + w[14] * w[c * 4 + 2]);
    out[15] = 1.0f;
}

bool RigidOk(const float* w)
{
    for (int r = 0; r < 3; ++r)
    {
        const float len = Length3(w[r * 4], w[r * 4 + 1], w[r * 4 + 2]);
        if (!(len > 0.98f && len < 1.02f) || std::fabs(w[r * 4 + 3]) > 0.001f)
            return false;
    }
    return std::fabs(w[15] - 1.0f) < 0.001f;
}

Pending* FindPending(uintptr_t soldier)
{
    for (Pending& p : g_pending)
        if (p.used && p.soldier == soldier)
            return &p;
    return nullptr;
}

Active* FindActive(uintptr_t soldier)
{
    for (Active& a : g_active)
        if (a.used && a.soldier == soldier)
            return &a;
    return nullptr;
}

int Busy()
{
    int n = 0;
    for (const Pending& p : g_pending)
        n += p.used ? 1 : 0;
    for (const Active& a : g_active)
        n += a.used ? 1 : 0;
    return n;
}

void TrackRagdollEnd(uintptr_t soldier, int id, const char* why); // 15i (the death tracker, below)

void EndActive(Active& a, const char* why)
{
    ragdoll::Status st;
    memset(&st, 0, sizeof(st));
    const bool have = ragdoll::GetStatus(a.handle, &st);
    if (have && RdLog())
    {
        const float dx = st.pelvis[0] - a.start[0];
        const float dz = st.pelvis[2] - a.start[2];
        Log("[ragdoll] %lu ms: ragdoll #%d (%s) ended: %s - lived %.2f s, %u frames drawn, pelvis moved %.2f m, "
            "lowest point %+.2f m above the terrain, fastest %.1f m/s, %s%s%s, terrain patch moved %d times; "
            "deepest self-overlap %.3f m, %d overlapping pairs still waiting",
            ScanMsSinceAttach(), a.id, WhoText(a.player), why, static_cast<double>(GetTickCount() - a.tick0) / 1000.0,
            static_cast<unsigned>(a.frames), static_cast<double>(std::sqrt(dx * dx + dz * dz)),
            static_cast<double>(st.lowestAbove), static_cast<double>(st.speedMax),
            st.asleep ? "asleep (settled)" : "still moving", st.frozen ? ", FROZEN: " : "",
            st.frozen ? st.frozenWhy : "", st.recentres, static_cast<double>(st.selfPenetration),
            st.overlapsWaiting);
    }
    if (g_trackOn)
        TrackRagdollEnd(a.soldier, a.id, why); // 15i
    if (a.wallRays && RdLog())
        Log("[ragdoll]   #%d walls: %d rays, %d slabs (%d kept)", a.id, a.wallRays, a.wallHits,
            ragdoll::WallCount(a.handle));
    ragdoll::Destroy(a.handle);
    a.used = false;
    ++g_rdEnded;
}

// ---------------------------------------------------------------------------
// 15i: DEATH TRACKER (log only - nothing in the game changes; INI DeathTracker)
// Every death and every throw: who it was (his character: name, team, class),
// what did it (the last damage: headshot / body / splash, the damaging
// object's class from its RTTI, an explosion push, his state), whether he got
// a ragdoll and if not why, and whether the game drew him in full detail
// (through the pose hook: the soldiers BF1 animates with their whole
// skeleton). One [deaths] line per death / throw when it is over; totals
// every 60 s.
// ---------------------------------------------------------------------------
constexpr int kTrackMax = 128;
constexpr DWORD kTrackMs = 6000;          // a death without a ragdoll is followed this long
constexpr DWORD kTrackRagdollMs = 20000;  // ... with one, until it ends (at most this)
constexpr DWORD kTrackHitMs = 1500;       // damage this recent counts as what killed him
constexpr int kTrackMaxLines = 4000;
constexpr uint32_t kCharName = 0x1C;      // the spawn scan's character layout (proven, 7a)
constexpr uint32_t kCharNameBytes = 0x80;
constexpr uint32_t kCharTeam = 0xA0;
constexpr uint32_t kCharClass = 0xAC;
constexpr int kMaxCharacters = 256;
constexpr uint32_t kCharBytes = 0xE8;     // one character (the spawn scan's kCharSize)
constexpr int kDrawnSlots = 512;

enum TrackOut
{
    kOutWaiting,  // a ragdoll candidate, waiting for him to be drawn
    kOutRagdoll,  // got a ragdoll (ragdoll id)
    kOutNone,     // no ragdoll (reason, why)
    kOutAlready,  // he already was a ragdoll (thrown) - it carries on
    kOutOff       // ragdolls switched off
};

struct Track
{
    bool used;
    char kind; // 'D' died, 'T' thrown
    int id;
    uintptr_t soldier;
    int player;
    DWORD t0;
    // who
    int ch;
    int team;
    char name[24];
    char cls[40];
    // where, how
    Vec3 pos;
    float dist;
    float gap;
    float speed;
    int32_t before;
    int32_t after;
    // what did it
    int zone;      // last damage: 2 head, 1 body, -1 splash, -99 none seen
    DWORD hitAgo;
    char damager[48];
    float push;    // an explosion push within 0.5 s (m/s), 0 none
    // outcome
    int out;
    int reason;    // SkipReason when out == kOutNone
    int ragdoll;
    char why[96];
    char endWhy[80];
    // full detail
    long drawnBefore; // ms since his last full-detail update before t0 (-1 never)
    int fullAtDeath;  // soldiers drawn in full detail in the last whole frame
    int drawnAfter;
    DWORD firstAfter;
    uint32_t lastFrame;
    DWORD deleted;
    bool reused;
    // 17c: his animation controller (the droid scan: why some bodies are never drawn after death)
    uint32_t ctl;
    DWORD ctlReuseMs;     // it drew another soldier this long after the death (0 = not seen)
    uintptr_t ctlReuseBy;
    int32_t ctlReuseState;
};

struct Drawn
{
    uintptr_t soldier;
    DWORD tick;
    uint32_t frame;
    uint32_t ctl; // 17c: the animation controller that drew him last
};

struct DamageRec
{
    uintptr_t soldier;
    DWORD tick;
    int zone;
    uint32_t vt;
};

struct NameCache
{
    uint32_t key;
    char name[48];
};

struct TrackTotals
{
    int deaths;
    int players;
    int ai;
    int ragdolls;
    int already;
    int off;
    int unknownEnd;
    int skip[kSkipCount];
    int neverDrawn;
    int headshot;
    int body;
    int splash;
    int pushed;
    int noHit;
    int throws;
    int throwRagdolls;
    int lostGiven;   // 17c: never drawn after death: his controller drew another soldier
    int lostKept;    // ... still his, not updated
    int lostOther;   // ... released / unknown
    double drawnSum;
    int drawnFrames;
    int drawnMax;
};

Track g_track[kTrackMax];
int g_trackDeaths = 0;
int g_trackThrows = 0;
int g_trackLines = 0;
Drawn g_drawn[kDrawnSlots];
int g_drawnThisFrame = 0;
int g_drawnLastFrame = 0;
DamageRec g_dmg[64];
int g_dmgNext = 0;
NameCache g_vtNames[64];
int g_vtNameCount = 0;
NameCache g_clsNames[64];
int g_clsNameCount = 0;
int g_clsProbes = 0;
TrackTotals g_tt60;
TrackTotals g_ttAll;
int g_trackSummaries = 0;

bool TrackLog()
{
    if (g_trackLines >= kTrackMaxLines)
        return false;
    if (++g_trackLines == kTrackMaxLines)
    {
        Log("[deaths] line limit reached: only the 60 s totals from now on");
        return false;
    }
    return true;
}

// a printable name (letters, digits, _ - . space), 4..len-1 long, ending in 0
bool NameAt(uintptr_t p, char* out, size_t len)
{
    if (!Readable(p, len))
        return false;
    size_t n = 0;
    for (; n < len; ++n)
    {
        const char c = static_cast<char>(ReadU8(p + n));
        if (!c)
            break;
        const bool ok = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' ||
                        c == '-' || c == '.' || c == ' ';
        if (!ok)
            return false;
    }
    if (n < 4 || n >= len)
        return false;
    memcpy(out, reinterpret_cast<const void*>(p), n);
    out[n] = 0;
    return true;
}

// the C++ class of an object's vtable, from its RTTI (".?AVOrdnanceBullet@@" -> "OrdnanceBullet")
const char* RttiName(uint32_t vt)
{
    if (!vt)
        return "";
    for (int i = 0; i < g_vtNameCount; ++i)
        if (g_vtNames[i].key == vt)
            return g_vtNames[i].name;
    NameCache* c = g_vtNameCount < 64 ? &g_vtNames[g_vtNameCount++] : &g_vtNames[63];
    c->key = vt;
    snprintf(c->name, sizeof(c->name), "vtable rva 0x%08X", static_cast<unsigned>(RvaOf(vt)));
    if (!Readable(vt - 4, 4))
        return c->name;
    const uintptr_t col = ReadU32(vt - 4);
    if (!Readable(col, 16))
        return c->name;
    const uintptr_t td = ReadU32(col + 12);
    char raw[48];
    if (!Readable(td + 8, 4) || !NameAt(td + 8, raw, sizeof(raw)))
    {
        // RTTI names start ".?AV": NameAt refuses '?', read it by hand
        if (!Readable(td + 8, sizeof(raw)))
            return c->name;
        memcpy(raw, reinterpret_cast<const void*>(td + 8), sizeof(raw));
        raw[sizeof(raw) - 1] = 0;
    }
    const char* s = raw;
    if (strncmp(s, ".?AV", 4) && strncmp(s, ".?AU", 4))
        return c->name; // not an RTTI type name: keep "vtable rva ..."
    s += 4;
    char clean[48];
    size_t n = 0;
    while (s[n] && n + 1 < sizeof(clean) && !(s[n] == '@' && s[n + 1] == '@'))
    {
        const char ch = s[n];
        const bool ok = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') ||
                        ch == '_' || ch == '@' || ch == '$' || ch == '?';
        if (!ok)
            return c->name;
        clean[n] = ch;
        ++n;
    }
    clean[n] = 0;
    if (n >= 3)
        snprintf(c->name, sizeof(c->name), "%s", clean);
    return c->name;
}

// a soldier class's ODF name: probed once per class - an inline name or a pointer to one in its
// first 0x200 bytes that looks like an ODF name (has a '_')
const char* ClassName(uint32_t cls)
{
    if (!cls)
        return "";
    for (int i = 0; i < g_clsNameCount; ++i)
        if (g_clsNames[i].key == cls)
            return g_clsNames[i].name;
    NameCache* c = g_clsNameCount < 64 ? &g_clsNames[g_clsNameCount++] : &g_clsNames[63];
    c->key = cls;
    snprintf(c->name, sizeof(c->name), "class 0x%08X", static_cast<unsigned>(cls));
    if (!Readable(cls, 0x200))
        return c->name;
    char found[40];
    int where = -1;
    bool viaPtr = false;
    for (uint32_t off = 0; off < 0x200 && where < 0; off += 4)
    {
        if (NameAt(cls + off, found, sizeof(found)) && strchr(found, '_'))
            where = static_cast<int>(off);
        else
        {
            const uintptr_t p = ReadU32(cls + off);
            if (p >= 0x10000 && (p & 3) == 0 && NameAt(p, found, sizeof(found)) && strchr(found, '_'))
            {
                where = static_cast<int>(off);
                viaPtr = true;
            }
        }
    }
    if (where >= 0)
    {
        snprintf(c->name, sizeof(c->name), "%s", found);
        if (g_clsProbes < 6 && TrackLog())
        {
            ++g_clsProbes;
            const char* type = RttiName(ReadU32(cls));
            Log("[deaths] soldier class 0x%08X: name \"%s\" found %s +0x%X%s%s%s", static_cast<unsigned>(cls), found,
                viaPtr ? "through the pointer at" : "inline at", static_cast<unsigned>(where), type[0] ? " (" : "",
                type, type[0] ? ")" : "");
        }
    }
    else if (g_clsProbes < 6 && TrackLog())
    {
        ++g_clsProbes;
        Log("[deaths] soldier class 0x%08X (vtable %s): no ODF-like name in its first 0x200 bytes",
            static_cast<unsigned>(cls), RttiName(ReadU32(cls)));
    }
    return c->name;
}

// his character (spawn scan's list): name, team, class. The character's unit pointer is
// soldier + kUnitOff (as PlayerSoldier).
struct Who
{
    int ch;
    int team;
    char name[24];
    uint32_t cls;
};

void FindWho(uintptr_t soldier, Who& w)
{
    w.ch = -1;
    w.team = -1;
    w.name[0] = 0;
    w.cls = 0;
    // 17d: the whole list at once (17c: one character at a time, two slow memory checks each - ~20 ms a death)
    uintptr_t array = 0;
    uint32_t count = 0;
    if (!SpawnScanCharacters(&array, &count) || count > static_cast<uint32_t>(kMaxCharacters) ||
        !Readable(array, count * kCharBytes))
        return;
    for (int k = 0; k < static_cast<int>(count); ++k)
    {
        const uintptr_t ch = array + static_cast<uint32_t>(k) * kCharBytes;
        const uintptr_t unit = ReadU32(ch + kCharUnit);
        if (!unit || unit != soldier + kUnitOff)
            continue;
        w.ch = k;
        w.team = static_cast<int>(static_cast<int32_t>(ReadU32(ch + kCharTeam)));
        // 15j: the name is UTF-16 (as the spawn scan reads it); 17d: inside the list, already checked
        {
            wchar_t wide[kCharNameBytes / 2 + 1];
            memcpy(wide, reinterpret_cast<const void*>(ch + kCharName), kCharNameBytes);
            wide[kCharNameBytes / 2] = 0;
            if (WideCharToMultiByte(CP_UTF8, 0, wide, -1, w.name, static_cast<int>(sizeof(w.name)), nullptr,
                                    nullptr) <= 0)
                w.name[0] = 0;
            w.name[sizeof(w.name) - 1] = 0;
        }
        w.cls = ReadU32(ch + kCharClass);
        return;
    }
}

Track* OpenTrack(uintptr_t soldier, char kind)
{
    Track* best = nullptr;
    for (Track& t : g_track)
        if (t.used && t.soldier == soldier && (!kind || t.kind == kind) && (!best || t.t0 - best->t0 < 0x80000000u))
            best = &t;
    return best;
}

Track* NewTrack()
{
    Track* oldest = nullptr;
    for (Track& t : g_track)
    {
        if (!t.used)
        {
            memset(&t, 0, sizeof(t));
            return &t;
        }
        if (!oldest || t.t0 - oldest->t0 >= 0x80000000u)
            oldest = &t;
    }
    // full: the oldest goes (its line is lost - it says so in the totals as "unknown end")
    ++g_tt60.unknownEnd;
    ++g_ttAll.unknownEnd;
    memset(oldest, 0, sizeof(*oldest));
    return oldest;
}

long DrawnAgo(uintptr_t soldier, DWORD now)
{
    const uint32_t mask = kDrawnSlots - 1;
    uint32_t i = static_cast<uint32_t>(soldier >> 4) & mask;
    for (uint32_t n = 0; n < 16; ++n, i = (i + 1) & mask)
    {
        if (g_drawn[i].soldier == soldier)
            return static_cast<long>(now - g_drawn[i].tick);
        if (!g_drawn[i].soldier)
            break;
    }
    return -1;
}

// 17c: the controller that drew this soldier last (0 if never seen)
uint32_t DrawnCtl(uintptr_t soldier)
{
    const uint32_t mask = kDrawnSlots - 1;
    uint32_t i = static_cast<uint32_t>(soldier >> 4) & mask;
    for (uint32_t n = 0; n < 16; ++n, i = (i + 1) & mask)
    {
        if (g_drawn[i].soldier == soldier)
            return g_drawn[i].ctl;
        if (!g_drawn[i].soldier)
            break;
    }
    return 0;
}

const char* SkipShort(int r)
{
    switch (r)
    {
    case kSkipAlone: return "no local player alive to see it";
    case kSkipRange: return "out of range";
    case kSkipTerrain: return "on an object or in the air";
    case kSkipLimit: return "MaxBodies";
    case kSkipLate: return "not drawn in time";
    case kSkipSkeleton: return "skeleton";
    case kSkipFailed: return "build failed";
    case kSkipThrown: return "died while thrown";
    case kSkipSeated: return "in a vehicle or turret seat";
    default: return "?";
    }
}

void FillWho(Track& t, const Who& w)
{
    t.ch = w.ch;
    t.team = w.team;
    snprintf(t.name, sizeof(t.name), "%s", w.name);
    snprintf(t.cls, sizeof(t.cls), "%s", w.cls ? ClassName(w.cls) : "");
}

void WhoLine(const Track& t, char* out, size_t len)
{
    char who[96];
    if (t.player >= 0)
        snprintf(who, sizeof(who), "player %d", t.player + 1);
    else
        snprintf(who, sizeof(who), "AI");
    if (t.ch >= 0)
        snprintf(out, len, "%s \"%s\" (team %d%s%s)", who, t.name, t.team, t.cls[0] ? ", " : "", t.cls);
    else
        snprintf(out, len, "%s 0x%08X (no character found)", who, static_cast<unsigned>(t.soldier));
}

struct Track;
void SnapDeath(const Track& t); // 17d: the droid scan (below)

// 15i: a death (from the death hook, after the game's own; w was read before it)
void TrackDeath(uintptr_t soldier, int p, const Vec3& pos, const Vec3& vel, float dist, int32_t before,
                int32_t after, const Who& w0)
{
    const DWORD now = GetTickCount();
    Track& t = *NewTrack();
    t.used = true;
    t.kind = 'D';
    t.id = ++g_trackDeaths;
    t.soldier = soldier;
    t.player = p;
    t.t0 = now;
    Who w = w0;
    if (w.ch < 0)
        FindWho(soldier, w);
    FillWho(t, w);
    t.pos = pos;
    t.dist = dist;
    float h = 0.0f;
    t.gap = Ground(pos.x, pos.z, &h) ? pos.y - h : 999.0f;
    t.speed = Length3(vel.x, vel.y, vel.z);
    t.before = before;
    t.after = after;
    // what did it
    t.zone = -99;
    const DamageRec* hit = nullptr;
    for (const DamageRec& d : g_dmg)
        if (d.soldier == soldier && now - d.tick <= kTrackHitMs && (!hit || d.tick - hit->tick < 0x80000000u))
            hit = &d;
    if (hit)
    {
        t.zone = hit->zone;
        t.hitAgo = now - hit->tick;
        const char* dn = InExe(hit->vt, 4) ? RttiName(hit->vt) : "";
        snprintf(t.damager, sizeof(t.damager), "%s", strncmp(dn, "vtable rva", 10) ? dn : ""); // 15j
    }
    const PushRec* push = RecentPush(soldier, now);
    t.push = push ? push->strength : 0.0f;
    // full detail
    t.drawnBefore = DrawnAgo(soldier, now);
    t.fullAtDeath = g_drawnLastFrame;
    t.ctl = DrawnCtl(soldier); // 17c
    // outcome so far (RagdollDeath decides the rest)
    t.out = kOutWaiting;
    const Active* a = FindActive(soldier);
    if (a)
    {
        t.out = kOutAlready;
        t.ragdoll = a->id;
    }
    else if (!g_ragdollIni || !g_rdLiveOn)
    {
        t.out = kOutOff;
    }
    if ((p >= 0 || (dist >= 0.0f && dist <= 60.0f)) && t.drawnBefore >= 0 && t.drawnBefore < 1000)
        SnapDeath(t); // 17d: the droid scan
    TrackTotals* tt[2] = { &g_tt60, &g_ttAll };
    for (TrackTotals* x : tt)
    {
        ++x->deaths;
        if (p >= 0)
            ++x->players;
        else
            ++x->ai;
        if (t.zone == 2)
            ++x->headshot;
        else if (t.zone == 1)
            ++x->body;
        else if (t.zone == -1)
            ++x->splash;
        else if (t.zone == -99)
            ++x->noHit;
        if (t.push > 0.0f)
            ++x->pushed;
    }
}

// 15i: a throw (a push that put a living soldier into state 5)
void TrackThrow(uintptr_t soldier, int p, const Vec3& pos, float strength, float dist)
{
    if (OpenTrack(soldier, 'T'))
        return; // the same throw pushed again
    const DWORD now = GetTickCount();
    Track& t = *NewTrack();
    t.used = true;
    t.kind = 'T';
    t.id = ++g_trackThrows;
    t.soldier = soldier;
    t.player = p;
    t.t0 = now;
    Who w;
    FindWho(soldier, w);
    FillWho(t, w);
    t.pos = pos;
    t.dist = dist;
    float h = 0.0f;
    t.gap = Ground(pos.x, pos.z, &h) ? pos.y - h : 999.0f;
    t.push = strength;
    t.zone = -99;
    t.drawnBefore = DrawnAgo(soldier, now);
    t.fullAtDeath = g_drawnLastFrame;
    t.out = g_rdLiveOn && g_ragdollIni ? kOutWaiting : kOutOff;
    ++g_tt60.throws;
    ++g_ttAll.throws;
}

void TrackOutcome(uintptr_t soldier, int reason, const char* text)
{
    Track* t = OpenTrack(soldier, 0);
    if (!t || t->out != kOutWaiting)
        return;
    t->out = kOutNone;
    t->reason = reason;
    snprintf(t->why, sizeof(t->why), "%s", text);
}

void TrackBuilt(uintptr_t soldier, int id)
{
    Track* t = OpenTrack(soldier, 0);
    if (!t || t->out != kOutWaiting)
        return;
    t->out = kOutRagdoll;
    t->ragdoll = id;
    if (t->kind == 'T')
    {
        ++g_tt60.throwRagdolls;
        ++g_ttAll.throwRagdolls;
    }
}

void CloseTrack(Track& t, DWORD now, const char* end)
{
    if (end && !t.endWhy[0])
        snprintf(t.endWhy, sizeof(t.endWhy), "%s", end);
    if (t.kind == 'D')
    {
        TrackTotals* tt[2] = { &g_tt60, &g_ttAll };
        for (TrackTotals* x : tt)
        {
            if (t.out == kOutRagdoll)
                ++x->ragdolls;
            else if (t.out == kOutAlready)
                ++x->already;
            else if (t.out == kOutOff)
                ++x->off;
            else if (t.out == kOutNone && t.reason >= 0 && t.reason < kSkipCount)
                ++x->skip[t.reason];
            else
                ++x->unknownEnd;
            if (t.drawnBefore < 0 && !t.drawnAfter)
                ++x->neverDrawn;
            if (!t.drawnAfter && t.drawnBefore >= 0 && t.drawnBefore < 1000)
            {
                // 17c: drawn just before death, never after
                if (t.ctlReuseMs)
                    ++x->lostGiven;
                else if (t.ctl && Readable(t.ctl + kCtlOwner, 4) && ReadU32(t.ctl + kCtlOwner) == t.soldier)
                    ++x->lostKept;
                else
                    ++x->lostOther;
            }
        }
    }
    if (TrackLog())
    {
        char who[160];
        WhoLine(t, who, sizeof(who));
        char what[160];
        if (t.kind == 'T')
            snprintf(what, sizeof(what), "thrown %.1f m/s by an explosion", static_cast<double>(t.push));
        else
        {
            const char* how = t.zone == 2 ? "a HEADSHOT" : t.zone == 1 ? "a body shot" : t.zone == -1 ? "splash damage"
                              : t.zone == -99 ? nullptr : "a hit";
            if (how)
                snprintf(what, sizeof(what), "killed by %s (%s%s%.2f s before)%s", how, t.damager,
                         t.damager[0] ? ", " : "", static_cast<double>(t.hitAgo) / 1000.0,
                         t.push > 0.0f ? ", pushed by an explosion" : "");
            else
                snprintf(what, sizeof(what), "died with no damage seen in the last %.1f s%s",
                         static_cast<double>(kTrackHitMs) / 1000.0,
                         t.push > 0.0f ? " (pushed by an explosion)" : " (a fall, a vehicle, the map edge, or a kill "
                                                                       "the damage hook does not see)");
        }
        char where[96];
        if (t.player >= 0 && t.kind == 'D')
            snprintf(where, sizeof(where), "his own body (his camera watches it)");
        else if (t.dist >= 0.0f)
            snprintf(where, sizeof(where), "%.0f m from the nearest player", static_cast<double>(t.dist));
        else
            snprintf(where, sizeof(where), "no other local player alive");
        char ground[40];
        if (t.gap > 900.0f)
            snprintf(ground, sizeof(ground), "no terrain height there");
        else
            snprintf(ground, sizeof(ground), "%+.2f m from the terrain", static_cast<double>(t.gap));
        char outcome[200];
        switch (t.out)
        {
        case kOutRagdoll:
            snprintf(outcome, sizeof(outcome), "RAGDOLL #%d (%s)", t.ragdoll, t.endWhy[0] ? t.endWhy : "running");
            break;
        case kOutAlready:
            snprintf(outcome, sizeof(outcome), "already a ragdoll from a throw (#%d) - it carries on", t.ragdoll);
            break;
        case kOutOff:
            snprintf(outcome, sizeof(outcome), "no ragdoll - ragdolls are off");
            break;
        case kOutNone:
            snprintf(outcome, sizeof(outcome), "NO RAGDOLL: %s", t.why);
            break;
        default:
            snprintf(outcome, sizeof(outcome), "NO RAGDOLL: still waiting to be drawn when it ended (%s)",
                     t.endWhy[0] ? t.endWhy : "?");
            break;
        }
        char full[200];
        char beforeText[48];
        char afterText[48];
        if (t.drawnBefore < 0)
            snprintf(beforeText, sizeof(beforeText), "never drawn before");
        else if (t.drawnBefore < 1000)
            snprintf(beforeText, sizeof(beforeText), "drawn %.2f s before", static_cast<double>(t.drawnBefore) / 1000.0);
        else
            snprintf(beforeText, sizeof(beforeText), "last drawn %.1f s before",
                     static_cast<double>(t.drawnBefore) / 1000.0);
        if (t.drawnAfter)
            snprintf(afterText, sizeof(afterText), "%d frames after (from +%.2f s)", t.drawnAfter,
                     static_cast<double>(t.firstAfter - t.t0) / 1000.0);
        else
            snprintf(afterText, sizeof(afterText), "none after");
        snprintf(full, sizeof(full), "full detail: %s, %s; soldiers drawn in full detail that frame: %d", beforeText,
                 afterText, t.fullAtDeath);
        // 17c: never drawn after death - what became of his animation controller
        char lost[160] = "";
        if (t.kind == 'D' && !t.drawnAfter && t.drawnBefore >= 0 && t.drawnBefore < 1000)
        {
            if (!t.ctl)
                snprintf(lost, sizeof(lost), " | his controller: not known");
            else if (t.ctlReuseMs)
                snprintf(lost, sizeof(lost), " | his controller 0x%08X drew another soldier (0x%08X, state %d %s) from "
                         "+%.2f s: the game gave it away",
                         static_cast<unsigned>(t.ctl), static_cast<unsigned>(t.ctlReuseBy),
                         static_cast<int>(t.ctlReuseState), StateName(t.ctlReuseState),
                         static_cast<double>(t.ctlReuseMs) / 1000.0);
            else
            {
                const uint32_t owner = Readable(t.ctl + kCtlOwner, 4) ? ReadU32(t.ctl + kCtlOwner) : 0xFFFFFFFFu;
                snprintf(lost, sizeof(lost), " | his controller 0x%08X: %s (owner 0x%08X)", static_cast<unsigned>(t.ctl),
                         owner == t.soldier ? "still his, but not updated" : owner == 0 ? "released" : "now another's",
                         static_cast<unsigned>(owner));
            }
        }
        if (t.kind == 'D')
            Log("[deaths] D%d %.1f s: %s, %s %d %s -> %d %s, %s, %s, %.1f m/s; %s -> %s | %s%s%s%s",
                t.id, static_cast<double>(ScanMsSinceAttach() - (now - t.t0)) / 1000.0, who,
                "state", static_cast<int>(t.before), StateName(t.before), static_cast<int>(t.after), StateName(t.after),
                where, ground, static_cast<double>(t.speed), what, outcome, full,
                t.deleted ? " | body deleted" : "", t.reused ? " | soldier reused (respawned)" : "", lost);
        else
            Log("[deaths] T%d %.1f s: %s %s, %s, %s -> %s | %s", t.id,
                static_cast<double>(ScanMsSinceAttach() - (now - t.t0)) / 1000.0, who, what, where, ground, outcome,
                full);
    }
    t.used = false;
}

void TrackRagdollEnd(uintptr_t soldier, int id, const char* why)
{
    for (Track& t : g_track)
        if (t.used && t.soldier == soldier && t.ragdoll == id &&
            (t.out == kOutRagdoll || t.out == kOutAlready))
        {
            char end[80];
            snprintf(end, sizeof(end), "ended at +%.1f s: %s", static_cast<double>(GetTickCount() - t.t0) / 1000.0,
                     why);
            CloseTrack(t, GetTickCount(), end);
        }
}

// every pose update (full detail): per-frame count, and his open tracks
void TrackDrawn(uintptr_t soldier, uint32_t frame, int32_t state, uint32_t ctl)
{
    const DWORD now = GetTickCount();
    const uint32_t mask = kDrawnSlots - 1;
    uint32_t i = static_cast<uint32_t>(soldier >> 4) & mask;
    Drawn* slot = nullptr;
    for (uint32_t n = 0; n < 16; ++n, i = (i + 1) & mask)
    {
        if (g_drawn[i].soldier == soldier || !g_drawn[i].soldier || now - g_drawn[i].tick > 30000)
        {
            slot = &g_drawn[i];
            break;
        }
    }
    if (slot)
    {
        if (slot->soldier != soldier)
        {
            slot->soldier = soldier;
            slot->frame = frame - 1;
        }
        slot->tick = now;
        slot->ctl = ctl; // 17c
        if (slot->frame != frame)
        {
            slot->frame = frame;
            ++g_drawnThisFrame;
        }
    }
    for (Track& t : g_track)
    {
        // 17c: a dead soldier's controller drawing somebody else (given away)
        if (t.used && t.kind == 'D' && t.ctl == ctl && t.soldier != soldier && !t.ctlReuseMs && !t.drawnAfter)
        {
            t.ctlReuseMs = (now - t.t0) ? now - t.t0 : 1;
            t.ctlReuseBy = soldier;
            t.ctlReuseState = state;
        }
        if (!t.used || t.soldier != soldier || t.lastFrame == frame || now == t.t0)
            continue;
        t.lastFrame = frame;
        if (!t.drawnAfter)
            t.firstAfter = now;
        ++t.drawnAfter;
        if (t.kind == 'D' && state != kStateDead && now - t.t0 > 200)
            t.reused = true;
    }
}

void TrackNewFrame()
{
    g_drawnLastFrame = g_drawnThisFrame;
    g_drawnThisFrame = 0;
    TrackTotals* tt[2] = { &g_tt60, &g_ttAll };
    for (TrackTotals* x : tt)
    {
        x->drawnSum += g_drawnLastFrame;
        ++x->drawnFrames;
        if (g_drawnLastFrame > x->drawnMax)
            x->drawnMax = g_drawnLastFrame;
    }
}

// closes what is over: no ragdoll after kTrackMs, deleted / reused bodies, ragdolls gone
void SnapAge(DWORD now); // 17d (below)

void TrackAge(DWORD now)
{
    SnapAge(now); // 17d
    for (Track& t : g_track)
    {
        if (!t.used)
            continue;
        const DWORD age = now - t.t0;
        const bool ragdoll = t.out == kOutRagdoll || t.out == kOutAlready;
        if (ragdoll && t.ragdoll && !FindActive(t.soldier) && age > 500)
            CloseTrack(t, now, "ragdoll gone");
        else if (!ragdoll && t.out != kOutWaiting && (age > kTrackMs || t.deleted || t.reused))
            CloseTrack(t, now, nullptr);
        else if (t.out == kOutWaiting && (age > kTrackMs || t.deleted))
            CloseTrack(t, now, t.deleted ? "body deleted" : "6 s passed");
        else if (age > kTrackRagdollMs)
            CloseTrack(t, now, "20 s passed");
        else if (t.kind == 'T' && t.out != kOutWaiting && !ragdoll && age > 3000)
            CloseTrack(t, now, nullptr);
    }
}

void TrackDeleted(uintptr_t soldier)
{
    for (Track& t : g_track)
        if (t.used && t.soldier == soldier && !t.deleted)
            t.deleted = GetTickCount();
}


// ---------------------------------------------------------------------------
// 17d: DROID SCAN, part 2 (log only, in the death tracker). 17c: 72 bodies
// were drawn just before death and never after, all keeping their controller
// - nearly all droids ("UNIT" names, either team, the player too when he played
// one): the soldier draw function stops before the animation update for them.
// For every death near a player that was being drawn, the soldier's object
// (0xB50 bytes) is copied right after the game's death code and again 0.4 s
// later; per dword, how often it changed and how often it is non-zero after,
// for bodies still drawn vs not. Every 60 s: the dwords that differ most
// between the two - the switch that sends droids off the normal drawing path.
// ---------------------------------------------------------------------------
constexpr int kSnaps = 24;
constexpr uint32_t kSnapDwords = kSoldierBytes / 4;
constexpr DWORD kSnapAfterMs = 400;

struct DeathSnap
{
    bool used;
    uintptr_t soldier;
    DWORD t0;
    int trackId;
    bool havePre;
    uint32_t pre[kSnapDwords]; // 17e: before the game's death code
    uint32_t a[kSnapDwords];   // right after it
};
uint32_t g_preSnap[kSnapDwords]; // 17e: the death hook's copy before the game's death code
uintptr_t g_preSnapSoldier = 0;
uint16_t g_snapDeathCode[2][kSnapDwords]; // 17e: dword changed by the game's death code
DeathSnap g_snaps[kSnaps];
uint32_t g_snapN[2];                    // samples: 0 drawn after death, 1 not drawn after death
uint16_t g_snapChanged[2][kSnapDwords]; // dword changed between death and +0.4 s
uint16_t g_snapNonzero[2][kSnapDwords]; // dword non-zero at +0.4 s
uint32_t g_snapLast[2][kSnapDwords];    // its last value at +0.4 s

void SnapDeath(const Track& t)
{
    if (!Readable(t.soldier, kSoldierBytes))
        return;
    for (DeathSnap& s : g_snaps)
    {
        if (s.used)
            continue;
        s.used = true;
        s.soldier = t.soldier;
        s.t0 = t.t0;
        s.trackId = t.id;
        memcpy(s.a, reinterpret_cast<const void*>(t.soldier), kSoldierBytes);
        s.havePre = g_preSnapSoldier == t.soldier;
        if (s.havePre)
            memcpy(s.pre, g_preSnap, sizeof(s.pre));
        g_preSnapSoldier = 0;
        return;
    }
}

void SnapAge(DWORD now)
{
    for (DeathSnap& s : g_snaps)
    {
        if (!s.used || now - s.t0 < kSnapAfterMs)
            continue;
        s.used = false;
        const Track* t = nullptr;
        for (const Track& x : g_track)
            if (x.used && x.kind == 'D' && x.id == s.trackId && x.soldier == s.soldier)
                t = &x;
        if (!t || !Readable(s.soldier, kSoldierBytes))
            continue;
        const int g = t->drawnAfter ? 0 : 1;
        if (g_snapN[g] >= 60000)
            continue;
        ++g_snapN[g];
        const uint32_t* b = reinterpret_cast<const uint32_t*>(s.soldier);
        for (uint32_t i = 0; i < kSnapDwords; ++i)
        {
            if (b[i] != s.a[i])
                ++g_snapChanged[g][i];
            if (s.havePre && s.pre[i] != s.a[i])
                ++g_snapDeathCode[g][i];
            if (b[i])
                ++g_snapNonzero[g][i];
            g_snapLast[g][i] = b[i];
        }
    }
}

void SnapSummary()
{
    if (g_snapN[0] < 5 || g_snapN[1] < 5)
    {
        if (g_snapN[0] + g_snapN[1])
            Log("[deaths] droid scan: %u drawn after death, %u not - need 5 of each to compare",
                static_cast<unsigned>(g_snapN[0]), static_cast<unsigned>(g_snapN[1]));
        return;
    }
    struct Hit
    {
        uint32_t off;
        float score;
    };
    Hit top[12] = {};
    for (uint32_t i = 0; i < kSnapDwords; ++i)
    {
        const float c0 = static_cast<float>(g_snapChanged[0][i]) / static_cast<float>(g_snapN[0]);
        const float c1 = static_cast<float>(g_snapChanged[1][i]) / static_cast<float>(g_snapN[1]);
        const float z0 = static_cast<float>(g_snapNonzero[0][i]) / static_cast<float>(g_snapN[0]);
        const float z1 = static_cast<float>(g_snapNonzero[1][i]) / static_cast<float>(g_snapN[1]);
        const float d0 = static_cast<float>(g_snapDeathCode[0][i]) / static_cast<float>(g_snapN[0]);
        const float d1 = static_cast<float>(g_snapDeathCode[1][i]) / static_cast<float>(g_snapN[1]);
        float score = std::fabs(c1 - c0) > std::fabs(z1 - z0) ? std::fabs(c1 - c0) : std::fabs(z1 - z0);
        if (std::fabs(d1 - d0) > score)
            score = std::fabs(d1 - d0); // 17e: what the game's death code changes for one kind only
        if (score < 0.6f)
            continue;
        for (int k = 0; k < 12; ++k)
        {
            if (score > top[k].score)
            {
                for (int m = 11; m > k; --m)
                    top[m] = top[m - 1];
                top[k].off = i * 4;
                top[k].score = score;
                break;
            }
        }
    }
    Log("[deaths] droid scan: %u bodies drawn after death, %u not (mostly droids) - the soldier's dwords that differ "
        "most between them (changed death -> +0.4 s / non-zero at +0.4 s, drawn vs not; last values):",
        static_cast<unsigned>(g_snapN[0]), static_cast<unsigned>(g_snapN[1]));
    int shown = 0;
    for (const Hit& h : top)
    {
        if (h.score <= 0.0f)
            break;
        const uint32_t i = h.off / 4;
        Log("[deaths]   +0x%03X: set by the death code %3.0f%% / %3.0f%%, changed after %3.0f%% / %3.0f%%, non-zero "
            "%3.0f%% / %3.0f%%; last 0x%08X / 0x%08X",
            static_cast<unsigned>(h.off), 100.0 * g_snapDeathCode[0][i] / g_snapN[0],
            100.0 * g_snapDeathCode[1][i] / g_snapN[1], 100.0 * g_snapChanged[0][i] / g_snapN[0],
            100.0 * g_snapChanged[1][i] / g_snapN[1], 100.0 * g_snapNonzero[0][i] / g_snapN[0],
            100.0 * g_snapNonzero[1][i] / g_snapN[1], static_cast<unsigned>(g_snapLast[0][i]),
            static_cast<unsigned>(g_snapLast[1][i]));
        ++shown;
    }
    if (!shown)
        Log("[deaths]   no dword differs in 60 %% or more of the cases");
}

void TrackTotalsLine(const char* label, const TrackTotals& x)
{
    char skips[400];
    size_t n = 0;
    skips[0] = 0;
    for (int r = 0; r < kSkipCount; ++r)
        if (x.skip[r])
            n += static_cast<size_t>(snprintf(skips + n, sizeof(skips) - n, "%s%d %s", n ? ", " : "", x.skip[r],
                                              SkipShort(r)));
    Log("[deaths] %s: %d deaths (%d players, %d AI): %d ragdolls, %d already ragdolls from a throw, %d off, %d with no "
        "ending seen; no ragdoll: %s | what killed them: %d headshots, %d body shots, %d splash, %d no damage seen, "
        "%d pushed by an explosion | %d never drawn in full detail; drawn just before death but never after: %d (its "
        "controller given to another soldier %d, kept but not updated %d, released / other %d) | throws: %d, %d "
        "ragdolls | soldiers in full detail per frame: %.1f average, %d most",
        label, x.deaths, x.players, x.ai, x.ragdolls, x.already, x.off, x.unknownEnd, n ? skips : "none", x.headshot,
        x.body, x.splash, x.noHit, x.pushed, x.neverDrawn, x.lostGiven + x.lostKept + x.lostOther, x.lostGiven,
        x.lostKept, x.lostOther, x.throws, x.throwRagdolls, x.drawnFrames ? x.drawnSum / x.drawnFrames : 0.0,
        x.drawnMax);
}

// every 10 s summary: totals every 60 s, the session's every 5 min
void TrackSummary()
{
    ++g_trackSummaries;
    if (g_trackSummaries % 6 == 0)
    {
        SnapSummary(); // 17d: the droid scan
        TrackTotalsLine("last 60 s", g_tt60);
        memset(&g_tt60, 0, sizeof(g_tt60));
    }
    if (g_trackSummaries % 30 == 0)
        TrackTotalsLine("this session", g_ttAll);
}

// ---- 22c: Bullet's memory -------------------------------------------------------------------------------------------------
constexpr size_t kRdReserveBytes = 2u * 1024u * 1024u; // given back the first time a Bullet malloc fails
constexpr size_t kRdProbeBytes = 1024u * 1024u;        // a ragdoll is built only while Windows can still give 1 MB at once
constexpr DWORD kRdMemPauseMs = 60000;                 // after a failed Bullet allocation: no new ragdolls for this long
void* volatile g_rdReserve = nullptr;
volatile LONG g_rdMemFails = 0;
volatile LONG g_rdMemLowTick = 0;                      // GetTickCount() | 1 of the last failure (0: none)
volatile LONG g_rdMemBytes = 0;                        // asked through Bullet's allocator so far
bool g_rdMemOurs = false;

void* RdAlloc(size_t size)
{
    void* p = malloc(size);
    if (!p)
    {
        const LONG n = InterlockedIncrement(&g_rdMemFails);
        void* r = InterlockedExchangePointer(&g_rdReserve, nullptr);
        if (r)
        {
            free(r);
            p = malloc(size);
        }
        InterlockedExchange(&g_rdMemLowTick, static_cast<LONG>(GetTickCount() | 1u));
        if (n <= 8)
        {
            char va[120];
            Log("[ragdoll] %lu ms: OUT OF MEMORY - Bullet asked for %lu bytes and Windows had none%s; %s; no new ragdolls for %lu s",
                ScanMsSinceAttach(), static_cast<unsigned long>(size),
                p ? " (it got them once BF1GameExt gave back its 2 MB reserve)"
                  : (r ? " - not even after BF1GameExt gave back its 2 MB reserve: Bullet writes through 0 next, the game crashes"
                       : " - no reserve left: Bullet writes through 0 next, the game crashes"),
                MemLogVaShort(va, sizeof(va)), static_cast<unsigned long>(kRdMemPauseMs / 1000));
        }
    }
    if (p)
    {
        memset(p, 0, size); // as Bullet's own allocator does
        InterlockedExchangeAdd(&g_rdMemBytes, static_cast<LONG>(size));
    }
    return p;
}

void RdFree(void* p)
{
    free(p);
}

// a ragdoll may be built: no failed Bullet allocation in the last 60 s, and Windows can still give 1 MB in one piece
bool RdMemRoom(char* why, size_t whyLen)
{
    if (!g_rdMemOurs)
        return true;
    const DWORD low = static_cast<DWORD>(g_rdMemLowTick);
    if (low && GetTickCount() - low < kRdMemPauseMs)
    {
        snprintf(why, whyLen, "out of memory: a Bullet allocation failed %lu s ago", static_cast<unsigned long>((GetTickCount() - low) / 1000));
        return false;
    }
    void* t = malloc(kRdProbeBytes);
    if (!t)
    {
        char va[120];
        snprintf(why, whyLen, "out of memory: Windows could not give 1 MB in one piece (%s)", MemLogVaShort(va, sizeof(va)));
        return false;
    }
    free(t);
    if (!g_rdReserve)
    {
        void* r = malloc(kRdReserveBytes); // memory is back: the reserve again
        if (r && InterlockedCompareExchangePointer(&g_rdReserve, r, nullptr) != nullptr)
            free(r);
    }
    return true;
}

void Skip(SkipReason r, uintptr_t soldier, int p, const char* text)
{
    ++g_rdSkips[r];
    if (g_trackOn)
        TrackOutcome(soldier, r, text); // 15i
    if (RdLog())
        Log("[ragdoll] %lu ms: %s soldier 0x%08X: no ragdoll - %s", ScanMsSinceAttach(), WhoText(p),
            static_cast<unsigned>(soldier), text);
}

// Death hook (under the lock): is this death a ragdoll candidate?
void RagdollDeath(uintptr_t soldier, int p, const Vec3& pos, const Vec3& vel, float dist, int32_t after)
{
    if (!g_ragdollIni || !g_rdLiveOn || FindPending(soldier) || FindActive(soldier)) // 15g: live Enabled
        return;
    char text[96];
    if (after != kStateDead)
    {
        snprintf(text, sizeof(text), "died while in state %d %s (the game did not set dead yet: thrown?)",
                 static_cast<int>(after), StateName(after));
        Skip(kSkipThrown, soldier, p, text);
        return;
    }
    float h = 0.0f;
    if (dist < 0.0f)
    {
        Skip(kSkipAlone, soldier, p, "no other local player to see the body");
        return;
    }
    if (dist > g_rdRange)
    {
        snprintf(text, sizeof(text), "%.0f m from the nearest player (Range %.0f)", static_cast<double>(dist),
                 static_cast<double>(g_rdRange));
        Skip(kSkipRange, soldier, p, text);
        return;
    }
    if (!FloorsOn() && !Ground(pos.x, pos.z, &h))
    {
        Skip(kSkipTerrain, soldier, p, "no terrain height here");
        return;
    }
    if (!FloorsOn() && std::fabs(pos.y - h) > kOnTerrain)
    {
        snprintf(text, sizeof(text), "%+.2f m from the terrain (on an object or in the air)",
                 static_cast<double>(pos.y - h));
        Skip(kSkipTerrain, soldier, p, text);
        return;
    }
    // 17b: with floors the floor check runs at the build (rays only from the pose update)
    if (Busy() >= g_rdMax)
    {
        snprintf(text, sizeof(text), "%d ragdolls already running or waiting (MaxBodies)", g_rdMax);
        Skip(kSkipLimit, soldier, p, text);
        return;
    }
    for (Pending& c : g_pending)
    {
        if (c.used)
            continue;
        c.used = true;
        c.soldier = soldier;
        c.tick = GetTickCount();
        c.player = p;
        c.kind = kKindDead;
        c.spin[0] = c.spin[1] = c.spin[2] = 0.0f;
        memset(c.v1, 0, sizeof(c.v1)); // 15g: thrown-only fields
        memset(c.pushAxis, 0, sizeof(c.pushAxis));
        c.pushStrength = 0.0f;
        c.pos[0] = pos.x; // 17b
        c.pos[1] = pos.y;
        c.pos[2] = pos.z;
        float v[3] = { vel.x, vel.y, vel.z };
        const float speed = Length3(v[0], v[1], v[2]);
        const float scale = speed > kMaxStartSpeed ? kMaxStartSpeed / speed : 1.0f;
        for (int k = 0; k < 3; ++k)
            c.vel[k] = std::isfinite(v[k]) ? v[k] * scale : 0.0f;
        if (RdLog())
            Log("[ragdoll] %lu ms: %s soldier 0x%08X died %.1f m from a player, %s, speed %.1f m/s: ragdoll candidate",
                ScanMsSinceAttach(), WhoText(p), static_cast<unsigned>(soldier), static_cast<double>(dist),
                FloorsOn() ? "the floor checked when he is drawn" : "on the terrain", static_cast<double>(speed));
        return;
    }
}

void LabRecord(const Active& a, const Pending& pd, const SkelInfo& s, const float* model, const float* w,
               const PushRec* pushed, const HitRec* hit); // 15g (below)
bool LabAnimSlot();                                   // 15g (below)
void LabAnim(const Active& a, float t);               // 15g (below)

// 15g: whether the game will draw this soldier's weapon (the render's own
// checks), for the log.
void WeaponText(uintptr_t soldier, char* out, size_t len)
{
    if (!Readable(soldier + kWeaponModeFlag, 4) || !Readable(soldier + kWeapons, kWeaponMode + 4 - kWeapons))
    {
        snprintf(out, len, "not readable");
        return;
    }
    const int32_t slot = ReadI32(soldier + kWeaponSlot);
    const uint32_t weapon = slot >= 0 && slot < 8 ? ReadU32(soldier + kWeapons + static_cast<uint32_t>(slot) * 4) : 0;
    const bool skip = ReadI32(soldier + kWeaponMode) == 12 && ReadI32(soldier + kWeaponModeFlag) == 0;
    snprintf(out, len, "slot %d, weapon 0x%08X: %s", static_cast<int>(slot), static_cast<unsigned>(weapon),
             (weapon && !skip) ? "the game draws it at hp_weapons" : "not drawn");
}

Active* BuildRagdoll(Pending& pd, uintptr_t soldier, uintptr_t ctl, int32_t state, SkelInfo* s)
{
    const int p = pd.player;
    pd.used = false;
    char text[96];
    const bool thrown = pd.kind == kKindThrown;
    if (thrown ? (state != 5 && state != 8) : state != kStateDead)
    {
        snprintf(text, sizeof(text), "state %d %s at his first draw after %s", static_cast<int>(state),
                 StateName(state), thrown ? "the push" : "death");
        Skip(kSkipLate, soldier, p, text);
        return nullptr;
    }
    const float t = thrown ? static_cast<float>(GetTickCount() - pd.tick) / 1000.0f : ReadF32(soldier + kStateTime);
    if (t > kBuildWindow)
    {
        snprintf(text, sizeof(text), "first drawn %.2f s after %s (left to the game's animation)",
                 static_cast<double>(t), thrown ? "the push" : "death");
        Skip(kSkipLate, soldier, p, text);
        return nullptr;
    }
    if (!s || !s->rsOk)
    {
        snprintf(text, sizeof(text), "skeleton: %s", s ? s->missing : g_skelWhy);
        Skip(kSkipSkeleton, soldier, p, text);
        return nullptr;
    }
    float w[16];
    ReadMatrix(soldier + kSoldierMatrix, w);
    if (!RigidOk(w))
    {
        Skip(kSkipFailed, soldier, p, "the soldier's matrix is not a plain rotation + position");
        return nullptr;
    }
    Active* a = nullptr;
    for (Active& c : g_active)
    {
        if (!c.used)
        {
            a = &c;
            break;
        }
    }
    if (!a)
    {
        Skip(kSkipLimit, soldier, p, "no free ragdoll slot");
        return nullptr;
    }
    // 17b: the floor where he died / was pushed - the game's ray test (here, in the pose update)
    FloorCtx* floor = nullptr;
    float floorH = 0.0f;
    const char* floorWhat = "terrain";
    if (FloorsOn())
    {
        floor = &g_floorCtx[a - g_active];
        memset(floor, 0, sizeof(*floor));
        floor->step = g_floorStep;
        floor->refY = pd.pos[1] + 0.5f;
        // 17c: no terrain where he died (Bespin Platforms): the floor rays skip the terrain call (the stutter)
        float th = 0.0f;
        floor->noTerrain = !Ground(pd.pos[0], pd.pos[2], &th);
        if (!FloorAt(pd.pos[0], pd.pos[2], pd.pos[1], &floorH, &floorWhat, floor, true))
        {
            // 17c: in the air with nothing below - built anyway: the body falls (into the clouds)
            floorH = pd.pos[1] - kVoidDepth;
            floorWhat = "nothing (no floor within 40 m below)";
        }
        // 17c: in the air (jet pack, jump, roll, a ledge): built anyway - the body falls onto the floor
    }
    const int n = s->rs.count;
    float world[ragdoll::kMaxBones * 16];
    float model[ragdoll::kMaxBones * 16]; // 15g: kept for the lab file
    for (int i = 0; i < n; ++i)
    {
        float* m = model + i * 16;
        ReadMatrix(BoneMatrix(ctl, i), m);
        Mul4(m, w, world + i * 16);
    }
    {
        char room[200];
        if (!RdMemRoom(room, sizeof(room))) // 22c
        {
            Skip(kSkipFailed, soldier, p, room);
            return nullptr;
        }
    }
    ragdoll::Built b;
    memset(&b, 0, sizeof(b));
    char why[96] = "";
    const double t0 = QpcMs();
    const DWORD now = GetTickCount();
    const PushRec* pushed = thrown ? nullptr : RecentPush(soldier, now); // 15f: an explosion just before death
    const int h = ragdoll::Create(s->rs, world, pd.vel, GroundCb, floor, &b, why, sizeof(why),
                                  thrown ? pd.spin : (pushed ? pushed->spin : nullptr),
                                  thrown ? g_thrownGravity : g_gravity);
    const double ms = QpcMs() - t0;
    if (h < 0)
    {
        snprintf(text, sizeof(text), "build failed: %s", why);
        Skip(kSkipFailed, soldier, p, text);
        return nullptr;
    }
    if (b.lowestAbove < -0.3f)
    {
        // 15f: part of him is under the terrain surface - he died over a cut-out
        ragdoll::Destroy(h);
        snprintf(text, sizeof(text), "starts %.2f m under the %s (%s)", static_cast<double>(-b.lowestAbove),
                 floor ? "floor" : "terrain", floor ? "the floor grid misses something under him" : "over a cut-out: bunker / tunnel?");
        Skip(kSkipTerrain, soldier, p, text);
        return nullptr;
    }
    memset(a, 0, sizeof(*a));
    a->used = true;
    a->id = ++g_rdIds;
    if (g_trackOn)
        TrackBuilt(soldier, a->id); // 15i
    a->handle = h;
    a->player = p;
    a->soldier = soldier;
    a->skel = s->skel;
    a->count = n;
    a->tick0 = pd.tick;
    a->lastSeen = now;
    a->kind = pd.kind;
    a->built = now;
    a->pelvisBone = s->rs.role[ragdoll::kRolePelvis]; // 15h: the getting-up matching
    a->headBone = s->rs.role[ragdoll::kRoleHead];
    // 15f: the shot that killed him - an impulse at the hit point
    const HitRec* hit = thrown ? nullptr : RecentHit(soldier, now);
    LabRecord(*a, pd, *s, model, w, pushed, hit); // 15g: the start data, for the lab page
    // 15g: WeaponDrop - a death ragdoll leaves the gun to the death animation
    a->keepBone = (!thrown && g_weaponDrop == 2 && s->weaponBone >= 0 && s->weaponBone < n) ? s->weaponBone : -1;
    if (!thrown && g_weaponDrop == 1 && s->rs.weaponBone > 0)
        ragdoll::ReleaseWeapon(h, kGunHoldSecs); // 15h: the hand lets go, the gun falls as its own body
    a->animRec = LabAnimSlot();
    char gunText[80];
    WeaponText(soldier, gunText, sizeof(gunText));
    char hitText[120] = "";
    if (hit)
    {
        const bool head = hit->zone == 2;
        const float imp = head ? g_headshotPunch : g_hitPush;
        const char* part = imp > 0.0f ? ragdoll::Hit(h, hit->pos, hit->dir, imp) : nullptr;
        if (head && imp > 0.0f)
        {
            const int sp = s->rs.role[ragdoll::kRoleSpine];
            const float chest[3] = { world[sp * 16 + 12], world[sp * 16 + 13], world[sp * 16 + 14] };
            ragdoll::Hit(h, chest, hit->dir, imp * 0.5f); // the body goes back with the head
        }
        snprintf(hitText, sizeof(hitText), "; %s %.2f s before: %.0f N s on the %s%s",
                 head ? "HEADSHOT" : (hit->zone == 1 ? "body hit" : "hit"),
                 static_cast<double>(now - hit->tick) / 1000.0, static_cast<double>(imp), part ? part : "-",
                 head ? " + half on the chest" : "");
    }
    if (pushed)
    {
        const size_t used = strlen(hitText);
        snprintf(hitText + used, sizeof(hitText) - used, "; blast %.2f s before (+%.1f m/s, tumble)",
                 static_cast<double>(now - pushed->tick) / 1000.0, static_cast<double>(pushed->strength));
    }
    ragdoll::Status st;
    if (ragdoll::GetStatus(h, &st))
        memcpy(a->start, st.pelvis, sizeof(a->start));
    ++g_rdBuilt;
    ++g_rdWinBuilt;
    if (RdLog())
    {
        const double r2d = 57.29578;
        Log("[ragdoll] %lu ms: %s ragdoll #%d built for %s soldier 0x%08X, %.2f s after %s, in %.2f ms: 11 bodies "
            "(%.0f kg), 10 joints; knees bent %.0f / %.0f deg, elbows %.0f / %.0f deg; terrain %.2f .. %.2f, lowest "
            "point %+.2f m; start speed %.1f m/s; %d part pairs overlapping in the death pose%s; gun: %s, %s%s",
            ScanMsSinceAttach(), thrown ? "THROWN" : "death", a->id, WhoText(p), static_cast<unsigned>(soldier),
            static_cast<double>(t), thrown ? "the push" : "death", ms,
            static_cast<double>(b.massKg), static_cast<double>(b.bendKnee[0]) * r2d,
            static_cast<double>(b.bendKnee[1]) * r2d, static_cast<double>(b.bendElbow[0]) * r2d,
            static_cast<double>(b.bendElbow[1]) * r2d, static_cast<double>(b.groundMin),
            static_cast<double>(b.groundMax), static_cast<double>(b.lowestAbove),
            static_cast<double>(Length3(pd.vel[0], pd.vel[1], pd.vel[2])), b.overlapsAtStart, hitText, gunText,
            a->keepBone >= 0 ? "follows the death animation (WeaponDrop 2)"
                             : (!thrown && g_weaponDrop == 1 && s->rs.weaponBone > 0 ? "falls from the hand (WeaponDrop 1)"
                                                                                     : "in the ragdoll's hand"),
            a->animRec ? "; animation recorded for the lab" : "");
    }
    if (floor)
    {
        // 17c: where the build time goes (the stutter): rays, terrain calls, naming what was hit, the rest
        g_floorBuilds += 1;
        g_floorBuildMs += ms;
        g_floorRayMs += floor->ms;
        g_floorTerrainMs += floor->terrainMs;
        g_floorClassMs += floor->classMs;
        if (ms > g_floorBuildMax)
            g_floorBuildMax = ms;
    }
    if (floor && RdLog())
        Log("[ragdoll]   #%d floor: on %s, %+.2f m from it%s; ground patch %.2f .. %.2f m; build %.2f ms = %d rays "
            "%.2f ms (%d hit, %d with nothing below) + %d terrain calls %.2f ms%s + naming hits %.2f ms + the rest %.2f "
            "ms; rays that also had terrain below: %d, they matched it %d times",
            a->id, floorWhat, static_cast<double>(pd.pos[1] - floorH),
            std::fabs(pd.pos[1] - floorH) > kOnFloor ? " (in the air: the body falls)" : "",
            static_cast<double>(b.groundMin), static_cast<double>(b.groundMax), ms, floor->rays, floor->ms,
            floor->rayHits, floor->voids, floor->terrainCalls, floor->terrainMs,
            floor->noTerrain ? " (none: no terrain here)" : "", floor->classMs,
            ms - floor->ms - floor->terrainMs - floor->classMs, floor->terrainBoth, floor->terrainMatched);
    return a;
}

// 15e: out = the game's bone matrix blended toward the ragdoll's by weight
// (1 = all ragdoll): rotations slerped, positions lerped, the game's scale kept.
void BlendBone(const float* rag, const float* game, float weight, float* out)
{
    btVector3 gr[3];
    btVector3 rr[3];
    float scale[3];
    for (int r = 0; r < 3; ++r)
    {
        gr[r] = btVector3(game[r * 4], game[r * 4 + 1], game[r * 4 + 2]);
        rr[r] = btVector3(rag[r * 4], rag[r * 4 + 1], rag[r * 4 + 2]);
        scale[r] = gr[r].length();
        const float rl = rr[r].length();
        gr[r] = scale[r] > 1e-6f ? gr[r] / scale[r] : btVector3(0, 0, 0);
        rr[r] = rl > 1e-6f ? rr[r] / rl : btVector3(0, 0, 0);
    }
    const btMatrix3x3 gm(gr[0].x(), gr[0].y(), gr[0].z(), gr[1].x(), gr[1].y(), gr[1].z(), gr[2].x(), gr[2].y(),
                         gr[2].z());
    const btMatrix3x3 rm(rr[0].x(), rr[0].y(), rr[0].z(), rr[1].x(), rr[1].y(), rr[1].z(), rr[2].x(), rr[2].y(),
                         rr[2].z());
    btQuaternion qg;
    btQuaternion qr;
    gm.getRotation(qg);
    rm.getRotation(qr);
    const btMatrix3x3 m(qg.slerp(qr, weight));
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
            out[r * 4 + c] = m[r][c] * scale[r];
        out[r * 4 + 3] = 0.0f;
    }
    for (int c = 0; c < 3; ++c)
        out[12 + c] = game[12 + c] + (rag[12 + c] - game[12 + c]) * weight;
    out[15] = 1.0f;
}

// Writes the ragdoll's pose into the controller's bone matrices (model space).
// weight < 1 (15e, a survivor getting up; 15f, the blend-in): blended with
// the game's own animated pose of this frame (copied at its first update).
// 15h: how much of the getting-up matching is left (1 at the start, 0 = none), smoothed
float MatchLeft(const Active& a, DWORD now)
{
    if (!a.matchTick)
        return 0.0f;
    const float x = 1.0f - static_cast<float>(now - a.matchTick) / static_cast<float>(a.matchMs ? a.matchMs : kMatchMs);
    return x > 0.0f ? x * x * (3.0f - 2.0f * x) : 0.0f;
}

// 15h: world matrix of the matching: about world y through the pivot by yaw x f,
// then moved by off x f (Direct3D row vectors)
void MatchMatrix(const Active& a, float f, float* m)
{
    const float c = std::cos(a.matchYaw * f);
    const float sn = std::sin(a.matchYaw * f);
    const float rows[16] = { c, 0.0f, -sn, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, sn, 0.0f, c, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
    memcpy(m, rows, sizeof(rows));
    const float* p = a.matchPivot;
    for (int k = 0; k < 3; ++k)
        m[12 + k] = p[k] - (p[0] * m[k] + p[1] * m[4 + k] + p[2] * m[8 + k]) + a.matchOff[k] * f;
}

// Writes the ragdoll's pose into the controller's bone matrices (model space).
// weight < 1 (15e, a survivor getting up; 15f, the blend-in): blended with
// the game's own animated pose of this frame (copied at its first update).
// 15h: a survivor's getting-up animation is moved onto where he lay first
// (MatchLeft), and is written alone once the ragdoll is gone (handle < 0).
void ApplyRagdoll(const Active& a, uintptr_t ctl, uintptr_t soldier, float weight, DWORD now)
{
    float bones[ragdoll::kMaxBones * 16];
    const bool rag = a.handle >= 0 && ragdoll::GetPose(a.handle, bones);
    if (rag && FloorsOn() && a.pelvisBone >= 0 && a.pelvisBone < a.count)
    {
        const float y = bones[a.pelvisBone * 16 + 13];
        if (std::isfinite(y))
            g_floorCtx[&a - g_active].refY = y; // 17b: the next patch's rays start above the body as it is now
    }
    if (!rag && !(a.haveAnim && a.matchTick))
        return;
    float w[16];
    float inv[16];
    ReadMatrix(soldier + kSoldierMatrix, w);
    RigidInverse(w, inv);
    // the matching in model space: W x M x W^-1
    float mm[16];
    const float f = a.haveAnim ? MatchLeft(a, now) : 0.0f;
    if (f > 0.0f)
    {
        float m[16];
        float t[16];
        MatchMatrix(a, f, m);
        Mul4(w, m, t);
        Mul4(t, inv, mm);
    }
    for (int i = 0; i < a.count; ++i)
    {
        if (i == a.keepBone)
            continue; // 15g: WeaponDrop 2 - the gun keeps the game's own (animated) matrix
        float anim[16];
        if (a.haveAnim)
        {
            if (f > 0.0f)
                Mul4(a.anim + i * 16, mm, anim);
            else
                memcpy(anim, a.anim + i * 16, sizeof(anim));
        }
        float out[16];
        if (!rag)
        {
            memcpy(out, anim, sizeof(out)); // 15h: the game's getting up, moved
        }
        else
        {
            float m[16];
            Mul4(bones + i * 16, inv, m);
            if (weight < 0.999f && a.haveAnim)
                BlendBone(m, anim, weight, out); // 15f: this frame's animated pose (both views)
            else
                memcpy(out, m, sizeof(out));
        }
        memcpy(reinterpret_cast<void*>(BoneMatrix(ctl, i)), out, sizeof(out));
    }
}

void SampleRagdoll(Active& a, float t)
{
    while (a.nextSample < kRdSampleCount && t >= kRdSampleTimes[a.nextSample])
    {
        ++a.nextSample;
        ragdoll::Status st;
        if (!ragdoll::GetStatus(a.handle, &st) || !RdLog())
            continue;
        Log("[ragdoll] ragdoll #%d at %.2f s: pelvis (%.2f %.2f %.2f), lowest point %+.2f m above the terrain, "
            "speed %.2f m/s (fastest %.1f)%s%s%s",
            a.id, static_cast<double>(t), static_cast<double>(st.pelvis[0]), static_cast<double>(st.pelvis[1]),
            static_cast<double>(st.pelvis[2]), static_cast<double>(st.lowestAbove), static_cast<double>(st.speed),
            static_cast<double>(st.speedMax), st.asleep ? ", asleep (settled)" : "", st.frozen ? ", FROZEN: " : "",
            st.frozen ? st.frozenWhy : "");
    }
}

// 15h: put the game's getting-up animation (its first frame, a->anim) onto where
// the ragdoll lies: the turn that lines up its pelvis -> head direction with the
// ragdoll's (about world y), and the move that puts its pelvis on the ragdoll's.
void MatchGetup(Active& a, uintptr_t soldier, DWORD now)
{
    a.matchPending = false;
    float rag[ragdoll::kMaxBones * 16];
    if (!ragdoll::GetPose(a.handle, rag))
        return;
    const int pel = a.pelvisBone;
    const int head = a.headBone;
    if (pel < 0 || head < 0 || pel >= a.count || head >= a.count)
        return;
    float w[16];
    ReadMatrix(soldier + kSoldierMatrix, w);
    float ap[16];
    float ah[16];
    Mul4(a.anim + pel * 16, w, ap);
    Mul4(a.anim + head * 16, w, ah);
    const float* rp = rag + pel * 16 + 12;
    const float* rh = rag + head * 16 + 12;
    const float aX = ah[12] - ap[12];
    const float aZ = ah[14] - ap[14];
    const float bX = rh[0] - rp[0];
    const float bZ = rh[2] - rp[2];
    a.matchYaw = std::atan2(aZ * bX - aX * bZ, aX * bX + aZ * bZ);
    if (!std::isfinite(a.matchYaw))
        a.matchYaw = 0.0f;
    a.matchOff[0] = rp[0] - ap[12];
    a.matchOff[1] = 0.0f;
    a.matchOff[2] = rp[2] - ap[14];
    a.matchPivot[0] = ap[12];
    a.matchPivot[1] = ap[13];
    a.matchPivot[2] = ap[14];
    a.matchTick = now ? now : 1;
    // the easing peaks at 1.5x its average speed: a long way gets longer (at most ~2 m/s)
    const float dist = std::sqrt(a.matchOff[0] * a.matchOff[0] + a.matchOff[2] * a.matchOff[2]);
    const DWORD byDistance = static_cast<DWORD>(dist * 750.0f);
    a.matchMs = byDistance > kMatchMs ? byDistance : kMatchMs;
    if (RdLog())
        Log("[ragdoll] ragdoll #%d getting up: the animation moved %.2f m and turned %.0f deg onto where he lies, "
            "easing back over %.1f s",
            a.id, static_cast<double>(dist), static_cast<double>(a.matchYaw * 57.29578f),
            static_cast<double>(a.matchMs) / 1000.0);
}

// Pose hook (under the lock), after the game's update for this soldier.
void RagdollPose(uintptr_t soldier, uintptr_t ctl, int32_t state, SkelInfo* s, uint32_t frame)
{
    Active* a = FindActive(soldier);
    if (!a)
    {
        Pending* pd = FindPending(soldier);
        if (!pd)
            return;
        a = BuildRagdoll(*pd, soldier, ctl, state, s);
        if (!a)
            return;
    }
    const DWORD now = GetTickCount();
    if (a->kind == kKindThrown)
    {
        if (state == kStateDead)
        {
            if (a->handle < 0)
            {
                // 15h: he was already back in his animation (the ragdoll is gone)
                EndActive(*a, "the game set him dead after he was back in his own animation");
                return;
            }
            a->kind = kKindDead;
            a->tick0 = now; // the 4.5 s body time runs from his death
            a->blendTick = 0;
            a->getupTick = 0;
            a->matchPending = false;
            a->matchTick = 0;
            if (g_weaponDrop == 1)
                ragdoll::ReleaseWeapon(a->handle, kGunHoldSecs); // 15h: dead now - the hand lets go
            if (RdLog())
                Log("[ragdoll] %lu ms: thrown ragdoll #%d (%s): the game set him dead - he stays a ragdoll",
                    ScanMsSinceAttach(), a->id, WhoText(a->player));
        }
        else if (!a->getupTick && a->handle >= 0 && (state == 9 || (state != 5 && state != 8)))
        {
            // 15h: he starts getting up (state 9, or he left 5 / 8 / 9 without it): blend into the
            // game's getting-up animation from now, put onto where the ragdoll lies
            a->getupTick = now;
            a->blendTick = now;
            a->matchPending = true;
            if (RdLog())
                Log("[ragdoll] %lu ms: thrown ragdoll #%d (%s) is getting up (state %d %s): blending into his "
                    "getting-up animation over %.1f s, moved onto where he lies",
                    ScanMsSinceAttach(), a->id, WhoText(a->player), static_cast<int>(state), StateName(state),
                    static_cast<double>(kGetupBlendMs) / 1000.0);
        }
    }
    else if (state != kStateDead)
    {
        EndActive(*a, "the soldier left state 3 (the game reused him)");
        return;
    }
    float weight = 1.0f;
    if (a->blendTick)
    {
        weight = 1.0f - static_cast<float>(now - a->blendTick) / static_cast<float>(kGetupBlendMs);
        if (weight <= 0.0f)
        {
            weight = 0.0f;
            if (a->handle >= 0)
            {
                // 15h: the physics is done; his own (moved) animation goes on until the matching ends
                ragdoll::Destroy(a->handle);
                a->handle = -1;
            }
            if (MatchLeft(*a, now) <= 0.0f)
            {
                if (RdLog())
                    Log("[ragdoll] %lu ms: thrown ragdoll #%d (%s) survived: back in his own animation at his "
                        "own place",
                        ScanMsSinceAttach(), a->id, WhoText(a->player));
                EndActive(*a, "survivor: back in his own animation");
                return;
            }
        }
    }
    if (ReadU32(ctl + kCtlInst) != a->skel)
    {
        EndActive(*a, "his controller now has another skeleton");
        return;
    }
    a->lastSeen = now;
    if (a->lastFrame != frame)
    {
        a->lastFrame = frame;
        ++a->frames;
        // 15f: the frame's first update has just written the game's animated
        // pose: keep it (blending) and let the muscles follow it
        memcpy(a->anim, reinterpret_cast<const void*>(BoneMatrix(ctl, 0)),
               static_cast<size_t>(a->count) * 16 * sizeof(float));
        a->haveAnim = true;
        const DWORD recMs = (a->kind == kKindThrown || a->getupTick) ? kLabAnimThrownMs : kLabAnimMs; // 15h
        if (a->animRec && now - a->built <= recMs && (a->frames == 1 || now - a->lastAnimRec >= kLabAnimStepMs))
        {
            LabAnim(*a, static_cast<float>(now - a->built) / 1000.0f); // 15g: the animation the game plays
            a->lastAnimRec = now;
        }
        if (!a->blendTick && a->handle >= 0)
        {
            float w[16];
            float world[ragdoll::kMaxBones * 16];
            ReadMatrix(soldier + kSoldierMatrix, w);
            for (int i = 0; i < a->count; ++i)
                Mul4(a->anim + i * 16, w, world + i * 16);
            ragdoll::SetAnimTarget(a->handle, world);
        }
        if (a->matchPending && a->handle >= 0)
            MatchGetup(*a, soldier, now); // 15h: the getting-up animation onto where he lies
    }
    if (g_blendIn > 0.0f)
    {
        float in = static_cast<float>(now - a->built) / (g_blendIn * 1000.0f);
        if (in < 1.0f)
        {
            in = in < 0.0f ? 0.0f : in;
            in = in * in * (3.0f - 2.0f * in); // smooth start and end
            if (in < weight)
                weight = in;
        }
    }
    const double t0 = QpcMs();
    ApplyRagdoll(*a, ctl, soldier, weight, now);
    g_rdApplyMs += QpcMs() - t0;
    SampleRagdoll(*a, a->kind == kKindThrown ? static_cast<float>(now - a->tick0) / 1000.0f
                                             : ReadF32(soldier + kStateTime));
}

// ---------------------------------------------------------------------------
// 15g: live settings (BF1GameExt.ini + the live file) and the lab file
// ---------------------------------------------------------------------------
constexpr DWORD kLiveCheckMs = 500;  // files checked this often (and the lab buffer written)
constexpr int kMaxReloadLines = 200; // "settings reloaded" lines per session
constexpr int kLabMaxRecords = 60;   // R lines per session
constexpr int kLabMaxSkels = 8;
constexpr size_t kLabBufBytes = 96 * 1024;
constexpr size_t kLabRoom = 4096; // free space wanted before each piece of text

struct FileStamp
{
    bool exists;
    DWORD timeLow;
    DWORD timeHigh;
    DWORD size;
};

FileStamp StampOf(const char* path)
{
    FileStamp st = { false, 0, 0, 0 };
    WIN32_FILE_ATTRIBUTE_DATA d;
    if (path[0] && GetFileAttributesExA(path, GetFileExInfoStandard, &d) &&
        (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
    {
        st.exists = true;
        st.timeLow = d.ftLastWriteTime.dwLowDateTime;
        st.timeHigh = d.ftLastWriteTime.dwHighDateTime;
        st.size = d.nFileSizeLow;
    }
    return st;
}

bool SameStamp(const FileStamp& a, const FileStamp& b)
{
    return a.exists == b.exists && a.timeLow == b.timeLow && a.timeHigh == b.timeHigh && a.size == b.size;
}

FileStamp g_iniSeen = { false, 0, 0, 0 };
FileStamp g_iniLoaded = { false, 0, 0, 0 };
FileStamp g_liveSeen = { false, 0, 0, 0 };
FileStamp g_liveLoaded = { false, 0, 0, 0 };
DWORD g_liveTick = 0;
uint32_t g_reloads = 0;
int g_reloadLines = 0;

// The settings a reload can change, for the "old -> new" log line.
constexpr int kSnapCount = 22;
const char* const kSnapNames[kSnapCount] = {
    "Enabled",        "Range",       "MaxBodies",     "Debug",       "MuscleStrength", "MuscleTime",
    "Friction",       "RollingFriction", "LinearDamping", "AngularDamping", "JointScale",  "Gravity",
    "ThrownGravity",  "ThrownSpin",  "ThrownMaxSpeed", "SlowMotion", "AnimKeep",       "BlendIn",
    "HitPush",        "HeadshotPunch", "CorpseHit",    "WeaponDrop",
};

struct SettingSnap
{
    float v[kSnapCount];
};

void TakeSnap(SettingSnap& s)
{
    const ragdoll::Tuning& t = ragdoll::CurrentTuning();
    const float v[kSnapCount] = { g_rdLiveOn ? 1.0f : 0.0f,
                                  g_rdRange,
                                  static_cast<float>(g_rdMax),
                                  g_rdDebug ? 1.0f : 0.0f,
                                  t.muscleStrength,
                                  t.muscleTime,
                                  t.friction,
                                  t.rollingFriction,
                                  t.linearDamping,
                                  t.angularDamping,
                                  t.jointScale,
                                  g_gravity,
                                  g_thrownGravity,
                                  g_thrownSpin,
                                  g_thrownMaxSpeed,
                                  g_slowMotion,
                                  t.animKeep,
                                  g_blendIn,
                                  g_hitPush,
                                  g_headshotPunch,
                                  g_corpseHit,
                                  static_cast<float>(g_weaponDrop) };
    memcpy(s.v, v, sizeof(v));
}

double Dbl(float v)
{
    return static_cast<double>(v);
}

// The lab file: text is gathered here and written at the 0.5 s check (never
// at the moment of a death).
char g_labBuf[kLabBufBytes];
size_t g_labLen = 0;
bool g_labOk = false; // created at startup
int g_labRecords = 0;
int g_labSkels = 0;
bool g_labFailLogged = false;
int g_labAnims = 0;

void LabFlush()
{
    if (!g_labLen)
        return;
    if (g_labOk)
    {
        const HANDLE f = CreateFileA(g_labPath, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                     nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (f != INVALID_HANDLE_VALUE)
        {
            DWORD written = 0;
            WriteFile(f, g_labBuf, static_cast<DWORD>(g_labLen), &written, nullptr);
            CloseHandle(f);
        }
        else if (!g_labFailLogged)
        {
            g_labFailLogged = true;
            Log("[ragdoll] lab file %s could not be opened (error %lu): lab data from here on is lost", g_labPath,
                GetLastError());
        }
    }
    g_labLen = 0;
}

void LabText(const char* fmt, ...)
{
    if (!g_labOk)
        return;
    if (kLabBufBytes - g_labLen < kLabRoom)
        LabFlush();
    va_list args;
    va_start(args, fmt);
    const int n = vsnprintf(g_labBuf + g_labLen, kLabBufBytes - g_labLen, fmt, args);
    va_end(args);
    if (n <= 0)
        return;
    const size_t room = kLabBufBytes - g_labLen - 1;
    g_labLen += static_cast<size_t>(n) < room ? static_cast<size_t>(n) : room;
}

// T line: the settings in force.
void LabSettings(const char* why)
{
    const ragdoll::Tuning& t = ragdoll::CurrentTuning();
    LabText("T %lu %s Enabled=%d Range=%g MaxBodies=%d Debug=%d MuscleStrength=%g MuscleTime=%g Friction=%g "
            "RollingFriction=%g LinearDamping=%g AngularDamping=%g JointScale=%g Gravity=%g ThrownGravity=%g "
            "ThrownSpin=%g ThrownMaxSpeed=%g SlowMotion=%g BlendIn=%g AnimKeep=%g HitPush=%g HeadshotPunch=%g "
            "CorpseHit=%g WeaponDrop=%d HitScan=%d BulletTest=%d Scan=%d ScanRange=%g LiveFile=%d LabFile=%d\r\n",
            ScanMsSinceAttach(), why, g_rdLiveOn ? 1 : 0, Dbl(g_rdRange), g_rdMax, g_rdDebug ? 1 : 0,
            Dbl(t.muscleStrength), Dbl(t.muscleTime), Dbl(t.friction), Dbl(t.rollingFriction), Dbl(t.linearDamping),
            Dbl(t.angularDamping), Dbl(t.jointScale), Dbl(g_gravity), Dbl(g_thrownGravity), Dbl(g_thrownSpin),
            Dbl(g_thrownMaxSpeed), Dbl(g_slowMotion), Dbl(g_blendIn), Dbl(t.animKeep), Dbl(g_hitPush),
            Dbl(g_headshotPunch), Dbl(g_corpseHit), g_weaponDrop, g_hitScanIniValue ? 1 : 0, g_bulletTest ? 1 : 0,
            g_scanIni ? 1 : 0, Dbl(g_range), g_liveFile ? 1 : 0, g_labFile ? 1 : 0);
}

// K + B lines: a ragdoll skeleton's rest data (first time it is seen).
void LabSkeleton(SkelInfo& s)
{
    s.labId = -1;
    if (!g_labOk || !s.rsOk || g_labSkels >= kLabMaxSkels)
        return;
    s.labId = g_labSkels++;
    const uintptr_t bones = ReadU32(s.skel + kSkelBones);
    LabText("K %d %d", s.labId, s.rs.count);
    for (int r = 0; r < ragdoll::kRoleCount; ++r)
        LabText(" %d", s.rs.role[r]);
    LabText("\r\n");
    for (int i = 0; i < s.rs.count; ++i)
    {
        const uintptr_t b = bones + static_cast<uint32_t>(i) * kBoneBytes;
        const uint32_t h = ReadU32(b + kBoneHash);
        const char* name = NameOf(h);
        const Vec3 rp = ReadVec3(b + kBoneRestPos);
        const Vec3 rq = ReadVec3(b + kBoneRestRot);
        LabText("B %d %d %08X %s %d %02X %.4f %.5f %.5f %.5f %.6f %.6f %.6f %.6f\r\n", s.labId, i,
                static_cast<unsigned>(h), name ? name : "-", ReadI8(b + kBoneParent),
                static_cast<unsigned>(ReadU8(b + kBoneFlags)), Dbl(ReadF32(b + kBoneScale)), Dbl(rp.x), Dbl(rp.y),
                Dbl(rp.z), Dbl(rq.x), Dbl(rq.y), Dbl(rq.z), Dbl(ReadF32(b + kBoneRestRot + 12)));
    }
}

// R line: what a ragdoll was built from (the lab page can replay it with
// other settings). Bones in model space (as the controller had them).
void LabRecord(const Active& a, const Pending& pd, const SkelInfo& s, const float* model, const float* w,
               const PushRec* pushed, const HitRec* hit)
{
    if (!g_labOk || s.labId < 0 || g_labRecords >= kLabMaxRecords)
        return;
    ++g_labRecords;
    const bool thrown = pd.kind == kKindThrown;
    float h = 0.0f;
    const float above = Ground(w[12], w[14], &h) ? w[13] - h : 0.0f;
    const float* vel = thrown ? pd.v1 : pd.vel;
    float strength = 0.0f;
    float axis[3] = { 0.0f, 0.0f, 0.0f };
    if (thrown)
    {
        strength = pd.pushStrength;
        memcpy(axis, pd.pushAxis, sizeof(axis));
    }
    else if (pushed)
    {
        strength = pushed->strength;
        memcpy(axis, pushed->axis, sizeof(axis));
    }
    LabText("R %d %d %d %d %.3f", a.id, thrown ? 1 : 0, s.labId, a.player, Dbl(above));
    for (int r = 0; r < 3; ++r)
        LabText(" %.5f %.5f %.5f", Dbl(w[r * 4]), Dbl(w[r * 4 + 1]), Dbl(w[r * 4 + 2]));
    LabText(" %.3f %.3f %.3f %.3f %.4f %.4f %.4f", Dbl(vel[0]), Dbl(vel[1]), Dbl(vel[2]), Dbl(strength), Dbl(axis[0]),
            Dbl(axis[1]), Dbl(axis[2]));
    if (hit)
        LabText(" %d %.3f %.3f %.3f %.4f %.4f %.4f", hit->zone == 2 ? 2 : 1, Dbl(hit->pos[0] - w[12]),
                Dbl(hit->pos[1] - w[13]), Dbl(hit->pos[2] - w[14]), Dbl(hit->dir[0]), Dbl(hit->dir[1]),
                Dbl(hit->dir[2]));
    else
        LabText(" 0 0 0 0 0 0 0");
    LabText(" %d", a.count);
    for (int i = 0; i < a.count; ++i)
    {
        const float* m = model + i * 16;
        LabText(" %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f", Dbl(m[0]), Dbl(m[1]), Dbl(m[2]),
                Dbl(m[4]), Dbl(m[5]), Dbl(m[6]), Dbl(m[8]), Dbl(m[9]), Dbl(m[10]), Dbl(m[12]), Dbl(m[13]),
                Dbl(m[14]));
    }
    LabText("\r\n");
}

// Whether the next ragdoll's animation is recorded (the first 10 that have an R line).
bool LabAnimSlot()
{
    if (!g_labOk || g_labAnims >= kLabAnimRecords || g_labRecords == 0)
        return false;
    ++g_labAnims;
    return true;
}

// A line: the game's animated pose this frame (model space, as the muscles get it).
void LabAnim(const Active& a, float t)
{
    if (!g_labOk)
        return;
    LabText("A %d %.3f", a.id, Dbl(t));
    for (int i = 0; i < a.count; ++i)
    {
        const float* m = a.anim + i * 16;
        LabText(" %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f", Dbl(m[0]), Dbl(m[1]), Dbl(m[2]),
                Dbl(m[4]), Dbl(m[5]), Dbl(m[6]), Dbl(m[8]), Dbl(m[9]), Dbl(m[10]), Dbl(m[12]), Dbl(m[13]),
                Dbl(m[14]));
    }
    LabText("\r\n");
}

// At startup (ragdolls on): a new lab file with its key and the settings.
void LabStart()
{
    if (!g_labPath[0])
    {
        Log("[ragdoll] lab file: off (%s)", g_labFile ? "no Documents folder found" : "LabFile=0");
        return;
    }
    const HANDLE f = CreateFileA(g_labPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                 nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE)
    {
        Log("[ragdoll] lab file %s could not be made (error %lu): no lab data this session", g_labPath,
            GetLastError());
        return;
    }
    CloseHandle(f);
    g_labOk = true;
    SYSTEMTIME now;
    GetLocalTime(&now);
    LabText("# BF1GameExt ragdoll lab file: written by the game for the ragdoll lab page (ragdoll.cpp %s, physics "
            "ragdoll_body.cpp %s). Remade at every game start.\r\n",
            kFileBuild, ragdoll::FileBuild());
    LabText("# session %04u-%02u-%02u %02u:%02u:%02u\r\n", static_cast<unsigned>(now.wYear),
            static_cast<unsigned>(now.wMonth), static_cast<unsigned>(now.wDay), static_cast<unsigned>(now.wHour),
            static_cast<unsigned>(now.wMinute), static_cast<unsigned>(now.wSecond));
    LabText("# T <ms> <why> <key=value ...>: the [Ragdoll] settings in force (at start, then after every reload)\r\n");
    LabText("# K <skel> <bones> <16 role bones: pelvis b_spine neck head l_thigh l_calf l_foot r_thigh r_calf r_foot "
            "l_upperarm l_forearm l_hand r_upperarm r_forearm r_hand>\r\n");
    LabText("# B <skel> <bone> <name hash> <name or -> <parent> <flags> <scale (+64h)> <rest position x y z> <rest "
            "rotation x y z w>\r\n");
    LabText("# R <ragdoll> <0 death, 1 thrown> <skel> <player: -1 AI, 0, 1> <origin above the terrain> <soldier "
            "rotation: 3 rows x y z> <velocity x y z: death = as the ragdoll got it (12 m/s at most), thrown = after "
            "the push, before ThrownMaxSpeed> <blast: m/s, tumble axis x y z> <hit: zone 0 none / 1 body / 2 head, "
            "point - origin x y z, direction x y z> <bones> <per bone 12: x axis, y axis, z axis, position - model "
            "space>\r\n");
    LabText("# A <ragdoll> <seconds after the build> <per bone 12, as R>: the game's own animated pose (20 a second "
            "for 2.5 s, the first %d ragdolls)\r\n",
            kLabAnimRecords);
    LabText("V 1\r\n");
    LabSettings("start");
    LabFlush();
    Log("[ragdoll] lab file: %s (settings, ragdoll skeletons, the first %d ragdolls' start data - for the lab page)",
        g_labPath, kLabMaxRecords);
}

int EndAllRagdolls(const char* why)
{
    int n = 0;
    for (Active& a : g_active)
    {
        if (a.used)
        {
            EndActive(a, why);
            ++n;
        }
    }
    for (Pending& p : g_pending)
        p.used = false;
    return n;
}

// Re-reads the live values; logs the keys that changed. token: one word for
// the lab file's T line.
void ReloadSettings(const char* why, const char* token)
{
    SettingSnap before;
    TakeSnap(before);
    const bool wasOn = g_rdLiveOn;
    ReadLiveValues();
    SettingSnap after;
    TakeSnap(after);
    ++g_reloads;
    char text[720];
    text[0] = 0;
    size_t used = 0;
    int changed = 0;
    for (int k = 0; k < kSnapCount; ++k)
    {
        if (before.v[k] == after.v[k])
            continue;
        if (used + 48 < sizeof(text))
        {
            const int n = snprintf(text + used, sizeof(text) - used, "%s%s %g -> %g", changed ? ", " : "",
                                   kSnapNames[k], Dbl(before.v[k]), Dbl(after.v[k]));
            if (n > 0)
                used += static_cast<size_t>(n) < sizeof(text) - used ? static_cast<size_t>(n) : sizeof(text) - used - 1;
        }
        ++changed;
    }
    if (g_reloadLines < kMaxReloadLines)
    {
        ++g_reloadLines;
        Log("[ragdoll] %lu ms: settings reloaded (%s, #%u): %s", ScanMsSinceAttach(), why,
            static_cast<unsigned>(g_reloads), changed ? text : "no value changed");
        if (g_reloadLines == kMaxReloadLines)
            Log("[ragdoll] reload line limit reached: further reloads are not logged (the lab file's T lines go on)");
    }
    if (wasOn && !g_rdLiveOn)
    {
        const int n = EndAllRagdolls("Enabled=0 in the settings");
        Log("[ragdoll] Enabled=0: ragdolls OFF - %d running ended, no new ones until Enabled=1", n);
    }
    else if (!wasOn && g_rdLiveOn)
    {
        Log("[ragdoll] Enabled=1: ragdolls ON again");
    }
    LabSettings(token);
}

// Every 0.5 s (from the first pose update of a frame): writes the lab buffer,
// and reloads the settings once BF1GameExt.ini or the live file has changed
// and then stayed the same for one check (a file being saved is not read
// half-written; a slider being dragged is read once it stops).
void LiveCheck(DWORD now)
{
    if (now - g_liveTick < kLiveCheckMs)
        return;
    g_liveTick = now;
    LabFlush();
    const FileStamp ini = StampOf(g_iniPath);
    const FileStamp none = { false, 0, 0, 0 };
    const FileStamp live = g_livePath[0] ? StampOf(g_livePath) : none;
    const bool settled = SameStamp(ini, g_iniSeen) && SameStamp(live, g_liveSeen);
    g_iniSeen = ini;
    g_liveSeen = live;
    if (!settled || !ini.exists)
        return;
    const bool iniChanged = !SameStamp(ini, g_iniLoaded);
    const bool liveChanged = !SameStamp(live, g_liveLoaded);
    if (!iniChanged && !liveChanged)
        return;
    const char* why = "BF1GameExt.ini changed";
    if (liveChanged)
        why = !live.exists ? "ragdoll_live.ini removed"
                           : (g_liveLoaded.exists ? (iniChanged ? "BF1GameExt.ini and ragdoll_live.ini changed"
                                                                : "ragdoll_live.ini changed")
                                                  : "ragdoll_live.ini appeared");
    g_iniLoaded = ini;
    g_liveLoaded = live;
    ReloadSettings(why, "reload");
}

// At startup: the files as they are now count as loaded.
void LiveStart()
{
    const FileStamp none = { false, 0, 0, 0 };
    g_iniLoaded = g_iniSeen = StampOf(g_iniPath);
    g_liveLoaded = g_liveSeen = g_livePath[0] ? StampOf(g_livePath) : none;
    g_liveTick = GetTickCount();
    if (!g_livePath[0])
        Log("[ragdoll] live settings: BF1GameExt.ini only (%s), checked every %.1f s",
            g_liveFile ? "no Documents folder found" : "LiveFile=0", Dbl(kLiveCheckMs / 1000.0f));
    else
        Log("[ragdoll] live settings: BF1GameExt.ini and %s (%s; its keys win over BF1GameExt.ini's), both checked "
            "every %.1f s - a change is used once the file has stayed the same for one check",
            g_livePath, g_liveLoaded.exists ? "FOUND" : "not there yet - the lab page writes it",
            Dbl(kLiveCheckMs / 1000.0f));
}

bool g_f7Down = false;
bool g_f8Down = false;

bool GameInFront()
{
    const HWND w = GetForegroundWindow();
    DWORD pid = 0;
    if (w)
        GetWindowThreadProcessId(w, &pid);
    return pid == GetCurrentProcessId();
}

// 15e: F7 reloads the [Ragdoll] settings, F8 toggles slow motion (game window only).
void RagdollKeys()
{
    const bool front = GameInFront();
    const bool f7 = front && (GetAsyncKeyState(VK_F7) & 0x8000) != 0;
    const bool f8 = front && (GetAsyncKeyState(VK_F8) & 0x8000) != 0;
    if (f7 && !g_f7Down)
    {
        // 15g: the same reload as the automatic one (both files, changes logged)
        const FileStamp none = { false, 0, 0, 0 };
        g_iniLoaded = g_iniSeen = StampOf(g_iniPath);
        g_liveLoaded = g_liveSeen = g_livePath[0] ? StampOf(g_livePath) : none;
        ReloadSettings("F7 pressed", "F7");
    }
    if (f8 && !g_f8Down)
    {
        g_slowOn = !g_slowOn;
        ragdoll::Tuning t = ragdoll::CurrentTuning();
        t.timeScale = g_slowOn ? g_slowMotion : 1.0f;
        ragdoll::SetTuning(t);
        Log("[ragdoll] F8: slow motion %s (ragdolls at x%.2f)", g_slowOn ? "ON" : "off",
            static_cast<double>(t.timeScale));
    }
    g_f7Down = f7;
    g_f8Down = f8;
}

// Once per frame (the first pose update of the frame).
void RagdollFrame(DWORD now)
{
    LiveCheck(now); // 15g: settings files, lab file
    RagdollKeys();
    ++g_rdWinFrames;
    for (Pending& p : g_pending)
    {
        if (p.used && now - p.tick > kPendingMs)
        {
            p.used = false;
            Skip(kSkipLate, p.soldier, p.player, "not drawn within 0.5 s of death (left to the death animation)");
        }
    }
    bool any = false;
    for (Active& a : g_active)
    {
        if (!a.used)
            continue;
        if (now - a.lastSeen > kNotSeenMs)
            EndActive(a, "not drawn for 1.5 s (off screen or faded out)");
        else if (a.kind == kKindDead && now - a.tick0 > kLifeMs)
            EndActive(a, "4.5 s after death (the game stops drawing bodies at 4 s)");
        else if (a.kind == kKindThrown && now - a.tick0 > kThrownLifeMs)
            EndActive(a, "thrown 9 s ago and neither dead nor up");
        else
        {
            any = true;
            ProbeWalls(a, now); // 17e
        }
    }
    if (!any)
        return;
    const float dt = g_dtVa ? ReadF32(g_dtVa) : 0.0f;
    const double t0 = QpcMs();
    ragdoll::Step(dt);
    const double ms = QpcMs() - t0;
    g_rdStepMs += ms;
    if (ms > g_rdStepMax)
        g_rdStepMax = ms;
}

void RagdollEndSoldier(uintptr_t soldier, const char* why)
{
    Active* a = FindActive(soldier);
    if (a)
        EndActive(*a, why);
    Pending* p = FindPending(soldier);
    if (p)
        p->used = false;
}

void WallScanRun(); // 17c (below)

void RagdollSummary(double secs)
{
    const double frames = g_rdWinFrames ? static_cast<double>(g_rdWinFrames) : 1.0;
    if (g_floorBuilds)
        Log("[ragdoll] floor builds so far: %d, %.1f ms on average (rays %.1f, terrain calls %.1f, naming hits %.1f, the "
            "rest %.1f), the longest %.1f ms",
            g_floorBuilds, g_floorBuildMs / g_floorBuilds, g_floorRayMs / g_floorBuilds, g_floorTerrainMs / g_floorBuilds,
            g_floorClassMs / g_floorBuilds,
            (g_floorBuildMs - g_floorRayMs - g_floorTerrainMs - g_floorClassMs) / g_floorBuilds, g_floorBuildMax);
    WallScanRun(); // 17c: log only
    if (g_wallRaysTotal)
        Log("[ragdoll] walls (17e) so far: %d rays from ragdolls, %d slabs placed (walls, railings, pillars, ceilings "
            "next to bodies)",
            g_wallRaysTotal, g_wallSlabsTotal);
    Log("[ragdoll] ragdolls, last %.1f s: %d running, %u built (%u in all, %u ended); no ragdoll: %u no other player, "
        "%u out of range, %u not on terrain, %u MaxBodies, %u drawn late, %u skeleton, %u build failed, %u died thrown, "
        "%u seated; physics %.3f ms per frame (max %.3f), writing bones %.3f ms per frame",
        secs, ragdoll::ActiveCount(), static_cast<unsigned>(g_rdWinBuilt), static_cast<unsigned>(g_rdBuilt),
        static_cast<unsigned>(g_rdEnded), static_cast<unsigned>(g_rdSkips[kSkipAlone]),
        static_cast<unsigned>(g_rdSkips[kSkipRange]), static_cast<unsigned>(g_rdSkips[kSkipTerrain]),
        static_cast<unsigned>(g_rdSkips[kSkipLimit]), static_cast<unsigned>(g_rdSkips[kSkipLate]),
        static_cast<unsigned>(g_rdSkips[kSkipSkeleton]), static_cast<unsigned>(g_rdSkips[kSkipFailed]),
        static_cast<unsigned>(g_rdSkips[kSkipThrown]), static_cast<unsigned>(g_rdSkips[kSkipSeated]),
        g_rdStepMs / frames, g_rdStepMax, g_rdApplyMs / frames);
    // 21g-r4: the skeleton table - on the first summary, then whenever a slot was given back or none was free
    const uint32_t skelNow = g_skelReused + g_skelReread + g_skelFull;
    if (skelNow != g_skelLogged)
    {
        g_skelLogged = skelNow;
        Log("[ragdoll] skeletons: %d of %d slots used; since the start %u slot(s) given to a new skeleton (the old one unused "
            "for %.0f s), %u read again (another skeleton at its address), %u time(s) no slot free",
            g_skelCount, kMaxSkeletons, static_cast<unsigned>(g_skelReused), static_cast<double>(kSkelReuseMs) / 1000.0,
            static_cast<unsigned>(g_skelReread), static_cast<unsigned>(g_skelFull));
    }
    g_rdWinBuilt = 0;
    g_rdStepMs = 0.0;
    g_rdStepMax = 0.0;
    g_rdApplyMs = 0.0;
    g_rdWinFrames = 0;
}

// ---------------------------------------------------------------------------
// 15d: hit scan (log-only) - pushes, damage, thrown soldiers. For the next
// round's thrown ragdolls, rocket push and headshot punch. INI [Ragdoll]
// HitScan=1. Only soldiers within Range metres of a local player (or the
// players' own) are logged; line counts are bounded.
// ---------------------------------------------------------------------------
typedef uint32_t(__fastcall* Push_t)(void* self, void* edx, uint32_t impulse, uint32_t scale);
typedef uint32_t(__fastcall* Damage_t)(void* self, void* edx, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4,
                                       uint32_t a5);

// EntitySoldier primary vtable slot 32 (GOG 0x00508510): velocity += impulse x
// scale; then upward speed > 2 m/s (and a check) -> SetState(5) "thrown",
// else speed > 2 m/s -> SetState(8). thiscall (const Vec3* impulse, float
// scale), ret 8; no caller uses its return value.
const char* const kPushBody =
    "55 8B EC 83 EC 0C F3 0F 10 45 0C 8D 45 F4 56 57 51 F3 0F 11 04 24 8B F1 FF 75 08 50 8D BE CC 08 00 00 E8";
const char* const kPushSetState8 = "6A 08 8B CE E8";
constexpr uint32_t kPushWindow = 0xE5; // the function's size on GOG 1.3
constexpr int kSlotPush = 32;
// EntitySoldier +8 (damage interface) vtable slot 5 (GOG 0x005083A0): soldier
// only, thiscall with 5 arguments, ret 14h, returns a bool in al. Its +A68h
// (soldier +A70h) is the animation controller.
const char* const kDamageBody =
    "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 83 EC 58 56 57 8B F1 E8 ?? ?? ?? ?? 84 C0 0F "
    "84 F0 00 00 00 8B BE 68 0A 00 00 85 FF";
constexpr uint32_t kDamageWindow = 0x125;
constexpr int kSlotDamage = 5;
constexpr int kMaxPushLines = 120;
constexpr int kMaxDamageLines = 150;
constexpr int kMaxThrownLines = 260;
constexpr int kMaxThrown = 8;
constexpr DWORD kThrownTrackMs = 8000;
constexpr uint32_t kCtlMove = 0x94;  // the move animation the controller chose (14e)
constexpr uint32_t kCtlState = 0x58; // the soldier state the controller was given

struct Thrown
{
    bool used;
    int id;
    int player;
    uintptr_t soldier;
    DWORD tick0;
    int32_t lastState;
    uint32_t lastMove;
    uint32_t lastFrame;
    bool died;
};

bool g_hitScanIni = false; // on once the push or damage hook is in
Push_t g_origPush = nullptr;
Damage_t g_origDamage = nullptr;
bool g_pushOn = false;
bool g_damageOn = false;
int g_pushLines = 0;
int g_damageLines = 0;
int g_thrownLines = 0;
int g_thrownIds = 0;
Thrown g_thrown[kMaxThrown];
uint32_t g_pushCount = 0;
uint32_t g_thrownCount = 0;
uint32_t g_damageCount = 0;

// Distance from the soldier to the nearest local player's soldier (0 for a
// player's own), or -1 if there is none.
float NearestPlayer(uintptr_t soldier, const Vec3& pos)
{
    float dist = -1.0f;
    for (int k = 0; k < kLocalMax; ++k) // 20g-r6: players 1-4
    {
        if (!g_player[k])
            continue;
        const float d = g_player[k] == soldier ? 0.0f : Dist(pos, SoldierPos(g_player[k]));
        if (dist < 0.0f || d < dist)
            dist = d;
    }
    return dist;
}

bool HitScanWanted(uintptr_t soldier, const Vec3& pos)
{
    const float d = NearestPlayer(soldier, pos);
    return d >= 0.0f && d <= g_rdRange;
}

Thrown* FindThrown(uintptr_t soldier)
{
    for (Thrown& t : g_thrown)
        if (t.used && t.soldier == soldier)
            return &t;
    return nullptr;
}

void StartThrown(uintptr_t soldier, int p)
{
    if (FindThrown(soldier))
        return;
    for (Thrown& t : g_thrown)
    {
        if (t.used)
            continue;
        memset(&t, 0, sizeof(t));
        t.used = true;
        t.id = ++g_thrownIds;
        t.player = p;
        t.soldier = soldier;
        t.tick0 = GetTickCount();
        t.lastState = -1;
        t.lastMove = 0xFFFFFFFFu;
        if (g_thrownLines < kMaxThrownLines)
        {
            ++g_thrownLines;
            Log("[hit]   -> tracking it as thrown #%d (states and animation for 8 s)", t.id);
        }
        return;
    }
}

// "x / f / (a b c)" for one damage argument: hex, as a float, and the first 3
// floats it points at when it is a readable pointer.
void ArgText(uint32_t v, char* out, size_t len)
{
    float f = 0.0f;
    memcpy(&f, &v, sizeof(f));
    int n = snprintf(out, len, "%08X", static_cast<unsigned>(v));
    if (std::isfinite(f) && std::fabs(f) > 1e-6f && std::fabs(f) < 1e7f)
        n += snprintf(out + n, len - static_cast<size_t>(n), " (%.3g)", static_cast<double>(f));
    if (v >= 0x10000 && (v & 3) == 0 && Readable(v, 12))
    {
        const Vec3 p = ReadVec3(v);
        if (std::isfinite(p.x) && std::isfinite(p.y) && std::isfinite(p.z) && std::fabs(p.x) < 1e5f &&
            std::fabs(p.y) < 1e5f && std::fabs(p.z) < 1e5f)
            snprintf(out + n, len - static_cast<size_t>(n), " -> (%.2f %.2f %.2f) [%08X]", static_cast<double>(p.x),
                     static_cast<double>(p.y), static_cast<double>(p.z), static_cast<unsigned>(ReadU32(v)));
        else
            snprintf(out + n, len - static_cast<size_t>(n), " -> [%08X %08X]", static_cast<unsigned>(ReadU32(v)),
                     static_cast<unsigned>(ReadU32(v + 4)));
    }
}

// 15e (under the lock): a push on a soldier. A ragdoll gets the push; a
// soldier the game has just thrown (state 5) becomes a thrown-ragdoll candidate.
void RagdollPush(uintptr_t soldier, int32_t after, const Vec3& pos, const Vec3& v0, const Vec3& v1, const Vec3& imp)
{
    if (!g_rdLiveOn)
        return; // 15g: Enabled=0 while playing - no ragdolls running, no new ones
    float dv[3] = { v1.x - v0.x, v1.y - v0.y, v1.z - v0.z };
    const float strength = Length3(dv[0], dv[1], dv[2]);
    // tumble about (push direction x up): the feet go with the blast
    float dx = imp.x;
    float dz = imp.z;
    float len = std::sqrt(dx * dx + dz * dz);
    if (len < 0.05f)
    {
        dx = dv[0];
        dz = dv[2];
        len = std::sqrt(dx * dx + dz * dz);
    }
    float spin[3] = { 0.0f, 0.0f, 0.0f };
    float axis[3] = { 0.0f, 0.0f, 0.0f }; // 15g: kept for the lab file
    if (len > 1e-3f)
    {
        float rate = g_thrownSpin * strength / 12.0f;
        if (rate > g_thrownSpin * 1.5f)
            rate = g_thrownSpin * 1.5f;
        axis[0] = -dz / len;
        axis[2] = dx / len;
        spin[0] = axis[0] * rate;
        spin[2] = axis[2] * rate;
    }
    Active* a = FindActive(soldier);
    if (a)
    {
        ragdoll::Push(a->handle, dv, spin);
        if (RdLog())
            Log("[ragdoll] %lu ms: explosion hit ragdoll #%d (%s): +%.1f m/s, tumble %.1f rad/s",
                ScanMsSinceAttach(), a->id, WhoText(a->player), static_cast<double>(strength),
                static_cast<double>(Length3(spin[0], spin[1], spin[2])));
        return;
    }
    if (after != 5)
    {
        // 15f: not thrown - remember it: a death within 0.5 s gets its tumble
        PushRec& rec = g_pushRecs[g_pushNext];
        g_pushNext = (g_pushNext + 1) % kRecs;
        rec.soldier = soldier;
        rec.tick = GetTickCount();
        memcpy(rec.spin, spin, sizeof(rec.spin));
        rec.strength = strength;
        memcpy(rec.axis, axis, sizeof(rec.axis));
        return;
    }
    if (FindPending(soldier))
        return;
    const int p = PlayerIndexNow(soldier); // 20g-r6: players 1-4
    const float dist = NearestPlayer(soldier, pos);
    char text[96];
    float h = 0.0f;
    if (g_trackOn)
        TrackThrow(soldier, p, pos, strength, dist); // 15i
    if (dist < 0.0f || dist > g_rdRange)
    {
        ++g_rdSkips[kSkipRange];
        if (g_trackOn)
        {
            if (dist < 0.0f)
                snprintf(text, sizeof(text), "thrown with no other local player alive");
            else
                snprintf(text, sizeof(text), "thrown %.0f m from the nearest player (Range %.0f)",
                         static_cast<double>(dist), static_cast<double>(g_rdRange));
            TrackOutcome(soldier, kSkipRange, text);
        }
        return; // thrown far away: not logged (there are many)
    }
    if (!FloorsOn() && (!Ground(pos.x, pos.z, &h) || std::fabs(pos.y - h) > kOnTerrain))
    {
        snprintf(text, sizeof(text), "thrown %+.2f m from the terrain (on an object or in the air)",
                 static_cast<double>(pos.y - h));
        Skip(kSkipTerrain, soldier, p, text);
        return;
    }
    if (Busy() >= g_rdMax)
    {
        Skip(kSkipLimit, soldier, p, "thrown, but MaxBodies ragdolls already");
        return;
    }
    for (Pending& c : g_pending)
    {
        if (c.used)
            continue;
        c.used = true;
        c.soldier = soldier;
        c.tick = GetTickCount();
        c.player = p;
        c.kind = kKindThrown;
        const float speed = Length3(v1.x, v1.y, v1.z);
        const float k = speed > g_thrownMaxSpeed ? g_thrownMaxSpeed / speed : 1.0f;
        c.vel[0] = v1.x * k;
        c.vel[1] = v1.y * k;
        c.vel[2] = v1.z * k;
        memcpy(c.spin, spin, sizeof(c.spin));
        c.v1[0] = v1.x; // 15g: lab file
        c.v1[1] = v1.y;
        c.v1[2] = v1.z;
        memcpy(c.pushAxis, axis, sizeof(c.pushAxis));
        c.pushStrength = strength;
        c.pos[0] = pos.x; // 17b
        c.pos[1] = pos.y;
        c.pos[2] = pos.z;
        if (RdLog())
            Log("[ragdoll] %lu ms: %s soldier 0x%08X THROWN %.1f m from a player (push +%.1f m/s, now %.1f m/s, "
                "tumble %.1f rad/s): thrown-ragdoll candidate",
                ScanMsSinceAttach(), WhoText(p), static_cast<unsigned>(soldier), static_cast<double>(dist),
                static_cast<double>(strength), static_cast<double>(speed * k),
                static_cast<double>(Length3(spin[0], spin[1], spin[2])));
        return;
    }
}

uint32_t __fastcall Hook_Push(void* self, void* edx, uint32_t impulse, uint32_t scale)
{
    const uintptr_t caller = BF1_RETURN_ADDRESS();
    const uintptr_t soldier = reinterpret_cast<uintptr_t>(self);
    const bool look = (g_hitScanIni || g_ragdollIni) && !g_disabled && g_lockReady && ReadU32(soldier) == g_vtSoldier;
    int32_t before = -1;
    Vec3 v0 = { 0.0f, 0.0f, 0.0f };
    if (look)
    {
        before = ReadI32(soldier + kState);
        v0 = ReadVec3(soldier + kVelocity);
    }
    const uint32_t ret = g_origPush(self, edx, impulse, scale);
    if (!look)
        return ret;
    EnterCriticalSection(&g_lock);
    ++g_pushCount;
    const Vec3 pos = SoldierPos(soldier);
    const int32_t after = ReadI32(soldier + kState);
    const Vec3 v1 = ReadVec3(soldier + kVelocity);
    Vec3 imp = { 0.0f, 0.0f, 0.0f };
    if (Readable(impulse, 12))
        imp = ReadVec3(impulse);
    if (g_ragdollIni)
        RagdollPush(soldier, after, pos, v0, v1, imp);
    if (g_hitScanIni && HitScanWanted(soldier, pos))
    {
        float s = 0.0f;
        memcpy(&s, &scale, sizeof(s));
        const int p = PlayerIndex(soldier);
        if (after == 5)
            ++g_thrownCount;
        if (g_pushLines < kMaxPushLines)
        {
            ++g_pushLines;
            char above[24];
            AboveText(pos, above, sizeof(above));
            Log("[hit] %lu ms: PUSH %s soldier 0x%08X: impulse (%.2f %.2f %.2f) x %.3f, velocity (%.2f %.2f %.2f) -> "
                "(%.2f %.2f %.2f), state %d %s -> %d %s, %s m above the terrain, from rva 0x%08X",
                ScanMsSinceAttach(), WhoText(p), static_cast<unsigned>(soldier), static_cast<double>(imp.x),
                static_cast<double>(imp.y), static_cast<double>(imp.z), static_cast<double>(s),
                static_cast<double>(v0.x), static_cast<double>(v0.y), static_cast<double>(v0.z),
                static_cast<double>(v1.x), static_cast<double>(v1.y), static_cast<double>(v1.z),
                static_cast<int>(before), StateName(before), static_cast<int>(after), StateName(after), above,
                static_cast<unsigned>(RvaOf(caller)));
        }
        if (after != before || Length3(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z) > 1.0f)
            StartThrown(soldier, p);
    }
    LeaveCriticalSection(&g_lock);
    return ret;
}

uint32_t __fastcall Hook_Damage(void* self, void* edx, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4,
                                uint32_t a5)
{
    const uintptr_t caller = BF1_RETURN_ADDRESS();
    const uintptr_t soldier = reinterpret_cast<uintptr_t>(self) - kSubDamage;
    const bool look = (g_hitScanIni || g_ragdollIni) && !g_disabled && g_lockReady && ReadU32(soldier) == g_vtSoldier;
    int32_t before = -1;
    if (look)
        before = ReadI32(soldier + kState);
    const uint32_t ret = g_origDamage(self, edx, a1, a2, a3, a4, a5);
    if (!look)
        return ret;
    EnterCriticalSection(&g_lock);
    ++g_damageCount;
    if (g_trackOn)
    {
        // 15i: what hit him (arg 1 = the damaging object, arg 4 = the zone)
        DamageRec& d = g_dmg[g_dmgNext];
        g_dmgNext = (g_dmgNext + 1) % 64;
        d.soldier = soldier;
        d.tick = GetTickCount();
        d.zone = static_cast<int>(static_cast<int32_t>(a4));
        d.vt = ((a1 & 3) == 0 && Readable(a1, 4)) ? ReadU32(a1) : 0;
    }
    const Vec3 pos = SoldierPos(soldier);
    // 15f: keep the hit (arg 2 = direction, arg 3 = hit point, arg 4 = zone)
    if (g_ragdollIni && (a2 & 3) == 0 && (a3 & 3) == 0 && Readable(a2, 12) && Readable(a3, 12))
    {
        const Vec3 dir = ReadVec3(a2);
        const Vec3 at = ReadVec3(a3);
        const float len = Length3(dir.x, dir.y, dir.z);
        if (std::isfinite(len) && len > 0.5f && len < 1.5f && std::isfinite(at.x) && std::isfinite(at.y) &&
            std::isfinite(at.z) && Dist(at, pos) < 4.0f)
        {
            HitRec& rec = g_hitRecs[g_hitNext];
            g_hitNext = (g_hitNext + 1) % kRecs;
            rec.soldier = soldier;
            rec.tick = GetTickCount();
            rec.pos[0] = at.x;
            rec.pos[1] = at.y;
            rec.pos[2] = at.z;
            rec.dir[0] = dir.x / len;
            rec.dir[1] = dir.y / len;
            rec.dir[2] = dir.z / len;
            rec.zone = static_cast<int>(static_cast<int32_t>(a4));
            Active* act = FindActive(soldier);
            if (act && g_corpseHit > 0.0f)
            {
                const char* part = ragdoll::Hit(act->handle, rec.pos, rec.dir, g_corpseHit);
                if (part && RdLog())
                    Log("[ragdoll] %lu ms: ragdoll #%d (%s) shot again: %.0f N s on the %s", ScanMsSinceAttach(),
                        act->id, WhoText(act->player), static_cast<double>(g_corpseHit), part);
            }
        }
    }
    if (g_hitScanIni && HitScanWanted(soldier, pos) && g_damageLines < kMaxDamageLines)
    {
        ++g_damageLines;
        const uint32_t args[5] = { a1, a2, a3, a4, a5 };
        char text[5][96];
        for (int i = 0; i < 5; ++i)
            ArgText(args[i], text[i], sizeof(text[i]));
        // his head and pelvis now (if his controller is his): to compare with a hit position
        char bones[160] = "";
        const uintptr_t ctl = ReadU32(soldier + kCtl);
        if (ctl && Readable(ctl, kCtlBytes) && ReadU32(ctl + kCtlOwner) == soldier)
        {
            SkelInfo* s = FindSkel(ReadU32(ctl + kCtlInst));
            if (s && s->key[kHead] >= 0 && s->key[kHead] < static_cast<int>(kCtlMaxBones) && s->key[kPelvis] >= 0 &&
                s->key[kPelvis] < static_cast<int>(kCtlMaxBones))
            {
                float w[16];
                ReadMatrix(soldier + kSoldierMatrix, w);
                const Vec3 head = ToWorld(MatrixPos(BoneMatrix(ctl, s->key[kHead])), w);
                const Vec3 pel = ToWorld(MatrixPos(BoneMatrix(ctl, s->key[kPelvis])), w);
                snprintf(bones, sizeof(bones), ", head (%.2f %.2f %.2f), pelvis (%.2f %.2f %.2f)",
                         static_cast<double>(head.x), static_cast<double>(head.y), static_cast<double>(head.z),
                         static_cast<double>(pel.x), static_cast<double>(pel.y), static_cast<double>(pel.z));
            }
        }
        Log("[hit] %lu ms: DAMAGE %s soldier 0x%08X at (%.2f %.2f %.2f)%s, state %d %s -> %d %s, returned %u, from "
            "rva 0x%08X",
            ScanMsSinceAttach(), WhoText(PlayerIndex(soldier)), static_cast<unsigned>(soldier),
            static_cast<double>(pos.x), static_cast<double>(pos.y), static_cast<double>(pos.z), bones,
            static_cast<int>(before), StateName(before), static_cast<int>(ReadI32(soldier + kState)),
            StateName(ReadI32(soldier + kState)), static_cast<unsigned>(ret & 0xFF),
            static_cast<unsigned>(RvaOf(caller)));
        Log("[hit]   args: 1 %s | 2 %s | 3 %s | 4 %s | 5 %s", text[0], text[1], text[2], text[3], text[4]);
    }
    LeaveCriticalSection(&g_lock);
    return ret;
}

// Pose hook (under the lock): thrown soldiers' states and animation.
void ThrownFrame(uintptr_t soldier, uintptr_t ctl, int32_t state, uint32_t frame)
{
    Thrown* t = FindThrown(soldier);
    if (!t || t->lastFrame == frame)
        return;
    t->lastFrame = frame;
    const uint32_t move = ReadU32(ctl + kCtlMove);
    if ((state != t->lastState || move != t->lastMove) && g_thrownLines < kMaxThrownLines)
    {
        ++g_thrownLines;
        const Vec3 pos = SoldierPos(soldier);
        const Vec3 v = ReadVec3(soldier + kVelocity);
        char above[24];
        AboveText(pos, above, sizeof(above));
        Log("[hit] thrown #%d (%s) %.2f s: state %d %s (%.2f s in it), controller state %d, move %u, velocity "
            "(%.2f %.2f %.2f), %s m above the terrain",
            t->id, WhoText(t->player), static_cast<double>(GetTickCount() - t->tick0) / 1000.0,
            static_cast<int>(state), StateName(state), static_cast<double>(ReadF32(soldier + kStateTime)),
            static_cast<int>(ReadI32(ctl + kCtlState)), static_cast<unsigned>(move), static_cast<double>(v.x),
            static_cast<double>(v.y), static_cast<double>(v.z), above);
    }
    t->lastState = state;
    t->lastMove = move;
}

void AgeThrown(DWORD now)
{
    for (Thrown& t : g_thrown)
    {
        if (t.used && now - t.tick0 > kThrownTrackMs)
        {
            t.used = false;
            if (g_thrownLines < kMaxThrownLines)
            {
                ++g_thrownLines;
                Log("[hit] thrown #%d: 8 s - no longer tracked%s", t.id, t.died ? " (died)" : "");
            }
        }
    }
}

// 15f seat scan (log-only): soldiers drawn while seated (states 12 / 13:
// turrets, vehicles) near a player, once each, and their deaths.
constexpr int kMaxSeatLines = 80;
uintptr_t g_seatSeen[32];
int g_seatNext = 0;
int g_seatLines = 0;

void SeatFrame(uintptr_t soldier, int32_t state)
{
    if ((state != 12 && state != 13) || g_seatLines >= kMaxSeatLines)
        return;
    for (uintptr_t s : g_seatSeen)
        if (s == soldier)
            return;
    const Vec3 pos = SoldierPos(soldier);
    const float d = NearestPlayer(soldier, pos);
    if (d < 0.0f || d > g_rdRange)
        return;
    g_seatSeen[g_seatNext] = soldier;
    g_seatNext = (g_seatNext + 1) % 32;
    ++g_seatLines;
    char above[24];
    AboveText(pos, above, sizeof(above));
    Log("[seat] %lu ms: %s soldier 0x%08X DRAWN while seated (state %d), %.1f m from a player, at (%.2f %.2f %.2f) %s m "
        "above the terrain",
        ScanMsSinceAttach(), WhoText(PlayerIndex(soldier)), static_cast<unsigned>(soldier), static_cast<int>(state),
        static_cast<double>(d), static_cast<double>(pos.x), static_cast<double>(pos.y), static_cast<double>(pos.z),
        above);
}

void SeatDeath(uintptr_t soldier, int32_t before, int32_t after)
{
    if ((before != 12 && before != 13) || g_seatLines >= kMaxSeatLines)
        return;
    ++g_seatLines;
    const HitRec* hit = RecentHit(soldier, GetTickCount());
    const PushRec* push = RecentPush(soldier, GetTickCount());
    Log("[seat] %lu ms: %s soldier 0x%08X DIED SEATED (state %d -> %d): last hit %s, last blast %s", ScanMsSinceAttach(),
        WhoText(PlayerIndex(soldier)), static_cast<unsigned>(soldier), static_cast<int>(before),
        static_cast<int>(after), hit ? (hit->zone == 2 ? "HEAD" : (hit->zone == 1 ? "body" : "zone -1")) : "none",
        push ? "yes" : "none");
}

void ThrownDeath(uintptr_t soldier, int32_t before, int32_t after)
{
    Thrown* t = FindThrown(soldier);
    if (!t)
        return;
    t->died = true;
    if (g_thrownLines < kMaxThrownLines)
    {
        ++g_thrownLines;
        Log("[hit] thrown #%d DIED %.2f s after the push: state %d %s -> %d %s", t->id,
            static_cast<double>(GetTickCount() - t->tick0) / 1000.0, static_cast<int>(before), StateName(before),
            static_cast<int>(after), StateName(after));
    }
}

// ---------------------------------------------------------------------------
// Frames and the 10 s summary
// ---------------------------------------------------------------------------
void SeenSoldier(uintptr_t soldier)
{
    const uint32_t mask = static_cast<uint32_t>(sizeof(g_seen) / sizeof(g_seen[0])) - 1;
    uint32_t i = static_cast<uint32_t>(soldier >> 4) & mask;
    for (uint32_t n = 0; n <= mask; ++n)
    {
        if (g_seen[i] == soldier)
            return;
        if (g_seen[i] == 0)
        {
            g_seen[i] = soldier;
            ++g_seenCount;
            return;
        }
        i = (i + 1) & mask;
    }
}

void Summary(DWORD now)
{
    const double secs = static_cast<double>(now - g_win.start) / 1000.0;
    const double frames = g_win.frames ? static_cast<double>(g_win.frames) : 1.0;
    Log("[ragdoll] %lu ms, last %.1f s: %u frames, %u pose updates (%.1f per frame), %u soldiers drawn (%u updates "
        "of dead ones), hook cost %.4f ms per frame (max %.3f), deaths %u, removed %u dead / %u alive, frame time "
        "%.4f s",
        ScanMsSinceAttach(), secs, static_cast<unsigned>(g_win.frames), static_cast<unsigned>(g_win.calls),
        static_cast<double>(g_win.calls) / frames, static_cast<unsigned>(g_seenCount),
        static_cast<unsigned>(g_win.deadCalls), g_win.costMs / frames, g_win.maxFrameMs,
        static_cast<unsigned>(g_win.deaths), static_cast<unsigned>(g_win.removedDead),
        static_cast<unsigned>(g_win.removedAlive), static_cast<double>(g_dtVa ? ReadF32(g_dtVa) : 0.0f));
    if (g_scanIni && g_totalRemovedDead)
        Log("[ragdoll]   so far: %u deaths, %u dead bodies removed, %.2f - %.2f s after death (average %.2f)",
            static_cast<unsigned>(g_totalDeaths), static_cast<unsigned>(g_totalRemovedDead),
            static_cast<double>(g_removedMin), static_cast<double>(g_removedMax),
            g_removedSum / static_cast<double>(g_totalRemovedDead));
    if (g_ragdollIni)
        RagdollSummary(secs);
    if (g_trackOn)
        TrackSummary(); // 15i: 60 s totals
    if (g_hitScanIni)
        Log("[hit] so far: %u pushes (%u threw a soldier), %u damage calls (all soldiers)",
            static_cast<unsigned>(g_pushCount), static_cast<unsigned>(g_thrownCount),
            static_cast<unsigned>(g_damageCount));
    memset(&g_win, 0, sizeof(g_win));
    g_win.start = now;
    memset(g_seen, 0, sizeof(g_seen));
    g_seenCount = 0;
}

void NewFrame(uint32_t frame)
{
    if (g_frameSeen)
    {
        ++g_win.frames;
        if (g_frameCostMs > g_win.maxFrameMs)
            g_win.maxFrameMs = g_frameCostMs;
    }
    if (g_frameSeen && g_trackOn)
        TrackNewFrame(); // 15i
    g_frameSeen = true;
    g_frame = frame;
    g_frameCostMs = 0.0;
    // 15c: the players' soldiers every 15 frames (15b did it every frame:
    // most of its 0.25 ms per frame were these memory checks)
    if (++g_playerFrames >= kPlayerRefreshFrames)
    {
        g_playerFrames = 0;
        for (int k = 0; k < kLocalMax; ++k) // 20g-r6: players 1-4
            g_player[k] = PlayerSoldier(k);
    }
    const DWORD now = GetTickCount();
    if (g_scanIni)
        AgeBodies(now);
    if (g_ragdollIni)
        RagdollFrame(now);
    if (g_hitScanIni)
        AgeThrown(now);
    if (g_trackOn)
        TrackAge(now); // 15i
    if (!g_win.start)
        g_win.start = now;
    else if (now - g_win.start >= kStatsMs)
        Summary(now);
}

// ---------------------------------------------------------------------------
// The hooks
// ---------------------------------------------------------------------------
void OnPose(uintptr_t ctl, uintptr_t pose)
{
    LARGE_INTEGER q0;
    QueryPerformanceCounter(&q0);
    const uintptr_t soldier = ReadU32(ctl + kCtlOwner);

    EnterCriticalSection(&g_lock);
    if (!g_poseThread)
    {
        g_poseThread = GetCurrentThreadId();
        Log("[ragdoll] %lu ms: first pose update through the hook (thread %lu, controller 0x%08X, soldier 0x%08X)",
            ScanMsSinceAttach(), static_cast<unsigned long>(g_poseThread), static_cast<unsigned>(ctl),
            static_cast<unsigned>(soldier));
    }
    if (g_disabled)
    {
        LeaveCriticalSection(&g_lock);
        return;
    }
    if (g_checksLeft > 0)
    {
        if (!FullCheck(ctl, pose, soldier))
        {
            LeaveCriticalSection(&g_lock);
            return;
        }
        if (!g_layoutLogged && g_scanIni)
        {
            g_layoutLogged = true;
            LogLayout(ctl, pose, soldier);
        }
        if (--g_checksLeft == 0)
            Log("[ragdoll] %lu ms: the first %d pose updates passed every run-time check", ScanMsSinceAttach(),
                kCheckCalls);
    }
    const uint32_t frame = ReadU32(g_frameVa);
    if (!g_frameSeen || frame != g_frame)
        NewFrame(frame);
    ++g_win.calls;
    SeenSoldier(soldier);
    const int32_t state = ReadI32(soldier + kState);
    if (g_trackOn)
        TrackDrawn(soldier, frame, state, static_cast<uint32_t>(ctl)); // 15i: drawn in full detail (17c: by ctl)
    if (state == kStateDead)
        ++g_win.deadCalls;

    SkelInfo* s = SkeletonOf(ctl, pose, soldier);
    if (g_scanIni)
    {
        const int p = PlayerIndex(soldier);
        if (p >= 0)
            PlayerFrame(p, soldier, ctl, pose, state, s);
        BodyFrame(soldier, ctl, pose, state, frame, s);
    }
    if (g_ragdollIni)
        RagdollPose(soldier, ctl, state, s, frame);
    if (g_hitScanIni)
        ThrownFrame(soldier, ctl, state, frame);
    if (g_hitScanIni)
        SeatFrame(soldier, state);

    LARGE_INTEGER q1;
    QueryPerformanceCounter(&q1);
    const double ms = static_cast<double>(q1.QuadPart - q0.QuadPart) * g_qpcToMs;
    g_win.costMs += ms;
    g_frameCostMs += ms;
    LeaveCriticalSection(&g_lock);
}

void __fastcall Hook_CtlUpdate(void* ctl, void* edx, void* pose)
{
    {
        const FramePerfScope perf(kPerfPoses); // 21j-r11: the frame-rate scan (the game's own pose update)
        g_origUpdate(ctl, edx, pose); // the game rebuilds the bone matrices first
    }
    if (g_poseOn && !g_disabled)
    {
        const FramePerfScope perf(kPerfOurRagdoll); // 21j-r11
        OnPose(reinterpret_cast<uintptr_t>(ctl), reinterpret_cast<uintptr_t>(pose));
    }
}

void __fastcall Hook_Death(void* self, void* edx)
{
    const uintptr_t caller = BF1_RETURN_ADDRESS();
    const uintptr_t soldier = reinterpret_cast<uintptr_t>(self) - kSubDamage;
    const bool look = !g_disabled && Readable(soldier, kSoldierBytes) && ReadU32(soldier) == g_vtSoldier;
    int32_t before = -1;
    int32_t prev = -1;
    Vec3 pos = { 0.0f, 0.0f, 0.0f };
    Vec3 vel = { 0.0f, 0.0f, 0.0f };
    Who who;
    who.ch = -1;
    if (look)
    {
        before = ReadI32(soldier + kState);
        prev = ReadI32(soldier + kPrevState);
        pos = SoldierPos(soldier);
        vel = ReadVec3(soldier + kVelocity);
        if (g_trackOn && g_lockReady)
        {
            EnterCriticalSection(&g_lock);
            FindWho(soldier, who); // 15i: before the game's death code (it may let go of his character)
            if (Readable(soldier, kSoldierBytes))
            {
                memcpy(g_preSnap, reinterpret_cast<const void*>(soldier), kSoldierBytes); // 17e: the droid scan
                g_preSnapSoldier = soldier;
            }
            LeaveCriticalSection(&g_lock);
        }
    }

    g_origDeath(self, edx);

    if (!look)
        return;
    EnterCriticalSection(&g_lock);
    if (!g_deathThreadLogged)
    {
        g_deathThreadLogged = true;
        const DWORD t = GetCurrentThreadId();
        Log("[ragdoll] first death through the hook on thread %lu (%s the pose updates' thread)",
            static_cast<unsigned long>(t), g_poseThread == 0 ? "no pose update yet to compare with"
                                                              : (t == g_poseThread ? "same as" : "NOT"));
    }
    ++g_totalDeaths;
    ++g_win.deaths;
    const int32_t after = ReadI32(soldier + kState);
    // the players' soldiers are refreshed every few frames; check again now (20g-r6: players 1-4)
    const int p = PlayerIndexNow(soldier);
    float dist = -1.0f;
    for (int k = 0; k < kLocalMax; ++k)
    {
        if (!g_player[k] || g_player[k] == soldier)
            continue;
        const float d = Dist(pos, SoldierPos(g_player[k]));
        if (dist < 0.0f || d < dist)
            dist = d;
    }
    if (p >= 0)
        dist = 0.0f; // 15j: a local player's own death - his own camera watches his body
    char above[24];
    AboveText(pos, above, sizeof(above));
    if (g_scanIni && g_deathLines < kMaxDeathLines && Budget())
    {
        ++g_deathLines;
        char distText[32];
        if (dist >= 0.0f)
            snprintf(distText, sizeof(distText), "%.1f m", static_cast<double>(dist));
        else
            snprintf(distText, sizeof(distText), "n/a");
        Log("[ragdoll] %lu ms: DEATH of %s soldier 0x%08X: state %d %s -> %d %s (before that %d), at (%.2f %.2f "
            "%.2f) %s m above the terrain, speed %.2f m/s (y %+.2f), %s from the nearest player, called from rva "
            "0x%08X",
            ScanMsSinceAttach(), WhoText(p), static_cast<unsigned>(soldier), static_cast<int>(before),
            StateName(before), static_cast<int>(after), StateName(after), static_cast<int>(prev),
            static_cast<double>(pos.x), static_cast<double>(pos.y), static_cast<double>(pos.z), above,
            static_cast<double>(Length3(vel.x, vel.y, vel.z)), static_cast<double>(vel.y), distText,
            static_cast<unsigned>(RvaOf(caller)));
    }
    else if (g_scanIni && g_deathLines == kMaxDeathLines)
    {
        ++g_deathLines;
        Log("[ragdoll] death lines limit reached (deaths are still counted in the summaries)");
    }
    if (g_scanIni && (p >= 0 || (dist >= 0.0f && dist <= g_range)))
        StartBody(soldier, p);
    if (g_trackOn)
        TrackDeath(soldier, p, pos, vel, dist, before, after, who); // 15i
    if (before == 12 || before == 13)
    {
        // 17c: in a vehicle / turret seat - the body is part of the vehicle there (17b skipped these by
        // chance, "in the air"; they get their own reason now that deaths in the air get ragdolls)
        char seat[96];
        snprintf(seat, sizeof(seat), "died in a vehicle or turret seat (state %d)", static_cast<int>(before));
        if (g_ragdollIni && g_rdLiveOn && !FindPending(soldier) && !FindActive(soldier))
            Skip(kSkipSeated, soldier, p, seat);
    }
    else
        RagdollDeath(soldier, p, pos, vel, dist, after);
    if (g_hitScanIni)
        ThrownDeath(soldier, before, after);
    if (g_hitScanIni)
        SeatDeath(soldier, before, after);
    LeaveCriticalSection(&g_lock);
}

void* __fastcall Hook_Delete(void* self, void* edx, unsigned flags)
{
    const uintptr_t soldier = reinterpret_cast<uintptr_t>(self);
    if (!g_disabled && g_lockReady)
    {
        EnterCriticalSection(&g_lock);
        if (!g_deleteThreadLogged)
        {
            g_deleteThreadLogged = true;
            const DWORD t = GetCurrentThreadId();
            Log("[ragdoll] first soldier delete through the hook on thread %lu (%s the pose updates' thread)",
                static_cast<unsigned long>(t), g_poseThread == 0 ? "no pose update yet to compare with"
                                                                  : (t == g_poseThread ? "same as" : "NOT"));
        }
        const int32_t state = ReadI32(soldier + kState);
        const float timer = ReadF32(soldier + kStateTime);
        if (state == kStateDead)
        {
            ++g_win.removedDead;
            ++g_totalRemovedDead;
            g_removedSum += static_cast<double>(timer);
            if (timer < g_removedMin)
                g_removedMin = timer;
            if (timer > g_removedMax)
                g_removedMax = timer;
        }
        else
        {
            ++g_win.removedAlive;
        }
        if (g_trackOn)
            TrackDeleted(soldier); // 15i
        RagdollEndSoldier(soldier, "the soldier was deleted");
        Body* b = g_scanIni ? FindBody(soldier) : nullptr;
        if (b)
        {
            char why[96];
            snprintf(why, sizeof(why), "DELETED (state %d %s, %.2f s in that state, flags %u)",
                     static_cast<int>(state), StateName(state), static_cast<double>(timer), flags);
            EndBody(*b, why);
        }
        else if (g_scanIni && g_deleteLines < kMaxDeleteLines && Budget())
        {
            ++g_deleteLines;
            Log("[ragdoll] %lu ms: soldier 0x%08X deleted: state %d %s, %.2f s in that state", ScanMsSinceAttach(),
                static_cast<unsigned>(soldier), static_cast<int>(state), StateName(state),
                static_cast<double>(timer));
        }
        for (int k = 0; k < kLocalMax; ++k)
        {
            if (g_player[k] == soldier)
                g_player[k] = 0;
            if (g_playerStateSoldier[k] == soldier)
                g_playerStateSoldier[k] = 0;
        }
        LeaveCriticalSection(&g_lock);
    }
    return g_origDelete(self, edx, flags);
}

// ---------------------------------------------------------------------------
// Finding and checking the sites
// ---------------------------------------------------------------------------
struct Found
{
    uintptr_t vt0;
    uintptr_t vt8;
    uintptr_t vt17C;
    uintptr_t render;
    uintptr_t sites[2];
    uintptr_t update;
    uintptr_t death;
    uintptr_t del;
    bool poseOk;
    bool deathOk;
    bool deleteOk;
};

bool FindPoseSites(Found& f)
{
    if (!f.vt17C)
        return false;
    const uintptr_t slot = f.vt17C + kSlotRender * 4;
    f.render = FollowThunks(ReadU32(slot));
    if (!MatchVa(f.render, kRenderEntry))
    {
        Log("[ragdoll]   soldier render (+17Ch vtable slot 3 -> 0x%08X): does NOT match the render's start",
            static_cast<unsigned>(f.render));
        return false;
    }
    Log("[ragdoll]   soldier render: +17Ch vtable slot 3 -> rva 0x%08X, start (0x658 frame) OK",
        static_cast<unsigned>(RvaOf(f.render)));

    uint32_t hits[4] = {};
    const int n = FindPattern(kUpdateCall, hits, 4);
    if (n != 2)
    {
        Log("[ragdoll]   controller update calls: %d pattern hits (need exactly 2)", n);
        return false;
    }
    for (int i = 0; i < 2; ++i)
    {
        const uintptr_t site = Rva(hits[i]) + kUpdateCallE8;
        if (site < f.render || site >= f.render + kRenderWindow)
        {
            Log("[ragdoll]   controller update call %d at rva 0x%08X is NOT inside the render", i + 1,
                static_cast<unsigned>(RvaOf(site)));
            return false;
        }
        f.sites[i] = site;
    }
    const uintptr_t t0 = CallTargetVa(f.sites[0]);
    const uintptr_t t1 = CallTargetVa(f.sites[1]);
    if (!t0 || t0 != t1)
    {
        Log("[ragdoll]   the two calls lead to different code (0x%08X / 0x%08X)", static_cast<unsigned>(t0),
            static_cast<unsigned>(t1));
        return false;
    }
    if (!MatchVa(t0, kUpdateBody))
    {
        Log("[ragdoll]   call target rva 0x%08X does NOT match the controller update's start",
            static_cast<unsigned>(RvaOf(t0)));
        return false;
    }
    f.update = t0;
    Log("[ragdoll]   controller update calls at render+0x%03X / +0x%03X (rva 0x%08X / 0x%08X) -> update rva "
        "0x%08X, start OK (+1FFCh frame test, +1C20h copy)",
        static_cast<unsigned>(f.sites[0] - f.render), static_cast<unsigned>(f.sites[1] - f.render),
        static_cast<unsigned>(RvaOf(f.sites[0])), static_cast<unsigned>(RvaOf(f.sites[1])),
        static_cast<unsigned>(RvaOf(t0)));

    const uintptr_t dtFn = CallTargetVa(f.update + kUpdateDtCall);
    const uintptr_t frFn = CallTargetVa(f.update + kUpdateFrameCall);
    if (!MatchVa(dtFn, kFldGetter) || !MatchVa(frFn, kMovGetter))
    {
        Log("[ragdoll]   frame time / frame number getters do NOT match (0x%08X / 0x%08X)",
            static_cast<unsigned>(dtFn), static_cast<unsigned>(frFn));
        return false;
    }
    g_dtVa = ReadU32(dtFn + 2);
    g_frameVa = ReadU32(frFn + 1);
    if (!InExe(g_dtVa, 4) || !InExe(g_frameVa, 4))
    {
        Log("[ragdoll]   frame time / frame number addresses are outside the exe");
        return false;
    }
    Log("[ragdoll]   frame number at rva 0x%08X (now %u), frame time at rva 0x%08X (now %.4f)",
        static_cast<unsigned>(RvaOf(g_frameVa)), static_cast<unsigned>(ReadU32(g_frameVa)),
        static_cast<unsigned>(RvaOf(g_dtVa)), static_cast<double>(ReadF32(g_dtVa)));
    return true;
}

// 15g (log-only): the soldier render's weapon draw - it looks hp_weapons up in
// the pose the body was drawn with (so WeaponDrop's matrix is the one used).
// GOG 1.3: render+0x1541; Steam is checked in memory (same code).
void CheckWeaponDraw(const Found& f)
{
    if (!f.render)
    {
        Log("[ragdoll] weapon draw: no soldier render found - WeaponDrop cannot be checked");
        return;
    }
    uintptr_t site = 0;
    int hits = 0;
    for (uint32_t off = 0; off + 16 < kRenderWindow; ++off)
    {
        if (MatchVa(f.render + off, kWeaponDraw))
        {
            if (!site)
                site = f.render + off;
            ++hits;
        }
    }
    if (hits != 1)
    {
        Log("[ragdoll] weapon draw: the hp_weapons lookup was found %d times in the soldier render (want 1) - "
            "WeaponDrop may have no effect on this build",
            hits);
        return;
    }
    // the weapon slot read before it and the draw call after it
    bool slot = false;
    bool draw = false;
    for (uint32_t k = 1; k <= 0x30 && !slot; ++k)
        slot = MatchVa(site - k, "8B 87 E0 08 00 00");
    for (uint32_t k = 12; k <= 0x80 && !draw; ++k)
        draw = MatchVa(site + k, "FF 50 5C");
    Log("[ragdoll] weapon draw at render+0x%X (rva 0x%08X): looks hp_weapons up in the drawn pose (ebp-658h, the "
        "controller's matrices), weapon slot read [+8E0h] %s, draw call [vtable+5Ch] %s - WeaponDrop %s",
        static_cast<unsigned>(site - f.render), static_cast<unsigned>(RvaOf(site)), slot ? "OK" : "NOT found",
        draw ? "OK" : "NOT found", slot && draw ? "should work" : "may have no effect");
}

bool FindDeath(Found& f)
{
    if (!f.vt8)
        return false;
    f.death = FollowThunks(ReadU32(f.vt8 + kSlotDeath * 4));
    if (!MatchVa(f.death, kDeathBody) || !Readable(f.death, kDeathWindow))
    {
        Log("[ragdoll]   death function (+8 vtable slot 1 -> 0x%08X) does NOT match", static_cast<unsigned>(f.death));
        return false;
    }
    uint32_t off = 0;
    const int n = CountInWindow(f.death, kDeathWindow, kDeathSetDead, &off);
    if (n != 1)
    {
        Log("[ragdoll]   death function: \"state != 5 -> SetState(3)\" found %d times (need 1)", n);
        return false;
    }
    Log("[ragdoll]   death function: +8 vtable slot 1 -> rva 0x%08X, start OK, SetState(3) at +0x%03X",
        static_cast<unsigned>(RvaOf(f.death)), static_cast<unsigned>(off + 9));
    return true;
}

bool FindDelete(Found& f)
{
    if (!f.vt0 || !f.vt8)
        return false;
    f.del = FollowThunks(ReadU32(f.vt0 + kSlotDelete * 4));
    if (!MatchVa(f.del, kDeleteBody))
    {
        Log("[ragdoll]   soldier delete (primary vtable slot 11 -> 0x%08X) does NOT match",
            static_cast<unsigned>(f.del));
        return false;
    }
    const uintptr_t dtor = CallTargetVa(f.del + kDeleteInnerCall);
    if (!MatchVa(dtor, kDtorStart) || ReadU32(dtor + kDtorVt0) != static_cast<uint32_t>(f.vt0) ||
        ReadU32(dtor + kDtorVt8) != static_cast<uint32_t>(f.vt8))
    {
        Log("[ragdoll]   soldier delete: its destructor (0x%08X) does not write EntitySoldier's vtables",
            static_cast<unsigned>(dtor));
        return false;
    }
    Log("[ragdoll]   soldier delete: primary vtable slot 11 -> rva 0x%08X, destructor rva 0x%08X writes "
        "EntitySoldier's vtables: OK",
        static_cast<unsigned>(RvaOf(f.del)), static_cast<unsigned>(RvaOf(dtor)));
    return true;
}

void FindTerrain()
{
    uint32_t hits[4] = {};
    const int n = FindPattern(kTerrainHeight, hits, 4);
    if (n != 1)
    {
        Log("[ragdoll]   terrain height: %d pattern hits (need 1) - no terrain heights in the log", n);
        return;
    }
    g_terrainHeight = reinterpret_cast<TerrainHeight_t>(Rva(hits[0]));
    Log("[ragdoll]   terrain height: rva 0x%08X (1 pattern hit)", static_cast<unsigned>(hits[0]));
}

// 15d: the push and damage slots (log-only hooks).
uintptr_t FindPush(const Found& f)
{
    if (!f.vt0)
        return 0;
    const uintptr_t target = FollowThunks(ReadU32(f.vt0 + kSlotPush * 4));
    if (!MatchVa(target, kPushBody) || !Readable(target, kPushWindow))
    {
        Log("[hit]   push (primary vtable slot 32 -> 0x%08X) does NOT match", static_cast<unsigned>(target));
        return 0;
    }
    const int s8 = CountInWindow(target, kPushWindow, kPushSetState8, nullptr);
    const int r8 = CountInWindow(target, kPushWindow, "C2 08 00", nullptr);
    if (s8 != 1 || r8 != 2)
    {
        Log("[hit]   push: SetState(8) x%d, ret 8 x%d (need 1 and 2) - not hooked", s8, r8);
        return 0;
    }
    Log("[hit]   push: primary vtable slot 32 -> rva 0x%08X, start OK, SetState(8) x1, ret 8 x2",
        static_cast<unsigned>(RvaOf(target)));
    return f.vt0 + kSlotPush * 4;
}

uintptr_t FindDamage(const Found& f)
{
    if (!f.vt8)
        return 0;
    const uintptr_t target = FollowThunks(ReadU32(f.vt8 + kSlotDamage * 4));
    if (!MatchVa(target, kDamageBody) || !Readable(target, kDamageWindow))
    {
        Log("[hit]   damage (+8 vtable slot 5 -> 0x%08X) does NOT match", static_cast<unsigned>(target));
        return 0;
    }
    const int r14 = CountInWindow(target, kDamageWindow, "C2 14 00", nullptr);
    if (r14 < 1 || r14 > 2)
    {
        Log("[hit]   damage: ret 14h x%d (need 1 or 2) - not hooked", r14);
        return 0;
    }
    Log("[hit]   damage: +8 vtable slot 5 -> rva 0x%08X, start OK, ret 14h x%d", static_cast<unsigned>(RvaOf(target)),
        r14);
    return f.vt8 + kSlotDamage * 4;
}

// ---------------------------------------------------------------------------
// Installing
// ---------------------------------------------------------------------------
bool PatchU32(uintptr_t at, uint32_t value, DWORD prot)
{
    DWORD oldProt = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(at), 4, prot, &oldProt))
        return false;
    memcpy(reinterpret_cast<void*>(at), &value, sizeof(value));
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(at), 4, oldProt, &ignored);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<LPCVOID>(at), 4);
    return true;
}

bool InstallPose(const Found& f)
{
    g_origUpdate = reinterpret_cast<CtlUpdate_t>(f.update);
    const uintptr_t hook = reinterpret_cast<uintptr_t>(&Hook_CtlUpdate);
    for (int i = 0; i < 2; ++i)
    {
        const uintptr_t site = f.sites[i];
        const uint32_t rel = static_cast<uint32_t>(hook - (site + 5));
        if (!PatchU32(site + 1, rel, PAGE_EXECUTE_READWRITE))
        {
            Log("[ragdoll] VirtualProtect on update call %d failed (error %lu)%s", i + 1, GetLastError(),
                i ? " - call 1 already goes through BF1GameExt (log-only, harmless)" : "");
            return i > 0;
        }
    }
    Log("[ragdoll] pose hook ON: both update calls now call BF1GameExt (0x%08X), which calls the game's update "
        "first",
        static_cast<unsigned>(hook));
    return true;
}

bool InstallSlot(uintptr_t slot, uintptr_t hook, const char* what)
{
    if (!PatchU32(slot, static_cast<uint32_t>(hook), PAGE_READWRITE))
    {
        Log("[ragdoll] VirtualProtect on the %s vtable slot failed (error %lu) - not hooked", what, GetLastError());
        return false;
    }
    Log("[ragdoll] %s hook ON: vtable slot rva 0x%08X -> BF1GameExt (0x%08X)", what,
        static_cast<unsigned>(RvaOf(slot)), static_cast<unsigned>(hook));
    return true;
}

// ---------------------------------------------------------------------------
// 17a: RAY SCAN (log only; INI [Ragdoll] RayScan). The game's world ray test -
// found on GOG 1.3 through the bullets: OrdnanceBullet / Ordnance update ->
// rva 0x9FCF0, 61 calls from 31 functions (camera, explosions, AI, buildings)
// all through one jump stub (rva 0x39B3), nothing else points at it. Cdecl,
// 9 arguments, returns a float: from its code - arg 1 a point, arg 2 a vector,
// arg 3 a length (0: returns 1 at once), arg 4 an output (zeroed first), arg 8
// a mask (0 = 0x5E), the rest passed on to CollisionManager (its instance in a
// global); returns 1.0 when nothing was hit. 17a redirects the stub to a
// pass-through that logs, per calling place, the first 3 calls in full (the
// point, the vector and its length, the mask, the other arguments, the result,
// the outputs, the hit point and its height above the terrain, the C++ class
// of any object involved) and every 20 s the busiest places. What it finds
// decides how 17b asks the game where the floor is under a ragdoll (platforms,
// walkways, interiors: Bespin, Cloud City).
// ---------------------------------------------------------------------------
const char* const kRayPattern =
    "55 8B EC F3 0F 10 45 10 0F 2E 05 ?? ?? ?? ?? 8B 55 14 C7 45 14 00 00 80 3F C7 02 00 00 00 00 9F F6 C4 44 7A 04 "
    "D9 E8 5D C3 8B 0D ?? ?? ?? ?? 85 C9 74 38 56 FF 75 28 8D 81 C8 00 00 00 BE 5E 00 00 00 50 8B 45 24 85 C0";
constexpr int kRayCallers = 128;
constexpr int kRayDetail = 3;
constexpr int kRayMaxLines = 700;
constexpr DWORD kRaySummaryMs = 20000;

struct RayCaller
{
    uint32_t rva;
    uint32_t calls;
    uint32_t hits;
    uint32_t windowCalls;
    int detailed;
    uint32_t masks[4];
    int maskCount;
};

Ray_t g_origRay = nullptr;
bool g_rayOn = false;
CRITICAL_SECTION g_rayLock;
RayCaller g_rayCallers[kRayCallers];
int g_rayCallerCount = 0;
int g_rayLines = 0;
uint32_t g_rayTotal = 0;
uint32_t g_rayWindowTotal = 0;
DWORD g_raySummaryTick = 0;

bool RayLog()
{
    if (g_rayLines >= kRayMaxLines)
        return false;
    if (++g_rayLines == kRayMaxLines)
    {
        Log("[ray] line limit reached: only the 20 s summaries from now on");
        return false;
    }
    return true;
}

// the C++ class of the object at p (its vtable's RTTI), or "-"
const char* ObjClass(uint32_t p)
{
    if (!p || (p & 3) || !Readable(p, 4))
        return "-";
    const uint32_t vt = ReadU32(p);
    if (!InExe(vt, 4))
        return "not an object";
    return RttiName(vt);
}

bool ReadVec(uint32_t p, float* v)
{
    if (!p || (p & 3) || !Readable(p, 12))
        return false;
    memcpy(v, reinterpret_cast<const void*>(static_cast<uintptr_t>(p)), 12);
    return std::isfinite(v[0]) && std::isfinite(v[1]) && std::isfinite(v[2]);
}

RayCaller* RayCallerAt(uint32_t rva)
{
    for (int i = 0; i < g_rayCallerCount; ++i)
        if (g_rayCallers[i].rva == rva)
            return &g_rayCallers[i];
    if (g_rayCallerCount >= kRayCallers)
        return nullptr;
    RayCaller& c = g_rayCallers[g_rayCallerCount++];
    memset(&c, 0, sizeof(c));
    c.rva = rva;
    return &c;
}

void RaySummary(DWORD now)
{
    g_raySummaryTick = now;
    if (!g_rayWindowTotal)
        return;
    // the 8 busiest places this window
    int order[kRayCallers];
    for (int i = 0; i < g_rayCallerCount; ++i)
        order[i] = i;
    for (int i = 0; i < g_rayCallerCount; ++i)
        for (int j = i + 1; j < g_rayCallerCount; ++j)
            if (g_rayCallers[order[j]].windowCalls > g_rayCallers[order[i]].windowCalls)
            {
                const int t = order[i];
                order[i] = order[j];
                order[j] = t;
            }
    char top[700];
    size_t n = 0;
    top[0] = 0;
    for (int k = 0; k < g_rayCallerCount && k < 8; ++k)
    {
        const RayCaller& c = g_rayCallers[order[k]];
        if (!c.windowCalls)
            break;
        char masks[48] = "";
        size_t m = 0;
        for (int i = 0; i < c.maskCount; ++i)
            m += static_cast<size_t>(snprintf(masks + m, sizeof(masks) - m, "%s0x%X", i ? "/" : "", c.masks[i]));
        n += static_cast<size_t>(snprintf(top + n, sizeof(top) - n, "%srva 0x%06X %u (%u%% of all its calls hit, mask %s)",
                                          k ? ", " : "", static_cast<unsigned>(c.rva),
                                          static_cast<unsigned>(c.windowCalls),
                                          static_cast<unsigned>(c.calls ? 100u * c.hits / c.calls : 0u), masks));
    }
    Log("[ray] last %.0f s: %u ray tests (%u in all, from %d places so far); busiest: %s",
        static_cast<double>(kRaySummaryMs) / 1000.0, static_cast<unsigned>(g_rayWindowTotal),
        static_cast<unsigned>(g_rayTotal), g_rayCallerCount, top);
    for (int i = 0; i < g_rayCallerCount; ++i)
        g_rayCallers[i].windowCalls = 0;
    g_rayWindowTotal = 0;
}

float __cdecl Hook_Ray(uint32_t a1, uint32_t a2, float len, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7,
                       uint32_t a8, uint32_t a9)
{
    const uintptr_t ret = BF1_RETURN_ADDRESS();
    bool detail = false;
    RayCaller* c = nullptr;
    int nth = 0;
    EnterCriticalSection(&g_rayLock);
    c = RayCallerAt(RvaOf(ret));
    ++g_rayTotal;
    ++g_rayWindowTotal;
    if (c)
    {
        ++c->calls;
        ++c->windowCalls;
        bool known = false;
        for (int i = 0; i < c->maskCount; ++i)
            known = known || c->masks[i] == a8;
        if (!known && c->maskCount < 4)
            c->masks[c->maskCount++] = a8;
        if (c->detailed < kRayDetail && g_rayLines < kRayMaxLines)
        {
            detail = true;
            nth = ++c->detailed;
        }
    }
    const DWORD now = GetTickCount();
    if (now - g_raySummaryTick >= kRaySummaryMs)
        RaySummary(now);
    LeaveCriticalSection(&g_rayLock);
    float s[3] = { 0, 0, 0 };
    float d[3] = { 0, 0, 0 };
    const bool haveS = detail && ReadVec(a1, s);
    const bool haveD = detail && ReadVec(a2, d);
    const float r = g_origRay(a1, a2, len, a4, a5, a6, a7, a8, a9);
    if (c && r < 1.0f)
        InterlockedIncrement(reinterpret_cast<volatile LONG*>(&c->hits));
    if (!detail)
        return r;
    const uint32_t out4 = (a4 && !(a4 & 3) && Readable(a4, 4)) ? ReadU32(a4) : 0xFFFFFFFFu;
    float o5[3] = { 0, 0, 0 };
    const bool haveO5 = ReadVec(a5, o5);
    char hitText[120] = "no hit";
    if (haveS && haveD && r < 1.0f)
    {
        const float dl = std::sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
        float p[3];
        if (dl > 0.98f && dl < 1.02f)
            for (int k = 0; k < 3; ++k)
                p[k] = s[k] + d[k] * len * r; // a direction
        else
            for (int k = 0; k < 3; ++k)
                p[k] = s[k] + (d[k] - s[k]) * r; // an end point?
        float h = 0.0f;
        char above[40] = "no terrain height there";
        if (Ground(p[0], p[2], &h))
            snprintf(above, sizeof(above), "%+.2f m from the terrain", static_cast<double>(p[1] - h));
        snprintf(hitText, sizeof(hitText), "hit point (%.2f %.2f %.2f) if arg 2 is %s, %s", static_cast<double>(p[0]),
                 static_cast<double>(p[1]), static_cast<double>(p[2]),
                 (dl > 0.98f && dl < 1.02f) ? "a direction" : "an end point", above);
    }
    EnterCriticalSection(&g_rayLock);
    if (RayLog())
    {
        char sText[64] = "unreadable";
        char dText[80] = "unreadable";
        if (haveS)
            snprintf(sText, sizeof(sText), "(%.2f %.2f %.2f)", static_cast<double>(s[0]), static_cast<double>(s[1]),
                     static_cast<double>(s[2]));
        if (haveD)
            snprintf(dText, sizeof(dText), "(%.3f %.3f %.3f) length %.3f", static_cast<double>(d[0]),
                     static_cast<double>(d[1]), static_cast<double>(d[2]),
                     static_cast<double>(std::sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2])));
        Log("[ray] rva 0x%06X #%d: arg1 %s, arg2 %s, arg3 %.2f, mask 0x%X, arg7 %u, arg9 0x%08X, arg6 0x%08X (%s) -> "
            "%.4f; out4 0x%08X (%s), out5 %s%.2f %.2f %.2f%s; %s",
            static_cast<unsigned>(RvaOf(ret)), nth, sText, dText, static_cast<double>(len), static_cast<unsigned>(a8),
            static_cast<unsigned>(a7), static_cast<unsigned>(a9), static_cast<unsigned>(a6), ObjClass(a6),
            static_cast<double>(r), static_cast<unsigned>(out4), out4 == 0xFFFFFFFFu ? "-" : ObjClass(out4),
            haveO5 ? "(" : "unreadable (", static_cast<double>(o5[0]), static_cast<double>(o5[1]),
            static_cast<double>(o5[2]), ")", hitText);
    }
    LeaveCriticalSection(&g_rayLock);
    return r;
}

// ---------------------------------------------------------------------------
// 17c: WALL SCAN (log only; INI [Ragdoll] WallScan). Ragdolls only know floors
// (17b); walls, railings and props need the game's real collision shapes. The
// exe names them (RTTI): CollisionManager, CollisionObject / PropCollisionObject,
// CollisionModel, CollisionBody(Mesh / Primitive), CollisionMesh,
// CollisionPrimitive. Every object a floor ray hits (EntityProp on Bespin) is
// noted (at most 2 of each class, 24 in all) and, at the next 10 s summary
// (never inside a build), walked: the C++ class behind every pointer in its
// first 0x400 bytes, two levels down into anything collision / model / class
// shaped, lists of such objects, a rigid matrix near the hit (its place), and
// the raw numbers of every primitive / body (their sizes and matrices). What it
// finds decides how 17d builds walls for the ragdolls.
// ---------------------------------------------------------------------------
constexpr int kWallProps = 24;
constexpr int kWallMaxLines = 900;
constexpr int kWallVisited = 96;

uint32_t g_wallQueue[kWallProps];
int g_wallQueued = 0;
int g_wallProbed = 0;
int g_wallLines = 0;
struct WallClassSeen
{
    uint32_t vt;
    int count;
};
WallClassSeen g_wallSeen[48];
int g_wallSeenCount = 0;

bool WallLog()
{
    if (g_wallLines >= kWallMaxLines)
        return false;
    if (++g_wallLines == kWallMaxLines)
    {
        Log("[walls] line limit reached: the wall scan goes quiet");
        return false;
    }
    return true;
}

void NoteWallProp(uint32_t obj)
{
    if (!g_wallScanIni || g_wallProbed + g_wallQueued >= kWallProps || !obj || (obj & 3) || !Readable(obj, 4))
        return;
    for (int i = 0; i < g_wallQueued; ++i)
        if (g_wallQueue[i] == obj)
            return;
    const uint32_t vt = ReadU32(obj);
    WallClassSeen* seen = nullptr;
    for (int i = 0; i < g_wallSeenCount; ++i)
        if (g_wallSeen[i].vt == vt)
            seen = &g_wallSeen[i];
    if (!seen)
    {
        if (g_wallSeenCount >= 48)
            return;
        seen = &g_wallSeen[g_wallSeenCount++];
        seen->vt = vt;
        seen->count = 0;
    }
    if (seen->count >= 2)
        return;
    ++seen->count;
    g_wallQueue[g_wallQueued++] = obj;
}

bool Interesting(const char* cls)
{
    return strstr(cls, "Collision") || strstr(cls, "Model") || strstr(cls, "Mesh") || strstr(cls, "Primitive") ||
           strstr(cls, "Body") || strstr(cls, "Class");
}

bool IsObjName(const char* cls)
{
    // a real RTTI class name ("vtable rva ..." = no RTTI: treated like data, so lists are still found)
    return strcmp(cls, "-") && strcmp(cls, "not an object") && strncmp(cls, "vtable rva", 10);
}

// the first 0x50 bytes as dwords / floats (a primitive's or body's size and matrix)
void WallRaw(const char* path, uint32_t obj)
{
    if (!Readable(obj, 0x50) || !WallLog())
        return;
    char line[700];
    size_t n = 0;
    for (uint32_t off = 4; off < 0x50 && n < sizeof(line) - 40; off += 4)
    {
        const uint32_t u = ReadU32(obj + off);
        float f = 0.0f;
        memcpy(&f, &u, 4);
        if (std::isfinite(f) && std::fabs(f) > 1e-4f && std::fabs(f) < 1e5f)
            n += static_cast<size_t>(snprintf(line + n, sizeof(line) - n, " +%02X=%.3f", static_cast<unsigned>(off),
                                              static_cast<double>(f)));
        else
            n += static_cast<size_t>(snprintf(line + n, sizeof(line) - n, " +%02X=0x%X", static_cast<unsigned>(off),
                                              static_cast<unsigned>(u)));
    }
    Log("[walls]     %s raw:%s", path, line);
}

// 17d: a CollisionMesh's layout (17c: +08 a count, +0C/+18 its box, +24 / +28 two arrays): its header, the
// first vertices if +24 is float triples, the first entries of +28 as 16- and 32-bit numbers
int g_wallMeshes = 0;
void WallMesh(const char* path, uint32_t mesh)
{
    if (g_wallMeshes >= 6 || !Readable(mesh, 0x80) || !WallLog())
        return;
    ++g_wallMeshes;
    char line[900];
    size_t n = 0;
    for (uint32_t off = 0; off < 0x80; off += 4)
        n += static_cast<size_t>(snprintf(line + n, sizeof(line) - n, " %08X", static_cast<unsigned>(ReadU32(mesh + off))));
    Log("[walls]     %s mesh header (dwords +00..+7C):%s", path, line);
    const uint32_t count = ReadU32(mesh + 0x08);
    const uint32_t a24 = ReadU32(mesh + 0x24);
    const uint32_t a28 = ReadU32(mesh + 0x28);
    if (count && count < 100000 && Readable(a24, 48) && WallLog())
    {
        float v[12];
        memcpy(v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a24)), sizeof(v));
        Log("[walls]     %s +24 as float triples: (%.3f %.3f %.3f) (%.3f %.3f %.3f) (%.3f %.3f %.3f) (%.3f %.3f %.3f); "
            "%u x 12 bytes readable: %s",
            path, static_cast<double>(v[0]), static_cast<double>(v[1]), static_cast<double>(v[2]),
            static_cast<double>(v[3]), static_cast<double>(v[4]), static_cast<double>(v[5]),
            static_cast<double>(v[6]), static_cast<double>(v[7]), static_cast<double>(v[8]),
            static_cast<double>(v[9]), static_cast<double>(v[10]), static_cast<double>(v[11]),
            static_cast<unsigned>(count), Readable(a24, count * 12u) ? "yes" : "NO");
    }
    if (Readable(a28, 64) && WallLog())
    {
        uint16_t h[24];
        uint32_t d[12];
        memcpy(h, reinterpret_cast<const void*>(static_cast<uintptr_t>(a28)), sizeof(h));
        memcpy(d, reinterpret_cast<const void*>(static_cast<uintptr_t>(a28)), sizeof(d));
        char h16[200];
        char h32[200];
        size_t a = 0;
        size_t b = 0;
        int over16 = 0;
        for (int i = 0; i < 24; ++i)
        {
            a += static_cast<size_t>(snprintf(h16 + a, sizeof(h16) - a, " %u", static_cast<unsigned>(h[i])));
            over16 += h[i] >= count ? 1 : 0;
        }
        for (int i = 0; i < 12; ++i)
            b += static_cast<size_t>(snprintf(h32 + b, sizeof(h32) - b, " %08X", static_cast<unsigned>(d[i])));
        Log("[walls]     %s +28 as 16-bit:%s (%d of 24 not below the count %u) | as 32-bit:%s", path, h16, over16,
            static_cast<unsigned>(count), h32);
    }
    // 17e: the triangle tree (17d: +28 = 12-byte nodes: a pointer + 8 bytes): the block before the vertices
    // (+58), the first 16 nodes, and what the first 4 node pointers point at (as 16-bit numbers)
    const uint32_t pre = ReadU32(mesh + 0x58);
    if (Readable(pre, 0x70) && WallLog())
    {
        char blk[700];
        size_t k = 0;
        for (uint32_t off = 0; off < 0x70; off += 4)
            k += static_cast<size_t>(snprintf(blk + k, sizeof(blk) - k, " %08X", static_cast<unsigned>(ReadU32(pre + off))));
        Log("[walls]     %s block at +58 (0x%08X, 0x70 bytes):%s", path, static_cast<unsigned>(pre), blk);
    }
    if (Readable(a28, 16 * 12) && WallLog())
    {
        char nodes[900];
        size_t k = 0;
        for (int i = 0; i < 16; ++i)
        {
            const uint32_t at = a28 + static_cast<uint32_t>(i) * 12;
            const uint8_t* b = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(at + 4));
            k += static_cast<size_t>(snprintf(nodes + k, sizeof(nodes) - k, " [%d] %08X %02X%02X%02X %02X%02X%02X %02X%02X",
                                              i, static_cast<unsigned>(ReadU32(at)), b[0], b[1], b[2], b[3], b[4], b[5],
                                              b[6], b[7]));
        }
        Log("[walls]     %s tree nodes (pointer, min, max, last 2 bytes):%s", path, nodes);
        uint32_t shown[4] = {};
        int n = 0;
        for (int i = 0; i < 16 && n < 4; ++i)
        {
            const uint32_t ptr = ReadU32(a28 + static_cast<uint32_t>(i) * 12);
            bool dup = false;
            for (int j = 0; j < n; ++j)
                dup = dup || shown[j] == ptr;
            if (dup || !Readable(ptr, 48) || !WallLog())
                continue;
            shown[n++] = ptr;
            uint16_t h[24];
            memcpy(h, reinterpret_cast<const void*>(static_cast<uintptr_t>(ptr)), sizeof(h));
            char l16[200];
            size_t q = 0;
            for (int j = 0; j < 24; ++j)
                q += static_cast<size_t>(snprintf(l16 + q, sizeof(l16) - q, " %u", static_cast<unsigned>(h[j])));
            Log("[walls]     %s at node [%d]'s pointer 0x%08X (+0x%X into the tree) as 16-bit:%s", path, i,
                static_cast<unsigned>(ptr), static_cast<unsigned>(ptr - a28), l16);
        }
    }
}

void WallWalk(uint32_t obj, int depth, const char* path, uint32_t* visited, int* nVisited)
{
    const uint32_t span = depth == 0 ? 0x400 : (depth == 1 ? 0x200 : 0x100);
    if (!Readable(obj, span))
        return;
    for (uint32_t off = 4; off < span; off += 4)
    {
        const uint32_t v = ReadU32(obj + off);
        if (v < 0x10000 || (v & 3) || v == obj)
            continue;
        bool seen = false;
        for (int i = 0; i < *nVisited; ++i)
            seen = seen || visited[i] == v;
        if (seen)
            continue;
        const char* cls = ObjClass(v);
        char sub[96];
        snprintf(sub, sizeof(sub), "%s+%X", path, static_cast<unsigned>(off));
        if (IsObjName(cls))
        {
            if (*nVisited < kWallVisited)
                visited[(*nVisited)++] = v;
            if (!Interesting(cls) && depth > 0)
                continue;
            if (WallLog())
                Log("[walls]   %s -> 0x%08X %s", sub, static_cast<unsigned>(v), cls);
            if (strstr(cls, "Primitive") || strstr(cls, "Body") || strstr(cls, "CollisionMesh"))
                WallRaw(sub, v);
            if (!strcmp(cls, "CollisionMesh"))
                WallMesh(sub, v); // 17d
            if (depth < 2 && Interesting(cls))
                WallWalk(v, depth + 1, sub, visited, nVisited);
        }
        else if (depth >= 1 && Readable(v, 16))
        {
            // a list of collision objects (a plain array of pointers)?
            int count = 0;
            const char* first = nullptr;
            for (int k = 0; k < 64 && Readable(v + k * 4, 4); ++k)
            {
                const char* c = ObjClass(ReadU32(v + k * 4));
                if (!IsObjName(c) || !(strstr(c, "Collision") || strstr(c, "Primitive") || strstr(c, "Body")))
                    break;
                if (!first)
                    first = c;
                ++count;
            }
            if (count && WallLog())
            {
                Log("[walls]   %s -> 0x%08X a list of %d: [0] %s", sub, static_cast<unsigned>(v), count, first);
                for (int k = 0; k < count && k < 4; ++k)
                {
                    char item[110];
                    snprintf(item, sizeof(item), "%s[%d]", sub, k);
                    WallRaw(item, ReadU32(v + k * 4));
                }
            }
        }
    }
}

// a rigid matrix (orthonormal 3x3, sane position) in the object: its place in the world
void WallMatrix(uint32_t obj)
{
    if (!Readable(obj, 0x400))
        return;
    int found = 0;
    for (uint32_t off = 0; off + 64 <= 0x400 && found < 2; off += 4)
    {
        float m[16];
        memcpy(m, reinterpret_cast<const void*>(static_cast<uintptr_t>(obj + off)), 64);
        bool ok = true;
        for (int i = 0; i < 16 && ok; ++i)
            ok = std::isfinite(m[i]);
        if (!ok || std::fabs(m[15] - 1.0f) > 1e-3f || std::fabs(m[3]) > 1e-3f || std::fabs(m[7]) > 1e-3f ||
            std::fabs(m[11]) > 1e-3f)
            continue;
        for (int r = 0; r < 3 && ok; ++r)
        {
            const float len = m[r * 4] * m[r * 4] + m[r * 4 + 1] * m[r * 4 + 1] + m[r * 4 + 2] * m[r * 4 + 2];
            ok = std::fabs(len - 1.0f) < 0.02f;
        }
        if (!ok || std::fabs(m[12]) > 5000.0f || std::fabs(m[13]) > 5000.0f || std::fabs(m[14]) > 5000.0f)
            continue;
        if (WallLog())
            Log("[walls]   matrix at +%X: position (%.2f %.2f %.2f), x axis (%.2f %.2f %.2f)", static_cast<unsigned>(off),
                static_cast<double>(m[12]), static_cast<double>(m[13]), static_cast<double>(m[14]),
                static_cast<double>(m[0]), static_cast<double>(m[1]), static_cast<double>(m[2]));
        ++found;
        off += 60;
    }
}

// at the 10 s summary (the game's thread, not inside a build)
void WallScanRun()
{
    for (int q = 0; q < g_wallQueued; ++q)
    {
        const uint32_t obj = g_wallQueue[q];
        ++g_wallProbed;
        if (!Readable(obj, 4) || !WallLog())
            continue;
        Log("[walls] prop %d: 0x%08X %s (hit by a ragdoll's floor ray)", g_wallProbed, static_cast<unsigned>(obj),
            ObjClass(obj));
        WallMatrix(obj);
        uint32_t visited[kWallVisited];
        int nVisited = 0;
        char path[16];
        snprintf(path, sizeof(path), "P%d", g_wallProbed);
        WallWalk(obj, 0, path, visited, &nVisited);
    }
    g_wallQueued = 0;
}

// finds the ray test and its one jump stub; 17b: keeps the real function for the floors; RayScan=1:
// redirects the stub to Hook_Ray (log only)
void InstallRayScan()
{
    if (!g_rayScanIni && !g_floorsIni)
    {
        Log("[ray] BF1GameExt.ini [Ragdoll] RayScan=0 and Floors=0: the game's ray test is not used");
        return;
    }
    uint32_t hits[4] = {};
    const int n = FindPattern(kRayPattern, hits, 4);
    if (n != 1)
    {
        Log("[ray] world ray test: %d pattern matches (need exactly 1) - ray scan off", n);
        return;
    }
    const uintptr_t fn = Rva(hits[0]);
    uint32_t textRva = 0;
    uint32_t textSize = 0;
    if (!CodeSection(&textRva, &textSize) || !Readable(Rva(textRva), textSize))
        return;
    const auto* code = reinterpret_cast<const unsigned char*>(Rva(textRva));
    uint32_t stub = 0;
    int stubs = 0;
    int calls = 0;
    for (uint32_t i = 0; i + 5 <= textSize; ++i)
    {
        if (code[i] != 0xE9 && code[i] != 0xE8)
            continue;
        int32_t rel = 0;
        memcpy(&rel, code + i + 1, 4);
        const uintptr_t target = Rva(textRva + i) + 5 + static_cast<uintptr_t>(static_cast<intptr_t>(rel));
        if (target != fn)
            continue;
        if (code[i] == 0xE9)
        {
            ++stubs;
            stub = textRva + i;
        }
        else
        {
            ++calls; // a direct call would miss the hook
        }
    }
    if (stubs != 1 || calls != 0)
    {
        Log("[ray] world ray test at rva 0x%06X: %d jump stubs, %d direct calls (need 1 and 0) - ray scan off",
            static_cast<unsigned>(hits[0]), stubs, calls);
        return;
    }
    int via = 0;
    for (uint32_t i = 0; i + 5 <= textSize; ++i)
    {
        if (code[i] != 0xE8)
            continue;
        int32_t rel = 0;
        memcpy(&rel, code + i + 1, 4);
        if (Rva(textRva + i) + 5 + static_cast<uintptr_t>(static_cast<intptr_t>(rel)) == Rva(stub))
            ++via;
    }
    g_rayFn = reinterpret_cast<Ray_t>(fn); // 17b: floors call it directly
    Log("[ray] world ray test at rva 0x%06X (jump stub rva 0x%06X, %d calls through it): floors for ragdolls %s",
        static_cast<unsigned>(hits[0]), static_cast<unsigned>(stub), via,
        g_floorsIni ? "ON (Floors=1)" : "off (Floors=0: terrain only)");
    if (!g_rayScanIni)
        return;
    InitializeCriticalSection(&g_rayLock);
    g_origRay = reinterpret_cast<Ray_t>(fn);
    g_raySummaryTick = GetTickCount();
    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(&Hook_Ray) - (Rva(stub) + 5));
    if (!PatchU32(Rva(stub) + 1, static_cast<uint32_t>(rel), PAGE_EXECUTE_READWRITE))
    {
        Log("[ray] could not write the jump stub - ray scan off");
        return;
    }
    g_rayOn = true;
    Log("[ray] world ray test at rva 0x%06X, its jump stub at rva 0x%06X (%d calls through it, GOG 1.3 has 61): "
        "hooked, log only - the first %d calls from each place in full, the busiest places every %.0f s",
        static_cast<unsigned>(hits[0]), static_cast<unsigned>(stub), via, kRayDetail,
        static_cast<double>(kRaySummaryMs) / 1000.0);
}

void ScanStart(bool onGameThread)
{
    const BuildInfo* build = CurrentBuild();
    if (!build || build->loadsMemExt)
    {
        Log("[ragdoll] ragdoll scan: not used on this build");
        return;
    }
    if (!g_scanIni && !g_ragdollIni)
    {
        Log("[ragdoll] [Ragdoll] Enabled=0 and Scan=0 at game start: no hooks (no ragdolls this session)");
        return;
    }
    InitHashes();
    LARGE_INTEGER qf;
    QueryPerformanceFrequency(&qf);
    g_qpcToMs = 1000.0 / static_cast<double>(qf.QuadPart);

    uintptr_t data = 0;
    uintptr_t rdata = 0;
    uint32_t dataLen = 0;
    uint32_t rdataLen = 0;
    if (!Section(".data", &data, &dataLen) || !Section(".rdata", &rdata, &rdataLen))
    {
        Log("[ragdoll] .data / .rdata not readable - ragdoll scan disabled");
        return;
    }
    Found f;
    memset(&f, 0, sizeof(f));
    f.vt0 = VtableByRtti("EntitySoldier", 0, data, dataLen, rdata, rdataLen);
    f.vt8 = VtableByRtti("EntitySoldier", kSubDamage, data, dataLen, rdata, rdataLen);
    f.vt17C = VtableByRtti("EntitySoldier", kSubRender, data, dataLen, rdata, rdataLen);
    Log("[ragdoll] hook setup: EntitySoldier vtables by RTTI: primary rva 0x%08X, +8 rva 0x%08X, +17Ch "
        "rva 0x%08X; ScanRange %.0f m",
        static_cast<unsigned>(RvaOf(f.vt0)), static_cast<unsigned>(RvaOf(f.vt8)),
        static_cast<unsigned>(RvaOf(f.vt17C)), static_cast<double>(g_range));
    if (!f.vt0)
    {
        Log("[ragdoll] EntitySoldier's vtable not found - ragdoll scan disabled");
        return;
    }
    g_vtSoldier = static_cast<uint32_t>(f.vt0);

    f.poseOk = FindPoseSites(f);
    CheckWeaponDraw(f); // 15g, log-only
    f.deathOk = FindDeath(f);
    f.deleteOk = FindDelete(f);
    FindTerrain();

    if (!onGameThread)
    {
        Log("[ragdoll] the scan did not run on the game thread - hooks NOT installed (not safe)");
        return;
    }
    InitializeCriticalSection(&g_lock);
    g_lockReady = true;
    memset(&g_win, 0, sizeof(g_win));

    if (f.deleteOk)
    {
        g_origDelete = reinterpret_cast<Delete_t>(ReadU32(f.vt0 + kSlotDelete * 4));
        g_deleteOn = InstallSlot(f.vt0 + kSlotDelete * 4, reinterpret_cast<uintptr_t>(&Hook_Delete), "soldier delete");
    }
    if (f.deathOk)
    {
        g_origDeath = reinterpret_cast<Death_t>(ReadU32(f.vt8 + kSlotDeath * 4));
        g_deathOn = InstallSlot(f.vt8 + kSlotDeath * 4, reinterpret_cast<uintptr_t>(&Hook_Death), "death");
    }
    if (f.poseOk)
    {
        g_poseOn = true; // set before the calls are redirected: the hook reads it
        if (!InstallPose(f))
            g_poseOn = false;
    }
    if (g_hitScanIniValue || g_ragdollIni)
    {
        const uintptr_t pushSlot = FindPush(f);
        const uintptr_t damageSlot = FindDamage(f); // 15f: the ragdolls use the hits too
        if (pushSlot)
        {
            g_origPush = reinterpret_cast<Push_t>(ReadU32(pushSlot));
            g_pushOn = InstallSlot(pushSlot, reinterpret_cast<uintptr_t>(&Hook_Push), "push (log-only)");
        }
        if (damageSlot)
        {
            g_origDamage = reinterpret_cast<Damage_t>(ReadU32(damageSlot));
            g_damageOn = InstallSlot(damageSlot, reinterpret_cast<uintptr_t>(&Hook_Damage), "damage (log-only)");
        }
        g_hitScanIni = g_hitScanIniValue && (g_pushOn || g_damageOn);
        Log("[hit] push hook %s (thrown ragdolls, blasts + hit scan), damage hook %s (hits, headshots + hit scan); "
            "hit scan logging %s",
            g_pushOn ? "ON" : "off", g_damageOn ? "ON" : "off", g_hitScanIni ? "ON" : "off");
    }
    else
    {
        g_hitScanIni = false;
        Log("[hit] BF1GameExt.ini [Ragdoll] HitScan=0 and Enabled=0: no push / damage hooks");
    }
    InstallRayScan(); // 17a: log only
    Log("[ragdoll] hooks: pose %s, death %s, delete %s, terrain height %s; 15b scan logging %s",
        g_poseOn ? "ON" : "off", g_deathOn ? "ON" : "off", g_deleteOn ? "ON" : "off",
        g_terrainHeight ? "found" : "missing", g_scanIni ? "ON (Scan=1)" : "off (Scan=0)");
    if (!g_ragdollIni)
    {
        Log("[ragdoll] [Ragdoll] Enabled=0 at game start (BF1GameExt.ini or the live file) and [Splitscreen] ModsMenu=0: no "
            "ragdolls this session");
        return;
    }
    if (!g_poseOn || !g_deathOn || !g_terrainHeight)
    {
        g_ragdollIni = false;
        Log("[ragdoll] ragdolls OFF: they need the pose hook, the death hook and the terrain height");
        return;
    }
    Log("[ragdoll] ragdolls ON: deaths on the %s (within %.1f m of it) within %.0f m of a local player, at most %d "
        "at once, built at the first draw within %.1f s of death; 11 capsules, 10 joints; Debug %s",
        FloorsOn() ? "floor (17b: the game's ray test - platforms, walkways, interiors, terrain)" : "terrain",
        static_cast<double>(FloorsOn() ? kOnFloor : kOnTerrain), static_cast<double>(g_rdRange), g_rdMax,
        static_cast<double>(kBuildWindow), g_rdDebug ? "ON" : "off");
    Log("[ragdoll] thrown ragdolls %s (players too); settings reload by themselves (F7 = now), F8 = slow motion for "
        "ragdolls",
        g_pushOn ? "ON" : "OFF (no push hook)");
    g_rdReady = true; // 21j-r17
    if (!g_rdLiveOn)
        Log("[ragdoll] 21j-r17: [Ragdoll] Enabled=0 at game start: ragdolls OFF for now - the hooks are in ([Splitscreen] "
            "ModsMenu=1), so the MODS page's RAGDOLL EFFECTS (or Enabled=1) turns them on");
    else
        Log("[ragdoll] 21j-r17: the MODS page's RAGDOLL EFFECTS row can switch them %s",
            g_modsMenu ? "off and on again" : "(ModsMenu=0: no MODS page - Enabled=0 while playing still ends them)");
    LogTuning("settings");
    LiveStart(); // 15g
    LabStart();
}

} // namespace

uintptr_t RagdollOrigDeathFn()
{
    return g_deathOn ? reinterpret_cast<uintptr_t>(g_origDeath) : 0;
}

// 21j-r17: the MODS page's RAGDOLL EFFECTS row
int RagdollMenuState()
{
    if (!g_rdReady || !g_lockReady)
        return -1;
    return g_rdLiveOn ? 1 : 0;
}

int RagdollMenuSet(bool on)
{
    if (!g_rdReady || !g_lockReady || !g_iniPath[0])
        return -1;
    const char* const v = on ? "1" : "0";
    const BOOL savedIni = WritePrivateProfileStringA("Ragdoll", "Enabled", v, g_iniPath);
    // the live file's keys win over BF1GameExt.ini's: written there too when it has the key
    bool liveHas = false;
    BOOL savedLive = TRUE;
    if (g_liveFile && g_livePath[0])
    {
        char buf[16] = {};
        GetPrivateProfileStringA("Ragdoll", "Enabled", "\x01", buf, sizeof(buf), g_livePath);
        liveHas = buf[0] != '\x01';
        if (liveHas)
            savedLive = WritePrivateProfileStringA("Ragdoll", "Enabled", v, g_livePath);
    }
    EnterCriticalSection(&g_lock);
    const bool was = g_rdLiveOn;
    // the same reload as F7 (both files, the changes logged; Enabled 1 -> 0 ends the running ragdolls)
    const FileStamp none = { false, 0, 0, 0 };
    g_iniLoaded = g_iniSeen = StampOf(g_iniPath);
    g_liveLoaded = g_liveSeen = g_livePath[0] ? StampOf(g_livePath) : none;
    ReloadSettings("the MODS page's RAGDOLL EFFECTS", "menu");
    bool forced = false;
    if (g_rdLiveOn != on)
    {
        // not saved (or a file still says otherwise): this session anyway, until a file changes again
        forced = true;
        g_rdLiveOn = on;
        if (!on)
            EndAllRagdolls("RAGDOLL EFFECTS OFF (the MODS page)");
    }
    const bool now = g_rdLiveOn;
    LeaveCriticalSection(&g_lock);
    Log("[ragdoll] %lu ms: 21j-r17: the MODS page: RAGDOLL EFFECTS %s (was %s) - saved in BF1GameExt.ini [Ragdoll] Enabled=%s%s"
        "%s%s",
        ScanMsSinceAttach(), now ? "ON" : "OFF", was ? "ON" : "OFF", v, savedIni ? "" : " (NOT SAVED - the write failed)",
        liveHas ? (savedLive ? " and in ragdoll_live.ini (it has the key)" : ", ragdoll_live.ini NOT written") : "",
        forced ? "; set for this session by hand (the files did not say it)" : "");
    return now ? 1 : 0;
}

void RagdollStart(bool onGameThread)
{
    Log("[ragdoll] ragdoll.cpp file build %s, ragdoll_body.cpp file build %s", kFileBuild, ragdoll::FileBuild());
    ReadIni();
    // 22c: Bullet's memory through BF1GameExt, before Bullet makes anything
    btAlignedAllocSetCustom(&RdAlloc, &RdFree);
    g_rdMemOurs = true;
    g_rdReserve = malloc(kRdReserveBytes);
    if (g_bulletTest)
        BulletDropTest();
    else
        Log("[ragdoll] BF1GameExt.ini [Ragdoll] BulletTest=0: no Bullet test");
    ScanStart(onGameThread);
    // 22c: the physics world now, while memory is plentiful (it was made by the first ragdoll)
    if (g_ragdollIni && !g_disabled)
    {
        const LONG before = g_rdMemBytes;
        const bool made = ragdoll::Prepare();
        char va[120];
        Log("[ragdoll] 22c: the physics world %s at the game's DirectInput call: %ld KB (pools of 512 contacts + 512 collision "
            "algorithms; Bullet's own 4096 + 4096 took 4.1 MB at the first ragdoll, 3.6 MB of it in one piece). Bullet's memory goes through BF1GameExt: a "
            "failed allocation is logged and stops new ragdolls for 60 s (a 2 MB reserve given back first); a ragdoll is built "
            "only while Windows can still give 1 MB in one piece; reserve %s; %s",
            made ? "made" : "NOT made", static_cast<long>((g_rdMemBytes - before) / 1024), g_rdReserve ? "held" : "NOT held",
            MemLogVaShort(va, sizeof(va)));
    }
    else
        Log("[ragdoll] 22c: ragdolls off at start - the physics world not made now; Bullet's memory goes through BF1GameExt");
}

} // namespace bf1
