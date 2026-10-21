-- ---------------------------------------------------------------------------------------------------------------------------
-- MODS (ifs_bfx_mods) - v6.4 (Ben James): one row per mod, from BF1GameExt's registry ("BFXQ: mods rows" -> gBFXM_R: every
-- add-on dinput8_*.dll's BFX_ModOptions and BF1GameExt's own sprint / ragdolls). A mod with an ON / OFF of its own shows it
-- on its row (left / right switch it there); A opens the mod's own page (ifs_bfx_modpage: "BFXQ: mods page K" -> gBFXM_P,
-- every row ON / OFF or a choice, left / right / A turn it: "BFXQ: mods turn K I D"). ADDON FOLDER stays the last row.
-- ---------------------------------------------------------------------------------------------------------------------------
gBFXM_MaxRows = 12

function bfxm_RowTag(i)
	return "m" .. i
end

-- the vertical button layout a page was made with (plain tables are kept out of the screen tables)
function bfxm_LayoutOf(this)
	if (this == ifs_bfx_modpage) then
		return bfxm_PageLayout
	end
	return bfxm_Layout
end

function bfxm_OnOff(v)
	if (v == 1) then
		return "ON"
	elseif (v == 0) then
		return "OFF"
	end
	return "NOT AVAILABLE"
end

-- the MODS page's rows from BF1GameExt (gBFXM_R), the buttons' texts, the unused ones hidden
function bfxm_SetText(this)
	local b = this.buttons
	local n = bfxm_Ask("rows")
	if ((not n) or (type(gBFXM_R) ~= "table")) then
		gBFXM_R = {}
	end
	local i = 1
	while (i <= gBFXM_MaxRows) do
		local r = gBFXM_R[i]
		local bt = b[bfxm_RowTag(i)]
		if (bt) then
			if (r) then
				local s = r.name
				if (r.on) then
					s = s .. ": " .. (r.text or bfxm_OnOff(r.on))
				end
				RoundIFButtonLabel_fnSetUString(bt, ScriptCB_tounicode(s))
				bt.hidden = nil
			else
				bt.hidden = 1
			end
		end
		i = i + 1
	end
	local c = bfxm_Ask("count")
	local a = "ADDON FOLDER"
	if (c == 0) then
		a = "ADDON FOLDER: NO MODS"
	elseif (c == 1) then
		a = "ADDON FOLDER: 1 MOD"
	elseif (c and (c > 1)) then
		a = format("ADDON FOLDER: %d MODS", c)
	end
	RoundIFButtonLabel_fnSetUString(b.addons, ScriptCB_tounicode(a))
end

-- which mod the highlighted row is (nil: ADDON FOLDER or none)
function bfxm_RowMod(this)
	local c = this.CurButton
	if ((not c) or (strsub(c, 1, 1) ~= "m")) then
		return nil
	end
	local i = tonumber(strsub(c, 2))
	if ((not i) or (type(gBFXM_R) ~= "table")) then
		return nil
	end
	return gBFXM_R[i]
end

-- left / right on a mod's row: its own ON / OFF switched (nil: the row has none)
function bfxm_Toggle(this, dir, sound)
	local r = bfxm_RowMod(this)
	if ((not r) or (not r.on)) then
		return nil
	end
	if (r.on < 0) then
		ifelm_shellscreen_fnPlaySound(this.errorSound)
		bfxm_Log(format("MODS: %s is not available this session - nothing changed", r.name))
		return 1
	end
	ifelm_shellscreen_fnPlaySound(sound or this.acceptSound)
	local v = bfxm_Ask(format("turn %d %d %d", r.k, r.mi, dir))
	bfxm_Log(format("MODS (%s): %s %s -> %s", bfxm_Where(), r.name, bfxm_OnOff(r.on), tostring(v)))
	bfxm_SetText(this)
	bfxm_RelayoutKeep(this)
	return 1
end

-- the rows laid out again with the highlight kept
function bfxm_RelayoutKeep(this)
	local cur = this.CurButton
	local layout = bfxm_LayoutOf(this)
	local first = ShowHideVerticalButtons(this.buttons, layout)
	if ((not cur) or (not this.buttons[cur]) or this.buttons[cur].hidden) then
		cur = first
	end
	this.CurButton = cur
	SetCurButton(cur)
end

function bfxm_ModsEnter(this, bFwd)
	gIFShellScreenTemplate_fnEnter(this, bFwd)
	bfxm_SetText(this)
	local cur = this.CurButton
	local first = ShowHideVerticalButtons(this.buttons, bfxm_LayoutOf(this))
	if (bFwd or (not cur) or (not this.buttons[cur]) or this.buttons[cur].hidden) then
		cur = first
	end
	this.CurButton = cur
	SetCurButton(cur)
	if (bFwd) then
		local n = 0
		if (type(gBFXM_R) == "table") then
			n = getn(gBFXM_R)
		end
		bfxm_Log(format("MODS up (%s): %d mod(s) listed", bfxm_Where(), n))
	end
end

function bfxm_ModsAccept(this)
	if (gShellScreen_fnDefaultInputAccept(this)) then
		return
	end
	if (this.CurButton == "addons") then
		ifelm_shellscreen_fnPlaySound(this.acceptSound)
		ifs_movietrans_PushScreen(ifs_bfx_addons)
		return
	end
	local r = bfxm_RowMod(this)
	if (not r) then
		return
	end
	ifelm_shellscreen_fnPlaySound(this.acceptSound)
	gBFXM_Cur = r.k
	gBFXM_CurName = r.name
	ifs_movietrans_PushScreen(ifs_bfx_modpage)
end

-- ---------------------------------------------------------------------------------------------------------------------------
-- a mod's page (ifs_bfx_modpage): its rows from BF1GameExt (gBFXM_P)
-- ---------------------------------------------------------------------------------------------------------------------------
function bfxm_PageSetText(this)
	local b = this.buttons
	local n = bfxm_Ask(format("page %d", gBFXM_Cur or 0))
	if ((not n) or (type(gBFXM_P) ~= "table") or (type(gBFXM_P.rows) ~= "table")) then
		gBFXM_P = { name = gBFXM_CurName or "MOD", rows = {} }
	end
	local i = 1
	while (i <= gBFXM_MaxRows) do
		local r = gBFXM_P.rows[i]
		local bt = b[bfxm_RowTag(i)]
		if (bt) then
			if (r) then
				local s = r.label .. ": " .. r.text
				if ((r.restart == 1) and (r.avail == 1)) then
					s = s .. " *"
				end
				RoundIFButtonLabel_fnSetUString(bt, ScriptCB_tounicode(s))
				bt.hidden = nil
			else
				bt.hidden = 1
			end
		end
		i = i + 1
	end
	if (b._titlebar_) then
		IFText_fnSetUString(b._titlebar_, ScriptCB_tounicode(gBFXM_P.name or "MOD"))
	end
	local note = ""
	local j = 1
	while (gBFXM_P.rows[j]) do
		if (gBFXM_P.rows[j].restart == 1) then
			note = "* TAKES EFFECT AT THE NEXT GAME START"
		end
		j = j + 1
	end
	IFText_fnSetUString(this.bfxNote.note, ScriptCB_tounicode(note))
end

function bfxm_PageTurn(this, dir, sound)
	local c = this.CurButton
	if ((not c) or (strsub(c, 1, 1) ~= "m") or (type(gBFXM_P) ~= "table")) then
		return
	end
	local i = tonumber(strsub(c, 2))
	local r = gBFXM_P.rows[i]
	if (not r) then
		return
	end
	if (r.avail ~= 1) then
		ifelm_shellscreen_fnPlaySound(this.errorSound)
		bfxm_Log(format("MODS: %s / %s is not available this session - nothing changed", gBFXM_P.name, r.label))
		return
	end
	ifelm_shellscreen_fnPlaySound(sound or this.acceptSound)
	local v = bfxm_Ask(format("turn %d %d %d", gBFXM_P.k, i - 1, dir))
	bfxm_Log(format("MODS (%s): %s / %s: %s -> %s", bfxm_Where(), gBFXM_P.name, r.label, r.text, tostring(v)))
	bfxm_PageSetText(this)
	bfxm_RelayoutKeep(this)
end

function bfxm_PageEnter(this, bFwd)
	gIFShellScreenTemplate_fnEnter(this, bFwd)
	bfxm_PageSetText(this)
	local cur = this.CurButton
	local first = ShowHideVerticalButtons(this.buttons, bfxm_LayoutOf(this))
	if (bFwd or (not cur) or (not this.buttons[cur]) or this.buttons[cur].hidden) then
		cur = first
	end
	this.CurButton = cur
	SetCurButton(cur)
	if (bFwd) then
		bfxm_Log(format("MOD PAGE up (%s): %s, %d row(s)", bfxm_Where(), gBFXM_P.name or "?", getn(gBFXM_P.rows)))
	end
end

