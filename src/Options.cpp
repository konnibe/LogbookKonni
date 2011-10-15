#include "Options.h"
#include <wx/msgdlg.h> 
Options::Options(void)
{
	// Global Options
	firstTime = true;
	dlgWidth = 1010;
	dlgHeight = 535;
	layoutPrefix = _("en_");
	filterLayout = false;
	showAllLayouts = true;
	noGPS = true;
	colouredMaintenance = true;

	// Timer 
	timer = false;
	local = true;
	UTC = false;
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
	Deg = _T("°");
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
