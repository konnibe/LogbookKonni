#pragma once
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/textfile.h> 
#include <wx/grid.h>

#define CREWFIELDS 13 

class LogbookDialog;
class AutomaticWatch;

class CrewList
{
public:
	CrewList(LogbookDialog* d, wxString data, wxString lay, wxString layoutODT);
	~CrewList(void);

	void addCrew(wxGrid* grid, wxGrid* wake);
	void changeCrew(wxGrid* grid, int row, int col, int offset);
	void changeCrewWake(wxGrid* grid, int row, int col, int offset);
	void saveCSV(wxString path);
	void saveHTML(wxString path,wxString layout, bool mode);
	void saveODT(wxString path,wxString layout, bool mode);
	void saveXML(wxString path);
	void saveODS(wxString path);
	void backup(wxString path);
	void viewHTML(wxString path,wxString layout);
	void viewODT(wxString path,wxString layout);
	void deleteRow(int row);
	void saveData();
	void setLayoutLocation(wxString loc);
	void showAutomaticWatchDlg();

	wxString		layout_locn;
	wxString		layout;
	wxString		ODTLayout;
	
	bool modified;

private:
	void loadData();
	wxString readLayout(wxString layoutFileName);
	wxString readLayoutODT(wxString layoutFileName);
	wxString replacePlaceholder(wxString html, wxString s, bool ngrid, int row, int col, bool mode);
	void setWatches(AutomaticWatch* dlg, wxString time);
	bool checkHourFormat(wxString s, int row, int col, wxDateTime* dt);

	wxTextFile* crewListFile;
	

private:
	wxGrid*			gridCrew;
	wxGrid*			gridWake;
	wxArrayString	crewFields;
	LogbookDialog*	dialog;
	wxString		data_locn;
	wxString 		html_locn;
	wxString		ODT_locn;

	enum fields{NAME,BIRTHNAME,FIRSTNAME,TITLE,BIRTHDATE,BIRTHPLACE,NATIONALITY,
				PASSPORT,EST_IN,EST_ON,ZIP,COUNTRY,TOWN,STREET};
	enum fieldsWake{LWNAME,LWFIRSTNAME,WAKESTART1,WAKEEND1,WAKESTART2,WAKEEND2,WAKESTART3,WAKEEND3,WAKESTART4,WAKEEND4,WAKESTART5,WAKEEND5,WAKESTART6,WAKEEND6};
};

/////////////////////////////// Automatic Watchlist ///////////////
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/listctrl.h>
#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/statline.h>

class AutomaticWatch : public wxDialog 
{
	private:
	LogbookDialog* parent;
	wxString       m_ctrlText;	

	void setStrings(int i);

	protected:
		wxStaticText* m_staticText87;
		wxStaticLine* m_staticline27;
		wxStaticText* m_staticText86;
		wxStaticLine* m_staticline28;
		wxStaticText* m_staticText80;
		wxStaticText* m_staticText82;
		wxStaticText* m_staticText85;
		wxStaticLine* m_staticline26;
		wxStdDialogButtonSizer* m_sdbSizer4;
		wxButton* m_sdbSizer4OK;
		wxButton* m_sdbSizer4Cancel;
		
		// Virtual event handlers, overide them in your derived class
		void OnInit( wxInitDialogEvent& event );
		void OnListBeginDrag( wxListEvent& event );
		void OnChoice( wxCommandEvent& event );
		
	
	public:
		wxListCtrl* m_listCtrlWatchNames;
		wxChoice* m_choice20;
		wxStaticText* m_staticTextLengthWatch;
		wxStaticText* m_staticTextPersons;
		wxTextCtrl* m_textCtrlStartTime;		
		
		int dndIndex;
		
		AutomaticWatch( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Automatic Watch Interval"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 398,339 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~AutomaticWatch();
	
};

#include "wx/dnd.h"
/////////// Drag n Drop for Automatic Watchlist //////////
class AutomaticWatch;

class DnDText : public wxTextDropTarget
{
public:
    DnDText(wxListCtrl *pOwner, AutomaticWatch* aWatch) { m_pOwner = pOwner; watch = aWatch;}

    bool virtual OnDropText(wxCoord x, wxCoord y, const wxString& text);

private:
    wxListCtrl *m_pOwner;
    AutomaticWatch* watch;
};
