//#pragma once
#ifndef WX_PRECOMP
      #include <wx/wx.h>
#endif

#include "OverView.h"
#include "LogbookDialog.h"
#include "logbook_pi.h"
#include "Options.h"
#include "Logbook.h"

#include <wx/filename.h>
#include <wx/txtstrm.h> 
#include <wx/zipstrm.h> 
#include <wx/generic/gridctrl.h>
#include <wx/dir.h>

#include <map>
using namespace std;
OverView::OverView(LogbookDialog* d, wxString data, wxString lay, wxString layoutODT)
	: Export(d)
{
	parent = d;

	ODTLayout_locn = layoutODT;
	HTMLLayout_locn = lay;
	data_locn = data;
	data_file = data + _T("overview.html");
	grid = d->m_gridOverview;
	opt = d->logbookPlugIn->opt;
	selectedRow = 0;
	logbook = d->logbook;
	collection t_coll;
	setLayoutLocation();
	loadAllLogbooks();
}

OverView::~OverView(void)
{
}

void OverView::refresh()
{
	grid->DeleteRows(0,grid->GetNumberRows());
	row = -1;
	if(parent->m_radioBtnActuellLogbook->GetValue())
		actuellLogbook();
	else if(parent->m_radioBtnAllLogbooks->GetValue())
		allLogbooks();
	else
		if(parent->m_radioBtnSelectLogbook->GetValue())
			if(!selectedLogbook.IsEmpty())
				loadLogbookData(selectedLogbook,false);
}

void OverView::viewODT(wxString path,wxString layout,int mode)
{
	wxString fn;// = data_locn;

	fn = toODT(path, layout, mode);

	if(layout != _T(""))
	{
	    fn.Replace(_T("txt"),_T("odt"));
		parent->startApplication(fn,_T(".odt"));
	}
}

void OverView::viewHTML(wxString path,wxString layout,int mode)
{
	wxString fn;// = data_locn;

	fn = toHTML(path, layout, mode);

	if(layout != _T(""))
	{
	    fn.Replace(_T("txt"),_T("html"));
		parent->startBrowser(fn);
	}
}

wxString OverView::toODT(wxString path,wxString layout,int mode)
{
	wxString top;
	wxString header;
	wxString middle;
	wxString bottom;
	wxString tempPath = data_file;

	wxString odt = readLayoutODT(layout_locn,layout);
	if(!cutInPartsODT( odt, &top, &header,	&middle, &bottom))
		return _T("");

	wxTextFile* text = setFiles(path, &tempPath, mode);
	writeToODT(text,parent->m_gridOverview,data_file,layout_locn+layout+_T(".odt"), top,header,middle,bottom,mode);

	return data_file;
}

wxString OverView::toHTML(wxString path,wxString layout,int mode)
{
	wxString top;
	wxString header;
	wxString middle;
	wxString bottom;
	wxString tempPath = data_file;

	wxString html = readLayoutHTML(layout_locn,layout);
	if(!cutInPartsHTML( html, &top, &header, &middle, &bottom))
		return _T("");

	wxTextFile* text = setFiles(path, &tempPath, mode);
	writeToHTML(text,parent->m_gridOverview,tempPath,layout_locn+layout+_T(".html"), top,header,middle,bottom,mode);

	return data_file;
}

void OverView::loadAllLogbooks()
{
	wxArrayString		files;

	int i = wxDir::GetAllFiles(data_locn,&files,_T("*.txt"));

	for(int f = 0; f < i; f++)
	{
		wxFileName name(files.Item(f));
		if(name.GetName().Contains(_T("logbook")))
			logbooks.Add(files[f]);
	}
}

void OverView::selectLogbook()
{
	int selIndex = -1;
	wxString path(*parent->pHome_Locn);
	path = path + wxFileName::GetPathSeparator() + _T("data");

	SelectLogbook selLogbook(parent,path);
	
	if(selLogbook.ShowModal() == wxID_CANCEL)
		return;

	parent->m_radioBtnSelectLogbook->SetValue(true);
	grid->DeleteRows(0,grid->GetNumberRows());
	row = -1;

	selIndex = selLogbook.m_listCtrlSelectLogbook->GetNextItem(selIndex,wxLIST_NEXT_ALL,wxLIST_STATE_SELECTED);
	if(selIndex == -1) return;

	selectedLogbook = selLogbook.files[selIndex];
	loadLogbookData(selectedLogbook,false);
}

void OverView::actuellLogbook()
{
	clearGrid();
	for(unsigned int i=0; i < logbooks.size(); i++)
	  if(!logbooks[i].Contains(_T("until")))
	  {
	    loadLogbookData(logbooks[i],false);
	    break;
	  }
    opt->overviewAll = false;
}

void OverView::allLogbooks()
{
	clearGrid();
	for(unsigned int i = 0; i < logbooks.Count(); i++)
	{
		if(i % 2)
			loadLogbookData(logbooks[i],true);
		else
			loadLogbookData(logbooks[i],false);
	}
	opt->overviewAll = true;
}

void OverView::clearGrid()
{
	if(row != -1)
		grid->DeleteRows(0,grid->GetNumberRows());
	row = -1;
}

void OverView::loadLogbookData(wxString logbook, bool colour)
{
	wxString t,s;
	bool test = true;
//	bool write = true;
	double x = 0;
	wxStringTokenizer tkz1;
	wxTimeSpan span;
	collection::iterator it;

	resetValues();

	wxFileInputStream input( logbook );
	wxTextInputStream* stream = new wxTextInputStream (input);
	wxString path = logbook;
	wxFileName fn(logbook);
	logbook = fn.GetName();
	if(logbook == _T("logbook"))
		logbook = _T("Active Logbook");
	else
	{
		wxDateTime dt = parent->getDateTo(logbook);
		logbook = _("Logbook until ")+dt.FormatDate();
	}

	int lastrow = 0;
	oneLogbookTotalReset();

	wxString route = _T("xxx");
	int rowNewLogbook = -1;
	while( (t = stream->ReadLine()))
	{
		if(input.Eof()) break;
		sign = wxEmptyString;
		rowNewLogbook++;
		wxStringTokenizer tkz(t, _T("\t"),wxTOKEN_RET_EMPTY );
		int c = 0;
		while ( tkz.HasMoreTokens() )
		{
			s = parent->restoreDangerChar(tkz.GetNextToken());
			s.RemoveLast();
			s.Replace(_T(","),_T("."));

			switch(c)
			{
			case ROUTE:				if(route != s)
									{
										//write = true;
										resetValues();
										grid->AppendRows();
										route = s;
										row++;
										lastrow = row;
										test = true;
										grid->SetCellValue(row,FROUTE,s);
									}
									else ;
										//write = false;

				break;
			case DATE:			if(test)
								{
									startdate = s;
									enddate = s;
									if(rowNewLogbook == 0)
										oneLogbookTotal.logbookStart = oneLogbookTotal.logbookEnd = s;
								}
								else
									enddate = s;

								if(etmaldate != s)
								{
									etmaldate = s;
									etmalcount++;
									bestetmaltemp = 0;
								}

									date = s;

				break;
			case TIME:			if(test)
								{
									starttime = s;
									endtime = s;
									if(rowNewLogbook == 0)
										oneLogbookTotal.logbookTimeStart = oneLogbookTotal.logbookTimeEnd = s;
								}
								else
									endtime = s;
				break;
			case SIGN:		   sign = s;
				break;
			case WATCH:	
				break;
			case DISTANCE:			if(sign != 'S') break;
									s.ToDouble(&x);

									distance += x;
									etmal = distance/etmalcount;

									if(date == etmaldate)
									{
										bestetmaltemp += x;
										if(bestetmaltemp > bestetmal)
											bestetmal = bestetmaltemp;
										if(bestetmaltemp > oneLogbookTotal.bestetmal)
											 oneLogbookTotal.bestetmal = bestetmaltemp;
									}
									oneLogbookTotal.distance += x;
									//allLogbooksTotal.distance += x;
				break;
			case DISTANCETOTAL:		
			case POSITION:			
			case COG:				
			case HEADING:	
				break;
			case SOG:				if(sign == _T("S"))
									{
										s.ToDouble(&x);
										speed += x;	oneLogbookTotal.speed += x;
										speedcount++; oneLogbookTotal.speedcount++;
										if(x > speedpeak) speedpeak = x;
										if(x > oneLogbookTotal.speedpeak) oneLogbookTotal.speedpeak = x;
									}
				break;
			case STW:				if(sign == _T("S"))
									{
										s.ToDouble(&x);
										speedSTW += x; oneLogbookTotal.speedSTW += x;
										speedcountSTW++; oneLogbookTotal.speedcountSTW++;
										if(x > speedpeakSTW) speedpeakSTW = x;
										if(x > oneLogbookTotal.speedpeakSTW) oneLogbookTotal.speedpeakSTW = x;
									}
				break;
			case DEPTH:				
			case REMARKS:			
			case BAROMETER:			
				break;
			case WIND:				if(!s.IsEmpty())
									{
									s.ToDouble(&x);
									winddir += x; oneLogbookTotal.winddir += x;
									windcount++; oneLogbookTotal.windcount++;
									}
				break;
			case WINDFORCE:			if(!s.IsEmpty())
									{
									s.ToDouble(&x);
									wind += x; oneLogbookTotal.wind += x;
									if(x > windpeak) windpeak = x; 
									if(x > oneLogbookTotal.windpeak) oneLogbookTotal.windpeak = x;
									}
				break;
			case CURRENT:			if(!s.IsEmpty())
									{
									s.ToDouble(&x);
									currentdir += x; oneLogbookTotal.currentdir += x;
									currentcount++; oneLogbookTotal.currentcount++;
									}
				break;
			case CURRENTFORCE:		if(!s.IsEmpty())
									{
									s.ToDouble(&x);
									current += x; oneLogbookTotal.current += x;
									if(x > currentpeak) currentpeak = x; 
									if(x > oneLogbookTotal.currentpeak) oneLogbookTotal.currentpeak = x;
									}
				break;
			case WAVE:				if(!s.IsEmpty())
									{
									s.ToDouble(&x);
									wave += x; oneLogbookTotal.wave += x;
									wavecount++; oneLogbookTotal.wavecount++;
									if(x > wavepeak) wavepeak = x; 
									if(x > oneLogbookTotal.wavepeak) oneLogbookTotal.wavepeak = x;
									}
				break;
			case SWELL:				if(!s.IsEmpty())
									{
									s.ToDouble(&x);
									swell += x; oneLogbookTotal.swell += x;
									swellcount++; oneLogbookTotal.swellcount++;
									if(x > swellpeak) swellpeak = x; 
									if(x > oneLogbookTotal.swellpeak) oneLogbookTotal.swellpeak = x;
									}
				break;
			case WEATHER:			
			case CLOUDS:			
			case VISIBILITY:
				break;
			case ENGINE:			tkz1.SetString(s, _T(":"),wxTOKEN_RET_EMPTY );
									long hours, minutes;
									tkz1.GetNextToken().ToLong(&hours);
									tkz1.GetNextToken().ToLong(&minutes);
									enginehours += hours; enginemin += minutes;
									if(enginemin >= 60) { enginehours++; enginemin -= 60; }
									oneLogbookTotal.enginehours += hours; oneLogbookTotal.enginemin += minutes;
									if(oneLogbookTotal.enginemin >= 60) { oneLogbookTotal.enginehours++; oneLogbookTotal.enginemin -= 60; }
				break;
			case ENGINETOTAL:		
				break;
			case FUEL:				s.ToDouble(&x);
									if(x < 0)
									{
										fuel += x;
										oneLogbookTotal.fuel += x;
									}
				break;
			case FUELTOTAL:			
				break;
			case SAILS:				if(!s.empty())
									{
										bool found = false;
										wxString result;
										for(it = t_coll.begin(); it != t_coll.end(); ++it)
										{
											if( s == it->first)
											{
												int a = it->second;
												it->second = ++a;
												result += it->first+_("\n");
												found = true;
											}
										}
										if(!found)
										{
											t_coll.insert(pair(s,1)); 
											wxString result;
										for(it = t_coll.begin(); it != t_coll.end(); ++it)
										{
											result = it->first;
										}
										}
									}
				break;
			case REEF:				
				break;
			case WATER:				s.ToDouble(&x);
									if(x < 0)
									{
										water += x;
										oneLogbookTotal.water += x;
									}
				break;
			case WATERTOTAL:		
				break;
			case MREMARKS:			
				break;
			}
			c++;
		}
//		if(write)
			writeSumColumn(lastrow, logbook, path, colour);
		test = false;
	}
	writeSumColumnLogbook(oneLogbookTotal,lastrow, logbook, colour);
}

void OverView::resetValues()
{
	startdate =_T("");
	enddate = _T("");
	starttime =_T("");
	endtime = _T("");
	etmaldate = _T("");
	etmal = 0;
	bestetmal = 0;
	bestetmaltemp = 0;
	enginehours = 0;
	enginemin = 0;
	distance = 0;
	speed = 0;
	speedpeak = 0;
	speedSTW = 0;
	speedpeakSTW = 0;
	water = 0;
	fuel = 0;
	wind = 0;
	winddir = 0;
	wave = 0;
	current = 0;
	currentdir=0;
	swell = 0;
	windpeak = 0;
	wavepeak = 0;
	currentpeak = 0;
	swellpeak = 0;

	windcount = 0;
	wavecount = 0;
	swellcount = 0;
	currentcount = 0;
	etmalcount = 0;
	speedcount = 0;
	speedcountSTW = 0;
	t_coll.clear();

	sign = wxEmptyString;
}

void OverView::oneLogbookTotalReset()
{
	oneLogbookTotal.sails.Clear();

	oneLogbookTotal.logbookStart = wxEmptyString;
	oneLogbookTotal.logbookEnd = wxEmptyString;
	oneLogbookTotal.logbookTimeStart = wxEmptyString;
	oneLogbookTotal.logbookTimeEnd = wxEmptyString;
	oneLogbookTotal.distance = 0;
	oneLogbookTotal.bestetmal = 0;
	oneLogbookTotal.speed = 0;
	oneLogbookTotal.speedpeak = 0;
	oneLogbookTotal.speedSTW = 0;
	oneLogbookTotal.speedpeakSTW = 0;
	oneLogbookTotal.enginehours = 0;
	oneLogbookTotal.enginemin = 0;
	oneLogbookTotal.fuel = 0;
	oneLogbookTotal.water = 0;
	oneLogbookTotal.winddir = 0;
	oneLogbookTotal.wind = 0;
	oneLogbookTotal.wave = 0;
	oneLogbookTotal.swell = 0;
	oneLogbookTotal.current = 0;
	oneLogbookTotal.currentdir = 0;
	oneLogbookTotal.windpeak = 0;
	oneLogbookTotal.wavepeak = 0;
	oneLogbookTotal.swellpeak = 0;
	oneLogbookTotal.currentpeak = 0;

	oneLogbookTotal.windcount = 0;
	oneLogbookTotal.currentcount = 0;
	oneLogbookTotal.wavecount = 0;
	oneLogbookTotal.swellcount = 0;
	oneLogbookTotal.etmalcount = 0;
	oneLogbookTotal.speedcount = 0;
	oneLogbookTotal.speedcountSTW = 0;
}

void OverView::writeSumColumn(int row, wxString logbook, wxString path, bool colour)
{
	wxString d, sail;
	wxString nothing = _T("-----");

	switch(opt->showWaveSwell)
	{
		case 0:	d = opt->meter; break;
		case 1: d = opt->feet; break;
		case 2: d = opt->fathom; break;
	}

	grid->SetCellAlignment(row,FLOG,wxALIGN_LEFT, wxALIGN_TOP);
	grid->SetCellAlignment(row,FROUTE,wxALIGN_LEFT, wxALIGN_TOP);
	grid->SetCellAlignment(row,FSAILS,wxALIGN_LEFT, wxALIGN_TOP);

	grid->SetCellValue(row,FLOG,logbook);
	grid->SetCellValue(row,FSTART,startdate);
	grid->SetCellValue(row,FEND,enddate);

	wxString temp = wxString::Format(_T("%6.2f %s"),distance,opt->distance.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FDISTANCE,temp);
	temp = wxString::Format(_T("%6.2f %s"),etmal,opt->distance.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FETMAL,temp);
	temp = wxString::Format(_T("%6.2f %s"),bestetmal,opt->distance.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FBESTETMAL,temp);

#ifdef __WXOSX__
	temp = wxString::Format(_T("%6.2f %s"),labs(fuel),opt->vol.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FFUEL,temp);
	temp = wxString::Format(_T("%6.2f %s"),labs(water),opt->vol.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FWATER,temp);
#else
	temp = wxString::Format(_T("%6.2f %s"),abs(fuel),opt->vol.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FFUEL,temp);
	temp = wxString::Format(_T("%6.2f %s"),abs(water),opt->vol.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FWATER,temp);
#endif
	if(windcount)
		temp = wxString::Format(_T("%6.2f %s"),wind/windcount,_T("kts"));
	else
		temp = nothing;
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FWIND,temp);

	if(windcount)
		temp = wxString::Format(_T("%6.2f %s"),winddir/windcount,opt->Deg.c_str());
	else
		temp = nothing;
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FWINDDIR,temp);

	temp = wxString::Format(_T("%6.2f %s"),windpeak,_T("kts"));
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FWINDPEAK,temp);

	if(wavecount)
		temp = wxString::Format(_T("%6.2f %s"),wave/wavecount,d.c_str());
	else
		temp = nothing;
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FWAVE,temp);

	temp = wxString::Format(_T("%6.2f %s"),wavepeak,d.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FWAVEPEAK,temp);

	if(swellcount)
		temp = wxString::Format(_T("%6.2f %s"),swell/swellcount,d.c_str());
	else
		temp = nothing;
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FSWELL,temp);

	temp = wxString::Format(_T("%6.2f %s"),swellpeak,d.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FSWELLPEAK,temp);

	if(currentcount)
		wxString::Format(_T("%6.2f %s"),currentdir/currentcount,opt->Deg.c_str());
	else
		temp = nothing;
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FCURRENTDIR,temp);

	if(currentcount)
		temp = wxString::Format(_T("%6.2f %s"),current/currentcount,d.c_str());
	else
		temp = nothing;
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FCURRENT,temp);

	temp = wxString::Format(_T("%6.2f %s"),currentpeak,d.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FCURRENTPEAK,temp);

	temp = wxString::Format(_T("%0002i:%02i %s"),enginehours,enginemin,opt->motorh.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FENGINE,temp);

	if(speedcount)
		temp = wxString::Format(_T("%6.2f %s"),speed/speedcount,opt->speed.c_str());
	else
		temp = nothing;
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FSPEED,temp);

	temp = wxString::Format(_T("%6.2f %s"),speedpeak,opt->speed.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FBSPEED,temp);

	if(speedcountSTW)
		temp = wxString::Format(_T("%6.2f %s"),speedSTW/speedcountSTW,opt->speed.c_str());
	else
		temp = nothing;
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FSPEEDSTW,temp);

	temp = wxString::Format(_T("%6.2f %s"),speedpeakSTW,opt->speed.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FBSPEEDSTW,temp);

	grid->SetCellValue(row,FPATH,path);

	wxDateTime startdt, enddt;

	parent->myParseDate(startdate,startdt);
//	startdt.ParseTime(starttime);
	parent->myParseTime(starttime,startdt);
	parent->myParseDate(enddate,enddt);
//	enddt.ParseTime(endtime);
	parent->myParseTime(endtime,enddt);

	wxTimeSpan journey = enddt.Subtract(startdt);
	grid->SetCellValue(row,FJOURNEY,journey.Format(_T("%D Days %H:%M "))+opt->motorh);

	int max = 0; 
	wxString result;
	collection::iterator it;

	for(it = t_coll.begin(); it != t_coll.end(); ++it)
		if(it->second >= max)  { sail = it->first; max = it->second; }
	grid->SetCellValue(row,FSAILS,sail);

	if(colour)
		for(int i = 0; i < grid->GetCols(); i++)
			grid->SetCellBackgroundColour(row,i,wxColour(230,230,230));
}

void OverView::writeSumColumnLogbook(total data, int row, wxString logbook, bool colour)
{
	wxString nothing = _T("-----");

	parent->m_gridOverview->AppendRows();
	row = parent->m_gridOverview->GetRows()-1;
	for(int i= 0; i < parent->m_gridOverview->GetNumberCols(); i++)
		grid->SetCellBackgroundColour(row,i,wxColour(156,156,156));

	wxString d, sail;
	switch(opt->showWaveSwell)
	{
		case 0:	d = opt->meter; break;
		case 1: d = opt->feet; break;
		case 2: d = opt->fathom; break;
	}

	grid->SetCellAlignment(row,FLOG,wxALIGN_LEFT, wxALIGN_TOP);
	grid->SetCellAlignment(row,FROUTE,wxALIGN_LEFT, wxALIGN_TOP);
	grid->SetCellAlignment(row,FSAILS,wxALIGN_LEFT, wxALIGN_TOP);

	grid->SetCellValue(row,FLOG,_("Logbook Total"));
	grid->SetCellValue(row,FSTART,data.logbookStart);
	grid->SetCellValue(row,FEND,enddate);

	wxString temp = wxString::Format(_T("%6.2f %s"),data.distance,opt->distance.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FDISTANCE,temp);
/*	temp = wxString::Format(_T("%6.2f %s"),etmal,opt->distance.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FETMAL,temp);*/
	temp = wxString::Format(_T("%6.2f %s"),data.bestetmal,opt->distance.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FBESTETMAL,temp);

#ifdef __WXOSX__
	temp = wxString::Format(_T("%6.2f %s"),labs(fuel),opt->vol.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FFUEL,temp);
	temp = wxString::Format(_T("%6.2f %s"),labs(water),opt->vol.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FWATER,temp);
#else
	temp = wxString::Format(_T("%6.2f %s"),abs(data.fuel),opt->vol.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FFUEL,temp);
	temp = wxString::Format(_T("%6.2f %s"),abs(data.water),opt->vol.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FWATER,temp);
#endif
	if(data.windcount)
		temp = wxString::Format(_T("%6.2f %s"),data.wind/data.windcount,_T("kts"));
	else
		temp = nothing;
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FWIND,temp);

	if(data.windcount)
		temp = wxString::Format(_T("%6.2f %s"),data.winddir/data.windcount,opt->Deg.c_str());
	else
		temp = nothing;
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FWINDDIR,temp);

	temp = wxString::Format(_T("%6.2f %s"),data.windpeak,_T("kts"));
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FWINDPEAK,temp);

	if(data.wavecount)
		temp = wxString::Format(_T("%6.2f %s"),data.wave/data.wavecount,d.c_str());
	else
		temp = nothing;
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FWAVE,temp);

	temp = wxString::Format(_T("%6.2f %s"),data.wavepeak,d.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FWAVEPEAK,temp);

	if(data.swellcount)
		temp = wxString::Format(_T("%6.2f %s"),data.swell/data.swellcount,d.c_str());
	else
		temp = nothing;
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FSWELL,temp);

	temp = wxString::Format(_T("%6.2f %s"),data.swellpeak,d.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FSWELLPEAK,temp);

	if(data.currentcount)
		wxString::Format(_T("%6.2f %s"),data.currentdir/data.currentcount,opt->Deg.c_str());
	else
		temp = nothing;
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FCURRENTDIR,temp);

	if(data.currentcount)
		temp = wxString::Format(_T("%6.2f %s"),data.current/data.currentcount,d.c_str());
	else
		temp = nothing;
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FCURRENT,temp);

	temp = wxString::Format(_T("%6.2f %s"),data.currentpeak,d.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FCURRENTPEAK,temp);

	temp = wxString::Format(_T("%0002i:%02i %s"),data.enginehours,data.enginemin,opt->motorh.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FENGINE,temp);

	if(data.speedcount)
		temp = wxString::Format(_T("%6.2f %s"),data.speed/data.speedcount,opt->speed.c_str());
	else
		temp = nothing;
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FSPEED,temp);

	temp = wxString::Format(_T("%6.2f %s"),data.speedpeak,opt->speed.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FBSPEED,temp);

	if(data.speedcountSTW)
		temp = wxString::Format(_T("%6.2f %s"),data.speedSTW/data.speedcountSTW,opt->speed.c_str());
	else
		temp = nothing;
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FSPEEDSTW,temp);

	temp = wxString::Format(_T("%6.2f %s"),data.speedpeakSTW,opt->speed.c_str());
	temp.Replace(_T("."),parent->decimalPoint);
	grid->SetCellValue(row,FBSPEEDSTW,temp);

	wxDateTime startdt, enddt;

	parent->myParseDate(data.logbookStart,startdt);
//	startdt.ParseTime(data.logbookTimeStart);
	parent->myParseTime(data.logbookTimeStart,startdt);
	parent->myParseDate(enddate,enddt);
//	enddt.ParseTime(endtime);
	parent->myParseTime(endtime,enddt);
//	wxMessageBox(enddate+endtime+_("\n")+data.logbookStart+data.logbookTimeStart+_("\n")+enddt.FormatDate()+enddt.FormatTime()+_T("\n")+startdt.FormatDate()+startdt.FormatTime());

	wxTimeSpan journey = enddt.Subtract(startdt);
	grid->SetCellValue(row,FJOURNEY,journey.Format(_T("%E Weeks %D Days %H:%M "))+opt->motorh);

	int max = 0; 
	wxString result;
	collection::iterator it;

	for(it = t_coll.begin(); it != t_coll.end(); ++it)
		if(it->second >= max)  { sail = it->first; max = it->second; }
	grid->SetCellValue(row,FSAILS,sail);

	if(colour)
		for(int i = 0; i < grid->GetCols(); i++)
			grid->SetCellBackgroundColour(row,i,wxColour(230,230,230));
}

void OverView::setLayoutLocation()
{
	if(parent->m_radioBtnHTMLOverview->GetValue())
		layout_locn = HTMLLayout_locn;
	else
		layout_locn = ODTLayout_locn;
	this->layout_locn = layout_locn;

	this->layout_locn.Append(_T("overview"));
	parent->appendOSDirSlash(&this->layout_locn);

	parent->loadLayoutChoice(this->layout_locn,parent->overviewChoice);
}

void OverView::setSelectedRow(int row)
{
	selectedRow = row;
}

void OverView::gotoRoute()
{
	wxString file = grid->GetCellValue(selectedRow,FLOG);
	wxString route =  grid->GetCellValue(selectedRow,FROUTE);
	wxString date =  grid->GetCellValue(selectedRow,FSTART);
	wxString path  =  grid->GetCellValue(selectedRow,FPATH);

	if(logbook->data_locn != path)
	{
		logbook->data_locn = path;
		logbook->loadSelectedData(path);
	}

	int i;
	for(i = 0; i < parent->m_gridGlobal->GetNumberRows(); i++)
	{
		if(parent->m_gridGlobal->GetCellValue(i,0) == route &&
			parent->m_gridGlobal->GetCellValue(i,1) == date)
			break;
	}

	parent->m_gridGlobal->MakeCellVisible(i,0);
	parent->m_gridGlobal->SelectRow(i);
	parent->m_gridWeather->MakeCellVisible(i,0);
	parent->m_gridWeather->SelectRow(i);
	parent->m_gridMotorSails->MakeCellVisible(i,0);
	parent->m_gridMotorSails->SelectRow(i);

	parent->m_logbook->SetSelection(0);

}

wxString OverView::setPlaceHolders(int mode, wxGrid *grid, int row, wxString middle)
{
	wxString newMiddleODT = middle;

	newMiddleODT.Replace(wxT("#FLOG#"),replaceNewLine(mode,grid->GetCellValue(row,FLOG)));
	newMiddleODT.Replace(wxT("#LLOG#"),grid->GetTable()->GetColLabelValue(FLOG));
	newMiddleODT.Replace(wxT("#FROUTE#"),replaceNewLine(mode,grid->GetCellValue(row,FROUTE)));
	newMiddleODT.Replace(wxT("#LROUTE#"),grid->GetTable()->GetColLabelValue(FROUTE));
	newMiddleODT.Replace(wxT("#FSTART#"),replaceNewLine(mode,grid->GetCellValue(row,FSTART)));
	newMiddleODT.Replace(wxT("#LSTART#"),grid->GetTable()->GetColLabelValue(FSTART));
	newMiddleODT.Replace(wxT("#FEND#"),replaceNewLine(mode,grid->GetCellValue(row,FEND)));
	newMiddleODT.Replace(wxT("#LEND#"),grid->GetTable()->GetColLabelValue(FEND));
	newMiddleODT.Replace(wxT("#FJOURNEY#"),replaceNewLine(mode,grid->GetCellValue(row,FJOURNEY)));
	newMiddleODT.Replace(wxT("#LJOURNEY#"),grid->GetTable()->GetColLabelValue(FJOURNEY));
	newMiddleODT.Replace(wxT("#FDISTANCE#"),replaceNewLine(mode,grid->GetCellValue(row,FDISTANCE)));
	newMiddleODT.Replace(wxT("#LDISTANCE#"),grid->GetTable()->GetColLabelValue(FDISTANCE));
	newMiddleODT.Replace(wxT("#FETMAL#"),replaceNewLine(mode,grid->GetCellValue(row,FETMAL)));
	newMiddleODT.Replace(wxT("#LETMAL#"),grid->GetTable()->GetColLabelValue(FETMAL));
	newMiddleODT.Replace(wxT("#FBESTETMAL#"),replaceNewLine(mode,grid->GetCellValue(row,FBESTETMAL)));
	newMiddleODT.Replace(wxT("#LBESTETMAL#"),grid->GetTable()->GetColLabelValue(FBESTETMAL));
	newMiddleODT.Replace(wxT("#FSPEED#"),replaceNewLine(mode,grid->GetCellValue(row,FSPEED)));
	newMiddleODT.Replace(wxT("#LSPEED#"),grid->GetTable()->GetColLabelValue(FSPEED));
	newMiddleODT.Replace(wxT("#FBSPEED#"),replaceNewLine(mode,grid->GetCellValue(row,FBSPEED)));
	newMiddleODT.Replace(wxT("#LBSPEED#"),grid->GetTable()->GetColLabelValue(FBSPEED));
	newMiddleODT.Replace(wxT("#FSPEEDSTW#"),replaceNewLine(mode,grid->GetCellValue(row,FSPEEDSTW)));
	newMiddleODT.Replace(wxT("#LSPEEDSTW#"),grid->GetTable()->GetColLabelValue(FSPEEDSTW));
	newMiddleODT.Replace(wxT("#FBSPEEDSTW#"),replaceNewLine(mode,grid->GetCellValue(row,FBSPEEDSTW)));
	newMiddleODT.Replace(wxT("#LBSPEEDSTW#"),grid->GetTable()->GetColLabelValue(FBSPEEDSTW));

	newMiddleODT.Replace(wxT("#FENGINE#"),replaceNewLine(mode,grid->GetCellValue(row,FENGINE)));
	newMiddleODT.Replace(wxT("#LENGINE#"),grid->GetTable()->GetColLabelValue(FENGINE));
	newMiddleODT.Replace(wxT("#FFUEL#"),replaceNewLine(mode,grid->GetCellValue(row,FFUEL)));
	newMiddleODT.Replace(wxT("#LFUEL#"),grid->GetTable()->GetColLabelValue(FFUEL));
	newMiddleODT.Replace(wxT("#FWATER#"),replaceNewLine(mode,grid->GetCellValue(row,FWATER)));
	newMiddleODT.Replace(wxT("#LWATER#"),grid->GetTable()->GetColLabelValue(FWATER));
	newMiddleODT.Replace(wxT("#FWINDDIR#"),replaceNewLine(mode,grid->GetCellValue(row,FWINDDIR)));
	newMiddleODT.Replace(wxT("#LWINDDIR#"),grid->GetTable()->GetColLabelValue(FWINDDIR));
	newMiddleODT.Replace(wxT("#FWIND#"),replaceNewLine(mode,grid->GetCellValue(row,FWIND)));
	newMiddleODT.Replace(wxT("#LWIND#"),grid->GetTable()->GetColLabelValue(FWIND));
	newMiddleODT.Replace(wxT("#FWINDPEAK#"),replaceNewLine(mode,grid->GetCellValue(row,FWINDPEAK)));
	newMiddleODT.Replace(wxT("#LWINDPEAK#"),grid->GetTable()->GetColLabelValue(FWINDPEAK));
	newMiddleODT.Replace(wxT("#FCURRENTDIR#"),replaceNewLine(mode,grid->GetCellValue(row,FCURRENTDIR)));
	newMiddleODT.Replace(wxT("#LCURRENTDIR#"),grid->GetTable()->GetColLabelValue(FCURRENTDIR));

	newMiddleODT.Replace(wxT("#FCURRENT#"),replaceNewLine(mode,grid->GetCellValue(row,FCURRENT)));
	newMiddleODT.Replace(wxT("#LCURRENT#"),grid->GetTable()->GetColLabelValue(FCURRENT));
	newMiddleODT.Replace(wxT("#FCURRENTPEAK#"),replaceNewLine(mode,grid->GetCellValue(row,FCURRENTPEAK)));
	newMiddleODT.Replace(wxT("#LCURRENTPEAK#"),grid->GetTable()->GetColLabelValue(FCURRENTPEAK));
	newMiddleODT.Replace(wxT("#FWAVE#"),replaceNewLine(mode,grid->GetCellValue(row,FWAVE)));
	newMiddleODT.Replace(wxT("#LWAVE#"),grid->GetTable()->GetColLabelValue(FWAVE));
	newMiddleODT.Replace(wxT("#FWAVEPEAK#"),replaceNewLine(mode,grid->GetCellValue(row,FWAVEPEAK)));
	newMiddleODT.Replace(wxT("#LWAVEPEAK#"),grid->GetTable()->GetColLabelValue(FWAVEPEAK));
	newMiddleODT.Replace(wxT("#FSWELL#"),replaceNewLine(mode,grid->GetCellValue(row,FSWELL)));
	newMiddleODT.Replace(wxT("#LSWELL#"),grid->GetTable()->GetColLabelValue(FSWELL));
	newMiddleODT.Replace(wxT("#FSWELLPEAK#"),replaceNewLine(mode,grid->GetCellValue(row,FSWELLPEAK)));
	newMiddleODT.Replace(wxT("#LSWELLPEAK#"),grid->GetTable()->GetColLabelValue(FSWELLPEAK));
	newMiddleODT.Replace(wxT("#FSAILS#"),replaceNewLine(mode,grid->GetCellValue(row,FSAILS)));
	newMiddleODT.Replace(wxT("#LSAILS#"),grid->GetTable()->GetColLabelValue(FSAILS));

	return newMiddleODT;
}
