
// CPageClassType
// Type page in the Class Wizard
//-----------------------------------------------------------------------------------------------------------------



#pragma once



#include "SheetWizard.h"

#include "BDoc.h"
#include "BObject.h"



class CPageClassType : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageClassType)

// Construction
public:
	CPageClassType();
	~CPageClassType();

// Dialog Data
private:
	//{{AFX_DATA(CPageClassType)
	enum { IDD = IDD_CLASS_TYPE };
	CComboBox	m_cboNameType;
	CStatic	m_lblInstructions;
	CStatic	m_lblTitle;
	//}}AFX_DATA

private:
	BDoc* m_pDoc;
	CSheetWizard* m_psh;
	BObject* m_pobj;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageClassType)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageClassType)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

