
// CSheetWizard



#include "precompiled.h"

#include "NeoMem.h"
#include "SheetWizard.h"
#include "PageClassIntro.h"
#include "PageClassName.h"
#include "PageClassInheritance.h"
#include "PageClassType.h"
#include "PageClassIcon.h"
#include "PageClassProperties.h"
#include "PageClassContents.h"
#include "PageClassViews.h"
//#include "PageClassFolder.h"
#include "PageClassSummary.h"
#include "Constants.h"
#include "Hint.h"

#include "ConstantsDatabase.h"
#include "BDataLink.h"
#include "BDataColumns.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




IMPLEMENT_DYNAMIC(CSheetWizard, CPropertySheetEx2)

BEGIN_MESSAGE_MAP(CSheetWizard, CPropertySheetEx2)
	//{{AFX_MSG_MAP(CSheetWizard)
	ON_WM_CREATE()
	ON_COMMAND(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



CSheetWizard::CSheetWizard() {
	m_pdoc = NULL;
	m_bAddMode = TRUE;
	m_pobj = NULL;
	m_pobjAdd = NULL;
	m_pobjEdit = NULL;
	m_pobjEditOriginal = NULL;
//	m_bAddFolder = FALSE;
	m_nAddEditMode = modeAddOrEdit; // default
}

CSheetWizard::CSheetWizard(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheetEx2(nIDCaption, pParentWnd, iSelectPage) {
}

CSheetWizard::CSheetWizard(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheetEx2(pszCaption, pParentWnd, iSelectPage) {
}

CSheetWizard::~CSheetWizard() {
}


int CSheetWizard::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CPropertySheetEx2::OnCreate(lpCreateStruct) == -1)
		return -1;	
	return 0;
}



// Run the dialog
// Basically, you have two dummy bobjects m_pobjAdd and m_pobjEdit.
// If user chooses Add the wizard will use m_pobjAdd, if they choose Edit, 
// the class will be copied to m_pobjEdit, so that the user can cancel out 
// of the wizard. 
// m_pobj points to one or the other of these bobjects, and is what the 
// pages in the wizard refer to.
//
// Parameters:
//	nAddEditMode - set to modeAddOrEdit to start the wizard with the intro screen
//	pobjClass - for modeEdit, pass the class to edit
//
// Returns
//	wizard answer (ID_WIZFINISH or IDCANCEL)
//
//. clean up this code - too complex
//int CSheetWizard::DoModalParameters(eWizardMode nMode, BObject* pobj /* = 0 */)
int CSheetWizard::DoModalParameters(int nAddEditMode, BObject* pobjClass /* = 0 */) {

	m_nAddEditMode = nAddEditMode;

	// Create the page objects
	CPageClassIntro pgIntro;
	CPageClassName pgName;
//	CPageClassInheritance pgInheritance;
	CPageClassType pgType;
	CPageClassIcon pgIcon;
	CPageClassProperties pgProperties;
	CPageClassContents pgContents;
//,	CPageClassAdvanced pgAdvanced;
//	CPageClassViews pgViews;
//	CPageClassFolder pgFolder;
	CPageClassSummary pgSummary;

	// Add pages to sheet
	if (nAddEditMode == modeAddOrEdit)
		AddPage(&pgIntro);
	AddPage(&pgName);
//	AddPage(&pgInheritance);
	AddPage(&pgType);
	AddPage(&pgIcon);
	AddPage(&pgProperties);
	AddPage(&pgContents);
//,	AddPage(&pgAdvanced);
//	AddPage(&pgViews);
//	AddPage(&pgFolder);
	AddPage(&pgSummary);

	// Get document 
	m_pdoc = BDoc::GetDoc();

	BObject* pobjBaseClass = m_pdoc->GetObjectNull(rootClass);

	// Save document modified flag so can restore if user hits cancel in wizard.
	BOOL bSaveDocModified = m_pdoc->IsModified();

	// Create our temporary objects.
	// Add object will be used if user chooses Add mode, 
	// Edit object will be used if they choose Edit mode. 
	// m_pobj will point to the correct object.
	// Both are added to the Classes folder. 

	// Create a new class object
	// Note: We create this with flagTemp, but remove this flag if user 
	// opts to create a new class, then set it back if they choose to 
	// edit an existing one.
	m_pobjAdd = &BObject::New(*m_pdoc, classClass, _T("New Class"), rootClass, 0, flagSystem | flagTemp);
//,	m_pobjAdd = &BClass::New(*m_pdoc, _T("New Class"), "", flagSystem | flagTemp);

	// Create a class object that will be a copy of an existing class object.
	// Note: if we pick a class that's a child of another class we have to use move.
	m_pobjEdit = &BObject::New(*m_pdoc, classClass, _T("@dummy edit class"), rootClass, 0, flagSystem | flagTemp);
	
	m_bAddMode = TRUE; // Default is to add a new class
	m_pobj = m_pobjAdd;
//	m_bAddFolder = FALSE;

	if (m_nAddEditMode == modeEditOnly) {
		m_bAddMode = FALSE;
//		m_pobj = pobj; // have it select this class in intro page
		SetEditClass(pobjClass); // copy the specified class to our edit bobject
	}

//	m_strFolderName = _T("New Folder");
//	m_strFolderName.Empty();
//	BObject* pobjUserRoot = m_pdoc->GetObject(rootUser);
//	m_pobjFolderLocation = pobjUserRoot;
//	m_bAutosort = TRUE;

	// Show wizard
	SetWizardMode();
	SetTitle(_T("Class Wizard"));

	// Show finish button always
//	m_psh.dwFlags |= PSH_WIZARDHASFINISH;

	// Show the wizard
	if (DoModal() == ID_WIZFINISH) { // user clicked Finish

		BObject* pobjClass = 0; // the class the user was adding or editing, for add folder later

		// Delete temporary objects if necessary
		if (m_bAddMode) { // Add mode

			// If in Add mode, just need to delete the Edit object quietly.
			// The add object is fine where it is.
			// First set it back to base class so we don't screw things up!
			m_pobjEdit->SetParent(pobjBaseClass);
			// Also clear the nodelete flag in case it's still set!
			m_pobjEdit->ClearFlag(flagNoDelete);
//			m_pdoc->UIDeleteObject(m_pobjEdit, TRUE);
			m_pobjEdit->DeleteObject(FALSE, FALSE);

			// Modify flag for new object (remove flagTemp so it will show up)
//			// Note: User might have set the flagObjectHasChildren flag so we need to preserve that.
//			m_pobjAdd->m_lngFlags = flagSystem;
//			m_pobjAdd->m_lngFlags |= m_bAutosort ? 0 : flagNoAutosort; // Turn autosort off if user chose it
			m_pobjAdd->ClearFlag(flagTemp);
			
			// Move object to new parent, if changed, 
			// but don't need to notify views because this is a new object
			if (m_pobjAdd->GetParent() != pobjBaseClass) {
				// first set it back to the base class so we don't screw things up
				BObject* pobjNewParent = m_pobj->GetParent();
				m_pobjAdd->SetParent(pobjBaseClass);
				ASSERT_VALID(m_pobjAdd->GetParent());
//				m_pdoc->MoveObject(m_pobjAdd, pobjNewParent, FALSE);
				m_pobjAdd->MoveTo(pobjNewParent, FALSE, FALSE);
			}

			// Add the object to the database index and tell views
			m_pdoc->AddObject(m_pobjAdd);

			pobjClass = m_pobjAdd;
		}

		else { // Edit mode
			
			// In Edit mode, we need to delete the add object, and copy the 
			// edit object properties to the original object, then delete 
			// the edit copy.

			//, use a CreateCopy or CopyFrom method

			// Copy edit object properties
			m_pobjEditOriginal->SetPropertyString(propName, m_pobj->GetPropertyString(propName), FALSE, FALSE);
			m_pobjEditOriginal->SetPropertyString(propDescription, m_pobj->GetPropertyString(propDescription), FALSE, FALSE);

			BOOL bIconChanged = (m_pobjEditOriginal->GetIconID() != m_pobj->GetIconID());
//			m_pobjEditOriginal->m_lngIconID = m_pobj->m_lngIconID;
			m_pobjEditOriginal->SetIconID(m_pobj->GetIconID());

			// Get flags from the edit object
//			// Get flags from the edit object (need to get flagObjectHasChildren at least)
			m_pobjEditOriginal->SetFlags(m_pobj->GetFlags());

			// Remove the temp flag so it will show up
			m_pobjEditOriginal->ClearFlag(flagTemp);

			// If the name property type changes, we need to tell all 
			// objects to change their bdata's!
			OBJID idNewPropType = m_pobj->GetPropertyLink(propObjectNamePropertyType);
			OBJID idOldPropType = m_pobjEditOriginal->GetPropertyLink(propObjectNamePropertyType);
			if (idNewPropType != idOldPropType)
			{
				OBJID idClass = m_pobjEditOriginal->id;
				m_pdoc->GetRoot()->ChangeNamePropertyType(idClass, idNewPropType);
			}

			// Copy property bdata objects, if any
			m_pobjEditOriginal->CopyPropertyDataFrom(m_pobj, propObjectProperties);
			m_pobjEditOriginal->CopyPropertyDataFrom(m_pobj, propObjectDefaultClass);
			m_pobjEditOriginal->CopyPropertyDataFrom(m_pobj, propObjectFlags);
			m_pobjEditOriginal->CopyPropertyDataFrom(m_pobj, propObjectNamePropertyType);
//.			m_pobjEditOriginal->CopyPropertyDataFrom(m_pobj, propObjectViewArrangement);

			// Note: Let's just assume that all properties changed - makes the code simpler
			CHint h;
			h.pobjObject = m_pobjEditOriginal;
			h.idProperty = propName; m_pdoc->UpdateAllViewsEx(NULL, hintPropertyChange, &h);
			h.idProperty = propDescription; m_pdoc->UpdateAllViewsEx(NULL, hintPropertyChange, &h);
			h.idProperty = propIconID; m_pdoc->UpdateAllViewsEx(NULL, hintPropertyChange, &h);
			h.idProperty = propObjectProperties; m_pdoc->UpdateAllViewsEx(NULL, hintPropertyChange, &h);
			h.idProperty = propObjectFlags; m_pdoc->UpdateAllViewsEx(NULL, hintPropertyChange, &h);
			h.idProperty = propObjectNamePropertyType; m_pdoc->UpdateAllViewsEx(NULL, hintPropertyChange, &h);
//.			h.idProperty = propObjectViewArrangement; m_pdoc->UpdateAllViewsEx(NULL, hintPropertyChange, &h);

			// Move object to new parent, if changed, and notify views
			// don't do if on base class cause step 20 is screwy
			// No longer handling inheritance in v1
//			if (m_pobjEditOriginal->GetObjectID() != rootClass)
//			{
//				if (m_pobjEditOriginal->m_pobjParent != m_pobj->m_pobjParent)
//				{
//					BObject* pobjNewParent = m_pobj->m_pobjParent;
//					ASSERT_VALID(m_pobj->m_pobjParent);
////					m_pdoc->MoveObject(m_pobjEditOriginal, pobjNewParent, TRUE);
//					m_pobjEditOriginal->MoveTo(pobjNewParent, FALSE, TRUE);
//				}
//			}

			// Delete the temporary objects
			// First set them back to base class so we don't screw things up! (in case inheritance was set)
			m_pobjAdd->SetParent(pobjBaseClass);
			m_pobjEdit->SetParent(pobjBaseClass);
			m_pobjEdit->SetFlag(flagNoDelete, FALSE); // clear no delete flag in case it was set
//			m_pdoc->UIDeleteObject(m_pobjAdd, TRUE);
//			m_pdoc->UIDeleteObject(m_pobjEdit, TRUE);
			m_pobjAdd->DeleteObject(FALSE, FALSE);
			m_pobjEdit->DeleteObject(FALSE, FALSE);

			// Set document modified flag
			m_pdoc->SetModifiedFlag();

			pobjClass = m_pobjEditOriginal;

			// Now update views by having them redraw themselves
			// at this point this is the best because if user changes class icon, don't have handling for that...
			// actually this won't fix that because we set the items icon, its not gotten through getdispinfo
			// maybe it should?
			// would slow things down even more, especially since most objects would need to look
			// up the class chain to get their icon, then in the map, etc.
			// so we'll need to walk through all items in each view,checking if
			// it's a member of the class
			// special hint message?
//			m_pdoc->UpdateAllViewsEx(NULL, hintRefresh);
//			h.m_lngClassID = pobjClass->GetObjectID();
//			m_pdoc->UpdateAllViewsEx(NULL, hintClassIconChange, &h);

			// this is now done in seticonid
//			if (bIconChanged)
//				m_pdoc->UpdateAllViewsEx(NULL, hintRefreshAllIcons);

			// walk through all objects
			// look up their class chain to see if classid is in it
			// if yes, refresh their icon
			// or, just refresh all item's icon's, which would be easier


		}

/*
		// Add or Edit mode can let user add a folder, do that here
		if (m_bAddFolder) {
			ASSERT_VALID(m_pobjFolderLocation);
			BObject* pobjFolder = m_pdoc->AddObject(m_pobjFolderLocation, classFolder, m_strFolderName);
			ASSERT_VALID(pobjFolder);

			// Set the default object for this folder to be the class the user was editing
			pobjFolder->SetPropertyLink(propDefaultClass, pobjClass, FALSE, FALSE);

			// Set the columns for the folder to be the properties selected for the class
			pobjFolder->SetColumnsBasedOnClass(pobjClass);

			// Tell views about new folder
			m_pdoc->UpdateAllViewsEx(NULL, hintAdd, pobjFolder);

			// Now select it
			m_pdoc->SetCurrentObject(pobjFolder, NULL);
		}
*/

		return ID_WIZFINISH;
	}
	else { // User hit Cancel

		// Delete both temporary objects quietly
		// First set objects back to the base class so we don't screw things up
		m_pobjAdd->SetParent(pobjBaseClass);
		m_pobjEdit->SetParent(pobjBaseClass);
		m_pobjEdit->SetFlag(flagNoDelete, FALSE); // clear no delete flag in case it was set 
//		m_pdoc->UIDeleteObject(m_pobjAdd, TRUE);
//		m_pdoc->UIDeleteObject(m_pobjEdit, TRUE);
		m_pobjAdd->DeleteObject(FALSE, FALSE);
		m_pobjEdit->DeleteObject(FALSE, FALSE);

		// Restore doc modified flag (adding and deleting temporary objects sets it to true)
		m_pdoc->SetModifiedFlag(bSaveDocModified);

		return IDCANCEL;
	}
}



// Copy the specified class bobject to our edit bobject.
// (This enables user to hit cancel after making changes to the edit bobject without
// affecting the original).
void CSheetWizard::SetEditClass(BObject *pobjClass)
{
	m_pobjEditOriginal = pobjClass;
//	m_bAddFolder = FALSE;

	// Use the edit object
	m_pobj = m_pobjEdit;

	// Now copy all of this item's properties to the copy that we'll be modifying in the wizard
	m_pobj->SetPropertyString(propName, pobjClass->GetPropertyString(propName), FALSE, FALSE);
	m_pobj->SetPropertyString(propDescription, pobjClass->GetPropertyString(propDescription), FALSE, FALSE);

	// Copy flags
	// Note: If NoDelete is set, then wouldn't be able to delete the temp object if user hit cancel, so...
	// but we need to copy the NoModify flag, or at least set a m_ variable to alert the inheritance page
	m_pobj->SetFlags(pobjClass->GetFlags());

	// Add a flag so the copy object will remain invisible in the class wizard pages
	m_pobj->SetFlag(flagTemp, TRUE);

	// Set or clear autosort bit at least
//	m_pobj->m_lngFlags &= ~flagNoAutosort; // clear it 
//	m_pobj->m_lngFlags |= (pobjClass->m_lngFlags & flagNoAutosort); // set it if set in orig

	// Set parent - Note: We move the object to the new parent when the wizard is finished
	m_pobj->SetParent(pobjClass->GetParent());
	m_pobj->SetIconID(pobjClass->GetIconID());

	// Copy property bdata objects, if any
	m_pobj->CopyPropertyDataFrom(pobjClass, propObjectProperties);
	m_pobj->CopyPropertyDataFrom(pobjClass, propObjectFlags);
	m_pobj->CopyPropertyDataFrom(pobjClass, propObjectDefaultClass);
	m_pobj->CopyPropertyDataFrom(pobjClass, propObjectNamePropertyType);

}



void CSheetWizard::OnHelp() 
{
	// Call base class
	CPropertySheetEx2::OnHelp();
}


// Set the title for the class wizard based on the current page and class being edited or added.
// e.g. "Class Wizard - Step 3 of 8 [Book]"
void CSheetWizard::SetClassWizardTitle()
{
	CString str;
	CString strClass;

	int nPage = GetActiveIndex() + 1;
	int nPages = GetPageCount();

	str.Format(_T("Class Wizard - Step %d of %d"), nPage, nPages);

	// If not on first page, also show name of class being edited or added.
//	if (m_pobj)
	if ((nPage > 1) && (m_pobj != NULL))
	{
//		strClass.Format(_T(" [Class: %s]"), m_pobj->GetPropertyString(propName));
		strClass.Format(_T(" [%s]"), m_pobj->GetPropertyString(propName));
		str += strClass;
	}
	SetWindowText(str);
}


