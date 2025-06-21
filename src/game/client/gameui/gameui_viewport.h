#ifndef GAMEUI_VIEWPORT_H
#define GAMEUI_VIEWPORT_H
#include <vgui_controls/EditablePanel.h>

class CGameUITestPanel;
class CAdvOptionsDialog;
class C_AchievementDialog;

class CGameUIViewport : public vgui2::EditablePanel
{
	DECLARE_CLASS_SIMPLE(CGameUIViewport, vgui2::EditablePanel);

public:
	static inline CGameUIViewport *Get()
	{
		return m_sInstance;
	}

	CGameUIViewport();
	~CGameUIViewport();

	/**
	 * If enabled, prevents ESC key from showing GameUI.
	 * In reality, hides GameUI whenever it is enabled.
	 */
	void PreventEscapeToShow(bool state);

	void OpenTestPanel();
	CAdvOptionsDialog *GetOptionsDialog();
	C_AchievementDialog *GetAchievementDialog();

	virtual void OnThink() override;

private:
	bool m_bPreventEscape = false;
	int m_iDelayedPreventEscapeFrame = 0;
	vgui2::DHANDLE<CGameUITestPanel> m_hTestPanel;
	vgui2::DHANDLE<CAdvOptionsDialog> m_hOptionsDialog;
	vgui2::DHANDLE<C_AchievementDialog> m_hAchDialog;

	template <typename T>
	inline T *GetDialog(vgui2::DHANDLE<T> &handle)
	{
		if (!handle.Get())
		{
			handle = new T(this);
		}

		return handle;
	}

	static inline CGameUIViewport *m_sInstance = nullptr;
};

#endif
