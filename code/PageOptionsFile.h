
// CPageOptionsFile
// File tab in the Options dialog.
//-----------------------------------------------------------------------------------------------------------------




#pragma once


class CPageOptionsFile : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageOptionsFile)

// Construction
public:
	CPageOptionsFile();
	~CPageOptionsFile();

// Public Attributes
public:
	BOOL	m_bAutoStart;
	BOOL	m_bAutoBackup;
	BOOL	m_bAutoRecover;
	UINT	m_nAutoRecoverMinutes;
	BOOL	m_bAutoLoad;
//	CString	m_strStartupFile;
	CString m_strCurrentFile;
	CString	m_strDocumentFolder;


// Dialog Data
private: 
	//{{AFX_DATA(CPageOptionsFile)
	enum { IDD = IDD_OPTIONS_FILE };
	CEdit	m_txtAutoRecover;
	CSpinButtonCtrl	m_spinAutoRecover;
	CStatic	m_lblAutoRecover2;
	CStatic	m_lblAutoRecover;
	CStatic	m_lblAutoBackup;
	CButton	m_chkAutoRecover;
	CButton	m_chkAutoBackup;
	CComboBox	m_cboAutoBackup;
	CButton	m_btnUseCurrent;
	CStatic	m_lblAutoStart;
	CButton	m_chkAutoStart;
	CStatic	m_lblStartup;
	CStatic	m_lblFolder;
	CEdit	m_txtFolder;
	CButton	m_chkStartup;
	CButton	m_btnBrowseStartup;
	CButton	m_btnBrowseFolder;
//	CEdit	m_txtAutoLoadFileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageOptionsFile)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnApply();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	BOOL Validate();
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageOptionsFile)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillFocusTxtFolder();
	afx_msg void OnBtnBrowse();
	afx_msg void OnBtnBrowseStartup();
	afx_msg void OnChkStartup();
	afx_msg void OnKillfocusTxtStartup();
	afx_msg void OnBtnUseCurrent();
	afx_msg void OnChkAutoRecover();
	afx_msg void OnChkAutoBackup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

