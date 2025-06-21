#include <vgui_controls/Label.h>
#include <vgui_controls/Slider.h>
#include <vgui_controls/ComboBox.h>
#include <KeyValues.h>
#include "client_vgui.h"
#include "client_steam_context.h"
#include "cvar_text_entry.h"
#include "cvar_check_button.h"
#include "options_general.h"
#include "hud.h"
#include "cl_util.h"
#include "engine_patches.h"
#include "sdl_rt.h"

extern ConVar m_input;

CGeneralSubOptions::CGeneralSubOptions(vgui2::Panel *parent)
    : BaseClass(parent, "GeneralSubOptions")
{
	SetSize(100, 100); // Silence "parent not sized yet" warning

	m_pFovLabel = new vgui2::Label(this, "FovLabel", "#ZP_AdvOptions_General_FOV");
	m_pFovValue = new CCvarTextEntry(this, "FovValue", "default_fov", CCvarTextEntry::CvarType::Float);
	m_pFovSlider = new vgui2::Slider(this, "FovSlider");
	m_pFovSlider->SetRange(90, 150);
	m_pFovSlider->SetValue(m_pFovValue->GetFloat());
	m_pFovSlider->AddActionSignalTarget(this);

	m_pRawInputLabel = new vgui2::Label(this, "RawInputLabel", "#ZP_AdvOptions_General_Input");
	m_pInputMethodBox = new vgui2::ComboBox(this, "InputMethodBox", 3, false);
	m_InputMethodItems[0] = m_pInputMethodBox->AddItem("#ZP_AdvOptions_General_InputWin", new KeyValues("Item", "value", 0));
	m_InputMethodItems[1] = m_pInputMethodBox->AddItem(GetItemText("ZP_AdvOptions_General_InputDX", IsWindows()), new KeyValues("Item", "value", 1));
	m_InputMethodItems[2] = m_pInputMethodBox->AddItem(GetItemText("ZP_AdvOptions_General_InputSDL", !IsWindows()), new KeyValues("Item", "value", 2));

	m_pZPSSnd = new CCvarCheckButton(this, "ZPSSnd", "#ZP_AdvOptions_General_ZPSSnd", "cl_zpssound");
	m_pZPSSndLabel = new vgui2::Label(this, "ZPSSndLabel", "#ZP_AdvOptions_General_ZPSSnd2");

	m_pKillSnd = new CCvarCheckButton(this, "KillSnd", "#ZP_AdvOptions_General_KillSnd", "cl_killsound");
	m_pKillSndLabel = new vgui2::Label(this, "KillSndLabel", "#ZP_AdvOptions_General_KillSnd2");

	m_pMOTD = new CCvarCheckButton(this, "MOTD", "#ZP_AdvOptions_General_HTML", "cl_enable_html_motd");
	m_pMOTDLabel = new vgui2::Label(this, "MOTDLabel", "#ZP_AdvOptions_General_HTML2");

	m_pLogChat = new CCvarCheckButton(this, "LogChat", "#ZP_AdvOptions_General_LogChat", "results_log_chat");
	m_pLogOther = new CCvarCheckButton(this, "LogOther", "#ZP_AdvOptions_General_LogOther", "results_log_other");
	m_pAutoDemo = new CCvarCheckButton(this, "AutoDemo", "#ZP_AdvOptions_General_AutoDemo", "results_demo_autorecord");
	m_pAutoDemoLabel = new vgui2::Label(this, "AutoDemoLabel", "#ZP_AdvOptions_General_AutoDemo2");
	m_pKeepFor = new CCvarTextEntry(this, "KeepFor", "results_demo_keepdays", CCvarTextEntry::CvarType::Int);
	m_pKeepForLabel = new vgui2::Label(this, "KeepForLabel", "#ZP_AdvOptions_General_KeepFor");
	m_pDaysLabel = new vgui2::Label(this, "DaysLabel", "#ZP_AdvOptions_General_Days");

	LoadControlSettings(VGUI2_ROOT_DIR "resource/options/GeneralSubOptions.res");

	// Disable unsupported input methods
	m_pInputMethodBox->SetItemEnabled(m_InputMethodItems[0], IsWindows()); // WindowsCursor
	m_pInputMethodBox->SetItemEnabled(m_InputMethodItems[1], IsWindows()); // DirectInput
	m_pInputMethodBox->SetItemEnabled(m_InputMethodItems[2], GetSDL()->IsGood()); // RawInput

	// Disable HTML MOTD if SteamAPI not available
	if (!SteamAPI_IsAvailable())
	{
		m_pMOTD->SetEnabled(false);
		m_pMOTD->SetSelected(false);
	}
}

void CGeneralSubOptions::PerformLayout()
{
	BaseClass::PerformLayout();
}

void CGeneralSubOptions::OnResetData()
{
	m_pFovValue->ResetData();
	m_pInputMethodBox->ActivateItem(m_InputMethodItems[clamp(m_input.GetInt(), 0, 2)]);
	m_pKillSnd->ResetData();
	m_pZPSSnd->ResetData();

	if (m_pMOTD->IsEnabled())
		m_pMOTD->ResetData();

	m_pLogChat->ResetData();
	m_pLogOther->ResetData();
	m_pAutoDemo->ResetData();
	m_pKeepFor->ResetData();
}

void CGeneralSubOptions::OnApplyChanges()
{
	m_pFovValue->ApplyChanges();
	m_pKillSnd->ApplyChanges();
	m_pZPSSnd->ApplyChanges();

	if (m_pMOTD->IsEnabled())
		m_pMOTD->ApplyChanges();

	m_pLogChat->ApplyChanges();
	m_pLogOther->ApplyChanges();
	m_pAutoDemo->ApplyChanges();
	m_pKeepFor->ApplyChanges();

	for (int i = 0; i <= 2; i++)
	{
		if (m_pInputMethodBox->GetActiveItem() == m_InputMethodItems[i])
		{
			m_input.SetValue(i);
			break;
		}
	}
}

void CGeneralSubOptions::OnSliderMoved(KeyValues *kv)
{
	void *pPanel = kv->GetPtr("panel");
	if (pPanel == m_pFovSlider)
	{
		float val = kv->GetFloat("position");
		m_pFovValue->SetValue(val);
	}
}

void CGeneralSubOptions::OnCvarTextChanged(KeyValues *kv)
{
	void *pPanel = kv->GetPtr("panel");
	if (pPanel == m_pFovValue)
	{
		float val = kv->GetFloat("value");
		m_pFovSlider->SetValue(val, false);
	}
}

const wchar_t *CGeneralSubOptions::GetItemText(const char *token, bool isRecommended)
{
	wchar_t *text = g_pVGuiLocalize->Find(token);

	if (!isRecommended)
		return text;

	static wchar_t wbuf[256];
	wchar_t *rec = g_pVGuiLocalize->Find("BHL_AdvOptions_General_InputRec");
	V_snwprintf(wbuf, std::size(wbuf), L"%ls %ls", text, rec);
	return wbuf;
}
