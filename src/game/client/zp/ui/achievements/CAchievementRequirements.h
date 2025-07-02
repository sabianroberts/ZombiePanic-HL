//========= Copyright (c) 2025 Monochrome Games, All rights reserved. ============//

#ifndef ACHLIST_REQUIREMENT_H
#define ACHLIST_REQUIREMENT_H

#ifdef _WIN32
#pragma once
#endif

#include <utllinkedlist.h>
#include <utlvector.h>
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ButtonImage.h>

class KeyValues;

namespace vgui2
{
	class CAchievementRequirements;

	class CAchievementRequirementsHolder : public vgui2::Panel
	{
	    DECLARE_CLASS_SIMPLE(CAchievementRequirementsHolder, Panel);
	    CAchievementRequirementsHolder(vgui2::Panel *parent, char const *panelName);
	    void AddItem(bool obtained, const char *text);
	    void OnCommand(const char *szCommand);
	    void UpdateSize(bool bExpand);
	private:
	    CAchievementRequirements *m_pList;
	    ButtonImage *m_pButton;
	    Label *m_pLabel;
	    bool m_bShouldExpand;
	};

	class CAchievementRequirements : public vgui2::EditablePanel
	{
		DECLARE_CLASS_SIMPLE(CAchievementRequirements, Panel);

	public:
		CAchievementRequirements(vgui2::Panel *parent, char const *panelName);
		~CAchievementRequirements();

		// DATA & ROW HANDLING
		// The list now owns the panel
	    virtual int AddItem(bool obtained, const char *text);
		int	GetItemCount() const;
		int GetItemIDFromRow(int nRow) const;

		// Iteration. Use these until they return InvalidItemID to iterate all the items.
		int FirstItem() const;
		int NextItem(int nItemID) const;
		int InvalidItemID() const;

		virtual vgui2::Panel *GetItemLabel(int itemID);
		virtual vgui2::Panel *GetItemPanel(int itemID);

		vgui2::ScrollBar*  GetScrollbar() { return m_vbar; }

		virtual void RemoveItem(int itemID); // removes an item from the table (changing the indices of all following items)
		virtual void DeleteAllItems(); // clears and deletes all the memory used by the data items
		void RemoveAll();

		// painting
		virtual vgui2::Panel *GetCellRenderer(int row);

		// layout
		void SetFirstColumnWidth(int width);
		int GetFirstColumnWidth();
		void SetNumColumns(int iNumColumns);
		int GetNumColumns(void);
		void MoveScrollBarToTop();

		// selection
	    void SetSelectedPanel(vgui2::Panel *panel);
	    vgui2::Panel *GetSelectedPanel();
		/*
		On a panel being selected, a message gets sent to it
		"PanelSelected"		int "state"
		where state is 1 on selection, 0 on deselection
		*/

		void		SetVerticalBufferPixels(int buffer);

		void		ScrollToItem(int itemNumber);

		CUtlVector< int > *GetSortedVector(void)
		{
			return &m_SortedItems;
		}

		int	ComputeVPixelsNeeded();

	protected:
		// overrides
		virtual void OnSizeChanged(int wide, int tall);
		MESSAGE_FUNC_INT(OnSliderMoved, "ScrollBarSliderMoved", position);
		virtual void PerformLayout();
		virtual void ApplySchemeSettings(vgui2::IScheme *pScheme);
		virtual void OnMouseWheeled(int delta);

	private:


		enum { DEFAULT_HEIGHT = 24, PANELBUFFER = 5 };

		typedef struct dataitem_s
		{
			// Always store a panel pointer
			vgui2::Panel *texture;
			vgui2::Panel *title;
		} DATAITEM;

		// list of the column headers

		CUtlLinkedList<DATAITEM, int>		m_DataItems;
		CUtlVector<int>						m_SortedItems;

		vgui2::ScrollBar				*m_vbar;
		vgui2::Panel					*m_pPanelEmbedded;

		vgui2::PHandle					m_hSelectedItem;
		int						m_iFirstColumnWidth;
		int						m_iNumColumns;
		int						m_iDefaultHeight;
		int						m_iPanelBuffer;

		CPanelAnimationVar(bool, m_bAutoHideScrollbar, "autohide_scrollbar", "0");
	};

}
#endif // ACHIVLIST_H
