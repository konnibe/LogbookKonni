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

	struct Pos{
		double posLat;
		double latitude;
		double latmin;
		char   NSflag;
		double posLon;
		double longitude;
		double lonmin;
		char   WEflag;
				}oldPosition,newPosition;


	NMEA0183       		m_NMEA0183;
	wxString			sLat;
	wxString			sLon;
	wxDateTime			mUTCDateTime;
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
	double				dLastHeading;
	double				dLastCourse;
	long				dLastMinute;
	bool				mode;
	bool				courseChange;
	bool				everySM;
	bool				guardChange;

	wxString			toSDMM ( int NEflag, double a, bool mode );
	void				getModifiedCellValue(int grid, int row, int selcol, int col);
	wxString			computeCell(int grid,int row, int col, wxString s, bool mode);
	void				clearAllGrids();
	wxString			calculateDistance(wxString fromstr, wxString tostr);
	wxDouble			positionStringToDezimal(wxString pos);
	wxString			getWake();
	void				checkCourseChanged();
	void				checkGuardChanged();
	void				checkDistance();
	wxString			positionTraditional(int NEflag, double a, bool mode );
	wxString			positionGPSLike(int NEflag, double a, bool mode );
	void				setOldPosition();

public:
	LogbookDialog*	dialog;
	LogbookHTML*	logbookHTML;
	wxString		layout_locn;
	wxString		layoutODT;
	wxString		data_locn;
	bool			modified;
	wxDateTime		mCorrectedDateTime;

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
	void changeCellValue(int row, int col, int offset);
	void setLayoutLocation(wxString loc);
	void SetGPSStatus(bool status);
	bool checkGPS();


	wxTextFile* logbookFile;
	wxString	title;

private:
	wxString	logbookData_actuell;
	bool		noAppend; // Old Logbook; append Rows not allowed
};


#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class NoAppendDialog
///////////////////////////////////////////////////////////////////////////////
class NoAppendDialog : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText73;
		wxStdDialogButtonSizer* m_sdbSizer5;
		wxButton* m_sdbSizer5OK;
	
	public:
		
		NoAppendDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Information"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 255,130 ), long style = wxDEFAULT_DIALOG_STYLE );
		~NoAppendDialog();
	
};
#endif