-- BF1GameExt_ctrlpage.lua   (file build p21j-r30)
gBFX_Build_ctrlpage = "BF1GameExt_ctrlpage.lua (file build p21j-r30)" -- 21j-r30: the build as a string too: a compiled script (the mod tools'
-- ScriptMunge, console.lvl) keeps no comments, and BF1GameExt's log reads its build from this string there
-- 21i-r14: DEAD ZONE and LOOK CURVE rows under JOYSTICK SENSITIVITY / VIBRATION (BF1GameExt's pad settings - [Splitscreen]
-- PadMenu=1; asked and set through "BFXQ: padf ...", saved in BF1GameExt.ini PadDeadZone / PadLookCurve, in effect at once).
-- The rows are copies of the 2020 page's own two (made before the game makes the screen); the three settings containers
-- move up a little (.85 -> .80 of the screen) so four rows clear RESET / BACK. Left / right (or A) change a row.
-- The 2020 controller page (ifs_opt_controller) made changeable, the way the console / Classic Collection page works.
-- BF1GameExt runs this (Lua 4.0, the game's own compiler) with two lines in front of it: gBFX_L3Sprint ([Sprint]
-- L3SprintOnly) and gBFX_Phase:
--   "build" - while the game makes the screen (ScriptCB_AddIFScreen("ifs_opt_controller"), when the common scripts
--             load): the RESET (Y) button is added to the screen table here, so the game makes it with the rest
--   "push"  - each time the game opens the screen (ScriptCB_PushScreen), right before its Enter (a fallback: the screen
--             is only changed once per Lua state, ifs_opt_controller.bfext)
-- r2: RESET TO DEFAULTS. bfx_ResetPadTab puts the shown tab's pad layout back to the game's default. It runs from the
-- page's own RESET (Y, or a click), and BF1GameExt also calls it right after the game's own ScriptCB_ResetControl -
-- the PC controls page's RESET (Y), which only resets the keyboard / mouse - so that one resets the pad too.
--
-- The 2020 script still has the console's remap code (the pick list, contents_functype_*, ifs_opt_controller_SetEntry
-- -> ScriptCB_SetFunctionIdForButtonId / ScriptCB_SetFunctionIdForAnalogId); Aspyr turned it off with
-- gNoControllerChanges = 1 and emptied the cursor functions. This puts the pad side back on top of the 2020 page:
--   - a pad label is picked with the d-pad / stick (it lights up orange); A opens the list of actions for it
--     (the list of that tab: infantry / vehicle / starfighter), A picks, B closes the list
--   - down from the bottom label goes to the 2020 settings (joystick sensitivity, rumble); up from them comes back
--   - LB / RB still change the tab; RESET (Y, or a click on it) puts the shown tab back to the game's default layout
--   - Back, Start and the d-pad (map, pause, squad commands) stay fixed, as on the Classic Collection
--   - L3 is BF1GameExt's sprint button while [Sprint] L3SprintOnly=1 (gBFX_L3Sprint): shown as SPRINT, not pickable
-- gNoControllerChanges stays 1: the 2020 handlers keep doing the settings; these wrappers do the pad.
-- bfx_Log lines appear in BF1GameExt_split.log as [ctrlpage] lines (BF1GameExt catches them).

-- Shared (defined on every run): the log channel and the pad reset.
function bfx_Log(s)
	ScriptCB_GetPlatform("BFX: " .. s)
end

-- The game's default layout (the same in all three modes): Lua button id -> function id; the sticks: left Move /
-- Strafe (0), right Free Look (2).
gBFX_DefaultFn = { [1] = 6, [2] = 4, [11] = 9, [12] = 2, [13] = 1, [14] = 7, [15] = 3, [16] = 5, [18] = 15, [19] = 8 }

-- The current player's pad layout of the current tab (control mode) back to the game's default.
function bfx_ResetPadTab(why)
	local mode = ScriptCB_GetControlMode()
	local id, fn
	for id, fn in gBFX_DefaultFn do
		ScriptCB_SetFunctionIdForButtonId(id, fn)
	end
	ScriptCB_SetFunctionIdForAnalogId(0, mode, 0)
	ScriptCB_SetFunctionIdForAnalogId(1, mode, 2)
	bfx_Log(format("pad layout of tab %d reset to the game's default (%s)", mode, why))
end

-- 21i-r14: BF1GameExt's pad settings (the DEAD ZONE / LOOK CURVE rows)
function bfx_PadAsk(s)
	return ScriptCB_GetPlatform("BFXQ: padf " .. s)
end

function bfx_CopyT(t)
	if (type(t) ~= "table") then
		return t
	end
	local c = {}
	local k, v
	for k, v in t do
		c[k] = bfx_CopyT(v)
	end
	return c
end

-- the choices: dead zone in percent (-1 = BF1's own square 24 %), look curve x 100 (100 = Battlefront II's straight line,
-- 332 = BF1's own)
gBFX_DzList = { -1, 0, 2, 4, 6, 8, 10, 12, 15, 20, 25 }
gBFX_CurveList = { 100, 150, 200, 250, 332 }

function bfx_PadNearest(list, v)
	local best = 1
	local i
	for i = 1, getn(list) do
		if (abs(list[i] - v) < abs(list[best] - v)) then
			best = i
		end
	end
	return best
end

function bfx_PadText(which, v)
	if (which == "dz") then
		if (v < 0) then
			return "BF1 (24% SQUARE)"
		end
		return format("%d%%", v)
	end
	if (v == 100) then
		return "1.0 (BF2)"
	end
	if (v == 332) then
		return "3.3 (BF1)"
	end
	return format("%d.%d", floor(v / 100), floor(mod(v, 100) / 10))
end

function bfx_PadShow(this)
	if (not this.bfxpadrows) then
		return
	end
	local s = this.extension_button_settings
	local v = bfx_PadAsk("get dz")
	if (v and s.bfxdz) then
		RoundIFButtonLabel_fnSetUString(s.bfxdz, ScriptCB_tounicode(bfx_PadText("dz", v)))
	end
	v = bfx_PadAsk("get curve")
	if (v and s.bfxcurve) then
		RoundIFButtonLabel_fnSetUString(s.bfxcurve, ScriptCB_tounicode(bfx_PadText("curve", v)))
	end
end

-- one step left (-1) / right (+1) on a row, round the list
function bfx_PadStep(this, which, d)
	local list = gBFX_DzList
	if (which == "curve") then
		list = gBFX_CurveList
	end
	local cur = bfx_PadAsk("get " .. which) or list[1]
	local i = bfx_PadNearest(list, cur) + d
	if (i < 1) then
		i = getn(list)
	elseif (i > getn(list)) then
		i = 1
	end
	bfx_PadAsk(format("set %s %d", which, list[i]))
	ifelm_shellscreen_fnPlaySound(this.selectSound)
	bfx_PadShow(this)
	bfx_Log(format("pad %s -> %s", which, bfx_PadText(which, list[i])))
end

function bfx_PadGo(this, tag)
	SetCurButtonTable(this.extension_button_settings[tag])
	ifelm_shellscreen_fnPlaySound(this.selectSound)
	bfx_Highlight(this)
end

-- While the game makes the screen: the two rows, copies of JOYSTICK SENSITIVITY's / VIBRATION's (not made yet)
if ((gBFX_Phase == "build") and ifs_opt_controller and ifs_opt_controller.extension_button_settings and
	ifs_opt_controller.exntension_labels and (not ifs_opt_controller.bfxpadrows) and (bfx_PadAsk("on") == 1)) then
	local t = ifs_opt_controller
	local s = t.extension_button_settings
	local l = t.exntension_labels
	if (s.joysens and s.rumble and l.joysens and l.rumble) then
		local dy = s.rumble.y - s.joysens.y
		local ldy = l.rumble.y - l.joysens.y
		s.bfxdz = bfx_CopyT(s.joysens)
		s.bfxdz.tag = "bfxdz"
		s.bfxdz.y = s.rumble.y + dy
		s.bfxcurve = bfx_CopyT(s.rumble)
		s.bfxcurve.tag = "bfxcurve"
		s.bfxcurve.y = s.rumble.y + dy + dy
		if (s.bfxdz.label) then
			s.bfxdz.label.string = ""
		end
		if (s.bfxcurve.label) then
			s.bfxcurve.label.string = ""
		end
		l.bfxdz = bfx_CopyT(l.joysens)
		l.bfxdz.y = l.rumble.y + ldy
		l.bfxdz.string = nil
		l.bfxdz.ustring = ScriptCB_tounicode("DEAD ZONE")
		l.bfxcurve = bfx_CopyT(l.rumble)
		l.bfxcurve.y = l.rumble.y + ldy + ldy
		l.bfxcurve.string = nil
		l.bfxcurve.ustring = ScriptCB_tounicode("LOOK CURVE")
		s.joysens.link_up = "bfxcurve"
		s.rumble.link_down = "bfxdz"
		s.bfxdz.link_up = "rumble"
		s.bfxdz.link_down = "bfxcurve"
		s.bfxcurve.link_up = "bfxdz"
		s.bfxcurve.link_down = "joysens"
		l.ScreenRelativeY = 0.8
		s.ScreenRelativeY = 0.8
		if (t.extension_sliders) then
			t.extension_sliders.ScreenRelativeY = 0.8
		end
		t.bfxpadrows = 1
		bfx_Log(format("DEAD ZONE / LOOK CURVE rows added under VIBRATION (rows %g apart, the settings moved up to .80)", dy))
	end
end

-- While the game makes the screen: the RESET (Y) button, as the PC controls page's (bottom middle).
if ((gBFX_Phase == "build") and ifs_opt_controller and (not ifs_opt_controller.bfxreset)) then
	ifs_opt_controller.bfxreset = CreateMiddleButton("common.reset", "Y", "reset")
	bfx_Log("RESET (Y) button added to ifs_opt_controller")
end

if (ifs_opt_controller and (not ifs_opt_controller.bfext)) then
ifs_opt_controller.bfext = 1

-- Platform_btn_map entries that can be changed: LB LT LS RS X A B Y RT RB R3 (L3 too when it is not sprint)
gBFX_Pick = { 1, 2, 7, 10, 11, 12, 13, 14, 15, 16, 19 }
if (not gBFX_L3Sprint) then
	gBFX_Pick[getn(gBFX_Pick) + 1] = 18
end

-- The 2020 handlers, called for everything that is not the pad
gBFX_Orig = {
	Enter = ifs_opt_controller.Enter,
	Input_Accept = ifs_opt_controller.Input_Accept,
	Input_Back = ifs_opt_controller.Input_Back,
	Input_GeneralUp = ifs_opt_controller.Input_GeneralUp,
	Input_GeneralDown = ifs_opt_controller.Input_GeneralDown,
	Input_GeneralLeft = ifs_opt_controller.Input_GeneralLeft,
	Input_GeneralRight = ifs_opt_controller.Input_GeneralRight,
	Input_LTrigger2 = ifs_opt_controller.Input_LTrigger2,
	Input_RTrigger2 = ifs_opt_controller.Input_RTrigger2,
	Update = ifs_opt_controller.Update,
}

-- The selected pad label lights up orange while the pad has the focus.
function bfx_Highlight(this)
	local lit = nil
	if (this.CurButton == "bfxpad") then
		lit = 1
	end
	local i
	for i = 1, getn(gBFX_Pick) do
		local k = gBFX_Pick[i]
		if (this.lines[k] and this.lines[k].label) then
			if (lit and (this.bfxPad == k)) then
				IFObj_fnSetColor(this.lines[k].label, 255, 170, 0)
			else
				IFObj_fnSetColor(this.lines[k].label, 255, 255, 255)
			end
		end
	end
	this.bfxLit = lit
end

-- Labels the 2020 code does not know about, then the highlight.
function bfx_Refresh(this)
	if (gBFX_L3Sprint and this.lines[18] and this.lines[18].label) then
		if (gControlMode == 0) then
			IFText_fnSetUString(this.lines[18].label, ScriptCB_tounicode("SPRINT"))
		else
			IFText_fnSetString(this.lines[18].label, "ifs.controls.General.none")
		end
	end
	bfx_Highlight(this)
end

function bfx_ToPad(this)
	SetCurButtonTable(nil)
	this.CurButton = "bfxpad"
	if (not this.bfxPad) then
		this.bfxPad = 2
	end
	ifelm_shellscreen_fnPlaySound(this.selectSound)
	bfx_Highlight(this)
end

function bfx_ToSettings(this)
	SetCurButtonTable(this.extension_button_settings.joysens)
	ifelm_shellscreen_fnPlaySound(this.selectSound)
	bfx_Highlight(this)
end

-- Moves the pad selection to the nearest label in a direction (labels stand in two columns, left and right of the
-- pad, at Platform_btn_map's lx1 / ly1). Returns 1 if it moved.
function bfx_Move(this, dir)
	local cur = Platform_btn_map[this.bfxPad]
	local cx = cur.lx1
	local cy = cur.ly1
	local best = nil
	local bestScore = 0
	local i
	for i = 1, getn(gBFX_Pick) do
		local k = gBFX_Pick[i]
		if (k ~= this.bfxPad) then
			local v = Platform_btn_map[k]
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
		this.bfxPad = best
		ifelm_shellscreen_fnPlaySound(this.selectSound)
		bfx_Highlight(this)
		return 1
	end
	return nil
end

-- A on a pad label: the list of actions for it, on the shown tab.
function bfx_OpenList(this)
	local k = this.bfxPad
	local Button = Platform_btn_map[k]
	local CurFunc = Button.func
	if (Button.name == "lang") then
		CurFunc = 16 + ScriptCB_GetFunctionIdForAnalogId(0, gControlMode)
	elseif (Button.name == "rang") then
		CurFunc = 16 + ScriptCB_GetFunctionIdForAnalogId(1, gControlMode)
	end
	if (Button.type == 3) then
		if (gControlMode == 0) then
			ifs_controller_listbox_contents = contents_functype_infantry_stick
		elseif (gControlMode == 1) then
			ifs_controller_listbox_contents = contents_functype_vehicle_stick
		else
			ifs_controller_listbox_contents = contents_functype_flyer_stick
		end
	else
		if (gControlMode == 0) then
			ifs_controller_listbox_contents = contents_functype_infantry_button
		elseif (gControlMode == 1) then
			ifs_controller_listbox_contents = contents_functype_vehicle_button
		else
			ifs_controller_listbox_contents = contents_functype_flyer_button
		end
	end
	local aListIndex = 1
	local i
	for i = 1, getn(ifs_controller_listbox_contents) do
		if (ifs_controller_listbox_contents[i].showidx == CurFunc) then
			aListIndex = i
		end
	end
	ifs_controller_listbox_layout.SelectedIdx = aListIndex
	ifs_controller_listbox_layout.CursorIdx = aListIndex
	ifs_controller_listbox_layout.FirstShownIdx = max(aListIndex - floor(ifs_controller_listbox_layout.showcount * 0.5), 1)
	ListManager_fnFillContents(this.listbox, ifs_controller_listbox_contents, ifs_controller_listbox_layout)

	this.cont_menu_state = 1
	this.cont_element_idx = k
	ifs_opt_controller_SetLineVisibility(this, nil, this.lines[k])
	if (Button.lx1 < 0) then
		IFObj_fnSetPos(this.listbox, 120, 0)
	else
		IFObj_fnSetPos(this.listbox, -120, 0)
	end
	IFObj_fnSetVis(this.listbox, 1)
	ifelm_shellscreen_fnPlaySound(this.acceptSound)
	bfx_Log(format("list open: button %d (%s), mode %d, current %d", k, Button.name, gControlMode, CurFunc))
end

function bfx_CloseList(this)
	IFObj_fnSetVis(this.listbox, nil)
	this.cont_menu_state = 0
	ifs_opt_controller_SetLineVisibility(this, 1)
	bfx_Refresh(this)
end

-- A in the list: the picked action goes to the button (the 2020 script's own SetEntry, the console's code).
function bfx_Pick(this)
	local Selection = ifs_controller_listbox_contents[ifs_controller_listbox_layout.SelectedIdx]
	local k = this.cont_element_idx
	bfx_CloseList(this)
	if (Selection) then
		ifs_opt_controller_SetEntry(this, k, Selection.showidx)
		bfx_Log(format("picked: button %d (%s), mode %d -> %d", k, Platform_btn_map[k].name, gControlMode, Selection.showidx))
	end
	ifelm_shellscreen_fnPlaySound(this.acceptSound)
	bfx_Refresh(this)
end

-- RESET (Y, or a click on it): the shown tab back to the game's default layout.
function bfx_ResetTab(this)
	bfx_ResetPadTab("the controller page's RESET")
	ifs_opt_controller_GetAllEntries(this)
	ifelm_shellscreen_fnPlaySound(this.acceptSound)
	bfx_Refresh(this)
end

ifs_opt_controller.Enter = function(this, bFwd)
	this.bfxMode0 = ScriptCB_GetControlMode()
	gBFX_Orig.Enter(this, bFwd)
	this.cont_menu_state = 0
	this.bfxPad = 2
	bfx_ToPad(this)
	bfx_Refresh(this)
	bfx_PadShow(this)
	bfx_Log(format("page up: control mode %d (put back when it closes)", this.bfxMode0))
end

ifs_opt_controller.Input_Accept = function(this)
	if (this.cont_menu_state == 1) then
		if (gMouseListBox) then
			gShellScreen_fnDefaultInputAccept(this)
		end
		bfx_Pick(this)
		return
	end
	if (this.CurButton == "bfxpad") then
		bfx_OpenList(this)
		return
	end
	if (this.CurButton == "reset") then
		bfx_ResetTab(this)
		return
	end
	if (this.CurButton == "bfxdz") then
		bfx_PadStep(this, "dz", 1)
		return
	end
	if (this.CurButton == "bfxcurve") then
		bfx_PadStep(this, "curve", 1)
		return
	end
	gBFX_Orig.Input_Accept(this)
	bfx_Refresh(this)
end

ifs_opt_controller.Input_Back = function(this)
	if (this.cont_menu_state == 1) then
		bfx_CloseList(this)
		ifelm_shellscreen_fnPlaySound(this.cancelSound)
		return
	end
	if (this.bfxMode0) then
		ScriptCB_SetControlMode(this.bfxMode0)
	end
	gBFX_Orig.Input_Back(this)
end

ifs_opt_controller.Input_GeneralUp = function(this)
	if (this.cont_menu_state == 1) then
		ListManager_fnNavUp(this.listbox, ifs_controller_listbox_contents, ifs_controller_listbox_layout)
		return
	end
	if (this.CurButton == "bfxpad") then
		bfx_Move(this, "up")
		return
	end
	if (this.CurButton == "joysens") then
		bfx_ToPad(this)
		return
	end
	if (this.CurButton == "bfxcurve") then
		bfx_PadGo(this, "bfxdz")
		return
	end
	if (this.CurButton == "bfxdz") then
		bfx_PadGo(this, "rumble")
		return
	end
	gBFX_Orig.Input_GeneralUp(this)
end

ifs_opt_controller.Input_GeneralDown = function(this)
	if (this.cont_menu_state == 1) then
		ListManager_fnNavDown(this.listbox, ifs_controller_listbox_contents, ifs_controller_listbox_layout)
		return
	end
	if (this.CurButton == "bfxpad") then
		if (not bfx_Move(this, "down")) then
			bfx_ToSettings(this)
		end
		return
	end
	if (this.bfxpadrows and (this.CurButton == "rumble")) then
		bfx_PadGo(this, "bfxdz")
		return
	end
	if (this.CurButton == "bfxdz") then
		bfx_PadGo(this, "bfxcurve")
		return
	end
	if (this.CurButton == "bfxcurve") then
		bfx_PadGo(this, "joysens")
		return
	end
	gBFX_Orig.Input_GeneralDown(this)
end

ifs_opt_controller.Input_GeneralLeft = function(this)
	if (this.cont_menu_state == 1) then
		return
	end
	if (this.CurButton == "bfxpad") then
		bfx_Move(this, "left")
		return
	end
	if (this.CurButton == "bfxdz") then
		bfx_PadStep(this, "dz", -1)
		return
	end
	if (this.CurButton == "bfxcurve") then
		bfx_PadStep(this, "curve", -1)
		return
	end
	gBFX_Orig.Input_GeneralLeft(this)
end

ifs_opt_controller.Input_GeneralRight = function(this)
	if (this.cont_menu_state == 1) then
		return
	end
	if (this.CurButton == "bfxpad") then
		bfx_Move(this, "right")
		return
	end
	if (this.CurButton == "bfxdz") then
		bfx_PadStep(this, "dz", 1)
		return
	end
	if (this.CurButton == "bfxcurve") then
		bfx_PadStep(this, "curve", 1)
		return
	end
	gBFX_Orig.Input_GeneralRight(this)
end

ifs_opt_controller.Input_LTrigger2 = function(this)
	if (this.cont_menu_state == 1) then
		return
	end
	gBFX_Orig.Input_LTrigger2(this)
	bfx_Refresh(this)
end

ifs_opt_controller.Input_RTrigger2 = function(this)
	if (this.cont_menu_state == 1) then
		return
	end
	gBFX_Orig.Input_RTrigger2(this)
	bfx_Refresh(this)
end

ifs_opt_controller.Input_Misc2 = function(this)
	if (this.cont_menu_state == 1) then
		return
	end
	bfx_ResetTab(this)
end

ifs_opt_controller.Update = function(this, fDt)
	gBFX_Orig.Update(this, fDt)
	-- the mouse can move the focus to the 2020 settings: the pad label goes dark again
	local lit = nil
	if (this.CurButton == "bfxpad") then
		lit = 1
	end
	if (lit ~= this.bfxLit) then
		bfx_Highlight(this)
	end
end

bfx_Log(format("installed on ifs_opt_controller (%s)", gBFX_Phase or "?"))
end
