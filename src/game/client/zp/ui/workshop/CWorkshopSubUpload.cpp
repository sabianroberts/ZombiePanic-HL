// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include <string>
#include <vector>
#include <FileSystem.h>
#include <tier1/strtools.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/URLLabel.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/RichText.h>
#include <KeyValues.h>
#include <appversion.h>
#include <bhl_urls.h>
#include "client_vgui.h"
#include "CWorkshopSubUpload.h"

CWorkshopSubUpload::CWorkshopSubUpload(vgui2::Panel *parent)
    : BaseClass(parent, "WorkshopSubUpload")
{
	SetSize(100, 100); // Silence "parent not sized yet" warning
	LoadControlSettings(VGUI2_ROOT_DIR "resource/workshop/upload.res");
}

CWorkshopSubUpload::~CWorkshopSubUpload()
{
}

void CWorkshopSubUpload::ApplySchemeSettings(vgui2::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CWorkshopSubUpload::PerformLayout()
{
	BaseClass::PerformLayout();
}
