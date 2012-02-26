//#pragma once
#include "LogbookHTML.h"
#include "LogbookDialog.h"
#include "Logbook.h"
#include "logbook_pi.h"
#include "MessageBoxOSX.h"
#ifndef WX_PRECOMP
      #include <wx/wx.h>
#endif

#include <wx/mimetype.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/grid.h>
#include <wx/wfstream.h> 
#include <wx/txtstrm.h> 
#include <wx/zipstrm.h> 

#include <memory>
using namespace std;

LogbookHTML::LogbookHTML(Logbook *l, LogbookDialog *d, wxString data, wxString layout)
{
	parent = d;
	logbook = l;
	data_locn = data;
	layout_locn = layout;
}

LogbookHTML::~LogbookHTML(void)
{

}

void LogbookHTML::viewHTML(wxString path, wxString layout, bool mode)
{
	setSelection();

	wxString file = toHTML(path, layout, mode);
	if(file != wxT(""))
		parent->startBrowser(file);
}

void LogbookHTML::setSelection()
{
	int row = parent->selGridRow;
	int z = 0 , r = 1, topRow, bottomRow, first, last;

	first = last = 0;
	
	if(parent->logGrids[0]->GetRows() == 0) return;

	if(parent->logGrids[parent->m_notebook8->GetSelection()]->IsSelection())
	{
		wxGridCellCoordsArray top = 
			parent->logGrids[parent->m_notebook8->GetSelection()]->GetSelectionBlockTopLeft();
		wxGridCellCoordsArray bottom = 
			parent->logGrids[parent->m_notebook8->GetSelection()]->GetSelectionBlockBottomRight();
		if(top.Count() != 0)
		{
			topRow = top[0].GetRow();
			bottomRow = bottom[0].GetRow();
			r = bottomRow-topRow+1;
		}
	}

	z = parent->m_gridGlobal->GetNumberRows();
	if(r == z)
	{
		parent->m_gridGlobal->SelectAll();
		parent->m_gridWeather->SelectAll();
		parent->m_gridMotorSails->SelectAll();
	}
	else
	{
		parent->m_gridGlobal->ClearSelection();	
		parent->m_gridWeather->ClearSelection();	
		parent->m_gridMotorSails->ClearSelection();	
	}

	wxString selRoute = parent->m_gridGlobal->GetCellValue(row,0);
	
	while(row > -1 && parent->m_gridGlobal->GetCellValue(row,0) == selRoute)
		first = row--;
	row = parent->selGridRow;
	while(row < parent->m_gridGlobal->GetNumberRows() && parent->m_gridGlobal->GetCellValue(row,0) == selRoute)
		last = row++;

	for(row = first; row <= last; row++)
	{
		parent->m_gridGlobal->SelectRow(row,true);
		parent->m_gridWeather->SelectRow(row,true);
		parent->m_gridMotorSails->SelectRow(row,true);
	}
}

wxString LogbookHTML::toHTML(wxString path, wxString layout, bool mode)
{
	wxArrayInt arrayRows;
	int count = 0, selCount = 0;
	bool selection = false;

	selCount = parent->m_gridGlobal->GetSelectedRows().Count() ;

	if(selCount > 0)
	{
		selection = true;
		arrayRows = parent->m_gridGlobal->GetSelectedRows();
	}

	if(layout == _T(""))
	{
#ifdef __WXOSX__
        MessageBoxOSX(NULL,_("Sorry, no Layout installed"),_T("Information"),wxID_OK);
#else
		wxMessageBox(_("Sorry, no Layout installed"),_("Information"),wxOK);
#endif
		return _T("");
	}

	wxString html = readLayoutFile(layout);

	wxString topHTML;
	wxString bottomHTML;
	wxString headerHTML;
	wxString middleHTML;

	wxString seperatorTop = _T("<!--Repeat -->");
	wxString seperatorBottom = _T("<!--Repeat End -->");
	wxString seperatorHeaderTop = _T("<!--Header -->");
	wxString seperatorHeaderBottom = _T("<!--Header end -->");

	int indexTop = html.Find(seperatorTop);
	if(!checkLayoutError(indexTop,seperatorTop,layout)) return _T("");
	indexTop += seperatorTop.Len();
	int indexBottom = html.Find(seperatorBottom);
	if(!checkLayoutError(indexBottom,seperatorBottom,layout)) return _T("");
	indexBottom += seperatorBottom.Len();
	int indexHeaderTop = html.Find(seperatorHeaderTop);
	if(!checkLayoutError(indexHeaderTop,seperatorHeaderTop,layout)) return _T("");
	indexHeaderTop += seperatorHeaderTop.Len();
	int indexHeaderBottom = html.Find(seperatorHeaderBottom);
	if(!checkLayoutError(indexHeaderBottom,seperatorHeaderBottom,layout)) return _T("");
	indexHeaderBottom += seperatorHeaderBottom.Len();

	topHTML = html.substr(0,indexHeaderTop);
	bottomHTML = html.substr(indexBottom,html.Len()-1);
	middleHTML = html.substr(indexTop,indexBottom-indexTop);
	headerHTML = html.substr(indexHeaderTop,indexHeaderBottom-indexHeaderTop);

	wxString filename = this->fileName;

	if(mode == false)
	{
		filename.Replace(wxT("txt"),wxT("html"));
	}
	else
		filename = path;

	if(::wxFileExists(filename))
		::wxRemoveFile(filename);
	
	wxFileOutputStream output( filename );
	wxTextOutputStream htmlFile(output);

	wxString newMiddleHTML;

	topHTML.Replace(wxT("#LLOGBOOK#"),parent->m_logbook->GetPageText(0));
	topHTML.Replace(wxT("#LFROM#"),_("from"));
	topHTML.Replace(wxT("#LTO#"),_("to"));
	topHTML.Replace(wxT("#SDATE#"),parent->m_gridGlobal->GetCellValue(0,1));
	topHTML.Replace(wxT("#EDATE#"),parent->m_gridGlobal->GetCellValue(
		parent->m_gridGlobal->GetNumberRows()-1,1));
	topHTML.Replace(wxT("#TYPE#"),parent->boatType->GetValue());
	topHTML.Replace(wxT("#BOATNAME#"),parent->boatName->GetValue());
	topHTML.Replace(wxT("#HOMEPORT#"),parent->homeport->GetValue());
	topHTML.Replace(wxT("#CALLSIGN#"),parent->callsign->GetValue());
	topHTML.Replace(wxT("#REGISTRATION#"),parent->registration->GetValue());
	topHTML.Replace(wxT("#LOCATION#"),layout_locn + layout + _T(".html"));
	htmlFile << topHTML;

	int rowsMax = parent->m_gridGlobal->GetNumberRows();
	for(int row = 0; row < rowsMax; row++)
	{
		count++;
		if(selection && arrayRows[0]+1 > count) continue;
		if(selection && arrayRows[selCount-1]+1 < count) break;

		newMiddleHTML = middleHTML;
		for(int grid = 0; grid < 3; grid++)
		{
			wxGrid* g = parent->logGrids[grid];
			for(int col = 0; col < g->GetNumberCols(); col++)
				newMiddleHTML = replacePlaceholder(newMiddleHTML,headerHTML,grid,row,col,0);
		}
		htmlFile << newMiddleHTML;
	}
	htmlFile << bottomHTML;

	if(count <= 0)
	{
#ifdef __WXOSX__
        MessageBoxOSX(NULL,_("Sorry, Logbook has no lines"),_T("Information"),wxID_OK);
#else
		wxMessageBox(_("Sorry, Logbook has no lines"),_("Information"),wxOK);
#endif
		return _T("");
	}

	output.Close();

	return filename;
}

wxString LogbookHTML::replacePlaceholder(wxString html,wxString htmlHeader,int grid, int row, int col, bool mode)
{
		static wxString route;
		wxString s;
		wxGrid* g = parent->logGrids[grid];

		if(row == 0 && col == 0 && grid == 0)  
			route = _T(""); 

			switch(grid)
			{
			case 0:
					switch(col)
					{
						case ROUTE:	if(route != replaceNewLine(g->GetCellValue(row,col),mode))
									{
										htmlHeader.Replace(wxT("#ROUTE#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Prepend(htmlHeader);
									}
									html.Replace(wxT("#LROUTE#"),g->GetColLabelValue(col));
									route = replaceNewLine(g->GetCellValue(row,col),mode);
								break;
						case RDATE:		html.Replace(wxT("#DATE#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LDATE#"),g->GetColLabelValue(col));
										html.Replace(wxT("#NO.#"),wxString::Format(_T("%i"),row+1));
								break;
						case RTIME:		html.Replace(wxT("#TIME#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LTIME#"),g->GetColLabelValue(col));
								break;
						case SIGN:		html.Replace(wxT("#SIGN#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LSIGN#"),g->GetColLabelValue(col));
								break;
						case WAKE:		html.Replace(wxT("#WAKE#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LWAKE#"),g->GetColLabelValue(col));
								break;
						case DISTANCE:	html.Replace(wxT("#DISTANCE#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LDISTANCE#"),g->GetColLabelValue(col));
								break;
						case DTOTAL:	html.Replace(wxT("#DTOTAL#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LDTOTAL#"),g->GetColLabelValue(col));
								break;
						case POSITION:	html.Replace(wxT("#POSITION#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LPOSITION#"),g->GetColLabelValue(col));
								break;
						case COG:		html.Replace(wxT("#COG#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LCOG#"),g->GetColLabelValue(col));
								break;
						case COW:		html.Replace(wxT("#COW#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LCOW#"),g->GetColLabelValue(col));
								break;
						case SOG:		html.Replace(wxT("#SOG#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LSOG#"),g->GetColLabelValue(col));
								break;
						case SOW:		html.Replace(wxT("#SOW#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LSOW#"),g->GetColLabelValue(col));
								break;
						case DEPTH:		html.Replace(wxT("#DEPTH#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LDEPTH#"),g->GetColLabelValue(col));
								break;
						case REMARKS:	html.Replace(wxT("#REMARKS#"),replaceNewLine(replaceNewLine(g->GetCellValue(row,col),mode),mode));
										html.Replace(wxT("#LREMARKS#"),g->GetColLabelValue(col));
								break;
					}
					break;
			case 1:
					switch(col)
					{
						case BARO:		html.Replace(wxT("#BARO#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LBARO#"),g->GetColLabelValue(col));
								break;
						case HYDRO:		html.Replace(wxT("#HYDRO#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LHYDRO#"),g->GetColLabelValue(col));
								break;
						case AIRTE:		html.Replace(wxT("#AIRTE#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LAIRTE#"),g->GetColLabelValue(col));
								break;
						case WATERTE:	html.Replace(wxT("#WTE#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LWTE#"),g->GetColLabelValue(col));
								break;
						case WIND:		html.Replace(wxT("#WIND#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LWIND#"),g->GetColLabelValue(col));
								break;
						case WSPD:		html.Replace(wxT("#WSPD#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LWSPD#"),g->GetColLabelValue(col));
								break;
						case CURRENT:	html.Replace(wxT("#CUR#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LCUR#"),g->GetColLabelValue(col));
								break;
						case CSPD:		html.Replace(wxT("#CSPD#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LCSPD#"),g->GetColLabelValue(col));
								break;
						case WAVE:		html.Replace(wxT("#WAVE#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LWAVE#"),g->GetColLabelValue(col));	
								break;
						case SWELL:		html.Replace(wxT("#SWELL#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LSWELL#"),g->GetColLabelValue(col));
								break;
						case WEATHER:	html.Replace(wxT("#WEATHER#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LWEATHER#"),g->GetColLabelValue(col));
								break;
						case CLOUDS:	html.Replace(wxT("#CLOUDS#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LCLOUDS#"),g->GetColLabelValue(col));
										{
										wxString cl = replaceNewLine(g->GetCellValue(row,col),mode).Lower();
										if(cl.Contains(_T(" ")))
											cl = cl.SubString(0,cl.find_first_of(' ')-1);
										if(html.Contains(wxT("#PCLOUDS%23")))
											html.Replace(wxT("../../../clouds/#PCLOUDS%23"),_T("../clouds/")+cl);
										else
											html.Replace(wxT("#PCLOUDS#"),cl);
										}
								break;
						case VISIBILITY:html.Replace(wxT("#VISIBILITY#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LVISIBILITY#"),g->GetColLabelValue(col));
								break;
					}
					break;
			case 2:
					switch(col)
					{
						case MOTOR:		html.Replace(wxT("#MOTOR#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LMOTOR#"),g->GetColLabelValue(col));
								break;
						case MOTORT:	html.Replace(wxT("#MOTORT#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LMOTORT#"),g->GetColLabelValue(col));
								break;
						case FUEL:		html.Replace(wxT("#FUEL#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LFUEL#"),g->GetColLabelValue(col));
								break;
						case FUELT:		html.Replace(wxT("#FUELT#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LFUELT#"),g->GetColLabelValue(col));
								break;
						case SAILS:		html.Replace(wxT("#SAILS#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LSAILS#"),g->GetColLabelValue(col));
								break;
						case REEF:		html.Replace(wxT("#REEF#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LREEF#"),g->GetColLabelValue(col));
								break;
						case WATER:		html.Replace(wxT("#WATER#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LWATER#"),g->GetColLabelValue(col));
								break;
						case WATERT:	html.Replace(wxT("#WATERT#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LWATERT#"),g->GetColLabelValue(col));
								break;
						case MREMARKS:	html.Replace(wxT("#MREMARKS#"),replaceNewLine(g->GetCellValue(row,col),mode));
										html.Replace(wxT("#LMREMARKS#"),g->GetColLabelValue(col));
								break;
					}
					break;
			}

	if(mode == 0)
		return html;
	else 
	{
		wxString str(html, wxConvUTF8);
		return str;
	}
}

wxString LogbookHTML::replaceNewLine(wxString s, bool mode)
{
	if(mode == 0) // HTML
		s.Replace(wxT("\n"),wxT("<br>"));
	else // ODT
		s.Replace(wxT("\n"),wxT("<text:line-break/>"));

	return s;
}

bool LogbookHTML::checkLayoutError(int result, wxString html, wxString layout)
{
	if(result == wxNOT_FOUND)
	{
#ifdef __WXOSX__
        MessageBoxOSX(NULL,html+_("\nnot found in layoutfile ")+layout+_("!\n\nDid you forget to add this line in your layout ?"),_("Information"),wxID_OK);
        return false;
#else
		wxMessageBox(html+_("\nnot found in layoutfile ")+layout+_("!\n\nDid you forget to add this line in your layout ?"),_("Information"));
		return false;
#endif
	}
	return true;
}

void LogbookHTML::setFileName(wxString s, wxString l)
{
	fileName = s;
	layout_locn = l;
}

wxString LogbookHTML::readLayoutFile(wxString layout)
{
	wxString html;

	wxString filename = layout_locn + layout + _T(".html");
	wxTextFile *layoutfile = new wxTextFile(filename);

	if(wxFileExists(filename))
	{
		layoutfile->Open();

		for(unsigned row = 0; row < layoutfile->GetLineCount(); row++)
		{
			html += layoutfile->GetLine(row)+wxT("\n");
		}
	}
	layoutfile->Close();
	return html;
}

wxString LogbookHTML::readLayoutFileODT(wxString layout)
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

void LogbookHTML::viewODT(wxString path, wxString layout, bool mode)
{
	setSelection();

	wxString file = toODT(path, layout, mode);
	if(file != wxT(""))
		parent->startApplication(file,_T(".odt"));
}

wxString LogbookHTML::toODT(wxString path,wxString layout, bool mode)
{
	wxArrayInt arrayRows;
	int count = 0, selCount = 0;
	bool selection = false;

	selCount = parent->m_gridGlobal->GetSelectedRows().Count() ;

	if(selCount > 0)
	{
		selection = true;
		arrayRows = parent->m_gridGlobal->GetSelectedRows();
	}

	if(layout == _T(""))
	{
#ifdef __WXOSX__
        MessageBoxOSX(NULL,_("Sorry, no Layout installed"),_T("Information"),wxID_OK);
#else
		wxMessageBox(_("Sorry, no Layout installed"),_("Information"),wxOK);
#endif
		return _T("");
	}

	wxString odt = readLayoutFileODT(layout);
	if(!odt.Contains(_T("[[")) && !odt.Contains(_T("{{")))
	{
#ifdef __WXOSX__
        MessageBoxOSX(NULL,_("Have You forgotten to enclose the Header with [[ and ]]\n or Data with {{ and }} ?"),_T("Information"),wxID_OK);
#else
		wxMessageBox(_("Have You forgotten to enclose the Header with [[ and ]]\n or Data with {{ and }} ?"));
#endif
		return _T("");
	}

	wxString topODT;
	wxString bottomODT;
	wxString headerODT;
	wxString middleODT;

	wxString seperatorTop = _T("{{");
	wxString seperatorBottom = _T("}}");
	wxString seperatorHeaderTop = _T("[[");
	wxString seperatorHeaderBottom = _T("]]");
	wxString textbegin = _T("<text:p");
	wxString textend = _T("</text:p");

	odt.Replace(wxT("#LLOGBOOK#"),parent->m_logbook->GetPageText(0));
	odt.Replace(wxT("#LFROM#"),_("from"));
	odt.Replace(wxT("#LTO#"),_("to"));
	odt.Replace(wxT("#SDATE#"),parent->m_gridGlobal->GetCellValue(0,1));
	odt.Replace(wxT("#EDATE#"),parent->m_gridGlobal->GetCellValue(
		parent->m_gridGlobal->GetNumberRows()-1,1));
	odt.Replace(wxT("#TYPE#"),parent->boatType->GetValue());
	odt.Replace(wxT("#BOATNAME#"),parent->boatName->GetValue());
	odt.Replace(wxT("#HOMEPORT#"),parent->homeport->GetValue());
	odt.Replace(wxT("#CALLSIGN#"),parent->callsign->GetValue());
	odt.Replace(wxT("#REGISTRATION#"),parent->registration->GetValue());


	int indexTopODT = odt.Find(seperatorHeaderTop);
	int top = indexTopODT;
	topODT = odt.substr(0,indexTopODT);
	indexTopODT = topODT.find_last_of('<');
	topODT = topODT.substr(0,indexTopODT);
	odt = odt.substr(top);

	int headerStart = odt.find_first_of('>')+1;
	odt = odt.substr(headerStart);

	int indexBottomODT = odt.Find(seperatorHeaderBottom);
	headerODT = odt.substr(0,indexBottomODT);
	odt = odt.substr(indexBottomODT);
	int headerEnd = odt.find_first_of('>')+1;
	odt = odt.substr(headerEnd);
	indexBottomODT = headerODT.find_first_of('<');
	headerODT = headerODT.substr(indexBottomODT);
	headerODT = headerODT.substr(0,headerODT.find_last_of('<'));

	indexBottomODT = odt.Find(seperatorTop);
	middleODT = odt.substr(indexBottomODT);
	odt = odt.substr(indexBottomODT);

	int middleStart = middleODT.find_first_of('>')+1;
	middleODT = middleODT.substr(middleStart);
	odt = odt.substr(odt.find_first_of('>')+1);

	indexBottomODT = odt.Find(seperatorBottom);
	middleODT = odt.substr(0,indexBottomODT);
	odt = odt.substr(indexBottomODT);

	int middleEnd = odt.find_first_of('>')+1;
	odt = odt.substr(middleEnd);
	middleODT = middleODT.substr(0,middleODT.find_last_of('<'));

	wxString filename = this->fileName;

	if(mode == false)
	{
		filename.Replace(wxT("txt"),wxT("odt"));
	}
	else
		filename = path;

	if(::wxFileExists(filename))
		::wxRemoveFile(filename);
	
    auto_ptr<wxFFileInputStream> in(new wxFFileInputStream(layout_locn + layout + _T(".odt")));
    wxTempFileOutputStream out(filename);

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

	wxString newMiddleODT;
	int rowsMax = parent->m_gridGlobal->GetNumberRows();
	for(int row = 0; row < rowsMax; row++)
	{
		count++;
		if(selection && arrayRows[0]+1 > count) continue;
		if(selection && arrayRows[selCount-1]+1 < count) break;

		newMiddleODT = middleODT;
		for(int grid = 0; grid < 3; grid++)
		{
			wxGrid* g = parent->logGrids[grid];
			for(int col = 0; col < g->GetNumberCols(); col++)
				newMiddleODT = replacePlaceholder(newMiddleODT,headerODT,grid,row,col,1);
		}
		odtFile << newMiddleODT;
	}

	odtFile << odt;

    inzip.Eof() && outzip.Close() && out.Commit();
//	replacePlaceholder(middleODT,headerODT,0,0,0,0); // Reset Route if starts with same Route
	return filename;
	
}

void LogbookHTML::toCSV(wxString path)
{
	wxString line;
	wxString s, temp;

	logbook->update();

	wxFileInputStream input( this->fileName );
	wxTextInputStream* stream = new wxTextInputStream (input);

	if(input.GetSize() == 0)
	{
#ifdef __WXOSX__
        MessageBoxOSX(NULL,_("Sorry, Your Logbook has no lines"),_T("Information"),wxID_OK);
#else
		wxMessageBox(_("Sorry, Your Logbook has no lines"),_("Information"),wxOK);
#endif
		return;
	}

	if(::wxFileExists(path))
		::wxRemoveFile(path);
	
	wxFileOutputStream output( path );
	wxTextOutputStream csvFile(output);

	for(int n = 0; n < parent->numPages; n++)
	{
		for(int i = 0; i < parent->logGrids[n]->GetNumberCols(); i++)
			csvFile << parent->logGrids[n]->GetColLabelValue(i)+_T(",");
	}
	csvFile << _T("\n");

	while(wxString line = stream->ReadLine())
	{
		if(input.Eof()) break;

		wxStringTokenizer tkz(line, _T("\t"));

		while ( tkz.HasMoreTokens() )
		{ 
			temp = tkz.GetNextToken();
			temp.RemoveLast();
			temp.Replace(_T("\""),_T("\"\""));
			temp.Replace(_T("\\n"),_T("\n"));
			temp.Prepend(_T("\""));
			temp.Append(_T("\","));
			s += temp;
		}
		s.RemoveLast();
		csvFile << s+_T("\n");
		s = _T("");
	}
	output.Close();
}

void LogbookHTML::toXML(wxString path)
{
	wxString s = _T("");
	wxString line;
	wxString temp;
	int count = 0;

	logbook->update();

	wxFileInputStream input( this->fileName );
	wxTextInputStream* stream = new wxTextInputStream (input);
	if(input.GetSize() == 0)
	{
#ifdef __WXOSX__
        MessageBoxOSX(NULL,_("Sorry, Your Logbook has no lines"),_T("Information"),wxID_OK);
#else
		wxMessageBox(_("Sorry, Your Logbook has no lines"),_("Information"),wxOK);
#endif
		return;
	}

	wxFileOutputStream output( path );
	wxTextOutputStream xmlFile(output);

	if(::wxFileExists(path))
		::wxRemoveFile(path);

	xmlFile << parent->xmlHead;

	s = wxString::Format(_T("<Row>"));//,parent->m_gridGlobal->GetRowHeight(count++));
	for(int n = 0; n < parent->numPages; n++)
	{
		for(int i = 0; i < parent->logGrids[n]->GetNumberCols(); i++)
		{
			s += _T("<Cell>\n");
			s += _T("<Data ss:Type=\"String\">#DATA#</Data>\n");
			s.Replace(_T("#DATA#"),parent->logGrids[n]->GetColLabelValue(i));
			s += _T("</Cell>");
		}
	}
	s += _T("</Row>>");
	xmlFile << s;

	while(wxString line = stream->ReadLine())
	{
		if(input.Eof()) break;
		wxStringTokenizer tkz(line, _T("\t"));
		s = wxString::Format(_T("<Row ss:Height=\"%u\">"),parent->m_gridGlobal->GetRowHeight(count++));

		while ( tkz.HasMoreTokens() )
		{ 
			s += _T("<Cell>\n");
			s += _T("<Data ss:Type=\"String\">#DATA#</Data>\n");
			temp = tkz.GetNextToken();
			temp.Replace(_T("&"),_T("&amp;"));
			temp.Replace(_T("\\n"),_T("&#xA;"));
			temp.Replace(_T("\""),_T("&quot;"));
			temp.Replace(_T("<"),_T("&lt;"));
			temp.Replace(_T(">"),_T("&gt;"));
			temp.Replace(_T("'"),_T("&apos;"));
			s.Replace(_T("#DATA#"),temp);
			s += _T("</Cell>");
		}
		s += _T("</Row>>");
		xmlFile << s;
	}

	xmlFile << parent->xmlEnd;
	output.Close();

}

void LogbookHTML::toODS(wxString path)
{
	wxString s = _T("");
	wxString line;
	wxString temp;

	wxFileInputStream input( this->fileName );
	wxTextInputStream* stream = new wxTextInputStream (input);

	logbook->update();

	wxFFileOutputStream out(path);
	wxZipOutputStream zip(out);
	wxTextOutputStream txt(zip);
	wxString sep(wxFileName::GetPathSeparator());

	zip.PutNextEntry(wxT("content.xml"));
	txt << parent->content;

//	bool empty = false;
	long emptyCol = 0;

	txt << _T("<table:table-row table:style-name=\"ro2\">");

	for(int n = 0; n < parent->numPages; n++)
	{
		for(int i = 0; i < parent->logGrids[n]->GetNumberCols(); i++)
		{
			txt << _T("<table:table-cell office:value-type=\"string\">");
			txt << _T("<text:p>");
			txt << parent->logGrids[n]->GetColLabelValue(i);
			txt << _T("</text:p>");
			txt << _T("</table:table-cell>");
		}
	}
	txt << _T("</table:table-row>");

	while(wxString line = stream->ReadLine())
	{
		if(input.Eof()) break;
		txt << _T("<table:table-row table:style-name=\"ro2\">");
		wxStringTokenizer tkz(line, _T("\t"));

		while ( tkz.HasMoreTokens() )
		{ 
			wxString s = parent->restoreDangerChar(tkz.GetNextToken());
			s.RemoveLast();

			if(s == _T(""))
			{
				txt <<  _T("<table:table-cell />");
				//empty = true;
				emptyCol++;
				continue;
			}

			txt << _T("<table:table-cell office:value-type=\"string\">");

			wxStringTokenizer str(s, _T("\t"));
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
		txt << _T("</table:table-row>");

	}
	txt << parent->contentEnd;

	zip.PutNextEntry(wxT("mimetype"));
	txt << wxT("application/vnd.oasis.opendocument.spreadsheet");

	zip.PutNextEntry(wxT("styles.xml"));
	txt << parent->styles;

	zip.PutNextEntry(wxT("meta.xml"));
	txt << parent->meta;

	zip.PutNextEntry(wxT("META-INF") + sep + wxT("manifest.xml"));
	txt << parent->manifest;

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

void LogbookHTML::backup(wxString path)
{
	wxCopyFile(data_locn+_T("logbook.txt"),path);
}
