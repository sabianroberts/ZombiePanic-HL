// ============== Copyright (c) 2025 Monochrome Games ============== \\

#ifndef CWORKSHOP_SUB_UPLOAD_H
#define CWORKSHOP_SUB_UPLOAD_H
#include <vgui_controls/PropertyPage.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/TextEntry.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/CheckButtonList.h>
#include <vgui_controls/ImagePanel.h>
#include "WorkshopItemList.h"

class CWorkshopSubUpload : public vgui2::PropertyPage
{
	DECLARE_CLASS_SIMPLE(CWorkshopSubUpload, vgui2::PropertyPage);

public:
	CWorkshopSubUpload(vgui2::Panel *parent);
	~CWorkshopSubUpload();
	virtual void ApplySchemeSettings(vgui2::IScheme *pScheme);
	virtual void PerformLayout();
	virtual void OnCommand(const char *pcCommand);

	void UpdateContentPath( DialogData *pData );
	void UpdatePreviewImage( DialogData *pData );

protected:
	std::string last_folder[2];
	MESSAGE_FUNC_PARAMS( OnCheckButtonChecked, "CheckButtonChecked", pParams );

private:
	vgui2::TextEntry *pDescBox;
	vgui2::TextEntry *pTitleBox;
	vgui2::TextEntry *pContentText;
	vgui2::Label *pChangeLogLabel;
	vgui2::TextEntry *pChangeLogText;
	vgui2::ImagePanel *pAddonImage;
	vgui2::ComboBox *pVisibilty;
	vgui2::CheckButtonList *pTags[3];
};

#endif
