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
#include <vgui_controls/CheckButtonList.h>
#include <KeyValues.h>
#include <appversion.h>
#include <bhl_urls.h>
#include "client_vgui.h"
#include "gameui/gameui_viewport.h"
#include "CWorkshopSubUpload.h"

static CWorkshopSubUpload *pUploader = nullptr;

static const char *szTags_Weapons[] = {
	"Crowbar",
	"Zombie Arms",
	"Sig Sauer",
	"Revolver",
	"M4",
	"MP5",
	"Shotgun",
};

static const char *szTags_Items[] = {
	"Medkit",
	"Armor",
	"Ammo Boxes",
	"Backpack",
	"TNT",
	"Satchel Charge",
};

static const char *szTags_Generic[] = {
	"Sounds",
	"Survivor",
	"Zombie",
	"Background",
	"Music",
	"Sprays",
};

CWorkshopSubUpload::CWorkshopSubUpload(vgui2::Panel *parent)
    : BaseClass(parent, "WorkshopSubUpload")
{
	SetSize(100, 100); // Silence "parent not sized yet" warning
	
	pDescBox = new vgui2::TextEntry( this, "AddonDescTextBox" );
	pTitleBox = new vgui2::TextEntry( this, "AddonTitleTextBox" );
	pContentText = new vgui2::TextEntry( this, "BrowseFolderTextBox" );
	pAddonImage = new vgui2::ImagePanel( this, "AddonImage" );
	pVisibilty = new vgui2::ComboBox( this, "Visibility", 5, false );
	pTags[0] = new vgui2::CheckButtonList( this, "Tags1" );
	pTags[1] = new vgui2::CheckButtonList( this, "Tags2" );
	pTags[2] = new vgui2::CheckButtonList( this, "Tags3" );
	pChangeLogLabel = new vgui2::Label( this, "ChangeLogText", "#ZP_UI_Workshop_Upload_Changelog" );
	pChangeLogText = new vgui2::TextEntry( this, "ChangeLogTextBox" );
	
	// Load this after we created our objects
	LoadControlSettings(VGUI2_ROOT_DIR "resource/workshop/upload.res");

	// KeyValues
	KeyValues *kv = new KeyValues( "Categories", "Key", "Value" );

	pVisibilty->AddItem( "#ZP_UI_Workshop_Upload_Tags_Private", kv );
	pVisibilty->AddItem( "#ZP_UI_Workshop_Upload_Tags_FriendsOnly", kv );
	pVisibilty->AddItem( "#ZP_UI_Workshop_Upload_Tags_Public", kv );
	// Auto select "Show All Achievements"
	pVisibilty->ActivateItem( 0 );

	// Never allow edit
	pVisibilty->SetEditable( false );

	// Allow edit on the desc box
	// And make sure it got a scrollbar
	pDescBox->SetEditable( true );
	pDescBox->SetVerticalScrollbar( true );
	pDescBox->SetMultiline( true );
	pDescBox->SetWrap( true );

	// Only show the changelog, if we are updating our addon.
	pChangeLogText->SetVisible( false );
	pChangeLogLabel->SetVisible( false );

	pChangeLogText->SetEditable( true );
	pChangeLogText->SetVerticalScrollbar( true );
	pChangeLogText->SetMultiline( true );
	pChangeLogText->SetWrap( true );

	// Make sure we scale it
	pAddonImage->SetShouldScaleImage( true );

	last_folder[0].clear();
	last_folder[1].clear();

	// Populate the tags
	for ( int i = 0; i < ARRAYSIZE( szTags_Weapons ); i++ )
		pTags[0]->AddItem( szTags_Weapons[i], false, new KeyValues( "Item", szTags_Weapons[i], "1" ) );

	for ( int i = 0; i < ARRAYSIZE( szTags_Items ); i++ )
		pTags[1]->AddItem( szTags_Items[i], false, new KeyValues( "Item", szTags_Items[i], "1" ) );

	for ( int i = 0; i < ARRAYSIZE( szTags_Generic ); i++ )
		pTags[2]->AddItem( szTags_Generic[i], false, new KeyValues( "Item", szTags_Generic[i], "1" ) );

	pUploader = this;
}

CWorkshopSubUpload::~CWorkshopSubUpload()
{
	pUploader = nullptr;
}

void CWorkshopSubUpload::ApplySchemeSettings(vgui2::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CWorkshopSubUpload::PerformLayout()
{
	BaseClass::PerformLayout();
}

void OnFileSelected( DialogData *pData )
{
	if ( pData->IsFolder )
		pUploader->UpdateContentPath( pData->LocalPath, pData->FullPath );
	else
		pUploader->UpdatePreviewImage( pData->FileExtension, pData->LocalPath, pData->LocalGamePath, pData->FullPath );
}

void CWorkshopSubUpload::OnCommand(const char *pcCommand)
{
	if ( !Q_stricmp( pcCommand, "BrowseImage" ) )
	{
		CGameUIViewport::Get()->OpenFileExplorer( OpenFileDialog_e::FILE_TGA, last_folder[0].c_str(), "WORKSHOP", OnFileSelected );
	}
	else if ( !Q_stricmp( pcCommand, "BrowseFolder" ) )
	{
		CGameUIViewport::Get()->OpenFileExplorer( OpenFileDialog_e::FOLDER, last_folder[1].c_str(), "WORKSHOP", OnFileSelected );
	}
	else if ( !Q_stricmp( pcCommand, "AddonUpload" ) )
	{
		ConPrintf( Color( 255, 255, 0, 255 ), "AddonUpload\n" );
	}
	else
	{
		BaseClass::OnCommand( pcCommand );
	}
}

void CWorkshopSubUpload::OnCheckButtonChecked( KeyValues *pParams )
{
	//ConPrintf( Color( 255, 255, 0, 255 ), "OnCheckButtonChecked(\n\t%s\n)\n", pParams->GetName() );
	//ConPrintf( Color( 255, 255, 0, 255 ), "ItemID(\n\t%i\n)\n", pParams->GetInt( "itemid" ) );
}

void CWorkshopSubUpload::UpdateContentPath( const std::string szLocalpath, const std::string szFullpath )
{
	last_folder[1] = szLocalpath;
	pContentText->SetText( szFullpath.c_str() );
}

void CWorkshopSubUpload::UpdatePreviewImage( const std::string szFileext, const std::string szLocalpath, const std::string szLocalGamepath, const std::string szFullpath )
{
	last_folder[0] = szLocalpath;
	std::string szFile = szLocalGamepath;
	const size_t found = szFileext.size();
	szFile = szFile.substr(0, szFile.size() - found);
	pAddonImage->SetImage( szFile.c_str() );
}
