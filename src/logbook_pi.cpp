/******************************************************************************
* $Id: logbookkonni_pi.cpp,v 1.8 2010/06/21 01:54:37 bdbcat Exp $
*
* Project:  OpenCPN
* Purpose:  DEMO Plugin
* Author:   David Register
*
***************************************************************************
*   Copyright (C) 2010 by David S. Register   *
*   $EMAIL$   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************
*/

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include "wx/wxprec.h"

#include "logbook_pi.h"
#include "Logbook.h"
#include "LogbookDialog.h"
#include "LogbookOptions.h"
#include "Options.h"

#include "wx/stdpaths.h"
#include <wx/timer.h> 
#include <wx/event.h> 
#include <wx/sysopt.h> 
#include <wx/dir.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/msgdlg.h>

#include <memory>
using namespace std;

#ifndef DECL_EXP
#ifdef __WXMSW__
  #define DECL_EXP     __declspec(dllexport)
#else
  #define DECL_EXP
#endif
#endif


// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr)
{
	return new logbookkonni_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p)
{
	delete p;
}


//---------------------------------------------------------------------------------------------------------
//
//    Demo PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

#include "icons.h"

//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------
logbookkonni_pi::logbookkonni_pi(void *ppimgr)
      :opencpn_plugin(ppimgr)
{	
      // Create the PlugIn icons
      initialize_images();
	  opt = new Options();
	  m_timer = NULL;
}


logbookkonni_pi::~logbookkonni_pi()
{
	if(m_timer != NULL && m_timer->IsRunning())
	{
		m_timer->Stop();
		m_timer = NULL;
	}
	delete opt;
}

int logbookkonni_pi::Init(void)
{
 //   AddLocaleCatalog( _T("opencpn-logbookkonni_pi") );
	m_plogbook_window = NULL;

	opt = new Options();
	// Get a pointer to the opencpn display canvas, to use as a parent for windows created
	m_parent_window = GetOCPNCanvasWindow();

	m_pconfig = GetOCPNConfigObject(); 

	LoadConfig();

	if(m_bLOGShowIcon)
            m_leftclick_tool_id  = InsertPlugInTool(_T(""), _img_logbook, _img_logbook, wxITEM_NORMAL,
                  _("Logbook Konni"), _T(""), NULL,
                   LOGBOOK_TOOL_POSITION, 0, this);

	// Create the Context Menu Items

	//    In order to avoid an ASSERT on msw debug builds,
	//    we need to create a dummy menu to act as a surrogate parent of the created MenuItems
	//    The Items will be re-parented when added to the real context meenu
	wxMenu dummy_menu;

	timer = new LogbookTimer(this);
	m_timer = new wxTimer(timer,wxID_ANY);
	timer->Connect( wxEVT_TIMER, wxObjectEventFunction( &LogbookTimer::OnTimer ));

	if(opt->timer)
	{
		m_plogbook_window = new LogbookDialog(this, m_timer, m_parent_window, wxID_ANY,_("Active Logbook"), wxDefaultPosition, wxSize( opt->dlgWidth,opt->dlgHeight ), wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER);
		m_plogbook_window->init();
		m_plogbook_window->SetPosition(wxPoint(-1,this->m_parent_window->GetParent()->GetPosition().y+80));

		m_timer->Start(opt->timerSec);
	}

	return (
		   WANTS_OVERLAY_CALLBACK |
           WANTS_CURSOR_LATLON       |
           WANTS_TOOLBAR_CALLBACK    |
           INSTALLS_TOOLBAR_TOOL     |
           WANTS_CONFIG              |
		   WANTS_PREFERENCES         |
         //  INSTALLS_CONTEXTMENU_ITEMS     |
           WANTS_NMEA_SENTENCES      |
           WANTS_NMEA_EVENTS		 |
           USES_AUI_MANAGER
		);
}

bool logbookkonni_pi::DeInit(void)
{
	if(m_timer->IsRunning())
		m_timer->Stop();

	timer->Disconnect( wxEVT_TIMER, wxObjectEventFunction( &LogbookTimer::OnTimer ));
    delete timer;
	m_timer = NULL;

	if(m_plogbook_window)
	{
	    if(m_plogbook_window->IsIconized()) m_plogbook_window->Iconize(false);
		m_plogbook_window->setIniValues();
		SaveConfig();
		m_plogbook_window->Close();
		m_plogbook_window->Destroy();
	}
	return true;
}

void logbookkonni_pi::GetOriginalColors()
{
	mcol = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	mcol1 = wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVEBORDER  );
	muitext = wxColour(0,0,0);
	mgridline = m_plogbook_window->m_gridGlobal->GetGridLineColour();
	mudkrd = m_plogbook_window->m_gridGlobal->GetCellTextColour(0,0);
	mback_color = wxColour(255,255,255);
	mtext_color = wxColour(0,0,0);
}

void logbookkonni_pi::SetOriginalColors()
{
	col = mcol;
	col1 = mcol1;
	gridline = mgridline;
	uitext = muitext;
	udkrd = mudkrd;
	back_color = mback_color;
	text_color = mtext_color;
}

void logbookkonni_pi::SetColorScheme(PI_ColorScheme cs)
{  
	if(NULL != m_plogbook_window)
	  {
		  if(cs == PI_GLOBAL_COLOR_SCHEME_DAY || cs == PI_GLOBAL_COLOR_SCHEME_RGB )
		  {
			SetOriginalColors();
		  }
		  else
		  {
			GetGlobalColor(_T("DILG0"),&col);       // Dialog Background white
			GetGlobalColor(_T("DILG1"),&col1);      // Dialog Background
			GetGlobalColor(_T("DILG2"),&back_color);// Control Background
			GetGlobalColor(_T("DILG3"),&text_color);// Text
			GetGlobalColor(_T("UITX1"),&uitext);    // Menu Text, derived from UINFF
			GetGlobalColor(_T("UDKRD"),&udkrd);
			GetGlobalColor(_T("GREY2"),&gridline);          
		  }

		  if(cs == 0 || cs == 1)
				m_plogbook_window->SetBackgroundColour(wxColour(255,255,255));
		  else
				m_plogbook_window->SetBackgroundColour(col);

            m_plogbook_window->SetForegroundColour(uitext);
			dialogDimmer(cs,m_plogbook_window,col,col1,back_color,text_color,uitext,udkrd);
			m_plogbook_window->Refresh();		  
	 }
}

void logbookkonni_pi::dialogDimmer(PI_ColorScheme cs,wxWindow* ctrl,wxColour col, wxColour col1, wxColour back_color,wxColour text_color,
								   wxColour uitext, wxColour udkrd)
{
            wxWindowList kids = ctrl->GetChildren();
            for(unsigned int i = 0 ; i < kids.GetCount() ; i++)
            {
                  wxWindowListNode *node = kids.Item(i);
                  wxWindow *win = node->GetData();

                  if(win->IsKindOf(CLASSINFO(wxListBox)))
					if(cs == PI_GLOBAL_COLOR_SCHEME_DAY || cs == PI_GLOBAL_COLOR_SCHEME_RGB )
						((wxListBox*)win)->SetBackgroundColour(wxNullColour);
					else
                        ((wxListBox*)win)->SetBackgroundColour(col1);

                  else if(win->IsKindOf(CLASSINFO(wxChoice)))
					if(cs == PI_GLOBAL_COLOR_SCHEME_DAY || cs == PI_GLOBAL_COLOR_SCHEME_RGB )
                        ((wxChoice*)win)->SetBackgroundColour(wxColour(255,255,255));
					else
						((wxChoice*)win)->SetBackgroundColour(col1);

                  else if(win->IsKindOf(CLASSINFO(wxRadioButton)))
					  		if(cs == PI_GLOBAL_COLOR_SCHEME_DAY || cs == PI_GLOBAL_COLOR_SCHEME_RGB )
								((wxRadioButton*)win)->SetForegroundColour(wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ));
							else
								((wxRadioButton*)win)->SetForegroundColour(col1);

                  else if(win->IsKindOf(CLASSINFO(wxNotebook)))
				  {
						if(cs == PI_GLOBAL_COLOR_SCHEME_DAY || cs == PI_GLOBAL_COLOR_SCHEME_RGB )
							((wxNotebook*)win)->SetBackgroundColour(wxColour(255,255,255));
						else
							((wxNotebook*)win)->SetBackgroundColour(col1);
						((wxNotebook*)win)->SetForegroundColour(text_color);
				  }

                  else if(win->IsKindOf(CLASSINFO(wxGrid)))
				  {
					  if(cs == PI_GLOBAL_COLOR_SCHEME_DAY || cs == PI_GLOBAL_COLOR_SCHEME_RGB )
						((wxGrid*)win)->SetDefaultCellBackgroundColour(wxColour(255,255,255));
					  else
						((wxGrid*)win)->SetDefaultCellBackgroundColour(col1);
					  ((wxGrid*)win)->SetDefaultCellTextColour(uitext);
					  ((wxGrid*)win)->SetLabelBackgroundColour(col);
					  ((wxGrid*)win)->SetLabelTextColour(uitext);
					  ((wxGrid*)win)->SetDividerPen(wxPen(col));
					  ((wxGrid*)win)->SetGridLineColour(gridline);
					  
				  }

                  else if(win->IsKindOf(CLASSINFO(wxButton)))
				  {
					  if(cs == PI_GLOBAL_COLOR_SCHEME_DAY || cs == PI_GLOBAL_COLOR_SCHEME_RGB )
					  {
						  ((wxButton*)win)->SetForegroundColour(wxNullColour);
						   ((wxButton*)win)->SetBackgroundColour(wxNullColour);
					  }
					  else
					  {
						((wxButton*)win)->SetBackgroundColour(col1);
					  }

				  }

                 else
				  {;
				  }

				  if(win->GetChildren().GetCount() > 0)
				  {
					  wxWindow * w = win;
					  dialogDimmer(cs,w,col,col1,back_color,text_color,uitext,udkrd);
				  }
            }
}

void logbookkonni_pi::UpdateAuiStatus(void)
{
}

int logbookkonni_pi::GetAPIVersionMajor()
{
	return MY_API_VERSION_MAJOR;
}

int logbookkonni_pi::GetAPIVersionMinor()
{
	return MY_API_VERSION_MINOR;
}

int logbookkonni_pi::GetPlugInVersionMajor()
{
	return PLUGIN_VERSION_MAJOR;
}

int logbookkonni_pi::GetPlugInVersionMinor()
{
	return PLUGIN_VERSION_MINOR;
}

wxString logbookkonni_pi::GetCommonName()
{
	return _("LogbookKonni");
}

wxString logbookkonni_pi::GetShortDescription()
{
    AddLocaleCatalog( _T("opencpn-logbookkonni_pi") );
	return _("Logbook for OpenCPN");
}

wxString logbookkonni_pi::GetLongDescription()
{
	return _("Logbook for OpenCPN\n\n\
	- automatic (NMEA) or manual input\n\
	- Timer for automatic line insert\n\
	- Fileexport to ods,html,xml(Windows only),csv\n\
	- Multiple layouts\n\
	- Costumizable layout\n\n\
Thanks's to the community for their helpfull suggestions.\n\n\
If You find a bug post it on\nhttp://www.cruisersforum.com/forums/f134/logbook-konni-for-opencpn-68945.html\n\
Helpfull Videos are at Youtube, search for LogbookKonni.");
}


void logbookkonni_pi::OnContextMenuItemCallback(int id)
{
}

void logbookkonni_pi::SetNMEASentence(wxString &sentence)
{
      if(m_plogbook_window)
      {
		  m_plogbook_window->logbook->SetSentence(sentence);
      }
}

void logbookkonni_pi::SetPositionFix(PlugIn_Position_Fix &pfix)
{
      if(m_plogbook_window)
	  {
		  m_plogbook_window->logbook->SetPosition(pfix);
      }
}

void logbookkonni_pi::SetDefaults(void)
{

      // If the config somehow says NOT to show the icon, override it so the user gets good feedback
      if(!m_bLOGShowIcon)
      {
            m_bLOGShowIcon = true;

            m_leftclick_tool_id  = InsertPlugInTool(_T(""), _img_logbook, _img_logbook, wxITEM_NORMAL,
                  _("Logbook Konni"), _T(""), NULL,
                   LOGBOOK_TOOL_POSITION, 0, this);
				   
      }
}

wxBitmap *logbookkonni_pi::GetPlugInBitmap()
{
      return _img_logbook_pi;
}

int logbookkonni_pi::GetToolbarToolCount(void)
{
      return 1;
}

void logbookkonni_pi::ShowPreferencesDialog( wxWindow* parent )
{
#ifdef __WXOSX__
;
#else
	if(opt->firstTime)
	{
		loadLayouts(parent);
		AddLocaleCatalog( _T("opencpn-logbookkonni_pi") );
		delete opt;
		opt = new Options();
		LoadConfig();
		opt->firstTime = false;
	}
#endif

#ifdef __WXMSW__
	optionsDialog = new LogbookOptions(parent, opt, this, -1, _("Logbook Preferences"), wxDefaultPosition,  wxSize( 613,541  ),
		wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
#else
	optionsDialog = new LogbookOptions(parent, opt, this, -1, _("Logbook Preferences"), wxDefaultPosition,  wxSize( 613,681 ),
		wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );	
#endif
	optionsDialog->m_checkBoxShowLogbook->SetValue(m_bLOGShowIcon);

     if(optionsDialog->ShowModal() == wxID_OK)
      {
		  optionsDialog->getValues();
            //    Show Icon changed value?
		  if(m_bLOGShowIcon != optionsDialog->m_checkBoxShowLogbook->GetValue())
            {
                  m_bLOGShowIcon= optionsDialog->m_checkBoxShowLogbook->GetValue();

                  if(m_bLOGShowIcon)
                        m_leftclick_tool_id  = InsertPlugInTool(_T(""), _img_logbook, _img_logbook, wxITEM_NORMAL,
                              _("Logbook Konni"), _T(""), NULL, LOGBOOK_TOOL_POSITION,
                              0, this);
                  else
                        RemovePlugInTool(m_leftclick_tool_id);
            }
            SaveConfig();
      }
}

void logbookkonni_pi::OnToolbarToolCallback(int id)
{
      // show the Logbook dialog
	if(NULL == m_plogbook_window)
	{
        m_plogbook_window = new LogbookDialog(this, m_timer, m_parent_window, wxID_ANY,_("Active Logbook"), wxDefaultPosition, wxSize( opt->dlgWidth,opt->dlgHeight ), wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER);
		m_plogbook_window->init();
		m_plogbook_window->SetPosition(wxPoint(-1,this->m_parent_window->GetParent()->GetPosition().y+80));
	}
	else
	//	m_plogbook_window->Centre();
	{
		if(m_plogbook_window->IsIconized()) m_plogbook_window->Iconize(false);
		m_plogbook_window->SetPosition(wxPoint(-1,this->m_parent_window->GetParent()->GetPosition().y+80));
	}
	
	m_plogbook_window->Show(); 
	m_plogbook_window->SetFocus();
}

void logbookkonni_pi::SaveConfig()
{
      wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

      if(pConf)
      {
			pConf->SetPath ( _T ( "/PlugIns/Logbook" ) );
            pConf->Write ( _T ( "ShowLOGIcon" ), m_bLOGShowIcon );
			pConf->Write ( _T( "Traditional" ),  opt->traditional );

			pConf->Write ( _T ( "FirstTime" ),  opt->firstTime);
			if(m_plogbook_window)
			{
			  pConf->Write( _T( "DlgWidth" ),  m_plogbook_window->GetSize().GetX());
			  pConf->Write ( _T( "DlgHeight" ), m_plogbook_window->GetSize().GetY());
			}
			pConf->Write ( _T ( "GuardChange" ), opt->guardChange );
			pConf->Write ( _T ( "GuardChangeText" ), opt->guardChangeText );
			pConf->Write ( _T ( "CourseChange" ), opt->courseChange );
			pConf->Write ( _T ( "CouseChangeDegrees" ), opt->courseChangeDegrees );
			pConf->Write ( _T ( "CouseChangeAfter" ), opt->courseTextAfterMinutes );
			pConf->Write ( _T ( "CourseChangeText" ), opt->courseChangeText );
			pConf->Write ( _T ( "EverySM" ), opt->everySM );
			pConf->Write ( _T ( "EverySMAmount" ), opt->everySMAmount );
			pConf->Write ( _T ( "everySMText" ), opt->everySMText );

			pConf->Write ( _T ( "Timer" ), opt->timer );
			pConf->Write ( _T ( "Local" ), opt->local );
			pConf->Write ( _T ( "UTC" ),   opt->UTC );
			pConf->Write ( _T ( "GPSAuto" ), opt->gpsAuto );
			pConf->Write ( _T ( "TzIndicator" ), opt->tzIndicator );
			pConf->Write ( _T ( "TzHours" ), opt->tzHour );
			pConf->Write ( _T ( "TimerHours" ), opt->thour );
			pConf->Write ( _T ( "TimerMin" ), opt->tmin );
			pConf->Write ( _T ( "TimerSec" ), opt->tsec );
			pConf->Write ( _T ( "TimerText" ), opt->ttext );

			pConf->Write ( _T ( "NavDegrees" ), opt->Deg );
			pConf->Write ( _T ( "NavMin" ), opt->Min );
			pConf->Write ( _T ( "NavSec" ), opt->Sec );
			pConf->Write ( _T ( "NavDistance" ), opt->distance );
			pConf->Write ( _T ( "NavSpeed" ), opt->speed );
			pConf->Write ( _T ( "NavMeter" ), opt->meter );
			pConf->Write ( _T ( "NavFeet" ), opt->feet);
			pConf->Write ( _T ( "NavFathom" ), opt->fathom );

			pConf->Write ( _T ( "Baro" ), opt->baro);
			pConf->Write ( _T ( "Windkts" ), opt->windkts );
			pConf->Write ( _T ( "WindMeter" ), opt->windmeter );
			pConf->Write ( _T ( "WindKmh" ), opt->windkmh );

			pConf->Write ( _T ( "Vol" ), opt->vol );
			pConf->Write ( _T ( "Motorhours" ), opt->motorh);

			pConf->Write ( _T ( "Days" ), opt->days );
			pConf->Write ( _T ( "Weeks" ), opt->weeks );
			pConf->Write ( _T ( "Month" ), opt->month );

			pConf->Write ( _T ( "ShowDepth" ), opt->showDepth);
			pConf->Write ( _T ( "ShowWaveSwell" ), opt->showWaveSwell);
			pConf->Write ( _T ( "ShowWindSpeed" ), opt->showWindSpeed);
			pConf->Write ( _T ( "ShowWindDir" ), opt->showWindDir);
			pConf->Write ( _T ( "ShowHeading" ), opt->showHeading);
			pConf->Write ( _T ( "ShowWindHeading" ), opt->showWindHeading);

			pConf->Write ( _T ( "NavGridLayout" ), opt->navGridLayoutChoice);
			pConf->Write ( _T ( "CrewGridLayout" ), opt->crewGridLayoutChoice);
			pConf->Write ( _T ( "BoatGridLayout" ), opt->boatGridLayoutChoice);

			pConf->Write ( _T ( "HTMLEditor" ), opt->htmlEditor);
			pConf->Write ( _T ( "GPSWarning" ), opt->noGPS);
			pConf->Write ( _T ( "ShowAllLayouts" ), opt->showAllLayouts);
			pConf->Write ( _T ( "ShowFilteredLayouts" ), opt->filterLayout);
			pConf->Write ( _T ( "PrefixLayouts" ), opt->layoutPrefix);

			for(unsigned int i = 0; i < opt->NavColWidth.Count(); i++)
				pConf->Write (wxString::Format(_T ( "NavGridColWidth/%i"),i), opt->NavColWidth[i]);
			for(unsigned int i = 0; i < opt->WeatherColWidth.Count(); i++)
				pConf->Write (wxString::Format(_T ( "WeatherGridColWidth/%i"),i), opt->WeatherColWidth[i]);
			for(unsigned int i = 0; i < opt->MotorColWidth.Count(); i++)
				pConf->Write (wxString::Format(_T ( "MotorGridColWidth/%i"),i), opt->MotorColWidth[i]);
			for(unsigned int i = 0; i < opt->CrewColWidth.Count(); i++)
				pConf->Write (wxString::Format(_T ( "CrewGridColWidth/%i"),i), opt->CrewColWidth[i]);
			for(unsigned int i = 0; i < opt->WakeColWidth.Count(); i++)
				pConf->Write (wxString::Format(_T ( "WakeGridColWidth/%i"),i), opt->WakeColWidth[i]);
			for(unsigned int i = 0; i < opt->EquipColWidth.Count(); i++)
				pConf->Write (wxString::Format(_T ( "EquipGridColWidth/%i"),i), opt->EquipColWidth[i]);	
			for(unsigned int i = 0; i < opt->OverviewColWidth.Count(); i++)
				pConf->Write (wxString::Format(_T ( "OverviewGridColWidth/%i"),i), opt->OverviewColWidth[i]);	
			for(unsigned int i = 0; i < opt->ServiceColWidth.Count(); i++)
				pConf->Write (wxString::Format(_T ( "ServiceGridColWidth/%i"),i), opt->ServiceColWidth[i]);
			for(unsigned int i = 0; i < opt->RepairsColWidth.Count(); i++)
				pConf->Write (wxString::Format(_T ( "RepairsGridColWidth/%i"),i), opt->RepairsColWidth[i]);
			for(unsigned int i = 0; i < opt->BuyPartsColWidth.Count(); i++)
				pConf->Write (wxString::Format(_T ( "BuyPartsGridColWidth/%i"),i), opt->BuyPartsColWidth[i]);
	  }
}

void logbookkonni_pi::LoadConfig()
{
      wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

      if(pConf)
      {
            pConf->SetPath ( _T( "/PlugIns/Logbook" ) );
            pConf->Read ( _T( "ShowLOGIcon" ),  &m_bLOGShowIcon, 1 );
			pConf->Read ( _T( "Traditional" ),  &opt->traditional, 1 );
			pConf->Read ( _T( "FirstTime" ),  &opt->firstTime);
			pConf->Read ( _T( "DlgWidth" ),  &opt->dlgWidth,1010);
			pConf->Read ( _T( "DlgHeight" ),  &opt->dlgHeight,535);

			pConf->Read ( _T ( "GuardChange" ), &opt->guardChange );
			pConf->Read ( _T ( "GuardChangeText" ), &opt->guardChangeText );
			pConf->Read ( _T ( "CourseChange" ), &opt->courseChange );
			pConf->Read ( _T ( "CouseChangeDegrees" ), &opt->courseChangeDegrees );
			pConf->Read ( _T ( "CouseChangeAfter" ), &opt->courseTextAfterMinutes );
			pConf->Read ( _T ( "CourseChangeText" ), &opt->courseChangeText );
			pConf->Read ( _T ( "EverySM" ), &opt->everySM );
			pConf->Read ( _T ( "EverySMAmount" ), &opt->everySMAmount );
			pConf->Read ( _T ( "everySMText" ), &opt->everySMText );
			opt->courseChangeDegrees.ToDouble(&opt->dCourseChangeDegrees);
			opt->everySMAmount.ToDouble(&opt->dEverySM);

			pConf->Read ( _T ( "Timer" ), &opt->timer );
			pConf->Read ( _T ( "Local" ), &opt->local );
			pConf->Read ( _T ( "UTC" ),	  &opt->UTC );
			pConf->Read ( _T ( "GPSAuto" ), &opt->gpsAuto);
			pConf->Read ( _T ( "TzIndicator" ), &opt->tzIndicator );
			pConf->Read ( _T ( "TzHours" ), &opt->tzHour );
			pConf->Read ( _T ( "TimerHours" ), &opt->thour );
			pConf->Read ( _T ( "TimerMin" ), &opt->tmin );
			pConf->Read ( _T ( "TimerSec" ), &opt->tsec );
			pConf->Read ( _T ( "TimerText" ), &opt->ttext );

			opt->timerSec = (wxAtol(opt->thour)* 3600000 +
							 wxAtol(opt->tmin) *60000 +
							 wxAtol(opt->tsec) * 1000);

			pConf->Read ( _T ( "NavDegrees" ), &opt->Deg );
			pConf->Read ( _T ( "NavMin" ), &opt->Min );
			pConf->Read ( _T ( "NavSec" ), &opt->Sec );
			pConf->Read ( _T ( "NavDistance" ), &opt->distance );
			pConf->Read ( _T ( "NavSpeed" ), &opt->speed );
			pConf->Read ( _T ( "Baro" ), &opt->baro);
			pConf->Read ( _T ( "NavMeter" ), &opt->meter );
			pConf->Read ( _T ( "NavFeet" ), &opt->feet);
			pConf->Read ( _T ( "NavFathom" ), &opt->fathom );

			pConf->Read ( _T ( "Vol" ), &opt->vol );
			pConf->Read ( _T ( "Motorhours" ), &opt->motorh);

			pConf->Read ( _T ( "Windkts" ), &opt->windkts );
			pConf->Read ( _T ( "WindMeter" ), &opt->windmeter );
			pConf->Read ( _T ( "WindKmh" ), &opt->windkmh );

			pConf->Read ( _T ( "Days" ), &opt->days );
			pConf->Read ( _T ( "Weeks" ), &opt->weeks );
			pConf->Read ( _T ( "Month" ), &opt->month );

			pConf->Read ( _T ( "ShowDepth" ), &opt->showDepth);
			pConf->Read ( _T ( "ShowWaveSwell" ), &opt->showWaveSwell);
			pConf->Read ( _T ( "ShowWindSpeed" ), &opt->showWindSpeed);
			pConf->Read ( _T ( "ShowWindDir" ), &opt->showWindDir);
			pConf->Read ( _T ( "ShowHeading" ), &opt->showHeading);
			pConf->Read ( _T ( "ShowWindHeading" ), &opt->showWindHeading);

			pConf->Read ( _T ( "NavGridLayout" ), &opt->navGridLayoutChoice);
			pConf->Read ( _T ( "CrewGridLayout" ), &opt->crewGridLayoutChoice);
			pConf->Read ( _T ( "BoatGridLayout" ), &opt->boatGridLayoutChoice);

			pConf->Read ( _T ( "HTMLEditor" ), &opt->htmlEditor);
			pConf->Read ( _T ( "GPSWarning" ), &opt->noGPS);
			pConf->Read ( _T ( "ShowAllLayouts" ), &opt->showAllLayouts);
			pConf->Read ( _T ( "ShowFilteredLayouts" ), &opt->filterLayout);
			pConf->Read ( _T ( "PrefixLayouts" ), &opt->layoutPrefix);

			int val;
			bool r;
			int i = 0;
			while(true)
			{
				r = pConf->Read (wxString::Format(_T ( "NavGridColWidth/%i"),i++), &val);
				if(!r) break;
			    opt->NavColWidth.Add(val);
			}
			i = 0;
			while(true)
			{
				r = pConf->Read (wxString::Format(_T ( "WeatherGridColWidth/%i"),i++), &val);
				if(!r) break;
				opt->WeatherColWidth.Add(val);
			}
			i = 0; 
			while(true)
			{
				r = pConf->Read (wxString::Format(_T ( "MotorGridColWidth/%i"),i++), &val);
				if(!r) break;
				opt->MotorColWidth.Add(val);
			}
			i = 0; 
			while(true)
			{
				r = pConf->Read (wxString::Format(_T ( "CrewGridColWidth/%i"),i++), &val);
				if(!r) break;
				opt->CrewColWidth.Add(val);
			}
			i = 0; 
			while(true)
			{
				r = pConf->Read (wxString::Format(_T ( "WakeGridColWidth/%i"),i++), &val);
				if(!r) break;
				opt->WakeColWidth.Add(val);
			}
			i = 0; 
			while(true)
			{
				r = pConf->Read (wxString::Format(_T ( "EquipGridColWidth/%i"),i++), &val);	
				if(!r) break;
				opt->EquipColWidth.Add(val);
			}	
			i = 0; 
			while(true)
			{
				r = pConf->Read (wxString::Format(_T ( "OverviewGridColWidth/%i"),i++), &val, -1);	
				opt->OverviewColWidth.Add(val);
				if(!r) break;
			}
			i = 0;
			while(true)
			{
				r = pConf->Read (wxString::Format(_T ( "ServiceGridColWidth/%i"),i++), &val, -1);	
				opt->ServiceColWidth.Add(val);
				if(!r) break;
			}
			i = 0;
			while(true)
			{
				r = pConf->Read (wxString::Format(_T ( "RepairsGridColWidth/%i"),i++), &val, -1);	
				opt->RepairsColWidth.Add(val);
				if(!r) break;
			}
			i = 0;
			while(true)
			{
				r = pConf->Read (wxString::Format(_T ( "BuyPartsGridColWidth/%i"),i++), &val, -1);	
				opt->BuyPartsColWidth.Add(val);
				if(!r) break;
			}
	  }
}


void logbookkonni_pi::loadLayouts(wxWindow *parent)
{
	static const wxChar *FILETYPES = _T(
		"OpenCPN_Logbook_Layouts.zip");
	std::auto_ptr<wxZipEntry> entry;
	wxString path, sep;
	sep = wxFileName::GetPathSeparator();

	wxStandardPathsBase& std_path = wxStandardPathsBase::Get();
#ifdef __WXMSW__
	wxString stdPath  = std_path.GetConfigDir();
#endif
#ifdef __POSIX__
	wxString stdPath  = std_path.GetUserDataDir();	
#endif
#ifdef __WXOSX__
	wxString stdPath  = std_path.GetConfigDir();
#endif

	wxString *pHome_Locn = new wxString();
	pHome_Locn->Append(stdPath);
	pHome_Locn->append(sep); ;

	pHome_Locn->append(_T("plugins"));
	pHome_Locn->append(sep);
	if(!wxDir::Exists(*pHome_Locn))
		wxMkdir(*pHome_Locn);

	pHome_Locn->Append(_T("logbook"));
	pHome_Locn->append(sep);
	if(!wxDir::Exists(*pHome_Locn))
		wxMkdir(*pHome_Locn);
  
	wxString data = *pHome_Locn;
	data.Append(_T("data"));
	data.append(sep);
	if(!wxDir::Exists(data))
		wxMkdir(data);

	wxString data1 = data;

	data.Append(_T("HTMLLayouts"));
	data.append(sep);
	if(!wxDir::Exists(data))
		wxMkdir(data);

	data1.Append(_T("ODTLayouts"));
	data1.append(sep);
	if(!wxDir::Exists(data1))
		wxMkdir(data1);


	wxFileDialog* openFileDialog =
		new wxFileDialog( parent, _("Select zipped Layout-Files"), _T(""), FILETYPES, FILETYPES,
		                  wxOPEN, wxDefaultPosition);
 
	if ( openFileDialog->ShowModal() == wxID_OK )
	{
		wxFFileInputStream in(openFileDialog->GetPath());
		wxZipInputStream zip(in);

		while (entry.reset(zip.GetNextEntry()), entry.get() != NULL)
		{
			if(entry->GetName().Contains(_T("HTMLLayouts")))
				path = data;
			else
				path = data1;

			wxString name = entry->GetName();

			if(!name.Contains(_T(".htm")) && !name.Contains(_T(".odt"))) continue;
			wxString fn = name.AfterLast(wxFileName::GetPathSeparator());

			if(name.Contains(sep+_T("boat")))
				path.append(_T("boat"));
			else if(name.Contains(sep+_T("logbook")))
				path.append(_T("logbook"));
			else if(name.Contains(sep + _T("crew")))
				path.append(_T("crew"));
			else if(name.Contains(sep + _T("overview")))
				path.append(_T("overview"));
			else if(name.Contains(sep + _T("service")))
				path.append(_T("service"));
			else if(name.Contains(sep + _T("repairs")))
				path.append(_T("repairs"));
			else if(name.Contains(sep + _T("buyparts")))
				path.append(_T("buyparts"));

			path.append(sep);
			if(!wxFileName::DirExists(path))
				wxMkdir(path);

			path.append(fn);
			
			wxFileOutputStream out(path);
			if(zip.OpenEntry(*entry) != true) { out.Close(); break; }
			zip.Read(out);
			out.Close();
		}
		if(m_plogbook_window != NULL)
		{
		m_plogbook_window->loadLayoutChoice(
			m_plogbook_window->logbook->layout_locn,m_plogbook_window->logbookChoice);
		m_plogbook_window->loadLayoutChoice(
			m_plogbook_window->crewList->layout_locn,m_plogbook_window->crewChoice);
		m_plogbook_window->loadLayoutChoice(
			m_plogbook_window->boat->layout_locn,m_plogbook_window->boatChoice);
		}
		wxMessageBox(_("OK"));
	}
	if(opt->firstTime)
		loadLanguages(parent);
}

void logbookkonni_pi::loadLanguages(wxWindow *parent)
{	wxString path;
	bool buildPath;

	std::auto_ptr<wxZipEntry> entry;
	static const wxChar *FILETYPES = _T(
		"OpenCPN_Logbook_Languages.zip");

	wxString sep = wxFileName::GetPathSeparator(); 
	wxString languagePath;
	wxStandardPaths sp;
#ifdef __WXMSW__
	languagePath = sp.GetExecutablePath();
	languagePath = languagePath.Remove(languagePath.find_last_of(sep));	
#endif
#ifdef __POSIX__
	languagePath = sp.GetInstallPrefix();	
#endif
#ifdef __WXOSX__
	languagePath = sp.GetExecutablePath();
	languagePath = languagePath.Remove(languagePath.find_last_of(sep));
#endif

	languagePath.append(sep + _T("share") + sep + _T("locale") + sep);

	wxFileDialog* openFileDialog =
		new wxFileDialog( parent, _("Select zipped Languages-Files"), _T(""), FILETYPES, FILETYPES,
		                  wxOPEN, wxDefaultPosition);
	if ( openFileDialog->ShowModal() == wxID_OK )
	{
		wxFFileInputStream in(openFileDialog->GetPath());
		wxZipInputStream zip(in);

		while (entry.reset(zip.GetNextEntry()), entry.get() != NULL)
		{
			if(!entry->GetName().Contains(_T(".mo")))
				continue;
			else
				path = languagePath + entry->GetName();
				buildPath = false;

			wxFileOutputStream out(path);

			if(zip.OpenEntry(*entry) != true) { out.Close(); break; }
			zip.Read(out);
			out.Close();
		}
		wxMessageBox(_("Languages installed"));
	}

}
////////////////////////////////////////////////////////
void LogbookTimer::OnTimer(wxTimerEvent& ev)
{
	wxFrame *frame = (wxFrame*)plogbook_pi->m_parent_window->GetParent();
	if(frame->IsIconized())
		frame->Iconize(false);

	if(!plogbook_pi->m_plogbook_window->IsShown())
	{
		plogbook_pi->m_plogbook_window->Show();
	}
	plogbook_pi->m_plogbook_window->logbook->appendRow(false);
}


