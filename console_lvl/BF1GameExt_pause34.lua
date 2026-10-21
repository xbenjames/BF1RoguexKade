-- BF1GameExt_pause34.lua   (file build p21i-r10)
-- 21i-r10: players 3 / 4's GAME OPTIONS get an AUTO AIM row (ON / OFF, each his own: "aimp get / set K" - BF1GameExt keeps it
-- in AutoAimP3 / P4 and hands it to the aim assist for that player only). "-" when BF1GameExt does not keep it.
-- 21h-r11: r10's RESPAWN hiding for player 1's pause menu over the stats screen moved out of this file - it is needed in
-- every match now (1 / 2-player too) and this script runs only in 3 / 4-player ones; BF1GameExt runs it for ifs_pausemenu
-- itself (controller_scan.cpp). This file is r7's again otherwise.
-- 21h-r7: the gamepad page's orange tabs as player 1's (from his and player 4's page side by side): up 12 (they sat half over the
-- blue window's top edge - now their bottom is on it), the bars narrower (their end pieces are not scaled, so 207 -> 186), the
-- STARFIGHTER tab 19 to the right, the two tabs not shown at 0.6 (a darker orange, as his) instead of 0.4 (nearly see-through).
-- 21h-r6: (1) THEIR BUTTONS CHANGED AS PLAYER 1'S: the gamepad page starts on the pad (LB's label lit orange); the d-pad / stick
-- moves between the labels that can change (LB LT X A B Y RB RT R3, L3 when it is not BF1GameExt's sprint), down from the
-- bottom one to the settings; A opens the list of actions beside it (player 1's list, the shown tab's), A picks ("rp
-- ctlfnset": his own map, kept in BF1GameExt.ini), B closes it. RESET TO DEFAULTS / Y: the shown tab's buttons back to the
-- game's too. ([Splitscreen] RemapP34=0: as r5.) (2) The page as player 1's: one SQUAD COMMANDS line (the d-pad's other
-- three hidden while they are squad commands - the SPRINT label no longer under one), RESET TO DEFAULTS in full, a "B BACK" at
-- player 1's size (the screen's own, twice as big, hidden while a page is up). (3) No RESPAWN while his pause menu is over
-- his spawn screens ("rp down").
-- 21h-r5: PLAYER 1'S PAGES FOR PLAYERS 3 / 4. GAME OPTIONS and CONTROL OPTIONS are copies of player 1's pages (from r4's scan of
-- ifs_opt_general / ifs_opt_controller), built in their own screens at player 1's size on screen (gBFXR_PopScale):
-- GAME OPTIONS - the title, VIEWPOINT (3RD PERSON - their cameras are third person only), DIFFICULTY (the game's, as player 1's
-- page), RESET TO DEFAULTS (difficulty back to MEDIUM). CONTROL OPTIONS goes straight to the gamepad page - the blue window,
-- the INFANTRY / VEHICLE / STARFIGHTER tabs (LB / RB), the pad picture with every button's action from his own button map
-- ("rp ctlfn"), JOYSTICK SENSITIVITY with its slider (his own sensitivity), VIBRATION (NO - players 3 / 4's pads have no
-- rumble), INVERT LOOK / INVERT FLYING (by tab, his own), RESET TO DEFAULTS. Up / down picks, left / right (and A) change, B
-- back. gBFXR_Pages = 0 ([Splitscreen] OptionsPagesP34=0): r4's lists instead.
-- 21h-r4: (1) Players 3 / 4's YES / NO popup window at player 1's size: player 1's popup is drawn in the interface root (1024
-- x 624, 90% safe area) squeezed into his quarter, theirs in their quarter's root (512 x 312, 88%) - so theirs came out about
-- twice as big. Now it is built at gBFXR_PopScale (PopupScaleP34 / 100, 0.51 = the two roots' ratio): its window, frame
-- corners, question text (text scale), YES / NO and their A / B icons. (2) Player 2's OPTIONS in 3 / 4-player matches
-- (gBFXR_P2Opts, [Splitscreen] OptionsP2): no VIDEO OPTIONS, and CONTROL OPTIONS opens the gamepad page (ifs_opt_controller)
-- at once - no keyboard page. Player 1's OPTIONS unchanged. Players 3 / 4's GAME OPTIONS / CONTROL OPTIONS pages as r3 (the
-- copies of player 1's pages come next, from this round's scan).
-- 21h-r3: PLAYER 1'S SCHEME. OPTIONS opens an OPTIONS list with GAME OPTIONS (DIFFICULTY, the game's own setting, as player
-- 1's GAME OPTIONS page has it) and CONTROL OPTIONS (his invert look / invert flying / sensitivity); B goes back one step, as
-- player 1's pages do. RESPAWN / RESTART MISSION / QUIT ask in a popup window like player 1's (the game's Popup_YesNo look:
-- its frame, its question text, YES / NO with the A / B icons; A = yes, B = no), built inside his own screen so it is drawn in
-- his quarter. Player 2's pause menu gets player 1's accept / back / cancel sounds (gBFXR_P2Sounds, [Splitscreen]
-- PauseSoundsP2; only in 3 / 4-player matches - this script does not run in 2-player ones).
-- 21h-r2: REAL PAUSE MENUS FOR PLAYERS 3 AND 4 - the game's own kind of screen, made by the game's own script functions:
-- ifs_pausemenu3 (viewport 2: player 3's quarter / half) and ifs_pausemenu4 (viewport 3: player 4's quarter), built the way
-- ifs_pausemenu2 is (NewIFShellScreen2, AddVerticalButtons with the split screen layout, the same container place: 0.6 / 0.5,
-- y 20) right after the game has made ifs_pausemenu2. BF1GameExt runs them on two IF managers of its own (players 3 / 4's
-- pads), opens them on Start and does what they ask ("BFXQ: rp ..."): RESUME GAME / OPTIONS / RESPAWN / RESTART MISSION / QUIT.
-- Each screen keeps its own highlighted item (never the game's gCurHiliteButton / gCurHiliteButton2).
-- Run by BF1GameExt with the game's Lua while the game makes ifs_pausemenu2 (the copy built into the DLL, or this file next
-- to the exe), with gBFXR_Players (this match's player count) and gBFXR_P2Sounds set first. Lua 4.0: no upvalues, every
-- helper a global bfxr_*. Its log lines come in as ScriptCB_GetPlatform("BFXR: ...") - [rp34] lua lines in
-- BF1GameExt_split.log.

function bfxr_Log(s)
	ScriptCB_GetPlatform("BFXR: " .. s)
end

-- a question to BF1GameExt: a number back (-1 if nothing answered)
function bfxr_Ask(what)
	local r = ScriptCB_GetPlatform("BFXQ: " .. what)
	if (type(r) == "number") then
		return r
	end
	return -1
end

-- the items (one layout for both screens: AddVerticalButtons fills it the same way each time). yes / no: only if the popup
-- window could not be made (the old YES / NO list)
bfxr_layout = {
	ySpacing = 5,
	width = 220,
	font = "gamefont_small",
	buttonlist = {
		{ tag = "resume", string = "common.resume", },
		{ tag = "opts", string = "ifs.main.options", },
		{ tag = "suicide", string = "game.pause.suicide", },
		{ tag = "restart", string = "common.restart", },
		{ tag = "quit", string = "common.quit", },
		{ tag = "gameopts", string = "ifs.GameOpt.title", },
		{ tag = "ctlopts", string = "ifs.GameOpt.pc_optionstitle", },
		{ tag = "diff", string = "common.accept", },
		{ tag = "invert", string = "common.accept", },
		{ tag = "fly", string = "common.accept", },
		{ tag = "sens", string = "common.accept", },
		{ tag = "yes", string = "common.yes", },
		{ tag = "no", string = "common.no", },
	},
	title = "game.pause.title",
}

function bfxr_Text(obj, s)
	if (obj) then
		IFText_fnSetUString(obj, ScriptCB_tounicode(s))
	end
end

-- the highlighted item's pulse, kept per screen (the game's globals put back after)
function bfxr_Hilite(this, dt)
	if (not this.bfxrHi) then
		return
	end
	local a = gButton_CurSizeAdd
	local d = gButton_CurDir
	gButton_CurSizeAdd = this.bfxrSz or 0.5
	gButton_CurDir = this.bfxrDir or 2
	IFButton_fnHilight(this.bfxrHi, 1, dt)
	this.bfxrSz = gButton_CurSizeAdd
	this.bfxrDir = gButton_CurDir
	gButton_CurSizeAdd = a
	gButton_CurDir = d
end

function bfxr_Select(this, tag)
	if (this.bfxrHi) then
		IFButton_fnHilight(this.bfxrHi, nil, 0)
		IFButton_fnSelect(this.bfxrHi, nil)
	end
	this.bfxrHi = nil
	this.CurButton = tag
	if (tag) then
		this.bfxrHi = this.buttons[tag]
	end
	if (this.bfxrHi) then
		IFButton_fnSelect(this.bfxrHi, 1)
		bfxr_Hilite(this, 0)
	end
end

function bfxr_OnOff(v)
	if (v == 1) then
		return "ON"
	elseif (v == 0) then
		return "OFF"
	end
	return "?"
end

-- CONTROL OPTIONS: the three items, with their values
function bfxr_OptsLabels(this)
	local k = this.bfxrPlayer
	local inv = bfxr_Ask(format("rp ctlget %d 0", k))
	local fly = bfxr_Ask(format("rp ctlget %d 1", k))
	local sens = bfxr_Ask(format("rp ctlget %d 2", k))
	bfxr_Text(this.buttons.invert.label, "INVERT LOOK   " .. bfxr_OnOff(inv))
	bfxr_Text(this.buttons.fly.label, "INVERT FLYING   " .. bfxr_OnOff(fly))
	bfxr_Text(this.buttons.sens.label, format("SENSITIVITY   < %d >", sens))
end

-- GAME OPTIONS: the game's difficulty (ScriptCB_Get / SetDifficulty, as player 1's GAME OPTIONS page) - only while the game's
-- pausing viewport is player 1's / 2's (the profile it reads)
function bfxr_DiffOk()
	local v = ScriptCB_GetPausingViewport()
	return (type(v) ~= "number") or (v <= 1)
end

function bfxr_DiffLabel(this)
	local s = "-"
	if (bfxr_DiffOk()) then
		local d = ScriptCB_GetDifficulty()
		if (d == 1) then
			s = "EASY"
		elseif (d == 2) then
			s = "MEDIUM"
		else
			s = "HARD"
		end
	end
	bfxr_Text(this.buttons.diff.label, "DIFFICULTY   < " .. s .. " >")
end

function bfxr_DiffChange(this, step)
	if (not bfxr_DiffOk()) then
		ifelm_shellscreen_fnPlaySound(this.errorSound)
		return
	end
	local d = ScriptCB_GetDifficulty()
	if (step > 0) then
		d = mod(d, 3) + 1
	else
		d = mod(d + 1, 3) + 1
	end
	ScriptCB_SetDifficulty(d)
	ifelm_shellscreen_fnPlaySound(this.acceptSound)
	bfxr_DiffLabel(this)
	bfxr_Hilite(this, 0)
	bfxr_Log(format("player %d: GAME OPTIONS - difficulty now %d", this.bfxrPlayer, ScriptCB_GetDifficulty()))
end

-- the popup window (player 1's Popup_YesNo look) on / off; the pause items hidden while it is up, as player 2's are
function bfxr_PopVis(this, on)
	if (this.bfxrPop) then
		IFObj_fnSetVis(this.bfxrPop, on)
	end
	IFObj_fnSetVis(this.buttons, not on)
end

function bfxr_PopAsk(this, what)
	local p = this.bfxrPop
	local key = "game.pause.suicide_prompt"
	if (what == "restart") then
		key = "ifs.pause.warn_restart"
	elseif (what == "quit") then
		if (ScriptCB_AreMetagameRulesOn()) then
			key = "ifs.pause.warn_quit_meta"
		else
			key = "ifs.pause.warn_quit"
		end
	end
	IFText_fnSetString(p.title, key)
	IFButton_fnSelect(p.buttons.yes, nil)
	IFButton_fnSelect(p.buttons.no, 1)
	p.CurButton = "no"
	bfxr_PopVis(this, 1)
end

-- which items are shown: "main" (the pause items), "optsmenu" (OPTIONS: GAME OPTIONS / CONTROL OPTIONS), "game" (GAME
-- OPTIONS), "opts" (CONTROL OPTIONS), "confirm" (the popup - or YES / NO in the list if there is no popup)
function bfxr_Show(this, mode, cur)
	local b = this.buttons
	local i = 1
	while (bfxr_layout.buttonlist[i]) do
		b[bfxr_layout.buttonlist[i].tag].hidden = 1
		i = i + 1
	end
	this.bfxrMode = mode
	if (this.bfxrPBg) then
		bfxr_PagesVis(this, nil, nil)
	end
	if ((mode == "confirm") and this.bfxrPop) then
		if (this.bfxrHi) then
			IFButton_fnHilight(this.bfxrHi, nil, 0)
			IFButton_fnSelect(this.bfxrHi, nil)
		end
		this.bfxrHi = nil
		bfxr_PopAsk(this, this.bfxrWhat)
		return
	end
	bfxr_PopVis(this, nil)
	if (mode == "main") then
		b.resume.hidden = nil
		b.opts.hidden = nil
		-- 21h-r6: no RESPAWN while his pause menu is over his spawn screens (he has no soldier)
		if (bfxr_Ask(format("rp down %d", this.bfxrPlayer)) ~= 1) then
			b.suicide.hidden = nil
		end
		b.quit.hidden = nil
		if (not ScriptCB_AreMetagameRulesOn()) then
			b.restart.hidden = nil
		end
		IFText_fnSetString(b._titlebar_, "game.pause.title")
	elseif (mode == "optsmenu") then
		b.gameopts.hidden = nil
		b.ctlopts.hidden = nil
		IFText_fnSetString(b._titlebar_, "ifs.Main.options")
	elseif (mode == "game") then
		b.diff.hidden = nil
		IFText_fnSetString(b._titlebar_, "ifs.GameOpt.title")
		bfxr_DiffLabel(this)
	elseif (mode == "opts") then
		b.invert.hidden = nil
		b.fly.hidden = nil
		b.sens.hidden = nil
		IFText_fnSetString(b._titlebar_, "ifs.GameOpt.pc_optionstitle")
		bfxr_OptsLabels(this)
	else
		b.yes.hidden = nil
		b.no.hidden = nil
		if (this.bfxrWhat == "suicide") then
			bfxr_Text(b._titlebar_, "RESPAWN?")
		elseif (this.bfxrWhat == "restart") then
			bfxr_Text(b._titlebar_, "RESTART MISSION?")
		else
			bfxr_Text(b._titlebar_, "QUIT?")
		end
	end
	local first = ShowHideVerticalButtons(b, bfxr_layout)
	if ((not cur) or b[cur].hidden) then
		cur = first
	end
	bfxr_Select(this, cur)
end

-- asks BF1GameExt to do it (it closes the screen); nothing more from this screen until it is up again
function bfxr_Do(this, what)
	this.bfxrBusy = 1
	local r = bfxr_Ask(format("rp %s %d", what, this.bfxrPlayer))
	bfxr_Log(format("player %d: %s - asked BF1GameExt (%d)", this.bfxrPlayer, what, r))
end

function bfxr_Enter(this, bFwd)
	this.bfxrBusy = nil
	this.bfxrSz = 0.5
	this.bfxrDir = 2
	this.bfxrFlash = random() * 15 + 10
	this.bfxrWhat = nil
	bfxr_Show(this, "main", "resume")
	IFFlashyText_StartFlashiness(this)
	if (bFwd) then
		ifelm_shellscreen_fnPlaySound(this.acceptSound)
	end
	bfxr_Log(format("player %d: %s up (Enter %s): %s, popup %s", this.bfxrPlayer, tostring(this.ScreenName), tostring(bFwd),
		tostring(this.CurButton), this.bfxrPop and "made" or "none"))
end

function bfxr_Leave(this, bFwd)
	if (this.bfxrHi) then
		IFButton_fnHilight(this.bfxrHi, nil, 0)
		IFButton_fnSelect(this.bfxrHi, nil)
	end
	this.bfxrHi = nil
	if ((this.bfxrMode == "opts") or (this.bfxrMode == "ppage")) then
		bfxr_Ask(format("rp ctlsave %d", this.bfxrPlayer))
	end
	if (this.bfxrPop) then
		IFObj_fnSetVis(this.bfxrPop, nil)
	end
	if (this.bfxrPBg) then
		bfxr_PagesVis(this, nil, nil)
	end
	this.bfxrMode = nil
end

function bfxr_Update(this, fDt)
	bfxr_Hilite(this, fDt)
	this.bfxrFlash = (this.bfxrFlash or 10) - fDt
	if (this.bfxrFlash <= 0) then
		IFFlashyText_DoRandomFlashiness(this)
		this.bfxrFlash = random() * 15 + 10
	end
end

function bfxr_Move(this, dir)
	if (this.bfxrBusy or (this.bfxrMode == "confirm" and this.bfxrPop)) then
		return
	end
	if ((this.bfxrMode == "gpage") or (this.bfxrMode == "ppage")) then
		if (dir == "link_up") then
			bfxr_PageMove(this, -1)
		else
			bfxr_PageMove(this, 1)
		end
		return
	end
	this.CurButton = FollowButtonLink(this.buttons, this.CurButton, dir)
	if (this.CurButton) then
		if (this.bfxrHi ~= this.buttons[this.CurButton]) then
			if (this.bfxrHi) then
				IFButton_fnHilight(this.bfxrHi, nil, 0)
			end
			this.bfxrHi = this.buttons[this.CurButton]
			bfxr_Hilite(this, 0)
		end
	end
end

function bfxr_Up(this)
	bfxr_Move(this, "link_up")
end

function bfxr_Down(this)
	bfxr_Move(this, "link_down")
end

-- CONTROL OPTIONS: left / right (and A) change the item picked
function bfxr_OptChange(this, step)
	local k = this.bfxrPlayer
	local c = this.CurButton
	local i = 0
	if (c == "fly") then
		i = 1
	elseif (c == "sens") then
		i = 2
	end
	local v = bfxr_Ask(format("rp ctlget %d %d", k, i))
	if (i == 2) then
		v = v + step
		if (v < 1) then
			v = 1
		elseif (v > 10) then
			v = 10
		end
	else
		v = 1 - v
	end
	bfxr_Ask(format("rp ctlset %d %d %d", k, i, v))
	ScriptCB_SndPlaySound("shell_select_change")
	bfxr_OptsLabels(this)
	bfxr_Hilite(this, 0)
end

-- the popup's YES / NO highlight (left / right), as the game's buttons are selected
function bfxr_PopToggle(this)
	local p = this.bfxrPop
	if (p.CurButton == "yes") then
		p.CurButton = "no"
	else
		p.CurButton = "yes"
	end
	IFButton_fnSelect(p.buttons.yes, p.CurButton == "yes")
	IFButton_fnSelect(p.buttons.no, p.CurButton == "no")
	ScriptCB_SndPlaySound("shell_select_change")
end

function bfxr_Side(this, step)
	if (this.bfxrBusy) then
		return
	end
	if ((this.bfxrMode == "ppage") and gBFXR_ListOpen[this.bfxrPlayer]) then
		return
	elseif ((this.bfxrMode == "ppage") and this.bfxrPadKey) then
		if (step < 0) then
			bfxr_PadMove(this, "left")
		else
			bfxr_PadMove(this, "right")
		end
	elseif ((this.bfxrMode == "gpage") or (this.bfxrMode == "ppage")) then
		bfxr_PageChange(this, step)
	elseif (this.bfxrMode == "opts") then
		bfxr_OptChange(this, step)
	elseif (this.bfxrMode == "game") then
		bfxr_DiffChange(this, step)
	elseif ((this.bfxrMode == "confirm") and this.bfxrPop) then
		bfxr_PopToggle(this)
	end
end

function bfxr_Left(this)
	bfxr_Side(this, -1)
end

function bfxr_Right(this)
	bfxr_Side(this, 1)
end

-- the popup's answer (A = yes, B = no - player 1's popup on a pad)
function bfxr_Answer(this, yes)
	ifelm_shellscreen_fnPlaySound("shell_menu_accept")
	if (yes) then
		ifelm_shellscreen_fnPlaySound(this.acceptSound)
		bfxr_Do(this, this.bfxrWhat)
	else
		ifelm_shellscreen_fnPlaySound(this.cancelSound)
		bfxr_Show(this, "main", this.bfxrWhat)
	end
end

function bfxr_Accept(this)
	if (this.bfxrBusy) then
		return
	end
	local c = this.CurButton
	local m = this.bfxrMode
	if ((m == "ppage") and gBFXR_ListOpen[this.bfxrPlayer]) then
		bfxr_ListPick(this)
		return
	elseif ((m == "ppage") and this.bfxrPadKey) then
		bfxr_ListOpen(this)
		return
	elseif ((m == "gpage") or (m == "ppage")) then
		bfxr_PageChange(this, 1)
		return
	elseif (m == "opts") then
		bfxr_OptChange(this, 1)
		return
	elseif (m == "game") then
		bfxr_DiffChange(this, 1)
		return
	elseif (m == "confirm") then
		if (this.bfxrPop) then
			bfxr_Answer(this, 1)
		else
			bfxr_Answer(this, c == "yes")
		end
		return
	end
	ifelm_shellscreen_fnPlaySound(this.acceptSound)
	if (m == "optsmenu") then
		if (c == "gameopts") then
			if (this.bfxrGItems) then
				bfxr_ShowPage(this, "gpage", 1)
			else
				bfxr_Show(this, "game", "diff")
			end
		elseif (c == "ctlopts") then
			if (bfxr_Ask(format("rp ctllive %d", this.bfxrPlayer)) == 1) then
				if (this.bfxrPBg) then
					this.bfxrTab = 0
					bfxr_ShowPage(this, "ppage", 1)
				else
					bfxr_Show(this, "opts", "invert")
				end
			else
				ifelm_shellscreen_fnPlaySound(this.errorSound)
				bfxr_Log(format("player %d: CONTROL OPTIONS - his controls cannot be changed now (pads 3 / 4 off?)", this.bfxrPlayer))
			end
		end
		return
	end
	if (c == "resume") then
		bfxr_Do(this, "resume")
	elseif (c == "opts") then
		bfxr_Show(this, "optsmenu", "gameopts")
	elseif ((c == "suicide") or (c == "restart") or (c == "quit")) then
		this.bfxrWhat = c
		bfxr_Show(this, "confirm", "no")
	end
end

function bfxr_Back(this)
	if (this.bfxrBusy) then
		return
	end
	local m = this.bfxrMode
	if (m == "confirm") then
		bfxr_Answer(this, nil)
	elseif ((m == "ppage") and gBFXR_ListOpen[this.bfxrPlayer]) then
		bfxr_ListClose(this)
		ifelm_shellscreen_fnPlaySound(this.cancelSound)
	elseif (m == "ppage") then
		ifelm_shellscreen_fnPlaySound(this.exitSound)
		bfxr_Ask(format("rp ctlsave %d", this.bfxrPlayer))
		bfxr_Show(this, "optsmenu", "ctlopts")
	elseif (m == "gpage") then
		ifelm_shellscreen_fnPlaySound(this.exitSound)
		bfxr_Show(this, "optsmenu", "gameopts")
	elseif (m == "opts") then
		ifelm_shellscreen_fnPlaySound(this.exitSound)
		bfxr_Ask(format("rp ctlsave %d", this.bfxrPlayer))
		bfxr_Show(this, "optsmenu", "ctlopts")
	elseif (m == "game") then
		ifelm_shellscreen_fnPlaySound(this.exitSound)
		bfxr_Show(this, "optsmenu", "gameopts")
	elseif (m == "optsmenu") then
		ifelm_shellscreen_fnPlaySound(this.exitSound)
		bfxr_Show(this, "main", "opts")
	else
		ifelm_shellscreen_fnPlaySound(this.exitSound)
		bfxr_Do(this, "resume")
	end
end

function bfxr_Start(this)
	if (this.bfxrBusy) then
		return
	end
	if ((this.bfxrMode == "opts") or (this.bfxrMode == "ppage")) then
		bfxr_Ask(format("rp ctlsave %d", this.bfxrPlayer))
	end
	ifelm_shellscreen_fnPlaySound(this.exitSound)
	bfxr_Do(this, "resume")
end

-- player 1's Popup_YesNo, rebuilt as a child of his screen (so it is drawn in his quarter): the game's button window (its
-- frame), the question text, YES / NO with the A / B icons. 21h-r4: built at scale f (player 1's popup size on screen: his is
-- drawn in the interface root, 1024 units wide in a quarter; ours in the quarter's own root, 512 units). nil if the game's
-- helpers are not there.
function bfxr_SkinSize(skin, w, h, c)
	local x1 = w * -0.5
	local x2 = x1 + c
	local x4 = w * 0.5
	local x3 = x4 - c
	local y1 = h * -0.5
	local y2 = y1 + c
	local y4 = h * 0.5
	local y3 = y4 - c
	if (y3 < y2) then
		y2 = 0
		y3 = 0
	end
	IFImage_fnSetTexturePos(skin.ChunkTL, x1, y1, x2, y2)
	IFImage_fnSetTexturePos(skin.ChunkTC, x2, y1, x3, y2)
	IFImage_fnSetTexturePos(skin.ChunkTR, x3, y1, x4, y2)
	IFImage_fnSetTexturePos(skin.ChunkCL, x1, y2, x2, y3)
	IFImage_fnSetTexturePos(skin.ChunkC, x2, y2, x3, y3)
	IFImage_fnSetTexturePos(skin.ChunkCR, x3, y2, x4, y3)
	IFImage_fnSetTexturePos(skin.ChunkBL, x1, y3, x2, y4)
	IFImage_fnSetTexturePos(skin.ChunkBC, x2, y3, x3, y4)
	IFImage_fnSetTexturePos(skin.ChunkBR, x3, y3, x4, y4)
end

function bfxr_NewPop()
	if ((not NewButtonWindow) or (not AddHorizontalButtons) or (not Horiz_YesNoButtons_layout)) then
		return nil
	end
	local f = (gBFXR_PopScale or 51) / 100
	if ((f < 0.2) or (f > 1.5)) then
		f = 0.51
	end
	local w = 300 * f
	local h = 180 * f
	local p = NewButtonWindow {
		ScreenRelativeX = 0.5,
		ScreenRelativeY = 0.5,
		height = h,
		width = w,
		ZPos = 50,
		title = NewIFText {
			font = "gamefont_medium",
			textw = 260 * f,
			texth = 120 * f,
			y = -80 * f,
			flashy = 0,
			HScale = f,
			VScale = f,
		},
		buttons = NewIFContainer {
			y = 60 * f,
		},
	}
	if (p.skin and p.skin.ChunkTL) then
		bfxr_SkinSize(p.skin, w, h, 16 * f)
	end
	-- the game's YES / NO layout at scale f (its own table is left as it is)
	local L = Horiz_YesNoButtons_layout
	local lay = {
		xLeft = (L.xLeft or -40) * f,
		yTop = (L.yTop or 0) * f,
		xWidth = (L.xWidth or 80) * f,
		xSpacing = (L.xSpacing or 5) * f,
		height = (L.height or 45) * f,
		font = L.font or "gamefont_medium",
		flashy = L.flashy,
		nocreatebackground = L.nocreatebackground,
		buttonlist = {},
	}
	local i = 1
	while (L.buttonlist[i]) do
		lay.buttonlist[i] = { tag = L.buttonlist[i].tag, string = L.buttonlist[i].string, gamepad = L.buttonlist[i].gamepad, }
		i = i + 1
	end
	AddHorizontalButtons(p.buttons, lay)
	i = 1
	while (lay.buttonlist[i]) do
		local b = p.buttons[lay.buttonlist[i].tag]
		if (b and b.label) then
			b.label.HScale = f
			b.label.VScale = f
		end
		i = i + 1
	end
	if (AddGamepadIcons and NewIFGamepadImage and gGamepadTexture) then
		AddGamepadIcons(p.buttons, lay, 1)
		i = 1
		while (lay.buttonlist[i]) do
			local ic = p.buttons[lay.buttonlist[i].tag .. "Icon"]
			if (ic and ic.localpos_l) then
				ic.localpos_l = ic.localpos_l * f
				ic.localpos_t = ic.localpos_t * f
				ic.localpos_r = ic.localpos_r * f
				ic.localpos_b = ic.localpos_b * f
			end
			i = i + 1
		end
	end
	p.bfxrScale = f
	return p
end

-- 21h-r5: PLAYER 1'S GAME OPTIONS AND GAMEPAD PAGES, copied for his screen. Built with the game's own element functions and
-- the numbers of player 1's pages (as the game builds them for the interface root, 1024 x 624, safe area 921.6 x 561.6),
-- then scaled to player 1's size on screen (f): offsets and sizes times f, text scaled by f; what the game places from the
-- safe area's corner placed at the same fraction of his quarter. The page elements are children of his screen, hidden
-- unless their page is up (their names: gBFXR_GNames / gBFXR_PNames).
gBFXR_GNames = { "bfxrGTitle", "bfxrGLabels", "bfxrGItems", }
gBFXR_PNames = { "bfxrPBg", "bfxrPTitle", "bfxrPTab0", "bfxrPTab1", "bfxrPTab2", "bfxrPLb", "bfxrPRb", "bfxrPPad", "bfxrPLines",
	"bfxrPLabels", "bfxrPVals", "bfxrPSlider", "bfxrPReset", }
gBFXR_GItems = { "persp", "diff", "autoaim", "reset", } -- 21i-r10: AUTO AIM
gBFXR_PItems = { "joysens", "invert", "reset", }
gBFXR_TabTitle = { [0] = "ifs.Controls.Soldier.title", [1] = "ifs.Controls.Vehicle.title", [2] = "ifs.Controls.Flyer.title", }
-- 21h-r6: the action list per player (globals: a screen table's tables become elements)
gBFXR_List = {}
gBFXR_ListOpen = {}
gBFXR_ListLogged = {}

function bfxr_Scale(t, f, seen)
	if (seen[t]) then
		return
	end
	seen[t] = 1
	if (t.x) then
		t.x = t.x * f
	end
	if (t.y) then
		t.y = t.y * f
	end
	if (t.textw) then
		t.textw = t.textw * f
	end
	if (t.texth) then
		t.texth = t.texth * f
	end
	if (t.localpos_l) then
		t.localpos_l = t.localpos_l * f
		t.localpos_t = t.localpos_t * f
		t.localpos_r = t.localpos_r * f
		t.localpos_b = t.localpos_b * f
	end
	if (t.type == "text") then
		t.HScale = (t.HScale or 1) * f
		t.VScale = (t.VScale or 1) * f
		if (t.bg_width) then
			t.bg_width = t.bg_width * f
		end
		if (t.bgoffsetx) then
			t.bgoffsetx = t.bgoffsetx * f
		end
		if (t.bgoffsety) then
			t.bgoffsety = t.bgoffsety * f
		end
		if (t.leading) then
			t.leading = t.leading * f
		end
	end
	local kids = {}
	local k, v
	for k, v in t do
		if (type(v) == "table") then
			tinsert(kids, v)
		end
	end
	local i = 1
	while (kids[i]) do
		bfxr_Scale(kids[i], f, seen)
		i = i + 1
	end
end

-- a value item as the game's option pages make them (a round button, text only)
function bfxr_Item(x, y, w, h, font, halign)
	local b = NewRoundIFButton {
		x = x,
		y = y,
		btnw = w,
		btnh = h,
		font = font,
		nocreatebackground = 1,
		flashy = 0,
	}
	if (b.label) then
		b.label.halign = halign
		b.label.flashy = 0
	end
	return b
end

function bfxr_Label(x, y, w, h, font, halign, str)
	return NewIFText {
		x = x,
		y = y,
		textw = w,
		texth = h,
		font = font,
		halign = halign,
		valign = "vcenter",
		string = str,
		style = "glow",
		flashy = 0,
		nocreatebackground = 1,
	}
end

-- every game helper the two pages are built with (nil: r4's lists instead)
function bfxr_PagesOk()
	return NewRoundIFButton and NewIFText and NewIFContainer and NewButtonWindow and NewIFImage and NewSegmentLine and
		Platform_btn_map and IFObj_fnSetAlpha and IFImage_fnSetTexturePos
end

-- the names of the missing ones, for the log
function bfxr_PagesMissing()
	local s = ""
	if (not NewRoundIFButton) then s = s .. " NewRoundIFButton" end
	if (not NewIFText) then s = s .. " NewIFText" end
	if (not NewIFContainer) then s = s .. " NewIFContainer" end
	if (not NewButtonWindow) then s = s .. " NewButtonWindow" end
	if (not NewIFImage) then s = s .. " NewIFImage" end
	if (not NewSegmentLine) then s = s .. " NewSegmentLine" end
	if (not Platform_btn_map) then s = s .. " Platform_btn_map" end
	if (not IFObj_fnSetAlpha) then s = s .. " IFObj_fnSetAlpha" end
	if (not IFImage_fnSetTexturePos) then s = s .. " IFImage_fnSetTexturePos" end
	return s
end

-- GAME OPTIONS (ifs_opt_general: rows at y 150 / 190 / 230 from the top, labels right-aligned left of the middle, values left-
-- aligned right of it)
function bfxr_NewGamePage(t, f)
	t.bfxrGTitle = NewIFText {
		ScreenRelativeX = 0.5,
		ScreenRelativeY = 0,
		x = -230,
		y = 10,
		textw = 460,
		texth = 30,
		font = "gamefont_large",
		halign = "hcenter",
		valign = "top",
		string = "ifs.GameOpt.title",
		style = "glow",
		flashy = 1,
		nocreatebackground = 1,
		bg_width = 0,
		bg_tail = 0,
		startdelay = 0,
	}
	t.bfxrGLabels = NewIFContainer { ScreenRelativeX = 0.5, ScreenRelativeY = 0, x = -365, y = 0, }
	t.bfxrGItems = NewIFContainer { ScreenRelativeX = 0.5, ScreenRelativeY = 0, x = 15, y = 0, }
	local titles = { "ifs.gameopt.camera", "ifs.gameopt.diff", "ifs.gameopt.autoaim", "", }
	local i = 1
	while (gBFXR_GItems[i]) do
		local row = 150 + (i - 1) * 40
		local tag = gBFXR_GItems[i]
		t.bfxrGLabels[tag] = bfxr_Label(0, row - 17.5, 350, 35, "gamefont_medium", "right", titles[i])
		local b = bfxr_Item(0, row, 350, 35, "gamefont_medium", "left")
		if (b.label) then
			b.label.x = 0
		end
		t.bfxrGItems[tag] = b
		i = i + 1
	end
	bfxr_Scale(t.bfxrGTitle, f, {})
	bfxr_Scale(t.bfxrGLabels, f, {})
	bfxr_Scale(t.bfxrGItems, f, {})
end

-- the gamepad page (ifs_opt_controller as BF1GameExt's controller page script has it): its window, title, tabs, the pad and its
-- lines, the settings. nil if the game's helpers or data are not there.
function bfxr_NewPadPage(t, f)
	if ((not Platform_btn_map) or (not NewSegmentLine) or (not NewButtonWindow) or (not NewIFImage)) then
		return nil
	end
	local sw = 921.6
	local sh = 561.6
	t.bfxrPBg = NewButtonWindow {
		ScreenRelativeX = 0.5,
		ScreenRelativeY = 338.88 / sh,
		ZPos = 255,
		width = 967.68,
		height = 492.224,
	}
	t.bfxrPTitle = NewIFText {
		ScreenRelativeX = 0.5,
		ScreenRelativeY = 0,
		x = -230,
		y = 0,
		textw = 460,
		texth = 30,
		font = "gamefont_large",
		halign = "hcenter",
		valign = "top",
		string = gBFXR_TabTitle[0],
		style = "glow",
		flashy = 1,
		nocreatebackground = 1,
		bg_width = 0,
		bg_tail = 0,
		startdelay = 0,
	}
	local tabs = { [0] = "ifs.Controls.SoldierTab", [1] = "ifs.Controls.VehicleTab", [2] = "ifs.Controls.FlyerTab", }
	local tx = { [0] = 0, [1] = 1 - 645.12 / sw, [2] = 1 - 303 / sw, } -- 21h-r7: STARFIGHTER 19 right
	local i = 0
	while (i <= 2) do
		t["bfxrPTab" .. i] = NewIFText {
			ScreenRelativeX = tx[i],
			ScreenRelativeY = (70.768 - 12) / sh, -- 21h-r7: up 12 (on the window's top edge, as player 1's)
			x = 0,
			y = 0,
			ZPos = 255,
			textw = 230.4,
			texth = 40,
			font = "gamefont_small",
			halign = "hcenter",
			valign = "top",
			string = tabs[i],
			style = "glow",
			flashy = 1,
			bgleft = "headerbuttonleft",
			bgmid = "headerbuttonmid",
			bgright = "headerbuttonright",
			bg_width = 186, -- 21h-r7: (207.36: its end pieces are not scaled - the bar came out wider than his)
			bg_tail = 0,
			startdelay = 0,
		}
		i = i + 1
	end
	if (NewIFGamepadImage and gGamepadTexture) then
		t.bfxrPLb = NewIFGamepadImage("LB", 0, 0)
		t.bfxrPRb = NewIFGamepadImage("RB", 0, 0)
		t.bfxrPLb.inert = nil
		t.bfxrPRb.inert = nil
		t.bfxrPLb.ScreenRelativeX = 0
		t.bfxrPLb.ScreenRelativeY = (80.768 - 12) / sh
		t.bfxrPRb.ScreenRelativeX = 1
		t.bfxrPRb.ScreenRelativeY = (80.768 - 12) / sh
	end
	t.bfxrPPad = NewIFImage {
		ScreenRelativeX = 0.5,
		ScreenRelativeY = 0.55,
		ZPos = 240,
		texture = "gamepad_xbox360",
		localpos_l = -128,
		localpos_t = -80.64,
		localpos_r = 128,
		localpos_b = 80.64,
		uvs_l = 0,
		uvs_t = 0,
		uvs_r = 1,
		uvs_b = 0.639,
	}
	t.bfxrPLines = NewIFContainer { ScreenRelativeX = 0.5, ScreenRelativeY = 0.55, }
	local k, v
	for k, v in Platform_btn_map do
		if ((type(v) == "table") and (v.type ~= 5) and (v.type ~= 99)) then
			t.bfxrPLines[k] = NewSegmentLine(v.lx1, v.ly1, v.lx2, v.ly2, "")
		end
	end
	t.bfxrPLabels = NewIFContainer { ScreenRelativeX = 0.2, ScreenRelativeY = 0.85, }
	t.bfxrPLabels.joysens = bfxr_Label(0, -10, 260, 20, "gamefont_tiny", "left", "ifs.GameOpt.pc_joysens")
	t.bfxrPLabels.rumble = bfxr_Label(0, 15, 260, 20, "gamefont_tiny", "left", "ifs.gameopt.rumble")
	t.bfxrPLabels.invert = bfxr_Label(0, 40, 260, 20, "gamefont_tiny", "left", "")
	t.bfxrPVals = NewIFContainer { ScreenRelativeX = 0.8, ScreenRelativeY = 0.85, }
	t.bfxrPVals.joysens = bfxr_Item(-130, 0, 260, 20, "gamefont_tiny", "right")
	t.bfxrPVals.rumble = bfxr_Item(-130, 25, 260, 20, "gamefont_tiny", "right")
	t.bfxrPVals.invert = bfxr_Item(-130, 50, 260, 20, "gamefont_tiny", "right")
	t.bfxrPSlider = NewIFContainer { ScreenRelativeX = 0.2, ScreenRelativeY = 0.85, x = 368.64, y = 3, }
	t.bfxrPSlider.bg = NewIFImage { ZPos = 160, texture = "slider_bg", localpos_l = -115.2, localpos_t = -12, localpos_r = 138.24,
		localpos_b = 12, uvs_l = 0, uvs_t = 0, uvs_r = 1, uvs_b = 1, }
	t.bfxrPSlider.fg = NewIFImage { ZPos = 159, texture = "slider_fg", localpos_l = -115.2, localpos_t = -12, localpos_r = -92.16,
		localpos_b = 12, uvs_l = 0, uvs_t = 0, uvs_r = 1, uvs_b = 1, }
	t.bfxrPReset = NewIFContainer { ScreenRelativeX = 0.5, ScreenRelativeY = 1, x = 0, y = -15, }
	t.bfxrPReset.btn = bfxr_Item(0, 0, 200, 25, "gamefont_medium", "left")
	if (t.bfxrPReset.btn.label) then
		t.bfxrPReset.btn.label.string = "common.reset"
		t.bfxrPReset.btn.label.textw = 400 -- 21h-r6: RESET TO DEFAULTS in full
	end
	if (NewIFGamepadImage and gGamepadTexture) then
		t.bfxrPReset.icon = NewIFGamepadImage("Y", -117.5, 0)
		t.bfxrPReset.icon.inert = nil
	end
	-- 21h-r6: the action list (player 1's listbox: 215 x 230 at the pad's centre + 40, five rows of 40, beside the label)
	t.bfxrPList = NewIFContainer { ScreenRelativeX = 0.5, ScreenRelativeY = 0.55, x = 120, y = 40, ZPos = 100, }
	t.bfxrPList.win = NewButtonWindow { ZPos = 200, x = 0, y = 0, width = 215, height = 230, }
	i = 1
	while (i <= 5) do
		t.bfxrPList["row" .. i] = bfxr_Label(-90, -99 + (i - 1) * 40, 148, 40, "gamefont_tiny", "left", "")
		i = i + 1
	end
	-- 21h-r6: the page's "B BACK" at player 1's size (the screen's own is his quarter root's size - twice as big)
	t.bfxrBack = NewIFContainer { ScreenRelativeX = 0, ScreenRelativeY = 1, x = 0, y = -15, }
	t.bfxrBack.icon = NewIFImage { ZPos = 100, texture = "gamepad_xbox360", localpos_l = -15, localpos_t = -12, localpos_r = 15,
		localpos_b = 18, uvs_l = 0.125, uvs_t = 0.639, uvs_r = 0.25, uvs_b = 0.764, }
	t.bfxrBack.label = bfxr_Label(24, -12.5, 200, 25, "gamefont_medium", "left", "common.back")
	i = 1
	while (gBFXR_PNames[i]) do
		if (t[gBFXR_PNames[i]]) then
			bfxr_Scale(t[gBFXR_PNames[i]], f, {})
		end
		i = i + 1
	end
	bfxr_Scale(t.bfxrPList, f, {})
	bfxr_Scale(t.bfxrBack, f, {})
	return 1
end

-- his pages' elements shown / hidden (g: GAME OPTIONS, p: the gamepad page). 21h-r6: the screen's own big "B BACK" (drawn at
-- his quarter's root size) hidden while a page is up and the page's small one shown (player 1's size); the action list closed
function bfxr_PagesVis(this, g, p)
	local i = 1
	while (gBFXR_GNames[i]) do
		if (this[gBFXR_GNames[i]]) then
			IFObj_fnSetVis(this[gBFXR_GNames[i]], g)
		end
		i = i + 1
	end
	i = 1
	while (gBFXR_PNames[i]) do
		if (this[gBFXR_PNames[i]]) then
			IFObj_fnSetVis(this[gBFXR_PNames[i]], p)
		end
		i = i + 1
	end
	local any = g or p
	if (this.bfxrBack) then
		IFObj_fnSetVis(this.bfxrBack, any)
	end
	if (this.Helptext_Back) then
		IFObj_fnSetVis(this.Helptext_Back, not any)
	end
	if (this.bfxrPList) then
		IFObj_fnSetVis(this.bfxrPList, nil)
	end
	gBFXR_ListOpen[this.bfxrPlayer] = nil
end

function bfxr_SelectItem(this, item)
	if (this.bfxrHi) then
		IFButton_fnHilight(this.bfxrHi, nil, 0)
		IFButton_fnSelect(this.bfxrHi, nil)
	end
	this.bfxrHi = item
	if (item) then
		IFButton_fnSelect(item, 1)
		bfxr_Hilite(this, 0)
	end
end

function bfxr_PageItem(this)
	local i = this.bfxrPageIdx or 1
	if (this.bfxrMode == "gpage") then
		return this.bfxrGItems[gBFXR_GItems[i]]
	end
	if (gBFXR_PItems[i] == "reset") then
		return this.bfxrPReset.btn
	end
	return this.bfxrPVals[gBFXR_PItems[i]]
end

function bfxr_GameValues(this)
	local it = this.bfxrGItems
	IFText_fnSetString(it.persp.label, "ifs.GameOpt.cam_forward")
	if (bfxr_DiffOk()) then
		local d = ScriptCB_GetDifficulty()
		if (d == 1) then
			IFText_fnSetString(it.diff.label, "ifs.difficulty.easy")
		elseif (d == 2) then
			IFText_fnSetString(it.diff.label, "ifs.difficulty.medium")
		else
			IFText_fnSetString(it.diff.label, "ifs.difficulty.hard")
		end
	else
		bfxr_Text(it.diff.label, "-")
	end
	IFText_fnSetString(it.reset.label, "common.reset")
	-- 21i-r10: his AUTO AIM (the title in English if the game has no text for it)
	if (it.autoaim) then
		local v = bfxr_Ask(format("aimp get %d", this.bfxrPlayer or 3))
		if (v < 0) then
			bfxr_Text(it.autoaim.label, "-")
		elseif (v == 1) then
			IFText_fnSetString(it.autoaim.label, "common.on")
		else
			IFText_fnSetString(it.autoaim.label, "common.off")
		end
		if (this.bfxrGLabels and this.bfxrGLabels.autoaim and not (ScriptCB_getlocalizestr and ScriptCB_getlocalizestr("ifs.gameopt.autoaim", 1))) then
			bfxr_Text(this.bfxrGLabels.autoaim, "AUTO AIM")
		end
	end
end

-- the button k's function on tab m from his own map (the page's number, -1 unknown), and the label's string index (as player
-- 1's page: the d-pad's four squad directions are one SQUAD COMMANDS, the sticks Move / Strafe)
function bfxr_PadFn(this, k, v, m)
	if ((v.name == "lang") or (v.name == "rang")) then
		return 16, 16 + 19 * m
	end
	local fn = bfxr_Ask(format("rp ctlfn %d %d %d", this.bfxrPlayer, m, k))
	local idx = fn
	if ((idx > 10) and (idx < 15)) then
		idx = 10
	end
	if (idx > 0) then
		idx = idx + 19 * m
	end
	return fn, idx
end

function bfxr_FnString(idx)
	local str = (idx > 0) and gButtonFunctionStringsGamePad and gButtonFunctionStringsGamePad[idx]
	if (type(str) == "string") then
		return str
	end
	return "ifs.controls.General.none"
end

-- the buttons he can change (player 1's page's: LB LT X A B Y RB RT R3, L3 when it is not BF1GameExt's sprint) - the sticks,
-- BACK / START and the d-pad stay as they are
function bfxr_PickList()
	local t = { 1, 2, 11, 12, 13, 14, 15, 16, 19, }
	if (not gBFX_L3Sprint) then
		tinsert(t, 18)
	end
	return t
end

function bfxr_PadValues(this)
	local m = this.bfxrTab or 0
	local k = this.bfxrPlayer
	IFText_fnSetString(this.bfxrPTitle, gBFXR_TabTitle[m])
	local i = 0
	while (i <= 2) do
		local a = 0.6 -- 21h-r7: (0.4 was nearly see-through; his are a darker orange)
		if (i == m) then
			a = 1
		end
		IFObj_fnSetAlpha(this["bfxrPTab" .. i], a)
		i = i + 1
	end
	local key, v
	for key, v in Platform_btn_map do
		local ln = this.bfxrPLines[key]
		if (ln and ln.label) then
			local show = 1
			-- as player 1's page: BACK = MAP, START = PAUSE MENU (fixed), L3 = SPRINT on the infantry tab (BF1GameExt's L3
			-- sprint) and nothing on the others; the d-pad's squad commands one line (the left one's), as his page has it
			if (key == 8) then
				bfxr_Text(ln.label, "MAP")
			elseif (key == 9) then
				bfxr_Text(ln.label, "PAUSE MENU")
			elseif ((key == 18) and gBFX_L3Sprint and (m == 0)) then
				bfxr_Text(ln.label, "SPRINT")
			elseif ((key == 18) and gBFX_L3Sprint) then
				IFText_fnSetString(ln.label, "ifs.controls.General.none")
			else
				local fn, idx = bfxr_PadFn(this, key, v, m)
				IFText_fnSetString(ln.label, bfxr_FnString(idx))
				if (((key == 3) or (key == 5) or (key == 6)) and (idx == 10 + 19 * m)) then
					show = nil
				end
			end
			if (gBFXR_ListOpen[k] and (key ~= this.bfxrPadKey)) then
				show = nil
			end
			IFObj_fnSetVis(ln, show)
			if (IFObj_fnSetColor) then
				if (this.bfxrPadKey and (key == this.bfxrPadKey)) then
					IFObj_fnSetColor(ln.label, 255, 170, 0)
				else
					IFObj_fnSetColor(ln.label, 255, 255, 255)
				end
			end
		end
	end
	local sens = bfxr_Ask(format("rp ctlget %d 2", k))
	if (sens < 1) then
		sens = 5
	end
	-- shown 10 - 100 as player 1's page shows his (a percent); the thumb from the left end (1) to the right end (10)
	bfxr_Text(this.bfxrPVals.joysens.label, format("%d", sens * 10))
	local f = this.bfxrPageScale or 0.51
	local l = -115.2 + (sens - 1) / 9 * 207.36
	IFImage_fnSetTexturePos(this.bfxrPSlider.fg, l * f, -12 * f, (l + 23.04) * f, 12 * f)
	IFText_fnSetString(this.bfxrPVals.rumble.label, "common.no")
	IFObj_fnSetAlpha(this.bfxrPVals.rumble, 0.4)
	IFObj_fnSetAlpha(this.bfxrPLabels.rumble, 0.4)
	local inv = 0
	if (m == 2) then
		bfxr_Text(this.bfxrPLabels.invert, "INVERT FLYING")
		inv = bfxr_Ask(format("rp ctlget %d 1", k))
	else
		bfxr_Text(this.bfxrPLabels.invert, "INVERT LOOK")
		inv = bfxr_Ask(format("rp ctlget %d 0", k))
	end
	if (inv == 1) then
		IFText_fnSetString(this.bfxrPVals.invert.label, "common.yes")
	else
		IFText_fnSetString(this.bfxrPVals.invert.label, "common.no")
	end
	IFText_fnSetString(this.bfxrPReset.btn.label, "common.reset")
end

-- his page up (mode "gpage" / "ppage"): the pause list hidden, the page shown, its first item picked (21h-r6: the gamepad
-- page starts on the pad - LB's label lit, as player 1's - when his buttons can be changed)
function bfxr_ShowPage(this, mode, idx)
	this.bfxrMode = mode
	bfxr_PopVis(this, nil)
	IFObj_fnSetVis(this.buttons, nil)
	this.CurButton = nil
	this.bfxrPageIdx = idx or 1
	this.bfxrPadKey = nil
	if (mode == "gpage") then
		bfxr_PagesVis(this, 1, nil)
		bfxr_GameValues(this)
		bfxr_SelectItem(this, bfxr_PageItem(this))
		return
	end
	bfxr_PagesVis(this, nil, 1)
	this.bfxrRemap = nil
	if (bfxr_Ask(format("rp remap %d", this.bfxrPlayer)) == 1) then
		this.bfxrRemap = 1
		this.bfxrPadKey = 2
		bfxr_SelectItem(this, nil)
	else
		bfxr_SelectItem(this, bfxr_PageItem(this))
	end
	bfxr_PadValues(this)
end

-- the nearest changeable label in a direction (labels in two columns left / right of the pad, at Platform_btn_map's lx1 /
-- ly1 - player 1's page's way); 1 if it moved
function bfxr_PadMove(this, dir)
	local cur = Platform_btn_map[this.bfxrPadKey]
	local cx = cur.lx1
	local cy = cur.ly1
	local best = nil
	local bestScore = 0
	local pick = bfxr_PickList()
	local i
	for i = 1, getn(pick) do
		local k = pick[i]
		local v = Platform_btn_map[k]
		if ((k ~= this.bfxrPadKey) and v) then
			local dy = v.ly1 - cy
			local sameSide = ((v.lx1 < 0) == (cx < 0))
			local ok = nil
			local score = 0
			if (dir == "up") then
				if (sameSide and (dy < 0)) then ok = 1 end
				score = -dy
			elseif (dir == "down") then
				if (sameSide and (dy > 0)) then ok = 1 end
				score = dy
			elseif (dir == "left") then
				if ((v.lx1 < 0) and (cx > 0)) then ok = 1 end
				score = abs(dy)
			else
				if ((v.lx1 > 0) and (cx < 0)) then ok = 1 end
				score = abs(dy)
			end
			if (ok and ((not best) or (score < bestScore))) then
				best = k
				bestScore = score
			end
		end
	end
	if (best) then
		this.bfxrPadKey = best
		ScriptCB_SndPlaySound("shell_select_change")
		bfxr_PadValues(this)
		return 1
	end
	return nil
end

-- the actions his button can get on tab m (player 1's list: the game's contents_functype_*_button when it is there, else the
-- tab's button actions), as the page's function numbers
function bfxr_ListFns(m)
	local c = contents_functype_infantry_button
	if (m == 1) then
		c = contents_functype_vehicle_button
	elseif (m == 2) then
		c = contents_functype_flyer_button
	end
	local out = {}
	local seen = {}
	if (type(c) == "table") then
		local i
		for i = 1, getn(c) do
			local e = c[i]
			if ((type(e) == "table") and (type(e.showidx) == "number")) then
				local fn = e.showidx
				if (fn > 19) then
					fn = fn - 19 * m
				end
				if ((fn >= 1) and (fn <= 15) and ((fn < 11) or (fn > 14)) and (not seen[fn])) then
					seen[fn] = 1
					tinsert(out, fn)
				end
			end
		end
	end
	if (getn(out) == 0) then
		local all = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 15, }
		local i
		for i = 1, getn(all) do
			local str = bfxr_FnString(all[i] + 19 * m)
			if (not strfind(strlower(str), "none")) then
				tinsert(out, all[i])
			end
		end
	end
	if (not gBFXR_ListLogged[m]) then
		gBFXR_ListLogged[m] = 1
		local s = ""
		local i
		for i = 1, getn(out) do
			s = s .. " " .. out[i]
		end
		bfxr_Log(format("tab %d's action list (%s):%s", m, (type(c) == "table") and "the game's contents_functype list" or
			"the tab's button actions", s))
	end
	return out
end

-- the list's rows (5 shown, as player 1's): the action names, the picked one lit
function bfxr_ListShow(this)
	local k = this.bfxrPlayer
	local L = gBFXR_List[k]
	local m = this.bfxrTab or 0
	local n = getn(L.fns)
	if (L.sel < L.first) then
		L.first = L.sel
	elseif (L.sel > L.first + 4) then
		L.first = L.sel - 4
	end
	local i = 1
	while (i <= 5) do
		local row = this.bfxrPList["row" .. i]
		local j = L.first + i - 1
		if (j <= n) then
			IFText_fnSetString(row, bfxr_FnString(L.fns[j] + 19 * m))
			IFObj_fnSetVis(row, 1)
			if (IFObj_fnSetColor) then
				if (j == L.sel) then
					IFObj_fnSetColor(row, 255, 170, 0)
				else
					IFObj_fnSetColor(row, 255, 255, 255)
				end
			end
		else
			IFObj_fnSetVis(row, nil)
		end
		i = i + 1
	end
end

-- A on a pad label: the list of actions for it, beside it (player 1's: the other side of the pad), the other lines hidden
function bfxr_ListOpen(this)
	local k = this.bfxrPlayer
	local key = this.bfxrPadKey
	local m = this.bfxrTab or 0
	local fns = bfxr_ListFns(m)
	local cur = bfxr_PadFn(this, key, Platform_btn_map[key], m)
	local sel = 1
	local i
	for i = 1, getn(fns) do
		if (fns[i] == cur) then
			sel = i
		end
	end
	gBFXR_List[k] = { fns = fns, sel = sel, first = max(sel - 2, 1), key = key, }
	gBFXR_ListOpen[k] = 1
	local f = this.bfxrPageScale or 0.51
	if (IFObj_fnSetPos) then
		if (Platform_btn_map[key].lx1 < 0) then
			IFObj_fnSetPos(this.bfxrPList, 120 * f, 40 * f)
		else
			IFObj_fnSetPos(this.bfxrPList, -120 * f, 40 * f)
		end
	end
	IFObj_fnSetVis(this.bfxrPList, 1)
	bfxr_ListShow(this)
	bfxr_PadValues(this)
	ifelm_shellscreen_fnPlaySound(this.acceptSound)
end

function bfxr_ListClose(this)
	gBFXR_ListOpen[this.bfxrPlayer] = nil
	IFObj_fnSetVis(this.bfxrPList, nil)
	bfxr_PadValues(this)
end

function bfxr_ListMove(this, step)
	local L = gBFXR_List[this.bfxrPlayer]
	local n = getn(L.fns)
	local s = L.sel + step
	if ((s < 1) or (s > n)) then
		return
	end
	L.sel = s
	ScriptCB_SndPlaySound("shell_select_change")
	bfxr_ListShow(this)
end

-- A in the list: the action goes to his button (his own map, kept in BF1GameExt.ini)
function bfxr_ListPick(this)
	local k = this.bfxrPlayer
	local L = gBFXR_List[k]
	local m = this.bfxrTab or 0
	local fn = L.fns[L.sel]
	local r = bfxr_Ask(format("rp ctlfnset %d %d %d %d", k, m, L.key, fn or -1))
	bfxr_ListClose(this)
	if (r == 1) then
		ifelm_shellscreen_fnPlaySound(this.acceptSound)
		bfxr_Log(format("player %d: tab %d, %s -> %s (function %d)", k, m, Platform_btn_map[L.key].name, bfxr_FnString(fn + 19 * m), fn))
	else
		ifelm_shellscreen_fnPlaySound(this.errorSound)
		bfxr_Log(format("player %d: tab %d, %s - NOT changed (%d)", k, m, Platform_btn_map[L.key].name, r))
	end
end

function bfxr_PageMove(this, step)
	if (gBFXR_ListOpen[this.bfxrPlayer]) then
		bfxr_ListMove(this, step)
		return
	end
	local list = gBFXR_GItems
	if (this.bfxrMode == "ppage") then
		list = gBFXR_PItems
		-- 21h-r6: the pad's labels first, then the settings (player 1's page: down from the bottom label to them, up from the
		-- first back to the pad)
		if (this.bfxrPadKey) then
			local dir = "down"
			if (step < 0) then
				dir = "up"
			end
			if ((not bfxr_PadMove(this, dir)) and (step > 0)) then
				this.bfxrLastPad = this.bfxrPadKey
				this.bfxrPadKey = nil
				this.bfxrPageIdx = 1
				ScriptCB_SndPlaySound("shell_select_change")
				bfxr_PadValues(this)
				bfxr_SelectItem(this, bfxr_PageItem(this))
			end
			return
		end
		if (this.bfxrRemap and (((step < 0) and ((this.bfxrPageIdx or 1) == 1)) or ((step > 0) and ((this.bfxrPageIdx or 1) == getn(list))))) then
			this.bfxrPadKey = this.bfxrLastPad or 2
			ScriptCB_SndPlaySound("shell_select_change")
			bfxr_SelectItem(this, nil)
			bfxr_PadValues(this)
			return
		end
	end
	local n = getn(list)
	local i = (this.bfxrPageIdx or 1) + step
	if (i < 1) then
		i = n
	elseif (i > n) then
		i = 1
	end
	this.bfxrPageIdx = i
	ScriptCB_SndPlaySound("shell_select_change")
	bfxr_SelectItem(this, bfxr_PageItem(this))
end

-- RESET TO DEFAULTS on the gamepad page: the shown tab's buttons back to the game's (player 1's page's RESET), and his
-- sensitivity / invert back to the defaults
function bfxr_PadReset(this)
	local k = this.bfxrPlayer
	local m = this.bfxrTab or 0
	if (this.bfxrRemap) then
		bfxr_Ask(format("rp ctlfnreset %d %d", k, m))
	end
	bfxr_Ask(format("rp ctlset %d 0 0", k))
	bfxr_Ask(format("rp ctlset %d 1 1", k))
	bfxr_Ask(format("rp ctlset %d 2 5", k))
	ifelm_shellscreen_fnPlaySound(this.acceptSound)
	bfxr_Log(format("player %d: CONTROL OPTIONS - RESET TO DEFAULTS (tab %d's buttons, invert look OFF, invert flying ON, sensitivity 5)", k, m))
end

-- left / right (and A, step 1) on a page item
function bfxr_PageChange(this, step)
	local k = this.bfxrPlayer
	local i = this.bfxrPageIdx or 1
	if (this.bfxrMode == "gpage") then
		local tag = gBFXR_GItems[i]
		if (tag == "persp") then
			ifelm_shellscreen_fnPlaySound(this.errorSound)
		elseif (tag == "diff") then
			bfxr_DiffChange(this, step)
			bfxr_GameValues(this)
		elseif (tag == "autoaim") then
			-- 21i-r10: his own auto aim
			local v = bfxr_Ask(format("aimp get %d", k))
			if (v < 0) then
				ifelm_shellscreen_fnPlaySound(this.errorSound)
			else
				bfxr_Ask(format("aimp set %d %d", k, 1 - v))
				ifelm_shellscreen_fnPlaySound(this.acceptSound)
				bfxr_Log(format("player %d: GAME OPTIONS - AUTO AIM %s", k, (v == 0) and "ON" or "OFF"))
			end
			bfxr_GameValues(this)
		elseif (step == 1) then
			if (bfxr_DiffOk()) then
				ScriptCB_SetDifficulty(2)
				ifelm_shellscreen_fnPlaySound(this.acceptSound)
				bfxr_Log(format("player %d: GAME OPTIONS - RESET TO DEFAULTS (difficulty MEDIUM)", k))
			else
				ifelm_shellscreen_fnPlaySound(this.errorSound)
			end
			bfxr_GameValues(this)
		end
		bfxr_Hilite(this, 0)
		return
	end
	local tag = gBFXR_PItems[i]
	if (tag == "joysens") then
		local v = bfxr_Ask(format("rp ctlget %d 2", k))
		if (v < 1) then
			v = 5
		end
		v = v + step
		if (v < 1) then
			v = 1
		elseif (v > 10) then
			v = 10
		end
		bfxr_Ask(format("rp ctlset %d 2 %d", k, v))
		ScriptCB_SndPlaySound("shell_select_change")
	elseif (tag == "invert") then
		local which = 0
		if ((this.bfxrTab or 0) == 2) then
			which = 1
		end
		local v = 1
		if (bfxr_Ask(format("rp ctlget %d %d", k, which)) == 1) then
			v = 0
		end
		bfxr_Ask(format("rp ctlset %d %d %d", k, which, v))
		ScriptCB_SndPlaySound("shell_select_change")
	elseif (step == 1) then
		bfxr_PadReset(this)
	end
	bfxr_PadValues(this)
	bfxr_Hilite(this, 0)
end

-- LB / RB on the gamepad page: the tab
function bfxr_Tab(this, step)
	if (this.bfxrBusy or (this.bfxrMode ~= "ppage") or gBFXR_ListOpen[this.bfxrPlayer]) then
		return
	end
	local m = (this.bfxrTab or 0) + step
	if (m < 0) then
		m = 2
	elseif (m > 2) then
		m = 0
	end
	this.bfxrTab = m
	ScriptCB_SndPlaySound("shell_select_change")
	bfxr_PadValues(this)
	bfxr_Hilite(this, 0)
	if (not gBFXR_TabLogged) then
		gBFXR_TabLogged = 1
		bfxr_Log(format("player %d: the gamepad page's tab %d (LB / RB)", this.bfxrPlayer, m))
	end
end

function bfxr_TabLeft(this)
	bfxr_Tab(this, -1)
end

function bfxr_TabRight(this)
	bfxr_Tab(this, 1)
end

-- Y on the gamepad page: RESET TO DEFAULTS (as player 1's page: the shown tab)
function bfxr_Misc2(this)
	if ((not this.bfxrBusy) and (this.bfxrMode == "ppage") and (not gBFXR_ListOpen[this.bfxrPlayer])) then
		bfxr_PadReset(this)
		bfxr_PadValues(this)
		bfxr_Hilite(this, 0)
	end
end

-- one screen: the same build as ifs_pausemenu2, on its own viewport (k = 3 / 4 -> viewport 2 / 3)
function bfxr_Make(k)
	local name = format("ifs_pausemenu%d", k)
	local t = NewIFShellScreen2 {
		nologo = 1,
		bfxrPlayer = k,
		acceptSound = "shell_menu_accept",
		exitSound = "shell_menu_exit",
		cancelSound = "shell_menu_cancel",
		errorSound = "shell_menu_error",
		buttons = NewIFContainer {
			ScreenRelativeX = 0.6,
			ScreenRelativeY = 0.5,
			y = 20,
			rotY = 25,
		},
		Enter = bfxr_Enter,
		Leave = bfxr_Leave,
		Update = bfxr_Update,
		Input_Accept = bfxr_Accept,
		Input_Back = bfxr_Back,
		Input_Start = bfxr_Start,
		Input_GeneralUp = bfxr_Up,
		Input_GeneralDown = bfxr_Down,
		Input_GeneralLeft = bfxr_Left,
		Input_GeneralRight = bfxr_Right,
		Input_LTrigger2 = bfxr_TabLeft,
		Input_RTrigger2 = bfxr_TabRight,
		Input_Misc2 = bfxr_Misc2,
	}
	t.bfxrPop = bfxr_NewPop()
	-- 21h-r5: player 1's GAME OPTIONS and gamepad pages, copied (gBFXR_Pages, [Splitscreen] OptionsPagesP34)
	local f = (gBFXR_PopScale or 51) / 100
	if ((f < 0.2) or (f > 1.5)) then
		f = 0.51
	end
	local pages = nil
	if (((gBFXR_Pages or 1) == 1) and bfxr_PagesOk()) then
		bfxr_NewGamePage(t, f)
		pages = bfxr_NewPadPage(t, f)
		if (not pages) then
			t.bfxrGTitle = nil
			t.bfxrGLabels = nil
			t.bfxrGItems = nil
		end
	end
	t.bfxrPageScale = f
	t.CurButton = AddVerticalButtons(t.buttons, bfxr_layout)
	t.Viewport = k - 1
	setglobal(name, t)
	AddIFScreen(t, name)
	if (t.bfxrPop) then
		IFObj_fnSetVis(t.bfxrPop, nil)
	end
	if (pages) then
		bfxr_PagesVis(t, nil, nil)
	end
	bfxr_Log(format("%s made: viewport %d, %s, popup window %s (scale %.2f), player 1's GAME OPTIONS / gamepad pages %s", name,
		t.Viewport, tostring(t.ScreenName), t.bfxrPop and "made (player 1's look)" or "NOT made (YES / NO in the list)",
		(t.bfxrPop and t.bfxrPop.bfxrScale) or 0, pages and "made" or ((gBFXR_Pages == 0) and "off (OptionsPagesP34=0)" or
		("NOT made (r4's lists; missing:" .. bfxr_PagesMissing() .. ")"))))
end

function bfxr_MakeAll()
	local n = gBFXR_Players or 2
	if (n >= 3) then
		bfxr_Make(3)
	end
	if (n >= 4) then
		bfxr_Make(4)
	end
end

-- 21h-r3: player 2's pause menu gets player 1's sounds (the game's ifs_pausemenu2 has none: its template has no sound fields)
function bfxr_P2Sounds()
	if ((gBFXR_P2Sounds ~= 1) or (not ifs_pausemenu2)) then
		return
	end
	local s = ifs_pausemenu2
	s.acceptSound = s.acceptSound or "shell_menu_accept"
	s.exitSound = s.exitSound or "shell_menu_exit"
	s.cancelSound = s.cancelSound or "shell_menu_cancel"
	s.errorSound = s.errorSound or "shell_menu_error"
	if (not gBFXR_P2SoundsLogged) then
		gBFXR_P2SoundsLogged = 1
		bfxr_Log("ifs_pausemenu2: player 1's accept / exit / cancel / error sounds set")
	end
end

bfxr_P2Sounds()

-- 21h-r4: player 2's OPTIONS (the game's ifs_opt_contmain while player 2's pause is the one on - the pausing viewport 1, in a
-- match): no VIDEO OPTIONS, and CONTROL OPTIONS goes straight to the gamepad page (ifs_opt_controller), never the keyboard
-- page. Player 1's (pausing viewport 0) as the game has it. Wrapped once per Lua state.
function bfxr_IsP2Opts()
	if (ScriptCB_GetShellActive and ScriptCB_GetShellActive()) then
		return nil
	end
	return ScriptCB_GetPausingViewport() == 1
end

function bfxr_P2OptsEnter(this, bFwd)
	gBFXR_OrigOptsEnter(this, bFwd)
	if (bfxr_IsP2Opts() and this.buttons and this.buttons.pcvideo and ifs_opt_top_vbutton_layout) then
		this.buttons.pcvideo.hidden = 1
		local cur = this.CurButton
		local first = ShowHideVerticalButtons(this.buttons, ifs_opt_top_vbutton_layout)
		if ((not cur) or (not this.buttons[cur]) or this.buttons[cur].hidden) then
			cur = first
		end
		this.CurButton = cur
		SetCurButton(this.CurButton)
		if (not gBFXR_P2OptsLogged) then
			gBFXR_P2OptsLogged = 1
			bfxr_Log(format("player 2's OPTIONS: VIDEO OPTIONS hidden (first item %s)", tostring(this.CurButton)))
		end
	end
end

function bfxr_P2OptsAccept(this)
	if (bfxr_IsP2Opts() and (this.CurButton == "pccontrols")) then
		ifelm_shellscreen_fnPlaySound(this.acceptSound)
		bfxr_Log("player 2's CONTROL OPTIONS: straight to the gamepad page (ifs_opt_controller)")
		ScriptCB_PushScreen("ifs_opt_controller")
		return
	end
	gBFXR_OrigOptsAccept(this)
end

function bfxr_P2Opts()
	if (gBFXR_P2Opts ~= 1) then
		return
	end
	if (gBFXR_P2OptsWrapped) then
		return
	end
	if ((not ifs_opt_contmain) or (not ifs_opt_contmain.Enter) or (not ifs_opt_contmain.Input_Accept)) then
		bfxr_Log("player 2's OPTIONS: ifs_opt_contmain is not made yet - not changed")
		return
	end
	gBFXR_P2OptsWrapped = 1
	gBFXR_OrigOptsEnter = ifs_opt_contmain.Enter
	gBFXR_OrigOptsAccept = ifs_opt_contmain.Input_Accept
	ifs_opt_contmain.Enter = bfxr_P2OptsEnter
	ifs_opt_contmain.Input_Accept = bfxr_P2OptsAccept
	bfxr_Log("player 2's OPTIONS: gamepad only (no VIDEO OPTIONS; CONTROL OPTIONS -> the gamepad page)")
end

bfxr_P2Opts()

-- made right after the game has finished ifs_pausemenu2 (its ScriptCB_EndIFScreen), once per Lua state
if ((not gBFXR_EndWrapped) and ((gBFXR_Players or 2) >= 3)) then
	gBFXR_EndWrapped = 1
	gBFXR_OrigEnd = ScriptCB_EndIFScreen
	ScriptCB_EndIFScreen = function(name)
		gBFXR_OrigEnd(name)
		if ((name == "ifs_pausemenu2") and (not gBFXR_Made)) then
			gBFXR_Made = 1
			ScriptCB_EndIFScreen = gBFXR_OrigEnd
			bfxr_P2Sounds()
			bfxr_P2Opts()
			bfxr_MakeAll()
		end
	end
	bfxr_Log(format("waiting for the game's ifs_pausemenu2 to be finished (%d players)", gBFXR_Players or 2))
end
