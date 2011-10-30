#ifndef WX_PRECOMP
      #include <wx/wx.h>
#endif
#include "EzGrid.h"
#include "FastComboEditor.h"
#include <wx/config.h>


BEGIN_EVENT_TABLE(EzGrid, wxGrid)
	EVT_GRID_CELL_LEFT_CLICK(EzGrid::OnCellLeftClick )
#ifdef __WXGTK__
    EVT_MOUSEWHEEL(EzGrid::OnMouseWheel)
#endif
END_EVENT_TABLE()


EzGrid::EzGrid(wxWindow *parent,
	wxWindowID id,	const wxPoint& pos,
	const wxSize& size, long style,
	const wxString& name) : wxGrid(parent, id, pos, size, style, name),
	m_selTemp(NULL)
{
	// Adjust the default row height to be more compact
	wxFont font = GetLabelFont();
	int nWidth = 0;
	int nHeight = 18;
	GetTextExtent(_T("W"), &nWidth, &nHeight, NULL, NULL, &font);
	SetColLabelSize(nHeight+6);
	#ifdef __WXGTK__
	SetDefaultRowSize(nHeight+8, TRUE);
	#else
	SetDefaultRowSize(nHeight+4, TRUE);
	#endif
}

EzGrid::~EzGrid()
{
	// Save column widths
	wxConfigBase *cfg = wxConfigBase::Get();
	wxString strCol;
	for (int nCol = 0; nCol < GetNumberCols(); nCol++)
	{
		strCol.Printf(_T("column%d"), nCol);
		cfg->Write(strCol, (long)GetColSize(nCol));
	}
}

#include <wx/window.h>
#include <typeinfo>
void EzGrid::OnCellLeftClick(wxGridEvent& ev)
{

	// Store the click co-ordinates in the editor if possible
	// if an editor has created a ClientData area, we presume it's
	// a wxPoint and we store the click co-ordinates
	wxGridCellEditor* pEditor  = GetCellEditor(ev.GetRow(), ev.GetCol());
	wxFastComboEditor* ed = NULL;
	ed = dynamic_cast<wxFastComboEditor*>(pEditor);

	if (ed) 
	{
		m_waitForSlowClick = TRUE;
		SetGridCursor(ev.GetRow(), ev.GetCol());
		wxPoint* pClickPoint = (wxPoint*)pEditor->GetClientData();
		if (pClickPoint)
		{
			*pClickPoint = ClientToScreen(ev.GetPosition());
#ifndef __WXMSW__
			EnableCellEditControl(true);
#endif
		}

	// hack to prevent selection from being lost when click combobox

//	if (ev.GetCol() == 0 )//&& IsInSelection(ev.GetRow(), ev.GetCol()))
//	{
//		m_selTemp = m_selection;
//		m_selection = NULL;
//	}
	
//	pEditor->DecRef();
	}
	ev.Skip();
}

#ifdef __WXGTK__
void EzGrid::OnMouseWheel( wxMouseEvent& event )
{
    int nWheelRotation = event.GetWheelRotation();
//    int lines = nWheelRotation / event.GetWheelDelta();
	int x, y;
	GetViewStart(&x, &y);
	if (nWheelRotation < 0) y+=5;
	else y -= 5;
	Scroll(x, y);
}
#endif

void EzGrid::PreviousState()
{
	wxConfigBase *cfg = wxConfigBase::Get();
	wxString strCol;
	long nColWidth;
	for (int nCol = 0; nCol < GetNumberCols(); nCol++)
	{
		strCol.Printf(_T("column%d"), nCol);
		if (cfg->Read(strCol, &nColWidth))
			SetColSize(nCol, nColWidth);
	}
}

wxSize EzGrid::DoGetBestSize() const
{
	return wxSize(100, 80);
}

void EzGrid::SetNumberRows(int nRows)
{
	if (GetRows() > 0) DeleteRows(0, GetRows());
	if (nRows > 0) AppendRows(nRows);
}

void EzGrid::RecalcRowLabel()
{
	wxFont font = GetLabelFont();
	int nWidth = 0;
	int nHeight = 0;
	GetTextExtent(wxString::Format(_T("%d"), GetRows()),
								&nWidth, &nHeight, NULL, NULL, &font);
	if (nWidth < 12) nWidth = 12;
	SetRowLabelSize(nWidth+6);
//	SetRowLabelSize(0);
}


void EzGrid::UpdateNumRows()
{
	m_numRows = m_table->GetNumberRows();
	m_numCols = m_table->GetNumberCols();
}


