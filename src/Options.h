#ifndef _LOGOPTIONS_H_
#define _LOGOPTIONS_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/dynarray.h> 

WX_DEFINE_ARRAY_INT(int, ArrayOfNavGridColWidth);
WX_DEFINE_ARRAY_INT(int, ArrayOfWeatherGridColWidth);
WX_DEFINE_ARRAY_INT(int, ArrayOfMotorGridColWidth);
WX_DEFINE_ARRAY_INT(int, ArrayOfCrewGridColWidth);
WX_DEFINE_ARRAY_INT(int, ArrayOfWakeGridColWidth);
WX_DEFINE_ARRAY_INT(int, ArrayOfEquipGridColWidth);
WX_DEFINE_ARRAY_INT(int, ArrayOfOverviewGridColWidth);
WX_DEFINE_ARRAY_INT(int, ArrayOfServiceGridColWidth);
WX_DEFINE_ARRAY_INT(int, ArrayOfRepairsGridColWidth);
WX_DEFINE_ARRAY_INT(int, ArrayOfBuyPartsGridColWidth);

class Options
{
public:
	Options(void);
	~Options(void);

	// Global Options
	bool		firstTime;
	int			dlgWidth;
	int			dlgHeight;

	wxString    layoutPrefix;
	bool		filterLayout;
	bool		showAllLayouts;
	bool		noGPS;
	bool		colouredMaintenance;
	bool		guardChange;
	wxString	guardChangeText;
	bool		courseChange;
	wxString	courseChangeDegrees;
	double		dCourseChangeDegrees;
	wxString	courseTextAfterMinutes;
	wxString    courseChangeText;
	bool		everySM;
	wxString	everySMAmount;
	double		dEverySM;
	wxString    everySMText;

	bool		traditional;
	bool		modern;
	bool		showToolTips;
	// Timer 
	bool		timer;
	wxString	thour;
	wxString	tmin;
	wxString	tsec;
	wxString	ttext;
	long		timerSec;
	
	// Grid Navigation
	wxString	Deg;
	wxString	Min;
	wxString	Sec;

	wxString	distance;
	wxString	speed;

	wxString	meter;
	wxString	feet;
	wxString	fathom;

	// Grid Weather
	wxString	baro;
	wxString	windkts;
	wxString	windmeter;
	wxString	windkmh;

	// Grid MotorSails
	wxString	vol;
	wxString	motorh;

	//Maintenance
	wxString	days;
	wxString	weeks;
	wxString	month;

	//Paths 
	wxString    htmlEditor;
	wxString    odtEditor;
	wxString    mailClient;
	wxString    dataManager;

	wxString    htmlEditorReset;
	wxString    odtEditorReset;
	wxString    mailClientReset;
	wxString    dataManagerReset;

	//wxChoice
	int			showDepth;
	int			showWaveSwell;
	int			showWindSpeed;
	int			showWindDir;
	int			showWindHeading;
	int			showHeading;

	int			navGridLayoutChoice;
	int			crewGridLayoutChoice;
	int			boatGridLayoutChoice;

	bool		local;
	bool		UTC;
	bool		gpsAuto;
	int			tzIndicator;
	int			tzHour;

	ArrayOfNavGridColWidth		NavColWidth;
	ArrayOfWeatherGridColWidth	WeatherColWidth;
	ArrayOfMotorGridColWidth	MotorColWidth;
	ArrayOfCrewGridColWidth		CrewColWidth;
	ArrayOfWakeGridColWidth		WakeColWidth;
	ArrayOfEquipGridColWidth	EquipColWidth;
	ArrayOfOverviewGridColWidth OverviewColWidth;
	ArrayOfOverviewGridColWidth ServiceColWidth;
	ArrayOfOverviewGridColWidth RepairsColWidth;
	ArrayOfOverviewGridColWidth BuyPartsColWidth;
};
#endif