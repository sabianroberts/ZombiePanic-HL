// ============== Copyright (c) 2025 Monochrome Games ============== \\

#ifndef CREATE_WORKSHOP_INFO_BOX
#define CREATE_WORKSHOP_INFO_BOX
#include <vgui_controls/Frame.h>
#include <vgui_controls/Label.h>

class CCreateWorkshopInfoBox : public vgui2::Frame
{
	DECLARE_CLASS_SIMPLE(CCreateWorkshopInfoBox, vgui2::Frame);

public:
	CCreateWorkshopInfoBox(vgui2::Panel *pParent);
	void SetData(const char *szString, uint64 nWorkshopID, float flTime);

private:
	virtual void OnTick();
	float m_RemoveTime;
	vgui2::Label *m_pText;
	vgui2::Label *m_pWorkshopID;
};

#endif