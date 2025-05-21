#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
#include <vgui/IVGui.h>
#include "hud.h"
#include "parsemsg.h"
#include "cl_util.h"
#include "cl_voice_status.h"
#include "zp_zombielives.h"
#include "vgui/client_viewport.h"

DEFINE_HUD_ELEM(CHudZombieLives);

CHudZombieLives::CHudZombieLives()
    : vgui2::Panel(NULL, "HudZombieLives")
{
	SetParent(g_pViewport);

	m_clrIcon = Color(255, 255, 255, 254);

	m_pLives = new vgui2::Label( this, "Lives", "0" );

	HookMessage<&CHudZombieLives::MsgFunc_ZombieLives>("ZombieLives");
}

void CHudZombieLives::ApplySchemeSettings(vgui2::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	SetBgColor( Color(0, 0, 0, 0) );
}

bool CHudZombieLives::IsAllowedToDraw()
{
	if ( gHUD.m_RoundState < ZP::RoundState::RoundState_RoundHasBegunPost ) return false;
	if ( g_pViewport->IsVGUIVisible( MENU_TEAM ) ) return false;
	if ( g_pViewport->IsVGUIVisible( MENU_MOTD ) ) return false;
	if ( gHUD.m_GameMode != ZP::GAMEMODE_SURVIVAL ) return false;
	CPlayerInfo *localplayer = GetPlayerInfo( gEngfuncs.GetLocalPlayer()->index );
	if ( !localplayer->IsConnected() ) return false;
	return true;
}

void CHudZombieLives::Paint()
{
	if ( !IsAllowedToDraw() )
	{
		if ( m_pLives->IsVisible() )
			m_pLives->SetVisible( false );
		return;
	}

	int x, y, w, h;
	GetBounds( x, y, w, h );

	vgui2::surface()->DrawSetTexture( m_iIconTexture );
	vgui2::surface()->DrawSetColor( m_clrIcon );
	vgui2::surface()->DrawTexturedRect( 0, 0, m_iIconWide, m_iIconTall );

	// Let's move our text to the right a bit
	// but not too far, or too close.
	int xPos = m_iIconWide + m_iTextXAdd;

	// Now draw our lives
	m_pLives->SetBounds( xPos, 0, m_iTextSizeWide, m_iTextSizeTall );
}

int CHudZombieLives::MsgFunc_ZombieLives( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	int lives = READ_SHORT();
	char text[4];
	sprintf( text, "%i", lives );
	m_pLives->SetText( text );
	return 1;
}
