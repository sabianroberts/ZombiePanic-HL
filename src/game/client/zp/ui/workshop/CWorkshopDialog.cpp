#include <IEngineVGui.h>
#include <vgui_controls/PropertySheet.h>
#include "CWorkshopDialog.h"
#include "client_vgui.h"
#include "gameui/gameui_viewport.h"
#include "IBaseUI.h"
#include "hud.h"
#include "cl_util.h"

#include "CWorkshopSubList.h"
#include "CWorkshopSubUploaded.h"
#include "CWorkshopSubUpload.h"

CON_COMMAND(gameui_workshop, "Opens Workshop dialog")
{
	// Since this command is called from game menu using "engine gameui_workshop"
	// GameUI will hide itself and show the game.
	// We need to show it again and after that activate CWorkshopDialog
	// Otherwise it may be hidden by the dev console
	gHUD.CallOnNextFrame([]() {
		CGameUIViewport::Get()->GetWorkshopDialog()->Activate();
	});
	g_pBaseUI->ActivateGameUI();
}

CWorkshopDialog::CWorkshopDialog(vgui2::Panel *pParent)
    : BaseClass(pParent, "WorkshopDialog")
{
	SetBounds(0, 0, 800, 600);
	SetSizeable(false);
	SetDeleteSelfOnClose(true);

	SetTitle("#ZP_Workshop", true);

	AddPage(new CWorkshopSubList(this), "#ZP_Workshop_Tab_Subscribed");
	AddPage(new CWorkshopSubUploaded(this), "#ZP_Workshop_Tab_Uploaded");
	AddPage(new CWorkshopSubUpload(this), "#ZP_Workshop_Tab_Upload");

	SetOKButtonVisible(false);
	SetApplyButtonVisible(false);
	EnableApplyButton(false);
	SetCancelButtonText("#PropertyDialog_Close");
	GetPropertySheet()->SetTabWidth(84);
	MoveToCenterOfScreen();
}

void CWorkshopDialog::OnCommand(const char *command)
{
	BaseClass::OnCommand(command);
}
