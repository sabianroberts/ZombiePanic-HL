#include <IEngineVGui.h>
#include <vgui_controls/PropertySheet.h>
#include "adv_options_dialog.h"
#include "client_vgui.h"
#include "gameui/gameui_viewport.h"
#include "IBaseUI.h"
#include "hud.h"
#include "cl_util.h"

#include "options_hud_root.h"
#include "options_models.h"
#include "options_chat.h"
#include "options_crosshair.h"
#include "options_scoreboard.h"
#include "options_general.h"
#include "options_about.h"

CON_COMMAND(gameui_cl_open_adv_options, "Opens Advanced Options dialog")
{
	// Since this command is called from game menu using "engine gameui_cl_open_adv_options"
	// GameUI will hide itself and show the game.
	// We need to show it again and after that activate CAdvOptionsDialog
	// Otherwise it may be hidden by the dev console
	gHUD.CallOnNextFrame([]() {
		CGameUIViewport::Get()->GetOptionsDialog()->Activate();
	});
	g_pBaseUI->ActivateGameUI();
}

CAdvOptionsDialog::CAdvOptionsDialog(vgui2::Panel *pParent)
    : BaseClass(pParent, "AdvOptionsDialog")
{
	switch ( gHUD.m_iRes )
	{
		default:
		case 320:
		case 640:
		case 1280:
		    SetBounds(0, 0, 512, 406);
		break;
		case 2560:
			SetBounds(0, 0, 1124, 812);
		break;
	}

	LoadControlSettings(VGUI2_ROOT_DIR "resource/options/AdvancedOptions.res");

	SetSizeable(false);
	SetDeleteSelfOnClose(true);

	SetTitle("#ZP_Options", true);

	AddPage(new CGeneralSubOptions(this), "#ZP_AdvOptions_General");
	AddPage(new CHudSubOptionsRoot(this), "#ZP_AdvOptions_Misc"); // Was ZP_AdvOptions_HUD
	AddPage(new CChatSubOptions(this), "#ZP_AdvOptions_Chat");
	AddPage(new CScoreboardSubOptions(this), "#ZP_AdvOptions_Scores");
	AddPage(new CCrosshairSubOptions(this), "#ZP_AdvOptions_Cross");
	//AddPage(new CModelSubOptions(this), "#ZP_AdvOptions_Models");
	AddPage(new CAboutSubOptions(this), "#ZP_AdvOptions_About");

	SetApplyButtonVisible(true);
	EnableApplyButton(true);
	GetPropertySheet()->SetTabWidth(84);
	MoveToCenterOfScreen();
}

void CAdvOptionsDialog::OnCommand(const char *command)
{
	if (!stricmp(command, "Apply"))
	{
		BaseClass::OnCommand("Apply");
		EnableApplyButton(true); // Apply should always be enabled
	}
	else
		BaseClass::OnCommand(command);
}
