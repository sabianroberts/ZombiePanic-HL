#ifndef GAMEUI_VIEWPORT_H
#define GAMEUI_VIEWPORT_H
#include <vgui_controls/EditablePanel.h>
#include "steam/steam_api.h"
#include "zp/ui/workshop/WorkshopItemList.h"

class CGameUITestPanel;
class CAdvOptionsDialog;
class C_AchievementDialog;
class CWorkshopDialog;

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
	CWorkshopDialog *GetWorkshopDialog();

	virtual void OnThink() override;

	void GetCurrentItems( std::vector<vgui2::WorkshopItem> &items );
	void AutoMountWorkshopItem( vgui2::WorkshopItem WorkshopFile );
	void MountWorkshopItem( vgui2::WorkshopItem WorkshopFile, const char *szPath, const char *szRootPath );
	bool HasConflictingFiles( vgui2::WorkshopItem WorkshopFile );
	vgui2::WorkshopItem GetWorkshopItem( PublishedFileId_t nWorkshopID );
	void SetConflictingFiles( PublishedFileId_t nWorkshopID, bool state );
	void SetMountedState( PublishedFileId_t nWorkshopID, bool state );

	void ShowWorkshopInfoBox( const char *szText, PublishedFileId_t nWorkshopID, float flDrawTime );

	bool WorkshopIDIsMounted( PublishedFileId_t nWorkshopID );

protected:
	void UpdateAddonList();
	void LoadWorkshop();
	bool HasWorkshopAddon( PublishedFileId_t nWorkshopID );
	bool HasLoadedItem( PublishedFileId_t nWorkshopID );
	void LoadWorkshopItems( bool bWorkshopFolder );

	typedef struct subscribedcontent_t
	{
		PublishedFileId_t	m_FileID;
	} SubscribedContent;

	// list of our sources
	std::vector<SubscribedContent> m_pSubscribedContent;
	std::vector<vgui2::WorkshopItem> m_Items;

	void OnSendQueryUGCRequest( SteamUGCQueryCompleted_t *pCallback, bool bIOFailure );
	CCallResult<CGameUIViewport, SteamUGCQueryCompleted_t> m_SteamCallResultOnSendQueryUGCRequest;
	UGCQueryHandle_t	handle;

private:
	bool m_bPreventEscape = false;
	int m_iDelayedPreventEscapeFrame = 0;
	vgui2::DHANDLE<CGameUITestPanel> m_hTestPanel;
	vgui2::DHANDLE<CAdvOptionsDialog> m_hOptionsDialog;
	vgui2::DHANDLE<C_AchievementDialog> m_hAchDialog;
	vgui2::DHANDLE<CWorkshopDialog> m_hWorkshopDialog;

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
