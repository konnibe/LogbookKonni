#ifndef WX_PRECOMP
      #include <wx/wx.h>
#endif

#define PI 3.14159265

#include "LogbookDialog.h"
#include "Logbook.h"
#include "LogbookHTML.h"
#include "logbook_pi.h"
#include "Options.h"
#include "MessageBoxOSX.h"

#include "nmea0183/nmea0183.h"

#include <wx/object.h>
#include "wx/stdpaths.h"
#include "wx/fileconf.h"
#include <wx/image.h>
#include <wx/fs_inet.h>
#include "wx/grid.h"
#include <wx/msgdlg.h>
#include <wx/textctrl.h>
#include "wx/generic/gridctrl.h"
#include <wx/dir.h> 
#include <wx/filefn.h>
#include <wx/msgdlg.h> 
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h> 
#include <wx/txtstrm.h> 

#include <math.h>

Logbook::Logbook(LogbookDialog* parent, wxString data, wxString layout, wxString layoutODT)
: LogbookHTML(this,parent,data,layout)
{
#ifdef PBVE_DEBUG
	pbvecount = 0;
#endif
	oldLogbook = false;
	OCPN_Message = false;
	noSentence = true;
	modified = false;
	wxString logLay;
	lastWayPoint = _T("");

	dialog = parent;
	opt = dialog->logbookPlugIn->opt;

	wxString logData = data; 
	logData.Append(_T("logbook.txt"));
	wxFileName wxHomeFiledir = logData ;
	if(!wxHomeFiledir.FileExists())
	{
		logbookFile = new wxTextFile(logData);	
		logbookFile->Create();
	}
	else
		logbookFile = new wxTextFile(logData);

	data_locn = logData;
	logbookData_actuell = logData;

	if(dialog->m_radioBtnHTML->GetValue())
		logLay = layout;
	else
		logLay = layoutODT;
	setLayoutLocation(logLay);

	weatherCol = dialog->m_gridGlobal->GetNumberCols();
	sailsCol   = dialog->m_gridGlobal->GetNumberCols()+weatherCol-1;

	noAppend = false;
	gpsStatus = false;
	waypointArrived = false;
	WP_skipped = false;
	bCOW = false;
	dCOW = -1;
	dCOG = -1;
	courseChange = false;
	everySM = false;
	guardChange = false;
	dLastMinute = -1;
	oldPosition.latitude = 500;
	activeRoute = wxEmptyString;
	activeRouteGUID = wxEmptyString;
}

Logbook::~Logbook(void)
{
	update();
}

void Logbook::setLayoutLocation(wxString loc)
{
	loc.Append(_T("logbook"));
	dialog->appendOSDirSlash(&loc);
	layout_locn = loc;
	setFileName(data_locn, layout_locn);
	dialog->loadLayoutChoice(layout_locn,dialog->logbookChoice);
}

void Logbook::SetPosition(PlugIn_Position_Fix &pfix)
{
	if(opt->traditional)
		sLat = this->toSDMM(1,pfix.Lat, true);
	else
		sLat = this->toSDMMOpenCPN(1,pfix.Lat, true);

	if(opt->traditional)
		sLon = this->toSDMM(2, pfix.Lon, true);
	else
		sLon = this->toSDMMOpenCPN(2, pfix.Lon, true);

	if(pfix.Sog >= 0.0)
		sSOG = wxString::Format(_T("%5.2f %s"), pfix.Sog,opt->speed.c_str());
	if(pfix.Cog >= 0.0)
		sCOG = wxString::Format(_T("%5.2f %s"),pfix.Cog, opt->Deg.c_str());

	mUTCDateTime.Set(pfix.FixTime);

	gpsStatus = true;
	dialog->GPSTimer->Start(5000);
}
void Logbook::clearNMEAData()
{
	noSentence = true;
}

void Logbook::SetSentence(wxString &sentence)
{
	wxDateTime dt;
	m_NMEA0183 << sentence;

#ifdef PBVE_DEBUG
	if(sentence.Contains(_T("$PBVE")))
	{
		if(pvbe != NULL && pbvecount < 15)
		{
			pvbe->m_textCtrlPVBE->AppendText(sentence);
			pvbe->SetFocus();
			pbvecount++;
		}
	}
#endif

	if(m_NMEA0183.PreParse())
    {
		noSentence = false;
		if(m_NMEA0183.LastSentenceIDReceived == _T("GGA"))
            {
                  if(m_NMEA0183.Parse())
                  {
                        if(m_NMEA0183.Gga.GPSQuality > 0)
                        {
							setPositionString(m_NMEA0183.Gga.Position.Latitude.Latitude,
											m_NMEA0183.Gga.Position.Latitude.Northing,
											m_NMEA0183.Gga.Position.Longitude.Longitude,
											m_NMEA0183.Gga.Position.Longitude.Easting);

                        }
                  }
            }
		else if(m_NMEA0183.LastSentenceIDReceived == _T("GLL"))
            {
                  if(m_NMEA0183.Parse())
                  {
					setPositionString(m_NMEA0183.Gll.Position.Latitude.Latitude,
									m_NMEA0183.Gll.Position.Latitude.Northing,
									m_NMEA0183.Gll.Position.Longitude.Longitude,
									m_NMEA0183.Gll.Position.Longitude.Easting);
                  }
            }
			else if(m_NMEA0183.LastSentenceIDReceived == _T("ZDA"))
            {
                  if(m_NMEA0183.Parse())
                  {
					dt = dt.Set(m_NMEA0183.Zda.Day,(wxDateTime::Month)(m_NMEA0183.Zda.Month-1),m_NMEA0183.Zda.Year);
					dt.ParseTime(dt.ParseFormat(m_NMEA0183.Zda.UTCTime,_T("%H%M%S")));

					setDateTimeString(dt);
				  }
			}
			else if(m_NMEA0183.LastSentenceIDReceived == _T("HDT"))
            {
                  if(m_NMEA0183.Parse())
                  {
					  if(opt->showHeading == 0)
						sCOW = wxString::Format(_T("%5.2f %s"), m_NMEA0183.Hdt.DegreesTrue,opt->Deg.c_str());
					  dCOW = m_NMEA0183.Hdt.DegreesTrue;
					  bCOW = true;
				  }
			}
			else if(m_NMEA0183.LastSentenceIDReceived == _T("HDM"))
            {
                  if(m_NMEA0183.Parse())
                  {
					  if(opt->showHeading == 1)
						sCOW = wxString::Format(_T("%5.2f %s"), m_NMEA0183.Hdm.DegreesMagnetic,opt->Deg.c_str());
					  dCOW = m_NMEA0183.Hdm.DegreesMagnetic;
					  bCOW = true;
				  }
			}
			else if(m_NMEA0183.LastSentenceIDReceived == _T("RMB"))
            {
				  if(opt->waypointArrived)
				  {
					if(m_NMEA0183.Parse())
					{
						  if(m_NMEA0183.Rmb.IsArrivalCircleEntered == NTrue)
						{
							  if(m_NMEA0183.Rmb.From != lastWayPoint)
							{
								//lastWayPoint = m_NMEA0183.Rmb.From;
								checkWayPoint(m_NMEA0183.Rmb);
							}
						}
					}
				  }
			}
			else if(m_NMEA0183.LastSentenceIDReceived == _T("RMC"))
            {
                  if(m_NMEA0183.Parse())
                  {
					setPositionString(m_NMEA0183.Rmc.Position.Latitude.Latitude,
									m_NMEA0183.Rmc.Position.Latitude.Northing,
									m_NMEA0183.Rmc.Position.Longitude.Longitude,
									m_NMEA0183.Rmc.Position.Longitude.Easting);


					if(m_NMEA0183.Rmc.SpeedOverGroundKnots != 999.0)
						sSOG = wxString::Format(_T("%5.2f %s"), m_NMEA0183.Rmc.SpeedOverGroundKnots,opt->speed.c_str());
					if(m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue != 999.0)
						sCOG = wxString::Format(_T("%5.2f %s"), m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue, opt->Deg.c_str());
					if(m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue != 999.0)
						dCOG = m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue;

					long day,month,year;
					m_NMEA0183.Rmc.Date.SubString(0,1).ToLong(&day);
					m_NMEA0183.Rmc.Date.SubString(2,3).ToLong(&month);
					m_NMEA0183.Rmc.Date.SubString(4,5).ToLong(&year);
					dt.Set(((int)day),(wxDateTime::Month)(month-1),((int)year+2000));
					dt.ParseTime(dt.ParseFormat(m_NMEA0183.Rmc.UTCTime,_T("%H%M%S")));

					setDateTimeString(dt);

					if(opt->courseChange)
						checkCourseChanged();
				  }
			}
			else if(m_NMEA0183.LastSentenceIDReceived == _T("VHW"))
            {
                  if(m_NMEA0183.Parse())
                  {
					  sSOW = wxString::Format(_T("%5.2f %s"), m_NMEA0183.Vhw.Knots,opt->speed.c_str());
				  }
			}
			else if(m_NMEA0183.LastSentenceIDReceived == _T("MWV"))
            {
                  if(m_NMEA0183.Parse())
				  {
			        double dWind = 0;
					if(opt->showWindHeading && bCOW)
					{
						dWind = m_NMEA0183.Mwv.WindAngle + dCOW;
						if(dWind > 360) { dWind -= 360; }
					}
					else
						dWind = m_NMEA0183.Mwv.WindAngle;

					sWind = wxString::Format(_T("%3.0f %s %s"), dWind,opt->Deg.c_str(),m_NMEA0183.Mwv.Reference.c_str());

					wxString temp = _T("");
					if(m_NMEA0183.Mwv.WindSpeedUnits == 'N')
						temp = opt->windkts;
					else if(m_NMEA0183.Mwv.WindSpeedUnits == 'M')
						temp = opt->windmeter;
					else if(m_NMEA0183.Mwv.WindSpeedUnits == 'K')
						temp = opt->windkmh;
					sWindSpeed = wxString::Format(_T("%5.2f %s"), m_NMEA0183.Mwv.WindSpeed,temp.c_str());
				  }
			}
            else if(m_NMEA0183.LastSentenceIDReceived == _T("MTW"))
            {	
					  if(m_NMEA0183.Parse())
					  {
						double t;
						if(opt->temperature == _T("F"))
							t = (( m_NMEA0183.Mtw.Temperature * 9 ) / 5 ) + 32;
						else
							t = m_NMEA0183.Mtw.Temperature;
						sTemperatureWater = wxString::Format(_T("%4.1f %s %s"),t,opt->Deg.c_str(),opt->temperature.c_str());
					  }
			}
            else if(m_NMEA0183.LastSentenceIDReceived == _T("DBT"))
            {			
					  m_NMEA0183.Parse();
					  if(m_NMEA0183.Dbt.ErrorMessage.Contains(_T("Invalid")) ||
						  (m_NMEA0183.Dbt.DepthMeters == m_NMEA0183.Dbt.DepthFathoms))
					  {
						sDepth = _T("-----");
					  }
					  else
					  {
						switch(opt->showDepth)
							{
							case 0:
								sDepth = wxString::Format(_T("%5.1f %s"), 
									m_NMEA0183.Dbt.DepthMeters, opt->meter.c_str());
								break;
							case 1:
								sDepth = wxString::Format(_T("%5.1f %s"), 
									m_NMEA0183.Dbt.DepthFeet, opt->feet.c_str());
								break;
							case 2:
								sDepth = wxString::Format(_T("%5.1f %s"), 
									m_NMEA0183.Dbt.DepthFathoms, opt->fathom.c_str());
							break;
						}
					  }
			}
	}
	m_NMEA0183.Dbt.ErrorMessage = _T("");
	m_NMEA0183.ErrorMessage = _T("");
}

void Logbook::setDateTimeString(wxDateTime dt)
{
	mUTCDateTime = dt;

	if(opt->gpsAuto)
	{
		if(newPosition.NSflag == 'E')
			opt->tzIndicator = 0;
		else
			opt->tzIndicator = 1;

		opt->tzHour = (int) newPosition.longitude / 15;
	}

	wxTimeSpan span(opt->tzHour, 0, 0, 0);
	if(opt->tzIndicator == 0)
		mCorrectedDateTime = mUTCDateTime + span;
	else
		mCorrectedDateTime = mUTCDateTime - span;

	if(opt->UTC)
		mCorrectedDateTime = mUTCDateTime;

	sDate = mCorrectedDateTime.FormatDate();
	sTime = mCorrectedDateTime.FormatTime();
	if(opt->guardChange)
		checkGuardChanged();
}

void Logbook::setPositionString(double dLat, int iNorth, double dLon, int iEast)
{
    double lat, lon;
	float llt = dLat;
    int lat_deg_int = (int)(llt / 100);
    float lat_deg = lat_deg_int;
    float lat_min = llt - (lat_deg * 100);
    lat = lat_deg + (lat_min/60.);
    if(iNorth == South)
           lat = -lat;
	if(opt->traditional)
		sLat = this->toSDMM(1,lat, true);
	else
		sLat = this->toSDMMOpenCPN(1,lat, true);

	float lln = dLon;
    int lon_deg_int = (int)(lln / 100);
    float lon_deg = lon_deg_int;
    float lon_min = lln - (lon_deg * 100);
    lon = lon_deg + (lon_min/60.);
    if(iEast == West)
          lon = -lon;
	if(opt->traditional)
		sLon = this->toSDMM(2,lon, false);
	else
		sLon = this->toSDMMOpenCPN(2,lon, true);

	gpsStatus = true;
	dialog->GPSTimer->Start(5000);

	if(opt->everySM)
		checkDistance();
}

void Logbook::newLogbook()
{
	bool zero = false;

	if(data_locn != this->logbookData_actuell)
		this->switchToActuellLogbook();

#ifdef __WXOSX__
    int i = MessageBoxOSX(this->dialog,_("Are you sure ?"),_("New Logbook"),wxID_NO|wxID_OK);
    if(i == wxID_NO)
        return;
    
    int ii = MessageBoxOSX(this->dialog,_("Reset all Values to zero ?"),_T("New Logbook"),wxID_OK|wxID_NO);
    if(ii == wxID_OK)
        zero = true;
    else if(ii == wxID_NO)
        return;
#else
	int i = wxMessageBox(_("Are you sure ?"),_("New Logbook"),wxYES_NO );
	if(i == wxNO)
		return;

	i = wxMessageBox(_("Reset all Values to zero ?"),_T(""),wxYES_NO );
	if(i == wxYES)
		zero = true;
#endif

	if(dialog->m_gridGlobal->GetNumberRows() <= 0)
	{
#ifdef __WXOSX__
        MessageBoxOSX(this->dialog, _("Your Logbook has no lines ?"),_("New Logbook"),wxID_OK);
        return;
#else
		wxMessageBox(_("Your Logbook has no lines ?"),_("New Logbook"),wxOK );
		return;
#endif
	}

	update();

	wxFileName fn = data_locn;
	wxString temp = fn.GetPath();
	dialog->appendOSDirSlash(&temp);
	wxString sn;
	wxString ss = wxDateTime::Now().FormatISOTime();
	ss.Replace(_T(":"),_T("_"));
	ss = wxString::Format(_T("until_%s_%s_logbook.txt"),wxDateTime::Now().FormatISODate().c_str(),ss.c_str());
	sn = temp+ss;
	//
	wxCopyFile(data_locn,sn);

	wxArrayString s;
	for(int i = 0; i < dialog->numPages; i++)
	{
		for(int n = 0; n < dialog->logGrids[i]->GetNumberCols(); n++)
		{
			s.Add(dialog->logGrids[i]->GetCellValue(dialog->logGrids[i]->GetNumberRows()-1,n));
		}
	}

	logbookFile->Open();
	logbookFile->Clear();
	logbookFile->Write();
	logbookFile->Close();

	clearAllGrids();

	int offset = 0;
	dialog->selGridRow = 0;
	for(int i = 0; i < dialog->numPages; i++)
	{
		if(zero) break;
		dialog->logGrids[i]->AppendRows();
		if(i > 0) offset += dialog->logGrids[i-1]->GetNumberCols();

		for(int n = 0; n < dialog->logGrids[i]->GetNumberCols(); n++)
		{
			dialog->logGrids[i]->SetCellValue(0,n,s[n+offset]);
		}
	}
	if(!zero)
	{
		dialog->logGrids[0]->SetCellValue(0,13,_("Last line from Logbook\n")+ss);
		dialog->logGrids[0]->SetCellValue(0,6,dialog->logGrids[0]->GetCellValue(0,6));
		wxString t = _T("0.00 ")+opt->distance;
		dialog->logGrids[0]->SetCellValue(0,5,t);
	}
	else
	{
		appendRow(true);
		dialog->logGrids[0]->SetCellValue(0,13,_("Last Logbook is\n")+ss);
	}

	update();

	dialog->setEqualRowHeight(0);
	setCellAlign(0);
}

void Logbook::selectLogbook()
{
	int selIndex = -1;
	wxString path(*dialog->pHome_Locn);
	path = path + wxFileName::GetPathSeparator() + _T("data");

	SelectLogbook selLogbook(dialog,path);
	
	if(selLogbook.ShowModal() == wxID_CANCEL)
		return;

	selIndex = selLogbook.m_listCtrlSelectLogbook->GetNextItem(selIndex,wxLIST_NEXT_ALL,wxLIST_STATE_SELECTED);
	if(selIndex == -1) return;
	
	wxString s = selLogbook.files[selIndex];

	update();

	for(int i = 0; i < LOGGRIDS; i++)
		if(dialog->logGrids[i]->GetNumberRows() != 0)
			dialog->logGrids[i]->DeleteRows(0,dialog->logGrids[i]->GetNumberRows());

	loadSelectedData(s);
}

void Logbook::loadSelectedData(wxString path)
{
	data_locn = path;
	logbookFile = new wxTextFile(path);
	setFileName(path,layout_locn);
	wxFileName fn(path);
	path = fn.GetName();
	if(path == _T("logbook"))
	{
		path = _("Active Logbook");
		oldLogbook = false;
	}
	else
	{
		wxDateTime dt = dialog->getDateTo(path);
		path = wxString::Format(_("Old Logbook until %s"),dt.FormatDate().c_str()); 
		oldLogbook = true;
	}
	title = path;
	dialog->SetTitle(title);

	loadData();
}
void Logbook::clearAllGrids()
{
	if(dialog->m_gridGlobal->GetNumberRows() > 0)
	{
		dialog->m_gridGlobal->DeleteRows(0,dialog->m_gridGlobal->GetNumberRows(),false);
		dialog->m_gridWeather->DeleteRows(0,dialog->m_gridWeather->GetNumberRows(),false);
		dialog->m_gridMotorSails->DeleteRows(0,dialog->m_gridMotorSails->GetNumberRows(),false);
	}
}

void Logbook::loadData()
{
	wxString s = _T(""),t;
	wxString nullhstr = _T("00:00");
	double nullval = 0.0;

	dialog->selGridCol = dialog->selGridRow = 0;
	title = _("Active Logbook");

	clearAllGrids();

	dialog->m_gridGlobal->BeginBatch();
	dialog->m_gridWeather->BeginBatch();
	dialog->m_gridMotorSails->BeginBatch();

	int row = 0;

	/** make a backup of 0.910 */
	wxString sep = wxFileName::GetPathSeparator();
	wxString source = *dialog->pHome_Locn +_T("data") + sep;
	wxString dest   = *dialog->pHome_Locn +_T("data") + sep + _T("910_Backup");

	wxFileInputStream input1( data_locn );
	wxTextInputStream* stream1 = new wxTextInputStream (input1);
	t = stream1->ReadLine();
	wxStringTokenizer tkz(t, _T("\t"),wxTOKEN_RET_EMPTY );
	//int k = tkz.CountTokens() ;

	if(tkz.CountTokens() == 33 && !wxDir::Exists(dest))
	{
		::wxMkdir(dest);
		wxArrayString files;
		wxDir dir;
		dir.GetAllFiles(source.RemoveLast(),&files,_T("*.txt"),wxDIR_FILES);
		for(unsigned int i = 0; i < files.Count(); i++)
		{
			wxFileName fn(files[i]);
			::wxCopyFile(files[i],dest+sep+fn.GetFullName(),true);
		}
	}
	
	/***************************/

	wxFileInputStream input( data_locn );
	wxTextInputStream* stream = new wxTextInputStream (input);

	while( !(t = stream->ReadLine()).IsEmpty())
	{
		if(input.Eof()) break;
		dialog->m_gridGlobal->AppendRows();
		dialog->m_gridWeather->AppendRows();
		dialog->m_gridMotorSails->AppendRows();

		setCellAlign(row);

		wxStringTokenizer tkz(t, _T("\t"),wxTOKEN_RET_EMPTY );
		int c = 0;
		int fields =  tkz.CountTokens();

		while ( tkz.HasMoreTokens() )
		{
			s = dialog->restoreDangerChar(tkz.GetNextToken());
			s.RemoveLast();

			switch(c)
			{
			case 0:		dialog->m_gridGlobal->SetCellValue(row,ROUTE,s);
				break;
			case 1:		dialog->m_gridGlobal->SetCellValue(row,RDATE,s);
				break;
			case 2:		dialog->m_gridGlobal->SetCellValue(row,RTIME,s);
				break;
			case 3:		dialog->m_gridGlobal->SetCellValue(row,SIGN,s);
				break;
			case 4:		dialog->m_gridGlobal->SetCellValue(row,WAKE,s);
				break;
			case 5:		dialog->m_gridGlobal->SetCellValue(row,DISTANCE,s);
				break;
			case 6:		dialog->m_gridGlobal->SetCellValue(row,DTOTAL,s);
						dialog->m_gridGlobal->SetReadOnly(row,DTOTAL);
				break;
			case 7:		dialog->m_gridGlobal->SetCellValue(row,POSITION,s);
				break;
			case 8:		dialog->m_gridGlobal->SetCellValue(row,COG,s);
				break;
			case 9:		dialog->m_gridGlobal->SetCellValue(row,COW,s);
				break;
			case 10:	dialog->m_gridGlobal->SetCellValue(row,SOG,s);
				break;
			case 11:	dialog->m_gridGlobal->SetCellValue(row,SOW,s);
				break;
			case 12:	dialog->m_gridGlobal->SetCellValue(row,DEPTH,s);
				break;
			case 13:	dialog->m_gridGlobal->SetCellValue(row,REMARKS,s);
				break;
			case 14:	dialog->m_gridWeather->SetCellValue(row,BARO-weatherCol,s);
				break;
			case 15:	dialog->m_gridWeather->SetCellValue(row,WIND-weatherCol,s);
				break;
			case 16:	dialog->m_gridWeather->SetCellValue(row,WSPD-weatherCol,s);
				break;
			case 17:	dialog->m_gridWeather->SetCellValue(row,CURRENT-weatherCol,s);
				break;
			case 18:	dialog->m_gridWeather->SetCellValue(row,CSPD-weatherCol,s);
				break;
			case 19:	dialog->m_gridWeather->SetCellValue(row,WAVE-weatherCol,s);
				break;
			case 20:	dialog->m_gridWeather->SetCellValue(row,SWELL-weatherCol,s);
				break;
			case 21:	dialog->m_gridWeather->SetCellValue(row,WEATHER-weatherCol,s);
				break;
			case 22:	dialog->m_gridWeather->SetCellValue(row,CLOUDS-weatherCol,s);
				break;
			case 23:	dialog->m_gridWeather->SetCellValue(row,VISIBILITY-weatherCol,s);
				break;
			case 24:	dialog->m_gridMotorSails->SetCellValue(row,MOTOR-sailsCol,s);
				break;
			case 25:	dialog->m_gridMotorSails->SetCellValue(row,MOTORT-sailsCol,s);
				break;
			case 26:	dialog->m_gridMotorSails->SetCellValue(row,FUEL-sailsCol,s);
				break;
			case 27:	dialog->m_gridMotorSails->SetCellValue(row,FUELT-sailsCol,s);
				break;
			case 28:	dialog->m_gridMotorSails->SetCellValue(row,SAILS-sailsCol,s);
				break;
			case 29:	dialog->m_gridMotorSails->SetCellValue(row,REEF-sailsCol,s);
				break;
			case 30:	dialog->m_gridMotorSails->SetCellValue(row,WATER-sailsCol,s);
				break;
			case 31:	dialog->m_gridMotorSails->SetCellValue(row,WATERT-sailsCol,s);
				break;
			case 32:	dialog->m_gridMotorSails->SetCellValue(row,MREMARKS-sailsCol,s);
				break;
			case 33:	dialog->m_gridWeather->SetCellValue(row,HYDRO-weatherCol,s);
				break;
			case 34:	dialog->m_gridWeather->SetCellValue(row,TEMPAIR-weatherCol,s);
				break;
			case 35:	dialog->m_gridWeather->SetCellValue(row,TEMPWATER-weatherCol,s);
				break;
			case 36:	dialog->m_gridMotorSails->SetCellValue(row,MOTOR1-sailsCol,s);
				break;
			case 37:	dialog->m_gridMotorSails->SetCellValue(row,MOTOR1T-sailsCol,s);
				break;
			case 38:	dialog->m_gridMotorSails->SetCellValue(row,GENE-sailsCol,s);
				break;
			case 39:	dialog->m_gridMotorSails->SetCellValue(row,GENET-sailsCol,s);
				break;
			case 40:	dialog->m_gridMotorSails->SetCellValue(row,BANK1-sailsCol,s);
				break;
			case 41:	dialog->m_gridMotorSails->SetCellValue(row,BANK1T-sailsCol,s);
				break;
			case 42:	dialog->m_gridMotorSails->SetCellValue(row,BANK2-sailsCol,s);
				break;
			case 43:	dialog->m_gridMotorSails->SetCellValue(row,BANK2T-sailsCol,s);
				break;
			case 44:	dialog->m_gridMotorSails->SetCellValue(row,WATERM-sailsCol,s);
				break;
			case 45:	dialog->m_gridMotorSails->SetCellValue(row,WATERMT-sailsCol,s);
				break;
			case 46:	dialog->m_gridMotorSails->SetCellValue(row,WATERMO-sailsCol,s);
				break;
			}			
			c++;
		}
		wxString temp = dialog->m_gridGlobal->GetCellValue(row,DISTANCE);
		temp.Replace(_T(","),_T("."));
		double dist = wxAtof(temp);
		if((dialog->m_gridGlobal->GetCellValue(row,SIGN) == wxEmptyString || 
			dialog->m_gridGlobal->GetCellValue(row,SIGN).GetChar(0) == ' ') && dist > 0)
				dialog->m_gridGlobal->SetCellValue(row,SIGN,_T("S"));

		if(fields < dialog->totalColumns) // data from 0.910 ? need zero-values to calculate the columns 
			{
				dialog->m_gridMotorSails->SetCellValue(row,MOTOR1-sailsCol, wxString::Format(_T("%s %s"),nullhstr.c_str(),opt->motorh.c_str()));
				dialog->m_gridMotorSails->SetCellValue(row,MOTOR1T-sailsCol,wxString::Format(_T("%s %s"),nullhstr.c_str(),opt->motorh.c_str()));
				dialog->m_gridMotorSails->SetCellValue(row,GENE-sailsCol,   wxString::Format(_T("%s %s"),nullhstr.c_str(),opt->motorh.c_str()));
				dialog->m_gridMotorSails->SetCellValue(row,GENET-sailsCol,  wxString::Format(_T("%s %s"),nullhstr.c_str(),opt->motorh.c_str()));
				dialog->m_gridMotorSails->SetCellValue(row,BANK1-sailsCol,  wxString::Format(_T("%2.2f %s"),nullval,opt->ampereh.c_str()));
				dialog->m_gridMotorSails->SetCellValue(row,BANK1T-sailsCol, wxString::Format(_T("%2.2f %s"),nullval,opt->ampereh.c_str()));
				dialog->m_gridMotorSails->SetCellValue(row,BANK2-sailsCol,  wxString::Format(_T("%2.2f %s"),nullval,opt->ampereh.c_str()));
				dialog->m_gridMotorSails->SetCellValue(row,BANK2T-sailsCol, wxString::Format(_T("%2.2f %s"),nullval,opt->ampereh.c_str()));
				dialog->m_gridMotorSails->SetCellValue(row,WATERM-sailsCol, wxString::Format(_T("%s %s"),nullhstr.c_str(),opt->motorh.c_str()));
				dialog->m_gridMotorSails->SetCellValue(row,WATERMT-sailsCol,wxString::Format(_T("%s %s"),nullhstr.c_str(),opt->motorh.c_str()));
				dialog->m_gridMotorSails->SetCellValue(row,WATERMO-sailsCol,wxString::Format(_T("%2.2f %s"),nullval,opt->vol.c_str()));
			}

		dialog->setEqualRowHeight(row);
		row++;
	}

	for(int i = 0; i < LOGGRIDS; i++)
	{
		dialog->logGrids[i]->Refresh();
		row = dialog->logGrids[i]->GetNumberRows()-1;
		dialog->logGrids[i]->MakeCellVisible(row,0);
	}

	dialog->m_gridGlobal->EndBatch();
	dialog->m_gridWeather->EndBatch();
	dialog->m_gridMotorSails->EndBatch();

	dialog->selGridRow = 0; dialog->selGridCol = 0;
}

void Logbook::setCellAlign(int i)
{
		dialog->m_gridGlobal->SetCellAlignment    (i,ROUTE,                wxALIGN_LEFT, wxALIGN_TOP);
		dialog->m_gridGlobal->SetCellAlignment    (i,RDATE,                wxALIGN_CENTRE, wxALIGN_TOP);
		dialog->m_gridGlobal->SetCellAlignment    (i,RTIME,                wxALIGN_CENTRE, wxALIGN_TOP);
		dialog->m_gridGlobal->SetCellAlignment    (i,SIGN,                 wxALIGN_CENTRE, wxALIGN_TOP);
		dialog->m_gridGlobal->SetCellAlignment    (i,WAKE,                 wxALIGN_LEFT, wxALIGN_TOP);
		dialog->m_gridGlobal->SetCellAlignment    (i,REMARKS,              wxALIGN_LEFT, wxALIGN_TOP);
		dialog->m_gridWeather->SetCellAlignment   (i,WEATHER-weatherCol,   wxALIGN_LEFT, wxALIGN_TOP);
		dialog->m_gridWeather->SetCellAlignment   (i,CLOUDS-weatherCol,    wxALIGN_LEFT, wxALIGN_TOP);
		dialog->m_gridWeather->SetCellAlignment   (i,VISIBILITY-weatherCol,wxALIGN_LEFT, wxALIGN_TOP);
		dialog->m_gridMotorSails->SetCellAlignment(i,SAILS-sailsCol,       wxALIGN_LEFT, wxALIGN_TOP);
		dialog->m_gridMotorSails->SetCellAlignment(i,REEF-sailsCol,        wxALIGN_LEFT, wxALIGN_TOP);
		dialog->m_gridMotorSails->SetCellAlignment(i,MREMARKS-sailsCol,    wxALIGN_LEFT, wxALIGN_TOP);

		dialog->m_gridGlobal->SetReadOnly(i,POSITION,true);
}

void Logbook::switchToActuellLogbook()
{
	dialog->selGridRow = 0; dialog->selGridCol = 0; 
	logbookFile = new wxTextFile(logbookData_actuell);
	data_locn = logbookData_actuell;
	setFileName(logbookData_actuell,layout_locn);
	dialog->SetTitle(_("Active Logbook"));	
	loadData();
}


void Logbook::appendRow(bool mode)
{
	wxString s;

	checkGPS(mode);

	if(noAppend) return;
	modified = true;

	wxFileName fn(logbookFile->GetName());
	if(fn.GetName() != (_T("logbook")))
	{

		this->switchToActuellLogbook();
		noAppend = true;
		NoAppendDialog *x = new NoAppendDialog(dialog);
		x->Show();

		noAppend = false;
		oldLogbook = false;
	}

	int lastRow = dialog->logGrids[0]->GetNumberRows();
	if(lastRow > 800)
	{
		dialog->timer->Stop();
#ifdef __WXOSX__
        MessageBoxOSX(this->dialog, _("Your Logbook has 800 lines or more\n\nPlease create a new logbook to minimize the loadingtime.\n\nIf you have a running timer, it's stopped now !!!"),_("Information"),wxID_OK);
#else
		wxMessageBox(_("Your Logbook has 800 lines or more\n\n\
Please create a new logbook to minimize the loadingtime.\n\nIf you have a running timer, it's stopped now !!!"),_("Information"));
#endif

		dialog->logbookPlugIn->opt->timer = false;
		dialog->menuItemTimer->Check(false);
		wxFileConfig *pConf = (wxFileConfig *)dialog->logbookPlugIn->m_pconfig;

		if(pConf)
		{
            pConf->SetPath ( _T ( "/PlugIns/Logbook" ) );
			pConf->Write ( _T( "Timer" ), dialog->logbookPlugIn->opt->timer );
		}		
	}

	for(int i = 0; i < dialog->numPages; i++ )
	{
			dialog->logGrids[i]->AppendRows();
	}

	if(lastRow > 0)
		{
			dialog->logGrids[0]->SetCellValue(lastRow,ROUTE,dialog->logGrids[0]->GetCellValue(lastRow-1,ROUTE));
			dialog->logGrids[0]->SetCellValue(lastRow,WAKE,dialog->logGrids[0]->GetCellValue(lastRow-1,WAKE));
			dialog->logGrids[0]->SetCellValue(lastRow,POSITION,sLat+sLon);
			changeCellValue(lastRow, 0,0);
			dialog->logGrids[0]->SetCellValue(lastRow,DTOTAL,dialog->logGrids[0]->GetCellValue(lastRow-1,DTOTAL));
			dialog->logGrids[2]->SetCellValue(lastRow,MOTORT-sailsCol,dialog->logGrids[2]->GetCellValue(lastRow-1,MOTORT-sailsCol));
			dialog->logGrids[2]->SetCellValue(lastRow,MOTOR1T-sailsCol,dialog->logGrids[2]->GetCellValue(lastRow-1,MOTOR1T-sailsCol));
			dialog->logGrids[2]->SetCellValue(lastRow,GENET-sailsCol,dialog->logGrids[2]->GetCellValue(lastRow-1,GENET-sailsCol));
			dialog->logGrids[2]->SetCellValue(lastRow,FUELT-sailsCol,dialog->logGrids[2]->GetCellValue(lastRow-1,FUELT-sailsCol));
			dialog->logGrids[2]->SetCellValue(lastRow,WATERT-sailsCol,dialog->logGrids[2]->GetCellValue(lastRow-1,WATERT-sailsCol));
			dialog->logGrids[2]->SetCellValue(lastRow,WATERMT-sailsCol,dialog->logGrids[2]->GetCellValue(lastRow-1,WATERMT-sailsCol));
			dialog->logGrids[2]->SetCellValue(lastRow,BANK1T-sailsCol,dialog->logGrids[2]->GetCellValue(lastRow-1,BANK1T-sailsCol));
			dialog->logGrids[2]->SetCellValue(lastRow,BANK2T-sailsCol,dialog->logGrids[2]->GetCellValue(lastRow-1,BANK2T-sailsCol));
		}
	else
	{
			dialog->logGrids[2]->SetCellValue(lastRow,FUELT- sailsCol,opt->fuelTank.c_str());
			dialog->logGrids[2]->SetCellValue(lastRow,WATERT-sailsCol,opt->waterTank.c_str());
			dialog->logGrids[2]->SetCellValue(lastRow,BANK1T-sailsCol,opt->bank1.c_str());
			dialog->logGrids[2]->SetCellValue(lastRow,BANK2T-sailsCol,opt->bank2.c_str());
	}

	if(sDate != _T(""))
		{
			dialog->logGrids[0]->SetCellValue(lastRow,RDATE,sDate);
			dialog->logGrids[0]->SetCellValue(lastRow,RTIME,sTime);
		}
		else
		{
			mCorrectedDateTime = wxDateTime::Now();
			dialog->logGrids[0]->SetCellValue(lastRow,RDATE,mCorrectedDateTime.FormatDate());
			dialog->logGrids[0]->SetCellValue(lastRow,RTIME,mCorrectedDateTime.FormatTime());
		}

	if(activeRouteGUID != wxEmptyString)
	{
		if(activeRoute != wxEmptyString)
			dialog->logGrids[0]->SetCellValue(lastRow,ROUTE,activeRoute);
		else
			dialog->logGrids[0]->SetCellValue(lastRow,ROUTE,_("(Unnamed Route)"));
	}
	dialog->logGrids[0]->SetCellValue(lastRow,POSITION,sLat+sLon);
	dialog->logGrids[0]->SetCellValue(lastRow,COG,sCOG);
	dialog->logGrids[0]->SetCellValue(lastRow,COW,sCOW);
	dialog->logGrids[0]->SetCellValue(lastRow,SOG,sSOG);
	dialog->logGrids[0]->SetCellValue(lastRow,SOW,sSOW);
	dialog->logGrids[0]->SetCellValue(lastRow,DEPTH,sDepth);
	dialog->logGrids[1]->SetCellValue(lastRow,TEMPWATER-weatherCol,sTemperatureWater);
	dialog->logGrids[1]->SetCellValue(lastRow,WIND-weatherCol,sWind);
	dialog->logGrids[1]->SetCellValue(lastRow,WSPD-weatherCol,sWindSpeed);
	dialog->logGrids[2]->SetCellValue(lastRow,MOTOR-sailsCol,_T("00.00"));
	dialog->logGrids[2]->SetCellValue(lastRow,MOTOR1-sailsCol,_T("00.00"));
	dialog->logGrids[2]->SetCellValue(lastRow,GENE-sailsCol,_T("00.00"));
//	dialog->logGrids[2]->SetCellValue(lastRow,BANK1-sailsCol,_T("0"));
//	dialog->logGrids[2]->SetCellValue(lastRow,BANK2-sailsCol,_T("0"));
	dialog->logGrids[2]->SetCellValue(lastRow,WATERM-sailsCol,_T("00.00"));
	dialog->logGrids[2]->SetCellValue(lastRow,WATERMO-sailsCol,_T("0"));
//	dialog->logGrids[2]->SetCellValue(lastRow,FUEL-sailsCol,_T("0"));
//	dialog->logGrids[2]->SetCellValue(lastRow,WATER-sailsCol,_T("0"));
	dialog->logGrids[2]->SetCellValue(lastRow,MREMARKS-sailsCol,_T(" "));
	dialog->logGrids[0]->SetCellValue(lastRow,WAKE,getWake());

	dialog->logGrids[0]->SetCellValue(lastRow,13,sLogText);

	changeCellValue(lastRow, 0,1);
	setCellAlign(lastRow);
	dialog->setEqualRowHeight(lastRow);

	dialog->m_gridGlobal->SetReadOnly(lastRow,6); 
/*	dialog->m_gridMotorSails->SetReadOnly(lastRow,1);
	dialog->m_gridMotorSails->SetReadOnly(lastRow,3);
	dialog->m_gridMotorSails->SetReadOnly(lastRow,7);
*/

	if(mode)
	{
		dialog->m_gridGlobal->MakeCellVisible(lastRow,0);
		dialog->m_gridWeather->MakeCellVisible(lastRow,0);
		dialog->m_gridMotorSails->MakeCellVisible(lastRow,0);
	}
}

void Logbook::checkCourseChanged()
{
	static wxDateTime dt;
	static bool timer = true;

	wxDouble cog;
	wxGrid* grid = dialog->m_gridGlobal;
	if(grid->GetNumberRows() == 0) return;
	wxString temp = grid->GetCellValue(grid->GetNumberRows()-1,8);
	temp.Replace(_T(","),_T("."));
	temp.ToDouble(&cog);

	if((cog == dCOG) || oldLogbook) return;

#ifdef __WXOSX__
	wxDouble result = labs(cog-dCOG); 
#else
	wxDouble result = abs(cog-dCOG); 
#endif
	if(result > 180) result -= 360;

#ifdef __WXOSX__
	if(labs(result) >= opt->dCourseChangeDegrees)
#else
	if(abs(result) >= opt->dCourseChangeDegrees)
#endif
		{
			if(timer)
			{
				timer = false;
				dt = mCorrectedDateTime;
				long min;
				opt->courseTextAfterMinutes.ToLong(&min);
				wxTimeSpan t(0,(int)min);
				dt.Add(t);
			}
			

			if(mCorrectedDateTime >= dt)
			{
				timer = true;
				courseChange = true;
				appendRow(false);
				courseChange = false;
			}
		}
}

void Logbook::checkWayPoint(RMB rmb)
{
	if(lastWayPoint == rmb.From) return;

	tempRMB = rmb;
	waypointArrived = true;
	appendRow(false);
	waypointArrived = false;
	lastWayPoint = rmb.From;
}

void Logbook::checkGuardChanged()
{
	if(dLastMinute == -1)
	{ dLastMinute = (long) mCorrectedDateTime.GetMinute()+1; return; }

	long hour,minute;
	long m_minute = (long) mCorrectedDateTime.GetMinute();
	long m_hour = (long) mCorrectedDateTime.GetHour();
	bool append = false;

	if(m_minute >= dLastMinute)
	{
		for(int row = 0; row < dialog->m_gridCrewWake->GetNumberRows(); row++)
		{
			for(int col = 2; col < dialog->m_gridCrewWake->GetNumberCols(); col += 2)
			{
				wxString s = dialog->m_gridCrewWake->GetCellValue(row,col);
				if(s.IsEmpty()) continue;
				wxStringTokenizer tkz(s,_T(":"));
				tkz.GetNextToken().ToLong(&hour);
				tkz.GetNextToken().ToLong(&minute);
				if(hour != m_hour) continue;
				if(minute == m_minute)
					append = true;
			}
		}
		if(append) 
		{ 
			guardChange = true;
			appendRow(false);
			guardChange = false;
		}
		dLastMinute = m_minute + 1;
	}
}

void Logbook::checkDistance()
{
	if(oldPosition.latitude == 500)
		oldPosition = newPosition;

	double fromlat = oldPosition.posLat * PI/180;
	double fromlon = oldPosition.posLon * PI/180;
	double tolat = newPosition.posLat * PI/180;
	double tolon = newPosition.posLon * PI/180;
	if(oldPosition.NSflag == 'S') fromlat = -fromlat;
	if(oldPosition.WEflag == 'W') fromlon = -fromlon;
	if(newPosition.NSflag == 'S') tolat = -fromlat;
	if(newPosition.WEflag == 'W') tolon = -fromlon;

	double sm = 
		acos(cos(fromlat)*cos(fromlon)*cos(tolat)*cos(tolon) + 
		  cos(fromlat)*sin(fromlon)*cos(tolat)*sin(tolon) + sin(fromlat)*sin(tolat)) * 3443.9;

	if(sm >= opt->dEverySM)
	{
		everySM = true;	
		appendRow(false);
		everySM = false;
		oldPosition = newPosition;
	}
}

wxString Logbook::getWake()
{
	wxString start, end, name = _T("");;
	wxDateTime dtstart,dtend, now;
	int count = 0;

	dtstart = dtend = mCorrectedDateTime;
	now = mCorrectedDateTime;

	for(int r = 0; r < dialog->m_gridCrewWake->GetRows(); r++)
	{
		for(int i = 0; i < 6; i++)
		{
			start = dialog->m_gridCrewWake->GetCellValue(r,2+i*2);
			end   = dialog->m_gridCrewWake->GetCellValue(r,3+i*2);

			if(start.IsEmpty() || end.IsEmpty()) continue;

			dtstart.ParseTime(start);
			dtend.ParseTime(end);

			wxDateSpan sp(0,0,0,1);
			if((dtstart.GetHour() > dtend.GetHour())) 
					dtend.Add(sp);

			if(now >= dtstart && now <= dtend)
			{		
				if(count != 0)
					name += _T("\n");
				name += dialog->m_gridCrewWake->GetCellValue(r,1)+ _T(" ") + dialog->m_gridCrewWake->GetCellValue(r,0);
				count++;
			}
			dtend = mCorrectedDateTime;
		}
	}
	return name;
}

wxString Logbook::calculateDistance(wxString fromstr, wxString tostr)
{
	if((fromstr.IsEmpty() || tostr.IsEmpty()) || fromstr == tostr) 
		return wxString(_T("0.00 ")+opt->distance);

	wxString sLat, sLon, sLatto, sLonto;
	wxDouble fromlat,fromlon,tolat,tolon, sm;

	wxStringTokenizer tkz(fromstr, _T("\n"));
	sLat = tkz.GetNextToken();
	sLon = tkz.GetNextToken();
	wxStringTokenizer tkzto(tostr, _T("\n"));
	sLatto = tkzto.GetNextToken();
	sLonto = tkzto.GetNextToken();

	if(opt->traditional)
	{
		fromlat = positionStringToDezimal(sLat)* (PI/180);
		fromlon = positionStringToDezimal(sLon)* (PI/180);
		tolat = positionStringToDezimal(sLatto)* (PI/180);
		tolon = positionStringToDezimal(sLonto)* (PI/180);
	}
	else
	{
		fromlat = positionStringToDezimalModern(sLat)* (PI/180);
		fromlon = positionStringToDezimalModern(sLon)* (PI/180);
		tolat = positionStringToDezimalModern(sLatto)* (PI/180);
		tolon = positionStringToDezimalModern(sLonto)* (PI/180);
	}
	if(oldPosition.NSflag == 'S') fromlat = -fromlat;
	if(oldPosition.WEflag == 'W') fromlon = -fromlon;
	if(newPosition.NSflag == 'S') tolat = -fromlat;
	if(newPosition.WEflag == 'W') tolon = -fromlon;

///////
sm = acos(cos(fromlat)*cos(fromlon)*cos(tolat)*cos(tolon) + 
		  cos(fromlat)*sin(fromlon)*cos(tolat)*sin(tolon) + sin(fromlat)*sin(tolat)) * 3443.9;
////// code snippet from http://www2.nau.edu/~cvm/latlongdist.html#formats

	wxString ret = wxString::Format(_T("%.2f %s"),sm,opt->distance.c_str());
	ret.Replace(_T("."),dialog->decimalPoint);
	return ret;
}

wxDouble Logbook::positionStringToDezimal(wxString pos)
{
	wxDouble resdeg, resmin, ressec = 0;
	wxString temp = pos;

	wxStringTokenizer tkz(pos, _T(" "));
	temp = tkz.GetNextToken();
	temp.Replace(_T(","),_T("."));
	temp.ToDouble(&resdeg);
	if(pos.Contains(_T("S"))) resdeg = -resdeg;
	if(pos.Contains(_T("W"))) resdeg = -resdeg;
	temp = tkz.GetNextToken();
	temp.Replace(_T(","),_T("."));
	temp.ToDouble(&resmin);
	if(pos.Contains(_T("S"))) resmin = -resmin;
	if(pos.Contains(_T("W"))) resmin = -resmin;
	temp = tkz.GetNextToken();
	temp.Replace(_T(","),_T("."));
	temp.ToDouble(&ressec);
	if(pos.Contains(_T("S"))) ressec = -ressec;
	if(pos.Contains(_T("W"))) ressec = -ressec;
	resmin = (resmin/60 + ressec/3600);

	return resdeg + resmin;
}

wxDouble Logbook::positionStringToDezimalModern(wxString pos)
{
	wxDouble resdeg, resmin;
	wxString temp = pos;

	wxStringTokenizer tkz(pos, _T(" "));
	temp = tkz.GetNextToken();
	temp.Replace(_T(","),_T("."));
	temp.ToDouble(&resdeg);
	if(pos.Contains(_T("S"))) resdeg = -resdeg;
	if(pos.Contains(_T("W"))) resdeg = -resdeg;
	temp = tkz.GetNextToken();
	temp.Replace(_T(","),_T("."));
	temp.ToDouble(&resmin);
	if(pos.Contains(_T("S"))) resmin = -resmin;
	if(pos.Contains(_T("W"))) resmin = -resmin;

	return resdeg + (resmin/60);
}

void Logbook::deleteRow(int row)
{
#ifdef __WXOSX__
    int answer = MessageBoxOSX(this->dialog, (wxString::Format(_("Delete Row Nr. %i ?"),row+1)), _("Confirm"), wxID_OK | wxID_CANCEL| wxID_NO);
    if (answer == wxID_OK)
        {
            for(int i =0; i < dialog->numPages; i++)
                dialog->logGrids[i]->DeleteRows(row);
        }
#else
	int answer = wxMessageBox(wxString::Format(_("Delete Row Nr. %i ?"),row+1), _("Confirm"),
                              wxYES_NO | wxCANCEL, dialog);
	if (answer == wxYES)
	{
		for(int i =0; i < dialog->numPages; i++)
			dialog->logGrids[i]->DeleteRows(row);
	}
#endif
	modified = true;
}

void Logbook::changeCellValue(int row, int col, int mode)
{
	if(mode)
		for(int g =0; g < LOGGRIDS; g++)
			for(int i = 0; i < dialog->logGrids[g]->GetNumberCols(); i++)
				getModifiedCellValue(g,row,i,i);
	else
		getModifiedCellValue(dialog->m_notebook8->GetSelection(),row,col,col);
}

void Logbook::update()
{
	if(!modified) return;
	modified = false;

	dialog->logGrids[0]->Refresh();
	dialog->logGrids[1]->Refresh();
	dialog->logGrids[2]->Refresh();
	
	int count;
	if((count  = dialog->logGrids[0]->GetNumberRows() )== 0) { wxFile f; f.Create(data_locn,true); return; }
	
	wxString s = _T(""), temp;

	wxString newLocn = data_locn;
	newLocn.Replace(_T("txt"),_T("Bak"));
	wxRename(data_locn,newLocn);

	wxFileOutputStream output( data_locn );
	wxTextOutputStream* stream = new wxTextOutputStream (output);

	for(int r = 0; r < count; r++)
	{
		for(int g = 0; g < LOGGRIDS; g++)
		{
			for(int c = 0; c < dialog->logGrids[g]->GetNumberCols(); c++)
			{
				if(g == 1 && (c == HYDRO-weatherCol || c == TEMPAIR-weatherCol || c == TEMPWATER-weatherCol))
					continue;
				if(g == 2 && (c == MOTOR1-sailsCol  || c == MOTOR1T-sailsCol || 
					          c == GENE-sailsCol    || c == GENET-sailsCol   ||
					          c == WATERM-sailsCol  || c == WATERMT-sailsCol ||
							  c == WATERMO-sailsCol || c == BANK1-sailsCol   ||
							  c == BANK1T-sailsCol  || c == BANK2-sailsCol   ||
							  c == BANK2T-sailsCol  ))
					continue;
				temp = dialog->logGrids[g]->GetCellValue(r,c);
				s += dialog->replaceDangerChar(temp);
				s += _T(" \t");
			}
		}

		for(int ext = HYDRO-weatherCol; ext != WIND-weatherCol; ext ++) // extended 3 columns in weathergrid
		{
			temp = dialog->logGrids[1]->GetCellValue(r,ext);
			s += dialog->replaceDangerChar(temp);
			s += _T(" \t");
		}

		for(int ext = MOTOR1-sailsCol; ext <= MOTOR1T-sailsCol; ext ++) // extend MOTOR #1
		{
			temp = dialog->logGrids[2]->GetCellValue(r,ext);
			s += dialog->replaceDangerChar(temp);
			s += _T(" \t");
		}

		for(int ext = GENE-sailsCol; ext <= BANK2T-sailsCol; ext ++) // extend for GENERATOR and Battery-Banks
		{
			temp = dialog->logGrids[2]->GetCellValue(r,ext);
			s += dialog->replaceDangerChar(temp);
			s += _T(" \t");
		}

		for(int ext = WATERM-sailsCol; ext <= WATERMO-sailsCol; ext ++) // extend WATERMAKER
		{
			temp = dialog->logGrids[2]->GetCellValue(r,ext);
			s += dialog->replaceDangerChar(temp);
			s += _T(" \t");
		}

		s.RemoveLast();
		stream->WriteString(s+_T("\n"));
		s = _T("");
	}
	output.Close();
}

void  Logbook::getModifiedCellValue(int grid, int row, int selCol, int col)
{
	wxString s, wind, depth;

	modified = true;

	s = dialog->logGrids[grid]->GetCellValue(row,col);

	if((grid == 0 && (col == ROUTE || col == WAKE || col == REMARKS)) ||
		(grid == 1 && (col == WEATHER-weatherCol || col == CLOUDS-weatherCol || col == VISIBILITY-weatherCol)) ||
		(grid == 2 && (col == SAILS-sailsCol || col == REEF-sailsCol || col == MREMARKS-sailsCol)))
	{
		return;
	}

	if(grid == 0 && col == RDATE )
					{
						wxDateTime dt;

						if(!dialog->myParseDate(s,dt))
						{
							dt = dt.Now();
#ifdef __WXOSX__
                            wxString s = dt.FormatDate().c_str();
                            MessageBoxOSX(NULL,_("Please enter the Date in the format:\n   ") + s, _("Information"),wxID_OK);                     
#else
							wxMessageBox(wxString::Format(_("Please enter the Date in the format:\n      %s"),dt.FormatDate().c_str(),_("Information")));
#endif
							dialog->logGrids[grid]->SetCellValue(row,col,_T(""));
						}
						else
						{
							dialog->logGrids[grid]->SetCellValue(row,col,dt.FormatDate());

							if(row == dialog->m_gridGlobal->GetNumberRows()-1)
								dialog->maintenance->checkService(row);
						}
					}
	else if(grid == 0 && col == RTIME)
					{
						wxDateTime dt;
						const wxChar* c;
						c = dialog->myParseTime(s,dt);

						if(!c)
						{
#ifdef __WXOSX__
                            MessageBoxOSX(NULL, _("Please enter the Time in the format:\n   12:30:00PM"),_("Information"),wxID_OK);
#else
							wxMessageBox(_("Please enter the Time in the format:\n   12:30:00PM"));
#endif
							dialog->logGrids[grid]->SetCellValue(row,col,_T(""));
						}
						else
						{
							dialog->logGrids[grid]->SetCellValue(row,col,dt.FormatTime());
							if(row == dialog->m_gridGlobal->GetNumberRows()-1)
								dialog->maintenance->checkService(row);
						}
					}
	else if(grid == 0 && col == DISTANCE)
					{	
						s.Replace(_T(","),_T("."));

						s = wxString::Format(_T("%.2f %s"),wxAtof(s),opt->distance.c_str());
										
						s.Replace(_T("."),dialog->decimalPoint);
						dialog->logGrids[grid]->SetCellValue(row,col,s);

						computeCell(grid, row, col, s, true);
						if(row == dialog->m_gridGlobal->GetNumberRows()-1)
							dialog->maintenance->checkService(row);
						
						s.Replace(_T(","),_T("."));
						if(wxAtof(s) >= 0.01) 
							dialog->m_gridGlobal->SetCellValue(row,SIGN,_T("S"));
						else
							dialog->m_gridGlobal->SetCellValue(row,SIGN,_T(""));
					}
	
	else if(grid == 0 && col== SIGN)
					{
						dialog->logGrids[grid]->SetCellValue(row,col,s.Upper());
						if(row == dialog->m_gridGlobal->GetNumberRows()-1)
							dialog->maintenance->checkService(row);
					}
	else if(grid == 0 && col== POSITION)
					{ 
						if(s != _T("") && !s.Contains(opt->Deg)
							&& !s.Contains(opt->Min)
							&& !s.Contains(opt->Sec))
						{
							if(opt->traditional && s.length() != 22)
							{
#ifdef __WXOSX__
                                MessageBoxOSX(NULL,_("Please enter 0544512.15n0301205.15e for\n054Deg 45Min 12.15Sec N 030Deg 12Min 05.15Sec E"),_("Information"),wxID_OK);
#else
								wxMessageBox(_("Please enter 0544512.15n0301205.15e for\n054Deg 45Min 12.15Sec N 030Deg 12Min 05.15Sec E"),_("Information"),wxOK);
#endif
								s = _T("");
							}
							else if(!opt->traditional && s.length() != 22)
							{
#ifdef __WXOSX__
                                MessageBoxOSX(NULL,_("Please enter 05445.1234n03012.0504e for\n054Deg 45.1234Min N 030Deg 12.0504Min E"),_("Information"),wxID_OK);
#else
								wxMessageBox(_("Please enter 05445.1234n03012.0504e for\n054Deg 45.1234Min N 030Deg 12.0504Min E"),_("Information"),wxOK);
#endif
								s = _T("");
							}
							if(s == _T("")) return;
							s.Replace(_T(","),_T("."));

							if(opt->traditional)
							{
								wxString temp = s.SubString(0,2)+opt->Deg+_T(" ");
								temp += s.SubString(3,4) + opt->Min+_T(" ");
								temp += s.SubString(5,9) + opt->Sec+_T(" ");
								temp += s.SubString(10,10).Upper() + _T("\n");
								temp += s.SubString(11,13) + opt->Deg+_T(" ");
								temp += s.SubString(14,15) + opt->Min+_T(" ");
								temp += s.SubString(16,20) + opt->Sec+_T(" ");
								temp += s.SubString(21,21).Upper();
								s = temp;
							}
							else
							{
								wxString temp = s.SubString(0,2)+opt->Deg+_T(" ");
								temp += s.SubString(3,9) + opt->Min+_T(" ");
								temp += s.SubString(10,10).Upper() + _T("\n");
								temp += s.SubString(11,13) + opt->Deg+_T(" ");
								temp += s.SubString(14,20) + opt->Min+_T(" ");
								temp += s.SubString(21,22).Upper();
								s = temp;
							}
						}
						s.Replace(_T("."),dialog->decimalPoint);
						dialog->logGrids[grid]->SetCellValue(row,col,s);
						if(row != 0)
						{
							for(int i = row; i < dialog->logGrids[grid]->GetNumberRows(); i++)
							{
							double distTotal,dist;
							dialog->logGrids[grid]->SetCellValue(i,5,
								calculateDistance(dialog->logGrids[grid]->GetCellValue(i-1,col),s));
							wxString temp = dialog->logGrids[grid]->GetCellValue(i-1,6);
							temp.Replace(_T(","),_T("."));
							temp.ToDouble(&distTotal);
							temp = dialog->logGrids[grid]->GetCellValue(i,5);
							temp.Replace(_T(","),_T("."));
							temp.ToDouble(&dist);
							s= wxString::Format(_T("%9.2f %s"),distTotal+dist,opt->distance.c_str());
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(i,6,s);

							if(dist >= 0.1)
								dialog->m_gridGlobal->SetCellValue(i,3,_T("S"));
							else
								dialog->m_gridGlobal->SetCellValue(i,3,dialog->m_gridGlobal->GetCellValue(i-1,3));

							if(i < dialog->m_gridGlobal->GetNumberRows()-1)
							{
								s = dialog->logGrids[grid]->GetCellValue(i+1,col);
								if(s.IsEmpty() || s == _T(" "))
									break;
							}
							}
								
						}
					}
	else if(grid == 0 && col == COG)
					{
						if(s != _T(""))
						{
							s.Replace(_T(","),_T("."));
							s = wxString::Format(_T("%3.2f%s"),wxAtof(s),opt->Deg.c_str());
						    s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 0 && col == COW)
					{
						if(s != _T(""))
						{
							s.Replace(_T(","),_T("."));
							s = wxString::Format(_T("%3.2f%s %s"),wxAtof(s),opt->Deg.c_str(),(opt->showHeading)?_T("M"):_T("T"));
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 0 && (col == SOG || col == SOW))
					{
						if(s != _T(""))
						{
							s.Replace(_T(","),_T("."));
							s = wxString::Format(_T("%2.2f %s"),wxAtof(s),+opt->speed.c_str());
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 0 && col == DEPTH)
					{
						if(s != _T(""))
						{
							switch(opt->showDepth)
							{
								case 0:	depth = opt->meter; break;
								case 1: depth = opt->feet; break;
								case 2: depth = opt->fathom; break;
							}
							if(s.Contains(opt->meter) || 
								s.Contains(opt->feet) || 
								s.Contains(opt->fathom.c_str()) ||
								s.Contains(_T("--")))
							{
								s.Replace(_T("."),dialog->decimalPoint);
								dialog->logGrids[grid]->SetCellValue(row,col,s);
							}
							else
							{
								s.Replace(_T(","),_T("."));
								s = wxString::Format(_T("%3.1f %s"),wxAtof(s),depth.c_str());
								s.Replace(_T("."),dialog->decimalPoint);
								dialog->logGrids[grid]->SetCellValue(row,col,s);
							}
					
						}
					}
	else if(grid == 1 && col == BARO-weatherCol)
					{
						if(s != _T(""))
						{
							s = wxString::Format(_T("%u %s"),wxAtoi(s),opt->baro.c_str());
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 1 && col == HYDRO-weatherCol)
					{
						if(s != _T(""))
						{
							s.Replace(_T(","),_T("."));
							s = wxString::Format(_T("%4.1f%%"),wxAtof(s));
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 1 && col == TEMPAIR-weatherCol)
					{
						if(s != _T("") && !s.Contains(opt->Deg))
						{
							s = wxString::Format(_T("%3.0f %s %s"),wxAtof(s), opt->Deg.c_str(),opt->temperature.c_str());
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 1 && col == TEMPWATER-weatherCol)
					{
						if(s != _T("") && !s.Contains(opt->Deg))
						{
							s = wxString::Format(_T("%3.0f %s %s"),wxAtof(s), opt->Deg.c_str(),opt->temperature.c_str());
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 1 && col == WIND-weatherCol)
					{
						if(s != _T("") && !s.Contains(opt->Deg))
						{
							s = wxString::Format(_T("%3.0f%s %s"),wxAtof(s), opt->Deg.c_str(),opt->showWindDir?_T("R"):_T("T"));
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 1 && col == WSPD-weatherCol )
					{
						if(s != _T(""))
						{
							switch(opt->showWindSpeed)
							{
								case 0:	wind = opt->windkts; break;
								case 1: wind = opt->windmeter; break;
								case 2: wind = opt->windkmh; break;
							}
							s.Replace(_T(","),_T("."));
							s = wxString::Format(_T("%3.2f %s"),wxAtof(s),wind.c_str());
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 1 && col == CURRENT-weatherCol)
					{
						if(s != _T("") && !s.Contains(opt->Deg))
						{
							s.Replace(_T(","),_T("."));
							s = wxString::Format(_T("%3.0f%s"),wxAtof(s), opt->Deg.c_str());
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);						
						}
					}				
	else if(grid == 1 && col == CSPD-weatherCol)
					{
						if(s != _T(""))
						{
							s.Replace(_T(","),_T("."));
							s = wxString::Format(_T("%3.2f %s"),wxAtof(s),opt->speed.c_str());
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 1 && (col == WAVE-weatherCol || col == SWELL-weatherCol))
					{
						wxString d;
						switch(opt->showWaveSwell)
							{
								case 0:	d = opt->meter; break;
								case 1: d = opt->feet; break;
								case 2: d = opt->fathom; break;
							}
						if(s != _T(""))
						{
							s.Replace(_T(","),_T("."));
							s = wxString::Format(_T("%3.2f %s"),wxAtof(s),d.c_str());
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 2 && ((col == MOTORT-sailsCol || col == MOTOR1T-sailsCol || 
		                   col == GENET-sailsCol  || col == WATERMT-sailsCol )  && !s.IsEmpty()))
					{
						wxString pre, cur;
						double hp,hc,mp,mc;
						double res,hp_,hc_;

						if(!s.Contains(_T(":")) && !s.Contains(_T(",")) && !s.Contains(_T("."))) 
							s += _T(":");

						if(s.Contains(_T(",")) || s.Contains(_T(".")))
						{
							double d;
							s.Replace(_T(","),_T("."));
							s.ToDouble(&d);
							int h = (int) d;
							int m = (60*(d - h));
							s = wxString::Format(_T("%i:%i"),h,m);

						}

						if(row > 0)
						{
							pre = dialog->m_gridMotorSails->GetCellValue(row-1,col);
							wxStringTokenizer tkz(pre,_T(":"));
							tkz.GetNextToken().ToDouble(&hp);
							tkz.GetNextToken().ToDouble(&mp);
 
						}
						else
						{ hp = 0; mp = 0; }

						cur = s;//dialog->m_gridMotorSails->GetCellValue(row,col);
						wxStringTokenizer tkz1(cur,_T(":"));
						tkz1.GetNextToken().ToDouble(&hc);
						tkz1.GetNextToken().ToDouble(&mc);

						hc_ = hc + ((mc*(100.0/60.0))/100);
						hp_ = hp + ((mp*(100.0/60.0))/100);

						res = hc_ - hp_;

						if(row == 0 || res <= 0.0)
							dialog->m_gridMotorSails->SetCellValue(row,col-1,wxString::Format(_T("00:00 %s"),opt->motorh.c_str()));
						else
							dialog->m_gridMotorSails->SetCellValue(row,col-1,decimalToHours(res,false));

						dialog->m_gridMotorSails->SetCellValue(row,col,  decimalToHours(hc_,true));

						if(row < dialog->m_gridMotorSails->GetNumberRows()-1)
							computeCell(grid,row+1,col-1,dialog->m_gridMotorSails->GetCellValue(row+1,col-1),true);
						if(row == dialog->m_gridGlobal->GetNumberRows()-1)
									dialog->maintenance->checkService(row);
					}
	else if(grid == 2 && ((col == MOTOR-sailsCol || col == MOTOR1-sailsCol || 
		                   col == GENE-sailsCol  || col == WATERM-sailsCol)  && !s.IsEmpty()))
					{
						double watermaker;
						opt->watermaker.ToDouble(&watermaker);
/*						if(col == WATERM-sailsCol && watermaker == 0.0)
						{
#ifdef __WXOSX__
							MessageBoxOSX(_("Please set a value for Watermaker in preferences/capacity"),_("Information"));
#else
							wxMessageBox(_("Please set a value for Watermaker in preferences/capacity"),_("Information"));
#endif
						}
*/
						bool t = false;
						wxString sep;

						if(s.Contains(_T(".")))
						{
							t = true;
							sep = _T(".");
						}
						if(s.Contains(_T(",")))
						{
							t = true;
							sep = _T(",");
						}
						if(s.Contains(_T(":")))
						{
							t = true;
							sep = _T(":");
						}

						if(true != t)
							s.Append(_T(":0"));

/*						if(t != true)
							{
#ifdef __WXOSX__
                                MessageBoxOSX(NULL,_("Please enter like 1.30 or 1,30 or 1:30\nfor 1:30h"),_T(""),wxID_OK);
#else
								wxMessageBox(_("Please enter like 1.30 or 1,30 or 1:30\nfor 1:30h"),_T(""),wxOK);
#endif
								dialog->logGrids[grid]->SetCellValue(row,col,_T("00:00"));
							}
*/
						wxStringTokenizer tkz(s,sep);
						wxString h = tkz.GetNextToken();
						wxString m = tkz.GetNextToken();
						if(wxAtoi(m) > 59)
							{
#ifdef __WXOSX__
                                MessageBoxOSX(NULL,_("Minutes greater than 59"),_T(""),wxID_OK);
#else
								wxMessageBox(_("Minutes greater than 59"),_T(""));
#endif
								dialog->logGrids[grid]->SetCellValue(row,col,_T("00:00"));
								return;
							}
						else
							{
								s = wxString::Format(_T("%s:%s"),h.c_str(),m.c_str());
								s = wxString::Format(_T("%s %s"),s.c_str(),opt->motorh.c_str());
								dialog->logGrids[grid]->SetCellValue(row,col,s);
								computeCell(grid, row, col,s, true);
								if(row == dialog->m_gridGlobal->GetNumberRows()-1)
									dialog->maintenance->checkService(row);
								if(col == WATERM-sailsCol)
								{
									wxString t = dialog->m_gridMotorSails->GetCellValue(row,Logbook::WATERM-sailsCol);
									wxStringTokenizer tkz(t,_T(":"));
									double h,m;
									tkz.GetNextToken().ToDouble(&h);
									tkz.GetNextToken().ToDouble(&m);
									h = h + (m*(100/60)/100);
									double output = watermaker * h;
									dialog->m_gridMotorSails->SetCellValue(row,Logbook::WATERMO-sailsCol,wxString::Format(_T("+%2.2f %s"),output,opt->vol.c_str()));
									computeCell(grid,row,Logbook::WATERMO-sailsCol, dialog->m_gridMotorSails->GetCellValue(row,Logbook::WATERMO-sailsCol),false);
								}
							}						
					}

	else if(grid == 2 && ( col == FUELT-sailsCol || col == WATERT-sailsCol ||
		                   col == BANK1T-sailsCol || col == BANK2T-sailsCol) && !s.IsEmpty())
					{
						double div = 1.0;
						long capacity;
						wxString ap;
						double t,c;
						wxString ind;

						if(col == BANK1T-sailsCol || col == BANK2T-sailsCol)
							ap = opt->ampereh;
						else
							ap = opt->vol;

						if(row > 0)
							dialog->m_gridMotorSails->GetCellValue(row-1,col).ToDouble(&t);						
						else
							{ t = 0; c = 0; }
						dialog->m_gridMotorSails->GetCellValue(row,col).ToDouble(&c);

						if(s.Contains(_T("/")))
						{
							double a,b;
							wxStringTokenizer tkz(s,_T("/"));
							tkz.GetNextToken().ToDouble(&a);
							tkz.GetNextToken().ToDouble(&b);
							div = a/b;
							if(col == FUELT-sailsCol) 
								opt->fuelTank.ToLong(&capacity);
							else if( col == BANK1T-sailsCol)
								opt->bank1.ToLong(&capacity);
							else if(col == BANK2T-sailsCol)
								opt->bank2.ToLong(&capacity);
							else
								opt->waterTank.ToLong(&capacity);

							c = capacity*div;						
						}

						s.Replace(_T(","),_T("."));
						ind = (c < t)?_T("-"):_T("+");

						if(row != 0)
								dialog->m_gridMotorSails->SetCellValue(row,col-1,wxString::Format(_T("%s%.2f %s"),ind.c_str(),fabs(t-c),ap.c_str()));
						else
								dialog->m_gridMotorSails->SetCellValue(row,col-1,wxString::Format(_T("%s0.00 %s"),ind.c_str(),ap.c_str()));

							dialog->m_gridMotorSails->SetCellValue(row,col,wxString::Format(_T("%.2f %s"),c,ap.c_str()));
						
						int x;
						if(col == WATERT-sailsCol)
							x = 2;
						else
							x =1;
						if(row < dialog->m_gridMotorSails->GetNumberRows()-1)
							computeCell(grid,row+1,col-x,dialog->m_gridMotorSails->GetCellValue(row+1,col-x),false);

						dialog->maintenance->checkService(row);
					}
	else if(grid == 2 && ( col == FUEL-sailsCol    || col == WATER-sailsCol || 
		                   col == WATERMO-sailsCol || col == BANK1-sailsCol || col == BANK2-sailsCol) && !s.IsEmpty())
					{
						wxChar ch;
						wxString ap;

						if(col == BANK1-sailsCol || col == BANK2-sailsCol)
							ap = opt->ampereh;
						else
							ap = opt->vol;

						s.Replace(_T(","),_T("."));
						if(col != WATERMO-sailsCol)
							ch = s.GetChar(0);
						else 
							ch = '+';

						s = wxString::Format(_T("%.2f %s"),wxAtof(s),ap.c_str());
						s.Replace(_T("."),dialog->decimalPoint);
						
						if(ch != '-' && ch != '+')
							dialog->logGrids[grid]->SetCellValue(row,col,_T("-")+s);
						else
						{
							if(ch == '+')
								dialog->logGrids[grid]->SetCellValue(row,col,wxString(ch)+s);
							else
								dialog->logGrids[grid]->SetCellValue(row,col,s);
						}

						computeCell(grid, row, col,s, false);
						//if(row == dialog->m_gridGlobal->GetNumberRows()-1)
									dialog->maintenance->checkService(row);
					}
	return;
}

wxString  Logbook::decimalToHours(double res,bool b)
{
	int h = (int) res;
	double m = res - h;
	m = m * (60.0/100.0)*100;

	wxString fmt = (b)?_T("%05i:%02.0f %s"):_T("%02i:%02.0f %s");
	wxString str =  wxString::Format(fmt,h,m,opt->motorh.c_str());
	return str;
}

wxString Logbook::computeCell(int grid, int row, int col, wxString s, bool mode)
{
	double current = 0, last = 0.0;
	long hourCur=0, minCur=0 ,hourLast=0, minLast = 0;
	int count;
	wxString cur;
	wxString abrev;

	s.Replace(_T(","),_T("."));

	if(col == DISTANCE)
	   abrev = opt->distance; 
	else if(col == MOTOR-sailsCol || col == MOTOR1-sailsCol || 
		    col == GENE-sailsCol || col == WATERM-sailsCol)
	   abrev = opt->motorh; 
	else if(col == FUEL-sailsCol || col == WATER-sailsCol ||
		    col == WATERMO-sailsCol)
	   abrev = opt->vol; 
	else if(col == BANK1-sailsCol || col == BANK2-sailsCol)
		abrev = opt->ampereh;


	count = dialog->m_gridGlobal->GetNumberRows();

	for(int i = row; i < count; i++)
	{
		if(col != WATERMO-sailsCol && col != WATER-sailsCol && col != FUEL-sailsCol &&
		   col != BANK1-sailsCol   && col != BANK2-sailsCol)
		{
			s = dialog->logGrids[grid]->GetCellValue(i,col);
			s.Replace(_T(","),_T("."));
			if(s == _T("0000")) s = _T("00:00");
			if(grid == 2 && (col == MOTOR-sailsCol || col == MOTOR1-sailsCol || 
				             col == GENE-sailsCol  || col == WATERM-sailsCol))
			{
				wxArrayString time = wxStringTokenize(s,_T(":"));
				time[0].ToLong(&hourCur);
				time[1].ToLong(&minCur);
			}
			else
			{
				s.ToDouble(&current);
			}
		}
		else
		{
			double t,t1 = 0.0 ,t2 = 0.0;
			//int ind = 1;

			s = dialog->logGrids[grid]->GetCellValue(i,col);
			s.Replace(_T(","),_T("."));
			s.ToDouble(&t);

			if(col == WATERMO-sailsCol)
			{
				s = dialog->logGrids[grid]->GetCellValue(i,WATER-sailsCol);
				s.Replace(_T(","),_T("."));
				s.ToDouble(&t1);

				if(i == 0)
				{
					s = dialog->logGrids[grid]->GetCellValue(i,WATERT-sailsCol);
					s.Replace(_T(","),_T("."));
					s.ToDouble(&t2);
				}
				current = t + t1 + t2;
			}
			else if(col == WATER-sailsCol)
			{
				s = dialog->logGrids[grid]->GetCellValue(i,WATERMO-sailsCol);
				s.Replace(_T(","),_T("."));
				s.ToDouble(&t1);

				if(i == 0)
				{
					s = dialog->logGrids[grid]->GetCellValue(i,WATERT-sailsCol);
					s.Replace(_T(","),_T("."));
					s.ToDouble(&t2);

					current = t + t2;
				}
				else
				{ current = t + t1 + t2; }

			}
			else
			{
				current = t + t1;
			}
		}

		if(i > 0)
		{
			wxString temp;
			if(col != WATERMO-sailsCol)
				 temp = dialog->logGrids[grid]->GetCellValue(i-1,col+1);
			else
				 temp = dialog->logGrids[grid]->GetCellValue(i-1,col+2);

			temp.Replace(_T(","),_T("."));
			if(grid == 2 && (col == MOTOR-sailsCol || col == MOTOR1-sailsCol || 
				             col == GENE-sailsCol  || col == WATERM-sailsCol))
			{
				if(temp.Contains(_T(":")))
				{
					wxArrayString time = wxStringTokenize(temp,_T(":"));
					time[0].ToLong(&hourLast);
					time[1].ToLong(&minLast);
				}
				else { hourLast = 0; minLast = 0; }
			}
			else
				temp.ToDouble(&last);
		}
		else { 
				/*				s = dialog->logGrids[grid]->GetCellValue(i,col);
					s.Replace(_T(","),_T("."));
					s.ToDouble(&last);*/
				last = 0.0f; hourLast = 0; minLast = 0; 
				
			 }
		
		if(grid == 2 && (col == MOTOR-sailsCol || col == MOTOR1-sailsCol || 
			             col == GENE-sailsCol  || col == WATERM-sailsCol))
			{
				hourLast += hourCur;
				minLast  += minCur;
				if(minLast >= 60) { hourLast++; minLast -= 60; }
				s = wxString::Format(_T("%05u:%02u %s"),hourLast,minLast,abrev.c_str());
				dialog->logGrids[grid]->SetCellValue(i,col+1,s);
				cur = wxString::Format(_T("%02u:%02u %s"),hourCur,minCur,abrev.c_str());
				dialog->logGrids[grid]->SetCellValue(i,col,cur);
			}
		else
			{
				s = wxString::Format(_T("%10.2f %s"),last+current,abrev.c_str());
				s.Replace(_T("."),dialog->decimalPoint);
				if(col != WATERMO-sailsCol)
					dialog->logGrids[grid]->SetCellValue(i,col+1,s);
				else
					dialog->logGrids[grid]->SetCellValue(i,col+2,s);
			}	
	}
	return cur;
}

wxString Logbook::toSDMM ( int NEflag, double a, bool mode )
{
      short neg = 0;
      int d;
      long m;
	  wxDouble sec;

      if ( a < 0.0 )
      {
            a = -a;
            neg = 1;
      }
      d = ( int ) a;
      m = ( long ) ( ( a - ( double ) d ) * 60000.0 );
	  double z = (m % 1000);
	  sec = 60 * (z/1000) ;

      if ( neg )
            d = -d;

      wxString s;

      if ( !NEflag )
            s.Printf ( _T ( "%d%02ld%02ld" ), d, m / 1000, m % 1000 );
      else
      {
            if ( NEflag == 1 )
            {
                  char c = 'N';

                  if ( neg )
                  {
                        d = -d;
                        c = 'S';
                  }
					newPosition.posLat = a;
					newPosition.latitude = d;
					newPosition.latmin   = m / 1000.0;
					newPosition.WEflag = c;

				  s.Printf ( _T("%03d%02ld%05.2f%c"), d, m / 1000, sec , c );
            }
            else if ( NEflag == 2 )
            {
                  char c = 'E';

                  if ( neg )
                  {
                        d = -d;
                        c = 'W';
                  }
					newPosition.posLon = a;
					newPosition.longitude = d;
					newPosition.lonmin   = m / 1000.0;
					newPosition.NSflag = c;
					s.Printf ( _T("%03d%02ld%05.2f%c"), d, m / 1000, sec, c );
            }
      }
      return s;
}

wxString Logbook::toSDMMOpenCPN ( int NEflag, double a, bool hi_precision )
{
      wxString s;
      double mpy;
      short neg = 0;
      int d;
      long m;
      double ang = a;
      char c = 'N';
	  int g_iSDMMFormat = 0;

      if ( a < 0.0 )
      {
            a = -a;
            neg = 1;
      }
      d = ( int ) a;
      if ( neg )
            d = -d;
      if(NEflag)
      {
            if ( NEflag == 1 )
            {
                  c = 'N';

                  if ( neg )
                  {
                        d = -d;
                        c = 'S';
                  }
            }
            else if ( NEflag == 2 )
            {
                  c = 'E';

                  if ( neg )
                  {
                        d = -d;
                        c = 'W';
                  }
            }
      }

	  switch (g_iSDMMFormat)
      {
      case 0:
            mpy = 600.0;
            if(hi_precision)
                  mpy = mpy * 1000;

            m = ( long ) wxRound( ( a - ( double ) d ) * mpy );

            if ( !NEflag || NEflag < 1 || NEflag > 2) //Does it EVER happen?
            {
                  if(hi_precision)
					  s.Printf ( _T ( "%d %02ld.%04ld'" ), d, m / 10000, m % 10000 );
                  else
					  s.Printf ( _T ( "%d %02ld.%01ld'" ), d, m / 10,   m % 10 );
            }
            else
            {
				if(NEflag == 1)
				{
					newPosition.posLat = a;
					newPosition.latitude = d;
					newPosition.latmin   = m / 1000.0;
					newPosition.WEflag = c;
				}
				else
				{
					newPosition.posLon = a;
					newPosition.longitude = d;
					newPosition.lonmin   = m / 1000.0;
					newPosition.NSflag = c;
				}
                  if(hi_precision)
					  s.Printf ( _T ( "%03d%02ld.%04ld%c" ), d, m / 10000,( m % 10000 ), c );
                   else
					   s.Printf ( _T ( "%03d%02ld.%01ld%c" ), d, m / 10,   ( m % 10 ), c );
            }
            break;
      case 1:
            if (hi_precision)
                  s.Printf (_T ( "%03.6f" ), ang); //cca 11 cm - the GPX precision is higher, but as we use hi_precision almost everywhere it would be a little too much....
            else
                  s.Printf (_T ( "%03.4f" ), ang); //cca 11m
            break;
      case 2:
            m = ( long ) ( ( a - ( double ) d ) * 60 );
            mpy = 10.0;
            if (hi_precision)
                  mpy = mpy * 100;
            long sec = (long) (( a - ( double ) d - ( ( ( double ) m ) / 60 ) ) * 3600 * mpy);

            if ( !NEflag || NEflag < 1 || NEflag > 2) //Does it EVER happen?
            {
                  if(hi_precision)
					  s.Printf ( _T ( "%d %ld'%ld.%ld\"" ), d, m, sec / 1000, sec % 1000 );
                  else
					  s.Printf ( _T ( "%d %ld'%ld.%ld\"" ), d, m, sec / 10, sec % 10 );
            }
            else
            {
                  if(hi_precision)
					  s.Printf ( _T ( "%03d %02ld %02ld.%03ld %c" ), d, m, sec / 1000, sec % 1000, c );
                   else
					   s.Printf ( _T ( "%03d %02ld %02ld.%ld %c" ), d, m,sec / 10, sec % 10, c );
            }
            break;
      }
      return s;
}

bool Logbook::checkGPS(bool appendClick)
{
	sLogText = _T("");

	if(gpsStatus)
	{
		if(opt->showWindHeading == 1 && !bCOW)
		{
			sLogText = _("Wind is set to Heading,\nbut GPS sends no Heading Data.\nWind is set now to Realative to boat\n\n"); 
			opt->showWindHeading = 0;
		}
		if(courseChange && !appendClick)
			sLogText += opt->courseChangeText+opt->courseChangeDegrees+opt->Deg;
		else if(guardChange)
			sLogText += opt->guardChangeText;
		else if(waypointArrived)
		{
			wxString s, ext;

			if(!OCPN_Message)
			{	/* s = wxString::Format(_("\nName of Waypoint: %s\nTrue bearing to destination: %4.1f%s\nRange to destination: %4.2f%s"),
																	tempRMB.From.c_str(),
																	tempRMB.BearingToDestinationDegreesTrue,opt->Deg.c_str(),
																	tempRMB.RangeToDestinationNauticalMiles,opt->distance.c_str());
				s.Replace(_T("."),dialog->decimalPoint);*/
			}
			else
			{
				setWayPointArrivedText();
			}
			
		}
		else if(everySM && !appendClick)
			sLogText += opt->everySMText+opt->everySMAmount+opt->distance;
		else if(dialog->timer->IsRunning() && !appendClick)
			sLogText += opt->ttext;
			
		return true;
	}
	else
	{
		sLat = sLon = sDate = sTime = _T("");
		sCOG = sCOW = sSOG = sSOW = sDepth = sWind = sWindSpeed = sTemperatureWater = _T("");
		bCOW = false;
		if(opt->noGPS)
			sLogText = _("No GPS-Signal !");
		else
			sLogText = _T("");
		if(waypointArrived)
		{
			setWayPointArrivedText();
		}
		return false;
	}
}

void Logbook::setWayPointArrivedText()
{
			wxString ext;
			wxString msg;

			if(tempRMB.To != _T("-1"))
				{ msg = _("Next WP Name: "); }
			else
				{ msg = _("Last waypoint of the Route"); tempRMB.To = wxEmptyString; }

			wxString s = wxString::Format(_("\nName of Waypoint: %s\n%s %s"),tempRMB.From.c_str(), msg.c_str(), tempRMB.To.c_str());

			if(WP_skipped)
				ext = _("Waypoint skipped");
			else
				ext = _("WayPoint arrived");

			if(sLogText != _T(""))
				sLogText += wxString::Format(_T("\n%s\n%s%s"),opt->waypointText.c_str(),ext.c_str(),s.c_str());
			else
				sLogText += wxString::Format(_T("%s\n%s%s"),opt->waypointText.c_str(),ext.c_str(),s.c_str());
}

void Logbook::SetGPSStatus(bool status)
{
	gpsStatus = status;
}

void Logbook::showSearchDlg(int row, int col)
{
	LogbookSearch* dlg = new LogbookSearch(dialog, row, col);
	dlg->Show(true);

/*	while(shown)
	{


	}
*/
}

////////////////////////////////////////////////////
NoAppendDialog::NoAppendDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText73 = new wxStaticText( this, wxID_ANY, _("It's not allowed to append Data to a old Logbook\n\n\
OpenCPN switchs to actuell logbook"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_staticText73->Wrap( -1 );
	bSizer20->Add( m_staticText73, 0, wxALL|wxEXPAND, 5 );
	
	m_sdbSizer5 = new wxStdDialogButtonSizer();
	m_sdbSizer5OK = new wxButton( this, wxID_OK );
	m_sdbSizer5->AddButton( m_sdbSizer5OK );
	m_sdbSizer5->Realize();
	bSizer20->Add( m_sdbSizer5, 0, wxALIGN_CENTER, 5 );
	
	this->SetSizer( bSizer20 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

NoAppendDialog::~NoAppendDialog()
{
}

////////////////////////////
// PVBE-DIALOG
////////////////////////////
PBVEDialog::PBVEDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	dialog = (LogbookDialog*)parent;
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrlPVBE = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	bSizer21->Add( m_textCtrlPVBE, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer21 );
	this->Layout();
	
	this->Centre( wxBOTH );
	m_textCtrlPVBE->AppendText(_T("this is a test if you have received PBVE-Sentences\nthey are manufacturer-specific\nit's use is for engine-hours and fuel-consumption\n"));
}

PBVEDialog::~PBVEDialog()
{
	dialog->logbook->pvbe = NULL;
}

void PBVEDialog::PBVEDialogOnClose(wxCloseEvent &event)
{
	dialog->logbook->pvbe = NULL;
}

void PBVEDialog::OnCloseWindow(wxCloseEvent& ev)
{
	dialog->logbook->pvbe = NULL;
}

/////////////////////// LogbookSearchDlg ////////////////////////////////////////////////////

LogbookSearch::LogbookSearch( wxWindow* parent, int row, int col, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->parent = (LogbookDialog*)parent;
	this->row = row;
	this->col = col;

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer41;
	fgSizer41 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer41->SetFlexibleDirection( wxBOTH );
	fgSizer41->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText96 = new wxStaticText( this, wxID_ANY, _("Search in"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText96->Wrap( -1 );
	fgSizer41->Add( m_staticText96, 0, wxALL, 5 );
	
	m_radioBtnActuell = new wxRadioButton( this, wxID_ANY, _("Actuell Logbook"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer41->Add( m_radioBtnActuell, 0, wxALL, 5 );
	
	m_radioBtnAll = new wxRadioButton( this, wxID_ANY, _("All Logbooks"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer41->Add( m_radioBtnAll, 0, wxALL, 5 );
	
	bSizer23->Add( fgSizer41, 0, wxALIGN_CENTER, 5 );
	
	m_staticline32 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer23->Add( m_staticline32, 0, wxEXPAND | wxALL, 5 );
	
	wxFlexGridSizer* fgSizer411;
	fgSizer411 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer411->SetFlexibleDirection( wxBOTH );
	fgSizer411->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText108 = new wxStaticText( this, wxID_ANY, _("Searchstring"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText108->Wrap( -1 );
	fgSizer411->Add( m_staticText108, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrl72 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 250,-1 ), wxTE_LEFT|wxTE_MULTILINE );
	fgSizer411->Add( m_textCtrl72, 0, wxALL, 5 );
	
	m_staticText110 = new wxStaticText( this, wxID_ANY, _("In Column"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText110->Wrap( -1 );
	fgSizer411->Add( m_staticText110, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choice23Choices;
	m_choice23 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxSize( 250,-1 ), m_choice23Choices, 0 );
	m_choice23->SetSelection( 0 );
	fgSizer411->Add( m_choice23, 0, wxALL, 5 );
	
	m_staticText97 = new wxStaticText( this, wxID_ANY, _("Date"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText97->Wrap( -1 );
	fgSizer411->Add( m_staticText97, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxFlexGridSizer* fgSizer42;
	fgSizer42 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer42->SetFlexibleDirection( wxBOTH );
	fgSizer42->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxString m_choiceGreaterEqualChoices[] = { wxT(">="), wxT("<=") };
	int m_choiceGreaterEqualNChoices = sizeof( m_choiceGreaterEqualChoices ) / sizeof( wxString );
	m_choiceGreaterEqual = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceGreaterEqualNChoices, m_choiceGreaterEqualChoices, 0 );
	m_choiceGreaterEqual->SetSelection( 0 );
	fgSizer42->Add( m_choiceGreaterEqual, 0, wxALL, 5 );
	
	m_datePicker = new wxDatePickerCtrl( this, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxDP_DEFAULT );
	fgSizer42->Add( m_datePicker, 0, wxALL, 5 );
	
	m_buttonSelectDate = new wxButton( this, wxID_ANY, _("Select"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer42->Add( m_buttonSelectDate, 0, wxALL, 5 );
	
	fgSizer411->Add( fgSizer42, 1, wxEXPAND, 5 );
	
	bSizer23->Add( fgSizer411, 0, wxEXPAND, 5 );
	
	m_staticline39 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer23->Add( m_staticline39, 0, wxEXPAND | wxALL, 5 );
	
	wxFlexGridSizer* fgSizer43;
	fgSizer43 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer43->SetFlexibleDirection( wxBOTH );
	fgSizer43->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_buttonBack = new wxButton( this, wxID_ANY, wxT("<<"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer43->Add( m_buttonBack, 0, wxALL, 5 );
	
	m_buttonForward = new wxButton( this, wxID_ANY, wxT(">>"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer43->Add( m_buttonForward, 0, wxALIGN_CENTER|wxALL, 5 );
	
	bSizer23->Add( fgSizer43, 0, wxALIGN_CENTER, 0 );
	
	this->SetSizer( bSizer23 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( LogbookSearch::OnInitDialog ) );
	m_buttonBack->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogbookSearch::OnButtonClickBack ), NULL, this );
	m_buttonForward->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogbookSearch::OnButtonClickForward ), NULL, this );
	m_buttonSelectDate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogbookSearch::OnButtonClickSelectDate ), NULL, this );
}

LogbookSearch::~LogbookSearch()
{
	// Disconnect Events
	this->Disconnect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( LogbookSearch::OnInitDialog ) );
	m_buttonBack->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogbookSearch::OnButtonClickBack ), NULL, this );
	m_buttonForward->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogbookSearch::OnButtonClickForward ), NULL, this );
	m_buttonSelectDate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogbookSearch::OnButtonClickSelectDate ), NULL, this );
	
}

void LogbookSearch::OnInitDialog( wxInitDialogEvent& event )
{
	wxDateTime dt;
	searchrow = 0;

	parent->myParseDate(parent->m_gridGlobal->GetCellValue(0,1),dt);
	m_datePicker->SetValue(dt);

	int gridNo = parent->m_notebook8->GetSelection();
	for(int i = 0; i < parent->logGrids[gridNo]->GetNumberCols(); i++)
		this->m_choice23->Append(parent->logGrids[gridNo]->GetColLabelValue(i));

	m_choice23->SetSelection(col);
	m_textCtrl72->SetFocus();

	m_radioBtnAll->Hide();
	Fit();
}

void LogbookSearch::OnButtonClickSelectDate( wxCommandEvent& event )
{
	DateDialog dateDlg(this);
	if(dateDlg.ShowModal() == wxID_OK)
		this->m_datePicker->SetValue(dateDlg.m_calendar2->GetDate());
}

void LogbookSearch::OnButtonClickForward( wxCommandEvent& event )
{
	int gridNo = parent->m_notebook8->GetSelection();
	int col = this->m_choice23->GetSelection();
	wxString ss = this->m_textCtrl72->GetValue().Lower();
	wxDateTime dt,dlgDt;

	dlgDt = m_datePicker->GetValue();
	if(searchrow < 0 ) searchrow = 0 ;
	if(!direction) searchrow++;
	direction = true;

	for(; searchrow < parent->logGrids[gridNo]->GetNumberRows(); searchrow++)
	{
		parent->myParseDate(parent->logGrids[0]->GetCellValue(searchrow,Logbook::RDATE),dt);

		if(m_choiceGreaterEqual->GetSelection() == 0)
		{
			if(dt < dlgDt) 
				continue;
		}
		else
		{
			if(dt > dlgDt) 
				continue;
		}

		if(parent->logGrids[gridNo]->GetCellValue(searchrow,col).Lower().Contains(ss))
		{
			parent->logGrids[gridNo]->SetFocus();
			parent->logGrids[gridNo]->SetGridCursor(searchrow,col);
			searchrow++;
			break;
		}
	}
}

void LogbookSearch::OnButtonClickBack( wxCommandEvent& event )
{
	int gridNo = parent->m_notebook8->GetSelection();
	int col = this->m_choice23->GetSelection();
	wxString ss = this->m_textCtrl72->GetValue().Lower();
	wxDateTime dt,dlgDt;

	if(direction) searchrow--;
	direction = false;

	dlgDt = m_datePicker->GetValue();
	if(searchrow > parent->logGrids[gridNo]->GetNumberRows()-1) searchrow--;

	for(; searchrow >= 0; searchrow--)
	{
		parent->myParseDate(parent->logGrids[0]->GetCellValue(searchrow,Logbook::RDATE),dt);
		if(m_choiceGreaterEqual->GetSelection() == 0)
		{
			if(m_choiceGreaterEqual->GetSelection() == 0)
			{
				if(dt < dlgDt) 
				continue;
			}
			else
			{
				if(dt > dlgDt) 
				continue;
			}
		}

		if(parent->logGrids[gridNo]->GetCellValue(searchrow,col).Lower().Contains(ss))
		{
			parent->logGrids[gridNo]->SetFocus();
			parent->logGrids[gridNo]->SetGridCursor(searchrow,col);
			searchrow--;
			break;
		}
	}
}