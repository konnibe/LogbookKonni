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

	gridCrew->AppendRows();
	gridWake->AppendRows();
	gridWake->SetCellValue(gridWake->GetNumberRows()-1,gridWake->GetNumberCols()-1,_T(" "));
}

void CrewList::changeCrew(wxGrid* grid, int row, int col, int offset)
{
	wxString result;

	if(col == 0 && offset == 0)
			gridWake->SetCellValue(row,0,gridCrew->GetCellValue(row,0));
	if(col == 2 && offset == 0)
			gridWake->SetCellValue(row,1,gridCrew->GetCellValue(row,2));

	if(gridWake->GetCellValue(row,gridWake->GetNumberCols()-1) == _T(""))
		gridWake->SetCellValue(row,gridWake->GetNumberCols()-1,_T(" "));
}

void CrewList::saveCSV(wxString path)
{
	wxString result;
//	bool timerStopped = true;

//	if(dialog->timer->IsRunning())
//	{
//		dialog->timer->Stop();
//		timerStopped = true;
//	}

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
//			int c = 0;
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


	saveData();

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
	}

	path = data_locn;
	wxTextFile *logFile = new wxTextFile(path);
	if(mode != 0)
		path.Replace(wxT("txt"),wxT("html"));
	else 
		path = savePath;

	wxTextFile *htmlFile = new wxTextFile(path);

	if(::wxFileExists(path))
		::wxRemoveFile(path);

	htmlFile->Create();
	logFile->Open();
	htmlFile->Open();
	htmlFile->Clear();

	int count = logFile->GetLineCount();

	wxString newMiddleHTML;
	wxString newWakeHTML;

	topHTML.Replace(wxT("#TYPE#"),dialog->boatType->GetValue());
	topHTML.Replace(wxT("#BOATNAME#"),dialog->boatName->GetValue());
	topHTML.Replace(wxT("#HOMEPORT#"),dialog->homeport->GetValue());
	topHTML.Replace(wxT("#CALLSIGN#"),dialog->callsign->GetValue());
	topHTML.Replace(wxT("#REGISTRATION#"),dialog->registration->GetValue());

	if(html.Contains(_T("<!--Repeat -->")))
	{
		htmlFile->AddLine(topHTML);
		for(int i = 0; i < count; i++)
		{
			wxString line = logFile->GetLine(i);
			newMiddleHTML = replacePlaceholder(middleHTML,line);
			htmlFile->AddLine(newMiddleHTML);
		}
		htmlFile->AddLine(bottomHTML);
		topHTML = _T("");
	}


	if(html.Contains(_T("<!--Repeat Wake -->")))
	{	
		htmlFile->AddLine(topHTML);
		for(int i = 0; i < count; i++)
		{
			wxString line = logFile->GetLine(i);
			newWakeHTML = replacePlaceholder(middleWakeHTML,line);
			htmlFile->AddLine(newWakeHTML);
		}
		htmlFile->AddLine(bottomWakeHTML);
	}
	
	htmlFile->Write();

	logFile->Close();
	htmlFile->Close();
}

wxString CrewList::replacePlaceholder(wxString html,wxString s)
{
		static wxString route;

		wxStringTokenizer tkz(s, _T("\t"),wxTOKEN_RET_EMPTY );
		int c = 0;
		while ( tkz.HasMoreTokens() )
		{
			wxString s;

			s = tkz.GetNextToken().RemoveLast();
			s.Replace(wxT("\\n"),wxT("<br>"));
			s.Replace(_T("&"),_T("&amp;"));
			s.Replace(_T("\""),_T("&quot;"));
			s.Replace(_T("<"),_T("&lt;"));
			s.Replace(_T(">"),_T("&gt;"));
			s.Replace(_T("'"),_T("&apos;"));

			switch(c)
				{
				case NAME:			html.Replace(wxT("#NAME#"),s);
									break;
				case BIRTHNAME:		html.Replace(wxT("#BIRTHNAME#"),s);		
									break;
				case FIRSTNAME:		html.Replace(wxT("#FIRSTNAME#"),s);
									break;
				case TITLE:			html.Replace(wxT("#TITLE#"),s);
									break;
				case BIRTHPLACE:	html.Replace(wxT("#BIRTHPLACE#"),s);
									break;
				case BIRTHDATE:		html.Replace(wxT("#BIRTHDATE#"),s);
									break;
				case NATIONALITY:	html.Replace(wxT("#NATIONALITY#"),s);
									break;
				case PASSPORT:		html.Replace(wxT("#PASSPORT#"),s);
									break;
				case EST_IN:		html.Replace(wxT("#EST_IN#"),s);
									break;
				case EST_ON:		html.Replace(wxT("#EST_ON#"),s);
									break;
				case ZIP:			html.Replace(wxT("#ZIP#"),s);
									break;
				case COUNTRY:		html.Replace(wxT("#COUNTRY#"),s);
									break;
				case TOWN:			html.Replace(wxT("#TOWN#"),s);
									break;
				case STREET:		html.Replace(wxT("#STREET#"),s);
									break;
				case WAKESTART1:		html.Replace(wxT("#WAKE1#"),s);
									break;
				case WAKEEND1:		html.Replace(wxT("#WAKEE1#"),s);
									break;
				case WAKESTART2:		html.Replace(wxT("#WAKE2#"),s);
									break;
				case WAKEEND2:		html.Replace(wxT("#WAKEE2#"),s);
									break;
				case WAKESTART3:		html.Replace(wxT("#WAKE3#"),s);
									break;
				case WAKEEND3:		html.Replace(wxT("#WAKEE3#"),s);
									break;
				}
				c++;
	}
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

	}

	topODT.Replace(wxT("#TYPE#"),dialog->boatType->GetValue());
	topODT.Replace(wxT("#BOATNAME#"),dialog->boatName->GetValue());
	topODT.Replace(wxT("#HOMEPORT#"),dialog->homeport->GetValue());
	topODT.Replace(wxT("#CALLSIGN#"),dialog->callsign->GetValue());
	topODT.Replace(wxT("#REGISTRATION#"),dialog->registration->GetValue());

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

	int count = logFile->GetLineCount();

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
//wxMessageBox(topODT,_("topODT"));
	if(odt.Contains(seperatorTop))
	{
		for(int i = 0; i < count; i++)
		{
			wxString line = logFile->GetLine(i);
			newMiddleODT = replacePlaceholder(middleODT,line);
			odtFile << newMiddleODT;
			//wxMessageBox(newMiddleODT,_("middleODT"));
		}
	}

	if(!middleData.IsEmpty())
		odtFile << middleData;
//wxMessageBox(middleData,_("middleData"));

	if(odt.Contains(seperatorWakeTop))
	{	
		for(int i = 0; i < count; i++)
		{
			wxString line = logFile->GetLine(i);
			newWakeODT = replacePlaceholder(middleWakeODT,line);
			odtFile << newWakeODT;
			//wxMessageBox(newWakeODT,_("middleWakeODT"));
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
//	bool timerStopped = true;

//	if(dialog->timer->IsRunning())
//	{
//		dialog->timer->Stop();
//		timerStopped = true;
//	}

	saveData();

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

//	if(timerStopped)
//		dialog->timer->Start();
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
//	bool timerStopped = false;

	saveData();

	wxFileInputStream input( data_locn );
	wxTextInputStream* stream = new wxTextInputStream (input);

//	if(parent->timer->IsRunning())
//	{
//		parent->timer->Stop();
//		timerStopped = true;
//	}

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
//	zip.PutNextEntry(wxT("settings.xml"));
//	txt << settings;

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

//	if(timerStopped)
//		parent->timer->Start();
}
