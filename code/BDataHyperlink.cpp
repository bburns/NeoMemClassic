
// BDataHyperlink


#include "precompiled.h"
#include "BDataHyperlink.h"
#include "ConstantsDatabase.h"
#include "BDoc.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



IMPLEMENT_SERIAL(BDataHyperlink, BData, VERSIONABLE_SCHEMA | versionFileStructure) // last parameter is version number


// Construction/Destruction
//------------------------------------------------------------------------------------------------------

BDataHyperlink::BDataHyperlink()
{
}

BDataHyperlink::~BDataHyperlink()
{
}



BOOL BDataHyperlink::SetBDataText(const CString& str, BObject* pobjPropertyDef /* = 0 */, BOOL bShowErrorMessage /* = TRUE */)
{
	ASSERT_VALID(this);
	m_strText = str;
	return TRUE;
}



CString BDataHyperlink::GetBDataText(BDoc* pDoc, ULONG lngPropertyID, BOOL bMachineVersion)
{
	ASSERT_VALID(this);
	return m_strText;
}



ULONG BDataHyperlink::GetMemoryUsed(BOOL bRecursive)
{
	ASSERT_VALID(this);
	ULONG nBytes = sizeof(BDataHyperlink);
	nBytes += m_strText.GetLength() * sizeof(TCHAR);
	return nBytes;
}



void BDataHyperlink::Serialize(CArchive &ar)
{
	// Call base class first
	BData::Serialize(ar);

	// serialize the string
	if (ar.IsStoring())
	{
		ar << m_strText;
	}
	else
	{
		ar >> m_strText;
	}
}




// Bring up dialog to edit string value.
// Updates value and returns TRUE if user hit OK in dialog.
BOOL BDataHyperlink::UIEditValue(CUI& ui, BObject* pobj, BObject* pobjPropertyDef)
{
	ASSERT_VALID(this);
	// pobj and pobjpd can be zero
	
	//, will want to say the object we're editing and the property name
	// Object: System
	// Property: Description
	// eg Enter the new value for the "Description" property of the object "System".
	
	//, for most string properties, we should use cdialogeditname
	// for description, etc. use the bigger one.
	// for now leave like this

	//, need different types of hyperlinks - websites, emails, files, folders
	// have proptype=hyperlink but need propsubtype=website,email,file,folder, etc
	// (add as another property of propertydefs?)
	//		websites - edit string
	//		emails - edit string
	//		files - bring up file open dialog
	//		folders - bring up folder dialog
	//, eg
//	ULONG lngPropertySubtypeID = pobjPropertyDef->GetPropertyLong(propPropertySubtype);
//	switch (lngPropertySubtypeID)
//	{
//		case propsubtypeEmail:
//		case propsubtypeWebsite:
//		case propsubtypeFile:
//		case propsubtypeFolder:
//	}
//	note: pobjPropertyDef->m_lngClassID = 4 (classProperty)


//x
/*	CDialogEditString dlg;
	dlg.m_strValue = m_strText;
	if (dlg.DoModal() == IDOK)
	{
		// Save new string value
		m_strText = dlg.m_strValue;
		return TRUE;
	}
	return FALSE;
*/
	// okay to edit mstrtext directly because it IS the underlying representation
	return ui.EditString(m_strText);
}



// Create a copy of this object
BData* BDataHyperlink::CreateCopy()
{
	ASSERT_VALID(this);
	BDataHyperlink* pdatCopy = new BDataHyperlink();
	ASSERT_VALID(pdatCopy);
	pdatCopy->m_strText = m_strText;
	return pdatCopy;
}




// Returns True if can find any references to the specified object in this bdata
BOOL BDataHyperlink::FindReferences(BObject* pobjFind)
{
	return FALSE;
}







// Returns true if this hyperlink is an email address (false if it's a website or blank)
BOOL BDataHyperlink::IsEmail()
{
	int nPos = m_strText.Find('@');
	return (nPos != -1);
}



BOOL BDataHyperlink::IsWebsite()
{
	//, check for valid website characters
	if (IsEmail()) 
		return FALSE;
	int nPos = m_strText.Find('.');
	return (nPos != -1);
}



//,, move to ui
void BDataHyperlink::UIOnClick()
{
	// See if we have an email address or website address
	// Send email to someone@somewhere.org? yes/no
	// Open website www.somewhere.org? yes/no
	CString strMsg;
	if (IsEmail())
	{
		strMsg.Format("Send email to %s?", (LPCTSTR) m_strText);
		if (IDYES == AfxMessageBox(strMsg, MB_ICONQUESTION + MB_YESNO))
		{
			CString strURL;
			if (_tcsncmp((LPCTSTR) m_strText, _T("mailto:"), 7) == 0)
				strURL = m_strText;
			else
				strURL.Format("mailto:%s", (LPCTSTR) m_strText);
			CWaitCursor wc;
			HINSTANCE h = ::ShellExecute(NULL, "open", strURL, NULL, NULL, SW_SHOWNORMAL);			
			Library::HandleShellExecuteError(h);
		}
	}
	else
	{
		strMsg.Format("Open the website %s?", (LPCTSTR) m_strText);
		if (IDYES == AfxMessageBox(strMsg, MB_ICONQUESTION + MB_YESNO))
		{
			CString strURL;
			if (_tcsncmp((LPCTSTR) m_strText, _T("http://"), 7) == 0)
				strURL = m_strText;
			else
				strURL.Format("http://%s", (LPCTSTR) m_strText);
			CWaitCursor wc;
			HINSTANCE h = ::ShellExecute(NULL, "open", strURL, NULL, NULL, SW_SHOWNORMAL);
			Library::HandleShellExecuteError(h);
		}
	}
}



int BDataHyperlink::UIGetCursor() {
	return 1; // hand
}


// Add menu items for bdata value popup
CStringArray& BDataHyperlink::UIGetCommands() {
	static CStringArray cmds; // static so can return reference to it
	cmds.RemoveAll();
	if (IsEmail())
		cmds.Add("&Send Email...");
	else
		cmds.Add("&Open Website...");
	return cmds;
}


BOOL BDataHyperlink::UIHandleCommand(UINT nCommandID) {
	if (nCommandID == 0) { // send email / open website
		UIOnClick();
		return TRUE;
	}
	return FALSE;
}



