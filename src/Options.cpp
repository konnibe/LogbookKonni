#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include "Options.h"
#include <wx/msgdlg.h> 
#include <wx/mimetype.h>

Options::Options(void)
{
	// Global Options
	traditional = true;
	modern      = false;
	showToolTips= true;

#ifdef __WXMSW__
	firstTime = false;
#endif
#ifdef __WXOSX__ 
	firstTime = false;
#endif
#ifdef __POSIX__
	firstTime = true;
#endif
	dlgWidth = 1010;
	dlgHeight = 535;
	layoutPrefix = _T("Label_");
	filterLayout = false;
	showAllLayouts = true;
	noGPS = true;
	colouredMaintenance = true;
	guardChange = false;
	guardChangeText = _("automatic line\nchange of guards");
	courseChange = false;
	courseChangeDegrees = _T("90");
	courseChangeText = _("automatic line\nchange of course >");
	courseTextAfterMinutes = _T("5");
	everySM = false;
	everySMAmount = _T("10");
	everySMText = _("automatic line\nDistance > ");
	dCourseChangeDegrees = -1;
	dEverySM = -1;

	// Timer 
	timer = false;
	local = true;
	UTC = false;
	gpsAuto = false;
	tzIndicator = 0;
	tzHour = 0;
	thour = _T("0");
	tmin = _T("0");;
	tsec = _T("5");;
	ttext = _("Automatic line by timer\n\n\
You can set/unset a timer-interval\n\
or change this text in Toolbox/Plugins/Logbook\n\nShift+Enter appends a new line");
	timerSec = 5;

	// Grid Navigation
	Deg = _T("\xB0");
	Min = _("'");
	Sec = _("\"");

	distance = _("sm");
	speed = _("kts");

	meter = _("m");
	feet = _("ft");
	fathom = _("fth");

	// Grid Weather
	baro = _("mb");
	windkts = _("kts");
	windmeter = _("m/s");
	windkmh = _("km/h");

	// Grid MotorSails
	vol = _("gal");
	motorh = _("h");

	//Maintenance
	days = _("day(s)");
	weeks = _("week(s)");
	month = _("month");

	//wxChoice
	showDepth = 0;
	showWaveSwell = 0;
	showWindSpeed = 0;
	showWindHeading = 0;
	showWindDir = 0;
	showHeading = 0;


	navGridLayoutChoice = 0;
	crewGridLayoutChoice = 0;
	boatGridLayoutChoice = 0;
	overviewGridLayoutChoice = 0;
	serviceGridLayoutChoice = 0;
	repairsGridLayoutChoice = 0;
	buypartsGridLayoutChoice = 0;
	
	navGridLayoutChoiceODT = 0;
	crewGridLayoutChoiceODT = 0;
	boatGridLayoutChoiceODT = 0;
	overviewGridLayoutChoiceODT = 0;
	serviceGridLayoutChoiceODT = 0;
	repairsGridLayoutChoiceODT = 0;
	buypartsGridLayoutChoiceODT = 0;
	
	navHTML = true;
	crewHTML = true;
	boatHTML = true;
	overviewHTML = true;
	serviceHTML = true;
	repairsHTML = true;
	buypartsHTML = false;
	
	// Paths
	htmlEditor  = _T("");
	htmlEditorReset = htmlEditor;

#ifdef __WXMSW__
	wxFileType *filetype1=wxTheMimeTypesManager->GetFileTypeFromExtension(_T("odt"));
	wxString command;

	if(filetype1)
	{
		if((command = filetype1->GetOpenCommand(_T("x.odt"))) != wxEmptyString)
		{
			command = command.Remove(command.find_last_of(_T(" ")));
			odtEditor   = command;
			odtEditorReset = odtEditor;
		}
	}
	else
	{
			odtEditor   = _T("");
			odtEditorReset = _T("");
	}
#endif
#ifdef __WXGTK__
	odtEditor   = _T("libreoffice --writer");
	odtEditorReset = odtEditor;
#endif
#ifdef __WXOSX__
//	wxFileType *filetype1=wxTheMimeTypesManager->GetFileTypeFromExtension(_T("odt"));
    wxFileType *filetype1=wxTheMimeTypesManager->GetFileTypeFromMimeType(_T("application/vnd.oasis.opendocument.text-template"));
	
	wxString command;

	if(filetype1)
	{
		if((command = filetype1->GetOpenCommand(_T("x.odt")))!= wxEmptyString)
		{
			command = command.Remove(command.find_last_of(_T(" ")));
			odtEditor   = command;
			odtEditorReset = odtEditor;
		}
	}
#endif

#ifdef __WXMSW__
#include <windows.h>

	filetype1=wxTheMimeTypesManager->GetFileTypeFromExtension(_T("RSS"));

	if(filetype1)
	{
		command=filetype1->GetOpenCommand(_T(" "));
		command = command.Remove(command.find_first_of(_T('/'))-1);
		mailClient = command;
		mailClientReset = mailClient;
	}
	else
	{
    OSVERSIONINFO osvi;
    BOOL bIsWindowsXPorLater;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&osvi);

    if((osvi.dwMajorVersion > 5) ||
       ( (osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion >= 1) ))
	{
		mailClient = _T("C:\\Programme\\Outlook Express\\msimn.exe");
		mailClientReset = mailClient;
	}
	else
	{
		mailClient = _T("");
		mailClientReset = _T("");
	}
	}

#endif
#ifdef __WXGTK__
	mailClient = _T("kmail --composer");
	mailClientReset = mailClient;	
#endif
#ifdef __WXOSX__
	mailClient = _T("/Applications/Mail.app ");
	mailClientReset = mailClient;
#endif

#ifdef __WXMSW__
	dataManager = _T("explorer.exe /select,");
	dataManagerReset = dataManager; 
#endif
#ifdef __WXGTK__
	dataManager = _T("dolphin --select");
	dataManagerReset = dataManager; 
	
	htmlEditor = _T("libreoffice");
	htmlEditorReset = htmlEditor;
#endif
#ifdef __WXOSW__
	dataManager = _T("");
	dataManagerReset = dataManagerReset; 
#endif

}

Options::~Options(void)
{
}
