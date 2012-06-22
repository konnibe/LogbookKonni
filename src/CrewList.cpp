#include "CrewList.h"
#include "LogbookDialog.h"
#include "MessageBoxOSX.h"
#include "Export.h"

#ifndef WX_PRECOMP
     #include <wx/wx.h>
#endif

#include <wx/tokenzr.h>
#include <wx/filename.h> 
#include <wx/mimetype.h>
#include <wx/wfstream.h> 
#include <wx/txtstrm.h> 
#include <wx/zipstrm.h> 

#include <memory>
using namespace std;

CrewList::CrewList(LogbookDialog* d, wxString data, wxString layout, wxString layoutODT)
{
	dialog = d;
	gridCrew = d->m_gridCrew;
	gridWake = d->m_gridCrewWake;
	rowHeight = gridCrew->GetRowHeight(0);

	this->layout = layout;
	this->ODTLayout = layoutODT;
	modified = false;

	if(dialog->m_radioBtnHTMLCrew->GetValue())
		layout_locn = layout;
	else
		layout_locn = layoutODT;

	wxString watchData = data;
	watchData.Append(_T("watchlist.txt"));
	wxFileName wxHomeFiledir(watchData) ;
	if(true != wxHomeFiledir.FileExists())
	{
		watchListFile = new wxTextFile(watchData);				
		watchListFile->Create();
	}
	else
		watchListFile = new wxTextFile(watchData);

	wxString crewData = data;
	crewData.Append(_T("crewlist.txt"));
	wxFileName wxHomeFiledir1(crewData) ;
	if(true != wxHomeFiledir1.FileExists())
	{
		crewListFile = new wxTextFile(crewData);				
		crewListFile->Create();
	}
	else
		crewListFile = new wxTextFile(crewData);

	wxString crewLay = layout_locn;
	crewLay.Append(_T("crew"));
	dialog->appendOSDirSlash(&crewLay);

	data_locn = crewData;
	layout_locn = crewLay;
	html_locn = data_locn;
	html_locn.Replace(_T("txt"),_T("html"));

	setLayoutLocation(layout_locn);

	loadData();
}

CrewList::~CrewList(void)
{
	saveData();
}

void CrewList::setLayoutLocation(wxString loc)
{
	if(dialog->m_radioBtnHTMLCrew->GetValue())
		layout_locn = layout;
	else
		layout_locn = ODTLayout;
	wxString crewLay = layout_locn;

	crewLay.Append(_T("crew"));
	dialog->appendOSDirSlash(&crewLay);
	layout_locn = crewLay;
	dialog->loadLayoutChoice(crewLay,dialog->crewChoice);
}

void CrewList::loadData()
{
	wxString s, line;
	wxGrid* grid;
	int lineCount, numRows;
	bool newCol;

	if(gridCrew->GetNumberRows() > 0)
		gridCrew->DeleteRows(0,gridCrew->GetNumberRows());
	if(gridWake->GetNumberRows() > 0)
		gridWake->DeleteRows(0,gridWake->GetNumberRows());

	crewListFile->Open();
	lineCount = crewListFile->GetLineCount();

	if(lineCount <= 0) { crewListFile->Close(); return; }

	for( int i = 0; i < lineCount; i++)
	{
		line = crewListFile->GetLine(i);

		gridCrew->AppendRows();
		numRows = gridCrew->GetNumberRows()-1;
		wxGridCellBoolEditor* boolEditor = new wxGridCellBoolEditor();
		boolEditor->UseStringValues(_("Yes"));
		gridCrew->SetCellEditor(numRows,0,boolEditor);
		gridCrew->SetCellAlignment(wxALIGN_CENTRE,numRows,0);

		wxStringTokenizer tkz(line, _T("\t"),wxTOKEN_RET_EMPTY);
		int c;
		int colCount = gridCrew->GetNumberCols();
		if(tkz.CountTokens() != (unsigned int) colCount)
			{ c = 1; newCol = true; }
		else
			{ c = 0; newCol = false; }

		grid = gridCrew;

		while ( tkz.HasMoreTokens() )
		{
			s = tkz.GetNextToken().RemoveLast();
			s = dialog->restoreDangerChar(s);

			grid->SetCellValue(numRows,c++,s);
			if(c == colCount) break;
		}

		if(newCol)
		{
			modified = true;
			grid->SetCellValue(numRows,0,_("Yes"));
		}
	}

	crewListFile->Close();

	watchListFile->Open();
	lineCount = watchListFile->GetLineCount();

	if(lineCount <= 0) { watchListFile->Close(); return; }

	for( int i = 0; i < lineCount; i++)
	{
		line = watchListFile->GetLine(i);

		gridWake->AppendRows();
		gridWake->SetReadOnly(i,0);
		gridWake->SetReadOnly(i,1);
		gridWake->SetCellAlignment(i,0,wxALIGN_LEFT, wxALIGN_TOP);
		gridWake->SetCellAlignment(i,1,wxALIGN_LEFT, wxALIGN_TOP);

		numRows = gridWake->GetNumberRows()-1;

		wxStringTokenizer tkz(line, _T("\t"),wxTOKEN_RET_EMPTY);
		int c = 0;
		grid = gridWake;

		while ( tkz.HasMoreTokens() )
		{
			s = tkz.GetNextToken().RemoveLast();
			s = dialog->restoreDangerChar(s);
			grid->SetCellValue(numRows,c++,s);
		}
	}
	watchListFile->Close();
}

void CrewList::filterCrewMembers()
{
	int i = 0;

	for(int row = 0; row < gridCrew->GetNumberRows(); row++)
	{
		if(gridCrew->GetCellValue(row,ONBOARD) == _T(""))
		{
			gridCrew->SetRowHeight(row,0);
			i++;
		}
	}

	if(i == gridCrew->GetNumberRows()) 
	{
		showAllCrewMembers();
		dialog->m_menu2->Check(MENUCREWONBOARD,false);
		dialog->m_menu2->Check(MENUCREWALL,true);
	}
	else
		gridCrew->ForceRefresh();
}

void CrewList::showAllCrewMembers()
{
	for(int row = 0; row < gridCrew->GetNumberRows(); row++)
	{
		if(gridCrew->GetCellValue(row,ONBOARD) == _T(""))
			gridCrew->SetRowHeight(row,rowHeight);
	}

	gridCrew->ForceRefresh();
}

void CrewList::saveData()
{
	if(!modified) return;
	modified = false;

	wxString s = _T("");
	crewListFile->Open();
	crewListFile->Clear();

	int count = gridCrew->GetNumberRows();
	for(int r = 0; r < count; r++)
	{
		for(int c = 0; c < gridCrew->GetNumberCols(); c++)
			s += gridCrew->GetCellValue(r,c)+_T(" \t");
		s.RemoveLast();
		crewListFile->AddLine(s);
		s = _T("");
	}
	crewListFile->Write();
	crewListFile->Close();

	s = _T("");
	watchListFile->Open();
	watchListFile->Clear();

	count = gridWake->GetNumberRows();
	for(int r = 0; r < count; r++)
	{
		for(int c = 0; c < gridWake->GetNumberCols(); c++)
			s += gridWake->GetCellValue(r,c)+_T(" \t");
		s.RemoveLast();
		watchListFile->AddLine(s);
		s = _T("");
	}
	watchListFile->Write();
	watchListFile->Close();
}

void CrewList::addCrew(wxGrid* grid, wxGrid* wake)
{
	wxString s;

	modified = true;

	gridCrew->AppendRows();

	int numRows = gridCrew->GetNumberRows()-1;
	wxGridCellBoolEditor* boolEditor = new wxGridCellBoolEditor();
	boolEditor->UseStringValues(_("Yes"));
	gridCrew->SetCellEditor(numRows,0,boolEditor);
	gridCrew->SetCellAlignment(wxALIGN_CENTRE,numRows,0);
	gridCrew->MakeCellVisible(numRows,NAME);

	if(dialog->m_menu2->IsChecked(MENUCREWALL))
		grid->SetCellValue(numRows,ONBOARD,_T(""));
	else
		grid->SetCellValue(numRows,ONBOARD,_("Yes"));

	gridCrew->SetFocus();
	gridCrew->SetGridCursor(numRows,NAME);
}

void CrewList::addToWatchList()
{
	if(gridCrew->GetCellValue(gridCrew->GetGridCursorRow(),ONBOARD) == _T(""))
	{
		wxMessageBox(_("Crewmember is not onboard\n\nPlease set column \'Onboard\' to Yes\n(click the column twice)"));
		return;
	}
	gridWake->AppendRows();
	int lastRow = gridWake->GetNumberRows()-1;
	gridWake->SetCellAlignment(lastRow,LWNAME,wxALIGN_LEFT, wxALIGN_TOP);
	gridWake->SetCellAlignment(lastRow,LWFIRSTNAME,wxALIGN_LEFT, wxALIGN_TOP);
	gridWake->SetCellValue(lastRow,LWNAME,gridCrew->GetCellValue(gridCrew->GetCursorRow(),NAME));
	gridWake->SetCellValue(lastRow,LWFIRSTNAME,gridCrew->GetCellValue(gridCrew->GetCursorRow(),FIRSTNAME));

	modified = true;
}

void CrewList::SameWatchAsDlg(int row)
{
	if(gridCrew->GetCellValue(gridCrew->GetGridCursorRow(),ONBOARD) == _T(""))
	{
		wxMessageBox(_("Crewmember is not onboard\n\nPlease set column \'Onboard\' to Yes\n(click the column twice)"));
		return;
	}

	SameWatchAs* dlg = new SameWatchAs(dialog, row);

	if(dlg->ShowModal() == wxID_OK)
	{
		int rowfound = dlg->m_choice23->GetSelection();

		wxStringTokenizer tkz1(dlg->m_staticTextPersonName->GetLabel(),_T(","));
		wxString firstname, name;
		firstname = tkz1.GetNextToken();
		if(tkz1.HasMoreTokens())
				name = tkz1.GetNextToken();

		if(rowfound >= 0)
		{

			rowfound++;
			gridWake->InsertRows(rowfound);
			gridWake->SetCellAlignment(rowfound,0,wxALIGN_LEFT, wxALIGN_TOP);
			gridWake->SetCellAlignment(rowfound,1,wxALIGN_LEFT, wxALIGN_TOP);

			for(int i = 0; i < gridWake->GetNumberCols(); i++)
			{
				if(i == CrewList::LWFIRSTNAME)
					gridWake->SetCellValue(rowfound,CrewList::LWFIRSTNAME,firstname);
				else if(i == CrewList::LWNAME)
					gridWake->SetCellValue(rowfound,CrewList::LWNAME,name);
				else
					gridWake->SetCellValue(rowfound,i,gridWake->GetCellValue(rowfound-1,i));
			}
		}
	}
	delete dlg;
}

void CrewList::changeCrew(wxGrid* grid, int row, int col, int offset)
{
	wxString result;

	modified = true;
	wxString search;

	if(col == ONBOARD && dialog->m_menu2->IsChecked(MENUCREWONBOARD))
	{
		if(grid->GetCellValue(row,col) == _T("") )
		{
//			if(!checkIsHiddenRow())
				filterCrewMembers();
/*			else
			{
				showAllCrewMembers();
				dialog->m_menu2->Check(MENUCREWONBOARD,false);
				dialog->m_menu2->Check(MENUCREWALL,true);
			}*/
			grid->ForceRefresh();				
		}
	}
	if(col == NAME && offset == 0)
	{
			int rowWake = searchInWatch();
			if(rowWake >= 0)
				gridWake->SetCellValue(rowWake,0,gridCrew->GetCellValue(dialog->selGridRow,NAME));
	}
	if(col == FIRSTNAME && offset == 0)
	{
			int rowWake = searchInWatch();
			if(rowWake >= 0)
				gridWake->SetCellValue(rowWake,1,gridCrew->GetCellValue(row,FIRSTNAME));
	}

/*	if(gridWake->GetCellValue(row,gridWake->GetNumberCols()-1) == _T(""))
		gridWake->SetCellValue(row,gridWake->GetNumberCols()-1,_T(" "));
*/
}

int CrewList::searchInWatch() 
{
	for(int row = 0; row < gridWake->GetNumberRows(); row++)
		if(lastSelectedName == gridWake->GetCellValue(row,LWNAME) && 
		   lastSelectedFirstName == gridWake->GetCellValue(row,LWFIRSTNAME) )
			return row;

	return -1;
}

void CrewList::changeCrewWake(wxGrid* grid, int row, int col, int offset)
{
	wxDateTime dt;
	wxString s = gridWake->GetCellValue(row,col);
	
	if(!checkHourFormat(s,row,col, &dt)) return;

	gridWake->SetCellValue(row,col,dt.Format(_T("%H:%M")));
          
	modified = true;
}

void CrewList::showAutomaticWatchDlg()
{
	wxListItem info;  
	AutomaticWatch* dlg = new AutomaticWatch(dialog);
	
	if(dlg->ShowModal() == wxID_OK)
	{
		wxString time = dlg->m_textCtrlStartTime->GetValue();
		
		//checkHourFormat(time);

		for(int i = 0; i < dialog->m_gridCrewWake->GetNumberRows(); i++)
			for(int c = 0; c < dialog->m_gridCrewWake->GetNumberCols(); c++)
				dialog->m_gridCrewWake->SetCellValue(i,c,wxEmptyString);

		dialog->m_gridCrewWake->SetCellValue(0,WAKESTART1,time);

		for(int row = 0; row < dlg->m_listCtrlWatchNames->GetItemCount(); row++)
		{
			for(int col = 0; col < 2; col++)
			{
				info.m_itemId = row;
				info.m_col = col;
				info.m_mask = wxLIST_MASK_TEXT;

				dlg->m_listCtrlWatchNames->GetItem(info);
				dialog->m_gridCrewWake->SetCellValue(row,col,info.GetText());
			}
		}
			setWatches(dlg, time);
	}
	delete dlg ;  
}

bool CrewList::checkHourFormat(wxString s, int row, int col, wxDateTime* dt)
{
	
	if(s.Len() != 4 || dt->ParseFormat(s,_T("%H%M")) == NULL)
	{
#ifdef __WXOSX__
        
        MessageBoxOSX(NULL,_("Please enter 4 digits in 24h-Format like 1545 = 03:45 PM"),_("Information"),wxID_OK);
        
#else

		wxMessageBox(_("Please enter 4 digits in 24h-Format like 1545 = 03:45 PM"));
#endif
		gridWake->SetCellValue(row,col,wxEmptyString);

		return false;
	}  
	else
	  return true;
}

void CrewList::setWatches(AutomaticWatch* dlg, wxString time)
{
	wxDateTime dt,df;
	dialog->myParseTime(time, dt);
	dialog->myParseTime(dlg->m_staticTextLengthWatch->GetLabel(),df);
	wxTimeSpan diff(df.GetHour(),df.GetMinute(), df.GetSecond());
	wxTimeSpan diffm(0,1,0);

	int end = ((dlg->m_choice20->GetSelection()) * 2) + WAKEEND1;

	int row = 0, col;
	for(col = WAKESTART1; col < end; col += 2)
	{
		for(row = 0; row < dlg->m_listCtrlWatchNames->GetItemCount(); row++)
		{
				gridWake->SetCellValue(row,col,dt.Format(_T("%H:%M")));
				dt.Add(diff);
				dt.Subtract(diffm);
				gridWake->SetCellValue(row,col+1,dt.Format(_T("%H:%M")));
				dt.Add(diffm);
		}
	}

	modified = true;
}

void CrewList::saveCSV(wxString path)
{
	wxString result;

	saveData();

	wxTextFile csvFile(path);

	if(csvFile.Exists())
		{
			::wxRemoveFile(path);
			csvFile.Create();
		}

		crewListFile->Open();


	for(unsigned int i = 0; i < crewListFile->GetLineCount(); i++)
		{
			wxString line = crewListFile->GetLine(i);
			wxStringTokenizer tkz(line, _T("\t"),wxTOKEN_RET_EMPTY);

			while ( tkz.HasMoreTokens() )
			{
				wxString s;
				s += tkz.GetNextToken().RemoveLast();
				s= dialog->restoreDangerChar(s);
				result += wxT("\"")+s+wxT("\",");
			}
			result.RemoveLast();
			csvFile.AddLine(result);
			result=_T("");
		}

	csvFile.Write();
	csvFile.Close();
	crewListFile->Close();
}



void CrewList::saveHTML(wxString savePath, wxString layout, bool mode)
{
	wxString path;

	if(layout == _T(""))
	{
#ifdef __WXOSX__
        MessageBoxOSX(NULL,_("Sorry, no Layout installed"),_("Information"),wxID_OK);
#else        
		wxMessageBox(_("Sorry, no Layout installed"),_("Information"),wxOK);
#endif
		return;
	}

	wxString html = readLayout(layout);

	int indexTop;
	int indexBottom;
	int indexWakeTop;
	int indexWakeBottom;

	wxString topHTML;
	wxString bottomHTML;
	wxString middleHTML;
	wxString topWakeHTML;
	wxString bottomWakeHTML;
	wxString middleWakeHTML;
	wxString headerHTML;

	wxString seperatorTop        = wxT("<!--Repeat -->");
	wxString seperatorBottom     = wxT("<!--Repeat End -->");
	wxString seperatorWakeTop    = wxT("<!--Repeat Wake -->");
	wxString seperatorWakeBottom = wxT("<!--Repeat Wake End -->");

	if(!html.Contains(_T("<!--Repeat Wake -->")))
	{
		indexTop        = html.First(seperatorTop)+seperatorTop.Len();
		indexBottom     = html.First(seperatorBottom)+seperatorBottom.Len();
		topHTML        = html.substr(0,indexTop);
		bottomHTML     = html.substr(indexBottom,html.Len()-indexBottom-1);
		middleHTML     = html.substr(indexTop,indexBottom-indexTop);
	}
	else if(!html.Contains(_T("<!--Repeat -->")))
	{
		indexWakeTop        = html.First(seperatorWakeTop)+seperatorWakeTop.Len();
		indexWakeBottom     = html.First(seperatorWakeBottom)+seperatorWakeBottom.Len();
		topHTML				= html.substr(0,indexWakeTop);
		bottomWakeHTML		= html.substr(indexWakeBottom,html.Len()-indexWakeBottom-1);
		middleWakeHTML		= html.substr(indexWakeTop,indexWakeBottom-indexWakeTop);
		topHTML.Replace(wxT("#LFROM#"),_("from"));
		topHTML.Replace(wxT("#LTO#"),_("to"));
		topHTML.Replace(wxT("#LWATCH#"),dialog->m_gridGlobal->GetColLabelValue(4));
	}
	else
	{
		indexTop        = html.First(seperatorTop)+seperatorTop.Len();
		indexBottom     = html.First(seperatorBottom)+seperatorBottom.Len();
		indexWakeTop    = html.First(seperatorWakeTop)+seperatorWakeTop.Len();
		indexWakeBottom = html.First(seperatorWakeBottom)+seperatorWakeBottom.Len();
	
		topHTML        = html.substr(0,indexTop);
		bottomHTML     = html.substr(indexBottom,indexWakeTop-indexBottom);
		middleHTML     = html.substr(indexTop,indexBottom-indexTop);
		bottomWakeHTML = html.substr(indexWakeBottom,html.Len()-1);
		middleWakeHTML = html.substr(indexWakeTop,indexWakeBottom-indexWakeTop);
		bottomHTML.Replace(wxT("#LFROM#"),_("from"));
		bottomHTML.Replace(wxT("#LTO#"),_("to"));
		bottomHTML.Replace(wxT("#LWATCH#"),dialog->m_gridGlobal->GetColLabelValue(4));
	}

	path = data_locn;
	wxTextFile *logFile = new wxTextFile(path);
	if(mode != 0)
		path.Replace(wxT("txt"),wxT("html"));
	else 
		path = savePath;

	wxFileOutputStream output( path );
	wxTextOutputStream htmlFile(output);

	if(::wxFileExists(path))
		::wxRemoveFile(path);

	logFile->Open();

	wxString newMiddleHTML;
	wxString newWakeHTML;

	topHTML.Replace(wxT("#TYPE#"),Export::replaceNewLine(mode,dialog->boatType->GetValue(),false));
	topHTML.Replace(wxT("#BOATNAME#"),Export::replaceNewLine(mode,dialog->boatName->GetValue(),false));
	topHTML.Replace(wxT("#HOMEPORT#"),Export::replaceNewLine(mode,dialog->homeport->GetValue(),false));
	topHTML.Replace(wxT("#CALLSIGN#"),Export::replaceNewLine(mode,dialog->callsign->GetValue(),false));
	topHTML.Replace(wxT("#REGISTRATION#"),Export::replaceNewLine(mode,dialog->registration->GetValue(),false));

	topHTML.Replace(wxT("#LTYPE#"),Export::replaceNewLine(mode,dialog->m_staticText128->GetLabel(),true));
	topHTML.Replace(wxT("#LBOATNAME#"),Export::replaceNewLine(mode,dialog->bname->GetLabel(),true));
	topHTML.Replace(wxT("#LHOMEPORT#"),Export::replaceNewLine(mode,dialog->m_staticText114->GetLabel(),true));
	topHTML.Replace(wxT("#LCALLSIGN#"),Export::replaceNewLine(mode,dialog->m_staticText115->GetLabel(),true));
	topHTML.Replace(wxT("#LREGISTRATION#"),Export::replaceNewLine(mode,dialog->m_staticText118->GetLabel(),true));
	topHTML.Replace(wxT("#LCREWLIST#"),Export::replaceNewLine(mode,dialog->m_logbook->GetPageText(2),true));

	if(html.Contains(_T("<!--Repeat -->")))
	{	
		htmlFile << topHTML;

		int rowsMax = dialog->m_gridCrew->GetNumberRows();
		int colsMax = dialog->m_gridCrew->GetNumberCols();
		for(int row = 0; row < rowsMax; row++)
		{
			if(dialog->m_menu2->IsChecked(MENUCREWONBOARD) && dialog->m_gridCrew->GetCellValue(row,ONBOARD) == _T(""))
				continue;
			newMiddleHTML = middleHTML;
			for(int col = 0; col < colsMax; col++)
				newMiddleHTML = replacePlaceholder(newMiddleHTML,headerHTML,0,row,col,0);
			htmlFile << newMiddleHTML;
		}
		htmlFile << bottomHTML;
		topHTML = _T("");
	}

	if(html.Contains(_T("<!--Repeat Wake -->")))
	{	
		htmlFile << topHTML;

		int rowsMax = dialog->m_gridCrewWake->GetNumberRows();
		int colsMax = dialog->m_gridCrewWake->GetNumberCols();
		for(int row = 0; row < rowsMax; row++)
		{
			newMiddleHTML = middleWakeHTML;
			for(int col = 0; col < colsMax; col++)
				newMiddleHTML = replacePlaceholder(newMiddleHTML,headerHTML,1,row,col,0);
			htmlFile << newMiddleHTML;
		}
		htmlFile << bottomWakeHTML;
	}
	
	logFile->Close();
	output.Close();
}

wxString CrewList::replacePlaceholder(wxString html,wxString s, bool nGrid, int row, int col, bool mode)
{
		wxGrid* grid = dialog->m_gridCrew;	
		wxGrid* wake = dialog->m_gridCrewWake;

			switch((int)nGrid) // Apple needs int
			{
			case 0:
				switch(col)
				{
				case NAME:				html.Replace(wxT("#NAME#"),Export::replaceNewLine(mode,grid->GetCellValue(row,col),false));
										html.Replace(wxT("#LNAME#"),Export::replaceNewLine(mode,grid->GetColLabelValue(col),true));
										break;
					case BIRTHNAME:		html.Replace(wxT("#BIRTHNAME#"),Export::replaceNewLine(mode,grid->GetCellValue(row,col),false));
										html.Replace(wxT("#LBIRTHNAME#"),Export::replaceNewLine(mode,grid->GetColLabelValue(col),true));
										break;
					case FIRSTNAME:		html.Replace(wxT("#FIRSTNAME#"),Export::replaceNewLine(mode,grid->GetCellValue(row,col),false));
										html.Replace(wxT("#LFIRSTNAME#"),Export::replaceNewLine(mode,grid->GetColLabelValue(col),true));
										break;
					case TITLE:			html.Replace(wxT("#TITLE#"),Export::replaceNewLine(mode,grid->GetCellValue(row,col),false));
										html.Replace(wxT("#LTITLE#"),Export::replaceNewLine(mode,grid->GetColLabelValue(col),true));		
										break;
					case BIRTHPLACE:	html.Replace(wxT("#BIRTHPLACE#"),Export::replaceNewLine(mode,grid->GetCellValue(row,col),false));
										html.Replace(wxT("#LBIRTHPLACE#"),Export::replaceNewLine(mode,grid->GetColLabelValue(col),true));
										break;
					case BIRTHDATE:		html.Replace(wxT("#BIRTHDATE#"),Export::replaceNewLine(mode,grid->GetCellValue(row,col),false));
										html.Replace(wxT("#LBIRTHDATE#"),Export::replaceNewLine(mode,grid->GetColLabelValue(col),true));
										break;
					case NATIONALITY:	html.Replace(wxT("#NATIONALITY#"),Export::replaceNewLine(mode,grid->GetCellValue(row,col),false));
										html.Replace(wxT("#LNATIONALITY#"),Export::replaceNewLine(mode,grid->GetColLabelValue(col),true));	
										break;
					case PASSPORT:		html.Replace(wxT("#PASSPORT#"),Export::replaceNewLine(mode,grid->GetCellValue(row,col),false));
										html.Replace(wxT("#LPASSPORT#"),Export::replaceNewLine(mode,grid->GetColLabelValue(col),true));
										break;
					case EST_IN:		html.Replace(wxT("#EST_IN#"),Export::replaceNewLine(mode,grid->GetCellValue(row,col),false));
										html.Replace(wxT("#LEST_IN#"),Export::replaceNewLine(mode,grid->GetColLabelValue(col),true));
										break;
					case EST_ON:		html.Replace(wxT("#EST_ON#"),Export::replaceNewLine(mode,grid->GetCellValue(row,col),false));
										html.Replace(wxT("#LEST_ON#"),Export::replaceNewLine(mode,grid->GetColLabelValue(col),true));
										break;
					case ZIP:			html.Replace(wxT("#ZIP#"),Export::replaceNewLine(mode,grid->GetCellValue(row,col),false));
										html.Replace(wxT("#LZIP#"),Export::replaceNewLine(mode,grid->GetColLabelValue(col),true));
										break;
					case COUNTRY:		html.Replace(wxT("#COUNTRY#"),Export::replaceNewLine(mode,grid->GetCellValue(row,col),false));
										html.Replace(wxT("#LCOUNTRY#"),Export::replaceNewLine(mode,grid->GetColLabelValue(col),true));	
										break;
					case TOWN:			html.Replace(wxT("#TOWN#"),Export::replaceNewLine(mode,grid->GetCellValue(row,col),false));
										html.Replace(wxT("#LTOWN#"),Export::replaceNewLine(mode,grid->GetColLabelValue(col),true));
										break;
					case STREET:		html.Replace(wxT("#STREET#"),Export::replaceNewLine(mode,grid->GetCellValue(row,col),false));
										html.Replace(wxT("#LSTREET#"),Export::replaceNewLine(mode,grid->GetColLabelValue(col),true));	
										break;
				}
				break;
			case 1:
				switch(col)
				{
					case LWNAME:		html.Replace(wxT("#NAME#"),Export::replaceNewLine(mode,wake->GetCellValue(row,col),false));
										html.Replace(wxT("#LNAME#"),Export::replaceNewLine(mode,wake->GetColLabelValue(col),true));	
										break;
					case LWFIRSTNAME:	html.Replace(wxT("#FIRSTNAME#"),Export::replaceNewLine(mode,wake->GetCellValue(row,col),false));
										html.Replace(wxT("#LFIRSTNAME#"),Export::replaceNewLine(mode,wake->GetColLabelValue(col),true));	
										break;
					case WAKESTART1:	html.Replace(wxT("#WAKE1#"),Export::replaceNewLine(mode,wake->GetCellValue(row,col),false));
										html.Replace(wxT("#LWAKE1#"),Export::replaceNewLine(mode,wake->GetColLabelValue(col),true));	
										break;
					case WAKEEND1:		html.Replace(wxT("#WAKEE1#"),Export::replaceNewLine(mode,wake->GetCellValue(row,col),false));
										html.Replace(wxT("#LWAKEE1#"),Export::replaceNewLine(mode,wake->GetColLabelValue(col),true));	
										break;
					case WAKESTART2:	html.Replace(wxT("#WAKE2#"),Export::replaceNewLine(mode,wake->GetCellValue(row,col),false));
										html.Replace(wxT("#LWAKE2#"),Export::replaceNewLine(mode,wake->GetColLabelValue(col),true));	
										break;
					case WAKEEND2:		html.Replace(wxT("#WAKEE2#"),Export::replaceNewLine(mode,wake->GetCellValue(row,col),false));
										html.Replace(wxT("#LWAKEE2#"),Export::replaceNewLine(mode,wake->GetColLabelValue(col),true));	
										break;
					case WAKESTART3:	html.Replace(wxT("#WAKE3#"),Export::replaceNewLine(mode,wake->GetCellValue(row,col),false));
										html.Replace(wxT("#LWAKE3#"),Export::replaceNewLine(mode,wake->GetColLabelValue(col),true));	
										break;
					case WAKEEND3:		html.Replace(wxT("#WAKEE3#"),Export::replaceNewLine(mode,wake->GetCellValue(row,col),false));
										html.Replace(wxT("#LWAKEE3#"),Export::replaceNewLine(mode,wake->GetColLabelValue(col),true));		
										break;
					case WAKESTART4:	html.Replace(wxT("#WAKE4#"),Export::replaceNewLine(mode,wake->GetCellValue(row,col),false));
										html.Replace(wxT("#LWAKE4#"),Export::replaceNewLine(mode,wake->GetColLabelValue(col),true));	
										break;
					case WAKEEND4:		html.Replace(wxT("#WAKEE4#"),Export::replaceNewLine(mode,wake->GetCellValue(row,col),false));
										html.Replace(wxT("#LWAKEE4#"),Export::replaceNewLine(mode,wake->GetColLabelValue(col),true));	
										break;
					case WAKESTART5:	html.Replace(wxT("#WAKE5#"),Export::replaceNewLine(mode,wake->GetCellValue(row,col),false));
										html.Replace(wxT("#LWAKE5#"),Export::replaceNewLine(mode,wake->GetColLabelValue(col),true));	
										break;
					case WAKEEND5:		html.Replace(wxT("#WAKEE5#"),Export::replaceNewLine(mode,wake->GetCellValue(row,col),false));
										html.Replace(wxT("#LWAKEE5#"),Export::replaceNewLine(mode,wake->GetColLabelValue(col),true));	
										break;
					case WAKESTART6:	html.Replace(wxT("#WAKE6#"),Export::replaceNewLine(mode,wake->GetCellValue(row,col),false));
										html.Replace(wxT("#LWAKE6#"),Export::replaceNewLine(mode,wake->GetColLabelValue(col),true));	
										break;
					case WAKEEND6:		html.Replace(wxT("#WAKEE6#"),Export::replaceNewLine(mode,wake->GetCellValue(row,col),false));
										html.Replace(wxT("#LWAKEE6#"),Export::replaceNewLine(mode,wake->GetColLabelValue(col),true));	
										break;
				}
				break;
			}
	html.Replace(wxT("#LADRESS#"),_("Adress"));

	wxString str(html);
	return str;
}

wxString CrewList::readLayout(wxString layoutFileName)
{
	wxString html, path;

	path = layout_locn + layoutFileName + wxT(".html");;
	wxTextFile layout(path);
	
	layout.Open();

	for(unsigned int i = 0; i < layout.GetLineCount(); i++)
	{
		html += layout.GetLine(i);
	}

	layout.Close();

	return html;
}

void CrewList::viewHTML(wxString path, wxString layout)
{
	saveHTML(path, layout, true);

	if(layout != _T(""))
		dialog->startBrowser(html_locn);
}

void CrewList::viewODT(wxString path, wxString layout)
{
	saveODT(path, layout, true);
	
	if(layout != _T(""))
		dialog->startApplication(ODT_locn,_T(".odt"));
}

void CrewList::saveODT(wxString savePath,wxString layout, bool mode)
{
	wxString path;

	if(layout == _T(""))
	{
#ifdef __WXOSX__
        MessageBoxOSX(NULL,_("Sorry, no Layout installed"),_("Information"),wxID_OK);
#else
		wxMessageBox(_("Sorry, no Layout installed"),_("Information"),wxOK);
#endif
		return;
	}

	saveData();

	wxString odt = readLayoutODT(layout);

	int indexTop;
	int indexBottom;
	int indexWakeTop;
	int indexWakeBottom;
	int indexTop1;
	int indexBottom1;
	int indexWakeTop1;
	int indexWakeBottom1;

	wxString topODT;
	wxString bottomODT;
	wxString middleODT;
	wxString topWakeODT;
	wxString bottomWakeODT;
	wxString middleWakeODT;
	wxString headerODT;
	wxString middleData = _T("");

	wxString seperatorTop        = wxT("[[");
	wxString seperatorBottom     = wxT("]]");
	wxString seperatorWakeTop    = wxT("{{");
	wxString seperatorWakeBottom = wxT("}}");

	if(!odt.Contains(seperatorWakeTop))
	{
		indexTop        = odt.First(seperatorTop);
		topODT          = odt.substr(0,indexTop);
		indexTop1        = topODT.find_last_of('<');
		topODT          = odt.substr(0,indexTop1);
		indexBottom     = odt.First(seperatorBottom);
		bottomODT       = odt.substr(indexBottom);
		indexBottom1     = bottomODT.find_first_of('>');
		bottomODT       = bottomODT.substr(indexBottom1+1);
		middleODT       = odt.substr(indexTop+11,indexBottom);
		indexTop		= middleODT.First(seperatorBottom);
		middleODT       = middleODT.substr(0,indexTop);
		indexTop	    = middleODT.find_last_of('<');
		middleODT       = middleODT.substr(0,indexTop);
	}
	else if(!odt.Contains(seperatorTop))
	{
		indexWakeTop        = odt.First(seperatorWakeTop);
		topODT				= odt.substr(0,indexWakeTop);
		indexWakeTop1		= topODT.find_last_of('<');
		topODT				= topODT.substr(0,indexWakeTop1);
		indexWakeBottom     = odt.First(seperatorWakeBottom);
		bottomWakeODT		= odt.substr(indexWakeBottom);
		indexWakeBottom1	= bottomWakeODT.find_first_of('>');
		bottomODT	    = bottomWakeODT.substr(indexWakeBottom1+1);
		middleODT			= odt.substr(indexWakeTop+11,indexWakeBottom);
		indexWakeTop		= middleODT.First(seperatorWakeBottom);
		middleODT			= middleODT.substr(0,indexWakeTop);
		indexWakeTop	    = middleODT.find_last_of('<');
		middleWakeODT		= middleODT.substr(0,indexWakeTop);
		topODT.Replace(wxT("#LFROM#"),_("from"));
		topODT.Replace(wxT("#LTO#"),_("to"));
		topODT.Replace(wxT("#LWATCH#"),Export::replaceNewLine(mode,dialog->m_gridGlobal->GetColLabelValue(4),true));
	}
	else
	{
		indexTop        = odt.First(seperatorTop);
		topODT          = odt.substr(0,indexTop);
		indexTop1       = topODT.find_last_of('<');
		topODT          = topODT.substr(0,indexTop1);
		indexBottom     = odt.First(seperatorBottom);
		bottomODT       = odt.substr(indexBottom);
		indexBottom1     = bottomODT.find_first_of('>');
		bottomODT       = bottomODT.substr(indexBottom1+1);

		middleODT       = odt.substr(indexTop+11,indexBottom);
		indexTop		= middleODT.First(seperatorBottom);
		middleODT       = middleODT.substr(0,indexTop);
		indexTop	    = middleODT.find_last_of('<');
		middleODT       = middleODT.substr(0,indexTop);

		middleData			= bottomODT;
		indexTop			= middleData.Find(seperatorWakeTop);
		middleData			= middleData.substr(0,indexTop);
		middleData			= middleData.substr(0,middleData.find_last_of('<'));
		indexWakeTop        = odt.First(seperatorWakeTop);

		indexWakeBottom     = odt.First(seperatorWakeBottom);
		bottomWakeODT		= odt.substr(indexWakeBottom);
		indexWakeBottom1	= bottomWakeODT.find_first_of('>');
		bottomODT			= bottomWakeODT.substr(indexWakeBottom1+1);
		middleWakeODT		= odt.substr(indexWakeTop+11,indexWakeBottom);
		indexWakeTop		= middleWakeODT.First(seperatorWakeBottom);
		middleWakeODT		= middleWakeODT.substr(0,indexWakeTop);
		indexWakeTop	    = middleWakeODT.find_last_of('<');
		middleWakeODT		= middleWakeODT.substr(0,indexWakeTop);

		middleData.Replace(wxT("#LFROM#"),_("from"));
		middleData.Replace(wxT("#LTO#"),_("to"));
		middleData.Replace(wxT("#LWATCH#"),Export::replaceNewLine(mode,dialog->m_gridGlobal->GetColLabelValue(4),true));
	}

	topODT.Replace(wxT("#TYPE#"),Export::replaceNewLine(mode,dialog->boatType->GetValue(),false));
	topODT.Replace(wxT("#BOATNAME#"),Export::replaceNewLine(mode,dialog->boatName->GetValue(),false));
	topODT.Replace(wxT("#HOMEPORT#"),Export::replaceNewLine(mode,dialog->homeport->GetValue(),false));
	topODT.Replace(wxT("#CALLSIGN#"),Export::replaceNewLine(mode,dialog->callsign->GetValue(),false));
	topODT.Replace(wxT("#REGISTRATION#"),Export::replaceNewLine(mode,dialog->registration->GetValue(),false));

	topODT.Replace(wxT("#LTYPE#"),Export::replaceNewLine(mode,dialog->m_staticText128->GetLabel(),true));
	topODT.Replace(wxT("#LBOATNAME#"),Export::replaceNewLine(mode,dialog->bname->GetLabel(),true));
	topODT.Replace(wxT("#LHOMEPORT#"),Export::replaceNewLine(mode,dialog->m_staticText114->GetLabel(),true));
	topODT.Replace(wxT("#LCALLSIGN#"),Export::replaceNewLine(mode,dialog->m_staticText115->GetLabel(),true));
	topODT.Replace(wxT("#LREGISTRATION#"),Export::replaceNewLine(mode,dialog->m_staticText118->GetLabel(),true));
	topODT.Replace(wxT("#LCREWLIST#"),Export::replaceNewLine(mode,dialog->m_logbook->GetPageText(2),true));

	path = data_locn;
	wxTextFile *logFile = new wxTextFile(path);
	if(mode != 0)
		path.Replace(wxT("txt"),wxT("odt"));
	else 
		path = savePath;

	if(::wxFileExists(path))
		::wxRemoveFile(path);

	ODT_locn = path;

	logFile->Open();

	wxString newMiddleODT;
	wxString newWakeODT;

    auto_ptr<wxFFileInputStream> in(new wxFFileInputStream(layout_locn + layout + _T(".odt")));
    wxTempFileOutputStream out(path);

    wxZipInputStream inzip(*in);
    wxZipOutputStream outzip(out);
	wxTextOutputStream odtFile(outzip);
    auto_ptr<wxZipEntry> entry;

    outzip.CopyArchiveMetaData(inzip);

    while (entry.reset(inzip.GetNextEntry()), entry.get() != NULL)
        if (!entry->GetName().Matches(_T("content.xml")))
            if (!outzip.CopyEntry(entry.release(), inzip))
                break;

    in.reset();

	outzip.PutNextEntry(_T("content.xml"));

	odtFile << topODT;

	if(odt.Contains(seperatorTop))
	{
		int rowsMax = dialog->m_gridCrew->GetNumberRows();
		int colsMax = dialog->m_gridCrew->GetNumberCols();
		for(int row = 0; row < rowsMax; row++)
		{
			if(dialog->m_menu2->IsChecked(MENUCREWONBOARD) && dialog->m_gridCrew->GetCellValue(row,ONBOARD) == _T(""))
				continue;
			newMiddleODT = middleODT;
			for(int col = 0; col < colsMax; col++)
				newMiddleODT = replacePlaceholder(newMiddleODT,headerODT,0,row,col,mode);
			odtFile << newMiddleODT;
		}
	}

	if(!middleData.IsEmpty())
		odtFile << middleData;

	if(odt.Contains(seperatorWakeTop))
	{
		int rowsMax = dialog->m_gridCrewWake->GetNumberRows();
		int colsMax = dialog->m_gridCrewWake->GetNumberCols();
		for(int row = 0; row < rowsMax; row++)
		{
			newWakeODT = middleWakeODT;
			for(int col = 0; col < colsMax; col++)
				newWakeODT = replacePlaceholder(newWakeODT,headerODT,1,row,col,mode);
			odtFile << newWakeODT;
		}
	}
	odtFile << bottomODT;
	inzip.Eof() && outzip.Close() && out.Commit();
	logFile->Close();
}

wxString CrewList::readLayoutODT(wxString layout)
{
	wxString odt = _T("");

	wxString filename = layout_locn + layout + _T(".odt");

	if(wxFileExists(filename))
	{
		static const wxString fn = _T("content.xml");
		wxZipInputStream zip(filename,fn);
		wxTextInputStream txt(zip);
		while(!zip.Eof())
			odt += txt.ReadLine();
	}
	return odt;
}

void CrewList::deleteRow(int row)
{
#ifdef __WXOSX__
    int answer = MessageBoxOSX(NULL,wxString::Format(_("Delete Row Nr. %i ?"),row+1), _("Confirm"), wxID_NO | wxID_CANCEL | wxID_OK);
    	if (answer == wxID_OK)
        {
            gridCrew->DeleteRows(row);
			int row = searchInWatch();
			if(row >= 0)
				gridWake->DeleteRows(row);
            
            crewListFile->Open();
            crewListFile->RemoveLine(row);
            crewListFile->Write();
            crewListFile->Close();
        }
#else
	int answer = wxMessageBox(wxString::Format(_("Delete Row Nr. %i ?"),row+1), _("Confirm"),
                              wxYES_NO | wxCANCEL, 0);
	if (answer == wxYES)
	{
		gridCrew->DeleteRows(row);
		int row = searchInWatch();
		if(row >= 0)
			gridWake->DeleteRows(row);
		modified = true;
	}
#endif
}

void CrewList::saveXML(wxString path)
{
	wxString s = _T("");
	wxString line;
	wxString temp;

	wxTextFile* xmlFile = new wxTextFile(path);

	if(xmlFile->Exists())
	{
		::wxRemoveFile(path);
		xmlFile->Create();
	}

	crewListFile->Open();

	if(crewListFile->GetLineCount() <= 0)
	{
#ifdef __WXOSX__
        MessageBoxOSX(NULL,_("Sorry, Logbook has no lines"),_("Information"),wxID_OK); 
#else
		wxMessageBox(_("Sorry, Logbook has no lines"),_("Information"),wxOK);
#endif
		return;
	}

	xmlFile->AddLine(dialog->xmlHead);
	for(unsigned int i = 0; i < crewListFile->GetLineCount(); i++)
	{
		line = crewListFile->GetLine(i);
		wxStringTokenizer tkz(line, _T("\t"),wxTOKEN_RET_EMPTY );
		s = wxString::Format(_T("<Row ss:Height=\"%u\">"),dialog->m_gridGlobal->GetRowHeight(i));

		while ( tkz.HasMoreTokens() )
		{ 
			s += _T("<Cell>\n");
			s += _T("<Data ss:Type=\"String\">#DATA#</Data>\n");
			temp = tkz.GetNextToken().RemoveLast();
			temp.Replace(_T("\\n"),_T("&#10;"));
			temp.Replace(_T("&"),_T("&amp;"));
			temp.Replace(_T("\""),_T("&quot;"));
			temp.Replace(_T("<"),_T("&lt;"));
			temp.Replace(_T(">"),_T("&gt;"));
			temp.Replace(_T("'"),_T("&apos;"));
			s.Replace(_T("#DATA#"),temp);
			s += _T("</Cell>");
		}
		s += _T("</Row>>");
		xmlFile->AddLine(s);
	}

	xmlFile->AddLine(dialog->xmlEnd);
	xmlFile->Write();
	crewListFile->Close();
	xmlFile->Close();
}

void CrewList::backup(wxString path)
{
	wxCopyFile(data_locn,path);
}

void CrewList::saveODS(wxString path)
{
	wxString s = _T("");
	wxString line;
	wxString temp;

	wxFileInputStream input( data_locn );
	wxTextInputStream* stream = new wxTextInputStream (input);

	wxFFileOutputStream out(path);
	wxZipOutputStream zip(out);
	wxTextOutputStream txt(zip);
	wxString sep(wxFileName::GetPathSeparator());

	temp = dialog->content;
	temp.Replace(_T("table:number-columns-repeated=\"33\""),_T("table:number-columns-repeated=\"14\""));
	temp.Replace(_T("Logbook"),_T("CrewList"));
	zip.PutNextEntry(wxT("content.xml"));
	txt << temp;

	txt << _T("<table:table-row table:style-name=\"ro2\">");

	for(int i = 0; i < dialog->m_gridCrew->GetNumberCols(); i++)
	{
		txt << _T("<table:table-cell office:value-type=\"string\">");
		txt << _T("<text:p>");
		txt << dialog->m_gridCrew->GetColLabelValue(i);
		txt << _T("</text:p>");
		txt << _T("</table:table-cell>");
	}

	txt << _T("</table:table-row>");

//	bool empty = false;
	long emptyCol = 0;
	while(wxString line = stream->ReadLine())
	{
		if(input.Eof()) break;
		txt << _T("<table:table-row table:style-name=\"ro2\">");
		wxStringTokenizer tkz(line, _T("\t"),wxTOKEN_RET_EMPTY);

		while ( tkz.HasMoreTokens() )
		{ 
			wxString s = dialog->restoreDangerChar(tkz.GetNextToken().RemoveLast());
			if(s == _T(""))
			{
				txt <<  _T("<table:table-cell />");
				//empty = true;
				emptyCol++;
				continue;
			}

			txt << _T("<table:table-cell office:value-type=\"string\">");

			wxStringTokenizer str(s, _T("\n"));
			while(str.HasMoreTokens())
			{
				wxString e = str.GetNextToken();
				e.Replace(_T("&"),_T("&amp;"));
				e.Replace(_T("\""),_T("&quot;"));
				e.Replace(_T("<"),_T("&lt;"));
				e.Replace(_T(">"),_T("&gt;"));
				e.Replace(_T("'"),_T("&apos;"));
				txt << _T("<text:p>");
				txt << e;
				txt << _T("</text:p>");
			}
			txt << _T("</table:table-cell>");
		}
		txt << _T("</table:table-row>");;

	}
	txt << dialog->contentEnd;

	zip.PutNextEntry(wxT("mimetype"));
	txt << wxT("application/vnd.oasis.opendocument.spreadsheet");

	zip.PutNextEntry(wxT("styles.xml"));
	txt << dialog->styles;

	zip.PutNextEntry(wxT("meta.xml"));
	txt << dialog->meta;

	zip.PutNextEntry(wxT("META-INF") + sep + wxT("manifest.xml"));
	txt << dialog->manifest;

	zip.PutNextEntry(wxT("Thumbnails") + sep);

	zip.PutNextEntry(wxT("Configurations2") + sep + wxT("floater"));
	zip.PutNextEntry(wxT("Configurations2") + sep + wxT("menubar"));
	zip.PutNextEntry(wxT("Configurations2") + sep + wxT("popupmenu"));
	zip.PutNextEntry(wxT("Configurations2") + sep + wxT("progressbar"));
	zip.PutNextEntry(wxT("Configurations2") + sep + wxT("statusbar"));
	zip.PutNextEntry(wxT("Configurations2") + sep + wxT("toolbar"));
	zip.PutNextEntry(wxT("Configurations2") + sep + wxT("images") + sep + wxT("Bitmaps"));

	zip.Close();
	out.Close();
}



//////////////////////// Automatic Watch Dialog ////////////////////

AutomaticWatch::AutomaticWatch( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->parent = (LogbookDialog*) parent;

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer34;
	fgSizer34 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer34->SetFlexibleDirection( wxBOTH );
	fgSizer34->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText87 = new wxStaticText( this, wxID_ANY, _("1. Watch of 1. Person"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText87->Wrap( -1 );
	fgSizer34->Add( m_staticText87, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrlStartTime = new wxTextCtrl( this, wxID_ANY, wxT("00:00"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	fgSizer34->Add( m_textCtrlStartTime, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer23->Add( fgSizer34, 0, wxALIGN_CENTER, 5 );
	
	m_staticline27 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer23->Add( m_staticline27, 0, wxEXPAND | wxALL, 5 );
	
	m_staticText86 = new wxStaticText( this, wxID_ANY, _("Drag 'n Drop to change the order in the list"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText86->Wrap( -1 );
	bSizer23->Add( m_staticText86, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_listCtrlWatchNames = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxLC_ALIGN_LEFT|wxLC_REPORT|wxLC_SINGLE_SEL|wxALWAYS_SHOW_SB );
	bSizer23->Add( m_listCtrlWatchNames, 1, wxALL|wxEXPAND, 5 );
	
	m_staticline28 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer23->Add( m_staticline28, 0, wxEXPAND | wxALL, 5 );

/*	wxFlexGridSizer* fgSizer46;
	fgSizer46 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer46->SetFlexibleDirection( wxBOTH );
	fgSizer46->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_radioBtnStatic = new wxRadioButton( this, wxID_ANY, _("Static Watchtimes"), wxDefaultPosition, wxDefaultSize, 0 );
	m_radioBtnStatic->SetValue( true ); 
	fgSizer46->Add( m_radioBtnStatic, 0, wxALL, 5 );
	
	m_radioBtnIndividual = new wxRadioButton( this, wxID_ANY, _("Individiual Watchtimes"), wxDefaultPosition, wxDefaultSize, 0 );
	m_radioBtnIndividual->SetToolTip( _("Experimental") );
	
	fgSizer46->Add( m_radioBtnIndividual, 0, wxALL, 5 );
	
	bSizer23->Add( fgSizer46, 0, wxALIGN_CENTER_HORIZONTAL, 5 );*/
	wxFlexGridSizer* fgSizer341;
	fgSizer341 = new wxFlexGridSizer( 0, 6, 0, 0 );
	fgSizer341->SetFlexibleDirection( wxBOTH );
	fgSizer341->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText80 = new wxStaticText( this, wxID_ANY, _("Persons:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText80->Wrap( -1 );
	fgSizer341->Add( m_staticText80, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticTextPersons = new wxStaticText( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 20,-1 ), 0 );
	m_staticTextPersons->Wrap( -1 );
	fgSizer341->Add( m_staticTextPersons, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText82 = new wxStaticText( this, wxID_ANY, _("Watchtime:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText82->Wrap( -1 );
	fgSizer341->Add( m_staticText82, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticTextLengthWatch = new wxStaticText( this, wxID_ANY, wxT("00:00"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	m_staticTextLengthWatch->Wrap( -1 );
	fgSizer341->Add( m_staticTextLengthWatch, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText85 = new wxStaticText( this, wxID_ANY, _("No. Watches"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText85->Wrap( -1 );
	fgSizer341->Add( m_staticText85, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_choice20Choices[] = { wxT("1"), wxT("2"), wxT("3"), wxT("4"), wxT("5"), wxT("6") };
	int m_choice20NChoices = sizeof( m_choice20Choices ) / sizeof( wxString );
	m_choice20 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxSize( 40,-1 ), m_choice20NChoices, m_choice20Choices, 0 );
	m_choice20->SetSelection( 0 );
	fgSizer341->Add( m_choice20, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer23->Add( fgSizer341, 0, wxALIGN_CENTER, 5 );
	
	m_staticline26 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer23->Add( m_staticline26, 0, wxEXPAND | wxALL, 5 );
	
	m_sdbSizer4 = new wxStdDialogButtonSizer();
	m_sdbSizer4OK = new wxButton( this, wxID_OK );
	m_sdbSizer4->AddButton( m_sdbSizer4OK );
	m_sdbSizer4Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer4->AddButton( m_sdbSizer4Cancel );
	m_sdbSizer4->Realize();
	bSizer23->Add( m_sdbSizer4, 0, wxALIGN_CENTER, 5 );
	
	this->SetSizer( bSizer23 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( AutomaticWatch::OnInit ) );
	m_listCtrlWatchNames->Connect( wxEVT_COMMAND_LIST_BEGIN_DRAG, wxListEventHandler( AutomaticWatch::OnListBeginDrag ), NULL, this );
//	m_radioBtnStatic->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( AutomaticWatch::OnRadioButtonStatic ), NULL, this );
//	m_radioBtnIndividual->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( AutomaticWatch::OnRadioButtonIndividual ), NULL, this );
	m_choice20->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AutomaticWatch::OnChoice ), NULL, this );
}

AutomaticWatch::~AutomaticWatch()
{
	// Disconnect Events
	this->Disconnect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( AutomaticWatch::OnInit ) );
	m_listCtrlWatchNames->Disconnect( wxEVT_COMMAND_LIST_BEGIN_DRAG, wxListEventHandler( AutomaticWatch::OnListBeginDrag ), NULL, this );
//	m_radioBtnStatic->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( AutomaticWatch::OnRadioButtonStatic ), NULL, this );
//	m_radioBtnIndividual->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( AutomaticWatch::OnRadioButtonIndividual ), NULL, this );
	m_choice20->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AutomaticWatch::OnChoice ), NULL, this );

	
}


void AutomaticWatch::OnInit( wxInitDialogEvent& event )
{
	wxListItem itemCol;
	long i;

	itemCol.SetText(_("Name"));
	itemCol.SetImage(-1);
	itemCol.SetWidth(150);
	m_listCtrlWatchNames->InsertColumn(0, itemCol);
	itemCol.SetText(_("Firstname"));
	itemCol.SetImage(-1);
	itemCol.SetWidth(150);
	m_listCtrlWatchNames->InsertColumn(1, itemCol);

	for(i = 0; i < parent->m_gridCrewWake->GetNumberRows(); i++)
	{
		long index = m_listCtrlWatchNames->InsertItem(i,parent->m_gridCrewWake->GetCellValue(i,CrewList::LWNAME));
		m_listCtrlWatchNames->SetItem(index,1,parent->m_gridCrewWake->GetCellValue(i,CrewList::LWFIRSTNAME));
	}

	setStrings(i);
	
	if(parent->m_gridCrewWake->GetCellValue(0,2) != wxEmptyString)
		this->m_textCtrlStartTime->SetValue(parent->m_gridCrewWake->GetCellValue(0,2));
	
	m_ctrlText = wxEmptyString;
	dndIndex = -1;
	m_listCtrlWatchNames->SetDropTarget(new DnDText(m_listCtrlWatchNames, this));	
	
	//this->Fit();

}

void AutomaticWatch::OnChoice( wxCommandEvent& event )
{
	setStrings(this->m_listCtrlWatchNames->GetItemCount());
}

void AutomaticWatch::setStrings(int i)
{
	this->m_staticTextPersons->SetLabel(wxString::Format(_T("%i"),i));
	wxDateTime dt = wxDateTime::Now();
	long sec = 24*(60*60);
	int noWatches = this->m_choice20->GetSelection()+1;
	int watchtime = wxRound(((double)sec / (double)i) / (double)noWatches);
	dt = dt.Set(0,0);
	wxTimeSpan diff(0,0,watchtime);
	dt.Add(diff);
	this->m_staticTextLengthWatch->SetLabel(dt.Format(_T("%H:%M")).c_str());
}

void AutomaticWatch::OnListBeginDrag( wxListEvent& event )
{
	m_ctrlText = wxEmptyString;
	
	int row = event.GetIndex();
	dndIndex = row;
	
	wxListItem info;
	info.SetId(row);
	info.m_col = 0;
	info.m_mask = wxLIST_MASK_TEXT;
	m_listCtrlWatchNames->GetItem(info);

	info.SetId(row);
	info.m_col = 1;
	info.m_mask = wxLIST_MASK_TEXT;
	m_ctrlText += _T("\t")+info.GetText();
	m_listCtrlWatchNames->GetItem(info);
	m_ctrlText += _T("\t")+info.GetText();	
	
    wxTextDataObject data(m_ctrlText);
	wxDropSource dragSource( this );
	dragSource.SetData( data );
	dragSource.DoDragDrop( TRUE );
}

bool DnDText::OnDropText(wxCoord x, wxCoord y, const wxString& text)
{
    wxString name, first, nameold, firstold = wxEmptyString;
    
    wxStringTokenizer tkz(text,_T("\t"));
    name = tkz.GetNextToken();
    if(tkz.HasMoreTokens())
      first = tkz.GetNextToken();
    else
      first = wxEmptyString;
    
    int flag = wxLIST_HITTEST_ONITEM;
    wxPoint p = wxPoint(x,y);
    long row = m_pOwner->HitTest(p,flag );
    if(row == wxNOT_FOUND )
      return false;
    
    wxListItem info;
    info.SetId(row);
    info.m_col = 0;
    info.m_mask = wxLIST_MASK_TEXT;
    this->m_pOwner->GetItem(info);
    nameold = info.GetText();
    
    info.SetId(row);
    info.m_col = 1;
    info.m_mask = wxLIST_MASK_TEXT; 
    this->m_pOwner->GetItem(info);    
    firstold = info.GetText();
    
    this->m_pOwner->SetItem(row,0,name);
    if(first != wxEmptyString)
      this->m_pOwner->SetItem(row,1,first);  
	else
      this->m_pOwner->SetItem(row,1,wxEmptyString);  
    
    this->m_pOwner->SetItem(watch->dndIndex,0,nameold);
	if(firstold != wxEmptyString)
		this->m_pOwner->SetItem(watch->dndIndex,1,firstold); 
	else
		this->m_pOwner->SetItem(watch->dndIndex,1,wxEmptyString);
    
    this->m_pOwner->Refresh();
    
    return true;
}

//////////////// Same Watch as Dialog //////////////

SameWatchAs::SameWatchAs( wxWindow* parent, int gridRow, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->parent = ((LogbookDialog*)parent)->crewList;
	row = gridRow;

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer41;
	fgSizer41 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer41->SetFlexibleDirection( wxBOTH );
	fgSizer41->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText108 = new wxStaticText( this, wxID_ANY, _("Person:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText108->Wrap( -1 );
	fgSizer41->Add( m_staticText108, 0, wxALL, 5 );
	
	m_staticTextPersonName = new wxStaticText( this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextPersonName->Wrap( -1 );
	fgSizer41->Add( m_staticTextPersonName, 0, wxALL, 5 );
	
	bSizer23->Add( fgSizer41, 0, wxALIGN_CENTER, 5 );
	
	wxFlexGridSizer* fgSizer42;
	fgSizer42 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer42->SetFlexibleDirection( wxBOTH );
	fgSizer42->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText110 = new wxStaticText( this, wxID_ANY, _("same watch as"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText110->Wrap( -1 );
	fgSizer42->Add( m_staticText110, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choice23Choices;
	m_choice23 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice23Choices, 0 );
	m_choice23->SetSelection( 0 );
	fgSizer42->Add( m_choice23, 0, wxALL, 5 );
	
	bSizer23->Add( fgSizer42, 0, wxALIGN_CENTER, 5 );
	
	m_staticline39 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer23->Add( m_staticline39, 0, wxEXPAND | wxALL, 5 );
	
	m_sdbSizer4 = new wxStdDialogButtonSizer();
	m_sdbSizer4OK = new wxButton( this, wxID_OK );
	m_sdbSizer4->AddButton( m_sdbSizer4OK );
	m_sdbSizer4Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer4->AddButton( m_sdbSizer4Cancel );
	m_sdbSizer4->Realize();
	bSizer23->Add( m_sdbSizer4, 0, wxALIGN_CENTER, 5 );
	
	this->SetSizer( bSizer23 );
	this->Layout();
	
	this->Centre( wxBOTH );

	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( SameWatchAs::OnInitDialog ) );
}

SameWatchAs::~SameWatchAs()
{
	this->Disconnect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( SameWatchAs::OnInitDialog ) );
}

void SameWatchAs::OnInitDialog( wxInitDialogEvent& event )
{
	m_staticTextPersonName->SetLabel(parent->gridCrew->GetCellValue(row,CrewList::FIRSTNAME)+_T(",")+
		                         parent->gridCrew->GetCellValue(row,CrewList::NAME));

	for(int i = 0; i < parent->gridWake->GetNumberRows(); i++)
		m_choice23->Append(parent->gridWake->GetCellValue(i,CrewList::LWFIRSTNAME)+_T(",")+
		                         parent->gridWake->GetCellValue(i,CrewList::LWNAME));

	m_choice23->SetSelection(0);
}

