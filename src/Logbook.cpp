#ifndef WX_PRECOMP
      #include <wx/wx.h>
#endif

#define PI 3.14159265

#include "LogbookDialog.h"
#include "Logbook.h"
#include "LogbookHTML.h"
#include "logbook_pi.h"
#include "Options.h"

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
	noSentence = true;
	modified = false;
	wxString logLay;

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

	loadData();

	noAppend = false;
	gpsStatus = false;
	bCOW = false;
	dCOW = -1;
	dLastHeading = -1;
	courseChange = false;
	everySM = false;
	guardChange = false;
	dLastMinute = -1;
	oldPosition.latitude = 500;

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
	mUTCDateTime.Set(pfix.FixTime);
}
void Logbook::clearNMEAData()
{
	noSentence = true;
}

void Logbook::SetSentence(wxString &sentence)
{
	m_NMEA0183 << sentence;

	if(m_NMEA0183.PreParse())
    {
		noSentence = false;
		if(m_NMEA0183.LastSentenceIDReceived == _T("GGA"))
            {
                  if(m_NMEA0183.Parse())
                  {
                        if(m_NMEA0183.Gga.GPSQuality > 0)
                        {
                                    double lat, lon;
                                    float llt = m_NMEA0183.Gga.Position.Latitude.Latitude;
                                    int lat_deg_int = (int)(llt / 100);
                                    float lat_deg = lat_deg_int;
                                    float lat_min = llt - (lat_deg * 100);
                                    lat = lat_deg + (lat_min/60.);
                                    if(m_NMEA0183.Gga.Position.Latitude.Northing == South)
                                          lat = -lat;
									sLat = this->toSDMM(1,lat, true);

                                    float lln = m_NMEA0183.Gga.Position.Longitude.Longitude;
                                    int lon_deg_int = (int)(lln / 100);
                                    float lon_deg = lon_deg_int;
                                    float lon_min = lln - (lon_deg * 100);
                                    lon = lon_deg + (lon_min/60.);
                                    if(m_NMEA0183.Gga.Position.Longitude.Easting == West)
                                          lon = -lon;
									sLon = this->toSDMM(2,lon, false);

									gpsStatus = true;
									dialog->GPSTimer->Start(5000);
									if(opt->everySM)
										checkDistance();
                        }
                  }
            }
		else if(m_NMEA0183.LastSentenceIDReceived == _T("GLL"))
            {
                  if(m_NMEA0183.Parse())
                  {
                                    double lat, lon;
									float llt = m_NMEA0183.Gll.Position.Latitude.Latitude;
                                    int lat_deg_int = (int)(llt / 100);
                                    float lat_deg = lat_deg_int;
                                    float lat_min = llt - (lat_deg * 100);
                                    lat = lat_deg + (lat_min/60.);
                                    if(m_NMEA0183.Gll.Position.Latitude.Northing == South)
                                          lat = -lat;
									sLat = this->toSDMM(1,lat, true);

									float lln = m_NMEA0183.Gll.Position.Longitude.Longitude;
                                    int lon_deg_int = (int)(lln / 100);
                                    float lon_deg = lon_deg_int;
                                    float lon_min = lln - (lon_deg * 100);
                                    lon = lon_deg + (lon_min/60.);
                                    if(m_NMEA0183.Gll.Position.Longitude.Easting == West)
                                          lon = -lon;
									sLon = this->toSDMM(2,lon, false);

									gpsStatus = true;
									dialog->GPSTimer->Start(5000);
									if(opt->everySM)
										checkDistance();
                  }
            }
			else if(m_NMEA0183.LastSentenceIDReceived == _T("ZDA"))
            {
                  if(m_NMEA0183.Parse())
                  {
                    wxString dt;
                    dt.Printf(_T("%4d%02d%02d"), 
						m_NMEA0183.Zda.Year, m_NMEA0183.Zda.Month, m_NMEA0183.Zda.Day);
                    dt.Append(m_NMEA0183.Zda.UTCTime);

                    mUTCDateTime.ParseFormat(dt.c_str(), _T("%Y%m%d%H%M%S"));
					if(opt->local)
					{
						wxTimeSpan span(opt->tzHour, 0, 0, 0);
						if(opt->tzIndicator == 0)
							mCorrectedDateTime = mUTCDateTime + span;
						else
							mCorrectedDateTime = mUTCDateTime - span;
					}
					else
						mCorrectedDateTime = mUTCDateTime;

					sDate = mCorrectedDateTime.FormatDate();
					sTime = mCorrectedDateTime.FormatTime();
					if(opt->guardChange)
						checkGuardChanged();
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
					  if(opt->courseChange)
						checkCourseChanged();
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
					  if(opt->courseChange)
						checkCourseChanged();
				  }
			}
			else if(m_NMEA0183.LastSentenceIDReceived == _T("RMC"))
            {
                  if(m_NMEA0183.Parse())
                  {
                    double lat, lon;
					float llt = m_NMEA0183.Rmc.Position.Latitude.Latitude;
                    int lat_deg_int = (int)(llt / 100);
                    float lat_deg = lat_deg_int;
                    float lat_min = llt - (lat_deg * 100);
                    lat = lat_deg + (lat_min/60.);
                    if(m_NMEA0183.Rmc.Position.Latitude.Northing == South)
                            lat = -lat;
					sLat = this->toSDMM(1,lat, true);

					float lln = m_NMEA0183.Rmc.Position.Longitude.Longitude;
                    int lon_deg_int = (int)(lln / 100);
                    float lon_deg = lon_deg_int;
                    float lon_min = lln - (lon_deg * 100);
                    lon = lon_deg + (lon_min/60.);
                    if(m_NMEA0183.Rmc.Position.Longitude.Easting == West)
                             lon = -lon;
					sLon = this->toSDMM(2,lon, false);

					gpsStatus = true;
					dialog->GPSTimer->Start(5000);
					sSOG = wxString::Format(_T("%5.2f %s"), m_NMEA0183.Rmc.SpeedOverGroundKnots,opt->speed.c_str());
					sCOG = wxString::Format(_T("%5.2f %s"), m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue, opt->Deg.c_str());
					if(opt->everySM)
						checkDistance();
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
            else if(m_NMEA0183.LastSentenceIDReceived == _T("DBT"))
            {			
					  bool error;

					  m_NMEA0183.Parse();
					  if(m_NMEA0183.Dbt.ErrorMessage.Contains(_T("Invalid")))
						  error = true;
					  else
						  error = false;

					  switch(opt->showDepth)
						{
						case 0:
							sDepth = wxString::Format(_T("%5.1f %s"), 
								(error)?-1:m_NMEA0183.Dbt.DepthMeters, opt->meter.c_str());
							break;
						case 1:
							sDepth = wxString::Format(_T("%5.1f %s"), 
								(error)?-1:m_NMEA0183.Dbt.DepthFeet, opt->feet.c_str());
							break;
						case 2:
							sDepth = wxString::Format(_T("%5.1f %s"), 
								(error)?-1:m_NMEA0183.Dbt.DepthFathoms, opt->fathom.c_str());
							break;
						}
			}
	}
	m_NMEA0183.Dbt.ErrorMessage = _T("");
	m_NMEA0183.ErrorMessage = _T("");
}

void Logbook::newLogbook()
{
	bool zero = false;

	if(data_locn != this->logbookData_actuell)
		this->switchToActuellLogbook();

	int i = wxMessageBox(_("Are you sure ?"),_("New Logbook"),wxYES_NO );
	if(i == wxNO)
		return;

	i = wxMessageBox(_("Reset all Values to zero ?"),_T(""),wxYES_NO );
	if(i == wxYES)
		zero = true;


	if(dialog->m_gridGlobal->GetNumberRows() <= 0)
	{
		wxMessageBox(_("Your Logbook has no lines ?"),_("New Logbook"),wxOK );
		return;
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
		appendRow();
		dialog->logGrids[0]->SetCellValue(0,13,_("Last Logbook is\n")+ss);
	}

	update();

	dialog->setEqualRowHeight(0);
	dialog->setCellAlign(0);
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
		path = _("Active Logbook");
	else
	{
		wxDateTime dt = dialog->getDateTo(path);
		path = wxString::Format(_("Old Logbook until %s"),dt.FormatDate().c_str()); 
	}
	title = path;
	dialog->SetTitle(title);

	loadData();
}
void Logbook::clearAllGrids()
{
	if(dialog->m_gridGlobal->GetNumberRows() > 0)
	{
		dialog->m_gridGlobal->DeleteRows(0,dialog->m_gridGlobal->GetNumberRows());
		dialog->m_gridWeather->DeleteRows(0,dialog->m_gridWeather->GetNumberRows());
		dialog->m_gridMotorSails->DeleteRows(0,dialog->m_gridMotorSails->GetNumberRows());
	}
}

void Logbook::loadData()
{
	wxString s = _T(""),t;

	dialog->selGridCol = dialog->selGridRow = 0;
	title = _("Active Logbook");

	dialog->m_gridGlobal->GetGridWindow()->Freeze();
	dialog->m_gridWeather->GetGridWindow()->Freeze();
	dialog->m_gridMotorSails->GetGridWindow()->Freeze();

	clearAllGrids();

	wxFileInputStream input( data_locn );
	wxTextInputStream* stream = new wxTextInputStream (input);

	int row = 0;
	while( t = stream->ReadLine())
	{
		if(input.Eof()) break;
		dialog->m_gridGlobal->AppendRows();
		dialog->m_gridWeather->AppendRows();
		dialog->m_gridMotorSails->AppendRows();

		dialog->setCellAlign(row);

		wxStringTokenizer tkz(t, _T("\t"),wxTOKEN_RET_EMPTY );
		int c = 0;
		while ( tkz.HasMoreTokens() )
		{
			s = dialog->restoreDangerChar(tkz.GetNextToken());
			s.RemoveLast();

			switch(c)
			{
			case ROUTE:		dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case RDATE:		dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case RTIME:		dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case SIGN:		dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case WAKE:		dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case DISTANCE:	dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case DTOTAL:	dialog->m_gridGlobal->SetCellValue(row,c,s);
							dialog->m_gridGlobal->SetReadOnly(row,c);
				break;
			case POSITION:	dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case COG:		dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case COW:		dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case SOG:		dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case SOW:		dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case DEPTH:		dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case REMARKS:	dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case BARO:		dialog->m_gridWeather->SetCellValue(row,0,s);
				break;
			case WIND:		dialog->m_gridWeather->SetCellValue(row,1,s);
				break;
			case WSPD:		dialog->m_gridWeather->SetCellValue(row,2,s);
				break;
			case CURRENT:	dialog->m_gridWeather->SetCellValue(row,3,s);
				break;
			case CSPD:		dialog->m_gridWeather->SetCellValue(row,4,s);
				break;
			case WAVE:		dialog->m_gridWeather->SetCellValue(row,5,s);
				break;
			case SWELL:		dialog->m_gridWeather->SetCellValue(row,6,s);
				break;
			case WEATHER:	dialog->m_gridWeather->SetCellValue(row,7,s);
				break;
			case CLOUDS:	dialog->m_gridWeather->SetCellValue(row,8,s);
				break;
			case VISIBILITY:dialog->m_gridWeather->SetCellValue(row,9,s);
				break;
			case MOTOR:		dialog->m_gridMotorSails->SetCellValue(row,0,s);
				break;
			case MOTORT:	dialog->m_gridMotorSails->SetCellValue(row,1,s);
							dialog->m_gridMotorSails->SetReadOnly(row,1);
				break;
			case FUEL:		dialog->m_gridMotorSails->SetCellValue(row,2,s);
				break;
			case FUELT:		dialog->m_gridMotorSails->SetCellValue(row,3,s);
							dialog->m_gridMotorSails->SetReadOnly(row,3);
				break;
			case SAILS:		dialog->m_gridMotorSails->SetCellValue(row,4,s);
				break;
			case REEF:		dialog->m_gridMotorSails->SetCellValue(row,5,s);
				break;
			case WATER:		dialog->m_gridMotorSails->SetCellValue(row,6,s);
				break;
			case WATERT:	dialog->m_gridMotorSails->SetCellValue(row,7,s);
							dialog->m_gridMotorSails->SetReadOnly(row,7);
				break;
			case MREMARKS:	dialog->m_gridMotorSails->SetCellValue(row,8,s);
				break;
			}
			c++;
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
	dialog->m_gridGlobal->GetGridWindow()->Thaw();
	dialog->m_gridWeather->GetGridWindow()->Thaw();
	dialog->m_gridMotorSails->GetGridWindow()->Thaw();
	dialog->selGridRow = 0; dialog->selGridCol = 0;
}
/*
void Logbook::loadDatanew()
{
	wxString s = _T(""),t;

	LogGridTable *tableL = new LogGridTable(2,13);
	LogGridTable *tableW = new LogGridTable(2,10);
	LogGridTable *tableM = new LogGridTable(2,11);
	

	bool o = dialog->m_gridGlobal->SetTable(tableL,true);
	o = dialog->m_gridWeather->SetTable(tableW,true);
	o = dialog->m_gridMotorSails->SetTable(tableM,true);

	wxFileInputStream input( data_locn );
	wxTextInputStream* stream = new wxTextInputStream (input);

	int row = 0;
	while( t = stream->ReadLine())
	{
		if(input.Eof()) break;
		o = dialog->m_gridGlobal->AppendRows();
		o = dialog->m_gridWeather->AppendRows();
		o = dialog->m_gridMotorSails->AppendRows();

		wxStringTokenizer tkz(t, _T(","));
		int c = 0;
		while ( tkz.HasMoreTokens() )
		{
			s = dialog->restoreDangerChar(tkz.GetNextToken());

			switch(c)
			{
			case ROUTE:		dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case RDATE:		dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case RTIME:		dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case SIGN:		dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case WAKE:		dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case DISTANCE:	dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case DTOTAL:	dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case POSITION:	dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case COG:		dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case COW:		dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case SOG:		dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case SOW:		dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case DEPTH:		dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case REMARKS:	dialog->m_gridGlobal->SetCellValue(row,c,s);
				break;
			case BARO:		dialog->m_gridWeather->SetCellValue(row,0,s);
				break;
			case WIND:		dialog->m_gridWeather->SetCellValue(row,1,s);
				break;
			case WSPD:		dialog->m_gridWeather->SetCellValue(row,2,s);
				break;
			case CURRENT:	dialog->m_gridWeather->SetCellValue(row,3,s);
				break;
			case CSPD:		dialog->m_gridWeather->SetCellValue(row,4,s);
				break;
			case WAVE:		dialog->m_gridWeather->SetCellValue(row,5,s);
				break;
			case SWELL:		dialog->m_gridWeather->SetCellValue(row,6,s);
				break;
			case WEATHER:	dialog->m_gridWeather->SetCellValue(row,7,s);
				break;
			case CLOUDS:	dialog->m_gridWeather->SetCellValue(row,8,s);
				break;
			case VISIBILITY:dialog->m_gridWeather->SetCellValue(row,9,s);
				break;
			case MOTOR:		dialog->m_gridMotorSails->SetCellValue(row,0,s);
				break;
			case MOTORT:	dialog->m_gridMotorSails->SetCellValue(row,1,s);
				break;
			case FUEL:		dialog->m_gridMotorSails->SetCellValue(row,2,s);
				break;
			case FUELT:		dialog->m_gridMotorSails->SetCellValue(row,3,s);
				break;
			case SAILS:		dialog->m_gridMotorSails->SetCellValue(row,4,s);
				break;
			case REEF:		dialog->m_gridMotorSails->SetCellValue(row,5,s);
				break;
			case WATER:		dialog->m_gridMotorSails->SetCellValue(row,6,s);
				break;
			case WATERT:	dialog->m_gridMotorSails->SetCellValue(row,7,s);
				break;
			case MREMARKS:	dialog->m_gridMotorSails->SetCellValue(row,8,s);
				break;
			}
			c++;
		}
		dialog->setEqualRowHeight(row);
		row++;
	}

	for(int i = 0; i < LOGGRIDS; i++)
		dialog->logGrids[i]->MakeCellVisible(dialog->logGrids[i]->GetNumberRows()-1,0);
}
*/
void Logbook::switchToActuellLogbook()
{
	dialog->selGridRow = 0; dialog->selGridCol = 0; 
	logbookFile = new wxTextFile(logbookData_actuell);
	data_locn = logbookData_actuell;
	setFileName(logbookData_actuell,layout_locn);
	dialog->SetTitle(_("Active Logbook"));	
	loadData();
}


void Logbook::appendRow()
{
	wxString s;

	checkGPS();

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
	}

	int lastRow = dialog->logGrids[0]->GetNumberRows();
	if(lastRow > 800)
	{
		dialog->timer->Stop();
		wxMessageBox(_("Your Logbook has 800 lines or more\n\n\
Please create a new logbook to minimize the loadingtime.\n\nIf you have a running timer, it's stopped now !!!"),_("Information"));

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

	dialog->setCellAlign(lastRow);

	if(lastRow > 0)
		{
			dialog->logGrids[0]->SetCellValue(lastRow,0,dialog->logGrids[0]->GetCellValue(lastRow-1,0));
			dialog->logGrids[0]->SetCellValue(lastRow,3,dialog->logGrids[0]->GetCellValue(lastRow-1,3));
			dialog->logGrids[0]->SetCellValue(lastRow,4,dialog->logGrids[0]->GetCellValue(lastRow-1,4));
			dialog->logGrids[0]->SetCellValue(lastRow,7,sLat+sLon);
			changeCellValue(lastRow, 0,1);
			dialog->logGrids[0]->SetCellValue(lastRow,5,
				calculateDistance(dialog->logGrids[0]->GetCellValue(lastRow-1,7),
								  dialog->logGrids[0]->GetCellValue(lastRow,7)));
			dialog->logGrids[0]->SetCellValue(lastRow,6,dialog->logGrids[0]->GetCellValue(lastRow-1,6));
			dialog->logGrids[2]->SetCellValue(lastRow,1,dialog->logGrids[2]->GetCellValue(lastRow-1,1));
			dialog->logGrids[2]->SetCellValue(lastRow,3,dialog->logGrids[2]->GetCellValue(lastRow-1,3));
			dialog->logGrids[2]->SetCellValue(lastRow,7,dialog->logGrids[2]->GetCellValue(lastRow-1,7));
		}

	if(sDate != _T(""))
		{
			dialog->logGrids[0]->SetCellValue(lastRow,1,sDate);
			dialog->logGrids[0]->SetCellValue(lastRow,2,sTime);
		}
		else
		{
			mCorrectedDateTime = wxDateTime::Now();
			dialog->logGrids[0]->SetCellValue(lastRow,1,mCorrectedDateTime.FormatDate());
			dialog->logGrids[0]->SetCellValue(lastRow,2,mCorrectedDateTime.FormatTime());
		}

	dialog->logGrids[0]->SetCellValue(lastRow,7,sLat+sLon);
	dialog->logGrids[0]->SetCellValue(lastRow,8,sCOG);
	dialog->logGrids[0]->SetCellValue(lastRow,9,sCOW);
	dialog->logGrids[0]->SetCellValue(lastRow,10,sSOG);
	dialog->logGrids[0]->SetCellValue(lastRow,11,sSOW);
	dialog->logGrids[0]->SetCellValue(lastRow,12,sDepth);
	dialog->logGrids[1]->SetCellValue(lastRow,1,sWind);
	dialog->logGrids[1]->SetCellValue(lastRow,2,sWindSpeed);
	dialog->logGrids[2]->SetCellValue(lastRow,0,_T("00.00"));
	dialog->logGrids[2]->SetCellValue(lastRow,2,_T("0"));
	dialog->logGrids[2]->SetCellValue(lastRow,6,_T("0"));
	dialog->logGrids[2]->SetCellValue(lastRow,8,_T(" "));
	dialog->logGrids[0]->SetCellValue(lastRow,4,getWake());

	dialog->logGrids[0]->SetCellValue(lastRow,13,sLogText);

	changeCellValue(lastRow, 0,1);
	dialog->setEqualRowHeight(lastRow);

	dialog->m_gridGlobal->SetReadOnly(lastRow,6); 
	dialog->m_gridMotorSails->SetReadOnly(lastRow,1);
	dialog->m_gridMotorSails->SetReadOnly(lastRow,3);
	dialog->m_gridMotorSails->SetReadOnly(lastRow,7);

	dialog->m_gridGlobal->MakeCellVisible(lastRow,0);
	dialog->m_gridWeather->MakeCellVisible(lastRow,0);
	dialog->m_gridMotorSails->MakeCellVisible(lastRow,0);
}

void Logbook::checkCourseChanged()
{
	if(dLastHeading != -1)
	{
		wxDouble headingChangeMax = dLastHeading+opt->dCourseChangeDegrees;
		wxDouble headingChangeMin = dLastHeading-opt->dCourseChangeDegrees;
		if(headingChangeMax > 360) headingChangeMax -= 360;
		if(headingChangeMin < 0) headingChangeMin += 360;

		if(abs(dLastHeading - dCOW) >= opt->dCourseChangeDegrees )
		{
			courseChange = true;
			dLastHeading = dCOW;
			appendRow();
			courseChange = false;
		}
	}
		else
			dLastHeading = dCOW;
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
			appendRow();
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
		oldPosition = newPosition;
		everySM = true;	
		appendRow();
		everySM = false;
	}
}

wxString Logbook::getWake()
{
	wxString start, end, name = _T("");;
	wxDateTime dtstart,dtend, now;

	dtstart = dtend = mCorrectedDateTime.Today();
	now = mCorrectedDateTime;

	int h,m,count = 0;

	for(int r = 0; r < dialog->m_gridCrewWake->GetRows(); r++)
	{
		for(int i = 0; i < 3; i++)
		{
			start = dialog->m_gridCrewWake->GetCellValue(r,2+i*2);
			end   = dialog->m_gridCrewWake->GetCellValue(r,3+i*2);

			if(start.IsEmpty() || end.IsEmpty()) continue;

			wxStringTokenizer tkz(start,_T(":")); 
			wxString token  = tkz.GetNextToken();
			h = wxAtoi(token);
			if(h > 23 || h < 0) h = 0;
			token  = tkz.GetNextToken();
			m  = wxAtoi(token);
			if(m > 59 || m < 0) m = 0;
			dtstart.Set(h,m);

			wxStringTokenizer tkze(end,_T(":")); 
			token  = tkze.GetNextToken();
			h = wxAtoi(token);
			if(h > 23 || h < 0) h = 0;
			token  = tkze.GetNextToken();
			m  = wxAtoi(token);
			if(m > 59 || m < 0) m = 0;
			dtend.Set(h,m);

			if((dtstart.GetHour() > dtend.GetHour())) 
			{
				wxDateSpan sp(0,0,0,1);
				if(now.GetHour() > dtstart.GetHour())
					dtend.Add(sp);
				else	
					dtstart.Subtract(sp);
			}
/*			wxString nodate =now.FormatDate();
			wxString noetime = now.FormatTime();
			wxString dtstartdate = dtstart.FormatDate();
			wxString dstarttime = dtstart.FormatTime();
			wxString dtenddate = dtend.FormatDate();
			wxString dtendtie = dtend.FormatTime();
*/
			if(now >= dtstart && now <= dtend)
			{			
				if(count != 0)
					name += _T("\n");
				name += dialog->m_gridCrewWake->GetCellValue(r,1)+ _T(" ") + dialog->m_gridCrewWake->GetCellValue(r,0);
				count++;
				dtend = mCorrectedDateTime.Today();
			}
		}
	}
	return name;
}

wxString Logbook::calculateDistance(wxString fromstr, wxString tostr)
{
	if(fromstr.IsEmpty() || tostr.IsEmpty()) return wxString(_T("0.00 ")+opt->distance);

	wxString sLat, sLon, sLatto, sLonto;
	wxDouble fromlat,fromlon,tolat,tolon, sm;

	wxStringTokenizer tkz(fromstr, _T("\n"));
	sLat = tkz.GetNextToken();
	sLon = tkz.GetNextToken();
	wxStringTokenizer tkzto(tostr, _T("\n"));
	sLatto = tkzto.GetNextToken();
	sLonto = tkzto.GetNextToken();
	
	fromlat = positionStringToDezimal(sLat) * (PI/180);
	fromlon = positionStringToDezimal(sLon)* (PI/180);
	tolat = positionStringToDezimal(sLatto)* (PI/180);
	tolon = positionStringToDezimal(sLonto)* (PI/180);

///////
sm = acos(cos(fromlat)*cos(fromlon)*cos(tolat)*cos(tolon) + 
		  cos(fromlat)*sin(fromlon)*cos(tolat)*sin(tolon) + sin(fromlat)*sin(tolat)) * 3443.9;
////// code snippet from http://www2.nau.edu/~cvm/latlongdist.html#formats

	return wxString::Format(_T("%.2f %s"),sm,opt->distance.c_str());
}

wxDouble Logbook::positionStringToDezimal(wxString pos)
{
	wxDouble resdeg, resmin, ressec = 0;
	wxString temp = pos;

	wxStringTokenizer tkz(pos, _T(" "));
	temp = tkz.GetNextToken();
	temp.ToDouble(&resdeg);
	if(pos.Contains(_T("S"))) resdeg = -resdeg;
	if(pos.Contains(_T("W"))) resdeg = -resdeg;
	temp = tkz.GetNextToken();
	temp.ToDouble(&resmin);
	temp = tkz.GetNextToken();
	temp.ToDouble(&ressec);
	resmin = (resmin/60 + ressec/3600);

	return resdeg + resmin;
}

void Logbook::deleteRow(int row)
{
	int answer = wxMessageBox(wxString::Format(_("Delete Row Nr. %i ?"),row+1), _("Confirm"),
                              wxYES_NO | wxCANCEL, dialog);
	if (answer == wxYES)
	{
		for(int i =0; i < dialog->numPages; i++)
			dialog->logGrids[i]->DeleteRows(row);
	}
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

	wxString s = _T(""), temp;

	wxString newLocn = data_locn;
	newLocn.Replace(_T("txt"),_T("Bak"));
	wxRename(data_locn,newLocn);

	wxFileOutputStream output( data_locn );
	wxTextOutputStream* stream = new wxTextOutputStream (output);

	int count = dialog->logGrids[0]->GetNumberRows();
	for(int r = 0; r < count; r++)
	{
		for(int g = 0; g < LOGGRIDS; g++)
		{
			for(int c = 0; c < dialog->logGrids[g]->GetNumberCols(); c++)
			{
				temp = dialog->logGrids[g]->GetCellValue(r,c);
				s += dialog->replaceDangerChar(temp);
				s += _T(" \t");
			}
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

	if((grid == 0 && (col == 0 || col == 2 || col == 4 || col == 13)) ||
		(grid == 1 && (col == 7 || col == 8 || col == 9)) ||
		(grid == 2 && (col == 4 || col == 5 || col == 8)))
	{
		return;
	}

	if(grid == 0 && col == 1 )
						if(row == dialog->m_gridGlobal->GetNumberRows()-1)
							dialog->maintenance->checkService(row);

	if(grid == 0 && col == 5)
					{
						s.Replace(_T(","),_T("."));

						s = wxString::Format(_T("%.2f %s"),wxAtof(s),opt->distance.c_str());
						dialog->logGrids[grid]->SetCellValue(row,col,s);

						computeCell(grid, row, col, s, true);
						if(row == dialog->m_gridGlobal->GetNumberRows()-1)
							dialog->maintenance->checkService(row);
					}
	
	else if(grid == 0 && col== 3)
					{
						if(row == dialog->m_gridGlobal->GetNumberRows()-1)
							dialog->maintenance->checkService(row);
					}
	else if(grid == 0 && col== 7)
					{
						if(s != _T("") && !s.Contains(opt->Deg)
							&& !s.Contains(opt->Min)
							&& !s.Contains(opt->Sec))
						{
							if(s.length() != 16)
							{
								wxMessageBox(_("Please enter 0544512n0301205e for\n054Deg 45Min 12Sec N 030Deg 12Min 05Sec E"),_("Information"),wxOK);
								s = _T("");
							}
							else
							{
								wxString temp = s.SubString(0,2)+opt->Deg+_T(" ");
								temp += s.SubString(3,4) + opt->Min+_T(" ");
								temp += s.SubString(5,6) + opt->Sec+_T(" ");
								temp += s.SubString(7,7).Upper() + _T("\n");
								temp += s.SubString(8,10) + opt->Deg+_T(" ");
								temp += s.SubString(11,12) + opt->Min+_T(" ");
								temp += s.SubString(13,14) + opt->Sec+_T(" ");
								temp += s.SubString(15,15).Upper();
								s = temp;
							}
						}
						if(row != 0)
							dialog->logGrids[grid]->SetCellValue(row,5,
							calculateDistance(dialog->logGrids[grid]->GetCellValue(row-1,col),s));
						dialog->logGrids[grid]->SetCellValue(row,col,s);
					}
	else if(grid == 0 && col == 8)
					{
						if(s != _T(""))
						{
							s.Replace(_T(","),_T("."));
							s = wxString::Format(_T("%3.2f%s"),wxAtof(s),opt->Deg.c_str());
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 0 && col == 9)
					{
						if(s != _T(""))
						{
							s.Replace(_T(","),_T("."));
							s = wxString::Format(_T("%3.2f%s %s"),wxAtof(s),opt->Deg.c_str(),(opt->showHeading)?_T("M"):_T("T"));
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 0 && (col == 10 || col == 11))
					{
						if(s != _T(""))
						{
							s.Replace(_T(","),_T("."));
							s = wxString::Format(_T("%2.2f %s"),wxAtof(s),+opt->speed.c_str());
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 0 && col == 12)
					{
						if(s != _T(""))
						{
							switch(opt->showDepth)
							{
								case 0:	depth = opt->meter; break;
								case 1: depth = opt->feet; break;
								case 2: depth = opt->fathom; break;
							}
							s.Replace(_T(","),_T("."));
							if(s.Contains(opt->meter) || s.Contains(opt->feet) || s.Contains(opt->fathom.c_str()))
							{
								if(s.Contains(_T("-1")))
									s.Replace(_T("-1.0"),_T("-----"));
								dialog->logGrids[grid]->SetCellValue(row,col,s);
							}
							else
							{
								s = wxString::Format(_T("%3.2f %s"),wxAtof(s.c_str()),depth.c_str());
								dialog->logGrids[grid]->SetCellValue(row,col,s);
							}
					
						}
					}
	else if(grid == 1 && col == 0)
					{
						if(s != _T(""))
						{
							s = wxString::Format(_T("%u %s"),wxAtoi(s),opt->baro.c_str());
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}

	else if(grid == 1 && col == 1)
					{
						if(s != _T("") && !s.Contains(opt->Deg))
						{
							s = wxString::Format(_T("%s%s %s"),s.c_str(), opt->Deg.c_str(),opt->showWindDir?_T("R"):_T("T"));
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 1 && col == 2 )
					{
						if(s != _T(""))
						{
							s.Replace(_T(","),_T("."));
							switch(opt->showWindSpeed)
							{
								case 0:	wind = opt->windkts; break;
								case 1: wind = opt->windmeter; break;
								case 2: wind = opt->windkmh; break;
							}
							s = wxString::Format(_T("%2.2f %s"),wxAtof(s),wind.c_str());
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 1 && col == 3)
					{
						if(s != _T("") && !s.Contains(opt->Deg))
						{
							s = wxString::Format(_T("%s%s"),s.c_str(), opt->Deg.c_str());
							dialog->logGrids[grid]->SetCellValue(row,col,s);						
						}
					}				
	else if(grid == 1 && col == 4)
					{
						if(s != _T(""))
						{
							s.Replace(_T(","),_T("."));
							s = wxString::Format(_T("%02.2f %s"),wxAtof(s),opt->speed.c_str());
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 1 && (col == 5 || col == 6))
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
							s = wxString::Format(_T("%02.2f %s"),wxAtof(s),d.c_str());
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}

	else if(grid == 2 && col == 0 && !s.IsEmpty())
					{
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
						if(t != true)
//						if(((s[2] != _T('.') && s[2] != _T(',') && s[2] != _T(':'))&& s.Length() != 5))
							{
								wxMessageBox(_("Please enter like 1.30 or 1,30 or 1:30\nfor 1:30h"),_T(""),wxOK);
								dialog->logGrids[grid]->SetCellValue(row,col,_T("00:00"));
							}

						wxStringTokenizer tkz(s,sep);
						wxString h = tkz.GetNextToken();
						wxString m = tkz.GetNextToken();
						if(wxAtoi(m) > 59)
							{
								wxMessageBox(_("Minutes greater than 59"),_T(""));
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
							}						
					}


	else if(grid == 2 && ( col == 2 || col == 6 ) && !s.IsEmpty())
					{
						s.Replace(_T(","),_T("."));
						s = wxString::Format(_T("%.2f %s"),wxAtof(s),opt->vol.c_str());
						dialog->logGrids[grid]->SetCellValue(row,col,s);

						computeCell(grid, row, col,s, false);

					}
	return;
}

wxString Logbook::computeCell(int grid, int row, int col, wxString s, bool mode)
{
	double current = 0, last = 0.0;
	long hourCur=0, minCur=0 ,hourLast=0, minLast = 0;
	int count;
	wxString cur;
	wxString abrev;

	switch(col)
	{
	case 5: abrev = opt->distance; break;
	case 0: abrev = opt->motorh; break;
	case 2: abrev = opt->vol; break;
	case 6: abrev = opt->vol; break;
	}

	count = dialog->m_gridGlobal->GetNumberRows();

	for(int i = row; i < count; i++)
	{
		s = dialog->logGrids[grid]->GetCellValue(i,col);
		if(s == _T("0000")) s = _T("00:00");
		if(grid == 2 && col == 0)
		{
			wxArrayString time = wxStringTokenize(s,_T(":"));
			time[0].ToLong(&hourCur);
			time[1].ToLong(&minCur);
		}
		else
			s.ToDouble(&current);

		if(i > 0)
		{
			wxString temp = dialog->logGrids[grid]->GetCellValue(i-1,col+1);
			if(grid == 2 && col == 0)
			{
				if(temp.Contains(_T(":")))
				{
					wxArrayString time = wxStringTokenize(temp,_T(":"));
					time[0].ToLong(&hourLast);
					time[1].ToLong(&minLast);
				}
				else { hourLast = 0; minLast =0; }
			}
			else
				temp.ToDouble(&last);
		}
		else { last = 0.0f; hourLast = 0; minLast = 0; }
		
		if(grid == 2 && col == 0)
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
				dialog->logGrids[grid]->SetCellValue(i,col+1,s);
			}	
	}
	return cur;
}

wxString Logbook::positionTraditional(int NEflag, double a, bool mode )
{
	wxString s;
	return s;
}

wxString Logbook::positionGPSLike(int NEflag, double a, bool mode )
{
	wxString s;
	return s;
}

wxString Logbook::toSDMM ( int NEflag, double a, bool mode )
{
      short neg = 0;
      int d;
      long m;
	  long sec;

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

				  s.Printf ( _T("%03d%02ld%02ld%c"), d, 
					  m / 1000, sec /*( m % 1000 )*/, c );
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
                  s.Printf ( _T("%03d%02ld%02ld%c"), d,
					  m / 1000,sec/*( m % 1000 )*/, c );
            }
      }
      return s;
}

bool Logbook::checkGPS()
{
	if(gpsStatus)
	{
		if(courseChange)
			sLogText = opt->courseChangeText+opt->courseChangeDegrees+opt->Deg;
		else if(guardChange)
			sLogText = opt->guardChangeText;
		else if(everySM)
			sLogText = opt->everySMText+opt->everySMAmount+opt->distance;
		else if(dialog->timer->IsRunning())
			sLogText = opt->ttext;
		else 
			sLogText = _T("");
		return true;
	}
	else
	{
		sLat = sLon = sDate = sTime = _T("");
		sCOG = sCOW = sSOG = sSOW = sDepth = sWind = sWindSpeed = _T("");
		if(opt->noGPS)
			sLogText = _("No GPS-Signal !");
		else
			sLogText = _T("");
		return false;
	}
}

void Logbook::SetGPSStatus(bool status)
{
	gpsStatus = status;
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

