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

//#define PBVE_DEBUG 1

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
	dCOG = -1;
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
	wxDateTime dt;
	m_NMEA0183 << sentence;

#ifdef PBVE_DEBUG
	static int pbve = 0;
	if(sentence.Contains(_T("$PBVE")))
	{
		if(pvbe != NULL && pbve < 15)
		{
			pvbe->m_textCtrlPVBE->AppendText(sentence);
			pvbe->SetFocus();
			pbve++;
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
			else if(m_NMEA0183.LastSentenceIDReceived == _T("RMC"))
            {
                  if(m_NMEA0183.Parse())
                  {
					setPositionString(m_NMEA0183.Rmc.Position.Latitude.Latitude,
									m_NMEA0183.Rmc.Position.Latitude.Northing,
									m_NMEA0183.Rmc.Position.Longitude.Longitude,
									m_NMEA0183.Rmc.Position.Longitude.Easting);


					sSOG = wxString::Format(_T("%5.2f %s"), m_NMEA0183.Rmc.SpeedOverGroundKnots,opt->speed.c_str());
					sCOG = wxString::Format(_T("%5.2f %s"), m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue, opt->Deg.c_str());
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
	bool goLocal = false;

	if(opt->gpsAuto)
	{
		if(newPosition.WEflag == 'E')
			opt->tzIndicator = 1;
		else
			opt->tzIndicator = 0;

		int tz = ((int)newPosition.longitude) % 15;
		if(!tz)
			opt->tzHour = ((int)newPosition.longitude) / 15;
		goLocal = true;
	}
	if(opt->local || goLocal)
	{
		wxTimeSpan span(opt->tzHour, 0, 0, 0);
		if(opt->tzIndicator == 0)
			mCorrectedDateTime = mUTCDateTime + span;
		else
			mCorrectedDateTime = mUTCDateTime - span;
	}
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
    if(iNorth== South)
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
		appendRow(true);
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
		dialog->m_gridGlobal->DeleteRows(0,dialog->m_gridGlobal->GetNumberRows(),false);
		dialog->m_gridWeather->DeleteRows(0,dialog->m_gridWeather->GetNumberRows(),false);
		dialog->m_gridMotorSails->DeleteRows(0,dialog->m_gridMotorSails->GetNumberRows(),false);
	}
}

void Logbook::loadData()
{
	wxString s = _T(""),t;

	dialog->selGridCol = dialog->selGridRow = 0;
	title = _("Active Logbook");

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

	dialog->selGridRow = 0; dialog->selGridCol = 0;
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

	if(lastRow > 0)
		{
			dialog->logGrids[0]->SetCellValue(lastRow,0,dialog->logGrids[0]->GetCellValue(lastRow-1,0));
			dialog->logGrids[0]->SetCellValue(lastRow,3,dialog->logGrids[0]->GetCellValue(lastRow-1,3));
			dialog->logGrids[0]->SetCellValue(lastRow,4,dialog->logGrids[0]->GetCellValue(lastRow-1,4));
			dialog->logGrids[0]->SetCellValue(lastRow,7,sLat+sLon);
			changeCellValue(lastRow, 0,0);
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
	dialog->setCellAlign(lastRow);
	dialog->setEqualRowHeight(lastRow);

	dialog->m_gridGlobal->SetReadOnly(lastRow,6); 
	dialog->m_gridMotorSails->SetReadOnly(lastRow,1);
	dialog->m_gridMotorSails->SetReadOnly(lastRow,3);
	dialog->m_gridMotorSails->SetReadOnly(lastRow,7);

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
	grid->GetCellValue(grid->GetNumberRows()-1,8).ToDouble(&cog);

	if(cog == dCOG) return;

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
		for(int i = 0; i < 3; i++)
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
	if(pos.Contains(_T("S"))) resmin = -resmin;
	if(pos.Contains(_T("W"))) resmin = -resmin;
	temp = tkz.GetNextToken();
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
	temp.ToDouble(&resdeg);
	if(pos.Contains(_T("S"))) resdeg = -resdeg;
	if(pos.Contains(_T("W"))) resdeg = -resdeg;
	temp = tkz.GetNextToken();
	temp.ToDouble(&resmin);
	if(pos.Contains(_T("S"))) resmin = -resmin;
	if(pos.Contains(_T("W"))) resmin = -resmin;
//	wxMessageBox(wxString::Format(_T("%f\n%f\n%f"),resdeg,resmin/60,resdeg+(resmin/60)));
	return resdeg + (resmin/60);
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
	modified = false;

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

	if((grid == 0 && (col == 0 || col == 4 || col == 13)) ||
		(grid == 1 && (col == 7 || col == 8 || col == 9)) ||
		(grid == 2 && (col == 4 || col == 5 || col == 8)))
	{
		return;
	}

	if(grid == 0 && col == 1 )
					{
						wxDateTime dt;

						if(!dialog->myParseDate(s,dt))
						{
							dt = dt.Now();
							wxMessageBox(wxString::Format(_("Please enter the Date in the format:\n      %s"),dt.FormatDate()));
							dialog->logGrids[grid]->SetCellValue(row,col,_T(""));
						}
						else
						{
							dialog->logGrids[grid]->SetCellValue(row,col,dt.FormatDate());

							if(row == dialog->m_gridGlobal->GetNumberRows()-1)
								dialog->maintenance->checkService(row);
						}
					}
	else if(grid == 0 && col == 2)
					{
						wxDateTime dt;
						const wxChar* c;

						s = s.Upper();
						c = dt.ParseTime(s);

						if(!c)
						{
							wxMessageBox(_("Please enter the Time in the format:\n   12:30:00PM"));
							dialog->logGrids[grid]->SetCellValue(row,col,_T(""));
						}
						else
						{
							dialog->logGrids[grid]->SetCellValue(row,col,dt.FormatTime());
							if(row == dialog->m_gridGlobal->GetNumberRows()-1)
								dialog->maintenance->checkService(row);
						}
					}
	else if(grid == 0 && col == 5)
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
							if(opt->traditional && s.length() != 22)
							{
								wxMessageBox(_("Please enter 0544512.15n0301205.15e for\n054Deg 45Min 12.15Sec N 030Deg 12Min 05.15Sec E"),_("Information"),wxOK);
								s = _T("");
							}
							else if(!opt->traditional && s.length() != 22)
							{
								wxMessageBox(_("Please enter 05445.1234n03012.0504e for\n054Deg 45.1234Min N 030Deg 12.0504Min E"),_("Information"),wxOK);
								s = _T("");
							}
							if(s == _T("")) return;

							if(s.Contains(_T(",")))
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
						dialog->logGrids[grid]->SetCellValue(row,col,s);
						if(row != 0)
						{
							double distTotal,dist ;
							dialog->logGrids[grid]->SetCellValue(row,5,
								calculateDistance(dialog->logGrids[grid]->GetCellValue(row-1,col),s));
							dialog->logGrids[grid]->GetCellValue(row-1,6).ToDouble(&distTotal);
							dialog->logGrids[grid]->GetCellValue(row,5).ToDouble(&dist);
							dialog->logGrids[grid]->SetCellValue(row,6,wxString::Format(_T("%9.2f %s"),distTotal+dist,opt->distance.c_str()));
								
						}
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
							if(s.Contains(opt->meter) || 
								s.Contains(opt->feet) || 
								s.Contains(opt->fathom.c_str()) ||
								s.Contains(_T("--")))
							{
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
                        s.Printf ( _T ( "%03d%02ld.%04ld%c" ), d, m / 10000, ( m % 10000 ), c );
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
                        s.Printf ( _T ( "%03d %02ld %02ld.%ld %c" ), d, m, sec / 10, sec % 10, c );
            }
            break;
      }
      return s;
}

bool Logbook::checkGPS()
{
	sLogText = _T("");

	if(gpsStatus)
	{
		if(opt->showWindHeading == 1 && !bCOW)
		{
			sLogText = _("Wind is set to Heading,\nbut GPS sends no Heading Data.\nWind is set now to Realative to boat\n\n"); 
			opt->showWindHeading = 0;
		}
		if(courseChange)
			sLogText += opt->courseChangeText+opt->courseChangeDegrees+opt->Deg;
		else if(guardChange)
			sLogText += opt->guardChangeText;
		else if(everySM)
			sLogText += opt->everySMText+opt->everySMAmount+opt->distance;
		else if(dialog->timer->IsRunning())
			sLogText += opt->ttext;
			
		return true;
	}
	else
	{
		sLat = sLon = sDate = sTime = _T("");
		sCOG = sCOW = sSOG = sSOW = sDepth = sWind = sWindSpeed = _T("");
		bCOW = false;
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
