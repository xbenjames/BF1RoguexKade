-- BF1GameExt_split.lua   (file build p21j-r30)
gBFX_Build_split = "BF1GameExt_split.lua (file build p21j-r30)" -- 21j-r30: the build as a string too: a compiled script (the mod tools'
-- ScriptMunge, console.lvl) keeps no comments, and BF1GameExt's log reads its build from this string there
-- 21j-r28: THE SIGN-IN'S SOUND AND MUSIC BACK ([Splitscreen] SignInSoundFix=1). On PC every profile switch in the shell
-- restarts the sound engine (the profile's sound settings are applied: the engine shut down, started again, core.lvl's
-- sounds read again) - the shell's own sounds (sound\shell.lvl: the menu clicks, the music and voice streams) and its movie
-- stream are gone after it. The PC's login and audio options screens know: ScriptCB_GetMixConfigChanged() says so and they
-- put the shell's sound back (ifs_opt_sound_closeShellSound / restoreShellSound). The console sign-in does not: its profile
-- list (ScriptCB_GetLoginList with split screen on makes player 2's profile current and back - two restarts) left the
-- sign-in silent, and the profile loading at its launch did it again - the music never came back (r27's [audio] lines:
-- every later SetShellMusic found no music stream). Now after both (and a cancelled load) the game's own restore runs when
-- the engine was restarted, the music starts again at once and the sign-in's background movie goes on.
-- 21j-r27: THE SIGN-IN'S BUTTONS. (1) BACK / ACCEPT show the pad's B / A for every player ([Splitscreen] SignInPadIcons=1):
-- the 2020 PC shell's btnb / btna are the mouse's right / left buttons - the quarters get the 2020 gamepad sheet's B / A
-- (gamepad_xbox360, gGamepadTexture's cells), set while the game makes the screen. (2) The BACK button at the bottom left
-- (the PC's own, CreateNegativeButton) works with the mouse ([Splitscreen] SignInMouseBack=1): the mouse over it, a click
-- leaves the sign-in as player 1's B on his list does (the screen's Input_Accept never looked at it); with 3 / 4 players it
-- is shown again (r1 hid it). A pad's A is never taken for the click (the game's last input says pad).
-- 21j-r8: PICK YOUR SIDE IN THE CONSOLE'S LOOK FOR 2 / 3 / 4 PLAYERS ([Splitscreen] GCPickLook=1, GCCommander2P=1). The
-- game's window gets a row of the two sides' pictures for every player - the console's two rows, players 3 / 4's made the
-- same way, four smaller slots with 3 / 4 players - the picked side lit, the other darker once he has locked in. The
-- PLAYERS list at the right (21g-r3's text) is now a box for each player in the class screen's look (the game's window,
-- its orange title bar): PLAYER n while he picks, then COMMANDER (the first who locked in on that side: he takes its turns
-- on the galaxy map) or PLAYER READY, the bar and his side's name green for the Rebels / Republic, red for the Empire /
-- CIS. With 2 players too: both lock in (A, B unlocks), player 1 launches; in co-op only the commander moves on the galaxy
-- map (GCCommander2P=0: the 2-player screen as 21f, the game's own galaxy). GCPickLook=0: 21g-r3's text list.
-- 21g-r4: GALACTIC CONQUEST - WHOSE TURN. (1) The AI's turn in co-op: with players 1 and 2 on one side the game's AI
-- moves the other side on the game's third joystick, whose team nothing ever set - the game's script stopped at every
-- AI move and the AI side never took its turn (2 players as well as 3 / 4 on one side). That joystick is put on the
-- AI's team now. (2) THE TURN SCANNER ([Splitscreen] GCTurnScan, log only, [gcturn] lines): on the galaxy map, who has
-- the turn and who moves that side (the player and his pad, or the AI), every change of it, every choice, every press
-- (whose, where it went, what the game made of it), the battle result and why the turn stays or passes (the game's rule:
-- the attacker attacks again while he wins), the game's script errors, and a turn that stands still.
-- 21g-r3: GALACTIC CONQUEST WITH 3 / 4 PLAYERS. PICK YOUR SIDE: every player picks a side (players 1 / 2 on their rows,
-- players 3 / 4 in the roster at the right, pads 3 / 4) and locks in with A (B unlocks); the first who locks in on a side
-- COMMANDS it - only he moves on the galaxy map and picks its bonuses (ifs_meta_main: the other players' presses are
-- dropped, players 3 / 4's commanders through their pads); both sides with players = VERSUS, one = CO-OP (the AI runs
-- the other). LAUNCH once everyone is locked in. BF1GameExt keeps the sides and commanders across the battles and locks
-- players 3 / 4 to their sides in them (the campaign: player 1's side). With 2 players: as 21f.
-- 21g-r2: (1) the 4-player sign-in's LAUNCH tells BF1GameExt how many signed in - the match has that many players (2 =
-- the 2-player split, 3 = the 3-player layout, 4 = quarters), and the pads of players 3 / 4 who signed in are theirs
-- (player 4 alone plays as player 3). (2) TEMPORARY PROFILES ([Splitscreen] TempProfiles): PLAYER 2 / 3 / 4 at the end of
-- the list where no profile has that name; players 2 / 3 / 4 start on their own; never saved (not in the profile screen):
-- player 2's is made by BF1GameExt at the launch ("BFXQ: tempprofile"), ifs_saveop does not save it; player 1 signs in
-- with a saved profile.
-- 21g-r1: THE 4-PLAYER SIGN-IN, as Battlefront II has it on the Xbox (with players 3 / 4 on - BF1GameExt.ini Players=3 / 4 -
-- and [Splitscreen] SignIn4=1): four quarters split by the game's own gray_rect line and a vertical one like it; player 1
-- top left, player 2 top right, player 3 bottom left, player 4 bottom right; each quarter PRESS START BUTTON TO JOIN ->
-- his profile list (BACK / ACCEPT) -> his profile's name over READY. Pads 3 and 4 join (BF1GameExt reads them). With 2
-- players the 2-player screen exactly as before. (The match's player count still comes from Players; 21g-r2: from
-- the players who sign in.)
-- 21f-r3: a conquest in progress (back from its battle, the game resumes it through ifs_meta_top) goes to the galaxy
-- again - the game's own Enter loads it - so the galaxy's save question comes up; PICK YOUR SIDE only for a new conquest
-- launched from the maps. (r2 showed PICK YOUR SIDE again after every battle.)
-- 21f-r2: Galactic Conquest rebuilt after the Classic Collection video: PICK YOUR SIDE (ifs_meta_top) instead of r1's
-- line on the maps screen; the split screen map list (4 conquests) as the Classic Collection has it.
-- The console's SPLIT SCREEN entry of the main menu, back on PC - 2 players (step 1; 4 players next).
-- 21f-r1: Galactic Conquest in split screen as the Classic Collection has it (below): the split screen map list ->
-- LAUNCH -> PICK YOUR SIDE (ifs_meta_top, the console's screen: each player picks his side - same side = CO-OP, other
-- sides = VERSUS) -> LAUNCH. Player 1 starts the sign-in on his own profile (by name when the list has no active slot 1).
-- (The campaign already puts both players on the mission's side; BF1GameExt skips the side screens while the game says
-- sides cannot be switched - [Splitscreen] SideLock.)
-- BF1GameExt runs this (Lua 4.0, the game's own compiler) with these lines in front of it:
--   gBFX_SplitSignIn  1 = SPLIT SCREEN opens the console sign-in screen (ifs_split_profile: each player picks a
--                     profile, player 2 joins with START on pad 2); nil = it goes straight to the split screen game
--                     modes ([Splitscreen] SplitSignIn)
--   gBFX_Screen       the screen the game is making or opening ("ifs_main" / "ifs_split_profile" / "ifs_meta_opts" /
--                     "ifs_meta_top")
--   gBFX_Phase        "build" (ScriptCB_AddIFScreen, while the shell scripts load) or "push" (ScriptCB_PushScreen)
--   gBFX_Players      21g: the players a split screen match has (2, or 3 / 4 with players 3 / 4 on)
--   gBFX_SignIn4      21g: 1 = the 4-player sign-in when gBFX_Players is 3 / 4 ([Splitscreen] SignIn4)
--   gBFX_TempProfiles 21g-r2: 1 = PLAYER 2 / 3 / 4 temporary profiles in the sign-in ([Splitscreen] TempProfiles, and
--                     BF1GameExt found the game's profiles)
--   gBFX_GCTurnScan   21g-r4: 1 = the galaxy turn scanner's [gcturn] lines ([Splitscreen] GCTurnScan)
--   gBFX_GCPickLook   21j-r8: 1 = PICK YOUR SIDE in the console's look: a row for each player, the player boxes
--                     ([Splitscreen] GCPickLook)
--   gBFX_GCCmd2       21j-r8: 1 = with 2 players both lock in and each side's commander moves it on the galaxy map
--                     ([Splitscreen] GCCommander2P)
--   gBFX_PadIcons     21j-r27: 1 = the sign-in's BACK / ACCEPT with the pad's B / A ([Splitscreen] SignInPadIcons)
--   gBFX_MouseBack    21j-r27: 1 = the sign-in's BACK button at the bottom left works with the mouse ([Splitscreen]
--                     SignInMouseBack)
--   gBFX_SoundFix     21j-r28: 1 = the shell's sound and music put back after the sign-in's profile switches
--                     ([Splitscreen] SignInSoundFix)
-- Every part below is put in once per Lua state (ifs_main.bfxsplit, ifs_split_profile.bfxpc, ifs_meta_opts.bfxgc,
-- ifs_meta_top.bfxtop), whichever run comes first.
--
-- What is in the shell already (the 2020 shell scripts): ifs_main has the "split" button (hidden on PC by its Enter);
-- its Input_Accept for it sets ifs_sp.bForSplitScreen, ScriptCB_SetSplitscreen(1) and opens ifs_split_profile, the
-- console sign-in screen (players' profiles; on success ScriptCB_PushScreen("ifs_sp") - the game modes with a SPLIT
-- SCREEN title, Instant Action -> ifs_split_map -> the map list -> the match).
-- What this adds:
--   ifs_main          the "split" button shown (its label in English if the game's text is missing)
--   ifs_split_profile player 1 = the game's own input (keyboard, mouse, the game's pad; A works as START where START
--                     is asked for: to launch); player 2 = pad 2, read by BF1GameExt (the PC game gives the shell only
--                     its own pad) and asked for each frame (ScriptCB_GetPlatform("BFXQ: pad2") -> an event number);
--                     player 1 starts on the profile he signed in with, player 2 on the second active profile;
--                     the texts in English where the game's are missing; everything logged
-- Its log lines come in as ScriptCB_GetPlatform("BFXS: ...") - [splitmenu] lines in BF1GameExt_split.log.

-- Shared (defined on every run)
function bfxs_Log(s)
	ScriptCB_GetPlatform("BFXS: " .. s)
end

-- 21g-r4: the galaxy turn scanner ([Splitscreen] GCTurnScan=1) - split screen only - and its lines ([gcturn] in
-- BF1GameExt_split.log)
function bfxt_On()
	return gBFX_GCTurnScan and ScriptCB_IsSplitscreen()
end

function bfxt_Log(s)
	if (bfxt_On()) then
		ScriptCB_GetPlatform("BFXT: " .. s)
	end
end

-- A question to BF1GameExt: a number back (-1 if nothing answered)
function bfxs_Ask(what)
	local r = ScriptCB_GetPlatform("BFXQ: " .. what)
	if (type(r) == "number") then
		return r
	end
	return -1
end

-- 1 if the game has the text, nil if not (ScriptCB_getlocalizestr's 2nd argument: nil when it is missing)
function bfxs_HasText(key)
	if (ScriptCB_getlocalizestr(key, 1)) then
		return 1
	end
	return nil
end

-- The game's text, or the English one when the game has none
function bfxs_SetText(obj, key, english)
	if (not obj) then
		return
	end
	if (bfxs_HasText(key)) then
		IFText_fnSetString(obj, key)
	else
		IFText_fnSetUString(obj, ScriptCB_tounicode(english))
		if (not gBFXS_Missing) then
			gBFXS_Missing = {}
		end
		if (not gBFXS_Missing[key]) then
			gBFXS_Missing[key] = 1
			bfxs_Log(format("text \"%s\" is not in the game - shown as \"%s\"", key, english))
		end
	end
end

-- ---------------------------------------------------------------------------------------------------------------------
-- ifs_main: the SPLIT SCREEN button
-- ---------------------------------------------------------------------------------------------------------------------
if (ifs_main and (not ifs_main.bfxsplit)) then
ifs_main.bfxsplit = 1

gBFXS_MainEnter = ifs_main.Enter
gBFXS_MainAccept = ifs_main.Input_Accept

ifs_main.Enter = function(this, bFwd)
	gBFXS_MainEnter(this, bFwd)
	if (this.buttons and this.buttons.split) then
		this.buttons.split.hidden = nil
		ShowHideVerticalButtons(this.buttons, ifsmain_vbutton_layout)
		if (this.buttons.split.label and (not bfxs_HasText("ifs.main.split"))) then
			IFText_fnSetUString(this.buttons.split.label, ScriptCB_tounicode("Split Screen"))
		end
		if (not gBFXS_MainShown) then
			gBFXS_MainShown = 1
			local where = "the console sign-in screen (ifs_split_profile)"
			if (not gBFX_SplitSignIn) then
				where = "the split screen game modes (ifs_sp) - SplitSignIn=0"
			elseif (not ifs_split_profile) then
				where = "the split screen game modes (ifs_sp) - ifs_split_profile is NOT in this shell"
			end
			local label = "the game's text"
			if (not bfxs_HasText("ifs.main.split")) then
				label = "\"Split Screen\" (the game's text ifs.main.split is missing)"
			end
			bfxs_Log(format("main menu: SPLIT SCREEN button shown, label %s; SPLIT SCREEN -> %s", label, where))
		end
	else
		bfxs_Log("main menu: NO \"split\" button in ifs_main - nothing shown")
	end
end

ifs_main.Input_Accept = function(this)
	if ((this.CurButton == "split") and ((not gBFX_SplitSignIn) or (not ifs_split_profile))) then
		-- straight to the game modes (as "sp", with split screen on)
		if (gShellScreen_fnDefaultInputAccept(this)) then
			return
		end
		ifelm_shellscreen_fnPlaySound(this.acceptSound)
		bfxs_Log("main menu: SPLIT SCREEN picked - split screen ON, straight to the game modes (ifs_sp)")
		ifs_sp.bForSplitScreen = 1
		ScriptCB_SetSplitscreen(1)
		ScriptCB_SetInNetGame(nil)
		ifs_main_GotoLoginScreen(this, ifs_sp)
		return
	end
	if (this.CurButton == "split") then
		bfxs_Log("main menu: SPLIT SCREEN picked - split screen ON, the sign-in screen next (the game's own code)")
	end
	gBFXS_MainAccept(this)
end

bfxs_Log(format("ifs_main patched (%s, %s)", gBFX_Phase or "?", gBFX_Screen or "?"))
end

-- ---------------------------------------------------------------------------------------------------------------------
-- ifs_split_profile: the console sign-in screen with player 2 on pad 2
-- ---------------------------------------------------------------------------------------------------------------------
if (ifs_split_profile and (not ifs_split_profile.bfxpc)) then
ifs_split_profile.bfxpc = 1

-- input events by number (BF1GameExt's pad 2 answers with these)
gBFXS_Name = { "Input_Accept", "Input_Back", "Input_Start", "Input_GeneralUp", "Input_GeneralDown", "Input_GeneralLeft",
	"Input_GeneralRight" }
gBFXS_Code = { Input_Accept = 1, Input_Back = 2, Input_Start = 3, Input_GeneralUp = 4, Input_GeneralDown = 5,
	Input_GeneralLeft = 6, Input_GeneralRight = 7 }
gBFXS_Short = { "A", "B", "START", "UP", "DOWN", "LEFT", "RIGHT" }
gBFXS_Clock = 0
gBFXS_LastPadCode = 0
gBFXS_LastPadTime = -10
gBFXS_GameP2 = nil   -- 1 once the game itself gives player 2's pad (Joystick 1): then BF1GameExt's is not used
gBFXS_Inside = nil   -- 1 while a handler runs: the screen's own calls (Input_Start -> Input_Accept) go straight on
gBFXS_Conn = -2
gBFXS_PadCheck = 0

-- ---------------------------------------------------------------------------------------------------------------------
-- 21g: THE 4-PLAYER SIGN-IN (Battlefront II's on the Xbox) - with players 3 / 4 on (BF1GameExt.ini Players=3 / 4) and
-- [Splitscreen] SignIn4=1. The screen in four quarters, split by the game's own middle line (gray_rect tinted 192 / 192
-- / 255 at 0.8 - the texture, tint and thickness of Battlefront II's lines) and a vertical one made the same way:
-- player 1 top left, player 2 top right, player 3 bottom left, player 4 bottom right (their places in the match). Each
-- quarter: PRESS START BUTTON TO JOIN -> the profile list with BACK / ACCEPT -> his profile's name over READY, at
-- Battlefront II's places. Players 1 and 2 keep the game's own pieces (moved into their quarters while the screen is
-- made); players 3 and 4 get copies of them, run here (pads 3 and 4 read by BF1GameExt: "BFXQ: pad3" / "pad4"). A
-- profile READY for one player is greyed for the others and cannot be picked twice. START launches once players 1 and
-- 2 are READY and every other player who joined is READY too. Players 3 and 4's profiles are names only (the game
-- loads 2 profiles). With Players=3 the bottom right quarter stays empty.
-- ---------------------------------------------------------------------------------------------------------------------
gBFXS_Four = nil       -- 1: the 4-player sign-in was built into this screen
gBFXS_Players = 2      -- players in the match (2 / 3 / 4, from BF1GameExt)
gBFXS_Mode = {}        -- players 3 / 4: -1 hidden, 0 PRESS START, 1 picking his profile, 3 READY
gBFXS_Layout = {}      -- players 3 / 4's list layouts (as the game's layoutT)
gBFXS_GameP = {}       -- players 3 / 4: 1 once the game itself gives his pad (Joystick 2 / 3)
gBFXS_PadCode = {}     -- players 3 / 4: BF1GameExt's last event and when (not done twice when the game gives it too)
gBFXS_PadTime = {}
gBFXS_PadConn = {}
gBFXS_Pulse = 1
gBFXS_PulseDir = 1
-- each player's places (fractions of the screen): the centre of his pieces, his JOIN / READY line, his list, his
-- BACK / ACCEPT line (y -20 under it, as the game's)
gBFXS_QX = { 0.34, 0.66, 0.34, 0.66 }
gBFXS_QText = { 0.223, 0.223, 0.648, 0.648 }
gBFXS_QList = { 0.267, 0.267, 0.692, 0.692 }
gBFXS_QHelp = { 0.5, 0.5, 0.93, 0.93 }

function bfxs4_ModeName(m)
	if (m == 0) then
		return "PRESS START"
	elseif (m == 1) then
		return "picking his profile"
	elseif (m == 3) then
		return "READY"
	end
	return "-"
end

-- player p's mode, list layout and list (1 / 2: the game's, 3 / 4: ours)
function bfxs4_State(this, p)
	if (p == 1) then
		return this.ModeT, ifs_split_profile_listbox_layoutT, this.ProfileT.listbox
	elseif (p == 2) then
		return this.ModeB, ifs_split_profile_listbox_layoutB, this.ProfileB.listbox
	end
	local c = this["bfx" .. p .. "Profile"]
	if (c) then
		return gBFXS_Mode[p], gBFXS_Layout[p], c.listbox
	end
	return nil, nil, nil
end

-- the other player (not p) on that row: strict - READY on it; else - joined and on it too
function bfxs4_UsedBy(this, row, p, strict)
	if (not row) then
		return nil
	end
	local q
	for q = 1, gBFXS_Players do
		if (q ~= p) then
			local m, lay = bfxs4_State(this, q)
			if (m and lay and (lay.SelectedIdx == row)) then
				if ((m == 3) or ((not strict) and (m >= 1) and (m <= 3))) then
					return q
				end
			end
		end
	end
	return nil
end

function bfxs4_HeldBy(this, row, p)
	return bfxs4_UsedBy(this, row, p, 1)
end

-- a player 3 / 4 who joined and is still picking (nil: none)
function bfxs4_Picking()
	local p
	for p = 3, gBFXS_Players do
		if (gBFXS_Mode[p] == 1) then
			return p
		end
	end
	return nil
end

-- a row for a player who joins: one nobody else is on, else one nobody holds READY
function bfxs4_FreeRow(this, p)
	local n = getn(ifs_split_profile_listbox_contents)
	local row
	for row = 1, n do
		if (not bfxs4_UsedBy(this, row, p, nil)) then
			return row
		end
	end
	for row = 1, n do
		if (not bfxs4_HeldBy(this, row, p)) then
			return row
		end
	end
	return 1
end

-- a list call for player 3 / 4: the game's list globals kept as they were (its cursor pulse and mouse wheel follow
-- players 1 and 2's lists only)
function bfxs4_ListCall(this, p, fn)
	local keep1 = gCurHiliteListbox
	local keep2 = gCurHiliteListbox2
	local keepL = gCurListboxLayout
	fn(this["bfx" .. p .. "Profile"].listbox, ifs_split_profile_listbox_contents, gBFXS_Layout[p])
	gCurHiliteListbox = keep1
	gCurHiliteListbox2 = keep2
	gCurListboxLayout = keepL
end

-- a name over READY: the profile on the player's row
function bfxs4_Name(obj, lay, on)
	if (not obj) then
		return
	end
	if (on and lay and lay.SelectedIdx) then
		local sel = ifs_split_profile_listbox_contents[lay.SelectedIdx]
		if (sel and sel.showstr) then
			IFText_fnSetUString(obj, sel.showstr)
			IFObj_fnSetVis(obj, 1)
			return
		end
	end
	IFObj_fnSetVis(obj, nil)
end

-- every list's rows: grey where another player holds that profile READY (the game does it for players 1 and 2 only)
function bfxs4_Grey(this)
	local n = getn(ifs_split_profile_listbox_contents)
	local p
	for p = 1, gBFXS_Players do
		local m, lay, list = bfxs4_State(this, p)
		if (lay and list and lay.showcount) then
			local first = max(lay.FirstShownIdx or 1, 1)
			local last = min(n, first - 1 + lay.showcount)
			local cnt
			for cnt = first, last do
				if (list[cnt - first + 1]) then
					if (bfxs4_HeldBy(this, cnt, p)) then
						IFObj_fnSetColor(list[cnt - first + 1], 128, 128, 128)
					else
						IFObj_fnSetColor(list[cnt - first + 1], 255, 255, 255)
					end
				end
			end
		end
	end
end

-- after the game's own ifs_split_profile_fnSetPieceVis: the names, players 3 / 4's pieces, the launch prompt
function bfxs4_Vis(this)
	local busy = (not this.ModeT) or (this.ModeT < 0) or (this.ModeT > 3)
	bfxs4_Name(this.bfxNameT, ifs_split_profile_listbox_layoutT, (not busy) and (this.ModeT == 3))
	bfxs4_Name(this.bfxNameB, ifs_split_profile_listbox_layoutB, (not busy) and (this.ModeB == 3))
	local p
	for p = 3, 4 do
		local m = gBFXS_Mode[p] or -1
		local on = (not busy) and (p <= gBFXS_Players) and (m >= 0)
		IFObj_fnSetVis(this["bfx" .. p .. "Press"], on and (m == 0))
		IFObj_fnSetVis(this["bfx" .. p .. "Profile"], on and (m == 1))
		IFObj_fnSetVis(this["bfx" .. p .. "Ready"], on and (m == 3))
		bfxs4_Name(this["bfx" .. p .. "Name"], gBFXS_Layout[p], on and (m == 3))
		IFObj_fnSetVis(this["bfx" .. p .. "Back"], on and (m >= 1))
		IFObj_fnSetVis(this["bfx" .. p .. "Accept"], on and (m == 1))
		if (on and (m == 1)) then
			bfxs4_ListCall(this, p, ListManager_fnFillContents)
		end
	end
	-- the launch prompt: players 1 and 2 READY and nobody still picking
	IFObj_fnSetVis(this.pressstart, (not busy) and (this.ModeT == 3) and (this.ModeB == 3) and (not bfxs4_Picking()))
	-- the PC's own BACK button at the bottom left: 21j-r27 - on again (gBFX_MouseBack: the mouse's way out of the sign-in,
	-- under player 3's quarter's own BACK / ACCEPT line); without it hidden as r1 had it (player 1 has his BACK in his quarter)
	if (this.Helptext_Back) then
		IFObj_fnSetVis(this.Helptext_Back, gBFX_MouseBack and (not busy))
	end
	if (not busy) then
		bfxs4_Grey(this)
	end
end

-- players 1 and 2's presses the 4-player screen refuses (1 = refused): READY on a profile player 3 / 4 holds; the launch
-- while player 3 / 4 is still picking
function bfxs4_Gate(this, name, joy)
	local p = 1
	local m = this.ModeT
	local lay = ifs_split_profile_listbox_layoutT
	if (joy > 0.5) then
		p = 2
		m = this.ModeB
		lay = ifs_split_profile_listbox_layoutB
	end
	if (((name == "Input_Accept") or (name == "Input_Start")) and ((m == 1) or (m == 2))) then
		local q = bfxs4_HeldBy(this, lay.SelectedIdx, p)
		if (q and (q > 2)) then
			ifelm_shellscreen_fnPlaySound(this.errorSound)
			bfxs_Log(format("player %d: row %s (%s) is player %d's READY profile - not READY", p, tostring(lay.SelectedIdx),
				bfxs_ProfileName(ifs_split_profile_listbox_contents[lay.SelectedIdx or 0]), q))
			return 1
		end
	end
	if ((this.ModeT == 3) and (this.ModeB == 3) and ((name == "Input_Start") or ((name == "Input_Accept") and (p == 1)))) then
		local q = bfxs4_Picking()
		if (q) then
			ifelm_shellscreen_fnPlaySound(this.errorSound)
			bfxs_Log(format("player %d: START - player %d is still picking his profile: no launch yet", p, q))
			return 1
		end
	end
	return nil
end

function bfxs4_Join(this, p)
	local lay = gBFXS_Layout[p]
	local row = bfxs_OwnRow(this, p) or bfxs4_FreeRow(this, p) -- 21g-r2: PLAYER p first
	gBFXS_Mode[p] = 1
	lay.SelectedIdx = row
	lay.CursorIdx = nil -- (the PC's mouse-over box stays off: the mouse is player 1's)
	lay.FirstShownIdx = 1
	if (row > lay.showcount) then
		lay.FirstShownIdx = row - lay.showcount + 1
	end
	ifelm_shellscreen_fnPlaySound(this.acceptSound)
	bfxs_Log(format("player %d joined - on row %d (%s)", p, row, bfxs_ProfileName(ifs_split_profile_listbox_contents[row])))
end

function bfxs4_Ready(this, p)
	local lay = gBFXS_Layout[p]
	local row = lay.SelectedIdx
	local sel = ifs_split_profile_listbox_contents[row or 0]
	if ((not sel) or (not sel.showstr)) then
		ifelm_shellscreen_fnPlaySound(this.errorSound)
		bfxs_Log(format("player %d: no profile on row %s - not READY", p, tostring(row)))
		return
	end
	local q = bfxs4_HeldBy(this, row, p)
	if (q) then
		ifelm_shellscreen_fnPlaySound(this.errorSound)
		bfxs_Log(format("player %d: row %d (%s) is player %d's READY profile - not READY", p, row, bfxs_ProfileName(sel), q))
		return
	end
	gBFXS_Mode[p] = 3
	ifelm_shellscreen_fnPlaySound(this.acceptSound)
end

-- one input for player 3 / 4
function bfxs4_Do(this, name, p)
	local m = gBFXS_Mode[p]
	if ((not m) or (m < 0) or (p > gBFXS_Players)) then
		return
	end
	local lay = gBFXS_Layout[p]
	if ((name == "Input_Start") or (name == "Input_Accept")) then
		if (m == 0) then
			bfxs4_Join(this, p)
		elseif (m == 1) then
			bfxs4_Ready(this, p)
		elseif ((m == 3) and (name == "Input_Start") and (this.ModeT == 3) and (this.ModeB == 3)) then
			-- his START launches too (as player 2's)
			bfxs_Log(format("player %d: START - the launch (player 1's START)", p))
			bfxs_Do(this, "Input_Start", 0)
			return
		end
	elseif (name == "Input_Back") then
		if (m == 3) then
			gBFXS_Mode[p] = 1
			ifelm_shellscreen_fnPlaySound(this.exitSound)
		elseif (m == 1) then
			gBFXS_Mode[p] = 0
			lay.SelectedIdx = nil
			lay.CursorIdx = nil
			ifelm_shellscreen_fnPlaySound(this.exitSound)
		end
	elseif ((name == "Input_GeneralUp") and (m == 1)) then
		bfxs4_ListCall(this, p, ListManager_fnNavUp)
	elseif ((name == "Input_GeneralDown") and (m == 1)) then
		bfxs4_ListCall(this, p, ListManager_fnNavDown)
	end
	ifs_split_profile_fnSetPieceVis(this)
	if (gBFXS_Mode[p] ~= m) then
		local sel = ""
		if (gBFXS_Mode[p] >= 1) then
			sel = format(" (row %s, %s)", tostring(lay.SelectedIdx),
				bfxs_ProfileName(ifs_split_profile_listbox_contents[lay.SelectedIdx or 0]))
		end
		bfxs_Log(format("player %d %s: %s -> %s%s", p, gBFXS_Short[gBFXS_Code[name]], bfxs4_ModeName(m),
			bfxs4_ModeName(gBFXS_Mode[p]), sel))
	end
end

-- the game itself giving player 3's / 4's pad (Joystick 2 / 3): BF1GameExt's pad for him is not read from then on
function bfxs4_GameInput(this, name, p, jtxt)
	if (not gBFXS_GameP[p]) then
		gBFXS_GameP[p] = 1
		bfxs_Log(format("game: %s(Joystick %s) - the game itself gives player %d's pad: BF1GameExt's pad %d is not read "
			.. "here from now on", name, jtxt, p, p))
		if ((gBFXS_PadCode[p] == gBFXS_Code[name]) and (gBFXS_Clock - (gBFXS_PadTime[p] or -10) < 0.5)) then
			bfxs_Log("  (the same press came from BF1GameExt's pad just before - not done twice)")
			return
		end
	else
		bfxs_Log(format("game: %s(Joystick %s) -> player %d", name, jtxt, p))
	end
	bfxs4_Do(this, name, p)
end

-- players 3 / 4 back to PRESS START (the profile list was just loaded)
function bfxs4_Reset(this)
	local p
	for p = 3, 4 do
		gBFXS_Mode[p] = 0
		gBFXS_GameP[p] = nil
		gBFXS_PadConn[p] = -2
		gBFXS_Layout[p].SelectedIdx = nil
		gBFXS_Layout[p].CursorIdx = nil
		gBFXS_Layout[p].FirstShownIdx = 1
		bfxs_Ask(format("pad%dreset", p))
	end
end

-- pads 3 / 4: connected or not, logged when it changes
function bfxs4_PadLog()
	local p
	for p = 3, gBFXS_Players do
		local conn = bfxs_Ask(format("pad%dconn", p))
		if (conn ~= gBFXS_PadConn[p]) then
			gBFXS_PadConn[p] = conn
			if (conn > 0) then
				bfxs_Log(format("pad %d is connected (XInput %d) - player %d can join", p, bfxs_Ask(format("pad%dindex", p)), p))
			elseif (conn == 0) then
				bfxs_Log(format("pad %d is NOT connected - player %d cannot join until it is", p, p))
			else
				bfxs_Log(format("pad %d: BF1GameExt did not answer", p))
			end
		end
	end
end

-- each frame: pads 3 / 4's presses, players 3 / 4's list cursors pulsing as the game's
function bfxs4_Update(this, fDt)
	if ((not this.ModeT) or (this.ModeT < 0) or (this.ModeT > 3)) then
		return
	end
	local p
	for p = 3, gBFXS_Players do
		if (not gBFXS_GameP[p]) then
			local n = 0
			local e = bfxs_Ask(format("pad%d", p))
			while ((e > 0) and (n < 8)) do
				local name = gBFXS_Name[e]
				if (name) then
					gBFXS_PadCode[p] = e
					gBFXS_PadTime[p] = gBFXS_Clock
					bfxs_Log(format("pad %d: %s -> player %d (%s)", p, gBFXS_Short[e], p, bfxs4_ModeName(gBFXS_Mode[p])))
					bfxs4_Do(this, name, p)
				end
				n = n + 1
				e = 0
				if (this.ModeT and (this.ModeT >= 0) and (this.ModeT < 4)) then
					e = bfxs_Ask(format("pad%d", p))
				end
			end
		end
	end
	gBFXS_Pulse = gBFXS_Pulse + fDt * gBFXS_PulseDir
	if (gBFXS_Pulse > 1) then
		gBFXS_Pulse = 1
		gBFXS_PulseDir = -1
	elseif (gBFXS_Pulse < 0.5) then
		gBFXS_Pulse = 0.5
		gBFXS_PulseDir = 1
	end
	for p = 3, gBFXS_Players do
		if (gBFXS_Mode[p] == 1) then
			local c = this["bfx" .. p .. "Profile"]
			if (c and c.listbox and c.listbox.cursor) then
				IFObj_fnSetAlpha(c.listbox.cursor, gBFXS_Pulse)
			end
		end
	end
end

-- texts in English where the game has none
function bfxs4_FixTexts(this)
	local p
	for p = 3, 4 do
		bfxs_SetText(this["bfx" .. p .. "Press"], "ifs.split.starttojoin", "Press START to join")
		bfxs_SetText(this["bfx" .. p .. "Ready"], "ifs.split.ready", "Ready")
		bfxs_SetText(this["bfx" .. p .. "Accept"].helpstr, "common.accept", "Accept")
		gIFShellScreenTemplate_fnMoveIcon(this["bfx" .. p .. "Accept"])
	end
end

-- 21j-r27: an icon of a BACK / ACCEPT pair made the pad's B / A (gBFX_PadIcons): the 2020 gamepad sheet (gamepad_xbox360) at
-- gGamepadTexture's cell for it, a square 22 units tall where the game's icon was (ACCEPT's icon is placed again by the
-- game's fnMoveIcon, 20 x 20). Only before the game makes the icon (its table is read then). 1 = done.
function bfxs_PadIcon(icon, which)
	if ((not gBFX_PadIcons) or (not icon) or (not gGamepadTexture) or (not gGamepadTexture[which])) then
		return nil
	end
	local g = gGamepadTexture[which]
	local cx = ((icon.localpos_l or 0) + (icon.localpos_r or 0)) * 0.5
	local cy = ((icon.localpos_t or 0) + (icon.localpos_b or 0)) * 0.5
	icon.texture = "gamepad_xbox360"
	icon.uvs_l = g.ul
	icon.uvs_t = g.ut
	icon.uvs_r = g.ur
	icon.uvs_b = g.ub
	icon.localpos_l = cx - 11
	icon.localpos_r = cx + 11
	icon.localpos_t = cy - 11
	icon.localpos_b = cy + 11
	return 1
end

-- 21j-r27: players 1 and 2's BACK / ACCEPT (the game's BackGroupT / B, AcceptGroupT / B) with the pad's B / A - while the
-- game makes the screen
function bfxs_PadIcons(this)
	if (not gBFX_PadIcons) then
		return
	end
	local n = 0
	local k, v
	for k, v in { BackGroupT = "B", BackGroupB = "B", AcceptGroupT = "A", AcceptGroupB = "A" } do
		if (this[k] and bfxs_PadIcon(this[k].icon, v)) then
			n = n + 1
		end
	end
	bfxs_Log(format("21j-r27: players 1 and 2's BACK / ACCEPT: %d of 4 icons the pad's B / A (gamepad_xbox360)%s", n,
		gGamepadTexture and "" or " - the game's gGamepadTexture is missing"))
end

-- 21j-r27: the last input was a pad's (ScriptCB_IsLastInputFromJoystick: 1 / non-zero) - nil for the mouse / keyboard
function bfxs_LastFromPad()
	local r = ScriptCB_IsLastInputFromJoystick()
	if (r and (r ~= 0)) then
		return 1
	end
	return nil
end

-- 21j-r27: a click on the BACK button at the bottom left - player 1's Input_Accept with the mouse over it (the game made it the
-- screen's CurButton "_back"), never a pad's A
function bfxs_MouseBackClick(this, name, Joystick)
	if ((not gBFX_MouseBack) or (name ~= "Input_Accept") or (this.CurButton ~= "_back")) then
		return nil
	end
	if ((type(Joystick) == "number") and (Joystick > 0.5)) then
		return nil
	end
	if (bfxs_LastFromPad()) then
		return nil
	end
	return 1
end

-- 21j-r27: the sign-in left with the mouse's BACK - as player 1's B on his profile list (the game's own lines), whatever
-- the players are on
function bfxs_MouseBack(this)
	bfxs_Log(format("mouse: BACK clicked (%s) - the sign-in screen closed, as player 1's B on his list", bfxs_Modes(this)))
	if (gBFXS_Four) then
		gBFXS_Mode[3] = -1
		gBFXS_Mode[4] = -1
	end
	this.CurButton = nil
	ScriptCB_UnbindController(2) -- (the game's: don't care about the second controller anymore)
	ScriptCB_ReadAllControllers(nil)
	ScriptCB_PopScreen()
end

-- 21j-r28: the shell's sound back after a profile switch (gBFX_SoundFix) - as the game's login does it: when
-- ScriptCB_GetMixConfigChanged() says the sound engine was restarted (it clears that), the shell's sound data, its voice /
-- music streams and its movie stream again (ifs_opt_sound_closeShellSound / restoreShellSound), the music started again
-- at once (its stream is new; the game's own music update would start it only after its 10 s interval) and the background
-- movie bgMovie again (looped, as the screen's own)
function bfxs_SoundRestore(where, bgMovie)
	if ((not gBFX_SoundFix) or (not ScriptCB_GetMixConfigChanged)) then
		return
	end
	local r = ScriptCB_GetMixConfigChanged()
	if ((not r) or (r == 0)) then
		bfxs_Log(format("sound: %s - the sound engine was not restarted, nothing to put back", where))
		return
	end
	if ((not ifs_opt_sound_closeShellSound) or (not ifs_opt_sound_restoreShellSound)) then
		bfxs_Log(format("sound: %s - the sound engine WAS restarted, but the game's restore (ifs_opt_sound) is not in this shell",
			where))
		return
	end
	ifs_opt_sound_closeShellSound(nil)
	ifs_opt_sound_restoreShellSound(nil)
	local music = (ifs_sp and ifs_sp.music) or "shell_soundtrack"
	ScriptCB_SetShellMusic()
	ScriptCB_SetShellMusic(music)
	local movie = "none"
	if (bgMovie) then
		local keep = gMovieDisabled
		gMovieDisabled = nil
		ifelem_shellscreen_fnStartMovie(bgMovie, 1, nil, 1)
		gMovieDisabled = keep
		movie = bgMovie
	end
	bfxs_Log(format("sound: %s - the sound engine was restarted (a profile switch): the shell's sounds, music and movie streams "
		.. "put back (the game's own restore, as its login does), music %s again, background movie %s", where, music, movie))
end

-- a BACK / ACCEPT pair as the game's (BackGroupT / AcceptGroupT); 21j-r27: the pad's B / A (gBFX_PadIcons)
function bfxs4_BackGroup(sx, sy, dx)
	local c = NewIFContainer {
		ScreenRelativeX = sx,
		ScreenRelativeY = sy,
		x = dx,
		y = -20,
		icon = NewIFImage { ZPos = 200, texture = "btnb", localpos_l = 0, localpos_t = -11, localpos_r = 20, localpos_b = 11,
			inert = 1 },
		helpstr = NewIFText { string = "common.back", font = "gamefont_medium", textw = 460, x = 25, y = -15, halign = "left",
			inert = 1, nocreatebackground = 1 },
	}
	bfxs_PadIcon(c.icon, "B")
	return c
end

function bfxs4_AcceptGroup(sx, sy, dx)
	local c = NewIFContainer {
		ScreenRelativeX = sx,
		ScreenRelativeY = sy,
		x = dx,
		y = -20,
		icon = NewIFImage { ZPos = 200, texture = "btna", localpos_l = -10, localpos_t = -11, localpos_r = 10, localpos_b = 11 },
		helpstr = NewIFText { string = "common.accept", font = "gamefont_medium", textw = 460, x = -460, y = -15,
			halign = "right", nocreatebackground = 1 },
	}
	bfxs_PadIcon(c.icon, "A")
	return c
end

-- While the game makes the screen (ScriptCB_AddIFScreen, before its pieces exist in the game): players 1 and 2's pieces
-- moved into the top quarters, players 3 and 4's made, the vertical line.
function bfxs4_Build(this)
	local r, b = ScriptCB_GetSafeScreenInfo()
	local lw = this.ProfileT.listbox.width
	local lh = this.ProfileT.listbox.height
	local QX = gBFXS_QX
	local QT = gBFXS_QText
	local QL = gBFXS_QList
	local QH = gBFXS_QHelp
	-- the vertical line: the game's middle line (ifssplit_profile_reposition_items) stood up
	this.bfxvline = NewIFImage {
		texture = "gray_rect",
		alpha = 0.8,
		localpos_l = r * 0.5 - 2, localpos_t = -100,
		localpos_r = r * 0.5 + 2, localpos_b = b + 100,
		inertUVs = 1,
		ScreenRelativeX = 0,
		ScreenRelativeY = 0,
		ColorR = 192, ColorG = 192, ColorB = 255,
	}
	-- player 1: top left; player 2: top right
	this.pressstartT.ScreenRelativeX = QX[1]
	this.pressstartT.ScreenRelativeY = QT[1]
	this.pressstartB.ScreenRelativeX = QX[2]
	this.pressstartB.ScreenRelativeY = QT[2]
	this.ReadyT.ScreenRelativeX = QX[1]
	this.ReadyT.ScreenRelativeY = QT[1]
	this.ReadyB.ScreenRelativeX = QX[2]
	this.ReadyB.ScreenRelativeY = QT[2]
	this.ProfileT.ScreenRelativeX = QX[1]
	this.ProfileT.listbox.y = b * QL[1]
	this.ProfileB.ScreenRelativeX = QX[2]
	this.ProfileB.listbox.y = b * QL[2]
	this.BackGroupT.ScreenRelativeX = QX[1]
	this.BackGroupT.ScreenRelativeY = QH[1]
	this.BackGroupT.x = lw * -0.5
	this.AcceptGroupT.ScreenRelativeX = QX[1]
	this.AcceptGroupT.ScreenRelativeY = QH[1]
	this.AcceptGroupT.x = lw * 0.5
	this.BackGroupB.ScreenRelativeX = QX[2]
	this.BackGroupB.ScreenRelativeY = QH[2]
	this.BackGroupB.x = lw * -0.5
	this.AcceptGroupB.ScreenRelativeX = QX[2]
	this.AcceptGroupB.ScreenRelativeY = QH[2]
	this.AcceptGroupB.x = lw * 0.5
	-- PRESS START BUTTON TO LAUNCH: under player 1's READY (his START launches), a little to the left (the long line
	-- clears the vertical one)
	this.pressstart.ScreenRelativeX = QX[1] - 0.01
	this.pressstart.ScreenRelativeY = QT[1]
	this.pressstart.y = 28
	-- the profile names over READY (Battlefront II: "Player 1" / "Ready")
	this.bfxNameT = NewIFText { string = "", font = "gamefont_large", textw = 460, y = -38, ScreenRelativeX = QX[1],
		ScreenRelativeY = QT[1], nocreatebackground = 1 }
	this.bfxNameB = NewIFText { string = "", font = "gamefont_large", textw = 460, y = -38, ScreenRelativeX = QX[2],
		ScreenRelativeY = QT[2], nocreatebackground = 1 }
	-- players 3 and 4: copies of the game's pieces in the bottom quarters
	local p
	for p = 3, 4 do
		this["bfx" .. p .. "Press"] = NewIFText { string = "ifs.split.starttojoin", font = "gamefont_medium", valign = "vcenter",
			textw = 460, texth = 120, ScreenRelativeX = QX[p], ScreenRelativeY = QT[p], nocreatebackground = 1 }
		this["bfx" .. p .. "Ready"] = NewIFText { string = "ifs.split.ready", font = "gamefont_large", textw = 460, y = -8,
			ScreenRelativeX = QX[p], ScreenRelativeY = QT[p], nocreatebackground = 1 }
		this["bfx" .. p .. "Name"] = NewIFText { string = "", font = "gamefont_large", textw = 460, y = -38,
			ScreenRelativeX = QX[p], ScreenRelativeY = QT[p], nocreatebackground = 1 }
		local t = ifs_split_profile_listbox_layoutT
		local lay = { showcount = t.showcount, yHeight = t.yHeight, ySpacing = t.ySpacing, width = t.width, slider = t.slider,
			CreateFn = t.CreateFn, PopulateFn = t.PopulateFn }
		gBFXS_Layout[p] = lay
		local c = NewIFContainer {
			ScreenRelativeX = QX[p],
			ScreenRelativeY = 0.0,
			listbox = NewButtonWindow { ZPos = 200, x = 0, y = b * QL[p], width = lw, height = lh },
		}
		ListManager_fnInitList(c.listbox, lay)
		-- no mouse spots on them (the mouse is player 1's)
		local i
		for i = 1, lay.showcount do
			if (c.listbox[i]) then
				c.listbox[i].bHotspot = nil
			end
		end
		if (c.listbox.sliderbg) then
			c.listbox.sliderbg.bHotspot = nil
		end
		this["bfx" .. p .. "Profile"] = c
		this["bfx" .. p .. "Back"] = bfxs4_BackGroup(QX[p], QH[p], lw * -0.5)
		this["bfx" .. p .. "Accept"] = bfxs4_AcceptGroup(QX[p], QH[p], lw * 0.5)
		gBFXS_Mode[p] = -1
	end
	bfxs_Log(format("4-player sign-in built (%d players): four quarters - the game's middle line and a vertical one (gray_rect, "
		.. "192/192/255 at 0.8, 4 units); screen %s x %s, lists %s x %s at x %.2f / %.2f, y %.3f / %.3f of the screen",
		gBFXS_Players, tostring(r), tostring(b), tostring(lw), tostring(lh), QX[1], QX[2], QL[1], QL[3]))
end


gBFXS_Orig = {
	Enter = ifs_split_profile.Enter,
	Exit = ifs_split_profile.Exit,
	Update = ifs_split_profile.Update,
}

function bfxs_Modes(this)
	return format("ModeT %s ModeB %s", tostring(this.ModeT), tostring(this.ModeB))
end

function bfxs_ProfileName(entry)
	if (entry and entry.showstr) then
		return ScriptCB_ununicode(entry.showstr)
	end
	return "?"
end

-- the sign-in list, logged
function bfxs_ListLog()
	local list = ifs_split_profile_listbox_contents
	local s = ""
	local i = 1
	while (list[i]) do
		local slot = "file"
		if (list[i].memslot) then
			slot = format("active slot %s", tostring(list[i].memslot))
		end
		s = s .. format("  [%d] %s (%s)", i, bfxs_ProfileName(list[i]), slot)
		i = i + 1
	end
	bfxs_Log(format("profile list (ScriptCB_GetLoginList, split): %d -%s", i - 1, s))
end

-- 21g-r2: the row of the list with that name (any case), nil if none
function bfxs_RowByName(name)
	local list = ifs_split_profile_listbox_contents
	local want = strupper(name)
	local i = 1
	while (list[i]) do
		if (strupper(bfxs_ProfileName(list[i])) == want) then
			return i
		end
		i = i + 1
	end
	return nil
end

-- 21g-r2: PLAYER 2 / 3 / 4 as temporary profiles ([Splitscreen] TempProfiles): added at the end of the list where no
-- profile has that name. Never saved, so never in the profile screen: player 2's is made by BF1GameExt right before the
-- game loads the profiles (a copy of player 1's, as the game's own PLAYER 2 when there are not 2 profiles) and kept from
-- ifs_saveop; players 3 / 4's are names only.
function bfxs_AddTemps(this)
	if (not gBFX_TempProfiles) then
		return
	end
	local list = ifs_split_profile_listbox_contents
	-- the game's own temporary profile (its list makes one in "active slot 2" when there are not 2 profiles) tagged now:
	-- the launch's slot swap changes the rows' memslot, the tag stays
	local i = 1
	while (list[i]) do
		if (list[i].memslot == 2) then
			list[i].bfxgametemp = 1
			bfxs_Log(format("row %d (%s) is the game's own temporary profile - not saved either", i, bfxs_ProfileName(list[i])))
		end
		i = i + 1
	end
	local last = 2
	if (gBFXS_Four) then
		last = gBFXS_Players
	end
	local added = ""
	local k
	for k = 2, last do
		local name = format("PLAYER %d", k)
		if (not bfxs_RowByName(name)) then
			list[getn(list) + 1] = { showstr = ScriptCB_tounicode(name), bfxtemp = k }
			added = added .. format("  [%d] %s", getn(list), name)
		end
	end
	if (added ~= "") then
		ListManager_fnFillContents(this.ProfileT.listbox, list, ifs_split_profile_listbox_layoutT)
		ListManager_fnFillContents(this.ProfileB.listbox, list, ifs_split_profile_listbox_layoutB)
		bfxs_Log("temporary profiles added (never saved):" .. added)
	else
		bfxs_Log(format("temporary profiles: none added (PLAYER 2 - %d are in the list already)", last))
	end
end

-- 21g-r2: player p's own row - PLAYER p (temporary or saved) - when nobody else is on it, else nil
function bfxs_OwnRow(this, p)
	if (not gBFX_TempProfiles) then
		return nil
	end
	local row = bfxs_RowByName(format("PLAYER %d", p))
	if (not row) then
		return nil
	end
	if (gBFXS_Four) then
		if (bfxs4_UsedBy(this, row, p, nil)) then
			return nil
		end
	elseif (row == ifs_split_profile_listbox_layoutT.SelectedIdx) then
		return nil
	end
	return row
end

-- 21g-r2: presses refused for the temporary profiles (1 = refused): player 1 READY on one (he signs in with a saved
-- profile); the launch while player 2's is one that cannot be made now
function bfxs_TempGate(this, name, joy)
	if (not gBFX_TempProfiles) then
		return nil
	end
	local list = ifs_split_profile_listbox_contents
	local lt = ifs_split_profile_listbox_layoutT
	local lb = ifs_split_profile_listbox_layoutB
	if ((joy < 0.5) and ((name == "Input_Accept") or (name == "Input_Start")) and ((this.ModeT == 1) or (this.ModeT == 2))) then
		local sel = list[lt.SelectedIdx or 0]
		if (sel and (sel.bfxtemp or sel.bfxgametemp)) then
			ifelm_shellscreen_fnPlaySound(this.errorSound)
			bfxs_Log(format("player 1: %s is a temporary profile - player 1 signs in with a saved one", bfxs_ProfileName(sel)))
			return 1
		end
	end
	if ((this.ModeT == 3) and (this.ModeB == 3) and ((name == "Input_Start") or ((name == "Input_Accept") and (joy < 0.5)))) then
		local sb = list[lb.SelectedIdx or 0]
		local st = list[lt.SelectedIdx or 0]
		if (sb and sb.bfxtemp) then
			local why = nil
			if (st and (st.memslot == 2)) then
				why = "player 1's profile is in the second profile slot"
			elseif (bfxs_Ask("tempok") ~= 1) then
				why = "BF1GameExt cannot make it now"
			end
			if (why) then
				ifelm_shellscreen_fnPlaySound(this.errorSound)
				bfxs_Log(format("START: player 2's temporary profile %s cannot be used (%s) - pick a saved profile for player 2",
					bfxs_ProfileName(sb), why))
				return 1
			end
		end
	end
	return nil
end

-- the row of the list with that active slot (nil if none), not the row skip
function bfxs_SlotRow(slot, skip)
	local list = ifs_split_profile_listbox_contents
	local i = 1
	while (list[i]) do
		if ((list[i].memslot == slot) and (i ~= skip)) then
			return i
		end
		i = i + 1
	end
	return nil
end

-- 21f: the row of the signed-in profile by name (the game lists it as a file when it has no active slot 1 for it)
function bfxs_NameRow()
	if (not ScriptCB_GetCurrentProfileName) then
		return nil
	end
	local me = ScriptCB_GetCurrentProfileName()
	if (not me) then
		return nil
	end
	me = ScriptCB_ununicode(me)
	local list = ifs_split_profile_listbox_contents
	local i = 1
	while (list[i]) do
		if ((not list[i].memslot) and (bfxs_ProfileName(list[i]) == me)) then
			return i
		end
		i = i + 1
	end
	return nil
end

function bfxs_SetRow(listbox, layout, row)
	layout.SelectedIdx = row
	layout.CursorIdx = row
	layout.FirstShownIdx = 1
	if (row > layout.showcount) then
		layout.FirstShownIdx = row - layout.showcount + 1
	end
	ListManager_fnFillContents(listbox, ifs_split_profile_listbox_contents, layout)
end

-- the bottom "press START" text: the PC game counts only its own pad (its check would say "needs 2 controllers" all
-- the time - it is kept from running, Update), so it follows pad 2 as BF1GameExt sees it. Written when it changes
-- (or force: on Enter).
function bfxs_Pad2Text(this, force)
	local conn = bfxs_Ask("pad2conn")
	if ((conn == gBFXS_Conn) and (not force)) then
		return
	end
	if (conn > 0) then
		bfxs_SetText(this.pressstartB, "ifs.split.starttojoin", "Press START to join")
	else
		bfxs_SetText(this.pressstartB, "ifs.split.needs2controllers", "Player 2: connect a second controller")
	end
	if (conn ~= gBFXS_Conn) then
		gBFXS_Conn = conn
		if (conn > 0) then
			bfxs_Log(format("pad 2 is connected (XInput %d) - \"press START to join\"", bfxs_Ask("pad2index")))
		elseif (conn == 0) then
			bfxs_Log("pad 2 is NOT connected - \"connect a second controller\"")
		else
			bfxs_Log("pad 2: BF1GameExt did not answer")
		end
	end
end

function bfxs_FixTexts(this)
	bfxs_SetText(this.pressstartT, "ifs.split.starttojoin", "Press START to join")
	bfxs_SetText(this.pressstart, "ifs.split.starttolaunch", "Press START to launch")
	bfxs_SetText(this.ReadyT, "ifs.split.ready", "Ready")
	bfxs_SetText(this.ReadyB, "ifs.split.ready", "Ready")
	if (this.AcceptGroupT) then
		bfxs_SetText(this.AcceptGroupT.helpstr, "common.accept", "Accept")
	end
	if (this.AcceptGroupB) then
		bfxs_SetText(this.AcceptGroupB.helpstr, "common.accept", "Accept")
	end
	bfxs_Pad2Text(this, 1)
	if (gBFXS_Four) then
		bfxs4_FixTexts(this)
	end
end

-- One input for one player (0 top, 1 bottom), with the PC's extras
function bfxs_Do(this, name, joy)
	if (bfxs_TempGate(this, name, joy)) then
		return
	end
	if (gBFXS_Four and bfxs4_Gate(this, name, joy)) then
		return
	end
	local before = bfxs_Modes(this)
	local beforeB = this.ModeB
	gBFXS_Inside = 1
	if (joy < 0.5) then
		if ((name == "Input_Accept") and ((this.ModeT == 0) or ((this.ModeT == 3) and (this.ModeB == 3)))) then
			-- A where START is asked for: join / launch
			ifs_split_profile.Input_Start(this, 0)
		else
			ifs_split_profile[name](this, 0)
		end
	else
		if ((name == "Input_Start") and (this.ModeT == 3) and (this.ModeB == 3)) then
			-- player 2's START launches too (on the console only player 1's did)
			ifs_split_profile.Input_Start(this, 0)
		elseif ((name == "Input_Accept") and (this.ModeB == 0)) then
			-- A joins too
			ifs_split_profile.Input_Start(this, 1)
		else
			ifs_split_profile[name](this, 1)
		end
		-- just joined: player 2 starts on PLAYER 2 (21g-r2, TempProfiles), else the second active profile, if the list has one
		if ((beforeB == 0) and (this.ModeB == 1)) then
			local row = bfxs_OwnRow(this, 2)
			if (not row) then
				row = bfxs_SlotRow(2, ifs_split_profile_listbox_layoutT.SelectedIdx)
			end
			if (row and (row ~= ifs_split_profile_listbox_layoutB.SelectedIdx)) then
				bfxs_SetRow(this.ProfileB.listbox, ifs_split_profile_listbox_layoutB, row)
				ifs_split_profile_fnSetPieceVis(this)
			end
			local sel = ifs_split_profile_listbox_contents[ifs_split_profile_listbox_layoutB.SelectedIdx]
			bfxs_Log(format("player 2 joined - on row %s (%s)", tostring(ifs_split_profile_listbox_layoutB.SelectedIdx),
				bfxs_ProfileName(sel)))
		end
	end
	gBFXS_Inside = nil
	local after = bfxs_Modes(this)
	if (after ~= before) then
		bfxs_Log(format("player %d %s: %s -> %s", joy + 1, gBFXS_Short[gBFXS_Code[name]], before, after))
	end
end

-- Every input handler of the screen goes through here
function bfxs_Input(this, name, Joystick)
	if (gBFXS_Inside) then
		-- the screen calling itself (Input_Start -> Input_Accept): straight on
		local j = Joystick
		if (type(j) ~= "number") then
			j = 0
		end
		gBFXS_Orig[name](this, j)
		return
	end
	local jtxt = tostring(Joystick)
	if ((not this.ModeT) or (this.ModeT < 0) or (this.ModeT > 3)) then
		bfxs_Log(format("game: %s(Joystick %s) while the screen is busy (%s) - ignored", name, jtxt, bfxs_Modes(this)))
		return
	end
	-- 21j-r27: a click on the BACK button at the bottom left leaves the sign-in
	if (bfxs_MouseBackClick(this, name, Joystick)) then
		bfxs_MouseBack(this)
		return
	end
	if (gBFXS_Four and (type(Joystick) == "number") and (Joystick > 1.5)) then
		-- 21g: the game itself gave player 3's / 4's pad
		local p = 3
		if (Joystick > 2.5) then
			p = 4
		end
		bfxs4_GameInput(this, name, p, jtxt)
		return
	end
	if ((type(Joystick) == "number") and (Joystick > 0.5)) then
		-- the game itself gave player 2's pad
		if (not gBFXS_GameP2) then
			gBFXS_GameP2 = 1
			bfxs_Log(format("game: %s(Joystick %s) - the game itself gives player 2's pad: BF1GameExt's pad 2 is not read "
				.. "here from now on", name, jtxt))
			if ((gBFXS_LastPadCode == gBFXS_Code[name]) and (gBFXS_Clock - gBFXS_LastPadTime < 0.5)) then
				bfxs_Log("  (the same press came from BF1GameExt's pad 2 just before - not done twice)")
				return
			end
		else
			bfxs_Log(format("game: %s(Joystick %s) -> player 2 (%s)", name, jtxt, bfxs_Modes(this)))
		end
		bfxs_Do(this, name, 1)
		return
	end
	bfxs_Log(format("game: %s(Joystick %s) -> player 1 (%s)", name, jtxt, bfxs_Modes(this)))
	bfxs_Do(this, name, 0)
end

-- the wrappers (Lua 4: the handler's name is an upvalue)
function bfxs_Wrap(name)
	gBFXS_Orig[name] = ifs_split_profile[name]
	ifs_split_profile[name] = function(this, Joystick)
		bfxs_Input(this, %name, Joystick)
	end
end
bfxs_Wrap("Input_Accept")
bfxs_Wrap("Input_Back")
bfxs_Wrap("Input_Start")
bfxs_Wrap("Input_GeneralUp")
bfxs_Wrap("Input_GeneralDown")
bfxs_Wrap("Input_GeneralLeft")
bfxs_Wrap("Input_GeneralRight")

ifs_split_profile.Enter = function(this, bFwd)
	local fromList = this.bFromLoadFileList
	gBFXS_Inside = nil
	bfxs_Log(format("Enter(bFwd %s, from the profile list %s) - %s", tostring(bFwd), tostring(fromList), bfxs_Modes(this)))
	if (gBFXS_Four) then
		if (fromList) then
			bfxs4_Reset(this) -- players 3 / 4 to PRESS START
		elseif (bFwd) then
			-- the profile list is loaded first (ifs_saveop): players 3 / 4 hidden until it is back
			gBFXS_Mode[3] = -1
			gBFXS_Mode[4] = -1
		end
	end
	gBFXS_Orig.Enter(this, bFwd)
	if (fromList) then
		bfxs_ListLog()
		-- player 1 starts on the profile he signed in with (the list's active slot 1, else his profile's row by name),
		-- not the login list's row
		local row = bfxs_SlotRow(1, nil)
		if (not row) then
			row = bfxs_NameRow()
		end
		if (row and (row ~= ifs_split_profile_listbox_layoutT.SelectedIdx)) then
			bfxs_SetRow(this.ProfileT.listbox, ifs_split_profile_listbox_layoutT, row)
			ifs_split_profile_fnSetPieceVis(this)
		end
		bfxs_Log(format("player 1 on row %s (%s)", tostring(ifs_split_profile_listbox_layoutT.SelectedIdx),
			bfxs_ProfileName(ifs_split_profile_listbox_contents[ifs_split_profile_listbox_layoutT.SelectedIdx])))
		bfxs_AddTemps(this) -- 21g-r2
		bfxs_Ask("pad2reset")
		-- 21j-r28: the profile list made player 2's profile current and back - the sound engine restarted
		bfxs_SoundRestore("the sign-in's profile list", this.movieBackground)
	end
	if (this.ModeT) then
		bfxs_FixTexts(this)
	end
	if (gBFXS_Four) then
		bfxs4_Vis(this)
		bfxs_Log(format("Enter done - %s, player 3 %s, player 4 %s", bfxs_Modes(this), bfxs4_ModeName(gBFXS_Mode[3]),
			bfxs4_ModeName(gBFXS_Mode[4])))
		return
	end
	bfxs_Log(format("Enter done - %s", bfxs_Modes(this)))
end

ifs_split_profile.Exit = function(this, bFwd)
	bfxs_Log(format("Exit(bFwd %s) - %s", tostring(bFwd), bfxs_Modes(this)))
	gBFXS_Orig.Exit(this, bFwd)
end

ifs_split_profile.Update = function(this, fDt)
	gBFXS_Inside = nil
	-- the game's controller check (its "needs 2 controllers" text every 0.3 s) kept from running: ours below
	this.fControllerCheckTimer = 1000
	gBFXS_Orig.Update(this, fDt)
	gBFXS_Clock = gBFXS_Clock + fDt
	gBFXS_PadCheck = gBFXS_PadCheck - fDt
	if (this.ModeT and (gBFXS_PadCheck < 0)) then
		gBFXS_PadCheck = 0.3
		bfxs_Pad2Text(this, nil)
		if (gBFXS_Four) then
			bfxs4_PadLog()
		end
	end
	if (gBFXS_Four) then
		bfxs4_Update(this, fDt)
	end
	-- pad 2's presses (BF1GameExt reads pad 2; one event per question)
	if (this.ModeT and (this.ModeT >= 0) and (this.ModeT < 4) and (not gBFXS_GameP2)) then
		local n = 0
		local e = bfxs_Ask("pad2")
		while ((e > 0) and (n < 8)) do
			local name = gBFXS_Name[e]
			if (name) then
				gBFXS_LastPadCode = e
				gBFXS_LastPadTime = gBFXS_Clock
				bfxs_Log(format("pad 2: %s -> player 2 (%s)", gBFXS_Short[e], bfxs_Modes(this)))
				bfxs_Do(this, name, 1)
			end
			n = n + 1
			e = 0
			if (this.ModeT and (this.ModeT >= 0) and (this.ModeT < 4)) then
				e = bfxs_Ask("pad2")
			end
		end
	end
end

-- the launch, logged
gBFXS_StartLoad = ifs_split_profile_StartLoadProfile
ifs_split_profile_StartLoadProfile = function(profile1, profile2)
	local this = ifs_split_profile
	local t = ifs_split_profile_listbox_contents[ifs_split_profile_listbox_layoutT.SelectedIdx]
	local b = ifs_split_profile_listbox_contents[ifs_split_profile_listbox_layoutB.SelectedIdx]
	local f1 = "already active"
	local f2 = "already active"
	local temp2 = nil
	-- 21g-r2: player 2 on a temporary profile: BF1GameExt makes it now (nothing to load); on the game's own temporary
	-- one (its "active slot 2"): marked so it is not saved either; on a saved one: saved as the game does it
	if (gBFX_TempProfiles) then
		if (b and b.bfxtemp) then
			if (bfxs_Ask("tempprofile " .. bfxs_ProfileName(b)) == 1) then
				if (ScriptCB_SetProfileNotDirty) then
					ScriptCB_SetProfileNotDirty(2)
				end
				profile2 = nil
				temp2 = 1
			else
				bfxs_Log("LAUNCH: player 2's temporary profile could NOT be made - the game looks for it as a file (it fails)")
			end
		elseif (b and b.bfxgametemp) then
			bfxs_Ask("tempmark " .. bfxs_ProfileName(b))
		else
			bfxs_Ask("tempclear")
		end
	end
	if (profile1) then
		f1 = "loaded from its file"
	end
	if (profile2) then
		f2 = "loaded from its file"
	elseif (temp2) then
		f2 = "temporary - made by BF1GameExt, never saved"
	end
	bfxs_Log(format("LAUNCH: player 1 = %s (%s, side %s), player 2 = %s (%s, side %s); split screen %s",
		bfxs_ProfileName(t), f1, tostring(this.SideT), bfxs_ProfileName(b), f2, tostring(this.SideB),
		tostring(ScriptCB_IsSplitscreen())))
	if (gBFXS_Four) then
		-- 21g-r2: the matches get as many players as signed in; the pads of the ones who did are players 3 / 4's (in that
		-- order - player 4 alone plays as player 3)
		local joined = 2
		local pads = {}
		local p
		for p = 3, gBFXS_Players do
			if (gBFXS_Mode[p] == 3) then
				joined = joined + 1
				local x = -1
				if (not gBFXS_GameP[p]) then
					x = bfxs_Ask(format("pad%dindex", p))
				end
				pads[getn(pads) + 1] = x
				bfxs_Log(format("LAUNCH: player %d = %s (READY on pad XInput %d - a name only: the game loads 2 profiles)%s", p,
					bfxs_ProfileName(ifs_split_profile_listbox_contents[gBFXS_Layout[p].SelectedIdx or 0]), x,
					(joined < p) and format(" - he plays as player %d", joined) or ""))
			else
				bfxs_Log(format("LAUNCH: player %d did not join", p))
			end
		end
		local set = bfxs_Ask(format("signin %d %d %d", joined, pads[1] or -1, pads[2] or -1))
		bfxs_Log(format("LAUNCH: %d of %d players signed in - the match has %d players", joined, gBFXS_Players, set))
	end
	gBFXS_StartLoad(profile1, profile2)
end
gBFXS_LoadOk = ifs_split_profile_LoadProfileSuccess
ifs_split_profile_LoadProfileSuccess = function()
	bfxs_Log("profiles loaded - the split screen game modes next (ifs_sp)")
	-- 21j-r28: the profiles loaded (and player 2's made current) - the sound engine restarted; the sign-in's background
	-- movie goes on behind the game modes (the game keeps it there: gMovieDisabled)
	bfxs_SoundRestore("the launch's profile loading", ifs_split_profile.movieBackground)
	gBFXS_LoadOk()
end
gBFXS_LoadCancel = ifs_split_profile_LoadProfileCancel
ifs_split_profile_LoadProfileCancel = function()
	bfxs_Log("profile load FAILED or cancelled - back to the sign-in screen")
	gBFXS_LoadCancel()
	bfxs_SoundRestore("the cancelled profile loading", ifs_split_profile.movieBackground) -- 21j-r28
end

-- 21j-r27: players 1 and 2's BACK / ACCEPT with the pad's B / A - while the game makes the screen
if (gBFX_Phase == "build") then
	bfxs_PadIcons(ifs_split_profile)
elseif (gBFX_PadIcons) then
	bfxs_Log(format("21j-r27: the sign-in was made before this script ran (%s) - players 1 and 2's BACK / ACCEPT keep the game's icons",
		gBFX_Phase or "?"))
end

-- 21g: the 4-player sign-in - made while the game makes the screen (its pieces are made right after this script)
gBFXS_Players = gBFX_Players or 2
if (gBFX_SignIn4 and (gBFXS_Players >= 3)) then
	if (gBFX_Phase == "build") then
		bfxs4_Build(ifs_split_profile)
		gBFXS_Four = 1
		gBFXS_SetPieceVis = ifs_split_profile_fnSetPieceVis
		ifs_split_profile_fnSetPieceVis = function(this, a)
			gBFXS_SetPieceVis(this, a)
			bfxs4_Vis(this)
		end
	else
		bfxs_Log(format("4-player sign-in NOT built: the screen was made before this script ran (%s) - the 2-player one",
			gBFX_Phase or "?"))
	end
end

if (gBFXS_Four) then
	bfxs_Log(format("ifs_split_profile patched (%s, %s): the 4-PLAYER sign-in (%d players) - player 1 = the game's input, "
		.. "players 2 / 3 / 4 = pads 2 / 3 / 4", gBFX_Phase or "?", gBFX_Screen or "?", gBFXS_Players))
else
bfxs_Log(format("ifs_split_profile patched (%s, %s): player 1 = the game's input, player 2 = pad 2", gBFX_Phase or "?",
	gBFX_Screen or "?"))
end
end

-- ---------------------------------------------------------------------------------------------------------------------
-- 21f: Galactic Conquest in split screen, as the Classic Collection has it: the map list (ifs_meta_opts: the split
-- screen list, the 4 conquests with both sides) -> LAUNCH -> PICK YOUR SIDE (ifs_meta_top, the console's own screen,
-- still in the 2020 shell: player 1's row on top, player 2's row below, left / right picks a side, the picked one lit)
-- -> LAUNCH -> the galaxy. Same side = CO-OP (the AI runs the other side), other sides = VERSUS (player 2 runs the other
-- side on the galaxy map - the game gives that screen both pads - and the battles are player 1 against player 2).
-- The PC script launched straight from the map list with player 2 on player 1's opponent (the AI on it), and read the
-- full list's row while it showed the split list: the split list is used for all of it now. At the launch,
-- metagame_state_local.mJoystickTeams[2] (player 2's team) and the AI control are set from the picks; the game's
-- ScriptCB_SetMetagameTeams puts both local players on their teams for each battle. mIsSplitscreen = 1: a saved split
-- screen conquest loads in split screen.
-- ---------------------------------------------------------------------------------------------------------------------
if (ifs_meta_opts and (not ifs_meta_opts.bfxgc)) then
ifs_meta_opts.bfxgc = 1

gBFXS_GCP2Side = nil -- player 2's side for the launch (nil: player 1's)

gBFXS_GCOrig = {
	Enter = ifs_meta_opts.Enter,
	Input_Accept = ifs_meta_opts.Input_Accept,
	Input_GeneralLeft = ifs_meta_opts.Input_GeneralLeft,
	Input_GeneralRight = ifs_meta_opts.Input_GeneralRight,
}
gBFXS_GCSetButtonPos = ifs_meta_opts_fnSetButtonPos

-- the list the screen shows
function bfxs_GCList()
	if (ScriptCB_IsSplitscreen()) then
		return ifs_meta_opts_listbox_contents_split
	end
	return ifs_meta_opts_listbox_contents
end

function bfxs_GCSelection()
	return bfxs_GCList()[ifs_meta_opts_name_listboxL_layout.SelectedIdx]
end

-- one of the screen's own functions, with the list it shows (in split screen the 2020 script read the full list's row)
function bfxs_GCWithList(fn, this)
	local keep = ifs_meta_opts_listbox_contents
	ifs_meta_opts_listbox_contents = bfxs_GCList()
	fn(this)
	ifs_meta_opts_listbox_contents = keep
end

-- player 1's side as the screen has it (its Side, else the configuration's default): 1 / 2
function bfxs_GCP1Side(this)
	local s = this.Side
	if (not s) then
		local sel = bfxs_GCSelection()
		if (sel) then
			s = sel.default
		end
	end
	if (s ~= 2) then
		s = 1
	end
	return s
end

-- a side's name: 1 = Rebels / Republic, 2 = Empire / CIS (the configuration's era)
function bfxs_GCSideName(side)
	local sel = bfxs_GCSelection()
	local key = "common.sides.all.name"
	if (sel and (sel.era == "new")) then
		key = "common.sides.rep.name"
		if (side == 2) then
			key = "common.sides.cis.name"
		end
	elseif (side == 2) then
		key = "common.sides.imp.name"
	end
	local u = ScriptCB_getlocalizestr(key, 1)
	if (u) then
		return ScriptCB_ununicode(u)
	end
	return format("side %d", side)
end

-- the launch: player 2's team and the AI from the picks
function bfxs_GCTeams(this)
	local j1 = metagame_state_local.mJoystickTeams[1]
	if ((j1 ~= 1) and (j1 ~= 2)) then
		bfxs_Log(format("Galactic Conquest LAUNCH: player 1's team is %s - the teams are left as the game set them",
			tostring(j1)))
		return
	end
	if (gBFXS_GC4) then
		bfxs_GCTeams4()
		return
	end
	local p2 = gBFXS_GCP2Side or j1
	metagame_state_local.mIsSplitscreen = 1
	if (p2 == j1) then
		metagame_state_local.mJoystickTeams[2] = j1
		metagame_state.numhumans = 1
		bfxs_Log(format("Galactic Conquest LAUNCH: CO-OP - players 1 and 2 on team %d (%s), the AI on team %d; AI control: "
			.. "team 1 %s, team 2 %s", j1, bfxs_GCSideName(j1), 3 - j1, tostring(metagame_state.team1.aicontrol),
			tostring(metagame_state.team2.aicontrol)))
		bfxt_Log(format("CONQUEST LAUNCH (2 players): CO-OP - players 1 and 2 on side %d (%s), the AI on side %d; AI control: "
			.. "team 1 %s, team 2 %s", j1, bfxs_GCSideName(j1), 3 - j1, tostring(metagame_state.team1.aicontrol),
			tostring(metagame_state.team2.aicontrol)))
	else
		metagame_state_local.mJoystickTeams[2] = 3 - j1
		metagame_state.team1.aicontrol = nil
		metagame_state.team2.aicontrol = nil
		metagame_state.numhumans = 2
		bfxs_Log(format("Galactic Conquest LAUNCH: VERSUS - player 1 on team %d (%s), player 2 on team %d (%s), no AI",
			j1, bfxs_GCSideName(j1), 3 - j1, bfxs_GCSideName(3 - j1)))
		bfxt_Log(format("CONQUEST LAUNCH (2 players): VERSUS - player 1 on side %d (%s), player 2 on side %d (%s), no AI",
			j1, bfxs_GCSideName(j1), 3 - j1, bfxs_GCSideName(3 - j1)))
	end
end

-- 21g-r3: 3 / 4 players (PICK YOUR SIDE's picks in gBFXS_GC4): a side with a player on it is human, a side without one is
-- the AI's; VERSUS when both sides have players. Players 1 / 2's battle sides go through the game's own joystick teams;
-- every player's side and each side's commander (the first who locked in on it - only he moves on the galaxy map) to
-- BF1GameExt: it keeps them across the battles (the shell starts again after each) and locks players 3 / 4's spawn
-- screens to their sides.
function bfxs_GCTeams4()
	local g = gBFXS_GC4
	local s = g.side
	local human1 = nil
	local human2 = nil
	local p
	for p = 1, g.n do
		if (s[p] == 1) then
			human1 = 1
		elseif (s[p] == 2) then
			human2 = 1
		end
	end
	metagame_state_local.mIsSplitscreen = 1
	metagame_state_local.mJoystickTeams[2] = s[2]
	if (human1) then
		metagame_state.team1.aicontrol = nil
	else
		metagame_state.team1.aicontrol = 1
	end
	if (human2) then
		metagame_state.team2.aicontrol = nil
	else
		metagame_state.team2.aicontrol = 1
	end
	local mode = "CO-OP"
	if (human1 and human2) then
		metagame_state.numhumans = 2
		mode = "VERSUS"
	else
		metagame_state.numhumans = 1
	end
	bfxs_Ask(format("gcset %d %d %d %d %d %d", s[1] or 0, s[2] or 0, s[3] or 0, s[4] or 0, g.cmd[1] or 0, g.cmd[2] or 0))
	local who = ""
	for p = 1, g.n do
		who = who .. format(" player %d %s,", p, bfxs_GCSideName(s[p]))
	end
	bfxs_Log(format("Galactic Conquest LAUNCH (%d players): %s -%s side 1 (%s) moved by %s, side 2 (%s) by %s; AI control: "
		.. "team 1 %s, team 2 %s", g.n, mode, who, bfxs_GCSideName(1),
		(g.cmd[1] and format("player %d", g.cmd[1])) or "the AI", bfxs_GCSideName(2),
		(g.cmd[2] and format("player %d", g.cmd[2])) or "the AI", tostring(metagame_state.team1.aicontrol),
		tostring(metagame_state.team2.aicontrol)))
	bfxt_Log(format("CONQUEST LAUNCH (%d players): %s -%s side 1 (%s) moved by %s, side 2 (%s) by %s (the first to lock in "
		.. "on each); players 1 / 2's joysticks on teams %s / %s", g.n, mode, who, bfxs_GCSideName(1),
		(g.cmd[1] and format("player %d", g.cmd[1])) or "the AI", bfxs_GCSideName(2),
		(g.cmd[2] and format("player %d", g.cmd[2])) or "the AI", tostring(metagame_state_local.mJoystickTeams[1]),
		tostring(metagame_state_local.mJoystickTeams[2])))
end

-- while the screen's own launch runs: its ScriptCB_SetIFScreen("ifs_meta_main") sets the teams first
function bfxs_GCSetIFScreen(name)
	if (name == "ifs_meta_main") then
		bfxs_GCTeams(ifs_meta_opts)
	end
	gBFXS_GCSetIF(name)
end

-- the 2020 script's own launch (ifs_meta_opts' LAUNCH), with player 1's side and player 2's
function bfxs_GCLaunch(p1side, p2side)
	local this = ifs_meta_opts
	this.Side = p1side
	gBFXS_GCP2Side = p2side
	this.CurButton = "launch"
	gBFXS_GCSetIF = ScriptCB_SetIFScreen
	ScriptCB_SetIFScreen = bfxs_GCSetIFScreen
	bfxs_GCWithList(gBFXS_GCOrig.Input_Accept, this)
	ScriptCB_SetIFScreen = gBFXS_GCSetIF
end

-- the side icons' places: the two-sided ones for the split list (the script placed by row number: rows 1-4 one side)
ifs_meta_opts_fnSetButtonPos = function(this)
	if (ScriptCB_IsSplitscreen()) then
		local keep = ifs_meta_opts_name_listboxL_layout.SelectedIdx
		ifs_meta_opts_name_listboxL_layout.SelectedIdx = 5
		gBFXS_GCSetButtonPos(this)
		ifs_meta_opts_name_listboxL_layout.SelectedIdx = keep
	else
		gBFXS_GCSetButtonPos(this)
	end
end

ifs_meta_opts.Enter = function(this, bFwd)
	local split = ScriptCB_IsSplitscreen()
	if (split) then
		ifs_meta_opts_name_listboxL_layout.enablecount = getn(ifs_meta_opts_listbox_contents_split)
	end
	gBFXS_GCOrig.Enter(this, bFwd)
	if (split) then
		bfxs_Log(format("Galactic Conquest maps (split screen, bFwd %s): %d conquests; LAUNCH -> PICK YOUR SIDE",
			tostring(bFwd), getn(ifs_meta_opts_listbox_contents_split)))
	end
end

ifs_meta_opts.Input_GeneralLeft = function(this)
	bfxs_GCWithList(gBFXS_GCOrig.Input_GeneralLeft, this)
end

ifs_meta_opts.Input_GeneralRight = function(this)
	bfxs_GCWithList(gBFXS_GCOrig.Input_GeneralRight, this)
end

ifs_meta_opts.Input_Accept = function(this)
	if (not ScriptCB_IsSplitscreen()) then
		gBFXS_GCOrig.Input_Accept(this)
		return
	end
	if (ScriptCB_IsLastInputFromJoystick()) then
		this.CurButton = "launch"
	end
	if (this.CurButton == "launch") then
		local sel = bfxs_GCSelection()
		if (ifs_meta_top and ifs_meta_top.bfxtop) then
			ifelm_shellscreen_fnPlaySound(this.acceptSound)
			gBFXS_GCFromMaps = 1 -- (r3) a new conquest: PICK YOUR SIDE, not the saved one
			bfxs_Log(format("Galactic Conquest (split screen): LAUNCH on \"%s\" - PICK YOUR SIDE next",
				(sel and sel.showstr) or "?"))
			ifs_movietrans_PushScreen(ifs_meta_top)
		else
			bfxs_Log("Galactic Conquest (split screen): NO PICK YOUR SIDE screen (ifs_meta_top) - launched co-op")
			bfxs_GCLaunch(bfxs_GCP1Side(this), nil)
		end
		return
	end
	bfxs_GCWithList(gBFXS_GCOrig.Input_Accept, this)
end

bfxs_Log(format("ifs_meta_opts patched (%s, %s): Galactic Conquest's split screen list, LAUNCH -> PICK YOUR SIDE",
	gBFX_Phase or "?", gBFX_Screen or "?"))
end

-- ---------------------------------------------------------------------------------------------------------------------
-- 21f: ifs_meta_top - PICK YOUR SIDE. The console's split screen layout (buttons_s1: player 1's row, buttons_s2: player
-- 2's) in a window titled PICK YOUR SIDE, BACK (B) / LAUNCH (A) at the bottom; left / right picks (ReadAllControllers:
-- the game gives pad 2 as joystick 1; BF1GameExt's pad 2 until it does). Player 1's A / LAUNCH launches, his B / BACK
-- goes back to the maps. Its 2020 Enter (the console's conquest set-up) is not used in split screen.
-- ---------------------------------------------------------------------------------------------------------------------
if (ifs_meta_top and (not ifs_meta_top.bfxtop)) then
ifs_meta_top.bfxtop = 1

-- 21j-r8: the rows of side pictures (top / bottom of each, the window's units): the console's two, or four slots with
-- 3 / 4 players (two above the side names, two below); the half widths; the player boxes (width, height, the step)
gBFXS_TopRows2 = { { -128, 2 }, { 40, 170 } }
gBFXS_TopRows4 = { { -128, -65 }, { -61, 2 }, { 40, 103 }, { 107, 170 } }
gBFXS_TopHalfW2 = 64
gBFXS_TopHalfW4 = 40
gBFXS_BoxW = 250
gBFXS_BoxH = 40
gBFXS_BoxStep = 86
gBFXS_TopLockIn = nil -- (the players lock in: 3 / 4 players, or 2 with GCCommander2P)

if (gBFX_Phase == "build") then
	-- the subtitle stays in Lua (inert: it would be dropped once made) so it can be hidden
	if (ifs_meta_top.subtitle) then
		ifs_meta_top.subtitle.inert = nil
	end
	-- the rows' soldiers under their side names (the PC spacing put them half over each other)
	local k, v
	for k, v in { side_a = -100, side_r = -100, side_i = 100, side_c = 100 } do
		if (ifs_meta_top.buttons_s1[k]) then
			ifs_meta_top.buttons_s1[k].x = v
		end
		if (ifs_meta_top.buttons_s2[k]) then
			ifs_meta_top.buttons_s2[k].x = v
		end
	end
	-- BACK (B) / LAUNCH (A), the 2020 screens' own buttons
	if (CreatePositiveButton and CreateNegativeButton) then
		ifs_meta_top.bfxback = CreateNegativeButton("common.back", "B", "_back")
		ifs_meta_top.bfxlaunch = CreatePositiveButton("ifs.onlinelobby.launch", "A", "_ok")
	end
	-- 21j-r8: the console's look - players 3 / 4's rows of side pictures (as the game's two, in containers of the same
	-- place and turn) and a box for each player at the right (the game's window with its orange title bar, as the class
	-- screen's): its title PLAYER n, then COMMANDER / PLAYER READY; inside, the player and his side
	if (gBFX_GCPickLook and NewButtonWindow) then
		local xs = { side_a = -100, side_r = -100, side_i = 100, side_c = 100 }
		local k
		if (gBFX_Players and (gBFX_Players >= 3)) then
			for k = 3, 4 do
				local c = NewIFContainer { ScreenRelativeX = 0.4, ScreenRelativeY = 0.5 }
				local nm, x
				for nm, x in xs do
					c[nm] = NewIFImage { x = x, y = 0, localpos_l = -40, localpos_r = 40, localpos_t = gBFXS_TopRows4[k][1],
						localpos_b = gBFXS_TopRows4[k][2], texture = nm, ZPos = 130 }
				end
				c.rotX = ifs_meta_top.button_rot_x
				c.rotY = ifs_meta_top.button_rot_y
				ifs_meta_top["bfxrow" .. k] = c
			end
		end
		local box = NewIFContainer { ScreenRelativeX = 0.8, ScreenRelativeY = 0.5 }
		local p
		for p = 1, 4 do
			local w = NewButtonWindow { ZPos = 200, x = 0, y = (p - 2.5) * gBFXS_BoxStep, width = gBFXS_BoxW, height = gBFXS_BoxH,
				titleText = "", font = "gamefont_small" }
			w.bfxname = NewIFText { string = "", font = "gamefont_medium", halign = "left", valign = "vcenter",
				textw = gBFXS_BoxW * 0.45, texth = gBFXS_BoxH, x = gBFXS_BoxW * -0.5 + 12, y = gBFXS_BoxH * -0.5,
				nocreatebackground = 1 }
			w.bfxside = NewIFText { string = "", font = "gamefont_medium", halign = "left", valign = "vcenter",
				textw = gBFXS_BoxW * 0.5, texth = gBFXS_BoxH, x = gBFXS_BoxW * -0.02, y = gBFXS_BoxH * -0.5,
				nocreatebackground = 1 }
			box["p" .. p] = w
		end
		ifs_meta_top.bfxbox = box
	end
	-- 21g-r3: 3 / 4 players - the roster at the right: each player, his side, READY / COMMANDER (21j-r8: GCPickLook=0)
	if (gBFX_Players and (gBFX_Players >= 3) and (not ifs_meta_top.bfxbox)) then
		local r = NewIFContainer { ScreenRelativeX = 0.8, ScreenRelativeY = 0.5 }
		r.title = NewIFText { string = "", font = "gamefont_medium", textw = 300, x = -150, y = -100, nocreatebackground = 1 }
		local p
		for p = 1, 4 do
			local y = -62 + (p - 1) * 34
			r["name" .. p] = NewIFText { string = "", font = "gamefont_medium", halign = "left", textw = 110, x = -150, y = y,
				nocreatebackground = 1 }
			r["side" .. p] = NewIFText { string = "", font = "gamefont_medium", halign = "left", textw = 120, x = -38, y = y,
				nocreatebackground = 1 }
			r["state" .. p] = NewIFText { string = "", font = "gamefont_small", halign = "left", textw = 100, x = 84, y = y + 3,
				nocreatebackground = 1 }
		end
		ifs_meta_top.bfxroster = r
	end
end

gBFXS_TopOrig = {
	Enter = ifs_meta_top.Enter,
	Update = ifs_meta_top.Update,
	Input_Accept = ifs_meta_top.Input_Accept,
	Input_Back = ifs_meta_top.Input_Back,
	Input_GeneralLeft = ifs_meta_top.Input_GeneralLeft,
	Input_GeneralRight = ifs_meta_top.Input_GeneralRight,
}
gBFXS_TopGameP2 = nil
gBFXS_TopClock = 0
gBFXS_TopPadTime = -10

function bfxs_TopSide(tag)
	if ((tag == "i") or (tag == "c")) then
		return 2
	end
	return 1
end

function bfxs_TopPicks(this)
	local p1 = bfxs_TopSide(this.CurButton_s1)
	local p2 = bfxs_TopSide(this.CurButton_s2)
	local mode = "CO-OP"
	if (p1 ~= p2) then
		mode = "VERSUS"
	end
	return format("player 1 %s, player 2 %s - %s", bfxs_GCSideName(p1), bfxs_GCSideName(p2), mode)
end

function bfxs_TopVis(this, split)
	if (this.bfxback) then
		IFObj_fnSetVis(this.bfxback, split)
	end
	if (this.bfxlaunch) then
		IFObj_fnSetVis(this.bfxlaunch, split)
	end
	if (this.bfxroster) then
		IFObj_fnSetVis(this.bfxroster, split and gBFXS_TopN and (gBFXS_TopN >= 3))
	end
	-- 21j-r8: the player boxes; players 3 / 4's rows (hidden here, shown for their players by bfxs_TopLayout)
	if (this.bfxbox) then
		IFObj_fnSetVis(this.bfxbox, split)
	end
	local k
	for k = 3, 4 do
		if (this["bfxrow" .. k] and (not split)) then
			IFObj_fnSetVis(this["bfxrow" .. k], nil)
		end
	end
end

-- 21j-r8: the era's two side pictures (side 1, side 2) - Rebels / Empire or Republic / CIS
function bfxs_TopEraNames()
	local sel = bfxs_GCSelection()
	if (sel and (sel.era == "new")) then
		return "side_r", "side_c"
	end
	return "side_a", "side_i"
end

-- 21j-r8: player p's row of side pictures (1 / 2: the game's, 3 / 4: BF1GameExt's)
function bfxs_TopRow(this, p)
	if (p == 1) then
		return this.buttons_s1
	elseif (p == 2) then
		return this.buttons_s2
	end
	return this["bfxrow" .. p]
end

-- 21j-r8: the rows' places and sizes for this many players (four slots with 3 / 4, the console's two with 2), players
-- 3 / 4's rows shown for their players, the boxes stacked round the middle - each time the screen opens
function bfxs_TopLayout(this)
	if (not this.bfxbox) then
		return
	end
	local n = gBFXS_TopN
	local four = (n >= 3)
	local hw = gBFXS_TopHalfW2
	if (four) then
		hw = gBFXS_TopHalfW4
	end
	local n1, n2 = bfxs_TopEraNames()
	local k
	for k = 1, 4 do
		local row = bfxs_TopRow(this, k)
		local r = gBFXS_TopRows2[k]
		if (four) then
			r = gBFXS_TopRows4[k]
		end
		if (row) then
			local nm
			for nm, _ in { side_a = 1, side_i = 1, side_r = 1, side_c = 1 } do
				if (row[nm] and r) then
					IFImage_fnSetTexturePos(row[nm], -hw, r[1], hw, r[2])
				end
			end
			if (k >= 3) then
				local on = (k <= n)
				IFObj_fnSetVis(row, on)
				for nm, _ in { side_a = 1, side_i = 1, side_r = 1, side_c = 1 } do
					if (row[nm]) then
						IFObj_fnSetVis(row[nm], on and ((nm == n1) or (nm == n2)))
					end
				end
			end
		end
	end
	local p
	for p = 1, 4 do
		local w = this.bfxbox["p" .. p]
		IFObj_fnSetVis(w, p <= n)
		IFObj_fnSetPos(w, 0, (p - (n + 1) * 0.5) * gBFXS_BoxStep)
	end
	local sw, sh = ScriptCB_GetSafeScreenInfo()
	bfxs_Log(format("PICK YOUR SIDE (21j-r8 look): %d players - %s rows of the side pictures (%s: %d wide, rows %s), %d player "
		.. "boxes (%d x %d, %d apart, at 0.80 of the screen across); lock in: %s; the safe screen %s x %s", n,
		(four and "four") or "two", n1 .. " / " .. n2, hw * 2, (four and "-128..-65 -61..2 40..103 107..170") or "-128..2 40..170",
		n, gBFXS_BoxW, gBFXS_BoxH, gBFXS_BoxStep, (gBFXS_TopLockIn and "yes (A locks in, B unlocks; the first on a side commands "
		.. "it)") or "no (2 players as 21f)", tostring(sw), tostring(sh)))
end

ifs_meta_top.Enter = function(this, bFwd)
	if (not ScriptCB_IsSplitscreen()) then
		gBFXS_TopOrig.Enter(this, bFwd)
		bfxs_TopVis(this, nil)
		return
	end
	local fromMaps = gBFXS_GCFromMaps
	gBFXS_GCFromMaps = nil
	-- r3: a conquest in progress (after its battle the game goes ifs_sp -> ifs_meta_new_load -> here): the game's own
	-- Enter loads it and opens the galaxy, which asks whether to save
	if (bFwd and (not fromMaps) and ScriptCB_IsMetagameStateSaved()) then
		gBFXS_TopResumed = 1
		bfxs_TopVis(this, nil)
		bfxs_Log("PICK YOUR SIDE: a conquest is in progress (saved state) - the game's own: it is loaded, the galaxy next")
		gBFXS_TopOrig.Enter(this, bFwd)
		return
	end
	-- back here from that galaxy: on to the menu before (no new side pick for a conquest in progress)
	if ((not bFwd) and gBFXS_TopResumed) then
		gBFXS_TopResumed = nil
		bfxs_TopVis(this, nil)
		bfxs_Log("PICK YOUR SIDE: back from the conquest in progress - on back to the menu before it")
		ScriptCB_PopScreen()
		return
	end
	gBFXS_TopResumed = nil
	gIFShellScreenTemplate_fnEnter(this, bFwd)
	local sel = bfxs_GCSelection()
	local eraNew = nil
	if (sel and (sel.era == "new")) then
		eraNew = 1
	end
	-- the 1-player buttons off; each player's row for the era (the console's split screen set-up)
	local k, v
	for k, v in { side_a = 1, side_i = 1, side_c = 1, side_r = 1, a = 1, i = 1, c = 1, r = 1 } do
		if (this.buttons[k]) then
			IFObj_fnSetVis(this.buttons[k], nil)
		end
	end
	IFObj_fnSetVis(this.buttons_s1.side_a, not eraNew)
	IFObj_fnSetVis(this.buttons_s1.side_i, not eraNew)
	IFObj_fnSetVis(this.buttons_s1.side_c, eraNew)
	IFObj_fnSetVis(this.buttons_s1.side_r, eraNew)
	IFObj_fnSetVis(this.buttons_s1.a, not eraNew)
	IFObj_fnSetVis(this.buttons_s1.i, not eraNew)
	IFObj_fnSetVis(this.buttons_s1.c, eraNew)
	IFObj_fnSetVis(this.buttons_s1.r, eraNew)
	IFObj_fnSetVis(this.buttons_s2.side_a, not eraNew)
	IFObj_fnSetVis(this.buttons_s2.side_i, not eraNew)
	IFObj_fnSetVis(this.buttons_s2.side_c, eraNew)
	IFObj_fnSetVis(this.buttons_s2.side_r, eraNew)
	IFObj_fnSetVis(this.buttonWindow, 1)
	if (this.buttonWindow.titleBarElement) then
		IFText_fnSetUString(this.buttonWindow.titleBarElement, ScriptCB_tounicode("Pick your side"))
	end
	if (this.subtitle) then
		IFObj_fnSetVis(this.subtitle, nil)
	end
	bfxs_TopVis(this, 1)
	-- player 1 starts on the side the maps screen had, player 2 with him (co-op)
	local t1 = "a"
	local t2 = "i"
	if (eraNew) then
		t1 = "r"
		t2 = "c"
	end
	if (bFwd or (not this.CurButton_s1)) then
		if (bfxs_GCP1Side(ifs_meta_opts) == 2) then
			this.CurButton_s1 = t2
		else
			this.CurButton_s1 = t1
		end
		this.CurButton_s2 = this.CurButton_s1
	end
	this.CurButton = this.CurButton_s1
	ifs_meta_top_fnHilightSide(this)
	ScriptCB_ReadAllControllers(1)
	gBFXS_TopGameP2 = nil
	bfxs_Ask("pad2reset")
	bfxs_Top4Begin(this, bFwd) -- 21g-r3
	bfxs_TopVis(this, 1)
	if (gBFXS_TopLockIn) then
		bfxs_Log(format("PICK YOUR SIDE (bFwd %s, %d players): %s", tostring(bFwd), gBFXS_TopN, bfxs_Top4Picks(this)))
		return
	end
	bfxs_Log(format("PICK YOUR SIDE (bFwd %s): %s", tostring(bFwd), bfxs_TopPicks(this)))
end

-- ---- 21g-r3: 3 / 4 players --------------------------------------------------------------------------------------------
-- Each player picks a side (left / right; players 1 / 2 on their rows of the console screen, players 3 / 4 in the
-- roster) and locks in with A (B: unlocks). The first player who locked in on a side commands it: only he moves on the
-- galaxy map for it (and picks its bonuses); the others on it fight in its battles. LAUNCH (player 1's A once he is
-- locked in) when everyone is locked in. Pads 3 / 4 are BF1GameExt's ("BFXQ: pad3" / "pad4").
gBFXS_TopN = 2
gBFXS_TopSides = {}
gBFXS_TopLock = {}
gBFXS_TopSeq = 0

function bfxs_Top4Begin(this, bFwd)
	gBFXS_TopN = 2
	if (this.bfxroster or this.bfxbox) then
		local n = bfxs_Ask("playersnow")
		if ((n >= 3) and (this.bfxroster or this.bfxrow3)) then
			gBFXS_TopN = n
		end
	end
	-- 21j-r8: 3 / 4 players lock in; 2 players too with GCCommander2P (and the boxes)
	gBFXS_TopLockIn = (gBFXS_TopN >= 3) or (gBFX_GCCmd2 and this.bfxbox and 1)
	bfxs_TopLayout(this)
	if (not gBFXS_TopLockIn) then
		bfxs_Top4Show(this)
		return
	end
	if (bFwd or (not gBFXS_TopSides[3])) then
		local s1 = bfxs_TopSide(this.CurButton_s1)
		gBFXS_TopSides = { s1, s1, s1, s1 } -- (players 1 / 2's own come from their rows)
		gBFXS_TopLock = {}
		gBFXS_TopSeq = 0
	end
	bfxs_Ask("pad3reset")
	bfxs_Ask("pad4reset")
	bfxs_Top4Show(this)
end

-- player p's side (1 / 2)
function bfxs_Top4Side(this, p)
	if (p == 1) then
		return bfxs_TopSide(this.CurButton_s1)
	elseif (p == 2) then
		return bfxs_TopSide(this.CurButton_s2)
	end
	return gBFXS_TopSides[p] or 1
end

-- the commander of a side: the player who locked in on it first (nil: nobody locked in on it)
function bfxs_Top4Cmd(this, side)
	local best = nil
	local bestSeq = nil
	local p
	for p = 1, gBFXS_TopN do
		local seq = gBFXS_TopLock[p]
		if (seq and (bfxs_Top4Side(this, p) == side) and ((not bestSeq) or (seq < bestSeq))) then
			best = p
			bestSeq = seq
		end
	end
	return best
end

function bfxs_Top4Picks(this)
	local s = ""
	local human1 = nil
	local human2 = nil
	local c1 = bfxs_Top4Cmd(this, 1)
	local c2 = bfxs_Top4Cmd(this, 2)
	local p
	for p = 1, gBFXS_TopN do
		local side = bfxs_Top4Side(this, p)
		if (side == 1) then
			human1 = 1
		else
			human2 = 1
		end
		local st = "picking"
		if ((p == c1) or (p == c2)) then
			st = "COMMANDER"
		elseif (gBFXS_TopLock[p]) then
			st = "READY"
		end
		s = s .. format("player %d %s (%s), ", p, bfxs_GCSideName(side), st)
	end
	if (human1 and human2) then
		return s .. "VERSUS"
	end
	return s .. "CO-OP"
end

-- 21j-r8: a side's colour (the bar's tint and its name's): 1 = Rebels / Republic green, 2 = Empire / CIS red
gBFXS_SideBar = { { 40, 255, 40 }, { 255, 40, 40 } }
gBFXS_SideText = { { 110, 255, 110 }, { 255, 95, 80 } }

-- 21j-r8: the console's look - every player's row (his side lit, the other darker once he has locked in) and his box (its
-- title PLAYER n while he picks, then COMMANDER / PLAYER READY in his side's colour; inside, the player and his side)
function bfxs_TopShowLook(this)
	local box = this.bfxbox
	local n1, n2 = bfxs_TopEraNames()
	local c1 = nil
	local c2 = nil
	if (gBFXS_TopLockIn) then
		c1 = bfxs_Top4Cmd(this, 1)
		c2 = bfxs_Top4Cmd(this, 2)
	end
	local p
	for p = 1, gBFXS_TopN do
		local side = bfxs_Top4Side(this, p)
		local locked = gBFXS_TopLockIn and gBFXS_TopLock[p]
		local row = bfxs_TopRow(this, p)
		if (row and row[n1] and row[n2]) then
			local dim = 0.5
			if (locked) then
				dim = 0.15
			end
			if (side == 1) then
				IFObj_fnSetAlpha(row[n1], 1.0)
				IFObj_fnSetAlpha(row[n2], dim)
			else
				IFObj_fnSetAlpha(row[n1], dim)
				IFObj_fnSetAlpha(row[n2], 1.0)
			end
		end
		local w = box["p" .. p]
		if (w) then
			local title = format("PLAYER %d", p)
			local sname = bfxs_GCSideName(side)
			local bar = { 255, 255, 255 }
			local txt = { 255, 255, 255 }
			if (locked) then
				if ((p == c1) or (p == c2)) then
					title = "COMMANDER"
				else
					title = "PLAYER READY"
				end
				bar = gBFXS_SideBar[side] or bar
				txt = gBFXS_SideText[side] or txt
			else
				sname = "< " .. sname .. " >"
			end
			if (w.titleBarElement) then
				IFText_fnSetUString(w.titleBarElement, ScriptCB_tounicode(title))
				IFObj_fnSetColor(w.titleBarElement, bar[1], bar[2], bar[3])
			end
			IFText_fnSetUString(w.bfxname, ScriptCB_tounicode(format("PLAYER %d", p)))
			IFText_fnSetUString(w.bfxside, ScriptCB_tounicode(sname))
			IFObj_fnSetColor(w.bfxside, txt[1], txt[2], txt[3])
		end
	end
end

function bfxs_Top4Show(this)
	if (this.bfxbox) then
		bfxs_TopShowLook(this) -- 21j-r8
		return
	end
	local r = this.bfxroster
	if (not r) then
		return
	end
	IFText_fnSetUString(r.title, ScriptCB_tounicode("PLAYERS"))
	local c1 = bfxs_Top4Cmd(this, 1)
	local c2 = bfxs_Top4Cmd(this, 2)
	local p
	for p = 1, 4 do
		local on = (p <= gBFXS_TopN)
		IFObj_fnSetVis(r["name" .. p], on)
		IFObj_fnSetVis(r["side" .. p], on)
		IFObj_fnSetVis(r["state" .. p], on)
		if (on) then
			local side = bfxs_Top4Side(this, p)
			local sname = bfxs_GCSideName(side)
			if (not gBFXS_TopLock[p]) then
				sname = "< " .. sname .. " >"
			end
			local st = ""
			if ((p == c1) or (p == c2)) then
				st = "COMMANDER"
			elseif (gBFXS_TopLock[p]) then
				st = "READY"
			end
			IFText_fnSetUString(r["name" .. p], ScriptCB_tounicode(format("PLAYER %d", p)))
			IFText_fnSetUString(r["side" .. p], ScriptCB_tounicode(sname))
			IFText_fnSetUString(r["state" .. p], ScriptCB_tounicode(st))
		end
	end
end

-- one input for player p (1 - 4)
function bfxs_Top4Do(this, name, p)
	if (p > gBFXS_TopN) then
		return
	end
	local locked = gBFXS_TopLock[p]
	if ((name == "Input_GeneralLeft") or (name == "Input_GeneralRight")) then
		if (locked) then
			return -- (B unlocks first)
		end
		if (p <= 2) then
			gBFXS_TopOrig.Input_GeneralLeft(this, p - 1) -- the console's own toggle of his row
		else
			gBFXS_TopSides[p] = 3 - (gBFXS_TopSides[p] or 1)
			ifelm_shellscreen_fnPlaySound("shell_select_change")
		end
		bfxs_Top4Show(this)
		bfxs_Log(format("PICK YOUR SIDE: player %d changed side - %s", p, bfxs_Top4Picks(this)))
	elseif (name == "Input_Accept") then
		if ((p == 1) and (this.CurButton == "_back")) then
			bfxs_TopBack(this)
			return
		end
		if (not locked) then
			gBFXS_TopSeq = gBFXS_TopSeq + 1
			gBFXS_TopLock[p] = gBFXS_TopSeq
			ifelm_shellscreen_fnPlaySound("shell_menu_accept")
			bfxs_Top4Show(this)
			bfxs_Log(format("PICK YOUR SIDE: player %d locked in - %s", p, bfxs_Top4Picks(this)))
		elseif (p == 1) then
			bfxs_Top4Launch(this)
		end
	elseif (name == "Input_Back") then
		if (locked) then
			gBFXS_TopLock[p] = nil
			ifelm_shellscreen_fnPlaySound("shell_menu_exit")
			bfxs_Top4Show(this)
			bfxs_Log(format("PICK YOUR SIDE: player %d unlocked - %s", p, bfxs_Top4Picks(this)))
		elseif (p == 1) then
			bfxs_TopBack(this)
		end
	end
end

-- LAUNCH: everyone locked in - the game's own launch with players 1 / 2's sides, the rest set by bfxs_GCTeams4
function bfxs_Top4Launch(this)
	local p
	for p = 1, gBFXS_TopN do
		if (not gBFXS_TopLock[p]) then
			ifelm_shellscreen_fnPlaySound("shell_menu_error")
			bfxs_Log(format("PICK YOUR SIDE: LAUNCH - player %d has not locked in yet", p))
			return
		end
	end
	local g = { n = gBFXS_TopN, side = {}, cmd = {} }
	for p = 1, gBFXS_TopN do
		g.side[p] = bfxs_Top4Side(this, p)
	end
	g.cmd[1] = bfxs_Top4Cmd(this, 1)
	g.cmd[2] = bfxs_Top4Cmd(this, 2)
	ifelm_shellscreen_fnPlaySound("shell_menu_accept")
	bfxs_Log("PICK YOUR SIDE: LAUNCH - " .. bfxs_Top4Picks(this))
	gBFXS_GC4 = g
	bfxs_GCLaunch(g.side[1], g.side[2])
	gBFXS_GC4 = nil
end

-- one input for one player (0 / 1)
function bfxs_TopDo(this, name, joy)
	if (gBFXS_TopLockIn) then
		bfxs_Top4Do(this, name, joy + 1) -- 21g-r3; 21j-r8: 2 players too (GCCommander2P)
		return
	end
	if ((name == "Input_GeneralLeft") or (name == "Input_GeneralRight")) then
		-- two sides: left and right both change it (the console's own toggle)
		gBFXS_TopOrig.Input_GeneralLeft(this, joy)
		bfxs_Top4Show(this) -- 21j-r8: the boxes
		bfxs_Log(format("PICK YOUR SIDE: player %d changed side - %s", joy + 1, bfxs_TopPicks(this)))
	elseif (name == "Input_Accept") then
		if (joy > 0.5) then
			return -- player 1 launches
		end
		if (this.CurButton == "_back") then
			bfxs_TopBack(this)
		elseif (ScriptCB_IsLastInputFromJoystick() or (this.CurButton == "_ok")) then
			ifelm_shellscreen_fnPlaySound("shell_menu_accept")
			local p1 = bfxs_TopSide(this.CurButton_s1)
			local p2 = bfxs_TopSide(this.CurButton_s2)
			bfxs_Log(format("PICK YOUR SIDE: LAUNCH - %s", bfxs_TopPicks(this)))
			bfxs_GCLaunch(p1, p2)
		end
	elseif (name == "Input_Back") then
		if (joy < 0.5) then
			bfxs_TopBack(this)
		end
	end
end

function bfxs_TopBack(this)
	ifelm_shellscreen_fnPlaySound("shell_menu_exit")
	bfxs_Log("PICK YOUR SIDE: BACK - the maps")
	ScriptCB_PopScreen()
end

-- the game's input: player 1 = its own devices (joystick 0 / none), player 2 = joystick 1
function bfxs_TopInput(this, name, Joystick)
	if (not ScriptCB_IsSplitscreen()) then
		if (gBFXS_TopOrig[name]) then
			gBFXS_TopOrig[name](this, Joystick)
		end
		return
	end
	local joy = 0
	if ((gBFXS_TopN >= 3) and (type(Joystick) == "number") and (Joystick > 1.5)) then
		return -- 21g-r3: (pads 3 / 4 come from BF1GameExt, not from the game)
	end
	if ((type(Joystick) == "number") and (Joystick > 0.5)) then
		joy = 1
		if (not gBFXS_TopGameP2) then
			gBFXS_TopGameP2 = 1
			bfxs_Log(format("PICK YOUR SIDE: the game itself gives player 2's pad (%s, joystick %s) - BF1GameExt's pad 2 "
				.. "is not read here from now on", name, tostring(Joystick)))
			if (gBFXS_TopClock - gBFXS_TopPadTime < 0.5) then
				bfxs_Log("  (BF1GameExt's pad 2 gave a press just before - not done twice)")
				return
			end
		end
	end
	bfxs_TopDo(this, name, joy)
end

ifs_meta_top.Input_GeneralLeft = function(this, Joystick)
	bfxs_TopInput(this, "Input_GeneralLeft", Joystick)
end
ifs_meta_top.Input_GeneralRight = function(this, Joystick)
	bfxs_TopInput(this, "Input_GeneralRight", Joystick)
end
ifs_meta_top.Input_Accept = function(this, Joystick)
	bfxs_TopInput(this, "Input_Accept", Joystick)
end
ifs_meta_top.Input_Back = function(this, Joystick)
	bfxs_TopInput(this, "Input_Back", Joystick)
end

ifs_meta_top.Update = function(this, fDt)
	if (gBFXS_TopOrig.Update) then
		gBFXS_TopOrig.Update(this, fDt)
	end
	if (not ScriptCB_IsSplitscreen()) then
		return
	end
	gBFXS_TopClock = gBFXS_TopClock + fDt
	-- 21g-r3: pads 3 / 4 with 3 / 4 players: left / right = his side, A = lock in, B = unlock
	if (gBFXS_TopN >= 3) then
		local p
		for p = 3, gBFXS_TopN do
			local m = 0
			local ev = bfxs_Ask(format("pad%d", p))
			while ((ev > 0) and (m < 8)) do
				if ((ev == 6) or (ev == 7)) then
					bfxs_Top4Do(this, "Input_GeneralLeft", p)
				elseif (ev == 1) then
					bfxs_Top4Do(this, "Input_Accept", p)
				elseif (ev == 2) then
					bfxs_Top4Do(this, "Input_Back", p)
				end
				m = m + 1
				ev = bfxs_Ask(format("pad%d", p))
			end
		end
	end
	if (gBFXS_TopGameP2) then
		return
	end
	-- pad 2 (BF1GameExt's) until the game gives it: left / right = his side (21g-r3: with 3 / 4 players A / B too; 21j-r8:
	-- with 2 players locking in too)
	local n = 0
	local e = bfxs_Ask("pad2")
	while ((e > 0) and (n < 8)) do
		if ((e == 6) or (e == 7)) then
			gBFXS_TopPadTime = gBFXS_TopClock
			bfxs_TopDo(this, "Input_GeneralLeft", 1)
		elseif (gBFXS_TopLockIn and ((e == 1) or (e == 2))) then
			gBFXS_TopPadTime = gBFXS_TopClock
			if (e == 1) then
				bfxs_TopDo(this, "Input_Accept", 1)
			else
				bfxs_TopDo(this, "Input_Back", 1)
			end
		end
		n = n + 1
		e = bfxs_Ask("pad2")
	end
end

bfxs_Log(format("ifs_meta_top patched (%s, %s): PICK YOUR SIDE for Galactic Conquest in split screen", gBFX_Phase or "?",
	gBFX_Screen or "?"))
end

-- ---------------------------------------------------------------------------------------------------------------------
-- 21g-r3: the galaxy map (ifs_meta_main) with 3 / 4 players: each side is moved only by its commander (the player who
-- locked in on it first on PICK YOUR SIDE). The game reads a side from the joystick a press came on (joystick 1 = player
-- 1's team, joystick 2 = player 2's: metagame_state_local.mJoystickTeams); a commander's press is handed to it as the
-- joystick of his side - when neither player 1 nor player 2 is on his side, joystick 2 is given his side for that one
-- call. Presses of players who do not command are dropped; players 3 / 4's come from BF1GameExt's pads 3 / 4. The pause
-- (B / BACK) and the pause menu stay the game's own (players 1 / 2). The sides and commanders come from BF1GameExt
-- ("gcget": it keeps them across the battles), checked against this conquest's players 1 / 2. With 2 players: the
-- game's own.
-- 21g-r4: (1) THE AI'S TURN IN CO-OP. With players 1 and 2 on one side the game's AI moves the other side on the third
-- joystick (metagame_ai.lua: "AI will take over the third joyStick input"), and its team (mJoystickTeams[3]) was never
-- set: the game's script stopped at every AI move (format "team%d" of nil), so the AI side never picked its planet or
-- its bonus and the conquest stood still - 2-player co-op as well as 3 / 4 players on one side. The third joystick is
-- put on the AI's team when the galaxy opens and in each frame (bfxs_GCAIJoy). (2) THE TURN SCANNER ([Splitscreen]
-- GCTurnScan=1, log only, [gcturn] lines, 2 / 3 / 4 players): each turn (the page, the side choosing, who moves it - the
-- player and his pad, or the AI and its joystick - and the raw state), each choice (planet, bonuses), the battle and its
-- result (the game's rule: the attacker attacks again after a win, the turn passes only when he loses), every press
-- (whose, from which pad, the joystick it went in on, what the game's own checks say, what changed), the game's script
-- errors on this screen (caught to be logged: its Update and the pads' presses), and a turn that stands still 30 s.
-- ---------------------------------------------------------------------------------------------------------------------
if (ifs_meta_main and ifs_meta_main.Input_Accept and (not ifs_meta_main.bfxmain)) then
ifs_meta_main.bfxmain = 1

gBFXS_MainOrig = {
	Enter = ifs_meta_main.Enter,
	Update = ifs_meta_main.Update,
}
gBFXS_MainCfg = nil
gBFXS_MainBlocked = nil
gBFXS_MainShort = { Input_GeneralLeft = "LEFT", Input_GeneralRight = "RIGHT", Input_GeneralUp = "UP",
	Input_GeneralDown = "DOWN", Input_Accept = "A", Input_Misc = "Y" }

-- this conquest's sides and commanders, or nil (the game's own: players 1 / 2 each move the side of their joystick).
-- BF1GameExt's picks are used when they are this conquest's: players 1 / 2 on its joystick teams, and every human side
-- (no aicontrol) commanded by a player of this match who is on it. Otherwise players 1 / 2 move the sides of their
-- joysticks, and a human side neither has (a conquest made with 3 / 4 players, resumed with fewer, or other picks) is
-- moved by player 1 too - never a human side nobody can move.
function bfxs_MainCfg()
	if (not ScriptCB_IsSplitscreen()) then
		return nil
	end
	local n = bfxs_Ask("playersnow")
	local jt = metagame_state_local.mJoystickTeams
	local human = { (not metagame_state.team1.aicontrol), (not metagame_state.team2.aicontrol) }
	local side = {}
	local cmd = { 0, 0 }
	local i
	local ok = (n >= 3) or ((n == 2) and gBFX_GCCmd2) -- 21j-r8: 2 players' commanders too (GCCommander2P)
	if (ok) then
		for i = 1, 4 do
			side[i] = bfxs_Ask(format("gcget %d", i))
		end
		cmd = { bfxs_Ask("gcget 5"), bfxs_Ask("gcget 6") }
		ok = (side[1] == jt[1]) and (side[2] == jt[2])
		for i = 1, 2 do
			if (ok and human[i] and ((cmd[i] < 1) or (cmd[i] > n) or (side[cmd[i]] ~= i))) then
				ok = nil
			end
		end
		if (not ok) then
			bfxs_Log(format("galaxy (%d players): BF1GameExt's picks (players 1 / 2 on %d / %d, commanders %d / %d) are not this "
				.. "conquest's (players 1 / 2 on %s / %s, human sides: 1 %s, 2 %s)", n, side[1], side[2], cmd[1], cmd[2],
				tostring(jt[1]), tostring(jt[2]), tostring(human[1]), tostring(human[2])))
			bfxt_Log(format("the picks BF1GameExt kept (players 1 / 2 on %d / %d, commanders %d / %d) are not this conquest's "
				.. "(players 1 / 2 on %s / %s) - players 1 / 2 move their joysticks' sides", side[1], side[2], cmd[1], cmd[2],
				tostring(jt[1]), tostring(jt[2])))
			bfxs_Ask("gcclear")
		end
	end
	if (ok) then
		return { n = n, side = side, cmd = cmd }
	end
	-- the game's own joysticks; a human side without one goes to player 1
	side = { jt[1], jt[2], 0, 0 }
	cmd = { 0, 0 }
	local orphan = nil
	for i = 1, 2 do
		if (human[i]) then
			if (jt[1] == i) then
				cmd[i] = 1
			elseif (jt[2] == i) then
				cmd[i] = 2
			else
				cmd[i] = 1
				orphan = i
			end
		end
	end
	if (not orphan) then
		return nil
	end
	bfxs_Log(format("galaxy: side %d is human but neither player 1 nor player 2 is on it - player 1 moves it too", orphan))
	bfxt_Log(format("side %d is a player's but neither player 1 nor player 2 is on it - player 1 moves it too", orphan))
	if (n < 2) then
		n = 2
	end
	return { n = n, side = side, cmd = cmd }
end

-- the side player p moves now (nil: none): the one he commands; when he commands both (a side without its own player),
-- the one choosing on this page (pages 1 / 3: the attacker, the picking team; 2 / 4: the defender)
function bfxs_MainTarget(cfg, p)
	local a = (cfg.cmd[1] == p)
	local b = (cfg.cmd[2] == p)
	if (a and b) then
		local pick = metagame_state.pickteam or 1
		if ((gMetagameCurrentPage == 2) or (gMetagameCurrentPage == 4)) then
			return 3 - pick
		end
		return pick
	elseif (a) then
		return 1
	elseif (b) then
		return 2
	end
	return nil
end

-- 21g-r4: co-op - the game's AI moves its side on the third joystick: that joystick on the AI's team
function bfxs_GCAIJoy(why)
	local jt = metagame_state_local.mJoystickTeams
	local j1 = jt[1]
	if (((j1 == 1) or (j1 == 2)) and (jt[2] == j1)) then
		local ai = 3 - j1
		local t = metagame_state[format("team%d", ai)]
		if (t and t.aicontrol and (jt[3] ~= ai)) then
			local was = jt[3]
			jt[3] = ai
			bfxs_Log(format("galaxy: co-op - the AI's joystick (the game's third) put on team %d (it was %s) - %s", ai,
				tostring(was), why))
			bfxt_Log(format("FIX: co-op - the AI moves %s on the game's third joystick, whose team was %s: set to %d (%s)",
				bfxt_Side(ai), tostring(was), ai, why))
		end
	end
end

-- ---- 21g-r4: the turn scanner ----
gBFXT = { turn = 0, sig = nil, since = 0, still = 30, lines = 0, more = 0, taken = 0, ignored = 0, dropped = 0,
	errn = 0, presses = 0, errs = {}, choice = nil, paused = nil, blocked = {} }
gBFXT_Err = nil

function bfxt_OnErr(m)
	gBFXT_Err = m
end

-- a side's name, from the conquest's own teams
function bfxt_Side(s)
	if ((s ~= 1) and (s ~= 2)) then
		return format("side %s", tostring(s))
	end
	local t = metagame_state[format("team%d", s)]
	if (t and t.teamname) then
		local u = ScriptCB_getlocalizestr(t.teamname, 1)
		if (u) then
			return format("side %d (%s)", s, ScriptCB_ununicode(u))
		end
	end
	return format("side %d", s)
end

-- player p's pad as the galaxy gets it
function bfxt_Pad(p)
	if (p == 1) then
		return "the game's joystick 0 / keyboard / mouse"
	elseif (p == 2) then
		return "the game's joystick 1"
	end
	return format("BF1GameExt's pad %d, XInput %d", p, bfxs_Ask(format("pad%dindex", p)))
end

-- who moves side s now: a player (his pad), the AI (its joystick), or nobody
function bfxt_Mover(s)
	if ((s ~= 1) and (s ~= 2)) then
		return "nobody (no side)"
	end
	local jt = metagame_state_local.mJoystickTeams
	local t = metagame_state[format("team%d", s)]
	if (t and t.aicontrol) then
		-- as metagame_ai_fnUpdate picks the AI's joystick
		local j = nil
		if (jt[1] == s) then
			j = 0
		end
		if (jt[2] == s) then
			j = 1
		end
		if (jt[1] == jt[2]) then
			j = 2
		end
		if (not j) then
			return "the AI - but no joystick is on its team: the game's AI does NOT move it"
		end
		if (jt[j + 1] ~= s) then
			return format("the AI on joystick %d - whose team is %s, not %d: the game's script STOPS at each AI move", j,
				tostring(jt[j + 1]), s)
		end
		return format("the AI (the game's joystick %d)", j)
	end
	local cfg = gBFXS_MainCfg
	if (cfg) then
		local c = cfg.cmd[s] or 0
		if (c < 1) then
			return "NOBODY (no commander)"
		end
		return format("player %d (%s), its commander", c, bfxt_Pad(c))
	end
	local who = nil
	if (jt[1] == s) then
		who = "player 1 (" .. bfxt_Pad(1) .. ")"
	end
	if (jt[2] == s) then
		if (who) then
			who = who .. " and player 2 (" .. bfxt_Pad(2) .. ")"
		else
			who = "player 2 (" .. bfxt_Pad(2) .. ")"
		end
	end
	return who or "NOBODY (no joystick on its team and not the AI's)"
end

-- the turn now: the page, the side choosing, what it chooses, who moves it
function bfxt_Turn()
	local page = gMetagameCurrentPage
	local pick = metagame_state.pickteam
	if ((pick ~= 1) and (pick ~= 2)) then
		return format("page %s - no side picks (pickteam %s)", tostring(page), tostring(pick))
	end
	local s = pick
	local what = format("(page %s?)", tostring(page))
	if (page == 1) then
		if (metagame_state.applyingbonus) then
			what = "picks the planet for its secret base bonus"
		else
			what = "picks the planet to attack"
		end
	elseif (page == 2) then
		what = "attacks - the attack is shown (nobody chooses)"
	elseif (page == 3) then
		what = "picks the attacker's bonus"
	elseif (page == 4) then
		s = 3 - pick
		what = "picks the defender's bonus"
	end
	return format("page %s: %s %s - moved by %s", tostring(page), bfxt_Side(s), what, bfxt_Mover(s))
end

-- the raw state behind it
function bfxt_State()
	local jt = metagame_state_local.mJoystickTeams
	return format("pickteam %s, page %s, applying bonus %s; joysticks' teams %s / %s / %s; AI control: team 1 %s, "
		.. "team 2 %s; numhumans %s; split %s", tostring(metagame_state.pickteam), tostring(gMetagameCurrentPage),
		tostring(metagame_state.applyingbonus), tostring(jt[1]), tostring(jt[2]), tostring(jt[3]),
		tostring(metagame_state.team1.aicontrol), tostring(metagame_state.team2.aicontrol),
		tostring(metagame_state.numhumans), tostring(metagame_state_local.mIsSplitscreen))
end

function bfxt_TurnSig()
	local jt = metagame_state_local.mJoystickTeams
	return format("%s|%s|%s|%s|%s|%s|%s|%s", tostring(gMetagameCurrentPage), tostring(metagame_state.pickteam),
		tostring(metagame_state.applyingbonus), tostring(jt[1]), tostring(jt[2]), tostring(jt[3]),
		tostring(metagame_state.team1.aicontrol), tostring(metagame_state.team2.aicontrol))
end

-- the choices made so far on this turn (for "choice:" lines)
gBFXT_ChoiceNames = { "ATTACK planet chosen", "attack planet (the game's)", "BASE BONUS planet chosen",
	"base bonus planet (the game's)", "side 1's BONUS chosen", "side 2's BONUS chosen", "side 1's bonus (the game's)",
	"side 2's bonus (the game's)", "the battle's map" }
function bfxt_Choices()
	local v = metagame_vote_state or {}
	local h = metagame_host_state or {}
	return { v.gAttackPlanetVote, h.gAttackPlanet, v.gBonusPlanetVote, h.gBonusPlanet, v.gBonus1Vote, v.gBonus2Vote,
		h.gBonus1, h.gBonus2, metagame_state.NextMapName }
end

-- what a press can change
gBFXT_SnapNames = { "planet picked", "base bonus planet picked", "side 1's bonus picked", "side 2's bonus picked",
	"ATTACK chosen", "BASE BONUS chosen", "side 1's BONUS chosen", "side 2's BONUS chosen", "page", "paused" }
function bfxt_Snap()
	local v = metagame_vote_state or {}
	return { gAttackPlanetSelect, gBonusPlanetSelect, gTopBonusIdx, gBotBonusIdx, v.gAttackPlanetVote,
		v.gBonusPlanetVote, v.gBonus1Vote, v.gBonus2Vote, gMetagameCurrentPage, ifs_meta_main.paused }
end

function bfxt_Changes(a, b, names, n)
	local s = nil
	local i
	for i = 1, n do
		if (a[i] ~= b[i]) then
			local one = format("%s %s -> %s", names[i], tostring(a[i]), tostring(b[i]))
			if (s) then
				s = s .. ", " .. one
			else
				s = one
			end
		end
	end
	return s
end

-- what the game's own checks make of a press on joystick j (0-based) now
function bfxt_Verdict(name, j, bFromAI)
	if (ifs_meta_main.paused) then
		return "the pause menu's"
	end
	if (name == "Input_Misc") then
		return "nothing (Y does nothing on the galaxy map)"
	end
	if (type(j) ~= "number") then
		return format("joystick %s (the mouse / keyboard?)", tostring(j))
	end
	local jt = metagame_state_local.mJoystickTeams
	local team = jt[j + 1]
	if ((team ~= 1) and (team ~= 2)) then
		return format("joystick %d has no team (%s): the game's script STOPS here", j, tostring(team))
	end
	local t = metagame_state[format("team%d", team)]
	if (t.aicontrol ~= bFromAI) then
		if (t.aicontrol) then
			return format("IGNORED - %s is the AI's", bfxt_Side(team))
		end
		return format("IGNORED - %s is a player's, not the AI's", bfxt_Side(team))
	end
	local page = gMetagameCurrentPage
	local pick = metagame_state.pickteam
	if (page == 1) then
		if (team == pick) then
			return format("taken - %s's turn", bfxt_Side(team))
		end
		return format("IGNORED - not %s's turn (%s picks)", bfxt_Side(team), bfxt_Side(pick))
	elseif ((page == 3) or (page == 4)) then
		if (name ~= "Input_Accept") then
			return "moves the bonus pick (on the bonus pages any side's pad moves it - the game's own)"
		end
		local turn = pick
		if (page == 4) then
			turn = 3 - pick
		end
		if (team == turn) then
			return format("taken - %s's bonus", bfxt_Side(team))
		end
		return format("IGNORED - %s picks its bonus now, not %s", bfxt_Side(turn), bfxt_Side(team))
	end
	return format("IGNORED - page %s (nothing to choose)", tostring(page))
end

-- one press, logged (each turn: 30 lines, 60 with A presses, 80 with errors; the rest counted)
function bfxt_Press(name, who, how, j, team, bFromAI, verdict, before, err)
	local g = gBFXT
	g.presses = g.presses + 1
	local ch = bfxt_Changes(before, bfxt_Snap(), gBFXT_SnapNames, 10)
	if (err) then
		-- (counted by bfxt_Error)
	elseif (strfind(verdict, "IGNORED", 1, 1)) then
		g.ignored = g.ignored + 1
	else
		g.taken = g.taken + 1
	end
	if ((g.lines < 30) or ((name == "Input_Accept") and (g.lines < 60)) or (err and (g.lines < 80))) then
		g.lines = g.lines + 1
		local e = ""
		if (err) then
			e = " | THE GAME'S SCRIPT STOPPED: " .. err
		end
		bfxt_Log(format("press: %s %s (%s) -> joystick %s (team %s)%s; the game: %s; %s%s", who,
			gBFXS_MainShort[name] or name, how, tostring(j), tostring(team), (bFromAI and ", from the AI") or "", verdict,
			ch or "nothing changed", e))
	else
		g.more = g.more + 1
	end
end

-- a press that went nowhere (player p commands no side, or a joystick BF1GameExt does not use) - once a turn each
function bfxt_Dropped(key, text)
	local g = gBFXT
	g.dropped = g.dropped + 1
	if (not g.blocked[key]) then
		g.blocked[key] = 1
		bfxt_Log("dropped: " .. text)
	end
end

-- the game's script stopped (counted by message: the 1st, 10th, 100th and 1000th time logged)
function bfxt_Error(where, msg)
	local g = gBFXT
	local n = (g.errs[msg] or 0) + 1
	g.errs[msg] = n
	g.errn = g.errn + 1
	if ((n == 1) or (n == 10) or (n == 100) or (n == 1000)) then
		bfxt_Log(format("ERROR - the game's galaxy script stopped (%s, %d time(s) so far): %s | %s", where, n, msg,
			bfxt_Turn()))
	end
end

-- once a frame: a new turn, a choice made, the pause, a turn standing still
function bfxt_Frame(this, fDt)
	local g = gBFXT
	local sig = bfxt_TurnSig()
	if (sig ~= g.sig) then
		if (g.sig and (g.more > 0)) then
			bfxt_Log(format("(turn %d: %d more press(es) not logged)", g.turn, g.more))
		end
		g.sig = sig
		g.turn = g.turn + 1
		g.since = 0
		g.still = 30
		g.lines = 0
		g.more = 0
		g.taken = 0
		g.ignored = 0
		g.dropped = 0
		g.presses = 0
		g.blocked = {}
		bfxt_Log(format("TURN %d - %s | %s", g.turn, bfxt_Turn(), bfxt_State()))
	else
		g.since = g.since + (fDt or 0)
		if (g.since >= g.still) then
			bfxt_Log(format("turn %d still on after %d s - %s | presses on it: %d (taken %d, ignored by the game %d, dropped %d), "
				.. "script errors so far %d | %s", g.turn, g.still, bfxt_Turn(), g.presses, g.taken, g.ignored, g.dropped,
				g.errn, bfxt_State()))
			g.still = g.still * 2
		end
	end
	local c = bfxt_Choices()
	if (g.choice) then
		local ch = bfxt_Changes(g.choice, c, gBFXT_ChoiceNames, 9)
		if (ch) then
			bfxt_Log("choice: " .. ch)
		end
	end
	g.choice = c
	if (ifs_meta_main.paused ~= g.paused) then
		g.paused = ifs_meta_main.paused
		if (g.paused) then
			bfxt_Log("paused (the galaxy's pause menu)")
		else
			bfxt_Log("pause menu closed")
		end
	end
end

-- the battle starting (the game's own, logged first)
function bfxt_BattleWrap()
	if (gBFXT_BattleOrig or (not ifs_meta_battle_fnOnEnter)) then
		return
	end
	gBFXT_BattleOrig = ifs_meta_battle_fnOnEnter
	ifs_meta_battle_fnOnEnter = function(this)
		local jt = metagame_state_local.mJoystickTeams
		local pick = metagame_state.pickteam
		local v = metagame_vote_state or {}
		bfxt_Log(format("BATTLE: %s attacks %s (map %s), %s defends; bonuses - side 1 %s, side 2 %s; players 1 / 2 on "
			.. "teams %s / %s | %s", bfxt_Side(pick), tostring(metagame_state.pickplanet),
			tostring(metagame_state.NextMapName), bfxt_Side(3 - (pick or 1)), tostring(v.gBonus1Vote),
			tostring(v.gBonus2Vote), tostring(jt[1]), tostring(jt[2]), bfxt_State()))
		gBFXT_BattleOrig(this)
	end
end

-- one press handed to the game's own input on joystick j (0-based) - logged by the scanner; the presses from the pads
-- (not the AI's) caught when the game's script stops, for the log
function bfxs_MainGame(this, name, j, bFromAI, who, how, catch)
	if (not bfxt_On()) then
		gBFXS_MainOrig[name](this, j, bFromAI)
		return
	end
	local before = bfxt_Snap()
	local verdict = bfxt_Verdict(name, j, bFromAI)
	local team = nil
	if (type(j) == "number") then
		team = metagame_state_local.mJoystickTeams[j + 1]
	end
	local err = nil
	if (catch and (type(call) == "function")) then
		gBFXT_Err = nil
		call(gBFXS_MainOrig[name], { this, j, bFromAI }, "x", bfxt_OnErr)
		err = gBFXT_Err
	else
		gBFXS_MainOrig[name](this, j, bFromAI)
	end
	bfxt_Press(name, who, how, j, team, bFromAI, verdict, before, err)
	if (err) then
		bfxt_Error(format("%s from %s", gBFXS_MainShort[name] or name, who), err)
	end
end

-- one press of player p (1 - 4): only a commander's, as the joystick of his side
function bfxs_MainAs(this, name, p, fromPad)
	local cfg = gBFXS_MainCfg
	bfxs_MainUnmap()
	local side = bfxs_MainTarget(cfg, p)
	local how = bfxt_Pad(p)
	if (not side) then
		if (gBFXS_MainBlocked ~= p) then
			gBFXS_MainBlocked = p
			bfxs_Log(format("galaxy: player %d's %s dropped - he does not command a side (side 1: player %d, side 2: player %d)",
				p, gBFXS_MainShort[name] or name, cfg.cmd[1], cfg.cmd[2]))
		end
		bfxt_Dropped(format("p%d", p), format("player %d's %s (%s) - he commands no side (side 1: player %d, side 2: player "
			.. "%d) | %s", p, gBFXS_MainShort[name] or name, how, cfg.cmd[1], cfg.cmd[2], bfxt_Turn()))
		return
	end
	gBFXS_MainBlocked = nil
	if (fromPad and (name == "Input_Accept")) then
		this.CurButton = "_ok" -- (the game sets it only for its own pads)
	end
	local who = format("player %d, %s's commander,", p, bfxt_Side(side))
	local jt = metagame_state_local.mJoystickTeams
	if (jt[1] == side) then
		bfxs_MainGame(this, name, 0, nil, who, how, 1)
	elseif (jt[2] == side) then
		bfxs_MainGame(this, name, 1, nil, who, how, 1)
	else
		gBFXS_MainKeep = { jt[2] } -- (put back after the call; and by bfxs_MainUnmap if the call broke off)
		jt[2] = side
		bfxs_MainGame(this, name, 1, nil, who, how .. "; joystick 1 lent his side for this press", 1)
		bfxs_MainUnmap()
	end
end

-- joystick 2's own team back, if a call left it changed
gBFXS_MainKeep = nil
function bfxs_MainUnmap()
	if (gBFXS_MainKeep) then
		metagame_state_local.mJoystickTeams[2] = gBFXS_MainKeep[1]
		gBFXS_MainKeep = nil
	end
end

-- the game's own press (its joysticks 0 / 1 = players 1 / 2, the AI's with bFromAI)
function bfxs_MainInput(this, name, iJoystick, bFromAI)
	bfxs_MainUnmap()
	-- (BACK clicked with the mouse is the game's pause; a pad's A the game turns into "_ok" itself)
	local back = (name == "Input_Accept") and (this.CurButton == "_back") and (not ScriptCB_IsLastInputFromJoystick())
	if ((not gBFXS_MainCfg) or bFromAI or ifs_meta_main.paused or back) then
		local who = "the AI"
		local how = "metagame_ai"
		if (not bFromAI) then
			who = "the game's joystick " .. tostring(iJoystick)
			how = "the game's own"
			if (iJoystick == 0) then
				who = "player 1"
				how = bfxt_Pad(1)
			elseif (iJoystick == 1) then
				who = "player 2"
				how = bfxt_Pad(2)
			end
		end
		bfxs_MainGame(this, name, iJoystick, bFromAI, who, how, not bFromAI)
		return
	end
	if ((type(iJoystick) == "number") and (iJoystick > 1.5)) then
		bfxt_Dropped(format("j%d", iJoystick), format("the game's joystick %d's %s - players 3 / 4 come from BF1GameExt's pads",
			iJoystick, gBFXS_MainShort[name] or name))
		return -- (pads 3 / 4 come from BF1GameExt, not from the game)
	end
	local p = 1
	if ((type(iJoystick) == "number") and (iJoystick > 0.5)) then
		p = 2
	end
	bfxs_MainAs(this, name, p, nil)
end

function bfxs_MainWrap(name)
	gBFXS_MainOrig[name] = ifs_meta_main[name]
	ifs_meta_main[name] = function(this, iJoystick, bFromAI)
		bfxs_MainInput(this, %name, iJoystick, bFromAI)
	end
end
bfxs_MainWrap("Input_GeneralLeft")
bfxs_MainWrap("Input_GeneralRight")
bfxs_MainWrap("Input_GeneralUp")
bfxs_MainWrap("Input_GeneralDown")
bfxs_MainWrap("Input_Accept")
bfxs_MainWrap("Input_Misc")

ifs_meta_main.Enter = function(this, bFwd)
	-- (21g-r4: back from a battle? - the game's Enter reloads the conquest and plays the result)
	local back = nil
	local pickWas = metagame_state.pickteam
	if (bfxt_On() and bFwd and ScriptCB_IsMetagameStateSaved and ScriptCB_IsMetagameStateSaved()) then
		back = 1
	end
	gBFXS_MainOrig.Enter(this, bFwd)
	bfxs_GCAIJoy("the galaxy opened")
	gBFXS_MainCfg = bfxs_MainCfg()
	gBFXS_MainBlocked = nil
	if (gBFXS_MainCfg) then
		local c = gBFXS_MainCfg
		bfxs_Log(format("galaxy (bFwd %s, %d players): sides - player 1 %s, player 2 %s, player 3 %s, player 4 %s; side 1 moved "
			.. "by player %d, side 2 by player %d (0: the AI)", tostring(bFwd), c.n, tostring(c.side[1]), tostring(c.side[2]),
			tostring(c.side[3]), tostring(c.side[4]), c.cmd[1], c.cmd[2]))
		bfxs_Ask("pad3reset")
		bfxs_Ask("pad4reset")
	end
	if (bfxt_On()) then
		bfxt_BattleWrap()
		local c = gBFXS_MainCfg
		local who = "the game's own: players 1 / 2 move their joysticks' sides"
		if (c) then
			who = format("%d players - sides: player 1 %s, player 2 %s, player 3 %s, player 4 %s; commanders: side 1 player %d, "
				.. "side 2 player %d (0: the AI)", c.n, tostring(c.side[1]), tostring(c.side[2]), tostring(c.side[3]),
				tostring(c.side[4]), c.cmd[1], c.cmd[2])
		end
		bfxt_Log(format("GALAXY MAP (bFwd %s, %s; caught script errors: %s) - %s | %s | %s", tostring(bFwd),
			(ScriptCB_IsSplitscreen() and "split screen") or "one player", (type(call) == "function" and "yes") or "no", who,
			bfxt_Turn(), bfxt_State()))
		if (back) then
			local r = nil
			if (ScriptCB_GetLastBattleVictory) then
				r = ScriptCB_GetLastBattleVictory()
			end
			local rule = "the attacker LOST - the turn passes to the other side"
			if (metagame_state.pickteam == pickWas) then
				rule = "the attacker WON - he attacks again (the game's rule: the turn passes only when the attacker loses)"
			end
			if ((type(r) ~= "number") or (r < 1)) then
				rule = "no result (the battle was left?) - " .. rule
			end
			bfxt_Log(format("BACK FROM THE BATTLE: result %s (1 = the attacker won, 2 = the defender won; sides backwards %s); "
				.. "the attacker was %s, %s picks now: %s", tostring(r), tostring(metagame_state.teamsAreBackwards),
				bfxt_Side(pickWas), bfxt_Side(metagame_state.pickteam), rule))
		end
		gBFXT.sig = nil -- (the turn line next frame)
		gBFXT.choice = nil
	end
end

-- pads 3 / 4: a commander's presses (d-pad / left stick, A)
gBFXS_MainPadEv = { [1] = "Input_Accept", [4] = "Input_GeneralUp", [5] = "Input_GeneralDown", [6] = "Input_GeneralLeft",
	[7] = "Input_GeneralRight" }
ifs_meta_main.Update = function(this, fDt)
	bfxs_MainUnmap()
	bfxs_GCAIJoy("a frame")
	if (gBFXS_MainOrig.Update) then
		if (bfxt_On() and (type(call) == "function")) then
			gBFXT_Err = nil
			call(gBFXS_MainOrig.Update, { this, fDt }, "x", bfxt_OnErr)
			if (gBFXT_Err) then
				bfxt_Error("its Update", gBFXT_Err)
			end
		else
			gBFXS_MainOrig.Update(this, fDt)
		end
	end
	bfxs_MainUnmap()
	if (bfxt_On()) then
		bfxt_Frame(this, fDt)
	end
	local cfg = gBFXS_MainCfg
	if (not cfg) then
		return
	end
	local p
	for p = 3, cfg.n do
		local m = 0
		local ev = bfxs_Ask(format("pad%d", p))
		while ((ev > 0) and (m < 8)) do
			local name = gBFXS_MainPadEv[ev]
			if (name and (not ifs_meta_main.paused)) then
				bfxs_MainAs(this, name, p, 1)
			end
			m = m + 1
			ev = bfxs_Ask(format("pad%d", p))
		end
	end
end

bfxs_Log(format("ifs_meta_main patched (%s, %s): the galaxy map - with 3 / 4 players each side moved by its commander only "
	.. "(21j-r8: 2 players too: %s); co-op: the AI's joystick on its team (21g-r4); turn scanner %s", gBFX_Phase or "?",
	gBFX_Screen or "?", (gBFX_GCCmd2 and "GCCommander2P=1") or "no - the game's own", (gBFX_GCTurnScan and "ON ([gcturn] lines)")
	or "off"))
end

-- ---------------------------------------------------------------------------------------------------------------------
-- 21g-r2: ifs_saveop - player 2's temporary profile is never saved (the campaign's briefing and Galactic Conquest save
-- profile 2 when it changed: that would put PLAYER 2 in the profile screen). While BF1GameExt says profile 2 is the
-- temporary one ("BFXQ: temp2"), a SaveProfile for it finds it not changed and goes on as the game does then.
-- ---------------------------------------------------------------------------------------------------------------------
if (ifs_saveop and ifs_saveop.Enter and (not ifs_saveop.bfxsave)) then
ifs_saveop.bfxsave = 1
ifs_saveop.bfxOrigEnter = ifs_saveop.Enter -- (one per table: the game may make two ifs_saveop)

ifs_saveop.Enter = function(this, bFwd)
	if (bFwd and gBFX_TempProfiles and (this.doOp == "SaveProfile") and (this.saveProfileNum == 2) and
		(bfxs_Ask("temp2") == 1)) then
		if (ScriptCB_SetProfileNotDirty) then
			ScriptCB_SetProfileNotDirty(2)
		end
		bfxs_Log("ifs_saveop: SaveProfile 2 - profile 2 is player 2's temporary profile: not saved")
	end
	this.bfxOrigEnter(this, bFwd)
end

bfxs_Log(format("ifs_saveop patched (%s, %s): a temporary profile 2 is never saved", gBFX_Phase or "?", gBFX_Screen or "?"))
end
