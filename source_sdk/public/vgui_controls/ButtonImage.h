// ============== Copyright (c) 2025 Monochrome Games ============== \\

#ifndef BUTTON_IMAGE_H
#define BUTTON_IMAGE_H

#include "Button.h"
#include "ImagePanel.h"

namespace vgui2
{

class ButtonImage : public Button
{
	DECLARE_CLASS_SIMPLE( ButtonImage, Button );

public:
	// You can optionally pass in the panel to send the click message to and the name of the command to send to that panel.
	ButtonImage(Panel *parent, const char *panelName, const char *image, Panel *pActionSignalTarget = NULL, const char *pCmd = NULL);
	~ButtonImage();

	void SetImage( const char *szImage );

private:
	virtual void Paint( void );
	virtual void ApplySettings( KeyValues *inResourceData );
	ImagePanel *pImage;
};

}

#endif