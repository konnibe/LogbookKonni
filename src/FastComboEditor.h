#ifndef _FASTCOMBOEDITOR_H_INCLUDED_
#define _FASTCOMBOEDITOR_H_INCLUDED_

#include "wx/grid.h"

class wxSComboBox;

class wxFastComboEditor : public wxGridCellEditor
{
public:
	wxFastComboEditor(size_t count = 0,
		const wxString choices[] = NULL,	bool allowOthers = FALSE);
	~wxFastComboEditor(void){}
	void Create(wxWindow* parent,
							wxWindowID id,
							wxEvtHandler* evtHandler);
	void PaintBackground(const wxRect& rectCell, wxGridCellAttr *attr);

	void BeginEdit(int row, int col, wxGrid* grid);
	bool EndEdit(int row, int col, wxGrid* grid);

	void Reset();

	// parameters string format is "item1[,item2[...,itemN]]"
	void SetParameters(const wxString& params);
	void SetParameters(size_t count, const wxString choices[]);
	wxGridCellEditor *Clone() const;
	wxString GetValue() const;
	wxSComboBox *Combo() const { return (wxSComboBox*)m_control; }	
	
protected:
	wxPoint m_pointActivate;
	wxString        m_startValue;
	wxArrayString   m_choices;
	bool            m_allowOthers;
};


#endif // _FASTCOMBOEDITOR_H_INCLUDED_
