


#include "precompiled.h"
#include "neomem.h"
#include "Error.h"

#include "Path.h"
using namespace nsPath;



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



// "Any CException-derived class that you create should use the 
// IMPLEMENT_DYNAMIC macro, too."
IMPLEMENT_DYNAMIC(CError, CException)


// You can report details about exceptions to the user by calling 
// GetErrorMessage or ReportError, two member functions that work with any 
// of CException's derived classes.


CError::CError(BOOL bAutoDelete)
{
//	CException::CException(bAutoDelete); // worked before vs2010, now says can't call abstract class
	m_bSevere = TRUE; 
}

CError::~CError()
{
}




void CError::SetName(CString strName) 
{
	m_strError = strName;
}


void CError::SetSeverity(BOOL bSevere) 
{
	m_bSevere = bSevere;
}


/*
// could override this...
BOOL CError::GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext)
{
	// Copy error message to provided buffer
//	_tcsncpy(lpszError, m_pszError, nMaxError);
//	return TRUE;
	return FALSE; // no message text available
}
*/





CString CError::GetDialogText()
{
	// this text is displayed in the error dialog

	CString str;
	str.Format(		"Error: %s\r\n"
					"Filename: %s\r\n"
					"Line: %d\r\n"
					"Expression: %s \r\n"
					"\r\n"
					"%s" // file saved information
					"\r\n",
					(LPCTSTR) m_strError,
					(LPCTSTR) m_strFiletitle,
					m_nLine,
					(LPCTSTR) m_strExpression,
					(LPCTSTR) m_strFilesSaved
					);
	return str;
}



CString CError::GetReportSubject()
{
	CString str;
	str.Format(
		_T("NeoMem Error - %s in %s, line %i, %s"), 
		(LPCTSTR) m_strError, 
		(LPCTSTR) m_strFiletitle,
		m_nLine,
		(LPCTSTR) m_strExpression
		);
	return str;
}


CString CError::GetReportText()
{
	// this text is used for the email

	// Create the text for the email message
	CString str;

	// the text is in neomemCode format, so can be parsed into a bug item easily.
	str.Format(
					"\r\n"
					"Name: %s\r\n" // error message & location
					"Description: %s\r\n" // user description
					"Project: neomemLite \r\n"
					"Found in version: %s %s \r\n" // version and build
					"Filename: %s \r\n"
					"Line: %i \r\n"
					"Expression: %s \r\n"
					"\r\n"
					"System Information: \r\n"
//					"%s \r\n" // build info
					"Processor: %s \r\n" // processor
					"OS: %s \r\n" // windows version
					"Common Controls Version: %.2f \r\n" // common controls version
					"RichEdit Control Version: %s \r\n" // rich edit version
					"Virtual Memory: %s \r\n" // memory info
					"Physical Memory: %s \r\n" // memory info
					"Screen: %s \r\n" // screen info
					"Locale: %s \r\n" // locale info
					"\r\n\r\n\r\n\r\n"
					,
					(LPCTSTR) m_strError,
					(LPCTSTR) m_strDescription,
					(LPCTSTR) app.m_objInfo.GetProgramVersion(),
					(LPCTSTR) app.m_objInfo.GetProgramBuild(),
					(LPCTSTR) m_strFiletitle,
					m_nLine,
					(LPCTSTR) m_strExpression,

					(LPCTSTR) app.m_objInfo.GetProcessor(),
					(LPCTSTR) app.m_objInfo.GetWindowsVersion(),
					app.m_objInfo.GetCommonControlsVersionNumber(),
					(LPCTSTR) app.m_objInfo.GetRichEditVersionString(),
					(LPCTSTR) app.m_objInfo.GetVirtualMemory(),
					(LPCTSTR) app.m_objInfo.GetPhysicalMemory(),
					(LPCTSTR) app.m_objInfo.GetScreenInfo(),
					(LPCTSTR) app.m_objInfo.GetLocaleInfo()
					);
	return str;
}





void CError::SetFileLocation(CString strFilename, int nLine, CString strExpression)
{
	m_strFilename = strFilename;
	m_nLine = nLine;
	m_strExpression = strExpression;
	// get file title also
	CPath path = CPath(strFilename);
	m_strFiletitle = path.GetTitleExtension();
}



// save info about windows message
void CError::SetMsg(const MSG *pMsg)
{	
	// wrap it in a CWinMsg object
	m_winmsg.SetMsg(pMsg);
}

void CError::SetDescription(CString str)
{
	m_strDescription = str;
}


