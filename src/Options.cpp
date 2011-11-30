#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include "Options.h"
#include <wx/msgdlg.h> 
Options::Options(void)
{
	// Global Options
	traditional = true;
	modern = false;
#ifdef __WXMSW__
	firstTime = false;
#elif __WXOSX__ 
	firstTime = false;
#else
	firstTime = true;
#endif
	dlgWidth = 1010;
	dlgHeight = 535;
	layoutPrefix = _("en_");
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


	navGridLayoutChoice = -1;
	crewGridLayoutChoice = 0;
	boatGridLayoutChoice = 0;

	// Path to HTML-Editor
	htmlEditor = _T("");

}

Options::~Options(void)
{
}
