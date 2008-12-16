
// CPageClassType



#include "precompiled.h"

#include "NeoMem.h"
#include "PageClassType.h"

#include "ConstantsDatabase.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CPageClassType, CPropertyPage)

BEGIN_MESSAGE_MAP(CPageClassType, CPropertyPage)
	//{{AFX_MSG_MAP(CPageClassType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



CPageClassType::CPageClassType() : CPropertyPage(CPageClassType::IDD)
{
	//{{AFX_DATA_INIT(CPageClassType)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPageClassType::~CPageClassType()
{
}




BOOL CPageClassType::OnInitDialog() 
{
	// Get document and parent sheet
	m_pDoc = CNeoDoc::GetDoc();
	ASSERT_VALID(m_pDoc);
	m_psh = (CSheetWizard*) GetParent();
	ASSERT_VALID(m_psh);

	// Call base class (calls DoDataExchange)
	CPropertyPage::OnInitDialog();

	// Set font for title
	m_lblTitle.SetFont(&theApp.m_fontControlsBold);

	// Set context sensitive help
	SetHelpID(IDD_CLASS_TYPE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPageClassType::DoDataExchange(CDataExchange* pDX)
{
	// Get temp object we're working with
	m_pobj = m_psh->m_pobj;
	ASSERT_VALID(m_pobj);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageClassType)
	DDX_Control(pDX, IDC_CBO_NAME_TYPE, m_cboNameType);
	DDX_Control(pDX, IDC_LBL_INSTRUCTIONS, m_lblInstructions);
	DDX_Control(pDX, IDC_LBL_TITLE, m_lblTitle);
	//}}AFX_DATA_MAP

	if (!pDX->m_bSaveAndValidate) // Load
	{
		ULONG lngPropTypeID = proptypeString; // default is regular string
		BObject* pobjPropType = m_pobj->GetPropertyLink(propObjectNamePropertyType);
		if (pobjPropType)
		{
			lngPropTypeID = pobjPropType->m_lngObjectID;
		}
		// Get combo box index based on property type
//		int nSel = (lngPropTypeID == proptypeString ? 0 : 1);
		int nSel = 0;
		switch (lngPropTypeID)
		{
			case proptypeString: nSel = 0; break;
			case proptypePersonName: nSel = 1; break;
			case proptypeDate: nSel = 2; break;
		}
		m_cboNameType.SetCurSel(nSel);
	}
	else // Save
	{
		int nSel = m_cboNameType.GetCurSel();
//		ULONG lngPropTypeID = (nSel == 0 ? proptypeString : proptypePersonName);
		ULONG lngPropTypeID = proptypeString;
		switch (nSel)
		{
			case 0: lngPropTypeID = proptypeString; break;
			case 1: lngPropTypeID = proptypePersonName; break;
			case 2: lngPropTypeID = proptypeDate; break;
		}
		BObject* pobjPropType = m_pDoc->GetObject(lngPropTypeID);
		m_pobj->SetPropertyLink(propObjectNamePropertyType, pobjPropType, FALSE, FALSE);
	}

}



BOOL CPageClassType::OnSetActive() 
{
	CSheetWizard* psheet = (CSheetWizard*) GetParent();
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	psheet->SetClassWizardTitle();
	return CPropertyPage::OnSetActive();
}


