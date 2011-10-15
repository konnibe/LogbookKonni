#pragma once
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/textfile.h> 
#include <wx/grid.h>
#include <wx/arrstr.h>

class LogbookDialog;

class Maintenance
{
public:
	enum fieldsRepairs{RPRIORITY,RTEXT};
	enum fieldsBuyParts{PPRIORITY,PCATEGORY,TITLE,PARTS,DATE,AT};
	enum fieldsService{PRIORITY,TEXT,IF,WARN,URGENT,START,ACTIVE};
public:
	Maintenance(LogbookDialog* d, wxString data, wxString lay, wxString layoutODT);
	~Maintenance(void);

	void addLine();
	void addLineRepairs();
	void addLineBuyParts();
	void setRowDone(int row);
	void setRepairDone(int row);
	void cellCollChanged(int col, int row);
    void cellSelected(int col, int row);
    void checkService(int row);
    void checkRepairs();
    void checkBuyParts();
	void loadData();
	void update();
	void updateRepairs();
	void updateBuyParts();
	void buyParts(int i);
	void setLayoutLocation();
	wxString toODT(int tab,wxString path,wxString layout,bool mode);
	void viewODT(int tab,wxString path,wxString layout,bool mode);
	wxString toODTRepairs(wxString path,wxString layout,bool mode);
	void viewODTRepairs(wxString path,wxString layout,bool mode);
	wxString toODTBuyParts(wxString path,wxString layout,bool mode);
	void viewODTBuyParts(wxString path,wxString layout,bool mode);
	void viewHTML(int tab,wxString path,wxString layout,bool mode);
	wxString toHTML(int tab,wxString path,wxString layout,bool mode);

	wxGrid*		grid;
	wxGrid*		buyparts;
	wxGrid*		repairs;
	int			lastRow;
	int			lastRowBuyParts;
	int			lastRowRepairs;
	int			selectedCol;
	int			selectedRow;
	int			selectedRowRepairs;
	int			selectedColRepairs;
	int			selectedRowBuyParts;
	int			selectedColBuyParts;
	wxString	layout_locn;
	wxString	layout_locnService;
	wxString	layout_locnRepairs;
	wxString	layout_locnBuyParts;

private:
//enum fields{ REEF,MREMARKS};	
	LogbookDialog*	dialog;

	void setRowBackground(int row, wxColour &c);
	void setRowBackgroundRepairs(int row, wxColour &c);
	void setRowBackgroundBuyParts(int row, wxColour &c);
	void setAlignmentService();
	void setAlignmentRepairs();
	void setAlignmentBuyParts();
	void setBuyPartsPriority(wxGrid *grid ,int row, int col, int text);
	wxString readLayoutHTML(wxString path,wxString layout);
	wxString readLayoutODT(wxString path,wxString layout);
	wxString setPlaceHoldersService(bool mode, wxGrid *grid, int row, wxString middleODT);
	wxString setPlaceHoldersRepairs(bool mode, wxGrid *grid, int row, wxString middleODT);
	wxString setPlaceHoldersBuyParts(bool mode, wxGrid *grid, int row, wxString middleODT);
	wxString replaceNewLine(bool mode, wxString str);

	wxColour green;
	wxColour red;
	wxColour redlight;
	wxColour yellow;
	wxColour yellowlight;
	wxColour white;
	
	wxString		layout;
	wxString		ODTLayout;

	wxArrayString	m_choices;
	wxArrayString	m_YesNo;
	wxArrayString	m_Priority;

	wxString		data_locn;
	wxString		data_locnRepairs;
	wxString		data_locnBuyParts;
};
