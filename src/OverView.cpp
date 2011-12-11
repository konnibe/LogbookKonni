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
	loadLogbookData(logbooks[0],false);
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
	bool write = true;
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

	wxString route = _T("xxx");
	while( t = stream->ReadLine())
	{
		if(input.Eof()) break;

		wxStringTokenizer tkz(t, _T("\t"),wxTOKEN_RET_EMPTY );
		int c = 0;
		while ( tkz.HasMoreTokens() )
		{
			s = parent->restoreDangerChar(tkz.GetNextToken());
			s.RemoveLast();

			switch(c)
			{
			case ROUTE:				if(route != s)
									{
										write = true;
										resetValues();
										grid->AppendRows();
										route = s;
										row++;
										lastrow = row;
										test = true;
										grid->SetCellValue(row,FROUTE,s);
									}
									else 
										write = false;

				break;
			case DATE:			if(test)
								{
									startdate = s;
									enddate = s;
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
								}
								else
									endtime = s;
				break;
			case SIGN:				
			case WATCH:	
				break;
			case DISTANCE:			s.ToDouble(&x);
									distance += x;
									etmal = distance/etmalcount;
									if(date == etmaldate)
									{
										bestetmaltemp += x;
										if(bestetmaltemp > bestetmal)
											bestetmal = bestetmaltemp;
									}

				break;
			case DISTANCETOTAL:		
			case POSITION:			
			case COG:				
			case HEADING:	
				break;
			case SOG:				s.ToDouble(&x);
									speed += x;
									speedcount++;
									if(x> speedpeak) speedpeak = x;
				break;
			case STW:				
			case DEPTH:				
			case REMARKS:			
			case BAROMETER:			
				break;
			case WIND:				s.ToDouble(&x);
									winddir += x;
									windcount++;
				break;
			case WINDFORCE:			s.ToDouble(&x);
									wind += x;
									if(x > windpeak) windpeak = x; 
				break;
			case CURRENT:			s.ToDouble(&x);
									currentdir += x;
									currentcount++;
				break;
			case CURRENTFORCE:		s.ToDouble(&x);
									current += x;
									if(x > currentpeak) currentpeak = x; 
				break;
			case WAVE:				s.ToDouble(&x);
									wave += x;
									wavecount++;
									if(x > wavepeak) wavepeak = x; 
				break;
			case SWELL:				s.ToDouble(&x);
									swell += x;
									swellcount++;
									if(x > swellpeak) swellpeak = x; 
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
				break;
			case ENGINETOTAL:		
				break;
			case FUEL:				s.ToDouble(&x);
									if(x < 0)
										fuel += x;
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
										water += x;
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
	t_coll.clear();
}

void OverView::writeSumColumn(int row, wxString logbook, wxString path, bool colour)
{
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

	grid->SetCellValue(row,FLOG,logbook);
	grid->SetCellValue(row,FSTART,startdate);
	grid->SetCellValue(row,FEND,enddate);
	grid->SetCellValue(row,FDISTANCE,wxString::Format(_T("%6.2f %s"),distance,opt->distance.c_str()));
	grid->SetCellValue(row,FETMAL,wxString::Format(_T("%6.2f %s"),etmal,opt->distance.c_str()));
	grid->SetCellValue(row,FBESTETMAL,wxString::Format(_T("%6.2f %s"),bestetmal,opt->distance.c_str()));

#ifdef __WXOSX__
	grid->SetCellValue(row,FFUEL,wxString::Format(_T("%6.2f %s"),labs(fuel),opt->vol.c_str()));
	grid->SetCellValue(row,FWATER,wxString::Format(_T("%6.2f %s"),labs(water),opt->vol.c_str()));
#else
	grid->SetCellValue(row,FFUEL,wxString::Format(_T("%6.2f %s"),abs(fuel),opt->vol.c_str()));
	grid->SetCellValue(row,FWATER,wxString::Format(_T("%6.2f %s"),abs(water),opt->vol.c_str()));
#endif
	grid->SetCellValue(row,FWIND,wxString::Format(_T("%6.2f %s"),wind/windcount,_T("kts")));
	grid->SetCellValue(row,FWINDDIR,wxString::Format(_T("%6.2f %s"),winddir/windcount,opt->Deg.c_str()));
	grid->SetCellValue(row,FWINDPEAK,wxString::Format(_T("%6.2f %s"),windpeak,_T("kts")));
	grid->SetCellValue(row,FWAVE,wxString::Format(_T("%6.2f %s"),wave/wavecount,d.c_str()));
	grid->SetCellValue(row,FWAVEPEAK,wxString::Format(_T("%6.2f %s"),wavepeak,d.c_str()));
	grid->SetCellValue(row,FSWELL,wxString::Format(_T("%6.2f %s"),swell/swellcount,d.c_str()));
	grid->SetCellValue(row,FSWELLPEAK,wxString::Format(_T("%6.2f %s"),swellpeak,d.c_str()));
	grid->SetCellValue(row,FCURRENTDIR,wxString::Format(_T("%6.2f %s"),currentdir/currentcount,opt->Deg.c_str()));
	grid->SetCellValue(row,FCURRENT,wxString::Format(_T("%6.2f %s"),current/currentcount,d.c_str()));
	grid->SetCellValue(row,FCURRENTPEAK,wxString::Format(_T("%6.2f %s"),currentpeak,d.c_str()));
	grid->SetCellValue(row,FENGINE,wxString::Format(_T("%0002i:%02i %s"),enginehours,enginemin,opt->motorh.c_str()));
	grid->SetCellValue(row,FSPEED,wxString::Format(_T("%4.2f %s"),speed/speedcount,opt->speed.c_str()));
	grid->SetCellValue(row,FBSPEED,wxString::Format(_T("%4.2f %s"),speedpeak,opt->speed.c_str()));
	grid->SetCellValue(row,FPATH,path);

	wxDateTime startdt, enddt;

	parent->myParseDate(startdate,startdt);
	startdt.ParseTime(starttime);
	parent->myParseDate(enddate,enddt);
	enddt.ParseTime(endtime);

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
