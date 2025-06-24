// ============== Copyright (c) 2025 Monochrome Games ============== \\

#ifndef CWORKSHOP_SUB_UPLOADED_H
#define CWORKSHOP_SUB_UPLOADED_H
#include <vgui_controls/PropertyPage.h>
#include "WorkshopItemList.h"

class CWorkshopSubUploaded : public vgui2::PropertyPage
{
	DECLARE_CLASS_SIMPLE(CWorkshopSubUploaded, vgui2::PropertyPage);

public:
	CWorkshopSubUploaded(vgui2::Panel *parent);
	~CWorkshopSubUploaded();
	virtual void ApplySchemeSettings(vgui2::IScheme *pScheme);
	virtual void PerformLayout();
};

#endif
