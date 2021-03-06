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
#include "tinyxml/tinyxml.h"

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
#include <wx/treectrl.h>
#include <wx/dialog.h>
#include <wx/filepicker.h>
#include <wx/radiobox.h>
#include <wx/calctrl.h>
#include <wx/tglbtn.h>
#include <wx/mimetype.h>
#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/dnd.h>

///////////////////////////////////////////////////////////////////////////

#define LOGGRIDS 3

#define DELETE_ROW				500
#define SELECT_ROUTE			501
#define MENUSAILS				502
#define SHOWHIDDENCOL			503
#define HIDDENCOLSEP			504
#define HIDECOLUMN				506
#define	MENUTIMER				505
#define ID_LOGTIMER				510
#define ID_GPSTIMER				510
#define COLDFINGER				511
#define MENUCREWALL				512
#define MENUCREWONBOARD			513


#define GPSTIMEOUT 5000
#define LOGSAVETIME 600000
///////////////////////////////////////////////////////////////////////////////
/// Class LogbookDialog
///////////////////////////////////////////////////////////////////////////////
class boat;
class Logbook;
class logbookkonni_pi;
class ColdFinger;

class LogbookDialog : public wxDialog 
{
	private:
		wxString columns[92];
		wxString			clouds[10];
		ColdFinger*         coldfinger;

		wxString			saveDialogFilter;
		int					lastRowSelectedService;
		int					lastRowSelectedRepairs;
		int					lastRowSelectedBuyParts;
		int					previousColumn;
		bool				noOpenPositionDlg;

		void				declareExportHeader();
		bool				checkHiddenColumns(wxGrid* grid,int i,bool use);
		void				navigationHideColumn(wxCommandEvent& ev);
		void				sortGrid(wxGrid* grid, int col, bool ascending);
		void				clearDataDir();
		void				addColdFingerDialog(wxMenu* m_menu1);
		void				addColdFingerTextBlocks(wxMenu* m_menu1);
		wxTreeItemId			FindMenuItem(int grid, int col, wxString name);

	public:
enum fields{ ROWHIGHT,ROUTE,RDATE,RTIME,WAKE,DISTANCE,POSITION,COG,SOG,REMARKS,
			 BARO,WIND,WSPD,CURRENT,CSPD,WEATHER,CLOUDS,VISIBILITY,
			 MOTOR,FUEL,SAILS,REEF,MREMARKS };

		Boat*			boat;
		CrewList*		crewList;
		Maintenance*	maintenance;

		wxImageList *imageList;
		
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
		wxButton* crewView;
		wxStaticLine* m_staticline2;
		wxGrid* m_gridCrew;
		wxMenu* m_menu2;
		wxGrid* m_gridCrewWake;
		wxMenu* m_menu21;
		wxPanel* m_panel3;
		wxButton* addEquipmentButton;
		wxToggleButton* m_toggleBtnShowEquipment;
		wxStaticLine* m_staticline27;
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
		wxTextCtrl* UserLabel1;
		wxTextCtrl* Userfield2;
		wxTextCtrl* Userfield3;
		wxStaticText* m_staticText122;
		wxTextCtrl* displacement;
		wxPanel* m_panel8;
		wxGrid* m_gridEquipment;
		wxMenu* m_menu3;
		wxPanel* m_panel14;
		wxStaticLine* m_staticline151;
		wxStaticText* m_staticText621;
		wxStaticLine* m_staticline141;
		wxMenu* m_menu7;
		wxPanel* m_panel141;
		wxStaticLine* m_staticline1513;
		wxStaticText* m_staticText6213;
		wxStaticLine* m_staticline1413;
		wxMenu* m_menu71;
		wxPanel* m_panel16;
		wxStaticLine* m_staticline1511;
		wxStaticText* m_staticText6211;
		wxStaticLine* m_staticline1411;
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
		virtual void OnGridLabelLeftDClickGlobal( wxGridEvent& event );
		virtual void OnGridLabelLeftDClickWeather( wxGridEvent& event );
		virtual void OnGridLabelLeftDClickMotorSails( wxGridEvent& event );
		virtual void OnGridLabelLeftDClickOverview( wxGridEvent& event );
		virtual void OnGridLabelLeftDClickCrew( wxGridEvent& event );
		virtual void OnGridLabelLeftDClickCrewWake( wxGridEvent& event );
		virtual void OnMenuSelectionHiddenCrew( wxCommandEvent& event );
		virtual void OnMenuSelectionHiddenWake( wxCommandEvent& event );
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
		virtual void OnMenuSelectionHideColumn( wxCommandEvent& event );
		virtual void OnMenuSelectionHideColumnOverView( wxCommandEvent& event );
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
		virtual void OnMenuSelectionOnboardCrew( wxCommandEvent& event );
		virtual void OnMenuSelectionAllEntriesCrew( wxCommandEvent& event );
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
		virtual void OnMenuSelectionTimerInterval( wxCommandEvent& event );
		virtual void OnMenuSelectionShowHiddenCols( wxCommandEvent& event );
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
		virtual void OnMenuSelectionShowHiddenColsOverview( wxCommandEvent& event );
		virtual void onButtonClickSelectLogbook( wxCommandEvent& event );
		virtual void OnRadioButtonActuellLogbook( wxCommandEvent& event );
		virtual void OnRadioButtonAllLogbooks( wxCommandEvent& event );
		virtual void onGridCellLeftClickService( wxGridEvent& event );
		virtual void onGridCellLeftClickBuyParts( wxGridEvent& event );
		virtual void OnToggleButtonShowEquip( wxCommandEvent& event );
		virtual void OnKeyDownOverview( wxKeyEvent& event );
		virtual void OnMenuSelectionSearch( wxCommandEvent& event );
		virtual void m_menuItem19MenuSelection( wxCommandEvent& event );		
		virtual void m_menuItem21MenuSelection( wxCommandEvent& event );		
		virtual void OnGridCellRightClickWake( wxGridEvent& event );
		virtual void OnMenuSelectionAddWatch( wxCommandEvent& event );
		virtual void OnMenuSelectionSameWatch( wxCommandEvent& event );
		virtual void OnGridLabelLeftClickService( wxGridEvent& event );
		virtual void OnGridLabelLeftClickRepairs( wxGridEvent& event );
		virtual void OnGridLabelLeftClickBuyParts( wxGridEvent& event );
		virtual void OnGridCellLeftClickGlobal( wxGridEvent& event );
		virtual void OnMenuSelectionAsc( wxCommandEvent& event );
		virtual void OnMenuSelectionDesc( wxCommandEvent& event );
		virtual void OnKeyDownCrew( wxKeyEvent& event );
		virtual void OnKeyDownWatch( wxKeyEvent& event );

		virtual void OnGridEditorShownCrew( wxGridEvent& event );

		virtual void m_menu1Highlighted(wxMenuEvent& event);

		virtual void gridGlobalScrolled( wxScrollWinEvent& event );
		virtual void gridWeatherScrolled( wxScrollWinEvent& event );
		virtual void gridMotorSailsScrolled( wxScrollWinEvent& event );

		 void appendOSDirSlash(wxString* pString);
//		 wxString setLogbookData(int rowHeight, int totalColumns, int numPages, int row);
//		 void changeCellValue(int row, int col, wxGrid* grid, int offset);
		 void getIniValues();
		 void setIniValues();
	
	public:
enum maintenanceTab {SERVICE,REPAIRS,BUYPARTS};
enum FORMAT {HTML,ODT};

		wxButton* m_buttonReloadLayout;
		wxButton* m_buttonEditLayout;
		wxRadioButton* m_radioBtnHTML;
		wxRadioButton* m_radioBtnODT;
		wxGrid* m_gridGlobal;
		wxChoice* overviewChoice;
		wxButton* m_buttonReloadLayoutOview;
		wxButton* m_buttonEditLayoutOview;
		wxRadioButton* m_radioBtnHTMLOverview;
		wxRadioButton* m_radioBtnODTOverview;
		wxRadioButton* m_radioBtnActuellLogbook;
		wxRadioButton* m_radioBtnAllLogbooks;
		wxRadioButton* m_radioBtnSelectLogbook;
		wxButton* m_buttonSelectLogbook;
		wxButton* m_buttonReloadCrew;
		wxButton* m_buttonEditLayoutCrew;
		wxRadioButton* m_radioBtnHTMLCrew;
		wxRadioButton* m_radioBtnODTCrew;
		wxButton* m_buttonReloadLayoutsBoat;
		wxButton* m_buttonEditLayoutBoat;
		wxRadioButton* m_radioBtnHTMLBoat;
		wxRadioButton* m_radioBtnODTBoat;
		wxStaticBoxSizer* sbSizer6;
		wxTextCtrl* Userfield1;
		wxTextCtrl* UserLabel2;
		wxTextCtrl* UserLabel3;
		wxStaticBoxSizer* sbSizer12;
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
		wxGrid* m_gridMaintanence;
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

		bool myParseDate(wxString s,wxDateTime  &dt);
		const wxChar* myParseTime(wxString s, wxDateTime &dt);
		void setDatePattern();

		void setTitleExt();
		wxString replaceDangerChar(wxString s);
		wxString restoreDangerChar(wxString s);
		void startBrowser(wxString filename);
		void startApplication(wxString filename, wxString ext);
		void loadLayoutChoice(wxString path, wxChoice* choice);
//		void setCellAlign(int count);
		void setEqualRowHeight(int row);
		void init();
		void OnTimerGPS(wxTimerEvent& ev);
		void OnLogTimer(wxTimerEvent& ev);
		int  showLayoutDialog(wxChoice *choice, wxString location, int format);
//		void filterLayout(wxChoice *choice, wxString location, int format);
		bool isInArrayString(wxArrayString ar, wxString s);
		wxDateTime getDateTo(wxString filename);

		logbookkonni_pi*	  logbookPlugIn;
		wxString*			  pHome_Locn;
		wxString              image_locn;
		Logbook*			  logbook;
		OverView*			  overview;

		wxString			datePattern;
		wxChar				dateSeparator;
		wxString			decimalPoint;

		wxMenuItem*			menuItemTimer;
		wxString			layoutHTML;
		wxString			layoutODT;
		wxTimer*			timer;
		wxTimer*			GPSTimer;
		wxTimer*			logbookTimer;
		int					sashPos; 

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
		
		RouteDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Select Route"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 269,318 ), long style = wxDEFAULT_DIALOG_STYLE );
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
#ifdef __WXMSW__		
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
		
		virtual void OnInit( wxInitDialogEvent& event );
		
	
	public:
		wxListCtrl* m_listCtrlSelectLogbook;
		wxArrayString files;

#ifdef __WXMSW__		
		SelectLogbook( wxWindow* parent, wxString path, wxWindowID id = wxID_ANY, const wxString& title = _("Select Logbook"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 297,252), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
#else
		SelectLogbook( wxWindow* parent, wxString path, wxWindowID id = wxID_ANY, const wxString& title = _("Select Logbook"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 297,260), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER  );		
#endif
		~SelectLogbook();
	
};

///////////////////////////////////////////////////////////////////////////
WX_DECLARE_OBJARRAY(wxArrayString, myGridStringArray);//(,
                              //class WXDLLIMPEXP_ADV);

class myGridStringTable : public wxGridTableBase
{
public:
    myGridStringTable();
    myGridStringTable( int numRows, int numCols );
    virtual ~myGridStringTable();

    // these are pure virtual in wxGridTableBase
    //
    int GetNumberRows();
    int GetNumberCols();
    wxString GetValue( int row, int col );
    void SetValue( int row, int col, const wxString& s );
    bool IsEmptyCell( int row, int col );

    // overridden functions from wxGridTableBase
    //
    void Clear();
    bool InsertRows( size_t pos = 0, size_t numRows = 1 );
    bool AppendRows( size_t numRows = 1 );
    bool DeleteRows( size_t pos = 0, size_t numRows = 1 );
    bool InsertCols( size_t pos = 0, size_t numCols = 1 );
    bool AppendCols( size_t numCols = 1 );
    bool DeleteCols( size_t pos = 0, size_t numCols = 1 );

    void SetRowLabelValue( int row, const wxString& );
    void SetColLabelValue( int col, const wxString& );
    wxString GetRowLabelValue( int row );
    wxString GetColLabelValue( int col );

    wxGridStringArray m_data;

private:


    // These only get used if you set your own labels, otherwise the
    // GetRow/ColLabelValue functions return wxGridTableBase defaults
    //
    wxArrayString     m_rowLabels;
    wxArrayString     m_colLabels;

//    DECLARE_DYNAMIC_CLASS_NO_COPY( wxGridStringTable )
};

class myTreeItem;
//////////////////// D�alog ColdFinger //////////////////////////////////
class ColdFinger : public wxDialog 
{
	private:
		LogbookDialog	*dialog;
	    bool			modified;
		wxString		dataPath;
		wxImageList*    imageList; 

		void            init();
		void			addElement(TiXmlElement* root, const char* key, const char* label);
		wxTreeItemId	recursiveWrite(wxTreeItemId id, TiXmlElement *elem);
		void			fillTree(wxTreeItemId id, TiXmlNode *elem);

	protected:
		wxSplitterWindow* m_splitter2;
		wxPanel* m_panel18;
		wxMenu* m_menu9;
		wxPanel* m_panel19;
		wxStaticText* m_staticText94;
		wxStaticText* m_staticText95;
		wxChoice* m_choice23;
		wxStaticText* m_staticText96;
		wxChoice* m_choice24;
		wxStaticLine* m_staticline32;
		wxStaticText* m_staticText97;
		wxStdDialogButtonSizer* m_sdbSizer8;
		wxButton* m_sdbSizer8OK;
		wxButton* m_sdbSizer8Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCloseCold( wxCloseEvent& event );
		virtual void OnInitDialog( wxInitDialogEvent& event );
		virtual void OnTreeBeginDragCold( wxTreeEvent& event );
		virtual void OnTreeItemRightClickCold( wxTreeEvent& event );
		virtual void OnTreeSelChanged( wxTreeEvent& event );
		virtual void OnMenuSelectionAddCold( wxCommandEvent& event );
		virtual void OnMenuSelectionDeleteCold( wxCommandEvent& event );
		virtual void OnMenuSelectionRenameCold( wxCommandEvent& event );
		virtual void OnMenuSelectionaddNodeCold( wxCommandEvent& event );
		virtual void OnMenuTreeSelectionDeleteNodeCold( wxCommandEvent& event );
		virtual void OnTextCold( wxCommandEvent& event );
		virtual void OnCancelButtonClickCold( wxCommandEvent& event );
		virtual void OnOKButtonClickCold( wxCommandEvent& event );
		
	
	public:
		enum treenodes { NODE,ITEM };
		wxTreeCtrl* m_treeCtrl3;
		wxTextCtrl* m_textCtrl73;
		myTreeItem* retItem;
		wxTreeItemId selectedItem;
		int				fo; 
		int				it;
		
		ColdFinger( LogbookDialog* parent,  wxWindowID id = wxID_ANY, const wxString& title = _("Textblocks Dialog"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 524,392 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~ColdFinger();

		void	writeTextblocks();
        void	loadTextBlocks();
		
		void m_splitter2OnIdle( wxIdleEvent& )
		{
			m_splitter2->SetSashPosition( 0 );
			m_splitter2->Disconnect( wxEVT_IDLE, wxIdleEventHandler( ColdFinger::m_splitter2OnIdle ), NULL, this );
		}
		
/*		void m_treeCtrl3OnContextMenu( wxMouseEvent &event )
		{
			m_treeCtrl3->PopupMenu( m_menu9, event.GetPosition() );
		}
*/
};

class myTreeItem : public wxTreeItemData
{
public:
	myTreeItem( int type, wxString name, wxString text, wxString guid, wxString route, wxString guidWP, wxString WP, int grid, int gridcol, bool deleteable, bool add, bool menu);
	myTreeItem( const myTreeItem* item);

	int		 type;
	wxString name;
	wxString text;
	wxString guid;
	wxString route;
	wxString guidWP;
	wxString WP;
	int      grid;
	int      gridcol;
	bool	 deleteable;
	bool     add;
	bool     menu;

};

class DnD : public wxTextDropTarget
{
public:
    DnD(ColdFinger* d, wxTreeCtrl *pOwner, wxTreeCtrl* pSender) { m_pOwner = pOwner; m_pSender = pSender; dialog = d;}

    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& str);

private:
    wxTreeCtrl *m_pOwner;
	wxTreeCtrl* m_pSender;
	ColdFinger* dialog;
};

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/spinctrl.h>
#include <wx/gdicmn.h>
////////////// Timer-Interval Dialog ///////////
class TimerInterval : public wxDialog 
{
	private:
		wxSpinCtrl* m_spinCtrlH;
		wxStaticText* m_staticTextH;
		wxSpinCtrl* m_spinCtrlM;
		wxStaticText* m_staticTextM;
		wxSpinCtrl* m_spinCtrlS;
		wxStaticText* m_staticTextS;

		Options* opt;
		LogbookDialog* dialog;
		bool timerruns;

		void init(Options* opt, LogbookDialog* dialog);
	
	protected:
		wxStdDialogButtonSizer* m_sdbSizer9;
		wxButton* m_sdbSizer9OK;
		wxButton* m_sdbSizer9Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnButtonOKClick( wxCommandEvent& event );
		
	
	public:
		
		TimerInterval( wxWindow* parent, Options* opt, wxWindowID id = wxID_ANY, const wxString& title = _("Set Timer Interval"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 219,100 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~TimerInterval();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class PositionDlg
///////////////////////////////////////////////////////////////////////////////
class PositionDlg : public wxDialog 
{
	private:
		wxTextCtrl* m_textCtrlDeg1;
		wxStaticText* m_staticTextDeg1;
		wxTextCtrl* m_textCtrlmin1;
		wxStaticText* m_staticTextmin1;
		wxTextCtrl* m_textCtrlsec1;
		wxStaticText* m_staticTextsec1;
		wxTextCtrl* m_textCtrlNS;
		wxFlexGridSizer* fgSizer441;
		wxTextCtrl* m_textCtrlDeg2;
		wxStaticText* m_staticTextDeg2;
		wxTextCtrl* m_textCtrlmin2;
		wxStaticText* m_staticTextmin2;
		wxTextCtrl* m_textCtrlsec2;
		wxStaticText* m_staticTextsec2;
		wxTextCtrl* m_textCtrlWE;
		wxChoice* m_choiceFormat;

		void           init(LogbookDialog* dlg);
		void           setFormat(int fmt);
		wxString       replaceComma(wxString s);
		LogbookDialog* dlg;
		int            oldSel;
	
	protected:
		wxStaticText* m_staticTextWE;
		wxStdDialogButtonSizer* m_sdbSizer10;
		wxButton* m_sdbSizer10OK;
		wxButton* m_sdbSizer10Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCancelButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOKButtonClick( wxCommandEvent& event );
		virtual void OnChoice( wxCommandEvent& event );
		
	
	public:
		
		PositionDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Position"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 250,127 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
	    ~PositionDlg();

		wxString retstr;
};

#endif //__logbook__
