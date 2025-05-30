// ============== Copyright (c) 2025 Monochrome Games ============== \\

#ifndef HUD_AMMOBANK_H
#define HUD_AMMOBANK_H
#include <vgui_controls/Panel.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/ImagePanel.h>
#include "../../hud/base.h"

class CHudAmmoBank : public CHudElemBase<CHudAmmoBank>, public vgui2::Panel
{
public:
	DECLARE_CLASS_SIMPLE( CHudAmmoBank, vgui2::Panel );

	CHudAmmoBank();

	void VidInit() override;

	virtual bool IsAllowedToDraw();
	virtual void PaintBackground();
	virtual void Paint();
	virtual void ApplySchemeSettings(vgui2::IScheme *pScheme);

	int AmmoDropToAmmoIndex( int index );
	std::string GetAmmoName( int index );

	float GetWeightPerBullet( int index, int amount );
	float GetCarry();
	float GetMaxCarry();

	void UpdateVisibility( bool state );

	int MsgFunc_AmmoBankUpdate(const char *pszName, int iSize, void *pbuf);

private:
	CPanelAnimationVar( int, m_pAmmoBank_width, "width", "250" );
	CPanelAnimationVar( int, m_pText_wide, "text_wide", "150" );
	CPanelAnimationVar( int, m_pText_wide_drop, "text_wide_drop", "200" );
	CPanelAnimationVar( int, m_pText_wide_ammo, "text_wide_ammo", "50" );
	CPanelAnimationVar( int, m_pText_tall, "text_tall", "24" );
	CPanelAnimationVar( int, m_pAmmoBank_up, "up", "35" );
	CPanelAnimationVar( int, m_pAmmoBank_left, "left", "25" );
	
	vgui2::Label			*m_pAmmoCount[4];
	vgui2::Label			*m_pAmmoName[4];
	vgui2::Label			*m_pWeightText;
	vgui2::Label			*m_pWeightStatus;

	Rect_t panelrect;
	vgui2::ImagePanel		*m_pBackground;

	int		m_iSelectedAmmoToDrop;
	bool	m_bHasPanelRect;
};

#endif
