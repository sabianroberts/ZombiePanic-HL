#include "options_misc_root.h"
#include "options_hud.h"
#include "options_hud_colors.h"
#include "options_misc_general.h"

CMiscSubOptionsRoot::CMiscSubOptionsRoot(vgui2::Panel *parent)
    : BaseClass(parent, "MiscSubOptionsRoot")
{
	SetSize(100, 100); // Silence "parent not sized yet" warning

	AddPage((m_pGeneral = new CMiscSubOptions(this)), "#ZP_AdvOptions_General");
	AddPage((m_pOptions = new CHudSubOptions(this)), "#ZP_AdvOptions_HUD"); // Was Options
	AddPage((m_pColors = new CHudSubOptionsColors(this)), "#ZP_AdvOptions_Colors");
}

void CMiscSubOptionsRoot::OnResetData()
{
	m_pGeneral->OnResetData();
	m_pOptions->OnResetData();
	m_pColors->OnResetData();
}

void CMiscSubOptionsRoot::OnApplyChanges()
{
	m_pGeneral->OnApplyChanges();
	m_pOptions->OnApplyChanges();
	m_pColors->OnApplyChanges();
}
