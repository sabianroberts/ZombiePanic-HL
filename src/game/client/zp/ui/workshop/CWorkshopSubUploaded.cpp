#include <string>
#include <vector>
#include <FileSystem.h>
#include <tier1/strtools.h>
#include <vgui/IVGui.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/Frame.h>
#include <KeyValues.h>
#include <appversion.h>
#include <bhl_urls.h>
#include "client_vgui.h"
#include "gameui/gameui_viewport.h"
#include "CWorkshopSubUploaded.h"

CWorkshopSubUploaded::CWorkshopSubUploaded(vgui2::Panel *parent)
    : BaseClass(parent, "WorkshopSubUploaded")
{
	SetSize(100, 100); // Silence "parent not sized yet" warning

	// Our Item List
	pList = new vgui2::WorkshopItemList( this, "listpanel" );
	pList->SetPos( 15, 100 );
	pList->SetSize( 600, 302 );

	// Load this last, so we can move our items around.
	LoadControlSettings( VGUI2_ROOT_DIR "resource/workshop/uploaded.res" );

	vgui2::ivgui()->AddTickSignal( GetVPanel(), 25 );

	if ( !GetSteamAPI() ) return;
	if ( GetSteamAPI()->SteamUGC() && GetSteamAPI()->SteamUser() )
	{
		handle = GetSteamAPI()->SteamUGC()->CreateQueryUserUGCRequest(
			GetSteamAPI()->SteamUser()->GetSteamID().GetAccountID(),
		    k_EUserUGCList_Published,
			k_EUGCMatchingUGCType_Items_ReadyToUse,
			k_EUserUGCListSortOrder_LastUpdatedDesc,
		    (AppId_t)3825360, (AppId_t)3825360, 1
		);
		GetSteamAPI()->SteamUGC()->SetReturnChildren( handle, true );
		SteamAPICall_t apiCall = GetSteamAPI()->SteamUGC()->SendQueryUGCRequest( handle );
		m_SteamCallResultOnSendQueryUGCRequest.Set( apiCall, this, &CWorkshopSubUploaded::OnSendQueryUGCRequest );
	}
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

void CWorkshopSubUploaded::AddItem( vgui2::WorkshopItem item )
{
	pList->DeleteAllItems();

	// Fonts
	vgui2::HFont hTextFont;
	vgui2::IScheme *pScheme = vgui2::scheme()->GetIScheme(
		vgui2::scheme()->LoadSchemeFromFile( VGUI2_ROOT_DIR "resource/ClientSourceScheme.res", "ClientSourceScheme" )
	);

	// Create Image
	vgui2::ImagePanel *pIcon = new vgui2::ImagePanel( this, "Icon" );

	char buffer[158];
	Q_snprintf( buffer, sizeof( buffer ), "%llu/thumb", item.uWorkshopID );

	pIcon->SetImage( vgui2::scheme()->GetImage( buffer, false ) );
	pIcon->SetSize( 56, 56 );
	pIcon->SetPos( 4, 4 );

	// Font Text
	vgui2::Label *pTitle = new vgui2::Label( this, "Title", "" );
	pTitle->SetSize( 400, 20 );
	pTitle->SetPos( 70, 5 );
	pTitle->SetPaintBackgroundEnabled( false );
	hTextFont = pScheme->GetFont( "AchievementItemTitle" );
	if ( hTextFont != vgui2::INVALID_FONT )
		pTitle->SetFont( hTextFont );
	pTitle->SetColorCodedText( item.szName );

	vgui2::Label *pAuthor = new vgui2::Label( this, "Author", "" );
	pAuthor->SetSize( 400, 20 );
	pAuthor->SetPos( 70, 35 );
	pAuthor->SetPaintBackgroundEnabled( false );
	hTextFont = pScheme->GetFont( "AchievementItemDescription" );
	if ( hTextFont != vgui2::INVALID_FONT )
		pAuthor->SetFont( hTextFont );
	pAuthor->SetColorCodedText( item.szAuthor );

	vgui2::Label *pDesc = new vgui2::Label( this, "Description", "" );
	pDesc->SetSize( 400, 20 );
	pDesc->SetPos( 70, 15 );
	pDesc->SetPaintBackgroundEnabled( false );
	hTextFont = pScheme->GetFont( "AchievementItemDescription" );
	if ( hTextFont != vgui2::INVALID_FONT )
		pDesc->SetFont( hTextFont );
	pDesc->SetColorCodedText( item.szDesc );

	pList->AddItem(
	    pIcon, pAuthor,
		pTitle, pDesc,
	    nullptr, nullptr,
	    item.uWorkshopID
	);
}

void CWorkshopSubUploaded::OnSendQueryUGCRequest( SteamUGCQueryCompleted_t *pCallback, bool bIOFailure )
{
	bool bFailed = ( bIOFailure || ( pCallback->m_eResult != k_EResultOK ) );
	if ( bFailed )
	{
#if defined( SPDLOG )
		SpdLog(
			"workshop_client",
			UTIL_CurrentMapLog(),
			LOGTYPE_WARN,
			"Failed to send query. ErrorID: %i",
			pCallback->m_eResult
		);
#else
		ConPrintf( Color( 255, 22, 22, 255 ), "[Workshop] Failed to retrieve uploaded addon. ErrorID: %i\n", pCallback->m_eResult );
#endif
		GetSteamAPI()->SteamUGC()->ReleaseQueryUGCRequest( handle );
		return;
	}

	// Create it
	SteamUGCDetails_t *pDetails = new SteamUGCDetails_t;

	// Get our info
	if ( GetSteamAPI()->SteamUGC()->GetQueryUGCResult( pCallback->m_handle, 0, pDetails ) )
	{
		vgui2::WorkshopItem WorkshopAddon;
		Q_snprintf( WorkshopAddon.szName, sizeof( WorkshopAddon.szName ), "%s", pDetails->m_rgchTitle );
		Q_snprintf( WorkshopAddon.szDesc, sizeof( WorkshopAddon.szDesc ), "%s", pDetails->m_rgchDescription );
		Q_snprintf( WorkshopAddon.szAuthor, sizeof( WorkshopAddon.szAuthor ), "%s", GetSteamAPI()->SteamFriends()->GetPersonaName() );
		WorkshopAddon.iFilterFlag = 0;
		WorkshopAddon.bMounted = false;
		WorkshopAddon.bIsWorkshopDownload = false;
		WorkshopAddon.bFoundConflictingFiles = false;
		WorkshopAddon.uWorkshopID = pDetails->m_nPublishedFileId;
		AddItem( WorkshopAddon );
	}

	// Delete it
	if ( pDetails )
		delete pDetails;

	GetSteamAPI()->SteamUGC()->ReleaseQueryUGCRequest( handle );
}