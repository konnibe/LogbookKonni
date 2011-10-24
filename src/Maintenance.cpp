#include "Maintenance.h"
#include "LogbookDialog.h"

#ifndef WX_PRECOMP
     # include <wx/wx.h>
#endif

#include <wx/tokenzr.h>
#include <wx/filename.h> 
#include <wx/mimetype.h>
#include <wx/wfstream.h> 
#include <wx/txtstrm.h> 
#include <wx/zipstrm.h> 
#include <wx/generic/gridctrl.h>

#include <memory>
using namespace std;

Maintenance::Maintenance(LogbookDialog* d, wxString data, wxString layout, wxString layoutODT)
{
	dialog = d;
	this->layout_locn = layout;
	this->ODTLayout = layoutODT;
	grid = d->m_gridMaintanence;
	repairs = d->m_gridMaintanenceRepairs;
	buyparts = d->m_gridMaintenanceBuyParts;
	selectedCol = 0;
	selectedRow = 0;
	modified = false;

	green = wxColour(0,255,0);
	red = wxColour(255,0,0);
	redlight = wxColour(255,128,128);
	yellow = wxColour(255,255,0);
	yellowlight = wxColour(255,255,155);
	white = wxColour(255,255,255);

	wxString serviceData = data;
	wxTextFile *serviceFile;
	serviceData.Append(_T("service.txt"));
	wxFileName wxHomeFiledir = serviceData ;
	if(!wxHomeFiledir.FileExists())
	{
		serviceFile = new wxTextFile(serviceData);	
		serviceFile->Create();
	}
	else
		serviceFile = new wxTextFile(serviceData);

	data_locn = serviceData;

	wxString buyPartsData = data;
	wxTextFile *buyPartsFile;
	buyPartsData.Append(_T("buyparts.txt"));
	wxHomeFiledir = buyPartsData ;
	if(!wxHomeFiledir.FileExists())
	{
		buyPartsFile = new wxTextFile(buyPartsData);	
		buyPartsFile->Create();
	}
	else
		buyPartsFile = new wxTextFile(buyPartsData);

	data_locnBuyParts = buyPartsData;

	wxString repairsData = data;
	wxTextFile *repairsFile;
	repairsData.Append(_T("repairs.txt"));
	wxHomeFiledir = repairsData ;
	if(!wxHomeFiledir.FileExists())
	{
		repairsFile = new wxTextFile(repairsData);	
		repairsFile->Create();
	}
	else
		repairsFile = new wxTextFile(repairsData);

	data_locnRepairs = repairsData;

	setLayoutLocation();

	m_choices = wxArrayString();
	m_choices.Add(dialog->m_gridGlobal->GetColLabelValue(6)+_T(" +")); // Distance/T
	m_choices.Add(dialog->m_gridMotorSails->GetColLabelValue(1)+_T(" +")); // Motor/h
	m_choices.Add(dialog->m_gridGlobal->GetColLabelValue(3)); // Sign

	m_YesNo = wxArrayString();
	m_YesNo.Add(_("Yes")); 
	m_YesNo.Add(_("No")); 

	m_Priority = wxArrayString();
	m_Priority.Add(_T("0")); 
	m_Priority.Add(_T("1")); 
	m_Priority.Add(_T("2")); 
	m_Priority.Add(_T("3")); 
	m_Priority.Add(_T("4")); 
	m_Priority.Add(_T("5"));

}

Maintenance::~Maintenance(void)
{
	update();
	updateRepairs();
	updateBuyParts();
}

void Maintenance::setLayoutLocation()
{
	wxString layout_locn;

	if(dialog->m_radioBtnHTMLBuyParts->GetValue())
		layout_locnBuyParts = this->layout_locn;
	else
		layout_locnBuyParts = ODTLayout;

	wxString buypartsLay = layout_locnBuyParts;

	buypartsLay.Append(_T("buyparts"));
	dialog->appendOSDirSlash(&buypartsLay);
	layout_locnBuyParts = buypartsLay;
	dialog->loadLayoutChoice(buypartsLay,dialog->m_choiceSelectLayoutBuyParts);


	if(dialog->m_radioBtnHTMLService->GetValue())
		layout_locnService = this->layout_locn;
	else
		layout_locnService = ODTLayout;

	wxString serviceLay = layout_locnService;

	serviceLay.Append(_T("service"));
	dialog->appendOSDirSlash(&serviceLay);
	layout_locnService = serviceLay;
	dialog->loadLayoutChoice(serviceLay,dialog->m_choiceSelectLayoutService);

	if(dialog->m_radioBtnHTMLRepairs->GetValue())
		layout_locnRepairs = this->layout_locn;
	else
		layout_locnRepairs = ODTLayout;

	wxString repairsLay = layout_locnRepairs;

	repairsLay.Append(_T("repairs"));
	dialog->appendOSDirSlash(&repairsLay);
	layout_locnRepairs = repairsLay;
	dialog->loadLayoutChoice(repairsLay,dialog->m_choiceSelectLayoutRepairs);
}

void Maintenance::setAlignmentService()
{
	grid->SetReadOnly(lastRow,START);
	grid->SetReadOnly(lastRow,PRIORITY);
	grid->SetCellAlignment( lastRow,PRIORITY,wxALIGN_CENTER, wxALIGN_TOP );
	grid->SetCellAlignment( lastRow,TEXT,wxALIGN_LEFT, wxALIGN_TOP );
	grid->SetCellAlignment( lastRow,IF,wxALIGN_CENTER, wxALIGN_TOP );
	grid->SetCellAlignment( lastRow,ACTIVE,wxALIGN_CENTER, wxALIGN_TOP );
}

void Maintenance::addLine()
{
	modified = true;
	grid->AppendRows();

	lastRow = grid->GetNumberRows()-1;
	selectedRow = lastRow;
	setAlignmentService();


	grid->SetCellEditor(lastRow,IF,new wxGridCellChoiceEditor(m_choices));
	grid->SetCellEditor(lastRow,ACTIVE,new wxGridCellChoiceEditor(m_YesNo));

	grid->SetCellValue(lastRow,PRIORITY,_T("0"));
	grid->SetCellValue(lastRow,IF,m_choices[0]);
	grid->SetCellValue(lastRow,WARN,_T("1"));
	grid->SetCellValue(lastRow,URGENT,_T("1"));
	cellCollChanged(IF, lastRow);
	cellCollChanged(WARN, lastRow);
	checkService(dialog->m_gridGlobal->GetNumberRows()-1);
	setRowBackground(lastRow,white);
	grid->SetCellBackgroundColour(lastRow,START,wxColour( 240, 240, 240 ));

	grid->SetCellValue(lastRow,ACTIVE,_("No"));
}

void Maintenance::addLineRepairs()
{
	modified = true;
	repairs->AppendRows();

	lastRowRepairs = repairs->GetNumberRows()-1;
	selectedRowRepairs = lastRowRepairs;
	setAlignmentRepairs();

	repairs->SetCellValue(lastRowRepairs,RPRIORITY,_T("0"));
	checkRepairs();
}

void Maintenance::addLineBuyParts()
{
	modified = true;
	buyparts->AppendRows();

	lastRowBuyParts = buyparts->GetNumberRows()-1;
	selectedRowBuyParts = lastRowBuyParts;	
	setAlignmentBuyParts();

	buyparts->SetCellValue(lastRowBuyParts,PPRIORITY,_T("0"));
	checkBuyParts();
}

void Maintenance::setAlignmentRepairs()
{
	repairs->SetCellAlignment(lastRowRepairs,RPRIORITY,wxALIGN_CENTER, wxALIGN_TOP);
	repairs->SetCellAlignment(lastRowRepairs,RTEXT,wxALIGN_LEFT, wxALIGN_TOP);

	//wxGridCellChoiceEditor *combo = new wxGridCellChoiceEditor(m_Priority);
//	wxComboBox *box = (wxComboBox*)combo->GetControl();
//	box->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( Maintenance::test ), NULL, this );
	repairs->SetCellEditor(lastRowRepairs,RPRIORITY,new wxGridCellChoiceEditor(m_Priority));
	repairs->SetCellEditor(lastRowRepairs,RTEXT,new wxGridCellAutoWrapStringEditor);
}

void Maintenance::setAlignmentBuyParts()
{
	buyparts->SetCellEditor(lastRowBuyParts,PPRIORITY,new wxGridCellChoiceEditor(m_Priority));
	buyparts->SetCellEditor(lastRowBuyParts,PARTS,new wxGridCellAutoWrapStringEditor);
	buyparts->SetCellAlignment(lastRowBuyParts,PPRIORITY,wxALIGN_CENTER, wxALIGN_TOP);
	buyparts->SetCellAlignment(lastRowBuyParts,PCATEGORY,wxALIGN_CENTER, wxALIGN_TOP);
	buyparts->SetCellAlignment(lastRowBuyParts,TITLE,wxALIGN_LEFT, wxALIGN_TOP);
	buyparts->SetCellAlignment(lastRowBuyParts,PARTS,wxALIGN_LEFT, wxALIGN_TOP);
	buyparts->SetCellAlignment(lastRowBuyParts,DATE,wxALIGN_CENTER, wxALIGN_TOP);
	buyparts->SetCellAlignment(lastRowBuyParts,AT,wxALIGN_LEFT, wxALIGN_TOP);
}

void Maintenance::loadData()
{
	wxString t, s;

	wxFileInputStream input( data_locn );
	wxTextInputStream* stream = new wxTextInputStream (input);

	int row = 0;
	while( t = stream->ReadLine())
	{
		if(input.Eof()) break;
		addLine();

		wxStringTokenizer tkz(t, _T("\t"),wxTOKEN_RET_EMPTY );
		int c = 0;
		while ( tkz.HasMoreTokens() )
		{
			s = dialog->restoreDangerChar(tkz.GetNextToken());
			s.RemoveLast();

			switch(c)
			{
			case TEXT:		grid->SetCellValue(row,TEXT,s); break;
			case IF:		grid->SetCellValue(row,IF,s); break;
			case WARN:		grid->SetCellValue(row,WARN,s); break;
			case URGENT:	grid->SetCellValue(row,URGENT,s); break;
			case START:		grid->SetCellValue(row,START,s); break;
			case ACTIVE:	grid->SetCellValue(row,ACTIVE,s); break;
			}
			c++;
		}
		row++;
	}

	wxFileInputStream input1( data_locnBuyParts );
	wxTextInputStream* stream1 = new wxTextInputStream (input1);

	row = 0;
	while( t = stream1->ReadLine())
	{
		if(input1.Eof()) break;
		addLineBuyParts();

		wxStringTokenizer tkz(t, _T("\t"),wxTOKEN_RET_EMPTY );
		int c = 0;
		while ( tkz.HasMoreTokens() )
		{
			s = dialog->restoreDangerChar(tkz.GetNextToken());
			s.RemoveLast();

			switch(c)
			{
			case PRIORITY:	buyparts->SetCellValue(row,PRIORITY,s);
							break;
			case PCATEGORY:	buyparts->SetCellValue(row,PCATEGORY,s);
							break;
			case TITLE:		buyparts->SetCellValue(row,TITLE,s);
							break;
			case PARTS   :	buyparts->SetCellValue(row,PARTS,s);
							break;
			case DATE    :	buyparts->SetCellValue(row,DATE,s);
							break;
			case AT      :	buyparts->SetCellValue(row,AT,s);
							break;
			}
			c++;
		}
		buyparts->AutoSizeRow(row,false);
		row++;
	}

	wxFileInputStream input2( data_locnRepairs );
	wxTextInputStream* stream2 = new wxTextInputStream (input2);

	row = 0;
	while( t = stream2->ReadLine())
	{
		if(input2.Eof()) break;
		addLineRepairs();

		wxStringTokenizer tkz(t, _T("\t"),wxTOKEN_RET_EMPTY );
		int c = 0;
		while ( tkz.HasMoreTokens() )
		{
			s = dialog->restoreDangerChar(tkz.GetNextToken());
			s.RemoveLast();

			switch(c)
			{
			case RPRIORITY:	repairs->SetCellValue(row,RPRIORITY,s);
							break;
			case RTEXT:		repairs->SetCellValue(row,RTEXT,s);
							break;
			}
			c++;
		}
		repairs->AutoSizeRow(row,false);
		row++;
	}

	checkService(dialog->m_gridGlobal->GetNumberRows()-1);
	checkRepairs();
	checkBuyParts();
	modified = false;
}

void Maintenance::buyParts(int i)
{
	wxString s;
	wxGrid *grid;
	int text, selectedRow;

	if(i == 0)
	{
		s = _("Service");
		grid = this->grid;
		text = TEXT;
		selectedRow = this->selectedRow;
	}
	else
	{
		s = _("Repairs");
		grid = repairs;
		text = RTEXT;
		selectedRow = selectedRowRepairs;
	}

	addLineBuyParts();

	dialog->m_gridMaintenanceBuyParts->SetCellValue(lastRowBuyParts,PPRIORITY,
		grid->GetCellValue(selectedRow,PRIORITY));
	dialog->m_gridMaintenanceBuyParts->SetCellValue(lastRowBuyParts,PCATEGORY,s);
	dialog->m_gridMaintenanceBuyParts->SetCellValue(lastRowBuyParts,TITLE,
		grid->GetCellValue(selectedRow,text).Trim());
	checkBuyParts();

	dialog->m_notebook6->SetSelection(2);
}

void Maintenance::setRowBackground(int row, wxColour &c)
{
	for(int i= 0; i < grid->GetNumberCols(); i++)
		grid->SetCellBackgroundColour(row,i,c);

	if(c == wxColour(255,0,0))
		grid->SetCellValue(row,PRIORITY,_T("1"));
	else if(c == wxColour(255,255,0))
		grid->SetCellValue(row,PRIORITY,_T("3"));
	else if(c == wxColour(0,255,0))
		grid->SetCellValue(row,PRIORITY,_T("5"));
	else if(c == wxColour(255,255,255))
		grid->SetCellValue(row,PRIORITY,_T("0"));
}

void Maintenance::checkService(int row)
{
//	wxGridCellChoiceEditor *ed;
	wxString g, yesno;
	int choice = -1;
	double startValue, warnValue, urgentValue;
	double distanceTotal, motorTotal;
	wxString sign, cell;
	int border = 0;

	for(int r = 0; r < grid->GetNumberRows(); r++)
	{
		g = grid->GetCellValue(r,IF);

		yesno = grid->GetCellValue(r,ACTIVE);
		if(g.IsEmpty()) continue;

		cell = grid->GetCellValue(r,START);
		cell.ToDouble(&startValue);
		cell = grid->GetCellValue(r,WARN);
		cell.ToDouble(&warnValue);
		cell = grid->GetCellValue(r,URGENT);
		cell.ToDouble(&urgentValue);

		cell = dialog->m_gridGlobal->GetCellValue(row,6);
		cell.ToDouble(&distanceTotal);
		cell = dialog->m_gridMotorSails->GetCellValue(row,1);
		cell.ToDouble(&motorTotal);

		if(g == m_choices[0])
			choice = 0;
		else if (g == m_choices[1])
			choice = 1;
		else if (g == m_choices[2])
			choice = 2;

		if(yesno != _("No"))
		{
			switch(choice)
			{
			case 0:
				if(distanceTotal >= startValue+urgentValue)
				{
					border = 2;
					dialog->SetBackgroundColour(red);
					setRowBackground(r,red);
					break;
				}
				else if(distanceTotal >= startValue+warnValue)
				{
					if(border != 2)
					{
						border = 1;
						dialog->SetBackgroundColour(yellow);
					}
					setRowBackground(r,yellow);
					break;
				}
				else
				{
					setRowBackground(r,green);
				}
				break;
			case 1:
				if(motorTotal >= startValue+urgentValue)
				{
					border = 2;
					dialog->SetBackgroundColour(red);
					setRowBackground(r,red);
					break;
				}
				else if(motorTotal >= startValue+warnValue)
				{
					if(border != 2)
					{
						border = 1;
						dialog->SetBackgroundColour(yellow);
					}
					setRowBackground(r,yellow);
					break;
				}
				else
				{
					setRowBackground(r,green);
				}
				break;
			case 2:
				if(grid->GetCellValue(r,URGENT) == dialog->m_gridGlobal->GetCellValue(row,3))
				{
					border = 2;
					dialog->SetBackgroundColour(red);
					setRowBackground(r,red);
					break;
				}
				else
				{
					setRowBackground(r,green);
				}
				break;
				break;
			}
		}
		else
			setRowBackground(r,white);

		setBuyPartsPriority(grid,r,PRIORITY,TEXT);
	}

	checkBuyParts();

	if(border != 0)
		;
//wxMessageBox(wxString::Format(_("startValue = %5.2f\nwarnValue = %5.2f\n gesamt = %5.2f\nmotorTotal = %5.2f"),
						//startValue,warnValue,startValue+warnValue,motorTotal));
	else
		dialog->SetBackgroundColour(dialog->defaultBackground);
	dialog->Refresh();
}

void Maintenance::checkRepairs()
{
	for(int row = 0; row < repairs->GetNumberRows(); row++)
	{
		long i;
		repairs->GetCellValue(row,RPRIORITY).ToLong(&i);
		switch(i)
		{
		case 0:
			setRowBackgroundRepairs(row,white);
			break;
		case 1:
			setRowBackgroundRepairs(row,red);
			break;
		case 2:
			setRowBackgroundRepairs(row,redlight);
			break;
		case 3:
			setRowBackgroundRepairs(row,yellow);
			break;
		case 4:
			setRowBackgroundRepairs(row,yellowlight);
			break;
		case 5:
			setRowBackgroundRepairs(row,green);
			break;
		}
		setBuyPartsPriority(repairs,row,RPRIORITY,RTEXT);
	}
	repairs->Refresh();
	checkBuyParts();
}

void Maintenance::checkBuyParts()
{
	for(int row = 0; row < buyparts->GetNumberRows(); row++)
	{
		long i;
		buyparts->GetCellValue(row,RPRIORITY).ToLong(&i);
		switch(i)
		{
		case 0:
			setRowBackgroundBuyParts(row,white);
			break;
		case 1:
			setRowBackgroundBuyParts(row,red);
			break;
		case 2:
			setRowBackgroundBuyParts(row,redlight);
			break;
		case 3:
			setRowBackgroundBuyParts(row,yellow);
			break;
		case 4:
			setRowBackgroundBuyParts(row,yellowlight);
			break;
		case 5:
			setRowBackgroundBuyParts(row,green);
			break;
		}
	}
	buyparts->Refresh();
}

void Maintenance::setBuyPartsPriority(wxGrid *grid ,int row, int p, int t)
{
	wxString priority = grid->GetCellValue(row,p);
	for(int i = 0; i < buyparts->GetNumberRows(); i++)
	{
		if(buyparts->GetCellValue(i,TITLE).Trim() == grid->GetCellValue(row,t).Trim())
			buyparts->SetCellValue(i,PRIORITY,grid->GetCellValue(row,p));
	}
}

void Maintenance::setRowDone(int row)
{
	wxString g = grid->GetCellValue(selectedRow,IF);
	int choice = -1;

	if(g == m_choices[0])
			choice = 0;
	else if (g == m_choices[1])
			choice = 1;
	else if (g == m_choices[2])
			choice = 2;

	switch(choice)
	{
	case 0:
		grid->SetCellValue(selectedRow,START,
		  			   dialog->m_gridGlobal->GetCellValue(
					   dialog->m_gridGlobal->GetNumberRows()-1,6));
		break;
	case 1:
		grid->SetCellValue(selectedRow,START,
			dialog->m_gridMotorSails->GetCellValue(
					   dialog->m_gridGlobal->GetNumberRows()-1,1));
		break;
	case 2:
		break;
	}

	if(grid->GetCellValue(row,ACTIVE) == _("Yes"))
		setRowBackground(row,green);
	else
		setRowBackground(row,green);
	grid->Refresh();
}

void Maintenance::setRepairDone(int row)
{
	repairs->SetCellValue(row,RPRIORITY,_("0"));
	setRowBackgroundRepairs(row, white);
}

void Maintenance::setRowBackgroundRepairs(int row, wxColour &c)
{
	for(int i = 0; i < repairs->GetNumberCols(); i++)
		repairs->SetCellBackgroundColour(row,i,c);
}

void Maintenance::setRowBackgroundBuyParts(int row, wxColour &c)
{
	for(int i = 0; i < buyparts->GetNumberCols(); i++)
		buyparts->SetCellBackgroundColour(row,i,c);
}

void Maintenance::cellCollChanged(int col, int row)
{
	if(col == IF)
	{
		wxString g = grid->GetCellValue(selectedRow,IF);

		if(g == m_choices[0])
					grid->SetCellValue(selectedRow,START,
							dialog->m_gridGlobal->GetCellValue(
							dialog->m_gridGlobal->GetNumberRows()-1,6));
		else if (g == m_choices[1])
						grid->SetCellValue(selectedRow,START,
							dialog->m_gridMotorSails->GetCellValue(
							dialog->m_gridMotorSails->GetNumberRows()-1,1));
		else if (g == m_choices[2])
						grid->SetCellValue(selectedRow,WARN,
							dialog->m_gridGlobal->GetCellValue(
							dialog->m_gridGlobal->GetNumberRows()-1,3));

		col = WARN;
	}

	if(col == WARN || col == URGENT)
	{
		wxString ss;

		wxString g = grid->GetCellValue(row,IF);

		if(g == m_choices[0] || g == m_choices[1])
		{
			wxString s = grid->GetCellValue(row,START);
			s = s.substr(s.find_last_of(' '));
			wxDouble d; 
			grid->GetCellValue(row,col).ToDouble(&d);
			ss = wxString::Format(_T("%5.0f %s"),d,s.c_str());
		}
		else 
		{
			ss = grid->GetCellValue(row,WARN);
		}
		if(col == WARN)
		{
			grid->SetCellValue(row,WARN,ss);
			grid->SetCellValue(row,URGENT,ss);
		}
		else
			grid->SetCellValue(row,URGENT,ss);
	}

	if(col == ACTIVE)
	{
		if(grid->GetCellValue(row,ACTIVE) == m_YesNo[0] && 
			grid->GetCellValue(row,PRIORITY) == _T("0"))
			grid->SetCellValue(row,PRIORITY,_T("5"));
//		checkService(dialog->m_gridGlobal->GetNumberRows()-1);
		setBuyPartsPriority(grid ,row, PRIORITY, TEXT);
//		checkBuyParts();
	}
}

void Maintenance::cellSelected(int col, int row)
{
	selectedCol = col;
	selectedRow = row;
}

void Maintenance::update()
{
//	if(!modified) return;

	wxString s = _T(""), temp;

	wxString newLocn = data_locn;
	newLocn.Replace(_T("txt"),_T("Bak"));
	wxRename(data_locn,newLocn);

	wxFileOutputStream output( data_locn );
	wxTextOutputStream* stream = new wxTextOutputStream (output);

	int count = grid->GetNumberRows();
	for(int r = 0; r < count; r++)
	{
		for(int c = 0; c < grid->GetNumberCols(); c++)
			{
				temp = grid->GetCellValue(r,c);
				s += dialog->replaceDangerChar(temp);
				s += _T(" \t");
			}
		s.RemoveLast();
		stream->WriteString(s+_T("\n"));
		s = _T("");
	}
	output.Close();
}

void Maintenance::updateRepairs()
{
//	if(!modified) return;
	wxString s = _T(""), temp;

	wxString newLocn = data_locnRepairs;
	newLocn.Replace(_T("txt"),_T("Bak"));
	wxRename(data_locnRepairs,newLocn);

	wxFileOutputStream output( data_locnRepairs );
	wxTextOutputStream* stream = new wxTextOutputStream (output);

	int count = dialog->m_gridMaintanenceRepairs->GetNumberRows();
	for(int r = 0; r < count; r++)
	{
		for(int c = 0; c < dialog->m_gridMaintanenceRepairs->GetNumberCols(); c++)
			{
				temp = dialog->m_gridMaintanenceRepairs->GetCellValue(r,c);
				s += dialog->replaceDangerChar(temp);
				s += _T(" \t");
			}
		s.RemoveLast();
		stream->WriteString(s+_T("\n"));
		s = _T("");
	}
	output.Close();
}

void Maintenance::updateBuyParts()
{
//	if(!modified) return;
	wxString s = _T(""), temp;

	wxString newLocn = data_locnBuyParts;
	newLocn.Replace(_T("txt"),_T("Bak"));
	wxRename(data_locnBuyParts,newLocn);

	wxFileOutputStream output( data_locnBuyParts );
	wxTextOutputStream* stream = new wxTextOutputStream (output);

	int count = dialog->m_gridMaintenanceBuyParts->GetNumberRows();
	for(int r = 0; r < count; r++)
	{
		for(int c = 0; c < dialog->m_gridMaintenanceBuyParts->GetNumberCols(); c++)
			{
				temp = dialog->m_gridMaintenanceBuyParts->GetCellValue(r,c);
				s += dialog->replaceDangerChar(temp);
				s += _T(" \t");
			}
		s.RemoveLast();
		stream->WriteString(s+_T("\n"));
		s = _T("");
	}
	output.Close();
}

void Maintenance::viewODT(int tab,wxString path,wxString layout,bool mode)
{
	wxString locn, fn;

	if(tab == dialog->SERVICE)
	  {
	    locn = layout_locnService;
	    fn = data_locn;
	  }
	else if(tab == dialog->REPAIRS)
	{
	    locn = layout_locnRepairs;
	    fn = data_locnRepairs;
	}
	else if(tab == dialog->BUYPARTS)
	{
	    locn = this->layout_locnBuyParts;
	    fn = data_locnBuyParts;
	}

	toODT(tab,locn, layout, mode);

	if(layout != _T(""))
	{
	    fn.Replace(_T("txt"),_T("odt"));
		dialog->startApplication(fn,_T(".odt"));
	}
}

void Maintenance::viewHTML(int tab,wxString path,wxString layout,bool mode)
{
	wxString locn, fn;

	if(tab == dialog->SERVICE)
	  {
	    locn = layout_locnService;
	    fn = data_locn;
	  }
	else if(tab == dialog->REPAIRS)
	{
	    locn = layout_locnRepairs;
	    fn = data_locnRepairs;
	}
	else if(tab == dialog->BUYPARTS)
	{
	    locn = this->layout_locnBuyParts;
	    fn = data_locnBuyParts;
	}

	toHTML(tab,locn, layout, mode);

	if(layout != _T(""))
	{
	    fn.Replace(_T("txt"),_T("html"));
		dialog->startBrowser(fn);
	}
}

wxString Maintenance::toHTML(int tab,wxString path,wxString layout,bool mode)
{
	wxArrayInt arrayRows;
	int selCount = 0;
	bool selection = false;

	selCount = dialog->m_gridGlobal->GetSelectedRows().Count() ;

	if(selCount > 0)
	{
		selection = true;
		arrayRows = dialog->m_gridGlobal->GetSelectedRows();
	}

	if(layout == _T(""))
	{
		wxMessageBox(_("Sorry, no Layout installed"),_("Information"),wxOK);
		return _T("");
	}

	update();

	wxString html = readLayoutHTML(path,layout);

	wxString topHTML;
	wxString bottomHTML;
	wxString middleHTML;

	wxString seperatorTop = _T("<!-- Repeat -->");
	wxString seperatorBottom = _T("<!-- Repeat End -->");

	int indexTop = html.Find(seperatorTop);
	indexTop += seperatorTop.Len();
	int indexBottom = html.Find(seperatorBottom);
	indexBottom += seperatorBottom.Len();

	topHTML = html.substr(0,indexTop);
	bottomHTML = html.substr(indexBottom);
	middleHTML = html.substr(indexTop,(indexBottom-indexTop)-seperatorBottom.Len());

	wxString layout_loc;
	wxGrid * grid = NULL;
	if(tab == dialog->SERVICE)
	{
		path = data_locn;
		layout_loc = layout_locnService;
		grid = this->grid;
	}
	else if(tab == dialog->REPAIRS)
	{

		path = data_locnRepairs;
		layout_loc = layout_locnRepairs;
		grid = repairs;
	}
	else if(tab == dialog->BUYPARTS)
	{
		path = this->data_locnBuyParts;
		layout_loc = layout_locnBuyParts;
		grid = buyparts;
	}

	wxString filename = path;

//	wxTextFile *logFile = new wxTextFile(path);
	if(mode == 0)
		path.Replace(wxT("txt"),wxT("html"));
//	else 
///		path = savePath;

	if(::wxFileExists(path))
		::wxRemoveFile(path);

	wxFileInputStream input( path );
//	wxTextInputStream* stream = new wxTextInputStream (input);
	
	wxFileOutputStream output( path );
	wxTextOutputStream htmlFile(output);

	topHTML.Replace(wxT("#TYPE#"),dialog->boatType->GetValue());
	topHTML.Replace(wxT("#BOATNAME#"),dialog->boatName->GetValue());
	topHTML.Replace(wxT("#HOMEPORT#"),dialog->homeport->GetValue());
	topHTML.Replace(wxT("#CALLSIGN#"),dialog->callsign->GetValue());
	topHTML.Replace(wxT("#REGISTRATION#"),dialog->registration->GetValue());
	topHTML.Replace(wxT("#LOCATION#"),layout_locn + layout + _T(".html"));

	htmlFile << topHTML;

	wxString newMiddleHTML;
	if(html.Contains(seperatorTop))
	{
		for(int row = 0; row < grid->GetNumberRows(); row++)
		{
		  if(tab == dialog->SERVICE)
		      newMiddleHTML = setPlaceHoldersService(mode, grid, row, middleHTML);
		  else if(tab == dialog->REPAIRS)
		      newMiddleHTML = setPlaceHoldersRepairs(mode, repairs, row, middleHTML);
		  else if(tab == dialog->BUYPARTS)		  
		      newMiddleHTML = setPlaceHoldersBuyParts(mode, buyparts, row, middleHTML);
			
		  htmlFile << newMiddleHTML;
		}
	}
	htmlFile << bottomHTML;

	output.Close();
	return path;
}

wxString Maintenance::toODT(int tab,wxString path,wxString layout,bool mode)
{
	wxString s, odt;

	if(layout == _T(""))
	{
		wxMessageBox(_("Sorry, no Layout installed"),_("Information"),wxOK);
		return _T("");
	}

	update();

	odt = readLayoutODT(path,layout);

	int indexTop;
	int indexBottom;
	wxString topODT;
	wxString bottomODT;
	wxString middleODT;

	wxString middleData = _T("");

	wxString seperatorTop        = wxT("[[");
	wxString seperatorBottom     = wxT("]]");

	if(odt.Contains(seperatorTop))
	{
		indexTop    = odt.Find(seperatorTop);
		indexBottom = odt.Find(seperatorBottom);
		topODT				= odt.substr(0,indexTop);
		topODT				= topODT.substr(0,topODT.find_last_of('<'));
		bottomODT			= odt.substr(indexBottom+1);
		bottomODT			= bottomODT.substr(bottomODT.find_first_of('>')+1);
		middleODT			= odt.substr(indexTop+11);
		middleODT			= middleODT.substr(0,middleODT.Find(seperatorBottom));
		middleODT			= middleODT.substr(0,middleODT.find_last_of('<'));
	}
	else
		return _("");

	wxString layout_loc;
	wxGrid * grid = NULL;
	if(tab == dialog->SERVICE)
	{
		path = data_locn;
		layout_loc = layout_locnService;
		grid = this->grid;
	}
	else if(tab == dialog->REPAIRS)
	{

		path = data_locnRepairs;
		layout_loc = layout_locnRepairs;
		grid = repairs;
	}
	else if(tab == dialog->BUYPARTS)
	{
		path = this->data_locnBuyParts;
		layout_loc = layout_locnBuyParts;
		grid = buyparts;
	}

	wxTextFile *logFile = new wxTextFile(path);
	if(mode == 0)
		path.Replace(wxT("txt"),wxT("odt"));
//	else 
///		path = savePath;

	if(::wxFileExists(path))
		::wxRemoveFile(path);

	auto_ptr<wxFFileInputStream> in(new wxFFileInputStream(layout_loc + layout + _T(".odt")));
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

	wxString newMiddleODT;
	if(odt.Contains(seperatorTop))
	{
		for(int row = 0; row < grid->GetNumberRows(); row++)
		{
		  if(tab == dialog->SERVICE)
		      newMiddleODT = setPlaceHoldersService(mode,grid, row, middleODT);
		  else if(tab == dialog->REPAIRS)
		      newMiddleODT = setPlaceHoldersRepairs(mode,repairs, row, middleODT);
		  else if(tab == dialog->BUYPARTS)		  
		      newMiddleODT = setPlaceHoldersBuyParts(mode,buyparts, row, middleODT);
		
		  if(mode == 0) // HTML
		      newMiddleODT.Replace(wxT("\n"),wxT("<br>"));
		  else // ODT
		    newMiddleODT.Replace(wxT("\n"),wxT("<text:line-break/>"));
		
		odtFile << newMiddleODT;
		}
	}

	odtFile << bottomODT;

	inzip.Eof() && outzip.Close() && out.Commit();
	logFile->Close();

	return path;
}

wxString Maintenance::setPlaceHoldersService(bool mode, wxGrid *grid, int row, wxString middleODT)
{
	wxString newMiddleODT;

	newMiddleODT = middleODT;
	newMiddleODT.Replace(wxT("#PRIORITY#"),replaceNewLine(mode,grid->GetCellValue(row,PRIORITY)));
	newMiddleODT.Replace(wxT("#TEXT#"),replaceNewLine(mode,grid->GetCellValue(row,TEXT)));
	newMiddleODT.Replace(wxT("#IF#"),replaceNewLine(mode,grid->GetCellValue(row,IF)));
	newMiddleODT.Replace(wxT("#WARN#"),replaceNewLine(mode,grid->GetCellValue(row,WARN)));
	newMiddleODT.Replace(wxT("#URGENT#"),replaceNewLine(mode,grid->GetCellValue(row,URGENT)));
	newMiddleODT.Replace(wxT("#START#"),replaceNewLine(mode,grid->GetCellValue(row,START)));
	//newMiddleODT.Replace(wxT("#END#"),grid->GetCellValue(row,PRIORITY);
	newMiddleODT.Replace(wxT("#ACTIVE#"),replaceNewLine(mode,grid->GetCellValue(row,ACTIVE)));

	return newMiddleODT;
}

wxString Maintenance::setPlaceHoldersRepairs(bool mode, wxGrid *grid, int row, wxString middleODT)
{
	wxString newMiddleODT;

	newMiddleODT = middleODT;
	newMiddleODT.Replace(wxT("#PRIORITY#"),replaceNewLine(mode,grid->GetCellValue(row,RPRIORITY)));
	newMiddleODT.Replace(wxT("#REPAIRSTEXT#"),replaceNewLine(mode,grid->GetCellValue(row,RTEXT)));

	return newMiddleODT;
}

wxString Maintenance::setPlaceHoldersBuyParts(bool mode, wxGrid *grid, int row, wxString middleODT)
{
	wxString newMiddleODT;

	newMiddleODT = middleODT;

	newMiddleODT.Replace(wxT("#PRIORITY#"),replaceNewLine(mode,grid->GetCellValue(row,PPRIORITY)));
	newMiddleODT.Replace(wxT("#CATEGORY#"),replaceNewLine(mode,grid->GetCellValue(row,PCATEGORY)));
	newMiddleODT.Replace(wxT("#TITLE#"),replaceNewLine(mode,grid->GetCellValue(row,TITLE)));
	newMiddleODT.Replace(wxT("#BUYPARTS#"),replaceNewLine(mode,grid->GetCellValue(row,PARTS)));
	newMiddleODT.Replace(wxT("#DATE#"),replaceNewLine(mode,grid->GetCellValue(row,DATE)));
	newMiddleODT.Replace(wxT("#AT#"),replaceNewLine(mode,grid->GetCellValue(row,AT)));

	return newMiddleODT;
}

wxString Maintenance::replaceNewLine(bool mode, wxString str)
{
	if(mode == 0) // HTML
		 str.Replace(wxT("\n"),wxT("<br>"));
	else // ODT
		 str.Replace(wxT("\n"),wxT("<text:line-break/>"));

	return str;
}

void Maintenance::viewODTRepairs(wxString path,wxString layout,bool mode)
{
	toODTRepairs(path, layout, mode);
	if(layout != _T(""))
	{
	    wxString fn = data_locn;
	    fn.Replace(_T("txt"),_T("odt"));
		dialog->startApplication(fn,_T(".odt"));
	}
}

wxString Maintenance::toODTRepairs(wxString path,wxString layout,bool mode)
{

return _("");
}

wxString Maintenance::readLayoutODT(wxString path,wxString layout)
{
	wxString odt = _T("");


	wxString filename = path + layout + _T(".odt");

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

wxString Maintenance::readLayoutHTML(wxString path1,wxString layoutFileName)
{
	wxString html, path;

	path = path1 + layoutFileName + wxT(".html");;
	wxTextFile layout(path);
	
	layout.Open();

	for(unsigned int i = 0; i < layout.GetLineCount(); i++)
	{
		html += layout.GetLine(i)+_T("\n");
	}

	layout.Close();

	return html;
}