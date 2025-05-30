#ifndef VGUI_TEAM_MENU_H
#define VGUI_TEAM_MENU_H
#include <vgui_controls/Frame.h>
#include "IViewportPanel.h"

namespace vgui2
{
class Button;
class RichText;
}

class CTeamMenu : public vgui2::Frame, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CTeamMenu, Frame);

	CTeamMenu();
	virtual ~CTeamMenu();

	virtual void OnCommand(const char *pCommand) override;
	virtual void OnKeyCodeTyped(vgui2::KeyCode code) override;

	void Update();
	void Activate();

	virtual void Paint();

	//IViewportPanel overrides
	virtual const char *GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual vgui2::VPANEL GetVPanel() override;
	virtual bool IsVisible() override;
	virtual void SetParent(vgui2::VPANEL parent) override;
	virtual void ReloadLayout() override;
	virtual void SetCurrentRound();

private:
	vgui2::CheckButton *m_pVolunteerForZombie = nullptr;
	vgui2::Button *m_pJoinGameButton = nullptr;
	vgui2::Button *m_pSpectateButton = nullptr;
	vgui2::RichText *m_pBriefingText = nullptr;
	vgui2::RichText *m_pGameModeInfo = nullptr;
	vgui2::Label *m_pLabelCurrentMapValue = nullptr;
	vgui2::Label *m_pLabelRounds = nullptr;
	vgui2::Label *m_pLabelGameModeName = nullptr;
	bool m_bUpdatedMapName = false;
	bool m_bUpdatedGameModeInfo = false;
	int m_iCurrentRounds = -1;
	bool m_bHasVolunteered = false;

	CPanelAnimationVarAliasType(int, m_iBtnX, "btn_x", "24", "proportional_int");
	CPanelAnimationVarAliasType(int, m_iBtnY, "btn_y", "28", "proportional_int");
	CPanelAnimationVarAliasType(int, m_iBtnSpacing, "btn_spacing", "8", "proportional_int");
};

#endif
