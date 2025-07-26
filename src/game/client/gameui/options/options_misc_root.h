#ifndef OPTIONS_HUD_ROOT_H
#define OPTIONS_HUD_ROOT_H
#include <vgui_controls/PropertySheet.h>

class CHudSubOptions;
class CMiscSubOptions;
class CHudSubOptionsColors;

class CMiscSubOptionsRoot : public vgui2::PropertySheet
{
public:
	DECLARE_CLASS_SIMPLE(CMiscSubOptionsRoot, vgui2::PropertySheet);

	CMiscSubOptionsRoot(vgui2::Panel *parent);

private:
	CMiscSubOptions *m_pGeneral = nullptr;
	CHudSubOptions *m_pOptions = nullptr;
	CHudSubOptionsColors *m_pColors = nullptr;

	MESSAGE_FUNC(OnResetData, "ResetData");
	MESSAGE_FUNC(OnApplyChanges, "ApplyChanges");
};

#endif
