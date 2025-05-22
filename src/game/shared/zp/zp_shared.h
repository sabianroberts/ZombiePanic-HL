// ============== Copyright (c) 2025 Monochrome Games ============== \\

#ifndef SHARED_ZOMBIEPANIC
#define SHARED_ZOMBIEPANIC
#pragma once

#include "Color.h"
#include <vector>

#define AMMOWEIGHT_BUCKSHOT 1.25f
#define AMMOWEIGHT_9MM 0.21f
#define AMMOWEIGHT_556AR 0.21f
#define AMMOWEIGHT_357 0.65f

namespace ZP
{
	enum RoundState
	{
		RoundState_Invalid = -1,
		RoundState_WaitingForPlayers = 0,
		RoundState_RoundIsStarting,
		RoundState_PickVolunteers,
		RoundState_RoundHasBegunPost,
		RoundState_RoundHasBegun,
		RoundState_RoundIsOver
	};
	
	enum Teams_e
	{
		TEAM_NONE,
		TEAM_OBSERVER,
		TEAM_SURVIVIOR,
		TEAM_ZOMBIE,
	
		MAX_TEAM
	};
	
	static int MaxSpeeds[2] = {
		220,	// Human Max Speed
		250		// Zombie Max Speed
	};
	
	static const char *Teams[MAX_TEAM] = {
		"",
		"Observer",
		"Survivor",
		"Zombie"
	};
	
	enum GameModeType_e
	{
		GAMEMODE_INVALID = -1,
		GAMEMODE_NONE,
		GAMEMODE_SURVIVAL,
		GAMEMODE_OBJECTIVE,
	
		MAX_GAMEMODES
	};
	GameModeType_e IsValidGameModeMap(const char *szLevel);
	static const char *GameModes[MAX_GAMEMODES] = {
		"Development",
		"Last Man Standing",
		"Objective"
	};
	
	namespace ColorGradient
	{
		class ColorBase
		{
		public:
			int r;
			int g;
			int b;
			int a;
			float flValue;
			ColorBase( Color cColor, float flPercent )
			{
				r = cColor.r();
				g = cColor.g();
				b = cColor.b();
				a = cColor.a();
				flValue = flPercent;
			}
		};
	
		class Base
		{
		public:
			void AddColor( ColorBase *value )
			{
				m_Colors.push_back( value );
			}
	
			Color GetColorForValue( float flValue, float flAlpha = 255 )
			{
				if ( m_Colors.size() < 1 )
					return Color( 255, 0, 255, flAlpha );
	
				for ( int i = 0; i < int(m_Colors.size()); i++ )
				{
					ColorBase *sCurrent = m_Colors[i];
	
					if ( flValue <= sCurrent->flValue )
					{
						int y = i-1;
						if ( y < 0 ) y = 0;
	
						ColorBase *sPrevious = m_Colors[y];
	
						float flDiff = (sPrevious->flValue - sCurrent->flValue);
						float flFrac = flDiff == 0.0f ? 0.0f : (flValue - sCurrent->flValue) / flDiff;
						int iRed     = Float2Int( (sPrevious->r - sCurrent->r) * flFrac + sCurrent->r );
						int iGreen   = Float2Int( (sPrevious->g - sCurrent->g) * flFrac + sCurrent->g );
						int iBlue    = Float2Int( (sPrevious->b - sCurrent->b) * flFrac + sCurrent->b );
	
						return Color( iRed, iGreen, iBlue, flAlpha );
					}
				}
				return Color( 255, 255, 255, flAlpha );
			}
	
			std::vector<ColorBase*> m_Colors;
		};
	
		class WhiteGreen : public Base
	    {
		public:
			WhiteGreen()
			{
				AddColor( new ColorBase( Color( 255, 255, 255 ), 0.099999999999999f ) );
				AddColor( new ColorBase( Color( 0, 255, 0 ), 1.000000f ) );
			}
		};
	
		class WhiteYellow : public Base
	    {
		public:
			WhiteYellow()
			{
				AddColor( new ColorBase( Color( 255, 255, 255 ), 0.099999999999999f ) );
				AddColor( new ColorBase( Color( 255, 255, 0 ), 1.000000f ) );
			}
		};
	
		class RedYellowGreen : public Base
		{
		public:
			RedYellowGreen()
			{
				AddColor( new ColorBase( Color( 255, 0, 0 ), 0.333333f ) ); // full red
				AddColor( new ColorBase( Color( 255, 255, 0 ), 0.666666f ) ); // full yellow
				AddColor( new ColorBase( Color( 0, 255, 0 ), 1.000000f ) ); // full green
			}
		};
	
		class RainBow : public Base
	    {
		public:
			RainBow()
			{
				AddColor( new ColorBase( Color( 255, 0, 0 ), 0.099999999999999f ) );
				AddColor( new ColorBase( Color( 255, 255, 0 ), 0.1666666666666667f ) );
				AddColor( new ColorBase( Color( 0, 255, 0 ), 0.5f ) );
				AddColor( new ColorBase( Color( 0, 255, 255 ), 0.6666666666666667f ) );
				AddColor( new ColorBase( Color( 0, 0, 255 ), 0.8333333333333333f ) );
				AddColor( new ColorBase( Color( 255, 0, 255 ), 1.000000f ) );
			}
		};
	}
}

#endif