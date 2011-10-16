//#pragma once
#ifndef WX_PRECOMP
      #include <wx/wx.h>
#endif

#include "OverView.h"
#include "LogbookDialog.h"
#include "logbook_pi.h"
#include "Options.h"

#include <wx/filename.h>
#include <wx/dir.h>

OverView::OverView(LogbookDialog* d, wxString data, wxString lay, wxString layoutODT)
{
	parent = d;

	ODTLayout_locn = layoutODT;
	HTMLLayout_locn = lay;
	data_locn = data;
	grid = d->m_gridOverview;
	opt = d->logbookPlugIn->opt;
	setLayoutLocation();
	loadAllLogbooks();
	loadLogbookData(logbooks[0]);

	grid->SetColLabelValue( FETMAL, grid->GetColLabelValue(FETMAL)+_T(" Ø") );
	grid->SetColLabelValue( FWINDDIR, grid->GetColLabelValue(FWINDDIR)+_T(" Ø") );
	grid->SetColLabelValue( FWIND, grid->GetColLabelValue(FWIND)+_T(" Ø") );
	grid->SetColLabelValue( FCURRENTDIR, grid->GetColLabelValue(FCURRENTDIR)+_T(" Ø") );
	grid->SetColLabelValue( FCURRENT, grid->GetColLabelValue(FCURRENT)+_T(" Ø") );
	grid->SetColLabelValue( FWAVE, grid->GetColLabelValue(FWAVE)+_T(" Ø") );
	grid->SetColLabelValue( FSWELL, grid->GetColLabelValue(FSWELL)+_T(" Ø") );
}

OverView::~OverView(void)
{
}

void OverView::loadAllLogbooks()
{
	wxArrayString files;

	int i = wxDir::GetAllFiles(data_locn,&files,_T("*.txt"));

	for(int f = 0; f < i; f++)
	{
		wxFileName name(files.Item(f));
		if(name.GetName().Contains(_T("logbook")))
			logbooks.Add(files[f]);
	}
}

void OverView::loadLogbookData(wxString logbook)
{
	wxString t,s;
	bool test = true;
	bool addValue = true;
	double x = 0;
	wxStringTokenizer tkz1;
	wxTimeSpan span;

	grid->DeleteRows(0,grid->GetNumberRows());
	resetValues();

	wxFileInputStream input( logbook );
	wxTextInputStream* stream = new wxTextInputStream (input);

	int row = -1;
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
										resetValues();
										grid->AppendRows();
										route = s;
										row++;
										lastrow = row;
										test = true;
										grid->SetCellValue(row,FROUTE,s);
									}

				break;
			case DATE:				if(test)
										startdate = s;
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
			case TIME:				
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
			case SOG:				
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
			case SAILS:				grid->SetCellValue(row,FSAILS,s);
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
		writeSumColumn(lastrow);
		test = false;
	}
}

void OverView::resetValues()
{
	startdate =_T("");
	enddate = _T("");
	etmaldate = _T("");
	etmal = 0;
	bestetmal = 0;
	bestetmaltemp = 0;
	enginehours = 0;
	enginemin = 0;
	distance = 0;
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
}

void OverView::writeSumColumn(int row)
{
	wxString d;
	switch(opt->showWaveSwell)
	{
		case 0:	d = opt->meter; break;
		case 1: d = opt->feet; break;
		case 2: d = opt->fathom; break;
	}

	grid->SetCellValue(row,FSTART,startdate);
	grid->SetCellValue(row,FEND,enddate);
	grid->SetCellValue(row,FDISTANCE,wxString::Format(_T("%6.2f %s"),distance,opt->distance));
	grid->SetCellValue(row,FETMAL,wxString::Format(_T("%6.2f %s"),etmal,opt->distance));
	grid->SetCellValue(row,FBESTETMAL,wxString::Format(_T("%6.2f %s"),bestetmal,opt->distance));

	grid->SetCellValue(row,FFUEL,wxString::Format(_T("%6.2f %s"),abs(fuel),opt->vol));
	grid->SetCellValue(row,FWATER,wxString::Format(_T("%6.2f %s"),abs(water),opt->vol));
	grid->SetCellValue(row,FWIND,wxString::Format(_T("%6.2f %s"),wind/windcount,_T("kts")));
	grid->SetCellValue(row,FWINDDIR,wxString::Format(_T("%6.2f %s"),winddir/windcount,opt->Deg));
	grid->SetCellValue(row,FWINDPEAK,wxString::Format(_T("%6.2f %s"),windpeak,_T("kts")));
	grid->SetCellValue(row,FWAVE,wxString::Format(_T("%6.2f %s"),wave/wavecount,d));
	grid->SetCellValue(row,FWAVEPEAK,wxString::Format(_T("%6.2f %s"),wavepeak,d));
	grid->SetCellValue(row,FSWELL,wxString::Format(_T("%6.2f %s"),swell/swellcount,d));
	grid->SetCellValue(row,FSWELLPEAK,wxString::Format(_T("%6.2f %s"),swellpeak,d));
	grid->SetCellValue(row,FCURRENTDIR,wxString::Format(_T("%6.2f %s"),currentdir/currentcount,opt->Deg));
	grid->SetCellValue(row,FCURRENT,wxString::Format(_T("%6.2f %s"),current/currentcount,d));
	grid->SetCellValue(row,FCURRENTPEAK,wxString::Format(_T("%6.2f %s"),currentpeak,d));
	grid->SetCellValue(row,FENGINE,wxString::Format(_T("%0002i:%02i %s"),enginehours,enginemin,opt->motorh));
}

void OverView::setLayoutLocation()
{
	if(parent->m_radioBtnHTMLOverview->GetValue())
		layout_locn = HTMLLayout_locn;
	else
		layout_locn = ODTLayout_locn;
	wxString boatLay = layout_locn;

	layout_locn.Append(_T("overview"));
	parent->appendOSDirSlash(&layout_locn);

	parent->loadLayoutChoice(layout_locn,parent->boatChoice);
}
