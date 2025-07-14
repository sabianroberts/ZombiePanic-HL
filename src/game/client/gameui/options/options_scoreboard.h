#ifndef CSCOREBOARDSUBOPTIONS_H
#define CSCOREBOARDSUBOPTIONS_H
#include <vgui_controls/PropertyPage.h>

namespace vgui2
{
class Label;
class ComboBox;
}

class CCvarCheckButton;
class CCvarTextEntry;

class CScoreboardSubOptions : public vgui2::PropertyPage
{
	DECLARE_CLASS_SIMPLE(CScoreboardSubOptions, vgui2::PropertyPage);

public:
	CScoreboardSubOptions(vgui2::Panel *parent);

	virtual void OnResetData();
	virtual void OnApplyChanges();

private:
	CCvarCheckButton *m_pShowAvatars = nullptr;
	CCvarCheckButton *m_pShowSteamId = nullptr;
	CCvarCheckButton *m_pShowPacketLoss = nullptr;
	CCvarCheckButton *m_pShowDeaths = nullptr;

	vgui2::Label *m_pMouseLabel = nullptr;
	vgui2::ComboBox *m_pMouseBox = nullptr;
	int m_MouseItems[3];

	vgui2::Label *m_pSizeLabel = nullptr;
	vgui2::ComboBox *m_pSizeBox = nullptr;
	int m_SizeItems[3];

	CCvarTextEntry *m_pShowInHud = nullptr;

	void ApplyMouse();
	void ApplySize();
};

#endif
