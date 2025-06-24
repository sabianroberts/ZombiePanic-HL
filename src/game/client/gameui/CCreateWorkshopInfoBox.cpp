// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include <vgui/IVGui.h>
#include "CCreateWorkshopInfoBox.h"
#include "gameui_viewport.h"
#include "client_vgui.h"

CCreateWorkshopInfoBox::CCreateWorkshopInfoBox(vgui2::Panel *pParent)
    : BaseClass(pParent, "CreateWorkshopInfoBox")
{
	SetTitle("", false);
	SetSizeable( false );
	SetSize( 100, 80 );
	SetPos( 0, 0 );
	SetTitleBarVisible( false );
	SetDeleteSelfOnClose( true );

	m_pText = new vgui2::Label( this, "Text", "Mounting Addon..." );
	m_pWorkshopID = new vgui2::Label( this, "WorkshopID", "12345" );

	SetScheme( vgui2::scheme()->LoadSchemeFromFile( VGUI2_ROOT_DIR "resource/ClientSourceSchemeBase.res", "ClientSourceSchemeBase" ) );

	LoadControlSettings( VGUI2_ROOT_DIR "resource/workshop/infobox.res" );

	SetScheme( CGameUIViewport::Get()->GetScheme() );
	InvalidateLayout();

	vgui2::ivgui()->AddTickSignal( GetVPanel(), 1000 );

	MoveToFront();

	m_RemoveTime = 2.0f;
}

void CCreateWorkshopInfoBox::SetData( const char *szString, uint64 nWorkshopID, float flTime )
{
	m_pText->SetColorCodedText( szString );
	m_pWorkshopID->SetText( vgui2::VarArgs( "%llu", nWorkshopID ) );
	m_RemoveTime = flTime;
}

void CCreateWorkshopInfoBox::OnTick()
{
	BaseClass::OnTick();
	if ( m_RemoveTime <= 0 )
		Close();
	m_RemoveTime--;
}