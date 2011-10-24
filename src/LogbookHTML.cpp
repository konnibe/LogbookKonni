//#pragma once
#include "LogbookHTML.h"
#include "LogbookDialog.h"
#include "Logbook.h"
#include "logbook_pi.h"

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
		wxMessageBox(_("Sorry, no Layout installed"),_("Information"),wxOK);
		return _T("");
	}

	logbook->update();

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

	wxFileInputStream input( filename );
	wxTextInputStream* stream = new wxTextInputStream (input);

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

	topHTML.Replace(wxT("#TYPE#"),parent->boatType->GetValue());
	topHTML.Replace(wxT("#BOATNAME#"),parent->boatName->GetValue());
	topHTML.Replace(wxT("#HOMEPORT#"),parent->homeport->GetValue());
	topHTML.Replace(wxT("#CALLSIGN#"),parent->callsign->GetValue());
	topHTML.Replace(wxT("#REGISTRATION#"),parent->registration->GetValue());
	topHTML.Replace(wxT("#LOCATION#"),layout_locn + layout + _T(".html"));
	htmlFile << topHTML;

	while(wxString line = stream->ReadLine())
	{
		if(input.Eof()) break;
		count++;
		if(selection && arrayRows[0]+1 > count) continue;
		if(selection && arrayRows[selCount-1]+1 < count) break;
		newMiddleHTML = replacePlaceholder(middleHTML,headerHTML,line,count,0);
		htmlFile << newMiddleHTML;
	}
	htmlFile << bottomHTML;

	if(count <= 0)
	{
		wxMessageBox(_("Sorry, Logbook has no lines"),_("Information"),wxOK);
		return _T("");
	}

	output.Close();

	replacePlaceholder(middleHTML,headerHTML,_T(""),0,0); // Reset Route if starts with same Route

	return filename;
}

bool LogbookHTML::checkLayoutError(int result, wxString html, wxString layout)
{
	if(result == wxNOT_FOUND)
	{
		wxMessageBox(html+_("\nnot found in layoutfile ")+layout+_("!\n\nDid you forget to add this line in your layout ?"),_("Information"));
		return false;
	}
	return true;
}

void LogbookHTML::setFileName(wxString s, wxString l)
{
	fileName = s;
	layout_locn = l;
}

wxString LogbookHTML::replacePlaceholder(wxString html,wxString htmlHeader,wxString st,int nr, bool mode)
{
		static wxString route;
		wxString s;

		if(st == _T("")) { route = _T(""); return _T(""); }

		wxStringTokenizer tkz(st, _T("\t"));
		int c = 0;
		while ( tkz.HasMoreTokens() )
		{
			s = tkz.GetNextToken();
			s.RemoveLast();
			if(mode == 0) // HTML
				s.Replace(wxT("\\n"),wxT("<br>"));
			else // ODT
				s.Replace(wxT("\\n"),wxT("<text:line-break/>"));

				switch(c)
				{
				case ROUTE:		if(route != s)
								{
									htmlHeader.Replace(wxT("#ROUTE#"),s);
									html.Prepend(htmlHeader);
								}
								route = s;
								break;
				case RDATE:		html.Replace(wxT("#DATE#"),s);
								html.Replace(wxT("#NO.#"),wxString::Format(_T("%i"),nr));
								break;
				case RTIME:		html.Replace(wxT("#TIME#"),s);
								break;
				case SIGN:		html.Replace(wxT("#SIGN#"),s);
								break;
				case WAKE:		html.Replace(wxT("#WAKE#"),s);
								break;
				case DISTANCE:	html.Replace(wxT("#DISTANCE#"),s);
								break;
				case DTOTAL:	html.Replace(wxT("#DTOTAL#"),s);
								break;
				case POSITION:	html.Replace(wxT("#POSITION#"),s);
								break;
				case COG:		html.Replace(wxT("#COG#"),s);
								break;
				case COW:		html.Replace(wxT("#COW#"),s);
								break;
				case SOG:		html.Replace(wxT("#SOG#"),s);
								break;
				case SOW:		html.Replace(wxT("#SOW#"),s);
								break;
				case DEPTH:		html.Replace(wxT("#DEPTH#"),s);
								break;
				case REMARKS:	html.Replace(wxT("#REMARKS#"),s);
								break;
				case BARO:		html.Replace(wxT("#BARO#"),s);
								break;
				case WIND:		html.Replace(wxT("#WIND#"),s);
								break;
				case WSPD:		html.Replace(wxT("#WSPD#"),s);
								break;
				case CURRENT:	html.Replace(wxT("#CUR#"),s);
								break;
				case CSPD:		html.Replace(wxT("#CSPD#"),s);
								break;
				case WAVE:		html.Replace(wxT("#WAVE#"),s);
								break;
				case SWELL:		html.Replace(wxT("#SWELL#"),s);
								break;
				case WEATHER:	html.Replace(wxT("#WEATHER#"),s);
								break;
				case CLOUDS:	html.Replace(wxT("#CLOUDS#"),s);
								break;
				case VISIBILITY:html.Replace(wxT("#VISIBILITY#"),s);
								break;
				case MOTOR:		html.Replace(wxT("#MOTOR#"),s);
								break;
				case MOTORT:	html.Replace(wxT("#MOTORT#"),s);
								break;
				case FUEL:		html.Replace(wxT("#FUEL#"),s);
								break;
				case FUELT:		html.Replace(wxT("#FUELT#"),s);
								break;
				case SAILS:		html.Replace(wxT("#SAILS#"),s);
								break;
				case REEF:		html.Replace(wxT("#REEF#"),s);
								break;
				case WATER:		html.Replace(wxT("#WATER#"),s);
								break;
				case WATERT:	html.Replace(wxT("#WATERT#"),s);
								break;
				case MREMARKS:	html.Replace(wxT("#MREMARKS#"),s);
								break;
				}
				c++;
	}
	//html.Replace(wxT("°"),wxT("&#176;"));
	//html.Replace(wxT("°"),wxT("Â&#176;"));
	if(mode == 0)
	{
//		wxString str(html, wxConvUTF8);
		return html;
	}
	else 
	{
		wxString str(html, wxConvUTF8);
		return str;
	}
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
		wxMessageBox(_("Sorry, no Layout installed"),_("Information"),wxOK);
		return _T("");
	}

	logbook->update();

	wxString odt = readLayoutFileODT(layout);
	if(!odt.Contains(_T("[[")) && !odt.Contains(_T("{{")))
	{
		wxMessageBox(_("Have You forgotten to enclose the Header with [[ and ]]\n or Data with {{ and }} ?"));
		return _T("");
	}
//wxMessageBox(odt);
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

	odt.Replace(wxT("#TYPE#"),parent->boatType->GetValue());
	odt.Replace(wxT("#BOATNAME#"),parent->boatName->GetValue());
	odt.Replace(wxT("#HOMEPORT#"),parent->homeport->GetValue());
	odt.Replace(wxT("#CALLSIGN#"),parent->callsign->GetValue());
	odt.Replace(wxT("#REGISTRATION#"),parent->registration->GetValue());
	odt.Replace(wxT("#LOCATION#"),layout_locn + layout + _T(".odt"));

	int indexTopODT = odt.Find(seperatorHeaderTop);
	int top = indexTopODT;
	topODT = odt.substr(0,indexTopODT);
	indexTopODT = topODT.find_last_of('<');
	topODT = topODT.substr(0,indexTopODT);
	odt = odt.substr(top);

//wxMessageBox(odt);
	int headerStart = odt.find_first_of('>')+1;
	odt = odt.substr(headerStart);

//wxMessageBox(topODT);
//wxMessageBox(odt);
	int indexBottomODT = odt.Find(seperatorHeaderBottom);
	headerODT = odt.substr(0,indexBottomODT);
	odt = odt.substr(indexBottomODT);
	int headerEnd = odt.find_first_of('>')+1;
	odt = odt.substr(headerEnd);
	indexBottomODT = headerODT.find_first_of('<');
	headerODT = headerODT.substr(indexBottomODT);
	headerODT = headerODT.substr(0,headerODT.find_last_of('<'));

//wxMessageBox(odt);
//wxMessageBox(headerODT);

	indexBottomODT = odt.Find(seperatorTop);
	middleODT = odt.substr(indexBottomODT);
	odt = odt.substr(indexBottomODT);
//wxMessageBox(odt);
//wxMessageBox(middleODT);
	int middleStart = middleODT.find_first_of('>')+1;
	middleODT = middleODT.substr(middleStart);
	odt = odt.substr(odt.find_first_of('>')+1);
//wxMessageBox(odt);
//wxMessageBox(middleODT);

	indexBottomODT = odt.Find(seperatorBottom);
	middleODT = odt.substr(0,indexBottomODT);
	odt = odt.substr(indexBottomODT);
//wxMessageBox(middleODT);
//wxMessageBox(odt);
	int middleEnd = odt.find_first_of('>')+1;
	odt = odt.substr(middleEnd);
	middleODT = middleODT.substr(0,middleODT.find_last_of('<'));
//wxMessageBox(odt);
//wxMessageBox(middleODT);

	wxString filename = this->fileName;

	wxFileInputStream input( filename );
	wxTextInputStream* stream = new wxTextInputStream (input);
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
	while(wxString line = stream->ReadLine())
	{
		if(input.Eof()) break;
		count++;
		if(selection && arrayRows[0]+1 > count) continue;
		if(selection && arrayRows[selCount-1]+1 < count) break;
		newMiddleODT = replacePlaceholder(middleODT,headerODT,line,count,1);
		odtFile << newMiddleODT;
//wxMessageBox(newMiddleODT);
	}
//wxMessageBox(odt);
	odtFile << odt;

    inzip.Eof() && outzip.Close() && out.Commit();
	replacePlaceholder(middleODT,headerODT,_T(""),0,0); // Reset Route if starts with same Route
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
		wxMessageBox(_("Sorry, Your Logbook has no lines"),_("Information"),wxOK);
		return;
	}

	if(::wxFileExists(path))
		::wxRemoveFile(path);
	
	wxFileOutputStream output( path );
	wxTextOutputStream csvFile(output);

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
		wxMessageBox(_("Sorry, Your Logbook has no lines"),_("Information"),wxOK);
		return;
	}

	wxFileOutputStream output( path );
	wxTextOutputStream xmlFile(output);

	if(::wxFileExists(path))
		::wxRemoveFile(path);

	xmlFile << parent->xmlHead;

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

	bool empty = false;
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
		txt << _T("</table:table-row>");

	}
	txt << parent->contentEnd;

	zip.PutNextEntry(wxT("mimetype"));
	txt << wxT("application/vnd.oasis.opendocument.spreadsheet");

	zip.PutNextEntry(wxT("styles.xml"));
	txt << parent->styles;
//	zip.PutNextEntry(wxT("settings.xml"));
//	txt << settings;

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

//	if(timerStopped)
//		parent->timer->Start();
}

void LogbookHTML::backup(wxString path)
{
	wxCopyFile(data_locn+_T("logbook.txt"),path);
}
