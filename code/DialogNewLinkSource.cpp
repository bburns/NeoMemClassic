
// CDialogNewLinkSource



#include "precompiled.h"

#include "DialogNewLinkSource.h"
#include "DialogEditName.h"
#include "NeoMem.h"
#include "Constants.h"
#include "HelpIDs.h"

#include "ConstantsDatabase.h"
#include "BDoc.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




BEGIN_MESSAGE_MAP(CDialogNewLinkSource, CDialog)
	//{{AFX_MSG_MAP(CDialogNewLinkSource)
	ON_BN_CLICKED(IDC_BTN_NEW_CLASS, OnBtnNewClass)
	ON_CBN_SELCHANGE(IDC_CBO_CLASS, OnSelChangeCboClass)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



CDialogNewLinkSource::CDialogNewLinkSource(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogNewLinkSource::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogNewLinkSource)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}




BOOL CDialogNewLinkSource::OnInitDialog() 
{
	// Call base class (calls DoDataExchange)
	CDialog::OnInitDialog();

	// Get document
	m_pDoc = BDoc::GetDoc();

	// Set bold font
	m_lblClass.SetFont(&app.m_fontControlsBold);
	m_lblFolder.SetFont(&app.m_fontControlsBold);
	
	// Fill combo with classes
	m_cboClass.SetImageList(m_pDoc->GetImageList());
	BObject* pobjClassFolder = m_pDoc->GetObject(rootClass);
	m_cboClass.AddObjects(pobjClassFolder, app.m_lngExcludeFlags, FALSE, TRUE);
	
	// Select default class (paper)
	BObject* pobjDefaultClass = m_pDoc->GetObject(classPaper);
	m_cboClass.SelectItemData((DWORD) pobjDefaultClass);

	m_txtFolderName.SetWindowText("New Folder");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CDialogNewLinkSource::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogNewLinkSource)
	DDX_Control(pDX, IDC_BTN_HELP, m_btnHelp);
	DDX_Control(pDX, IDC_TXT_FOLDER_NAME, m_txtFolderName);
	DDX_Control(pDX, IDC_LBL_FOLDER, m_lblFolder);
	DDX_Control(pDX, IDC_LBL_CLASS, m_lblClass);
	DDX_Control(pDX, IDC_BTN_NEW_CLASS, m_btnNewClass);
	DDX_Control(pDX, IDC_CBO_CLASS, m_cboClass);
	//}}AFX_DATA_MAP
	
	if (!pDX->m_bSaveAndValidate) // Load
	{
	}
	else // Save
	{
		// Get class
		BObject* pobjClass = (BObject*) m_cboClass.GetSelectedItemData();
		ASSERT_VALID(pobjClass);
		m_lngClassID = pobjClass->GetObjectID();

		// Get folder name
		m_txtFolderName.GetWindowText(m_strFolderName);
	}

}




// Add a new class (just need name really)
void CDialogNewLinkSource::OnBtnNewClass() 
{
	CDialogEditName dlg;
	dlg.m_strCaption = _T("Add Link Class");
	dlg.m_strInstructions = _T("Enter the name for the new link class:");
	dlg.m_strName = _T("New Link Class");
	if (dlg.DoModal() == IDOK)
	{
		// Create the new class
//		CString strClassName = dlg.m_strName;
//		HOBJECT hobjNewClass = m_pDoc->CreateClass(strClassName);

		BObject& objClass = BObject::New(*m_pDoc, classClass, dlg.m_strName, rootClass);

//,		objClass.SetPropertyString(propDescription, strDescription, FALSE, FALSE);

		// Add to combo and select it
		//. test
		BObject* pobjParent = objClass.GetParent();
		int nItem = m_cboClass.AddObject(&objClass, pobjParent);
		m_cboClass.SetCurSel(nItem);
	}

	// Set focus back to combo
	m_cboClass.SetFocus();
}



// Different class selected - modify the folder name if not already modified by user.
// Eg for class Category, folder name would be Categorys (just add an s to the end).
// or just let user type it all in themselves for now
void CDialogNewLinkSource::OnSelChangeCboClass() 
{
}




// Bring up help for this dialog
void CDialogNewLinkSource::OnBtnHelp() 
{
	app.WinHelp(HID_BASE_RESOURCE + IDD_NEW_LINK_SOURCE);
}


void CDialogNewLinkSource::OnOK() 
{
	m_txtFolderName.GetWindowText(m_strFolderName);
	m_strFolderName.TrimLeft();
	if (m_strFolderName.IsEmpty())
	{
		AfxMessageBox("Please enter a name for the new folder.", MB_ICONINFORMATION);
		m_txtFolderName.SetFocus();
	}
	else
	{
		CDialog::OnOK();
	}
}

