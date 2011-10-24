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

class OverView : public Export
{
public:
	OverView(LogbookDialog* d, wxString data, wxString lay, wxString layoutODT);
	~OverView(void);

	void refresh();
	void actuellLogbook();
	void allLogbooks();
	void selectLogbook();
	void setSelectedRow(int row);
	void gotoRoute();
	void setLayoutLocation();
	void viewODT(wxString path,wxString layout,bool mode);
	void viewHTML(wxString path,wxString layout,bool mode);
	wxString toODT(wxString path,wxString layout,bool mode);
	wxString toHTML(wxString path,wxString layout,bool mode);

	wxString			layout_locn;

private:
	enum logfields{ROUTE,DATE, TIME, SIGN,WATCH, DISTANCE, DISTANCETOTAL, POSITION, COG, HEADING, SOG, STW, DEPTH,
					REMARKS, BAROMETER, WIND, WINDFORCE, CURRENT, CURRENTFORCE, WAVE, SWELL, WEATHER,
					CLOUDS, VISIBILITY, ENGINE, ENGINETOTAL, FUEL, FUELTOTAL, SAILS, REEF,WATER, WATERTOTAL, MREMARKS};
	enum gridfields{FLOG, FROUTE, FSTART, FEND, FDISTANCE, FETMAL, FBESTETMAL, FENGINE, FFUEL, FWATER, FWINDDIR, FWIND, FWINDPEAK, 
					FCURRENTDIR, FCURRENT, FCURRENTPEAK, FWAVE, FWAVEPEAK, FSWELL, FSWELLPEAK, FSAILS, FPATH };

	void loadLogbookData(wxString filename, bool colour);
	void loadAllLogbooks();
	void writeSumColumn(int row, wxString logbook, wxString path, bool colour);
	void resetValues();
	void clearGrid();
	wxString setPlaceHolders(bool mode, wxGrid *grid, int row, wxString middle);

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
	wxString			date;
	wxString			etmaldate;
	double				distance;
	double				etmal;
	double				bestetmal;
	double				bestetmaltemp;
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

	typedef std::map<wxString, int> collection;
	typedef std::pair<wxString, int> pair;
	collection t_coll;
};
#endif