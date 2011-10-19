#ifndef _LOGBOOK_H_
#define _LOGBOOK_H_

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "../../../include/ocpn_plugin.h"
#include "LogbookHTML.h"
#include "nmea0183/nmea0183.h"

class Options;
class LogbookDialog;


class Logbook : public LogbookHTML
{
private:
	enum fields{ ROUTE,RDATE,RTIME,SIGN,WAKE,DISTANCE,DTOTAL,POSITION,COG,COW,SOG,SOW,DEPTH,REMARKS,
				 BARO,WIND,WSPD,CURRENT,CSPD,WAVE,SWELL,WEATHER,CLOUDS,VISIBILITY,
				 MOTOR,MOTORT,FUEL,FUELT,SAILS,REEF,WATER,WATERT,MREMARKS};	

	NMEA0183            		m_NMEA0183;
	wxString			sLat;
	wxString			sLon;
	wxDateTime			mUTCDateTime;
	wxDateTime			mCorrectedDateTime;
	wxString			sDate;
	wxString			sTime;
	wxString		    sSOG;
	wxString			sSOW;
	wxString		    sCOG;
	wxString		    sCOW;
	wxString			sDistance;
	wxString		    sWind;
	wxString			sWindSpeed;
	wxString			sDepth;
	wxString			sLogText;
	Options				*opt;
	bool				noSentence;
	bool				gpsStatus;
	bool				bCOW;
	double				dCOW;

	wxString			toSDMM ( int NEflag, double a );
	void				getModifiedCellValue(int grid, int row, int selcol, int col);
	wxString			computeCell(int grid,int row, int col, wxString s, bool mode);
	void				clearAllGrids();
	wxString			calculateDistance(wxString fromstr, wxString tostr);
	wxDouble			positionStringToDezimal(wxString pos);
	wxString			getWake();

public:
	LogbookDialog*	dialog;
	LogbookHTML*	logbookHTML;
	wxString		layout_locn;
	wxString		layoutODT;
	wxString		data_locn;
	bool			modified;

public:
	Logbook(LogbookDialog* parent, wxString data, wxString layout, wxString layoutODT);
	~Logbook(void);

	void SetSentence(wxString &sentence);
	void SetPosition(PlugIn_Position_Fix &pfix);
	void loadData();
	void loadSelectedData(wxString path);
	void loadDatanew();
	void deleteRow(int row);
	void appendRow();
	void update();
	void clearNMEAData();
	void newLogbook();
	void switchToActuellLogbook();
	void selectLogbook();
//	wxString setLogbookData(int rowHeight, int totalColumns, int numPages, int row);
	void changeCellValue(int row, int col, int offset);
	void setLayoutLocation(wxString loc);
	void SetGPSStatus(bool status);


	wxTextFile* logbookFile;

private:
	wxString	logbookData_actuell;
	bool		noAppend; // Old Logbook; append Rows not allowed
	bool		checkGPS();
};
#endif