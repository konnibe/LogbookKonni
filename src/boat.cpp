//#pragma once
#ifndef WX_PRECOMP
      #include <wx/wx.h>
#endif

#include "boat.h"
#include "LogbookDialog.h"
#include "logbook_pi.h"

#include <wx/textctrl.h>
#include <wx/filefn.h>
#include <wx/dir.h> 
#include <wx/event.h>
#include <wx/object.h>
#include <wx/tokenzr.h>
#include <wx/filename.h>
#include <wx/wfstream.h> 
#include <wx/txtstrm.h> 
#include <wx/zipstrm.h> 

#include <memory>
using namespace std;


Boat::Boat(LogbookDialog* d, wxString data, wxString lay, wxString layoutODT)
{
	parent = d;
	modified = false;
	this->layout = lay;
	this->ODTLayout = layoutODT;

	createFiles(data,lay);
	createTextCtrlConnections();
	createStaticTextList();

	loadData();
}

Boat::~Boat(void)
{
	for(unsigned int i = 0; i < ctrl.GetCount(); i++)
	{
		if(ctrl[i]->IsKindOf(CLASSINFO(wxTextCtrl)))
		{
			bool t =ctrl[i]->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, 
				wxObjectEventFunction(&LogbookDialog::boatNameOnTextEnter)  );
		}
	}
//	ctrl.Clear();
	saveData();
}

void Boat::setLayoutLocation(wxString loc)
{
	if(parent->m_radioBtnHTMLBoat->GetValue())
		layout_locn = layout;
	else
		layout_locn = ODTLayout;
	wxString boatLay = layout_locn;

//	layout_locn = lay;
	layout_locn.Append(_T("boat"));
	parent->appendOSDirSlash(&layout_locn);

	parent->loadLayoutChoice(layout_locn,parent->boatChoice);
}

void Boat::createFiles(wxString data, wxString lay)
{
	data_locn = data;
	data_locn.Append(_T("boat.txt"));
	boatFile = new wxTextFile(data_locn);
	wxFileName wxHomeFiledir = data_locn ;

	if(true != wxHomeFiledir.FileExists())
	{				
		boatFile->Create();
	}

	equip_locn = data;
	equip_locn.Append(_T("equipment.txt"));
	equipFile = new wxTextFile(equip_locn);
	wxHomeFiledir = equip_locn ;

	if(true != wxHomeFiledir.FileExists())
	{				
		equipFile->Create();
	}

	setLayoutLocation(lay);
}

void Boat::createTextCtrlConnections()
{
	wxWindowList l = parent->m_panel72->GetWindowChildren();

	for(unsigned int i = 0; i < l.GetCount(); i++)
	{
		if(l[i]->IsKindOf(CLASSINFO(wxTextCtrl)))
		{
			l[i]->Connect( wxEVT_COMMAND_TEXT_ENTER, 
				wxCommandEventHandler(LogbookDialog::boatNameOnTextEnter),NULL,parent);		
			ctrl.Append(l[i]);
		}
	}
}

void Boat::createStaticTextList()
{
	wxWindowList l = parent->m_panel72->GetWindowChildren();

	for(unsigned int i = 0; i < l.GetCount(); i++)
	{
		if(l[i]->IsKindOf(CLASSINFO(wxStaticText)))
			ctrlStaticText.Append(l[i]);
	}
}

void Boat::loadData()
{
	wxString line;
	
	boatFile->Open();
	if(boatFile->GetLineCount() > 0) 
	{
		line = boatFile->GetLine(0);

		wxStringTokenizer tkz(line, _T("\t"),wxTOKEN_RET_EMPTY );
		
		int c = 0;

		while ( tkz.HasMoreTokens() )
		{
			wxString s;
			s += tkz.GetNextToken().RemoveLast();
			s = parent->restoreDangerChar(s);
			wxTextCtrl* t = wxDynamicCast(ctrl[c++], wxTextCtrl);
			t->SetValue(s);	
		}
	}
	boatFile->Close();

	equipFile->Open();
	if(equipFile->GetLineCount() <= 0) { equipFile->Close(); return; }
	if(parent->m_gridEquipment->GetNumberRows() > 0)
		parent->m_gridEquipment->DeleteRows(0,parent->m_gridEquipment->GetNumberRows()-1);

	for(unsigned int i = 0; i <  equipFile->GetLineCount(); i++)
	{
		line = 	equipFile->GetLine(i);

		parent->m_gridEquipment->AppendRows();

		wxStringTokenizer tkz(line, _T("\t"),wxTOKEN_RET_EMPTY );
		int c = 0;

		while ( tkz.HasMoreTokens() )
		{
			wxString s;
			s = tkz.GetNextToken().RemoveLast();
			s = parent->restoreDangerChar(s);
			parent->m_gridEquipment->SetCellValue(i,c++,s);
		}
	}
	equipFile->Close();
}

void Boat::saveData()
{
	if(!modified) return;

	wxString t,s;

	boatFile->Open();
	boatFile->Clear();

	for(unsigned int i = 0; i < ctrl.GetCount(); i++)
	{
		if(ctrl[i]->IsKindOf(CLASSINFO(wxTextCtrl)))
		{
			wxTextCtrl* te = wxDynamicCast(ctrl[i], wxTextCtrl);
			wxString temp = te->GetValue();
			temp = parent->replaceDangerChar(temp);
			t += temp+wxT(" \t");
		}
	}
	t.RemoveLast();

	boatFile->AddLine(t);
	boatFile->Write();
	boatFile->Close();

	equipFile->Open();
	equipFile->Clear();

	int count = parent->m_gridEquipment->GetNumberRows();
	for(int r = 0; r < count; r++)
	{
		s = _T("");
		for(int i= 0; i < parent->m_gridEquipment->GetNumberCols(); i++)
		{
			s += parent->replaceDangerChar(
						 parent->m_gridEquipment->GetCellValue(r,i))
				+wxT(" \t");
		}
		equipFile->AddLine(s);
	}

	equipFile->Write();
	equipFile->Close();
}

wxString Boat::readLayoutFileODT(wxString layout)
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

void Boat::viewODT(wxString path,wxString layout,bool mode)
{
	toODT(path, layout, mode);
	if(layout != _T(""))
	{
	    wxString fn = data_locn;
	    fn.Replace(_T("txt"),_T("odt"));
		parent->startApplication(fn,_T(".odt"));
	}
}

wxString Boat::toODT(wxString path,wxString layout,bool mode)
{
	wxString s, odt;
//	bool timerStopped = false;

	if(layout == _T(""))
	{
		wxMessageBox(_("Sorry, no Layout installed"),_("Information"),wxOK);
		return _T("");
	}

	saveData();

	odt = readLayoutFileODT(layout);

	for(unsigned int i = 0; i < ctrl.GetCount(); i++)
	{
		if(ctrl[i]->IsKindOf(CLASSINFO(wxTextCtrl)))
		{
			wxTextCtrl* te = wxDynamicCast(ctrl[i], wxTextCtrl);

			switch(i)
			{
			case 0: 
				odt.Replace(wxT("#BOATNAME#"),te->GetValue());
				break;
			case 1: 
				odt.Replace(wxT("#HOMEPORT#"),te->GetValue());
				break;
			case 2: 
				odt.Replace(wxT("#CALLSIGN#"),te->GetValue());
				break;
			case 3: 
				odt.Replace(wxT("#HIN#"),te->GetValue());
				break;
			case 4: 
				odt.Replace(wxT("#SAILNO#"),te->GetValue());
				break;
			case 6: 
				odt.Replace(wxT("#REGISTRATION#"),te->GetValue());
				break;
			case 5: 
				odt.Replace(wxT("#INSURANCE#"),te->GetValue());
				break;
			case 7: 
				odt.Replace(wxT("#POLICY#"),te->GetValue());
				break;
			case 8: 
				odt.Replace(wxT("#MMSI#"),te->GetValue());
				break;
			case 9: 
				odt.Replace(wxT("#ONAME#"),te->GetValue());
				break;
			case 10: 
				odt.Replace(wxT("#OFIRST#"),te->GetValue());
				break;
			case 11: 
				odt.Replace(wxT("#TELEPHONE#"),te->GetValue());
				break;
			case 12: 
				odt.Replace(wxT("#STREET#"),te->GetValue());
				break;
			case 13: 
				odt.Replace(wxT("#ZIP#"),te->GetValue());
				break;
			case 14: 
				odt.Replace(wxT("#TOWN#"),te->GetValue());				
				break;
			case 15: 
				odt.Replace(wxT("#TYPE#"),te->GetValue());
				break;
			case 16: 
				odt.Replace(wxT("#BUILDER#"),te->GetValue());
				break;
			case 17: 
				odt.Replace(wxT("#HULL#"),te->GetValue());
				break;
			case 18: 
				odt.Replace(wxT("#LAUNCHED#"),te->GetValue());
				break;
			case 19:
				odt.Replace(wxT("#YARDNO#"),te->GetValue());
				break;
			case 20:
				odt.Replace(wxT("#DESIGNER#"),te->GetValue());
				break;
			case 21: 
				odt.Replace(wxT("#CONSTRUCT#"),te->GetValue());
				break;
			case 22: 
				odt.Replace(wxT("#LOA#"),te->GetValue());
				break;
			case 23:
				odt.Replace(wxT("#LOD#"),te->GetValue());
				break;
			case 24:
				odt.Replace(wxT("#LWL#"),te->GetValue());				
				break;
			case 25: 
				odt.Replace(wxT("#BEAM#"),te->GetValue());				
				break;
			case 26: 
				odt.Replace(wxT("#DRAFT#"),te->GetValue());
				break;
			case 27: 
				odt.Replace(wxT("#GRT#"),te->GetValue());				
				break;
			case 28: 
				odt.Replace(wxT("#NRT#"),te->GetValue());				
				break;
			case 29: 
				odt.Replace(wxT("#THAMES#"),te->GetValue());
				break;
			case 30: 
				odt.Replace(wxT("#DISPL#"),te->GetValue());				
				break;
			}
		}
	}

	if(odt.Contains(wxT("{{")))
		odt = repeatAreaODT(odt);
	
	wxString fn = data_locn;
	fn.Replace(_T("txt"),_T("odt"));
	if(mode == false)
	{
		fn.Replace(wxT("txt"),wxT("odt"));
	}
	else
		fn = path;

	if(::wxFileExists(fn))
		::wxRemoveFile(fn);

    auto_ptr<wxFFileInputStream> in(new wxFFileInputStream(layout_locn + layout + _T(".odt")));
    wxTempFileOutputStream out(fn);

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

	odtFile << odt;

	inzip.Eof() && outzip.Close() && out.Commit();
	return fn;
}

wxString Boat::repeatAreaODT(wxString odt)
{
	wxString topODT;
	wxString bottomODT;
	wxString middleODT;

	wxString seperatorTop = wxT("{{");
	wxString seperatorBottom = wxT("}}");

	int indexTop = odt.First(seperatorTop)+seperatorTop.Len();
	int indexBottom = odt.First(seperatorBottom)+seperatorBottom.Len();

	topODT = odt.substr(0,indexTop);
	middleODT = odt.substr(indexTop+9);
	indexTop = topODT.find_last_of('<');
	topODT = odt.substr(0,indexTop);
	bottomODT = odt.substr(indexBottom,odt.Len()-1);
	indexBottom = bottomODT.find_first_of('>')+1;
	bottomODT = bottomODT.substr(indexBottom,odt.Len()-1);

	middleODT.Replace(bottomODT,_T(""));
	indexBottom = middleODT.First(seperatorBottom)+seperatorBottom.Len();
	middleODT = middleODT.substr(0,indexBottom-1);
	indexBottom = middleODT.find_last_of('<');
	middleODT = middleODT.substr(0,indexBottom);

	wxString newMiddleODT = _T("");
	for(int i = 0; i < parent->m_gridEquipment->GetNumberRows(); i++)
	{
		wxString temp = middleODT;
		for(int c = 0; c < parent->m_gridEquipment->GetNumberCols(); c++)
		{
		wxString text = parent->m_gridEquipment->GetCellValue(i,c);

		switch(c)
		{
			case 0: 		
				temp.Replace(_T("#ETYPE#"),text);
				break;
			case 1: 		
				temp.Replace(_T("#DISCRIPTION#"),text);
				break;
			case 2: 		
				temp.Replace(_T("#SERIAL#"),text);
				break;
			case 3: 		
				temp.Replace(_T("#REMARKS#"),text);
				break;
		}
		}
		newMiddleODT += temp;
	}

	odt = topODT + newMiddleODT + bottomODT;
	return odt;
}

void Boat::toHTML(wxString path, wxString layout, bool mode)
{
	wxString s, html;

	if(layout == _T(""))
	{
		wxMessageBox(_("Sorry, no Layout installed"),_("Information"),wxOK);
		return;
	}

	saveData();

	boatLayoutFile = new wxTextFile(layout_locn+layout+wxT(".html"));
	boatLayoutFile->Open();

	wxString fn = data_locn;
	fn.Replace(_T("txt"),_T("html"));
	if(mode == false)
		boatHTMLFile = new wxFile(fn,wxFile::write);
	else
		boatHTMLFile = new wxFile(path,wxFile::write);

	for(unsigned int i = 0; i < boatLayoutFile->GetLineCount(); i++)
		html += boatLayoutFile->GetLine(i);
	boatLayoutFile->Close();

	for(unsigned int i = 0; i < ctrl.GetCount(); i++)
	{
		if(ctrl[i]->IsKindOf(CLASSINFO(wxTextCtrl)))
		{
			wxTextCtrl* te = wxDynamicCast(ctrl[i], wxTextCtrl);

			switch(i)
			{
			case 0: 
				html.Replace(wxT("#BOATNAME#"),te->GetValue());
				break;
			case 1: 
				html.Replace(wxT("#HOMEPORT#"),te->GetValue());
				break;
			case 2: 
				html.Replace(wxT("#CALLSIGN#"),te->GetValue());
				break;
			case 3: 
				html.Replace(wxT("#HIN#"),te->GetValue());
				break;
			case 4: 
				html.Replace(wxT("#SAILNO#"),te->GetValue());
				break;
			case 6: 
				html.Replace(wxT("#REGISTRATION#"),te->GetValue());
				break;
			case 5: 
				html.Replace(wxT("#INSURANCE#"),te->GetValue());
				break;
			case 7: 
				html.Replace(wxT("#POLICY#"),te->GetValue());
				break;
			case 8: 
				html.Replace(wxT("#MMSI#"),te->GetValue());
				break;
			case 9: 
				html.Replace(wxT("#ONAME#"),te->GetValue());
				break;
			case 10: 
				html.Replace(wxT("#OFIRST#"),te->GetValue());
				break;
			case 11: 
				html.Replace(wxT("#TELEPHONE#"),te->GetValue());
				break;
			case 12: 
				html.Replace(wxT("#STREET#"),te->GetValue());
				break;
			case 13: 
				html.Replace(wxT("#ZIP#"),te->GetValue());
				break;
			case 14: 
				html.Replace(wxT("#TOWN#"),te->GetValue());				
				break;
			case 15: 
				html.Replace(wxT("#TYPE#"),te->GetValue());
				break;
			case 16: 
				html.Replace(wxT("#BUILDER#"),te->GetValue());
				break;
			case 17: 
				html.Replace(wxT("#HULL#"),te->GetValue());
				break;
			case 18: 
				html.Replace(wxT("#LAUNCHED#"),te->GetValue());
				break;
			case 19:
				html.Replace(wxT("#YARDNO#"),te->GetValue());
				break;
			case 20:
				html.Replace(wxT("#DESIGNER#"),te->GetValue());
				break;
			case 21: 
				html.Replace(wxT("#CONSTRUCT#"),te->GetValue());
				break;
			case 22: 
				html.Replace(wxT("#LOA#"),te->GetValue());
				break;
			case 23:
				html.Replace(wxT("#LOD#"),te->GetValue());
				break;
			case 24:
				html.Replace(wxT("#LWL#"),te->GetValue());				
				break;
			case 25: 
				html.Replace(wxT("#BEAM#"),te->GetValue());				
				break;
			case 26: 
				html.Replace(wxT("#DRAFT#"),te->GetValue());
				break;
			case 27: 
				html.Replace(wxT("#GRT#"),te->GetValue());				
				break;
			case 28: 
				html.Replace(wxT("#NRT#"),te->GetValue());				
				break;
			case 29: 
				html.Replace(wxT("#THAMES#"),te->GetValue());
				break;
			case 30: 
				html.Replace(wxT("#DISPL#"),te->GetValue());				
				break;
			}
		}
	}

	if(html.Contains(wxT("<!--Repeat -->")))
		html = repeatArea(html);
	
	wxString str(html, wxConvUTF8);	

	boatHTMLFile->Write(str);
	boatHTMLFile->Close();
}

wxString Boat::repeatArea(wxString html)
{
	wxString topHTML;
	wxString bottomHTML;
	wxString middleHTML;

	wxString seperatorTop = wxT("<!--Repeat -->");
	wxString seperatorBottom = wxT("<!--Repeat End -->");

	int indexTop = html.First(seperatorTop)+seperatorTop.Len();
	int indexBottom = html.First(seperatorBottom)+seperatorBottom.Len();

	topHTML = html.substr(0,indexTop);
	bottomHTML = html.substr(indexBottom,html.Len()-1);
	middleHTML = html.substr(indexTop,indexBottom-indexTop);


	wxString newMiddleHTML = _T("");
	for(int i = 0; i < parent->m_gridEquipment->GetNumberRows(); i++)
	{
		wxString temp = middleHTML;
		for(int c = 0; c < parent->m_gridEquipment->GetNumberCols(); c++)
		{
		wxString text = parent->m_gridEquipment->GetCellValue(i,c);

		switch(c)
		{
			case 0: 		
				temp.Replace(_T("#ETYPE#"),text);
				break;
			case 1: 		
				temp.Replace(_T("#DISCRIPTION#"),text);
				break;
			case 2: 		
				temp.Replace(_T("#SERIAL#"),text);
				break;
			case 3: 		
				temp.Replace(_T("#REMARKS#"),text);
				break;
		}
		}
		newMiddleHTML += temp;
	}

	html = topHTML + newMiddleHTML + bottomHTML;
	return html;
}

void Boat::viewHTML(wxString path, wxString layout, bool mode)
{
	toHTML(path, layout, mode);
	if(layout != _T(""))
	{
	    wxString fn = data_locn;
	    fn.Replace(_T("txt"),_T("html"));
	    parent->startBrowser(fn);
	}
}

void Boat::addEquip()
{
	wxString s;

	modified = true;

	parent->m_gridEquipment->AppendRows();
	parent->m_gridEquipment->SetCellValue(parent->m_gridEquipment->GetNumberRows()-1,
										  parent->m_gridEquipment->GetNumberCols()-1,_T(" "));
	
	equipFile->Open();

	for(int i = 0; i < parent->m_gridEquipment->GetNumberCols(); i++)
	{
		s += _T(",");	
	}
	
	equipFile->AddLine(s);
	equipFile->Write();
	equipFile->Close();
}
void Boat::cellChanged(int row, int col)
{
	modified = true;

	if(parent->m_gridEquipment->GetCellValue(
		row,parent->m_gridEquipment->GetNumberCols()-1) == _T(""))
			parent->m_gridEquipment->SetCellValue(
				row,parent->m_gridEquipment->GetNumberCols()-1,_T(" "));
}

void Boat::deleteRow(int row)
{
	int answer = wxMessageBox(wxString::Format(_("Delete Row Nr. %i ?"),row+1), _("Confirm"),
                              wxYES_NO | wxCANCEL, parent);
	if (answer == wxYES)
		parent->m_gridEquipment->DeleteRows(row);
}

void Boat::toCSV(wxString savePath)
{
	wxString line;
	wxString s;
	bool timerStopped = false;

	saveData();

	if(parent->timer->IsRunning())
	{
		parent->timer->Stop();
		timerStopped = true;
	}

	wxTextFile* csvFile = new wxTextFile(savePath);
	wxString path = savePath;
	path.Replace(wxT("Boat"),wxT("Equipment"));
	wxTextFile* csvEquipFile = new wxTextFile(path);

	if(csvFile->Exists())
	{
		::wxRemoveFile(savePath);
		::wxRemoveFile(path);
		csvFile->Create();
		equipFile->Create();
	}

	saveCSV(csvFile,true);
	saveCSV(csvEquipFile,false);

	if(timerStopped)
		parent->timer->Start();
}

void Boat::saveCSV(wxTextFile* file, bool mode)
{
	wxString line, s;

//	if(parent->timer->IsRunning())
//		parent->timer->Stop();

	saveData();

	((mode)?boatFile:equipFile)->Open();

	for(unsigned int i = 0; i < ((mode)?boatFile:equipFile)->GetLineCount(); i++)
	{
		line = ((mode)?boatFile:equipFile)->GetLine(i);
		wxStringTokenizer tkz(line, _T("\t"),wxTOKEN_RET_EMPTY );

		while ( tkz.HasMoreTokens() )
		{
			s += wxT("\"")+parent->restoreDangerChar(tkz.GetNextToken().RemoveLast())+_T("\",");
		}
		s.RemoveLast();
		file->AddLine(s);
		s=_T("");
	}

	file->Write();
	file->Close();
	((mode)?boatFile:equipFile)->Close();

//	if(parent->timer->IsRunning())
//		parent->timer->Start();
}

void Boat::toXML(wxString savePath)
{
	wxString line;
	wxString s;
//	bool timerStopped = false;

/*	if(parent->timer->IsRunning())
	{
		parent->timer->Stop();
		timerStopped = true;
	}
*/
	wxTextFile* xmlFile = new wxTextFile(savePath);
	wxString path = savePath;
	path.Replace(wxT("Boat"),wxT("Equipment"));
	wxTextFile* xmlEquipFile = new wxTextFile(path);

	if(xmlFile->Exists())
	{
		::wxRemoveFile(savePath);
		::wxRemoveFile(path);
		xmlFile->Create();
		xmlEquipFile->Create();
	}

	saveData();
	saveXML(xmlFile,true);
	saveXML(xmlEquipFile,false);

//	if(timerStopped)
//		parent->timer->Start();
}

void Boat::saveXML(wxTextFile* xmlFile, bool mode)
{
	wxTextFile* file;
	wxString temp;

	wxString line, s;

//	if(parent->timer->IsRunning())
//		parent->timer->Stop();

	if(mode)
		file = boatFile;
	else 
		file = equipFile;

	file->Open();

	xmlFile->AddLine(parent->xmlHead);

	for(unsigned int i = 0; i < file->GetLineCount(); i++)
	{
		line = file->GetLine(i);
		wxStringTokenizer tkz(line, _T("\t"),wxTOKEN_RET_EMPTY );
		s = wxString::Format(_T("<Row ss:Height=\"%u\">"),parent->m_gridGlobal->GetRowHeight(i));

		while ( tkz.HasMoreTokens() )
		{ 
			s += _T("<Cell>");
			s += _T("<Data ss:Type=\"String\">#DATA#</Data>");
			temp = parent->restoreDangerChar(tkz.GetNextToken().RemoveLast());
			temp.Replace(_T("\n"),_T("&#10;"));
			temp.Replace(_T("&"),_T("&amp;"));
			temp.Replace(_T("\""),_T("&quot;"));
			temp.Replace(_T("<"),_T("&lt;"));
			temp.Replace(_T(">"),_T("&gt;"));
			temp.Replace(_T("'"),_T("&apos;"));
		//	temp.Replace(_T("\""),_T(""));
			s.Replace(_T("#DATA#"),temp);
			s += _T("</Cell>");
		}
		s += _T("</Row>");
		xmlFile->AddLine(s);
	}

	xmlFile->AddLine(parent->xmlEnd);
	xmlFile->Write();
	file->Close();
	xmlFile->Close();

//	if(parent->timer->IsRunning())
//		parent->timer->Start();
}

void Boat::backup(wxString path)
{
	wxFileName fn(path);
	wxString z = fn.GetName();

	saveData();

	wxCopyFile(data_locn,path);
	path.Replace(z,_T("equipment"));
	wxCopyFile(equip_locn,path);
}

void Boat::toODS(wxString path)
{
	wxString s = _T("");
	wxString line;
	wxString temp;
//	bool timerStopped = false;

	saveData();

//	if(parent->timer->IsRunning())
//	{
//		parent->timer->Stop();
//		timerStopped = true;
//	}

	saveODS(path,false);

	wxFileName fn(path);
	wxString sf = fn.GetName();
	path.Replace(sf,_T("equipment"));

	saveODS(path, true);

//	if(timerStopped)
//		parent->timer->Start();
}

void Boat::saveODS( wxString path, bool mode )
{	
	wxString temp, col, tableName;
	int x = 0;

	if(mode)
	{
		temp = equip_locn;
		col = _T("table:number-columns-repeated=\"4\"");
		tableName = _T("Equipment");
		x = parent->m_gridEquipment->GetNumberCols();
	}
	else
	{
		temp = data_locn;
		col = _T("table:number-columns-repeated=\"31\""); 
		tableName = _T("Boat");
		x = ctrlStaticText.GetCount();
	}


	wxFileInputStream input( temp );
	wxTextInputStream* stream = new wxTextInputStream (input);

	wxFFileOutputStream out(path);
	wxZipOutputStream zip(out);
	wxTextOutputStream txt(zip);
	wxString sep(wxFileName::GetPathSeparator());

	temp = parent->content;
	temp.Replace(_T("table:number-columns-repeated=\"33\""),col);
	temp.Replace(_T("Logbook"),tableName);
	zip.PutNextEntry(wxT("content.xml"));
	txt << temp;

	txt << _T("<table:table-row table:style-name=\"ro2\">");
	for(int i = 0; i < x; i++)
	{
		txt << _T("<table:table-cell office:value-type=\"string\">");
		txt << _T("<text:p>");
		if(!mode)
		{
			wxStaticText* t = wxDynamicCast(ctrlStaticText[i], wxStaticText);
			txt << t->GetLabel();
		}
		else
			txt << parent->m_gridEquipment->GetColLabelValue(i);
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
		wxStringTokenizer tkz(line, _T("\t"),wxTOKEN_RET_EMPTY );

		while ( tkz.HasMoreTokens() )
		{ 
			wxString s = parent->restoreDangerChar(tkz.GetNextToken().RemoveLast());
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
}

