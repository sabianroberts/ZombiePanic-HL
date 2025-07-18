#include <vgui/ISurface.h>
#include "gameui_scale.h"

float g_flGUIScaleX = 1.0f;
float g_flGUIScaleY = 1.0f;

void CGameUIViewport::ComputeGUIScale()
{
	int screenWide, screenTall;
	vgui2::surface()->GetScreenSize(screenWide, screenTall);

	g_flGUIScaleX = screenWide / 1920.0f;
	g_flGUIScaleY = screenTall / 1080.0f;
}

void vgui2::ScaleAllChildren(vgui2::Panel* parent, float scaleX, float scaleY)
{
	if (!parent)
		return;

	for (int i = 0; i < parent->GetChildCount(); i++)
	{
		vgui2::Panel *child = parent->GetChild(i);
		if (!child)
			continue;

		int x, y, w, h;
		child->GetPos(x, y);
		child->GetSize(w, h);
		child->SetPos(static_cast<int>(x * scaleX), static_cast<int>(y * scaleY));
		child->SetSize(static_cast<int>(x * scaleX), static_cast<int>(h * scaleY));
	}
}