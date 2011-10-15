///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "logbook_pi.h"
#include "Logbook.h"
#include "LogbookOptions.h"
#include "Options.h"
#include "LogbookDialog.h"

#include <wx/dir.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>

#include <memory>
#include "../../grib2_pi/src/folder.xpm"
using namespace std;

///////////////////////////////////////////////////////////////////////////
LogbookOptions::LogbookOptions( wxWindow* parent, Options* opt, logbookkonni_pi* log_pi, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->opt = opt;
	this->log_pi = log_pi;

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer10;
	fgSizer10 = new wxFlexGridSizer( 12, 1, 0, 0 );
	fgSizer10->SetFlexibleDirection( wxBOTH );
	fgSizer10->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizer29;
	fgSizer29 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer29->SetFlexibleDirection( wxBOTH );
	fgSizer29->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_checkBoxShowLogbook = new wxCheckBox( this, wxID_ANY, _("Show Logbook Icon"), wxDefaultPosition, wxSize( -1,-1 ), wxCHK_2STATE );
	m_checkBoxShowLogbook->SetValue(true); 
	fgSizer29->Add( m_checkBoxShowLogbook, 0, wxALL, 5 );
	
	
	fgSizer29->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer29->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_checkBoxShowAllLayouts = new wxCheckBox( this, wxID_ANY, _("Show all Layouts"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxShowAllLayouts->SetValue(true); 
	fgSizer29->Add( m_checkBoxShowAllLayouts, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_checkBoxShowOnlySelectedLayouts = new wxCheckBox( this, wxID_ANY, _("Only Layouts with prefix:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer29->Add( m_checkBoxShowOnlySelectedLayouts, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlLayoutPrefix = new wxTextCtrl( this, wxID_ANY, _("en_"), wxDefaultPosition, wxSize( 30,-1 ), wxTE_PROCESS_ENTER );
	fgSizer29->Add( m_textCtrlLayoutPrefix, 0, wxALL, 0 );
	
	m_checkBoxMaintenanceRowColoured = new wxCheckBox( this, wxID_ANY, _("Show Maintence Rows coloured"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxMaintenanceRowColoured->SetValue(true); 
	m_checkBoxMaintenanceRowColoured->Enable( false );
	
	fgSizer29->Add( m_checkBoxMaintenanceRowColoured, 0, wxALL, 5 );
	
	m_checkBoxNoGPS = new wxCheckBox( this, wxID_ANY, _("Write warning \"No GPS\""), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxNoGPS->SetValue(true); 
	
	fgSizer29->Add( m_checkBoxNoGPS, 0, wxALL, 5 );
	
	fgSizer10->Add( fgSizer29, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer13;
	fgSizer13 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer13->SetFlexibleDirection( wxBOTH );
	fgSizer13->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	fgSizer10->Add( fgSizer13, 1, wxEXPAND, 5 );
	
	m_staticline25 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer10->Add( m_staticline25, 0, wxEXPAND | wxALL, 5 );
	
	m_staticText54 = new wxStaticText( this, wxID_ANY, _("Timer"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_staticText54->Wrap( -1 );
	m_staticText54->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Tahoma") ) );
	
	fgSizer10->Add( m_staticText54, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_staticline10 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer10->Add( m_staticline10, 0, wxEXPAND | wxALL, 5 );
	
	wxFlexGridSizer* fgSizer14;
	fgSizer14 = new wxFlexGridSizer( 2, 11, 0, 0 );
	fgSizer14->SetFlexibleDirection( wxBOTH );
	fgSizer14->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	
	fgSizer14->Add( 10, 0, 1, wxEXPAND, 5 );
	
	m_checkBoxTimer = new wxCheckBox( this, wxID_ANY, _("Timer on/off"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	fgSizer14->Add( m_checkBoxTimer, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	
	fgSizer14->Add( 30, 0, 1, wxEXPAND, 5 );
	
	m_staticText56 = new wxStaticText( this, wxID_ANY, _("Interval:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText56->Wrap( -1 );
	fgSizer14->Add( m_staticText56, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrTimerH = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	m_textCtrTimerH->SetMaxLength( 4 ); 
	fgSizer14->Add( m_textCtrTimerH, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText58 = new wxStaticText( this, wxID_ANY, _("h"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText58->Wrap( -1 );
	fgSizer14->Add( m_staticText58, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlTimerM = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	m_textCtrlTimerM->SetMaxLength( 4 ); 
	fgSizer14->Add( m_textCtrlTimerM, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText581 = new wxStaticText( this, wxID_ANY, _("m"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText581->Wrap( -1 );
	fgSizer14->Add( m_staticText581, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlTimerS = new wxTextCtrl( this, wxID_ANY, wxT("1"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	m_textCtrlTimerS->SetMaxLength( 4 ); 
	fgSizer14->Add( m_textCtrlTimerS, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText5811 = new wxStaticText( this, wxID_ANY, _("s"), wxDefaultPosition, wxSize( 20,-1 ), 0 );
	m_staticText5811->Wrap( -1 );
	fgSizer14->Add( m_staticText5811, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_timerText = new wxTextCtrl( this, wxID_ANY, _("Automatic Line by Timer\nyou can set/unset a timer\nor change this text in options"), wxDefaultPosition, wxSize( 250,40 ), wxTE_MULTILINE );
	fgSizer14->Add( m_timerText, 0, wxLEFT, 5 );
	
	fgSizer10->Add( fgSizer14, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer18;
	fgSizer18 = new wxFlexGridSizer( 2, 8, 0, 0 );
	fgSizer18->SetFlexibleDirection( wxBOTH );
	fgSizer18->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	
	fgSizer18->Add( 70, 0, 1, wxEXPAND, 5 );
	
	m_radioBtnUTC = new wxRadioButton( this, wxID_ANY, _("UTC"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer18->Add( m_radioBtnUTC, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_radioBtnLocal = new wxRadioButton( this, wxID_ANY, _("Local = UTC"), wxDefaultPosition, wxDefaultSize, 0 );
	m_radioBtnLocal->SetValue( true ); 
	fgSizer18->Add( m_radioBtnLocal, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0 );
	
	wxString m_choiceTzIndicatorChoices[] = { wxT("+"), wxT("-") };
	int m_choiceTzIndicatorNChoices = sizeof( m_choiceTzIndicatorChoices ) / sizeof( wxString );
	m_choiceTzIndicator = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxSize( 40,-1 ), m_choiceTzIndicatorNChoices, m_choiceTzIndicatorChoices, 0 );
	m_choiceTzIndicator->SetSelection( 0 );
	fgSizer18->Add( m_choiceTzIndicator, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0 );
	
	wxString m_choiceTzHoursChoices[] = { wxT("0"), wxT("1"), wxT("2"), wxT("3"), wxT("4"), wxT("5"), wxT("6"), wxT("7"), wxT("8"), wxT("9"), wxT("10"), wxT("11"), wxT("12") };
	int m_choiceTzHoursNChoices = sizeof( m_choiceTzHoursChoices ) / sizeof( wxString );
	m_choiceTzHours = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxSize( 40,-1 ), m_choiceTzHoursNChoices, m_choiceTzHoursChoices, 0 );
	m_choiceTzHours->SetSelection( 0 );
	fgSizer18->Add( m_choiceTzHours, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText71 = new wxStaticText( this, wxID_ANY, _("h"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText71->Wrap( -1 );
	fgSizer18->Add( m_staticText71, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0 );
	
	m_radioBtnGPSAuto = new wxRadioButton( this, wxID_ANY, _("GPS Auto (Ocean Crossing)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_radioBtnGPSAuto->Enable( false );
	m_radioBtnGPSAuto->SetHelpText( _("not implemented yet") );
	
	fgSizer18->Add( m_radioBtnGPSAuto, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	fgSizer10->Add( fgSizer18, 1, wxEXPAND, 5 );
	
	m_staticline11 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer10->Add( m_staticline11, 0, wxEXPAND | wxALL, 5 );
	
	m_staticText33 = new wxStaticText( this, wxID_ANY, _("Abbreviations"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_staticText33->Wrap( -1 );
	m_staticText33->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Tahoma") ) );
	
	fgSizer10->Add( m_staticText33, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticline91 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer10->Add( m_staticline91, 1, wxEXPAND | wxALL, 5 );
	
	wxFlexGridSizer* fgSizer91;
	fgSizer91 = new wxFlexGridSizer( 2, 7, 0, 0 );
	fgSizer91->SetFlexibleDirection( wxBOTH );
	fgSizer91->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	
	fgSizer91->Add( 100, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 100, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 80, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 80, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 60, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 50, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 100, 0, 1, wxEXPAND, 5 );
	
	m_staticText34 = new wxStaticText( this, wxID_ANY, _("Navigation"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText34->Wrap( -1 );
	m_staticText34->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer91->Add( m_staticText34, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText31 = new wxStaticText( this, wxID_ANY, _("Degres"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText31->Wrap( -1 );
	fgSizer91->Add( m_staticText31, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_sDeg = new wxTextCtrl( this, wxID_ANY, wxT("°"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	fgSizer91->Add( m_sDeg, 0, 0, 5 );
	
	m_staticText35 = new wxStaticText( this, wxID_ANY, _("Minutes"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText35->Wrap( -1 );
	fgSizer91->Add( m_staticText35, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_sMin = new wxTextCtrl( this, wxID_ANY, _("'"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	fgSizer91->Add( m_sMin, 0, 0, 5 );
	
	m_staticText36 = new wxStaticText( this, wxID_ANY, _("Seconds"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText36->Wrap( -1 );
	fgSizer91->Add( m_staticText36, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_sSec = new wxTextCtrl( this, wxID_ANY, _("\""), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	fgSizer91->Add( m_sSec, 0, 0, 5 );
	
	
	fgSizer91->Add( 100, 0, 1, wxEXPAND, 5 );
	
	m_staticText38 = new wxStaticText( this, wxID_ANY, _("Distance"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText38->Wrap( -1 );
	fgSizer91->Add( m_staticText38, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_sDist = new wxTextCtrl( this, wxID_ANY, _("sm"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	fgSizer91->Add( m_sDist, 0, 0, 5 );
	
	m_staticText39 = new wxStaticText( this, wxID_ANY, _("Speed"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText39->Wrap( -1 );
	fgSizer91->Add( m_staticText39, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_sSpeed = new wxTextCtrl( this, wxID_ANY, _("kts"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	fgSizer91->Add( m_sSpeed, 0, 0, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText40 = new wxStaticText( this, wxID_ANY, _("Show Depth in"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText40->Wrap( -1 );
	fgSizer91->Add( m_staticText40, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_choiceDepthChoices[] = { _("Meter"), _("Feet"), _("Fathom") };
	int m_choiceDepthNChoices = sizeof( m_choiceDepthChoices ) / sizeof( wxString );
	m_choiceDepth = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceDepthNChoices, m_choiceDepthChoices, 0 );
	m_choiceDepth->SetSelection( 0 );
	fgSizer91->Add( m_choiceDepth, 0, 0, 5 );
	
	m_staticText41 = new wxStaticText( this, wxID_ANY, _("Meter"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText41->Wrap( -1 );
	fgSizer91->Add( m_staticText41, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_smeter = new wxTextCtrl( this, wxID_ANY, _("m"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	fgSizer91->Add( m_smeter, 0, 0, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 0 );
	
	m_staticText63 = new wxStaticText( this, wxID_ANY, _("Show Wave/Swell in"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText63->Wrap( -1 );
	fgSizer91->Add( m_staticText63, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0 );
	
	wxString m_choiceWaveSwellChoices[] = { _("Meter"), _("Feet"), _("Fathom") };
	int m_choiceWaveSwellNChoices = sizeof( m_choiceWaveSwellChoices ) / sizeof( wxString );
	m_choiceWaveSwell = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceWaveSwellNChoices, m_choiceWaveSwellChoices, 0 );
	m_choiceWaveSwell->SetSelection( 0 );
	fgSizer91->Add( m_choiceWaveSwell, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText48 = new wxStaticText( this, wxID_ANY, _("Feet"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText48->Wrap( -1 );
	fgSizer91->Add( m_staticText48, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_sFeet = new wxTextCtrl( this, wxID_ANY, _("ft"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	fgSizer91->Add( m_sFeet, 0, 0, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 0 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 0 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 0 );
	
	m_staticText632 = new wxStaticText( this, wxID_ANY, _("Heading"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText632->Wrap( -1 );
	fgSizer91->Add( m_staticText632, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0 );
	
	wxString m_choiceHeadingChoices[] = { _("True"), _("Magnetic") };
	int m_choiceHeadingNChoices = sizeof( m_choiceHeadingChoices ) / sizeof( wxString );
	m_choiceHeading = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceHeadingNChoices, m_choiceHeadingChoices, 0 );
	m_choiceHeading->SetSelection( 0 );
	fgSizer91->Add( m_choiceHeading, 1, wxALIGN_CENTER_VERTICAL, 0 );
	
	m_staticText49 = new wxStaticText( this, wxID_ANY, _("Fathom"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText49->Wrap( -1 );
	fgSizer91->Add( m_staticText49, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5 );
	
	m_sFathom = new wxTextCtrl( this, wxID_ANY, _("fm"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	fgSizer91->Add( m_sFathom, 0, 0, 5 );
	
	
	fgSizer91->Add( 0, 1, 1, wxEXPAND, 0 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 0 );
	
	m_staticText42 = new wxStaticText( this, wxID_ANY, _("Weather"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText42->Wrap( -1 );
	m_staticText42->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer91->Add( m_staticText42, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText43 = new wxStaticText( this, wxID_ANY, _("Barometer"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText43->Wrap( -1 );
	fgSizer91->Add( m_staticText43, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_sBaro = new wxTextCtrl( this, wxID_ANY, _("mb"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	fgSizer91->Add( m_sBaro, 0, 0, 5 );
	
	m_staticText44 = new wxStaticText( this, wxID_ANY, _("Windspeed"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText44->Wrap( -1 );
	fgSizer91->Add( m_staticText44, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_sKnots = new wxTextCtrl( this, wxID_ANY, _("kts"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_PROCESS_ENTER );
	fgSizer91->Add( m_sKnots, 0, 0, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_sMeterSec = new wxTextCtrl( this, wxID_ANY, _("m/s"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_PROCESS_ENTER );
	fgSizer91->Add( m_sMeterSec, 0, 0, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_sKmh = new wxTextCtrl( this, wxID_ANY, _("km/h"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_PROCESS_ENTER );
	fgSizer91->Add( m_sKmh, 0, 0, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText51 = new wxStaticText( this, wxID_ANY, _("Windspeed (GPS off)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText51->Wrap( -1 );
	fgSizer91->Add( m_staticText51, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_choiceWindChoices[] = { _("kts"), _("m/s"), _("km/h") };
	int m_choiceWindNChoices = sizeof( m_choiceWindChoices ) / sizeof( wxString );
	m_choiceWind = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceWindNChoices, m_choiceWindChoices, 0 );
	m_choiceWind->SetSelection( 0 );
	fgSizer91->Add( m_choiceWind, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText45 = new wxStaticText( this, wxID_ANY, _("Direction"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText45->Wrap( -1 );
	fgSizer91->Add( m_staticText45, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_choiceDirChoices[] = { _("True"), _("Relative") };
	int m_choiceDirNChoices = sizeof( m_choiceDirChoices ) / sizeof( wxString );
	m_choiceDir = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxSize( 65,-1 ), m_choiceDirNChoices, m_choiceDirChoices, 0 );
	m_choiceDir->SetSelection( 1 );
	fgSizer91->Add( m_choiceDir, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText711 = new wxStaticText( this, wxID_ANY, _("to"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText711->Wrap( -1 );
	fgSizer91->Add( m_staticText711, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_choiceWindToChoices[] = { _("default") ,_("Heading") };
	int m_choiceWindToNChoices = sizeof( m_choiceWindToChoices ) / sizeof( wxString );
	m_choiceWindTo = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxSize( 100,-1 ), m_choiceWindToNChoices, m_choiceWindToChoices, 0 );
	m_choiceWindTo->SetSelection( 0 );
	
	fgSizer91->Add( m_choiceWindTo, 0, wxALL, 0 );
	
	m_staticText46 = new wxStaticText( this, wxID_ANY, _("Fuel/Water"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText46->Wrap( -1 );
	m_staticText46->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer91->Add( m_staticText46, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText47 = new wxStaticText( this, wxID_ANY, _("Capacity"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText47->Wrap( -1 );
	fgSizer91->Add( m_staticText47, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_sLiter = new wxTextCtrl( this, wxID_ANY, _("gal"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	fgSizer91->Add( m_sLiter, 0, 0, 5 );
	
	m_staticText50 = new wxStaticText( this, wxID_ANY, _("Motor Hours"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText50->Wrap( -1 );
	fgSizer91->Add( m_staticText50, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_sMotorh = new wxTextCtrl( this, wxID_ANY, _("h"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	fgSizer91->Add( m_sMotorh, 0, 0, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer91->Add( 0, 0, 1, wxEXPAND, 5 );
	
	fgSizer10->Add( fgSizer91, 1, wxEXPAND, 5 );
	
	m_staticline22 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer10->Add( m_staticline22, 0, wxEXPAND | wxALL, 5 );
	
	m_buttonInstallHTMLFiles = new wxButton( this, wxID_ANY, _("Install OpenCPN_Logbook_Layouts.zip"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer10->Add( m_buttonInstallHTMLFiles, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer16;
	fgSizer16 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer16->SetFlexibleDirection( wxBOTH );
	fgSizer16->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText62 = new wxStaticText( this, wxID_ANY, _("Path to HTML-Editor (optional)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText62->Wrap( -1 );
	fgSizer16->Add( m_staticText62, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlHTMLEditorPath = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 350,-1 ), 0 );
	fgSizer16->Add( m_textCtrlHTMLEditorPath, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_bpButtonHTMLEditor = new wxBitmapButton( this, wxID_ANY, wxBitmap( folder), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer16->Add( m_bpButtonHTMLEditor, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0 );
	
	fgSizer10->Add( fgSizer16, 1, wxEXPAND, 5 );
	
	m_staticline13 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer10->Add( m_staticline13, 0, wxEXPAND | wxALL, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	fgSizer10->Add( m_sdbSizer1, 0, wxALIGN_CENTER, 5 );
	
	this->SetSizer( fgSizer10 );
	this->Layout();
	
	this->Centre( wxBOTH );

	m_checkBoxShowAllLayouts->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogbookOptions::onCeckBoxShowAllLayouts ), NULL, this );
	m_checkBoxShowOnlySelectedLayouts->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogbookOptions::onCheckBoxShowOnlySelectedLayouts ), NULL, this );
	m_textCtrlLayoutPrefix->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( LogbookOptions::OnTextEnterLayoutPrefix ), NULL, this );
	m_checkBoxMaintenanceRowColoured->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogbookOptions::OnCheckBoxMaintenanceRowColoured ), NULL, this );
	m_checkBoxNoGPS->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogbookOptions::onCheckBoNoGPS ), NULL, this );
	m_checkBoxTimer->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogbookOptions::m_checkBoxTimerOnCheckBox ), NULL, this );
	m_radioBtnUTC->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( LogbookOptions::onRadioBtnUTC ), NULL, this );
	m_radioBtnLocal->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( LogbookOptions::onRadioBtnLocal ), NULL, this );
	m_radioBtnGPSAuto->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( LogbookOptions::onRadioBtnGPSAuto ), NULL, this );
	m_sKnots->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( LogbookOptions::onTextEnterm_sKnots ), NULL, this );
	m_sMeterSec->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( LogbookOptions::onTextEnterm_sMeterSec ), NULL, this );
	m_sKmh->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( LogbookOptions::onTextEnterm_sKmh ), NULL, this );
	m_buttonInstallHTMLFiles->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogbookOptions::onClickInstallHTMLFiles ), NULL, this );
	m_bpButtonHTMLEditor->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogbookOptions::onButtonHTMLEditor ), NULL, this );

	setValues();
	if(opt->timer)
	{
		this->m_textCtrTimerH->SetEditable(false);
		this->m_textCtrlTimerM->SetEditable(false);
		this->m_textCtrlTimerS->SetEditable(false);
	}
}

LogbookOptions::~LogbookOptions()
{
	// Disconnect Events
	m_checkBoxShowAllLayouts->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogbookOptions::onCeckBoxShowAllLayouts ), NULL, this );
	m_checkBoxShowOnlySelectedLayouts->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogbookOptions::onCheckBoxShowOnlySelectedLayouts ), NULL, this );
	m_textCtrlLayoutPrefix->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( LogbookOptions::OnTextEnterLayoutPrefix ), NULL, this );
	m_checkBoxMaintenanceRowColoured->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogbookOptions::OnCheckBoxMaintenanceRowColoured ), NULL, this );
	m_checkBoxNoGPS->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogbookOptions::onCheckBoNoGPS ), NULL, this );
	m_checkBoxTimer->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LogbookOptions::m_checkBoxTimerOnCheckBox ), NULL, this );
	m_radioBtnUTC->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( LogbookOptions::onRadioBtnUTC ), NULL, this );
	m_radioBtnLocal->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( LogbookOptions::onRadioBtnLocal ), NULL, this );
	m_radioBtnGPSAuto->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( LogbookOptions::onRadioBtnGPSAuto ), NULL, this );
	m_sKnots->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( LogbookOptions::onTextEnterm_sKnots ), NULL, this );
	m_sMeterSec->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( LogbookOptions::onTextEnterm_sMeterSec ), NULL, this );
	m_sKmh->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( LogbookOptions::onTextEnterm_sKmh ), NULL, this );
	m_buttonInstallHTMLFiles->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogbookOptions::onClickInstallHTMLFiles ), NULL, this );
	m_bpButtonHTMLEditor->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogbookOptions::onButtonHTMLEditor ), NULL, this );

}

void LogbookOptions::setValues()
{
	m_checkBoxTimer->SetValue(opt->timer);
	m_textCtrTimerH->SetValue(opt->thour);
	m_textCtrlTimerM->SetValue(opt->tmin);
	m_textCtrlTimerS->SetValue(opt->tsec);
	m_timerText->SetValue(opt->ttext);

	m_sDeg->SetValue(opt->Deg);
	m_sMin->SetValue(opt->Min);
	m_sSec->SetValue(opt->Sec);

	m_sDist->SetValue(opt->distance);
	m_sSpeed->SetValue(opt->speed);

	m_sBaro->SetValue(opt->baro);
	m_smeter->SetValue(opt->meter);
	m_sFeet->SetValue(opt->feet);
	m_sFathom->SetValue(opt->fathom);

	m_sKnots->SetValue(opt->windkts);
	m_sMeterSec->SetValue(opt->windmeter);
	m_sKmh->SetValue(opt->windkmh);

	m_sLiter->SetValue(opt->vol);
	m_sMotorh->SetValue(opt->motorh);

	choicesWind[0] = opt->windkts;
	choicesWind[1] = opt->windmeter;
	choicesWind[2] = opt->windkmh;
	updateWindChoice();

	m_choiceDepth->SetSelection(opt->showDepth);
	m_choiceWaveSwell->SetSelection(opt->showWaveSwell);
	m_choiceWind->SetSelection(opt->showWindSpeed);
	m_choiceDir->SetSelection(opt->showWindDir);
	m_choiceHeading->SetSelection(opt->showHeading);
	m_choiceWindTo->SetSelection(opt->showWindHeading);

	m_textCtrlHTMLEditorPath->SetValue(opt->htmlEditor);

	if(opt->local == true)
		m_radioBtnLocal->SetValue(true);
	else
		m_radioBtnUTC->SetValue(true);

	m_choiceTzIndicator->SetSelection(opt->tzIndicator);
	m_choiceTzHours->SetSelection(opt->tzHour);

	m_checkBoxNoGPS->SetValue(opt->noGPS);
	m_checkBoxShowAllLayouts->SetValue(opt->showAllLayouts);
	m_checkBoxShowOnlySelectedLayouts->SetValue(opt->filterLayout);
	m_textCtrlLayoutPrefix->SetValue(opt->layoutPrefix);

}

void LogbookOptions::getValues()
{
	opt->timer = m_checkBoxTimer->GetValue();
	opt->thour = m_textCtrTimerH->GetValue();
	opt->tmin  = m_textCtrlTimerM->GetValue();
	opt->tsec  = m_textCtrlTimerS->GetValue();
	opt->ttext = m_timerText->GetValue();

	opt->timerSec = (wxAtol(opt->thour)* 3600 +
					 wxAtol(opt->tmin) *60 +
					 wxAtol(opt->tsec) * 1000);

	opt->Deg = m_sDeg->GetValue();
	opt->Min = m_sMin->GetValue();
	opt->Sec =m_sSec->GetValue();

	opt->distance = m_sDist->GetValue();
	opt->speed    = m_sSpeed->GetValue();

	opt->baro   = m_sBaro->GetValue();
	opt->meter  = m_smeter->GetValue();
	opt->feet   = m_sFeet->GetValue();
	opt->fathom = m_sFathom->GetValue();

	opt->windkts   = m_sKnots->GetValue();
	opt->windmeter = m_sMeterSec->GetValue();
	opt->windkmh   = m_sKmh->GetValue();

	opt->vol    = m_sLiter->GetValue();
	opt->motorh = m_sMotorh->GetValue();

	opt->showDepth       = m_choiceDepth->GetSelection();
	opt->showWaveSwell   = m_choiceWaveSwell->GetSelection();
	opt->showWindSpeed   = m_choiceWind->GetSelection();
	opt->showWindDir     = m_choiceDir->GetSelection();
	opt->showHeading     = m_choiceHeading->GetSelection();
	opt->showWindHeading = m_choiceWindTo->GetSelection();

	if(m_radioBtnLocal->GetValue() == true)
		opt->local = true;
	else
		opt->UTC = true;

	opt->tzIndicator = m_choiceTzIndicator->GetSelection();
	opt->tzHour = m_choiceTzHours->GetSelection();
	opt->noGPS = m_checkBoxNoGPS->GetValue();
	opt->showAllLayouts = m_checkBoxShowAllLayouts->GetValue();
	opt->filterLayout = m_checkBoxShowOnlySelectedLayouts->GetValue();
	opt->layoutPrefix = m_textCtrlLayoutPrefix->GetValue();
}

void LogbookOptions::m_checkBoxTimerOnCheckBox( wxCommandEvent& ev )
{
	opt->timerSec = (wxAtol(m_textCtrTimerH->GetValue())* 3600000 +
					 wxAtol(m_textCtrlTimerM->GetValue()) * 60000 +
					 wxAtol(m_textCtrlTimerS->GetValue()) * 1000);

	long sec = opt->timerSec;

	if(sec == 0)
	{
		m_checkBoxTimer->SetValue(false);
		wxMessageBox(_("Sorry, 0 Hrs 0 Min 0 Sec are not possible"),_("Information"));
		return;
	}

	if(ev.IsChecked())
	{
		m_textCtrTimerH->Enable(false);
		m_textCtrlTimerM->Enable(false);
		m_textCtrlTimerS->Enable(false);
		this->opt->timer = true;

		if(log_pi->m_plogbook_window == NULL)
		{
			log_pi->m_plogbook_window =
				new LogbookDialog(log_pi, log_pi->m_timer, log_pi->m_parent_window,wxID_ANY);
			log_pi->m_plogbook_window->init();
		}
		else
		{
			log_pi->m_plogbook_window->titleExt = _(" - Timer on - Interval: ")+wxString::Format(_T("%s h %s m %s s"),
				this->m_textCtrTimerH->GetValue().c_str(),this->m_textCtrlTimerM->GetValue().c_str(),this->m_textCtrlTimerS->GetValue().c_str());
			log_pi->m_plogbook_window->SetTitle(log_pi->m_plogbook_window->GetTitle()+ log_pi->m_plogbook_window->titleExt);
		}

		log_pi->m_timer->Start(sec);
	}
	else
	{
		m_textCtrTimerH->Enable(true);
		m_textCtrlTimerM->Enable(true);
		m_textCtrlTimerS->Enable(true);
		this->opt->timer = false;
		if(log_pi->m_plogbook_window != NULL)
		{
			log_pi->m_timer->Stop();
			log_pi->m_plogbook_window->SetTitle(log_pi->m_plogbook_window->GetTitle().BeforeFirst(' '));
		}
	}
}

void LogbookOptions::onRadioBtnUTC( wxCommandEvent& ev )
{
	if(ev.IsChecked())
	{
		opt->local = false;
		this->m_choiceTzIndicator->Enable(false);
		this->m_choiceTzHours->Enable(false);
	}

}
void LogbookOptions::onRadioBtnLocal( wxCommandEvent& ev )
{
	if(ev.IsChecked())
	{
		opt->local = true;
		this->m_choiceTzIndicator->Enable(true);
		this->m_choiceTzHours->Enable(true); 
	}
}

void LogbookOptions::onRadioBtnGPSAuto( wxCommandEvent& event )
{
//	loadLayouts();
}

void LogbookOptions::onClickInstallHTMLFiles( wxCommandEvent& event )
{
	log_pi->loadLayouts(this);
}



void LogbookOptions::onButtonHTMLEditor(wxCommandEvent & ev)
{
	wxFileDialog *openFileDialog = 
			new wxFileDialog(this, _("Set Path to HTML-Editor"), _T(""), _T(""),
										   _T(""), 
										   wxFD_OPEN);

	if (openFileDialog->ShowModal() == wxID_CANCEL)
            return;

	m_textCtrlHTMLEditorPath->SetValue(openFileDialog->GetPath());
	opt->htmlEditor = openFileDialog->GetPath();
}

void LogbookOptions::onTextEnterm_sKnots( wxCommandEvent& ev )
{
	choicesWind[0] = ev.GetString();
	updateWindChoice();
	m_sMeterSec->SetFocus();
}

void LogbookOptions::onTextEnterm_sMeterSec( wxCommandEvent& ev )
{
	choicesWind[1] = ev.GetString();
	updateWindChoice();
	m_sKmh->SetFocus();
}

void LogbookOptions::onTextEnterm_sKmh( wxCommandEvent& ev )
{
	choicesWind[2] = ev.GetString();
	updateWindChoice();
	m_choiceWind->SetFocus();
}

void LogbookOptions::onCeckBoxShowAllLayouts( wxCommandEvent& event )
{
	opt->filterLayout = false;
	m_checkBoxShowOnlySelectedLayouts->SetValue(false);
	updateChoiceBoxes();
}
void LogbookOptions::onCheckBoxShowOnlySelectedLayouts( wxCommandEvent& event )
{
	opt->filterLayout = true;
	opt->layoutPrefix = m_textCtrlLayoutPrefix->GetValue();
	m_checkBoxShowAllLayouts->SetValue(false);
	updateChoiceBoxes();
}

void LogbookOptions::OnCheckBoxMaintenanceRowColoured( wxCommandEvent& event )
{

}

void LogbookOptions::onCheckBoNoGPS( wxCommandEvent& event )
{
	opt->noGPS = m_checkBoxNoGPS->GetValue();
}

void LogbookOptions::OnTextEnterLayoutPrefix( wxCommandEvent& event )
{
	opt->layoutPrefix = m_textCtrlLayoutPrefix->GetValue();
	if(m_checkBoxShowOnlySelectedLayouts->GetValue())
		updateChoiceBoxes();
}

void LogbookOptions::updateChoiceBoxes()
{
	LogbookDialog *dialog = log_pi->m_plogbook_window;

	if(dialog != NULL)
	{
		dialog->loadLayoutChoice(dialog->logbook->layout_locn, dialog->logbookChoice);
		dialog->loadLayoutChoice(dialog->crewList->layout_locn, dialog->crewChoice);
		dialog->loadLayoutChoice(dialog->boat->layout_locn, dialog->boatChoice);
		dialog->loadLayoutChoice(dialog->maintenance->layout_locnService, dialog->m_choiceSelectLayoutService);
		dialog->loadLayoutChoice(dialog->maintenance->layout_locnRepairs, dialog->m_choiceSelectLayoutService);
		dialog->loadLayoutChoice(dialog->maintenance->layout_locnBuyParts, dialog->m_choiceSelectLayoutService);
	}
}

void LogbookOptions::updateWindChoice()
{
	int s = m_choiceWind->GetSelection();
	for(int i = 0; i < 3; i++)
		m_choiceWind->SetString(i,choicesWind[i]);
	m_choiceWind->SetSelection(s);
}
