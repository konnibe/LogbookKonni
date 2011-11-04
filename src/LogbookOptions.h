///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __logbookoptions__
#define __logbookoptions__

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/sizer.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/dialog.h>


///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class LogbookOptions
///////////////////////////////////////////////////////////////////////////////
class Options;
class logbook_pi;

class LogbookOptions : public wxDialog 
{
	private:
	
	public:
		wxNotebook* m_notebook4;
		wxPanel* m_panel15;
		
		wxCheckBox* m_checkBoxShowLogbook;
		
		
		
		wxCheckBox* m_checkBoxShowOnlySelectedLayouts;
		
		
		
		
		wxStaticText* m_staticText74;
		
		
		
		
		wxStaticLine* m_staticline25;
		wxStaticLine* m_staticline10;
		
		wxCheckBox* m_checkBoxTimer;
		
		wxStaticText* m_staticText56;
		wxTextCtrl* m_textCtrTimerH;
		wxStaticText* m_staticText58;
		wxTextCtrl* m_textCtrlTimerM;
		wxStaticText* m_staticText581;
		wxTextCtrl* m_textCtrlTimerS;
		wxStaticText* m_staticText5811;
		wxTextCtrl* m_timerText;
		
		wxRadioButton* m_radioBtnUTC;
		wxStaticText* m_staticText71;
		wxStaticLine* m_staticline22;
		
		wxButton* m_buttonInstallHTMLFiles;
		wxButton* m_buttonInstallLanguages;
		wxStaticText* m_staticText62;
		wxStaticLine* m_staticline13;
		wxPanel* m_panel16;
		
		
		
		
		
		
		
		wxStaticText* m_staticText34;
		wxStaticText* m_staticText31;
		wxTextCtrl* m_sDeg;
		wxStaticText* m_staticText35;
		wxTextCtrl* m_sMin;
		wxStaticText* m_staticText36;
		wxTextCtrl* m_sSec;
		
		wxStaticText* m_staticText38;
		wxTextCtrl* m_sDist;
		wxStaticText* m_staticText39;
		wxTextCtrl* m_sSpeed;
		
		
		
		wxStaticText* m_staticText40;
		wxStaticText* m_staticText41;
		wxTextCtrl* m_smeter;
		
		
		
		wxStaticText* m_staticText63;
		wxStaticText* m_staticText48;
		wxTextCtrl* m_sFeet;
		
		
		
		wxStaticText* m_staticText632;
		wxStaticText* m_staticText49;
		wxTextCtrl* m_sFathom;
		
		
		wxStaticText* m_staticText42;
		wxStaticText* m_staticText43;
		wxTextCtrl* m_sBaro;
		wxStaticText* m_staticText44;
		wxTextCtrl* m_sKnots;
		
		
		
		
		
		
		wxTextCtrl* m_sMeterSec;
		
		
		
		
		
		
		wxTextCtrl* m_sKmh;
		
		
		
		wxStaticText* m_staticText51;
		wxChoice* m_choiceWind;
		wxStaticText* m_staticText45;
		wxChoice* m_choiceDir;
		wxStaticText* m_staticText711;
		wxStaticText* m_staticText46;
		wxStaticText* m_staticText47;
		wxTextCtrl* m_sLiter;
		wxStaticText* m_staticText50;
		wxTextCtrl* m_sMotorh;
		
		
		wxStaticText* m_staticText70;
		wxStaticText* m_staticText712;
		wxStaticText* m_staticText72;
		wxStaticText* m_staticText73;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onCeckBoxShowAllLayouts( wxCommandEvent& event );
		virtual void onCheckBoxShowOnlySelectedLayouts( wxCommandEvent& event );
		virtual void OnTextEnterLayoutPrefix( wxCommandEvent& event );
		virtual void OnCheckBoxMaintenanceRowColoured( wxCommandEvent& event );
		virtual void onCheckBoNoGPS( wxCommandEvent& event );
		virtual void m_checkBoxTimerOnCheckBox( wxCommandEvent& event );
		virtual void onRadioBtnUTC( wxCommandEvent& event );
		virtual void onRadioBtnLocal( wxCommandEvent& event );
		virtual void onRadioBtnGPSAuto( wxCommandEvent& event );
		virtual void onButtonClickInstallHTMLFiles( wxCommandEvent& event );
		virtual void onButtonClickInstallLanguages( wxCommandEvent& event );
		virtual void onButtonHTMLEditor( wxCommandEvent& event );
		virtual void onTextEnterm_sKnots( wxCommandEvent& event );
		virtual void onTextEnterm_sMeterSec( wxCommandEvent& event );
		virtual void onTextEnterm_sKmh( wxCommandEvent& event );
		virtual void OnButtonOKClick( wxCommandEvent& event );
		virtual void onCheckBoxGuardChanged( wxCommandEvent& event );
	
	public:
		wxCheckBox* m_checkBoxShowAllLayouts;
		wxTextCtrl* m_textCtrlLayoutPrefix;
		wxCheckBox* m_checkBoxMaintenanceRowColoured;
		wxCheckBox* m_checkBoxNoGPS;
		wxCheckBox* m_checkBoxCourseChanged;
		wxTextCtrl* m_textCtrlCourseCahngedMoreThen;
		wxTextCtrl* m_textCtrlChancedCourse;
		wxTextCtrl* m_textCtrlCourseMessageAfter;
		wxCheckBox* m_checkBoxGuardChanged;
		wxTextCtrl* m_textCtrlGuradChanged;
		wxCheckBox* m_checkBoxEverySM;
		wxTextCtrl* m_textCtrlEverySM;
		wxTextCtrl* m_textCtrlEverySMText;
		wxStaticText* m_staticText54;
		wxRadioButton* m_radioBtnLocal;
		wxChoice* m_choiceTzIndicator;
		wxChoice* m_choiceTzHours;
		wxRadioButton* m_radioBtnGPSAuto;
		wxTextCtrl* m_textCtrlHTMLEditorPath;
		wxBitmapButton* m_bpButtonHTMLEditor;
		wxChoice* m_choiceDepth;
		wxChoice* m_choiceWaveSwell;
		wxChoice* m_choiceHeading;
		wxChoice* m_choiceWindTo;
		wxTextCtrl* m_Days;
		wxTextCtrl* m_Weeks;
		wxTextCtrl* m_textMonth;

		LogbookOptions( wxWindow* parent, Options* opt, logbookkonni_pi* log_pi, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 585,687  ), long style = wxDEFAULT_DIALOG_STYLE );
		~LogbookOptions();

		void getValues();


	private:
		Options *opt;
		logbookkonni_pi* log_pi;

		wxString choicesWind[3];

		void setValues();
		void loadLanguages();
		void updateWindChoice();
		void updateChoiceBoxes();
	
};
#endif
