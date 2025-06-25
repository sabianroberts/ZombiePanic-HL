// ============== Copyright (c) 2025 Monochrome Games ============== \\

#ifndef CWORKSHOP_SUB_UPLOADED_H
#define CWORKSHOP_SUB_UPLOADED_H
#include <vgui_controls/PropertyPage.h>
#include <vgui_controls/ComboBox.h>
#include "WorkshopItemList.h"

class CWorkshopSubUploaded : public vgui2::PropertyPage
{
	DECLARE_CLASS_SIMPLE(CWorkshopSubUploaded, vgui2::PropertyPage);

public:
	CWorkshopSubUploaded(vgui2::Panel *parent);
	~CWorkshopSubUploaded();

	virtual void AddItem( vgui2::WorkshopItem item );

	virtual void ApplySchemeSettings(vgui2::IScheme *pScheme);
	virtual void PerformLayout();

protected:
	void OnSendQueryUGCRequest( SteamUGCQueryCompleted_t *pCallback, bool bIOFailure );
	CCallResult<CWorkshopSubUploaded, SteamUGCQueryCompleted_t> m_SteamCallResultOnSendQueryUGCRequest;
	UGCQueryHandle_t	handle;

private:
	vgui2::WorkshopItemList *pList;
};

#endif
