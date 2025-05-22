// ============== Copyright (c) 2025 Monochrome Games ============== \\

#ifndef HUD_ZOMBIELIVES_H
#define HUD_ZOMBIELIVES_H
#include <vgui_controls/Panel.h>
#include <vgui_controls/Label.h>
#include "../../hud/base.h"

class CHudZombieLives : public CHudElemBase<CHudZombieLives>, public vgui2::Panel
{
public:
	DECLARE_CLASS_SIMPLE(CHudZombieLives, vgui2::Panel);

	CHudZombieLives();

	virtual bool IsAllowedToDraw();
	virtual void Paint();
	virtual void ApplySchemeSettings(vgui2::IScheme *pScheme);

	int MsgFunc_ZombieLives(const char *pszName, int iSize, void *pbuf);

private:
	CPanelAnimationVarAliasType( int, m_iIconTexture, "IconTexture", "ui/gfx/hud/zombielives", "textureid" );
	CPanelAnimationVarAliasType( int, m_iIconWide, "IconWide", "20", "proportional_int" );
	CPanelAnimationVarAliasType( int, m_iIconTall, "IconTall", "20", "proportional_int" );
	CPanelAnimationVarAliasType( int, m_iTextXAdd, "TextXAdd", "5", "proportional_int" );
	CPanelAnimationVarAliasType( int, m_iTextSizeWide, "TextWide", "100", "proportional_int" );
	CPanelAnimationVarAliasType( int, m_iTextSizeTall, "TextTall", "20", "proportional_int" );

	vgui2::Label *m_pLives;
	Color m_clrIcon;
};

#endif
