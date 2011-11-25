#include "CrewList.h"
#include "LogbookDialog.h"

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
	this->layout = layout;
	this->ODTLayout = layoutODT;
	modified = false;

	if(dialog->m_radioBtnHTMLCrew->GetValue())
		layout_locn = layout;
	else
		layout_locn = layoutODT;

	wxString crewData = data;
	crewData.Append(_T("crewlist.txt"));
	wxFileName wxHomeFiledir(crewData) ;
	if(true != wxHomeFiledir.FileExists())
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
	int count, lineCount, numRows;

	if(gridCrew->GetNumberRows() > 0)
	{
		gridCrew->DeleteRows(0,gridCrew->GetNumberRows());
		gridWake->DeleteRows(0,gridWake->GetNumberRows());
	}

	crewListFile->Open();
	lineCount = crewListFile->GetLineCount();

	if(lineCount <= 0) { crewListFile->Close(); return; }

	for( int i = 0; i < lineCount; i++)
	{
		line = crewListFile->GetLine(i);

		gridCrew->AppendRows();
		gridWake->AppendRows();

		count = gridCrew->GetNumberCols();
		numRows = gridCrew->GetNumberRows()-1;

		wxStringTokenizer tkz(line, _T("\t"),wxTOKEN_RET_EMPTY);
		int c = 0;
		grid = gridCrew;

		while ( tkz.HasMoreTokens() )
		{
			s = tkz.GetNextToken().RemoveLast();
			s = dialog->restoreDangerChar(s);
			if(c == count)
			{					
				grid = gridWake;
				c = 2;
			}

			if(grid == gridCrew && c == 0) 	gridWake->SetCellValue(numRows,0,s);
			if(grid == gridCrew && c == 2) 	gridWake->SetCellValue(numRows,1,s);
			grid->SetCellValue(numRows,c++,s);
		}
	}
	crewListFile->Close();
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
		for(int c = 2; c < gridWake->GetNumberCols(); c++)
			s += gridWake->GetCellValue(r,c)+_T(" \t");
		s.RemoveLast();
		crewListFile->AddLine(s);
		s = _T("");
	}
	crewListFile->Write();
	crewListFile->Close();
}

void CrewList::addCrew(wxGrid* grid, wxGrid* wake)
{
	wxString s;

	modified = true;

	gridCrew->AppendRows();
	gridWake->AppendRows();
	gridWake->SetCellValue(gridWake->GetNumberRows()-1,gridWake->GetNumberCols()-1,_T(" "));
}

void CrewList::changeCrew(wxGrid* grid, int row, int col, int offset)
{
	wxString result;

	modified = true;

	if(col == 0 && offset == 0)
			gridWake->SetCellValue(row,0,gridCrew->GetCellValue(row,0));
	if(col == 2 && offset == 0)
			gridWake->SetCellValue(row,1,gridCrew->GetCellValue(row,2));

	if(gridWake->GetCellValue(row,gridWake->GetNumberCols()-1) == _T(""))
		gridWake->SetCellValue(row,gridWake->GetNumberCols()-1,_T(" "));
}

void CrewList::changeCrewWake(wxGrid* grid, int row, int col, int offset)
{
	wxDateTime dt;
	wxString s = gridWake->GetCellValue(row,col);
	if(s.Len() != 4 || dt.ParseFormat(s,_T("%H%M")) == NULL)
	{
		wxMessageBox(_("Please enter 4 digits in 24h-Format like 1545 = 03:45:00 PM"));
		gridWake->SetCellValue(row,col,wxEmptyString);

		return;
	}

	gridWake->SetCellValue(row,col,dt.FormatTime());
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
		wxMessageBox(_("Sorry, no Layout installed"),_("Information"),wxOK);
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

	topHTML.Replace(wxT("#TYPE#"),dialog->boatType->GetValue());
	topHTML.Replace(wxT("#BOATNAME#"),dialog->boatName->GetValue());
	topHTML.Replace(wxT("#HOMEPORT#"),dialog->homeport->GetValue());
	topHTML.Replace(wxT("#CALLSIGN#"),dialog->callsign->GetValue());
	topHTML.Replace(wxT("#REGISTRATION#"),dialog->registration->GetValue());

	topHTML.Replace(wxT("#LTYPE#"),dialog->m_staticText128->GetLabel());
	topHTML.Replace(wxT("#LBOATNAME#"),dialog->bname->GetLabel());
	topHTML.Replace(wxT("#LHOMEPORT#"),dialog->m_staticText114->GetLabel());
	topHTML.Replace(wxT("#LCALLSIGN#"),dialog->m_staticText115->GetLabel());
	topHTML.Replace(wxT("#LREGISTRATION#"),dialog->m_staticText118->GetLabel());
	topHTML.Replace(wxT("#LCREWLIST#"),dialog->m_logbook->GetPageText(2));

	if(html.Contains(_T("<!--Repeat -->")))
	{	
		htmlFile << topHTML;

		int rowsMax = dialog->m_gridCrew->GetNumberRows();
		int colsMax = dialog->m_gridCrew->GetNumberCols();
		for(int row = 0; row < rowsMax; row++)
		{
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

			switch(nGrid)
			{
			case 0:
				switch(col)
				{
					case NAME:			html.Replace(wxT("#NAME#"),grid->GetCellValue(row,col));
										html.Replace(wxT("#LNAME#"),grid->GetColLabelValue(col));
										break;
					case BIRTHNAME:		html.Replace(wxT("#BIRTHNAME#"),grid->GetCellValue(row,col));
										html.Replace(wxT("#LBIRTHNAME#"),grid->GetColLabelValue(col));
										break;
					case FIRSTNAME:		html.Replace(wxT("#FIRSTNAME#"),grid->GetCellValue(row,col));
										html.Replace(wxT("#LFIRSTNAME#"),grid->GetColLabelValue(col));
										break;
					case TITLE:			html.Replace(wxT("#TITLE#"),grid->GetCellValue(row,col));
										html.Replace(wxT("#LTITLE#"),grid->GetColLabelValue(col));			
										break;
					case BIRTHPLACE:	html.Replace(wxT("#BIRTHPLACE#"),grid->GetCellValue(row,col));
										html.Replace(wxT("#LBIRTHPLACE#"),grid->GetColLabelValue(col));	
										break;
					case BIRTHDATE:		html.Replace(wxT("#BIRTHDATE#"),grid->GetCellValue(row,col));
										html.Replace(wxT("#LBIRTHDATE#"),grid->GetColLabelValue(col));	
										break;
					case NATIONALITY:	html.Replace(wxT("#NATIONALITY#"),grid->GetCellValue(row,col));
										html.Replace(wxT("#LNATIONALITY#"),grid->GetColLabelValue(col));	
										break;
					case PASSPORT:		html.Replace(wxT("#PASSPORT#"),grid->GetCellValue(row,col));
										html.Replace(wxT("#LPASSPORT#"),grid->GetColLabelValue(col));	
										break;
					case EST_IN:		html.Replace(wxT("#EST_IN#"),grid->GetCellValue(row,col));
										html.Replace(wxT("#LEST_IN#"),grid->GetColLabelValue(col));	
										break;
					case EST_ON:		html.Replace(wxT("#EST_ON#"),grid->GetCellValue(row,col));
										html.Replace(wxT("#LEST_ON#"),grid->GetColLabelValue(col));	
										break;
					case ZIP:			html.Replace(wxT("#ZIP#"),grid->GetCellValue(row,col));
										html.Replace(wxT("#LZIP#"),grid->GetColLabelValue(col));	
										break;
					case COUNTRY:		html.Replace(wxT("#COUNTRY#"),grid->GetCellValue(row,col));
										html.Replace(wxT("#LCOUNTRY#"),grid->GetColLabelValue(col));	
										break;
					case TOWN:			html.Replace(wxT("#TOWN#"),grid->GetCellValue(row,col));
										html.Replace(wxT("#LTOWN#"),grid->GetColLabelValue(col));	
										break;
					case STREET:		html.Replace(wxT("#STREET#"),grid->GetCellValue(row,col));
										html.Replace(wxT("#LSTREET#"),grid->GetColLabelValue(col));	
										break;
				}
				break;
			case 1:
				switch(col)
				{
					case LWNAME:		html.Replace(wxT("#NAME#"),wake->GetCellValue(row,col));
										html.Replace(wxT("#LNAME#"),wake->GetColLabelValue(col));	
										break;
					case LWFIRSTNAME:	html.Replace(wxT("#FIRSTNAME#"),wake->GetCellValue(row,col));
										html.Replace(wxT("#LFIRSTNAME#"),wake->GetColLabelValue(col));	
										break;
					case WAKESTART1:	html.Replace(wxT("#WAKE1#"),wake->GetCellValue(row,col));
										html.Replace(wxT("#LWAKE1#"),wake->GetColLabelValue(col));	
										break;
					case WAKEEND1:		html.Replace(wxT("#WAKEE1#"),wake->GetCellValue(row,col));
										html.Replace(wxT("#LWAKEE1#"),wake->GetColLabelValue(col));	
										break;
					case WAKESTART2:	html.Replace(wxT("#WAKE2#"),wake->GetCellValue(row,col));
										html.Replace(wxT("#LWAKE2#"),wake->GetColLabelValue(col));	
										break;
					case WAKEEND2:		html.Replace(wxT("#WAKEE2#"),wake->GetCellValue(row,col));
										html.Replace(wxT("#LWAKEE2#"),wake->GetColLabelValue(col));	
										break;
					case WAKESTART3:	html.Replace(wxT("#WAKE3#"),wake->GetCellValue(row,col));
										html.Replace(wxT("#LWAKE3#"),wake->GetColLabelValue(col));	
										break;
					case WAKEEND3:		html.Replace(wxT("#WAKEE3#"),wake->GetCellValue(row,col));
										html.Replace(wxT("#LWAKEE3#"),wake->GetColLabelValue(col));	
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
		wxMessageBox(_("Sorry, no Layout installed"),_("Information"),wxOK);
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
		topODT.Replace(wxT("#LWATCH#"),dialog->m_gridGlobal->GetColLabelValue(4));
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
		middleData.Replace(wxT("#LWATCH#"),dialog->m_gridGlobal->GetColLabelValue(4));
	}

	topODT.Replace(wxT("#TYPE#"),dialog->boatType->GetValue());
	topODT.Replace(wxT("#BOATNAME#"),dialog->boatName->GetValue());
	topODT.Replace(wxT("#HOMEPORT#"),dialog->homeport->GetValue());
	topODT.Replace(wxT("#CALLSIGN#"),dialog->callsign->GetValue());
	topODT.Replace(wxT("#REGISTRATION#"),dialog->registration->GetValue());

	topODT.Replace(wxT("#LTYPE#"),dialog->m_staticText128->GetLabel());
	topODT.Replace(wxT("#LBOATNAME#"),dialog->bname->GetLabel());
	topODT.Replace(wxT("#LHOMEPORT#"),dialog->m_staticText114->GetLabel());
	topODT.Replace(wxT("#LCALLSIGN#"),dialog->m_staticText115->GetLabel());
	topODT.Replace(wxT("#LREGISTRATION#"),dialog->m_staticText118->GetLabel());
	topODT.Replace(wxT("#LCREWLIST#"),dialog->m_logbook->GetPageText(2));

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
			newMiddleODT = middleODT;
			for(int col = 0; col < colsMax; col++)
				newMiddleODT = replacePlaceholder(newMiddleODT,headerODT,0,row,col,0);
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
				newWakeODT = replacePlaceholder(newWakeODT,headerODT,1,row,col,0);
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
	int answer = wxMessageBox(wxString::Format(_("Delete Row Nr. %i ?"),row+1), _("Confirm"),
                              wxYES_NO | wxCANCEL, 0);
	if (answer == wxYES)
	{
		gridCrew->DeleteRows(row);
		gridWake->DeleteRows(row);

		crewListFile->Open();
		crewListFile->RemoveLine(row);
		crewListFile->Write();
		crewListFile->Close();
	}
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
		wxMessageBox(_("Sorry, Logbook has no lines"),_("Information"),wxOK);
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

	bool empty = false;
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
				empty = true;
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
