#include <vgui_controls/Label.h>
#include <vgui_controls/ComboBox.h>
#include "client_vgui.h"
#include "cvar_color.h"
#include "options_hud_colors.h"
#include "tier1/KeyValues.h"

extern ConVar player_glow_style;

CHudSubOptionsColors::CHudSubOptionsColors(vgui2::Panel *parent)
    : BaseClass(parent, "HudSubOptionsColors")
{
	SetSize(100, 100); // Silence "parent not sized yet" warning

	m_pColorLabel[0] = new vgui2::Label(this, "ColorLabel", "#ZP_AdvOptions_HUD_Color");
	m_pColorValue[0] = new CCvarColor(this, "ColorValue", "hud_color", "#ZP_AdvOptions_HUD_Color_Title");

	m_pColorLabel[1] = new vgui2::Label(this, "Color1Label", "#ZP_AdvOptions_HUD_Color1");
	m_pColorValue[1] = new CCvarColor(this, "Color1Value", "hud_color1", "#ZP_AdvOptions_HUD_Color1_Title");

	m_pColorLabel[2] = new vgui2::Label(this, "Color2Label", "#ZP_AdvOptions_HUD_Color2");
	m_pColorValue[2] = new CCvarColor(this, "Color2Value", "hud_color2", "#ZP_AdvOptions_HUD_Color2_Title");

	m_pColorLabel[3] = new vgui2::Label(this, "Color3Label", "#ZP_AdvOptions_HUD_Color3");
	m_pColorValue[3] = new CCvarColor(this, "Color3Value", "hud_color3", "#ZP_AdvOptions_HUD_Color3_Title");

	m_pColorLabel[4] = new vgui2::Label(this, "Color4Label", "#ZP_AdvOptions_HUD_Color4");
	m_pColorValue[4] = new CCvarColor(this, "Color4Value", "player_glow1", "#ZP_AdvOptions_HUD_Color4_Title");

	m_pColorLabel[5] = new vgui2::Label(this, "Color5Label", "#ZP_AdvOptions_HUD_Color5");
	m_pColorValue[5] = new CCvarColor(this, "Color5Value", "player_glow2", "#ZP_AdvOptions_HUD_Color5_Title");

	m_pGlowDropdown = new vgui2::ComboBox(this, "GlowCombo", 4, false);

	LoadControlSettings(VGUI2_ROOT_DIR "resource/options/HudSubOptionsColors.res");

	m_pGlowDropdown->SetEditable(false);

	KeyValues *pKV = new KeyValues("item");
	m_pGlowDropdown->AddItem("#ZP_AdvOptions_HUD_GlowOption_Default", pKV);
	m_pGlowDropdown->AddItem("#ZP_AdvOptions_HUD_GlowOption_Swapped", pKV);
	m_pGlowDropdown->AddItem("#ZP_AdvOptions_HUD_GlowOption_Custom", pKV);
	m_pGlowDropdown->AddItem("#ZP_AdvOptions_HUD_GlowOption_Disabled", pKV);
}

void CHudSubOptionsColors::OnResetData()
{
	m_pColorValue[0]->ResetData();
	m_pColorValue[1]->ResetData();
	m_pColorValue[2]->ResetData();
	m_pColorValue[3]->ResetData();
	m_pColorValue[4]->ResetData();
	m_pColorValue[5]->ResetData();
	m_pGlowDropdown->ActivateItem( clamp( player_glow_style.GetInt(), 0, 3 ) );
}

void CHudSubOptionsColors::OnApplyChanges()
{
	m_pColorValue[0]->ApplyChanges();
	m_pColorValue[1]->ApplyChanges();
	m_pColorValue[2]->ApplyChanges();
	m_pColorValue[3]->ApplyChanges();
	m_pColorValue[4]->ApplyChanges();
	m_pColorValue[5]->ApplyChanges();
	player_glow_style.SetValue( m_pGlowDropdown->GetActiveItem() );
}
