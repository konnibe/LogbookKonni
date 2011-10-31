///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __LOGBOOKDIALOG__
#define __LOGBOOKDIALOG__

#include "CrewList.h"
#include "Maintenance.h"
#include "boat.h"
#include "OverView.h"
#include "EzGrid.h"

#include <wx/string.h>
#include <wx/button.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/grid.h>
#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/splitter.h>
#include <wx/dialog.h>
#include <wx/filepicker.h>
#include <wx/radiobox.h>
#include <wx/calctrl.h>
#include <wx/tglbtn.h>
#include <wx/mimetype.h>
#include <wx/listctrl.h>
#include <wx/gdicmn.h>

///////////////////////////////////////////////////////////////////////////

#define LOGGRIDS 3
#define DELETE_ROW   500
#define SELECT_ROUTE 501
#define MENUSAILS 502
#define GPSTIMEOUT 5000

///////////////////////////////////////////////////////////////////////////////
/// Class LogbookDialog
///////////////////////////////////////////////////////////////////////////////
class boat;
class Logbook;
class logbookkonni_pi;

class LogbookDialog : public wxDialog 
{
	private:
enum fields{ ROWHIGHT,ROUTE,RDATE,RTIME,WAKE,DISTANCE,POSITION,COG,SOG,REMARKS,BARO,WIND,WSPD,CURRENT,CSPD,
			 WEATHER,CLOUDS,VISIBILITY,MOTOR,FUEL,SAILS,REEF,MREMARKS };	

		wxString columns[92];
		wxString			clouds[10];

		wxString			saveDialogFilter;
		int					lastRowSelectedService;
		int					lastRowSelectedRepairs;
		int					lastRowSelectedBuyParts;

		void				declareExportHeader();
	
	public:
		Boat*			boat;
		CrewList*		crewList;
		Maintenance*	maintenance;

		wxNotebook* m_logbook;
		wxPanel* m_panel2;
		wxButton* m_button4;
		wxButton* logSave;
		wxStaticLine* m_staticline8;
		wxStaticText* m_staticText32;
		wxChoice* logbookChoice;
		wxButton* logView;
		wxStaticLine* m_staticline7;
		wxButton* newLogbook;
		wxButton* selLogbook;
		wxStaticLine* m_staticline1;
		wxNotebook* m_notebook8;
		wxPanel* m_panel6;
		wxGrid* m_gridGlobal;
		wxMenu* m_menu1;
		wxPanel* m_panel7;
		wxGrid* m_gridWeather;
		wxPanel* m_panel71;
		wxGrid* m_gridMotorSails;
		wxPanel* m_panel142;
		wxButton* logSaveOverview;
		wxStaticLine* m_staticline81;
		wxStaticText* m_staticText323;
		wxButton* logViewOverview;
		wxStaticLine* m_staticline71;
		wxStaticLine* m_staticline11;
		wxGrid* m_gridOverview;
		wxMenu* m_menuOverView;
		wxPanel* m_panel21;
		wxButton* crewAdd;
		wxButton* crewSave;
		wxStaticLine* m_staticline5;
		wxStaticText* m_staticText321;
		wxChoice* crewChoice;
		wxComboBox* m_comboBox1;
		wxButton* crewView;
		wxStaticLine* m_staticline2;
		wxGrid* m_gridCrew;
		wxMenu* m_menu2;
		wxGrid* m_gridCrewWake;
		wxPanel* m_panel3;
		wxButton* addEquipmentButton;
		wxButton* boatSave;
		wxStaticLine* m_staticline4;
		wxStaticText* m_staticText322;
		wxChoice* boatChoice;
		wxButton* boatView;
		wxStaticLine* m_staticline3;
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel72;
		wxStaticText* bname;
		wxTextCtrl* boatName;
		wxStaticText* m_staticText114;
		wxTextCtrl* homeport;
		wxStaticText* m_staticText115;
		wxTextCtrl* callsign;
		wxStaticText* m_staticText116;
		wxTextCtrl* hin;
		wxStaticText* m_staticText117;
		wxTextCtrl* sailno;
		
		
		wxStaticText* m_staticText119;
		wxTextCtrl* insurance;
		wxStaticText* m_staticText118;
		wxTextCtrl* registration;
		wxStaticText* m_staticText120;
		wxTextCtrl* policy;
		wxStaticText* m_staticText53;
		wxTextCtrl* mmsi;
		wxStaticText* m_staticText90;
		wxTextCtrl* ownerName;
		wxStaticText* m_staticText91;
		wxTextCtrl* ownerFirstname;
		wxStaticText* m_staticText95;
		wxTextCtrl* ownerPhone;
		
		
		
		
		wxStaticText* m_staticText92;
		wxTextCtrl* ownerStreet;
		wxStaticText* m_staticText93;
		wxTextCtrl* owenerZip;
		wxStaticText* m_staticText94;
		wxTextCtrl* ownerTown;
		wxStaticText* m_staticText128;
		wxTextCtrl* boatType;
		wxStaticText* m_staticText125;
		wxTextCtrl* builder;
		wxStaticText* m_staticText124;
		wxTextCtrl* hull;
		wxStaticText* m_staticText126;
		wxTextCtrl* dateLaunched;
		wxStaticText* m_staticText127;
		wxTextCtrl* yardNr;
		wxStaticText* m_staticText123;
		wxTextCtrl* designer;
		wxStaticText* m_staticText129;
		wxTextCtrl* construction;
		
		
		
		
		
		
		wxStaticText* m_staticText106;
		wxTextCtrl* LOA;
		wxStaticText* m_staticText107;
		wxTextCtrl* LOD;
		wxStaticText* m_staticText108;
		wxTextCtrl* LWL;
		wxStaticText* m_staticText109;
		wxTextCtrl* beam;
		wxStaticText* m_staticText110;
		wxTextCtrl* draft;
		wxStaticText* m_staticText111;
		wxTextCtrl* GRT;
		wxStaticText* m_staticText113;
		wxTextCtrl* NRT;
		wxStaticText* m_staticText121;
		wxTextCtrl* thames;
		wxStaticText* m_staticText122;
		wxTextCtrl* displacement;
		wxPanel* m_panel8;
		wxGrid* m_gridEquipment;
		wxMenu* m_menu3;
		wxPanel* m_panel14;
		wxStaticLine* m_staticline151;
		wxStaticText* m_staticText621;
		
		wxStaticLine* m_staticline141;
		wxStaticText* m_staticText601;
		wxMenu* m_menu7;
		wxPanel* m_panel141;
		wxStaticLine* m_staticline1513;
		wxStaticText* m_staticText6213;
		
		wxStaticLine* m_staticline1413;
		wxStaticText* m_staticText6013;
		wxMenu* m_menu71;
		wxPanel* m_panel16;
		wxStaticLine* m_staticline1511;
		wxStaticText* m_staticText6211;
		
		wxStaticLine* m_staticline1411;
		wxStaticText* m_staticText6011;
		wxMenu* m_menu711;
		wxPanel* m_panel15;
		wxStaticLine* m_staticline1512;
		wxStaticText* m_staticText6212;
		
		wxStaticLine* m_staticline1412;
		wxStaticText* m_staticText6012;
		wxCalendarCtrl* m_calendar1;
		
		// Virtual event handlers, overide them in your derived class
		virtual void LogbookDialogOnClose( wxCloseEvent& event ) ;
		virtual void LogbookDialogOnInitDialog( wxInitDialogEvent& event ) ;
		virtual void OnNoteBookPageChangedLogbook( wxNotebookEvent& event );
		virtual void m_button4OnButtonClick( wxCommandEvent& event ) ;
		virtual void logSaveOnButtonClick( wxCommandEvent& event ) ;
		virtual void onButtonReloadLayoutLogbook( wxCommandEvent& event ) ;
		virtual void onButtonClickEditLayoutLogbook( wxCommandEvent& event ) ;
		virtual void onRadioButtonHTML( wxCommandEvent& event ) ;
		virtual void onRadioButtonODT( wxCommandEvent& event ) ;
		virtual void logViewOnButtonClick( wxCommandEvent& event ) ;
		virtual void newLogbookOnButtonClick( wxCommandEvent& event ) ;
		virtual void selLogbookOnButtonClick( wxCommandEvent& event ) ;
		virtual void m_gridGlobalOnGridCellRightClick( wxGridEvent& event ) ;
		virtual void m_gridGlobalOnGridCmdCellChange( wxGridEvent& event ) ;
		virtual void m_gridGlobalOnGridSelectCell( wxGridEvent& event ) ;
		virtual void m_gridGlobalOnKeyDown( wxKeyEvent& event ) ;
		virtual void m_TimerOnMenuSelection( wxCommandEvent& event ) ;
		virtual void m_menuItem1OnMenuSelection( wxCommandEvent& event ) ;
		virtual void m_gridWeatherOnGridCellRightClick( wxGridEvent& event ) ;
		virtual void m_gridWeatherOnGridCmdCellChange( wxGridEvent& event ) ;
		virtual void m_gridWeatherOnGridSelectCell( wxGridEvent& event ) ;
		virtual void m_gridWeatherOnKeyDown( wxKeyEvent& event ) ;
		virtual void m_gridMotorSailsOnGridCellRightClick( wxGridEvent& event ) ;
		virtual void m_gridMotorSailsOnGridCmdCellChange( wxGridEvent& event ) ;
		virtual void m_gridMotorSailsOnGridSelectCell( wxGridEvent& event ) ;
		virtual void m_gridMotorSailsOnKeyDown( wxKeyEvent& event ) ;
		virtual void crewAddOnButtonClick( wxCommandEvent& event ) ;
		virtual void crewSaveOnButtonClick( wxCommandEvent& event ) ;
//		virtual void test( wxCommandEvent& event ) ;
		virtual void onButtonClickReloadLayoutsCrew( wxCommandEvent& event ) ;
		virtual void onButtonClickEditLayoutCrew( wxCommandEvent& event ) ;
		virtual void onRadioButtonHTMLCrew( wxCommandEvent& event ) ;
		virtual void onRadioButtonODTCrew( wxCommandEvent& event ) ;
		virtual void crewViewOnButtonClick( wxCommandEvent& event ) ;
		virtual void m_gridCrewOnGridCellChange( wxGridEvent& event ) ;
		virtual void m_gridCrewOnGridCellRightClick( wxGridEvent& event ) ;
		virtual void m_menuItem2OnMenuSelection( wxCommandEvent& event ) ;
		virtual void m_gridCrewWakeOnGridCellChange( wxGridEvent& event ) ;
		virtual void boataddEquipmentButtonOnButtonClick( wxCommandEvent& event ) ;
		virtual void boatSaveOnButtonClick( wxCommandEvent& event ) ;
		virtual void onButtonClickReloadLayoutsBoat( wxCommandEvent& event ) ;
		virtual void onButtonClickEditLayoutBoat( wxCommandEvent& event ) ;
		virtual void onRadioButtonHTMLBoat( wxCommandEvent& event ) ;
		virtual void onRadioButtonODTBoat( wxCommandEvent& event ) ;
		virtual void boatViewOnButtonClick( wxCommandEvent& event ) ;
		virtual void boatNameOnTextEnter( wxCommandEvent& event ) ;
		virtual void m_gridEquipmentOnGridCellChange( wxGridEvent& event ) ;
		virtual void m_gridEquipmentOnGridCellRightClick( wxGridEvent& event ) ;
		virtual void m_menuItem3OnMenuSelection( wxCommandEvent& event ) ;
		virtual void onButtobClickAddLineService( wxCommandEvent& event ) ;
		virtual void onButtobClickSaveService( wxCommandEvent& event ) ;
		virtual void onButtonClickReloadLayoutsServiceHTML( wxCommandEvent& event ) ;
		virtual void onButtonClickEditLayoutODTService( wxCommandEvent& event ) ;
		virtual void onRadioButtonHTMLService( wxCommandEvent& event ) ;
		virtual void onRadioButtonODTService( wxCommandEvent& event ) ;
		virtual void onButtonClickViewService( wxCommandEvent& event ) ;
		virtual void onGridCellServiceChange( wxGridEvent& event ) ;
		virtual void onGridCellRightClickService( wxGridEvent& event ) ;
		virtual void onGridCellServiceSelected( wxGridEvent& event ) ;
		virtual void OnKeyDownService( wxKeyEvent& event ) ;
		virtual void onMenuSelectionServiceOK( wxCommandEvent& event ) ;
		virtual void onMenuSelectionServiceBuyParts( wxCommandEvent& event ) ;
		virtual void onMenuSelectionServiceDelete( wxCommandEvent& event ) ;
		virtual void onButtobClickAddLineRepairs( wxCommandEvent& event ) ;
		virtual void onButtobClickSaveRepairs( wxCommandEvent& event ) ;
		virtual void onButtonClickReloadLayoutsRepairsHTML( wxCommandEvent& event ) ;
		virtual void onButtonClickEditLayoutODTRepairs( wxCommandEvent& event ) ;
		virtual void onRadioButtonHTMLRepairs( wxCommandEvent& event ) ;
		virtual void onRadioButtonODTRepairs( wxCommandEvent& event ) ;
		virtual void onButtonClickViewRepairs( wxCommandEvent& event ) ;
		virtual void onGridCellRepairsChange( wxGridEvent& event ) ;
		virtual void onGridCellRightClickRepairs( wxGridEvent& event ) ;
		virtual void onGridCellRepairsSelected( wxGridEvent& event ) ;
		virtual void OnKeyDownRepairs( wxKeyEvent& event ) ;
		virtual void onMenuSelectionRepairsOK( wxCommandEvent& event ) ;
		virtual void onMenuSelectionRepairsBuyParts( wxCommandEvent& event ) ;
		virtual void onMenuSelectionRepairsDelete( wxCommandEvent& event ) ;
		virtual void onButtobClickAddLineBuyParts( wxCommandEvent& event ) ;
		virtual void onButtobClickSaveBuyParts( wxCommandEvent& event ) ;
		virtual void onButtonClickReloadLayoutsBuyPartsHTML( wxCommandEvent& event ) ;
		virtual void onButtonClickEditLayoutODTBuyParts( wxCommandEvent& event ) ;
		virtual void onRadioButtonHTMLBuyParts( wxCommandEvent& event ) ;
		virtual void onRadioButtonODTBuyParts( wxCommandEvent& event ) ;
		virtual void onButtonClickViewBuyParts( wxCommandEvent& event ) ;
		virtual void onGridCellChangeBuyParts( wxGridEvent& event ) ;
		virtual void onGridCellRightClickBuyParts( wxGridEvent& event ) ;
		virtual void onGridEditorHidden( wxGridEvent& event ) ;
		virtual void onGridEditorShow( wxGridEvent& event ) ;
		virtual void onGridCellSelectedBuyParts( wxGridEvent& event ) ;
		virtual void OnKeyDownBuyParts( wxKeyEvent& event ) ;
		virtual void onMenuSelectionBuyPartsDelete( wxCommandEvent& event ) ;
		virtual void OnButtonClickOverviewSave( wxCommandEvent& event );
		virtual void onButtonReloadLayoutOverView( wxCommandEvent& event );
		virtual void onButtonClickEditLayoutOverview( wxCommandEvent& event );
		virtual void onRadioButtonHTMLOverview( wxCommandEvent& event );
		virtual void onRadioButtonODTOverView( wxCommandEvent& event );
		virtual void OnButtonClickOverView( wxCommandEvent& event );
		virtual void OnGridCellRightClickOverview( wxGridEvent& event );
		virtual void OnGridLabelLeftClickOverview( wxGridEvent& event );
		virtual void OnMenuSelectionGotoRoute( wxCommandEvent& event );
//		virtual void OnMenuSelectionViewRoute( wxCommandEvent& event );
		virtual void onButtonClickSelectLogbook( wxCommandEvent& event );
		virtual void OnRadioButtonActuellLogbook( wxCommandEvent& event );
		virtual void OnRadioButtonAllLogbooks( wxCommandEvent& event );
		virtual void onGridCellLeftClickService( wxGridEvent& event );
		virtual void onGridCellLeftClickBuyParts( wxGridEvent& event );

/*		virtual void onButtobClickAddLineMaintenance( wxCommandEvent& event ) ;
		virtual void onButtobClickSaveMaintenance( wxCommandEvent& event ) ;
		virtual void onButtonClickReloadLayoutsMaintenanceHTML( wxCommandEvent& event ) ;
		virtual void onButtonClickEditLayoutODTMaintenance( wxCommandEvent& event ) ;
		virtual void onRadioButtonHTMLMaintenance( wxCommandEvent& event ) ;
		virtual void onButtonClickViewMaintenance( wxCommandEvent& event ) ;
*/
		virtual void gridGlobalScrolled( wxScrollWinEvent& event );
		virtual void gridWeatherScrolled( wxScrollWinEvent& event );
		virtual void gridMotorSailsScrolled( wxScrollWinEvent& event );

		 void appendOSDirSlash(wxString* pString);
		 wxString setLogbookData(int rowHeight, int totalColumns, int numPages, int row);
		 void changeCellValue(int row, int col, wxGrid* grid, int offset);
		 void getIniValues();
		 void setIniValues();
	
	public:
enum maintenanceTab {SERVICE,REPAIRS,BUYPARTS};
enum FORMAT {HTML,ODT};

		wxButton* m_buttonReloadLayout;
		wxButton* m_buttonEditLayout;
		wxRadioButton* m_radioBtnHTML;
		wxRadioButton* m_radioBtnODT;
		wxChoice* overviewChoice;
		wxButton* m_buttonReloadLayoutOview;
		wxButton* m_buttonEditLayoutOview;
		wxRadioButton* m_radioBtnHTMLOverview;
		wxRadioButton* m_radioBtnODTOverview;
		wxRadioButton* m_radioBtnActuellLogbook;
		wxRadioButton* m_radioBtnAllLogbooks;
		wxButton* m_buttonSelectLogbook;
		wxButton* m_buttonReloadCrew;
		wxButton* m_buttonEditLayoutCrew;
		wxRadioButton* m_radioBtnHTMLCrew;
		wxRadioButton* m_radioBtnODTCrew;
		wxButton* m_buttonReloadLayoutsBoat;
		wxButton* m_buttonEditLayoutBoat;
		wxRadioButton* m_radioBtnHTMLBoat;
		wxRadioButton* m_radioBtnODTBoat;
		wxPanel* m_panel13;
		wxNotebook* m_notebook6;
		wxButton* m_buttonAddLineService;
		wxButton* m_buttonSaveService;
		wxChoice* m_choiceSelectLayoutService;
		wxButton* m_buttonReloadLayoutsServiceHTML;
		wxButton* m_buttonEditLayoutODTService;
		wxRadioButton* m_radioBtnHTMLService;
		wxRadioButton* m_radioBtnODTService;
		wxButton* m_buttonViewService;
		EzGrid* m_gridMaintanence;
		wxButton* m_buttonAddLineRepairs;
		wxButton* m_buttonSaveRepairs;
		wxChoice* m_choiceSelectLayoutRepairs;
		wxButton* m_buttonReloadLayoutsRepairsHTML;
		wxButton* m_buttonEditLayoutODTRepairs;
		wxRadioButton* m_radioBtnHTMLRepairs;
		wxRadioButton* m_radioBtnODTRepairs;
		wxButton* m_buttonViewRepairs;
		wxGrid* m_gridMaintanenceRepairs;
		wxButton* m_buttonAddLineBuyParts;
		wxButton* m_buttonSaveBuyParts;
		wxChoice* m_choiceSelectLayoutBuyParts;
		wxButton* m_buttonReloadLayoutsBuyPartsHTML;
		wxButton* m_buttonEditLayoutODTBuyParts;
		wxRadioButton* m_radioBtnHTMLBuyParts;
		wxRadioButton* m_radioBtnODTBuyParts;
		wxButton* m_buttonViewBuyParts;
		wxGrid* m_gridMaintenanceBuyParts;
		wxButton* m_buttonAddLineMaintenance1;
		wxButton* m_buttonSaveMaintenance2;
		wxChoice* m_choiceSelectLayoutMaintenance12;
		wxButton* m_buttonReloadLayoutsMaintenanceHTML2;
		wxButton* m_buttonEditLayoutODTMaintenance12;
		wxRadioButton* m_radioBtnHTMLMaintenance2;
		wxRadioButton* m_radioBtnODTMaintenance2;
		wxButton* m_buttonViewMaintenance2;
		wxToggleButton* m_toggleScheduleToday;
		wxToggleButton* m_toggleScheduleWeek;
		wxToggleButton* m_toggleScheduleMonth;
		wxGrid* m_gridSchedule;
		wxRadioButton* m_radioBtnSelectLogbook;

		LogbookDialog( logbookkonni_pi* d, wxTimer* t, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Active Logbook"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1010,535 ), long style = wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER);
		~LogbookDialog();
		
		void m_gridGlobalOnContextMenu( wxMouseEvent &event )
		{
			m_gridGlobal->PopupMenu( m_menu1, event.GetPosition() );
		}

		void m_gridServiceOnContextMenu( wxMouseEvent &event )
		{
			m_gridMaintanence->PopupMenu( m_menu7, event.GetPosition() );
		}
		
		void m_splitter1OnIdle( wxIdleEvent& )
		{
			m_splitter1->SetSashPosition( 310 );
			m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( LogbookDialog::m_splitter1OnIdle ), NULL, this );
		}

		void writeToLogbook();
		void setTitleExt();
		wxString replaceDangerChar(wxString s);
		wxString restoreDangerChar(wxString s);
		void startBrowser(wxString filename);
		void startApplication(wxString filename, wxString ext);
		void loadLayoutChoice(wxString path, wxChoice* choice);
		void setCellAlign(int count);
		void setEqualRowHeight(int row);
		void init();
		void OnTimerGPS(wxTimerEvent& ev);
		int  showLayoutDialog(wxChoice *choice, wxString location, int format);
		void filterLayout(wxChoice *choice, wxString location, int format);
		bool isInArrayString(wxArrayString ar, wxString s);
		wxDateTime getDateTo(wxString filename);

		logbookkonni_pi*	logbookPlugIn;
		wxString*			pHome_Locn;
		Logbook*			logbook;
		OverView*			overview;

		wxMenuItem*			menuItemTimer;
		wxString			layoutHTML;
		wxString			layoutODT;
		wxTimer*			timer;
		wxTimer*			GPSTimer;

		wxColour			defaultBackground;
		int					gridGlobalCol;
		int					gridWeatherCol;
		int					gridMotorSailsCol;

		int					selGridRow;
		int					selGridCol;

		wxGrid				*logGrids[LOGGRIDS];
		int					numPages;
		int					totalColumns;
		wxFont				font;

		// ODT-Strings for Export
		wxString			content;
		wxString			contentEnd;
		wxString			manifest;
		wxString			styles;
		wxString			meta;

		// XML-String for Export Excel
		wxString			xmlHead;
		wxString			xmlEnd;

		wxString			titleExt;
};
/*
class LogGridTable : public wxGridTableBase
{
public:
	LogGridTable(long sizeGridRows, long sizeGridCols) { m_sizeGridRows = sizeGridRows;  m_sizeGridCols = sizeGridCols;}
//	~LogGridTable();

	bool AppendRows(size_t numRows = 1 ){ return true;}
    int GetNumberRows() { return m_sizeGridRows; }
    int GetNumberCols() { return m_sizeGridCols; }
    wxString GetValue( int row, int col )
    {
        return wxString::Format(wxT("(%d, %d)"), row, col);
    }

    void SetValue( int row, int col, const wxString& s  ) {  ignore  }
    bool IsEmptyCell( int , int  ) { return false; }

private:
    long m_sizeGridRows;
	long m_sizeGridCols;

};

*/


///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class RouteDialog
///////////////////////////////////////////////////////////////////////////////
class RouteDialog : public wxDialog 
{
	private:
	
	protected:
	
	public:
		wxListCtrl* m_listCtrlRoute;
		wxStdDialogButtonSizer* m_sdbSizer6;
		wxButton* m_sdbSizer6OK;
		wxButton* m_sdbSizer6Cancel;
		
		RouteDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Select Route"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 269,318 ), long style = wxDEFAULT_DIALOG_STYLE );
		~RouteDialog();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class LayoutDialog
///////////////////////////////////////////////////////////////////////////////
class LayoutDialog : public wxDialog 
{
	private:
	
	protected:
		wxBitmapButton* m_bpButtonLoadLayout;
		wxStdDialogButtonSizer* m_sdbSizer3;
		wxButton* m_sdbSizer3OK;
		wxButton* m_sdbSizer3Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnButtonClickLoadLayout( wxCommandEvent& event );
		
	
	public:
		wxRadioButton* m_radioBtnEdit;
		wxRadioButton* m_radioBtnFilter;
		wxRadioButton* m_radioBtnRename;
		wxTextCtrl* m_textCtrlRename;
		wxRadioButton* m_radioBtnDelete;
		wxRadioButton* m_radioBtnEmail;
		wxRadioButton* m_radioBtnInstall;
		wxChoice* m_choice15;
		wxString layoutPath;
		wxString layoutFileName;
		LogbookDialog *dialog;
#ifdef _WXMSW__		
		LayoutDialog( wxWindow* parent, wxString location, wxChoice* choice,wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 307,205 ), long style = wxDEFAULT_DIALOG_STYLE );
#else
		LayoutDialog( wxWindow* parent, wxString location, wxChoice* choice,wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 307,230 ), long style = wxDEFAULT_DIALOG_STYLE );
		
#endif
		~LayoutDialog();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SelectLogbookDialog
///////////////////////////////////////////////////////////////////////////////
class SelectLogbook : public wxDialog 
{
	private:
		wxString path;
		LogbookDialog* parent;
		wxDateTime getDateTo(wxString filename);
	
	protected:
		wxStdDialogButtonSizer* m_sdbSizer4;
		wxButton* m_sdbSizer4OK;
		wxButton* m_sdbSizer4Cancel;
	
	public:
		wxListCtrl* m_listCtrlSelectLogbook;
		wxArrayString files;

		virtual void OnInit( wxInitDialogEvent& event );
#ifdef __WXMSW__		
		SelectLogbook( wxWindow* parent, wxString path, wxWindowID id = wxID_ANY, const wxString& title = _("Select Logbook"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 297,252), long style = wxDEFAULT_DIALOG_STYLE );
#else
		SelectLogbook( wxWindow* parent, wxString path, wxWindowID id = wxID_ANY, const wxString& title = wxT("Select Logbook"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 297,260), long style = wxDEFAULT_DIALOG_STYLE );		
#endif
		~SelectLogbook();
	
};

#endif //__logbook__
