//========= Copyright (c) 2022 Zombie Panic! Team, All rights reserved. ============//

#include "AchievementList.h"
 
#include <vgui/VGUI2.h>
#include <vgui_controls/PropertyDialog.h>

#include <vgui/IVGui.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/Controls.h>
#include <vgui_controls/RichText.h>

// ===================================
// Achievement Dialog
// ===================================

class C_AchievementDialog : public vgui2::PropertyDialog
{
public:
	DECLARE_CLASS_SIMPLE(C_AchievementDialog, vgui2::PropertyDialog);

	C_AchievementDialog(vgui2::Panel *pParent);

protected:
	virtual void OnTick();
	virtual void OnCommand(const char* pcCommand);
	virtual void LoadAchievements();

private:

	vgui2::ComboBox *ui_AchvList;
	vgui2::CheckButton *ui_AchvTaken;
	vgui2::AchievementList *ui_AchvPList;
	vgui2::ImagePanel *ui_TotalProgress;
	vgui2::Label *ui_CurrentCompleted;

	int iAchievement;
	int CurrentCategory;

	bool HideAchieved;
	int miTotalAchievements;
	int miCompletedAchievements;
	int miProgressBar;
};

// ===================================
// Setup
// ===================================

#define _ACH_ID( id, category, type, maxvalue ) { id, #id, 0, 0, category, #type, maxvalue }

struct DialogAchievement_t
{
	int m_eAchievementID;
	const char *m_pchAchievementID;
	bool m_bAchieved;
	int m_iIconImage;
	int m_eCategory;
	const char *m_cStatName;
	int tmp_mvalue;
	int tmp_ivalue;
};

DialogAchievement_t GetAchievementByID( int eAchievement );