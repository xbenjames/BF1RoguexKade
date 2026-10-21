	-- v6.4: the MODS page (one row per mod, ADDON FOLDER last) and a mod's page - both with gBFXM_MaxRows rows made once,
	-- the unused ones hidden
	local list = {}
	local i = 1
	while (i <= gBFXM_MaxRows) do
		list[i] = { tag = bfxm_RowTag(i) }
		i = i + 1
	end
	list[gBFXM_MaxRows + 1] = { tag = "addons" }
	bfxm_Layout = {
		ySpacing = 2,
		width = 520,
		font = "gamefont_medium",
		title = "ifs.Main.options",
		rotY = 40,
		buttonlist = list,
	}
	ifs_bfx_mods = NewIFShellScreen {
		nologo = 1,
		movieIntro = nil,
		movieBackground = nil,
		bNohelptext_backPC = 1,
		buttons = NewIFContainer {
			ScreenRelativeX = 0.6, -- where OPTIONS stands
			ScreenRelativeY = 0.5,
			y = 0,
		},
		Enter = function(this, bFwd)
			bfxm_ModsEnter(this, bFwd)
		end,
		Input_Accept = function(this)
			bfxm_ModsAccept(this)
		end,
		Input_Back = function(this)
			gBFXM_BackToOpts = 1
			ScriptCB_PopScreen()
		end,
		Input_GeneralUp = function(this)
			if (gShellScreen_fnDefaultInputUp(this)) then
				return
			end
			gDefault_Input_GeneralUp(this)
		end,
		Input_GeneralDown = function(this)
			if (gShellScreen_fnDefaultInputDown(this)) then
				return
			end
			gDefault_Input_GeneralDown(this)
		end,
		Input_GeneralLeft = function(this)
			bfxm_Toggle(this, -1, this.selectSound)
		end,
		Input_GeneralRight = function(this)
			bfxm_Toggle(this, 1, this.selectSound)
		end,
	}
	ifs_bfx_mods.CurButton = AddVerticalButtons(ifs_bfx_mods.buttons, bfxm_Layout)
	if (ifs_bfx_mods.buttons._titlebar_) then
		ifs_bfx_mods.buttons._titlebar_.string = nil
		ifs_bfx_mods.buttons._titlebar_.ustring = ScriptCB_tounicode("MODS")
	end
	ifs_bfx_mods.cancelbutton = CreateNegativeButton("common.back", "B", "_back")
	bfxm_SetText(ifs_bfx_mods)
	AddIFScreen(ifs_bfx_mods, "ifs_bfx_mods")

	-- a mod's own page
	local plist = {}
	i = 1
	while (i <= gBFXM_MaxRows) do
		plist[i] = { tag = bfxm_RowTag(i) }
		i = i + 1
	end
	bfxm_PageLayout = {
		ySpacing = 2,
		width = 560,
		font = "gamefont_medium",
		title = "ifs.Main.options",
		rotY = 40,
		buttonlist = plist,
	}
	ifs_bfx_modpage = NewIFShellScreen {
		nologo = 1,
		movieIntro = nil,
		movieBackground = nil,
		bNohelptext_backPC = 1,
		buttons = NewIFContainer {
			ScreenRelativeX = 0.6,
			ScreenRelativeY = 0.5,
			y = 0,
		},
		bfxNote = NewIFContainer {
			ScreenRelativeX = 0.5,
			ScreenRelativeY = 0.92,
			note = NewIFText {
				textw = 700,
				texth = 30,
				font = "gamefont_small",
				halign = "hcenter",
				valign = "vcenter",
				string = "",
				nocreatebackground = 1,
			},
		},
		Enter = function(this, bFwd)
			bfxm_PageEnter(this, bFwd)
		end,
		Input_Accept = function(this)
			if (gShellScreen_fnDefaultInputAccept(this)) then
				return
			end
			bfxm_PageTurn(this, 1, nil)
		end,
		Input_Back = function(this)
			ScriptCB_PopScreen()
		end,
		Input_GeneralUp = function(this)
			if (gShellScreen_fnDefaultInputUp(this)) then
				return
			end
			gDefault_Input_GeneralUp(this)
		end,
		Input_GeneralDown = function(this)
			if (gShellScreen_fnDefaultInputDown(this)) then
				return
			end
			gDefault_Input_GeneralDown(this)
		end,
		Input_GeneralLeft = function(this)
			bfxm_PageTurn(this, -1, this.selectSound)
		end,
		Input_GeneralRight = function(this)
			bfxm_PageTurn(this, 1, this.selectSound)
		end,
	}
	ifs_bfx_modpage.CurButton = AddVerticalButtons(ifs_bfx_modpage.buttons, bfxm_PageLayout)
	ifs_bfx_modpage.cancelbutton = CreateNegativeButton("common.back", "B", "_back")
	AddIFScreen(ifs_bfx_modpage, "ifs_bfx_modpage")
