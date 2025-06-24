// ============== Copyright (c) 2025 Monochrome Games ============== \\

#ifndef CWORKSHOP_SUB_UPLOAD_H
#define CWORKSHOP_SUB_UPLOAD_H
#include <vgui_controls/PropertyPage.h>
#include "WorkshopItemList.h"

class CWorkshopSubUpload : public vgui2::PropertyPage
{
	DECLARE_CLASS_SIMPLE(CWorkshopSubUpload, vgui2::PropertyPage);

public:
	CWorkshopSubUpload(vgui2::Panel *parent);
	~CWorkshopSubUpload();
	virtual void ApplySchemeSettings(vgui2::IScheme *pScheme);
	virtual void PerformLayout();
};

#endif
