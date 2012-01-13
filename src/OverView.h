#pragma once
#ifndef _OVERVIEW_H_
#define _OVERVIEW_H_

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "wx/generic/gridctrl.h"
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h> 
#include <wx/txtstrm.h> 

#include "Export.h"

#include <map>

class LogbookDialog;
class Options;
class Logbook;


using namespace std;

struct total{
	wxArrayString		sails;

	wxString			logbookStart;
	wxString			logbookEnd;
	wxString			logbookTimeStart;
	wxString			logbookTimeEnd;
	double				distance;
	double				bestetmal;
	double				speed;
	double				speedpeak;
	double				speedSTW;
	double				speedpeakSTW;
	int					enginehours;
	int					enginemin;
	double				fuel;
	double				water;
	double				winddir;
	double				wind;
	double				wave;
	double				swell;
	double				current;
	double				currentdir;
	double				windpeak;
	double				wavepeak;
	double				swellpeak;
	double				currentpeak;

	int					windcount;
	int					currentcount;
	int					wavecount;
	int					swellcount;
	int					etmalcount;
	int					speedcount;
	int					speedcountSTW;
};

class OverView : public Export
{
public:
	enum gridfields{FLOG, FROUTE, FSTART, FEND, FJOURNEY, FDISTANCE, FETMAL, FBESTETMAL, FSPEED, FBSPEED, FSPEEDSTW, FBSPEEDSTW, FENGINE, FFUEL, FWATER, FWINDDIR, FWIND, FWINDPEAK, 
					FCURRENTDIR, FCURRENT, FCURRENTPEAK, FWAVE, FWAVEPEAK, FSWELL, FSWELLPEAK, FSAILS, FPATH };
	OverView(LogbookDialog* d, wxString data, wxString lay, wxString layoutODT);
	~OverView(void);

	void refresh();
	void actuellLogbook();
	void allLogbooks();
	void selectLogbook();
	void setSelectedRow(int row);
	void gotoRoute();
	void setLayoutLocation();
	void viewODT(wxString path,wxString layout,int mode);
	void viewHTML(wxString path,wxString layout,int mode);
	wxString toODT(wxString path,wxString layout,int mode);
	wxString toHTML(wxString path,wxString layout,int mode);

	wxString			layout_locn;
	struct total		oneLogbookTotal;

private:
	enum logfields{ROUTE,DATE, TIME, SIGN,WATCH, DISTANCE, DISTANCETOTAL, POSITION, COG, HEADING, SOG, STW, DEPTH,
					REMARKS, BAROMETER, WIND, WINDFORCE, CURRENT, CURRENTFORCE, WAVE, SWELL, WEATHER,
					CLOUDS, VISIBILITY, ENGINE, ENGINETOTAL, FUEL, FUELTOTAL, SAILS, REEF,WATER, WATERTOTAL, MREMARKS};

	void loadLogbookData(wxString filename, bool colour);
	void loadAllLogbooks();
	void writeSumColumn(int row, wxString logbook, wxString path, bool colour);
	void writeSumColumnLogbook(total data, int row, wxString logbook, bool colour);
	void resetValues();
	void clearGrid();
	wxString setPlaceHolders(int mode, wxGrid *grid, int row, wxString middle);
	void oneLogbookTotalReset();

	LogbookDialog*		parent;
	wxGrid*				grid;
	Options*			opt;
	Logbook*			logbook;
	wxString			selectedLogbook;

	int					row;
	int					selectedRow;

	wxString			ODTLayout_locn;
	wxString			HTMLLayout_locn;
	wxString			data_locn;
	wxString			data_file;
	wxArrayString		logbooks;
	wxArrayString		sails;

	wxString			startdate;
	wxString			enddate;
	wxString			starttime;
	wxString			endtime;
	wxString			date;
	wxString			etmaldate;
	double				distance;
	double				etmal;
	double				bestetmal;
	double				bestetmaltemp;
	double				speed;
	double				speedpeak;
	double				speedSTW;
	double				speedpeakSTW;
	int					enginehours;
	int					enginemin;
	double				fuel;
	double				water;
	double				winddir;
	double				wind;
	double				wave;
	double				swell;
	double				current;
	double				currentdir;
	double				windpeak;
	double				wavepeak;
	double				swellpeak;
	double				currentpeak;

	int					windcount;
	int					currentcount;
	int					wavecount;
	int					swellcount;
	int					etmalcount;
	int					speedcount;
	int					speedcountSTW;
	wxString			sign;

	typedef std::map<wxString, int> collection;
	typedef std::pair<wxString, int> pair;
	collection t_coll;
};
#endif