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
#include "CWorkshopSubUploaded.h"

CWorkshopSubUploaded::CWorkshopSubUploaded(vgui2::Panel *parent)
    : BaseClass(parent, "WorkshopSubUploaded")
{
	SetSize(100, 100); // Silence "parent not sized yet" warning
	LoadControlSettings(VGUI2_ROOT_DIR "resource/workshop/uploaded.res");
}

CWorkshopSubUploaded::~CWorkshopSubUploaded()
{
}

void CWorkshopSubUploaded::ApplySchemeSettings(vgui2::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CWorkshopSubUploaded::PerformLayout()
{
	BaseClass::PerformLayout();
}
