
// BDoc


#include "precompiled.h"

#include "BDoc.h"
#include "BDataBoolean.h"
#include "BDataColumns.h"
#include "BDataDate.h"
#include "BDataEmail.h"
#include "BDataFile.h"
#include "BDataFolder.h"
#include "BDataHyperlink.h"
#include "BDataIcon.h"
#include "BDataLink.h"
#include "BDataLong.h"
#include "BDataNumber.h"
#include "BDataPersonName.h"
#include "BDataString.h"
#include "BDataWebsite.h"
#include "BObject.h"
#include "BObjects.h"
#include "ConstantsDatabase.h"
#include "CryptoFile.h"
#include "CryptoArchive.h"
#include "Strings.h"

//. get rid of all these dependencies
#include "Constants.h"
#include "DialogEditLink.h"
#include "DialogEditFolder.h"
#include "DialogEditName.h"
#include "DialogEditProperty.h"
#include "DialogEditObject.h"
#include "DialogGetLocation.h"
#include "DialogImport.h"
//#include "DialogPassword.h"
#include "DialogSaveOptions.h"
#include "DialogSelectProperty.h"
#include "DialogUpgradeFile.h"
#include "DocManagerEx.h"
#include "FileDialogEx.h"
#include "FileFormat.h"
#include "FileText.h"
#include "FrameChild.h"
#include "FrameMain.h"
#include "Hint.h"
#include "IconCache.h"
#include "MirrorFileEx.h"
#include "NeoMem.h"
#include "PageFileGeneral.h"
#include "PropertySheetEx2.h"
#include "Resource.h"
#include "SheetWizard.h"
#include "ViewRtf.h"


#include "Path.h"
using namespace nsPath;



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif





// Macros
//---------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(BDoc, CDocument)

BEGIN_MESSAGE_MAP(BDoc, CDocument)
	//{{AFX_MSG_MAP(BDoc)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_BACK, OnUpdateNavigateBack)
	ON_UPDATE_COMMAND_UI(ID_NAVIGATE_FORWARD, OnUpdateNavigateForward)
	ON_UPDATE_COMMAND_UI(ID_OBJ_AUTOSORT, OnUpdateObjAutosort)
	ON_UPDATE_COMMAND_UI(ID_OBJ_EDIT_IN_DIALOG, OnUpdateObjEditInDialog)
	ON_UPDATE_COMMAND_UI(ID_OBJ_MOVE_DOWN, OnUpdateObjMoveDown)
	ON_UPDATE_COMMAND_UI(ID_OBJ_MOVE_UP, OnUpdateObjMoveUp)
	ON_UPDATE_COMMAND_UI(ID_OBJ_PROPERTIES, OnUpdateObjProperties)
	ON_UPDATE_COMMAND_UI(ID_OBJ_SORT_CHILDREN, OnUpdateObjSortChildren)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOGGLE_CONTENTS, OnUpdateViewToggleContents)
	ON_COMMAND(ID_EDIT_CLASS_WIZ, OnCmdEditClasses)
	ON_COMMAND(ID_EDIT_PROPERTY_WIZ, OnCmdPropertyWizard)
	ON_COMMAND(ID_FILE_DELETE_ALL, OnCmdFileDeleteAll)
	ON_COMMAND(ID_FILE_PROPERTIES, OnCmdFileProperties)
	ON_COMMAND(ID_FILE_SECURITY, OnFileSecurity)
	ON_COMMAND(ID_NAVIGATE_BACK, OnNavigateBack)
	ON_COMMAND(ID_NAVIGATE_FORWARD, OnNavigateForward)
	ON_COMMAND(ID_NAVIGATE_GOTO, OnNavigateGoto)
	ON_COMMAND(ID_OBJ_ADD, OnObjAdd) //.
	ON_COMMAND(ID_OBJ_ADD_FOLDER, OnObjAddFolder)
	ON_COMMAND(ID_OBJ_AUTOSORT, OnObjAutosort)
	ON_COMMAND(ID_OBJ_CHANGE_OBJECT_CLASS, OnObjChangeObjectClass)
	ON_COMMAND(ID_OBJ_CHANGE_OBJECT_CONTENTS, OnObjChangeObjectContents)
	ON_COMMAND(ID_OBJ_CHANGE_CLASS_ICON, OnObjChangeClassIcon)

//x	ON_COMMAND(ID_OBJ_CHANGE_OBJECT_ICON, OnObjChangeObjectIcon)
//x	ON_COMMAND(ID_OBJ_CHANGE_CLASS_CONTENTS, OnObjChangeClassContents)

	ON_COMMAND(ID_OBJ_DELETE, OnObjDelete)
	ON_COMMAND(ID_OBJ_EDIT_IN_DIALOG, OnObjEditInDialog)
	ON_COMMAND(ID_OBJ_MOVE_DOWN, OnObjMoveDown)
	ON_COMMAND(ID_OBJ_MOVE_UP, OnObjMoveUp)
	ON_COMMAND(ID_OBJ_OPEN, OnObjOpen)
	ON_COMMAND(ID_OBJ_PRIORITY_HIGH, OnObjPriorityHigh)
	ON_COMMAND(ID_OBJ_PROPERTIES, OnObjProperties)
	ON_COMMAND(ID_OBJ_SORT_CHILDREN, OnObjSortChildren)
	ON_COMMAND(ID_VIEW_TOGGLE_CONTENTS, OnViewToggleContents)
	//}}AFX_MSG_MAP
//	ON_COMMAND(ID_OBJ_MOVE_TO, OnObjMoveTo)
//	ON_UPDATE_COMMAND_UI(ID_OBJ_MOVE_TO, OnUpdateObjMoveTo)
//	ON_COMMAND(ID_FILE_SEND_MAIL, OnFileSendMail)
//	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, OnUpdateFileSendMail)
ON_COMMAND(ID_FILE_SAVE, &BDoc::OnFileSave)
END_MESSAGE_MAP()



// FileClose - ID_FILE_CLOSE - handled by MFC - see SaveModified, SaveAllModified



// Construction/Destruction
//---------------------------------------------------------------------------

//,mil
BDoc::BDoc() {
	//trace("BDoc Constructor\n");

	// Initialize variables
	
	m_bUnicode = FALSE;
	m_nEncryptionType = encryptNone;
	m_strSessionKeyHash.Empty();

	m_pobjRoot = NULL;
	m_pobjCurrent = NULL;
	m_idNextObject = 0;
	m_lngSplitterPos = 0;
	m_pobjTarget = NULL;
	m_idDefaultFolderLocation = 0;
	m_idStartObject = 0;

	m_bSettingCurrentObject = FALSE;
	m_bAutoRecoverDirty = FALSE;
	m_bUpgradeFileStructure = FALSE;
	m_bUpgradeDataModel = FALSE;
	m_bLoadingFile = FALSE;

	// Create the icon cache (encapsulates an image list)
	m_pIconCache = new CIconCache(this);

	m_objHistory.SetSize(50); // number of objects to remember in history
}


BDoc::~BDoc() {
	//trace("BDoc Destructor\n");

	// The root object should have been deleted and its pointer set to
	// zero by the time this destructor is called - just verify that it happened.
	ASSERT (m_pobjRoot == 0);

	// Delete the icon cache (destructor will destroy image list)
	if (m_pIconCache)
		delete m_pIconCache;

	// Delete the Index - note this doesn't delete the BObjects themselves
	m_mapObjects.RemoveAll();
	ASSERT (m_mapObjects.GetCount() == 0);
	ASSERT (m_mapObjects.IsEmpty());
}




// Serialization
//---------------------------------------------------------------------------


// Get the next prime after the given number.
// Used by hash function.
// Have enough for about 1.4 million objects now (uses 2*nobjects as base)
ULONG GetNextPrime(ULONG n) {

	ULONG anPrimes[] = {257, 521, 1031, 2053, 4201, 8209, 16411, 32369, 66541, 104729, 
										200003, 400009, 800011, 1600033, 2750159};
	int nPrimes = sizeof(anPrimes) / sizeof(ULONG);
	for (int i = 0; i < nPrimes; i++) {
		if (n < anPrimes[i])
			return anPrimes[i];
	}
	// Past the end of the array - just use the number given
	return n;
}






// Called if an exception is thrown (typically a CFileException or CArchiveException) while saving 
// or loading the document. The default implementation examines the exception object and looks 
// for an error message that specifically describes the cause. If a specific message is not found 
// or if e is NULL, the general message specified by the nIDPDefault parameter is used. 
// The function then displays a message box containing the error message. Override this function 
// if you want to provide additional, customized failure messages. This is an advanced overridable.
void BDoc::ReportSaveLoadException(LPCTSTR lpszPathName, CException* e, BOOL bSaving, UINT nIDPDefault) {
	
	TRACE("BDoc::ReportSaveLoadException\n");
	// check if it's one we want to handle
/*
	if (e != NULL) {
		ASSERT_VALID(e);
		if (e->IsKindOf(RUNTIME_CLASS(CUserException)))
			return; // already reported

		if (e->IsKindOf(RUNTIME_CLASS(CArchiveException))) {
			switch (((CArchiveException*)e)->m_cause)
*/
	// if not, call the default
	CDocument::ReportSaveLoadException(lpszPathName, e, bSaving, nIDPDefault);
}




// Get a pointer to the child frame associated with this document
//, this fails if the frame is not active, which is true in opening a file
CFrameChild* BDoc::GetMDIChildFrame() {
	CFrameMain* pMainFrame = DYNAMIC_DOWNCAST(CFrameMain, AfxGetApp()->m_pMainWnd); // from CWnd to CFrameMain
	ASSERT_VALID(pMainFrame);
	CFrameChild* pChildFrame = DYNAMIC_DOWNCAST(CFrameChild, pMainFrame->MDIGetActive()); // from CMDIChildWnd to CFrameChild
	ASSERT_VALID(pChildFrame);
	return pChildFrame;
}



// Diagnostics
//---------------------------------------------------------------------------

#ifdef _DEBUG
void BDoc::AssertValid() const {
	CDocument::AssertValid();
}

void BDoc::Dump(CDumpContext& dc) const {
	CDocument::Dump(dc);
}
#endif //_DEBUG



// Static (class) methods
//----------------------------------------------------------------


// To get pointer to the current document from anywhere in application 
// (eg dialog box) say BDoc::GetDoc();
// static
BDoc* BDoc::GetDoc() {

	CMDIFrameWnd* pFrame = DYNAMIC_DOWNCAST(CMDIFrameWnd, AfxGetApp()->m_pMainWnd);
	if (!pFrame) return NULL;
	
	CMDIChildWnd* pChild = pFrame->MDIGetActive();
	if (!pChild) return NULL;

	CDocument * pDoc = pChild->GetActiveDocument();
	if (!pDoc) return NULL;
	

	BDoc* pNDoc = DYNAMIC_DOWNCAST(BDoc, pDoc);
	if (!pNDoc) return NULL;

	return pNDoc;
}



// Create a new document and return a reference to it
//, doesn't work
// static
BDoc& BDoc::New() {
	//, cannot access protected member declared in class 'CWinApp'
//	app.OnFileNew();
	app.NewFile(); // calls onfilenew
	BDoc* pdoc = BDoc::GetDoc(); //, get rid of this fn, if possible, because it uses app, and we don't want that connection. but i think mfc uses it?
	ASSERT_VALID(pdoc);
	return *pdoc;
}



// Message Handlers
//---------------------------------------------------------------------------
// You can override the CDocument member functions OnNewDocument and OnOpenDocument 
// to provide application-specific initialization of the document. Typically, you 
// must override both since a document can be created in two ways.


// Create a new blank document.
// Returns True if successful.
BOOL BDoc::OnNewDocument() {

	//trace("BDoc::OnNewDocument\n");

	m_bLoadingFile = TRUE;

	ASSERT_VALID(this);
	ASSERT(m_pobjRoot == 0);

	// The default implementation of this function calls the DeleteContents member function 
	// to ensure that the document is empty and then marks the new document as clean. 
	if (!CDocument::OnNewDocument())
		return FALSE;
	
	// Make sure the map index is clear (object id to bobject)
	m_mapObjects.RemoveAll();

	// Reset the next object id counter
	m_idNextObject = lngUserIDStart;

	// Load the template document here.
	// The template document contains all the base classes and properties and folders.
//,	CDialogNewFile dlg;
//	dlg.DoModal()
//	CreateTemplate();
//	CString strTemplateFile = app.m_strApplicationFolder + _T("\\") + app.m_strTemplateFileName;
//	if (!OnOpenDocument(strTemplateFile))
	//	if (!OnOpenDocument(app.m_strTemplatePath))
	//		return FALSE;
	if (!CreateTemplate())
		return FALSE;


	// Mark document as clean
	SetModifiedFlag(FALSE);

	// Set current object.
	m_pobjCurrent = GetObject(rootUser);

	//, call SetTitle and remove the .neo from the title, even though windows settings determine 
	// it being there

	//, Clear status bar
//	app.SetStatusBarText();

	//trace("  exiting OnNewDocument\n");

	m_bLoadingFile = FALSE;

	return TRUE;
}






// Make any changes to the document's file structure here
// (ie temporary, admin type stuff)
void BDoc::UpdateDocument(BObject* pobj) {

	ASSERT_VALID(pobj);

/*
	// Update the object then call this routine recursively for its properties and its children
	// Convert from BDataArrayLinks to BDataLinks
	if (pobj->m_pdat) {
		ASSERT_VALID(pobj->m_pdat);
		if (pobj->m_pdat->IsKindOf(RUNTIME_CLASS(BDataArrayLinks))) {
			BDataLinks* pdatNew = new BDataLinks;
			ASSERT_VALID(pdatNew);
			BDataArrayLinks* pdatOld = (BDataArrayLinks*) pobj->m_pdat;
			int nItems = pdatOld->m_apobj.GetSize();
			pdatNew->m_apobj.SetSize(nItems);
			for (int i = 0; i < nItems; i++) {
				BObject* pobj = (BObject*) pdatOld->m_apobj.GetAt(i);
				ASSERT_VALID(pobj);
				pdatNew->m_apobj.SetAt(i, pobj);
			}
			// Now delete the old BData and replace it with our new one
			delete pobj->m_pdat;
			pobj->m_pdat = pdatNew;
		}
	}

	// Walk through properties and call this routine recursively
	if (pobj->m_paProperties) {
		ASSERT_VALID(pobj->m_paProperties);
		int nProps = pobj->m_paProperties->GetSize();
		for (int i = 0; i < nProps; i++) {
			BObject* pobjProp = (BObject*) pobj->m_paProperties->GetAt(i);
			ASSERT_VALID(pobjProp);
			UpdateDocument(pobjProp);
		}
	}

	// Walk through children and call this routine recursively
	if (pobj->GetChildren()) {
		ASSERT_VALID(pobj->GetChildren());
		int nChildren = pobj->GetChildren()->GetSize();
		for (int i = 0; i < nChildren; i++) {
			BObject* pobjChild = (BObject*) pobj->GetChildren()->GetAt(i);
			ASSERT_VALID(pobjChild);
			UpdateDocument(pobjChild);
		}
	}
*/

}





// Methods
//---------------------------------------------------------------------------




// Get a pointer to the imagelist associated with this document
CImageList* BDoc::GetImageList() {
	ASSERT_VALID(this);
	ASSERT_VALID(m_pIconCache);
	return m_pIconCache->GetImageList();
}


// Returns a pointer to the main root object, which may be NULL if document has not been initialized
/*
inline BObject* BDoc::GetRoot() {
	ASSERT_VALID(this);
	return m_pobjRoot;
}
*/

inline void BDoc::SetRoot(BObject* pobj) {
	ASSERT_VALID(this);
	ASSERT_VALID(pobj);
	m_pobjRoot = pobj;
}

// Gets the index of the specified icon in the image list
int BDoc::GetIconIndex(OBJID idIcon) {
	ASSERT_VALID(this);
	ASSERT_VALID(m_pIconCache);
	return m_pIconCache->GetIconIndex(idIcon);
}


// Set the currently selected object.
// Will broadcast message to all views that current object has changed.
// Will also set the target object to the specified object.
// Pass Navigating True if user is navigating through history list 
// (back or forward), so won't write selection to history list.
// default psender null, bnav false
void BDoc::SetCurrentObject(BObject *pobjCurrent, CView* pSender, BOOL bNavigating) {
                   
	ASSERT_VALID(this);
	ASSERT_VALID(pobjCurrent);

	TRACE("BDoc::SetCurrentObject to %s\n", pobjCurrent->GetName(TRUE));


//x	ui.



	// Set flag
	m_bSettingCurrentObject = TRUE;

	// Save current data in all views
	if (m_pobjCurrent) {
		ASSERT_VALID(m_pobjCurrent);
		UpdateAllViewsEx(pSender, hintSave, m_pobjCurrent);
	}

	// Set flag if current object is changing
	BOOL bChanged = (m_pobjCurrent != pobjCurrent);

	// Store pointer to current object
	m_pobjCurrent = pobjCurrent;

	if (pobjCurrent) {
		ASSERT_VALID(pobjCurrent);

		// Notify all views other than the one responsible for calling this method
		// that the selected BObject has changed.
		
		// Note: hintSelect and hintLoad are separate events because you don't want to 
		// add views in the middle of walking through the list of views.
		
		// Select is handled by the tab views - will add views dynamically.
		// Bug: Had to add the ignoreloadhint flag because hintLoad was being triggered twice
		// the first time you clicked on a text object. The reason is that the Tab view will load all
		// child views, select one as the current one, and send hintLoad to that view. But we call
		// hintLoad down below, so it gets triggered twice. We don't want to change how the tab view
		// operates because it works that way for when the user clicks on a tab with the mouse. 2003-04-06.
		app.m_bIgnoreLoadHint = TRUE;
		UpdateAllViewsEx(pSender, hintSelect, pobjCurrent);
		app.m_bIgnoreLoadHint = FALSE;

		// Load is handled by the child views - actually loads the data
		UpdateAllViewsEx(pSender, hintLoad, pobjCurrent);

		// Save the object to the history list if it's a different object from the last one and 
		// user is not walking through history list.
		if (bChanged && (bNavigating == FALSE))
		{
			m_objHistory.SetCurrent(pobjCurrent);
		}
	}

	// Set this as the target object also.
	SetTargetObject(pobjCurrent);

	// Clear flag
	m_bSettingCurrentObject = FALSE;

}




// Get a pointer to the currently selected object.
BObject* BDoc::GetCurrentObject() {
	ASSERT_VALID(this);
	ASSERT_VALID(m_pobjCurrent); 
	return m_pobjCurrent;
}


// Return a pointer to the specified object.
// If object not found, will assert. 
BObject* BDoc::GetObject(OBJID idObject) {
	BObject* pobj = GetObjectNull(idObject);
	ASSERT(pobj); // "BDoc GetObject - Couldn't find ObjectID %d in document map\n", idObject
	return pobj;
}


// Lookup an object - may return NULL
BObject* BDoc::GetObjectNull(OBJID idObject) {
	ASSERT_VALID(this);
	ASSERT_VALID(&m_mapObjects);
	BObject* pobj = 0;
	m_mapObjects.Lookup(idObject, pobj);
	if (pobj)
		ASSERT_VALID(pobj);
	return pobj;
}





// Return a pointer to a COPY of the specified object, or NULL if not found in index
// Be sure to check result for NULL
//, hmm, so null, couldn't return reference, unless had a null object?
//, or... why allow it to return a null at all? could throw an error
BObject* BDoc::GetObject2(OBJID idObject) {
	ASSERT_VALID(this);
	ASSERT_VALID(&m_mapObjects);
	BObject* pobj;
	if (m_mapObjects.Lookup(idObject, pobj)) {
		// Object found in map - return pointer to it
		ASSERT_VALID(pobj);
		return pobj;
	}
	else
	{
		// Object not found in map - return NULL
		TRACE("! BDoc GetObject - Couldn't find ObjectID %d in document map\n", idObject);
		return NULL;
	}
}



//, placeholder for general query fn
BObjects* BDoc::GetObjects() {
	BObjects* pobjs = new BObjects();
	return pobjs;
}



// Search through the document recursively starting at the specified object, looking in the 
// specified properties. Adds pointers to matching objects to array aResults.
// To search in all properties, pass 0 for idProperty.
// lngExcludeFlags of 0 includes all objects. You can exclude system objects by passing flagSystem.
// Returns total number of hits.
//. try to avoid using cstrings in this code
// Currently only used 3 times - in viewsearch and bdatalink
int BDoc::GetObjects(
						BObject* pobjStart, 
						OBJID idProperty, 
						CString strFindText, 
						BObjects& aResults, 
						ULONG lngExcludeFlags /* = 0 */, 
						BOOL bMatchCase /* = FALSE */, 
						BOOL bWholeWord /* = FALSE */, 
						BOOL bSearchStartObject /* = FALSE */,
						BOOL bOriginalCall /* = TRUE */
						) {

	// Search for the specified text, adding results to collection
	// search recursively starting at the root object
	ASSERT_VALID(this);
	ASSERT_VALID(&aResults);
	ASSERT_VALID(pobjStart);

	//trace("BDoc::SearchForText - Search in %s, property %d for %s\n", 
	//	pobjStart->GetPropertyString(propName), idProperty, (LPCTSTR) strFindText);

	CString strPropertyValue;
	BOOL bFoundInObject = FALSE;

	// If this object has any of the bits set as specified in lngExcludeFlags, then exclude this object
	// from the search results
	// Bug:: Used && instead of & and screwed up results! Be careful of this! & is bitwise and && is logical!
	BOOL bExcludeThisObject = pobjStart->GetFlag(lngExcludeFlags);

	// Convert search string to lower case for case insensitive sort.
	// unbelievable - there is no case-insensitive search in either cstring or the run-time library!
	// for now, copy string and make it all lower-case and do same for search string
	//, this is getting executed each time cause it's recursive!!
	// Note: We only do this once because bOriginalCall will be false for any recursive calls.
	if (bMatchCase == FALSE && bOriginalCall)
		strFindText.MakeLower();

	// Search in start object if specified and it's not excluded
	if (bSearchStartObject && !bExcludeThisObject) {
		// Search in name first
		// Since name is not included in the paProperties array, we must handle it as a special case,
		// since it is the only pseudo property requiring searching through.
		if ((idProperty == 0) || (idProperty == propName)) {
			strPropertyValue = pobjStart->GetPropertyString(propName);
			if (!bMatchCase)
				strPropertyValue.MakeLower();
			//trace("  Searching in ObjectID %d's name \"%s\"\n", pobjStart->GetObjectID(), (LPCTSTR) strPropertyValue);
			int nPos = strPropertyValue.Find(strFindText, 0);
			if (nPos != -1) {
				//trace("    Found in name!\n");
				// Found in object name, so add object to collection
				aResults.Add(pobjStart);
				// Set flag so rest of code doesn't execute unnecessarily
				bFoundInObject = TRUE;
			}
		}

		// If the text was not found in the name and the property we need to search in is not propName, then 
		// continue...
		// The bFoundInObject flag just saves us from searching through other properties when the 
		// text was already found in the name.
		if ((!bFoundInObject) && (idProperty != propName)) {
			// Search in classname manually, since it's not included in the m_paProperties collection.
			if ((idProperty == 0) || (idProperty == propClassName)) {
				strPropertyValue = pobjStart->GetPropertyString(propClassName);
				if (!bMatchCase)
					strPropertyValue.MakeLower();
				//trace("  Searching in classname \"%s\"\n", strPropertyValue);
				int nPos = strPropertyValue.Find(strFindText, 0);
				if (nPos != -1) {
					//trace("      Found in classname!\n");
					// Found in class name, so add object to collection
					aResults.Add(pobjStart);
					// Set flag so rest of code doesn't execute unnecessarily
					bFoundInObject = TRUE;
				}
			}

			// If not found in classname, continue
			if (!bFoundInObject) {
				//trace("  Searching in property collection\n");
				BObjects* paProperties = pobjStart->GetProperties(); //, add asserts etc to fn
				if (paProperties) {
					ASSERT_VALID(paProperties);
					// Walk through all properties, searching through text of each.
					int nItems = paProperties->GetSize();
					for (int i = 0; i < nItems; i++) {
						// Search through property text
						BObject* pobjProp = DYNAMIC_DOWNCAST(BObject, paProperties->GetAt(i));
						ASSERT_VALID(pobjProp);

						// Exclude property if it's a system prop, etc.
						// Special case for rtf text - it's a system prop but we still want to search it
						if ((!(pobjProp->GetFlag(lngExcludeFlags))) || pobjProp->GetClassID() == propRtfText) {
							// Search through this property only if we're interested in all properties,
							// or it's the property we're interested in.
							if ((idProperty == 0) || (idProperty == pobjProp->GetClassID())) {
								//trace("    Searching in property %d\n", pobjProp->GetClassID());
								//, confusing, but gps(propName) will get the string version of the propvalue, right? 
								strPropertyValue = pobjProp->GetPropertyString(propName);
								// this trace bombs because of string length!
//								//trace("    Searching in property %d value \"%s\"\n", pobjProp->GetClassID(), strPropertyValue);

								// If this is an rtf text property, search through it with the app's hidden rtf control
								// Bug: Used idProperty instead of pobj->m_lngClassID! wound up
								// searching rtf text as if it were plain text.
//								if (idProperty == propRtfText)
								if (pobjProp->GetClassID() == propRtfText) {
									// Assign to invisible rtf control so we can use its searching capabilities.
									long nPos = app.SearchRtfString(strPropertyValue, strFindText, bMatchCase, bWholeWord);
									if (nPos != -1) {
										//trace("      Found in rtf string!\n");
										aResults.Add(pobjStart);
										break;
									}
								}
								else {
									// This is a plain text property, so search through it using CString method.
									// No case insensitive option, so must convert to lower case! lame!
									//, add option to stringex to compare case-insensitive
									//    would need to walk through string character by character, etc.
									if (!bMatchCase)
										strPropertyValue.MakeLower();
									int nPos = strPropertyValue.Find(strFindText, 0);
									if (nPos != -1) {
										//trace("      Found in regular string!\n");
										aResults.Add(pobjStart);
										break;
									}
								}
							}					
						}
					}
				}
			}
		}
	} // if (!bExcludeThisObject)

	// Now search through children by calling this routine recursively.
	BObjects* paChildren = pobjStart->GetChildren();
	if (paChildren) {
		ASSERT_VALID(paChildren);
		int nItems = paChildren->GetSize();
		for (int i = 0; i < nItems; i++) {
			BObject* pobj = DYNAMIC_DOWNCAST(BObject, paChildren->GetAt(i));
			ASSERT_VALID(pobj);
			// Search recursively through children
			GetObjects(pobj, idProperty, strFindText, aResults, lngExcludeFlags, bMatchCase, bWholeWord, TRUE, FALSE);
			// Note: We pass FALSE for match case because we've already converted to 
//			Search(pobj, idProperty, strFindText, aResults, lngExcludeFlags, FALSE, bWholeWord, TRUE);
		}
	}

	// Clear the invisible rtf to save memory
	app.m_rtf.SetWindowText("");

	return aResults.GetSize();
}






// Add an object to the database. 
// Assigns a new ObjectID and keeps the index up to date. 
// Sets document modified flag.
// Tells views about new object.
// If a view already knows about the object, pass it here so it won't be updated also. 
void BDoc::AddObject(HOBJECT hobj, CView* pviewIgnore /*=NULL*/) {
	ASSERT_VALID(this);

	BObject* pobj = (BObject*) hobj;

	pobj->SetDoc(this);

	// Handle default class
//	if (pobj->GetClassID() == 0)
//		pobj->SetClassID(classPaper);

	// Handle default location
//	BObject* pobjRoot = this->GetRoot();
//	if (pobj->GetParent() == NULL)
//		pobj->SetParent(pobjRoot);

	//, add AssertValid method
	ASSERT_VALID(pobj);
	ASSERT(pobj->GetClassID());
//////////	ASSERT(pobj->GetParent());

	// Set the object id
	OBJID idObject = pobj->GetObjectID();
	// If no object id specified then get next available one
	if (idObject == 0) {
		// Get the next available ObjectID
		idObject = GetNextObjectID();
	}
	else {
		// Keep track of next available object ID
		if (idObject > m_idNextObject)
			m_idNextObject = idObject;
	}
	pobj->SetObjectID(idObject);

	// Add the new object to the document's Index
	AddObjectToIndex(pobj);

	// Mark document as modified
	//, check if temp object first?
	SetModifiedFlag(TRUE);
	
	// Validate object again
	ASSERT_VALID(pobj);

	// Tell views about new object
//	this->UpdateAllViewsEx(NULL, hintAdd, pobj);
	this->UpdateAllViewsEx(pviewIgnore, hintAdd, pobj);
}


// Add an object to the index at the specified ObjectID slot.
//. could use our own lookup class rather than the cmap hash - code in assembly
// Note: If for some reason the ObjectID is already occupied, this routine will
// try getting the next valid ID until it finds an empty space.
// Returns True if successful.
BOOL BDoc::AddObjectToIndex(HOBJECT hobj) {

	ASSERT_VALID(this);
	ASSERT_VALID(hobj);

/*
//, better to put this stuff here - ie pass in an objectid to use. 
	// Set the object id
	// If no object id specified then get next available one
	if (idObject == 0) {
		// Get the next available ObjectID
		idObject = GetNextObjectID();
	}
	else {
		// Keep track of next available object ID
		if (idObject > m_idNextObject)
			m_idNextObject = idObject;
	}
	hobj->SetObjectID(idObject);
*/

	OBJID idObject = hobj->GetObjectID();
	ASSERT(idObject);

	HOBJECT hobjTest = NULL; // a dummy object

	// Make sure no object is already in the slot
	if (m_mapObjects.Lookup(idObject, hobjTest)) {
		TRACE("! NeoDoc::AddObjectToIndex ObjectID %d \"%s\" is already in map!\n", idObject, 
				(LPCTSTR) hobj->GetPropertyString(propName));
		
		// Try assigning a new objectid until you find one that's unoccupied
		while (TRUE) {
			idObject = GetNextObjectID();
			TRACE("    assigning new objectid of %d\n", idObject);
			// If found an empty slot, exit
			if (!m_mapObjects.Lookup(idObject, hobjTest))
				break;
		}
		
		// Assign new objectid to the object
		hobj->SetObjectID(idObject);
	}

	// Add the object to the index at the object id
	//. catch memory exception here
	// Note: SetAt returns void
	m_mapObjects.SetAt(idObject, hobj);

	// Make sure object was added correctly
	ASSERT(m_mapObjects.Lookup(idObject, hobjTest));
	return TRUE;
}





// Delete the contents of this document object.
// DeleteContents provides the best place to destroy a document’s data when you want 
// to keep the document object around. The function is called automatically by the framework 
// any time it’s necessary to delete only the document’s contents. It’s called in response 
// to the Close command on the File menu, when the user closes the document’s last open 
// window, and before creating or opening a document with the New and Open commands. 
// This is all part of the base-class functionality of DeleteContents.
// Need to return document object to initial state.
void BDoc::DeleteContents() {
	ASSERT_VALID(this);

	CWaitCursor cw;

	app.SetStatusBarText(_T("Clearing memory..."));

	// The base class implementation does nothing.
//	CDocument::DeleteContents();
	
	if (m_pobjRoot) {
		// Delete the root bobject and all its children and properties recursively
		delete m_pobjRoot;
		m_pobjRoot = NULL;
	}

 	// Clear the icon cache
 	m_pIconCache->RemoveIcons();
 
 	// Clear the index map
 	m_mapObjects.RemoveAll();
 
	m_pobjCurrent = NULL;

	// Clear status bar
	app.SetStatusBarText();

	// Check assumptions
	ASSERT(m_pobjRoot == NULL);
	ASSERT(m_pobjCurrent == NULL);
	ASSERT(m_mapObjects.GetCount() == 0);
//	ASSERT(m_pIconCache->GetImageList() == NULL);
}




// User Interface routines
//-----------------------------------------------------------------------------------------------------------------


// Delete the specified objects.
// This method will keep the Index up to date.
// Pass bQuiet True to keep from asking user if they want to delete the object.
// Returns TRUE if successful, FALSE if not.
//. could pass flag to do hard delete (skip recycle folder)
// This sends hintDelete to all views for each collection of objects that it recursively deletes.
// Note: This is in BDoc because this must modify the index, and check the current object, etc.
BOOL BDoc::UIDeleteObjects(BObjects* paObjects, BOOL bOriginalCall /* = TRUE */, BOOL bQuiet /* = FALSE */) {

	ASSERT_VALID(this);
	ASSERT_VALID(m_pobjCurrent);
	ASSERT_VALID(paObjects);

	// if any object in collection (or descendents) is nodelete, give message and don't delete any
	// ask user if they are sure they want to delete x
	// walk through all objects
	// call delete object recursively
	// walks through objects and calls itself
	// THEN checks if object has references - if yes, asks user if they want to remove them. if no, return false.

	// First make sure all data for the current object is saved
//	UpdateAllViewsEx(NULL, hintSave, m_pobjCurrent);
	if (m_pobjCurrent) {
		ASSERT_VALID(m_pobjCurrent);
		UpdateAllViewsEx(NULL, hintSave, m_pobjCurrent);
	}

	// Check recursively if any objects are marked NoDelete (system objects etc).
//	// or if they are referenced by other objects, and lets user remove references.
	// This will display a message explaining why objects can't be deleted if it fails.
	if (!paObjects->IsDeleteValid())
		return FALSE;

	// Ask user if they want to delete the object(s)
	if (!bQuiet) {
		CString strMsg;
		CString strNames = paObjects->GetText();
		BOOL bSingle = (paObjects->GetSize() == 1);
		if (bSingle) {
			// Get classname
			BObject* pobj = DYNAMIC_DOWNCAST(BObject, paObjects->GetAt(0)); 
			ASSERT_VALID(pobj);
			CString strClassName = pobj->GetPropertyString(propClassName);
			strClassName.MakeLower();

			// Get number of children
			int nChildren = pobj->GetChildCount(FALSE, FALSE);
			CString strChildren;
			if (nChildren > 0)
				strChildren = _T(" and any child objects?");
			else
				strChildren = _T("?");
			strMsg.Format(_T("Are you sure you want to delete the %s \"%s\"%s \n\nWarning: This cannot be undone!"),
						(LPCTSTR) strClassName, 
						(LPCTSTR) strNames, 
						(LPCTSTR) strChildren
						);
		}
		else {
			strMsg.Format(_T("Are you sure you want to delete the objects \"%s\" and any child objects? \n\nWarning: This cannot be undone!"), 
					(LPCTSTR) strNames);
		}
		if (IDNO == AfxMessageBox(strMsg, MB_YESNO + MB_ICONQUESTION))
			return FALSE;
	}

	// If deleting the currently selected item, move to another item now, 
	// otherwise deleting the htreeitem will arbitrarily reposition selection 
	// to another item and cause error.
//	BOOL bDeleteCurrent = (paObjects->FindObject(m_pobjCurrent, TRUE) == -1) ? FALSE : TRUE;
//	if (bDeleteCurrent) {
//		// Set document to have no current object - will avoid problems with deleting currently selected item
		// NEVER set m_pobjCurrent to 0!!
//		SetCurrentObject(NULL, NULL);
//	}

	//, Move item to recycle folder
//	BObject* pobjRecycleFolder = GetObject(folderRecycle);
//	pobj->MoveTo(pobjRecycleFolder);

	// Walk through objects, calling pobj->DeleteObject for each one.
	int nObjects = paObjects->GetSize();
	for (int i = 0; i < nObjects; i++) {
		BObject* pobj = DYNAMIC_DOWNCAST(BObject, paObjects->GetAt(i));
		ASSERT_VALID(pobj);
		// Attempt to delete object and children - if failed, return False.
		// This also checks for references to the object and asks user if they 
		// want to remove them.
		// This set modified flag and notifies views.
		if (!pobj->DeleteObject())
			return FALSE;
	}

//	BObject* pobj = GetObject(rootUser);
//	SetCurrentObject(pobj);
	
	return TRUE;
}



// Delete an object using the UIDeleteObjects method.
// Pass bQuiet True to keep from asking user if they want to delete the object.
// Note: This notifies all views of deletion.
BOOL BDoc::UIDeleteObject(BObject *pobj, BOOL bQuiet /* = FALSE */) {
	ASSERT_VALID(pobj);
	BObjects aObjects;
	aObjects.Add(pobj);
	return UIDeleteObjects(&aObjects, TRUE, bQuiet);
}



//x
/*
// Let user choose a new icon for the specified object.
//. don't let user set icon for classIcon objects!!
BOOL BDoc::UIChangeObjectIcon(BObject* pobj) {
	ASSERT_VALID(pobj);

	// Get current icon and default icon
	OBJID idIcon = pobj->GetIconID();
	OBJID idDefaultIcon = pobj->GetDefaultIconID();

	CDialogEditLink dlg;
//	dlg.m_nMode = IDD_CHANGE_ICON;
//	dlg.m_strCaption = "Change Icon";
//	dlg.m_strInstructions = "Select an icon from the list below, or click on Import to import a new icon.";
//	dlg.m_idStart = folderIcons;
//	dlg.m_idSelected = idIcon;
//	dlg.m_idDefault = idDefaultIcon;
	dlg.m_nHelpID = IDD_CHANGE_OBJECT_ICON;
	if (dlg.DoModalLinkSimple("Change Icon", "&Select an icon from the list below, or click on Import to import a new icon.", 
									folderIcons, idIcon, idDefaultIcon, app.m_lngExcludeFlags) == IDOK) {
		// Check if icon changed
		OBJID idNewIcon = dlg.m_lngSelectedID;
		if (idIcon != idNewIcon && idNewIcon != 0) {
			// This will set doc modified flag and update views
			pobj->SetIconID(idNewIcon);
			return TRUE;
		}

	}
	return FALSE;
}
*/



// Let user change class of object
BOOL BDoc::UIChangeObjectClass(BObject* pobj) {
	ASSERT_VALID(pobj);

	BOOL bNoModifyClass = pobj->GetFlag(flagNoModifyClass);
	if (bNoModifyClass) {
		AfxMessageBox("The class of this object can't be changed.", MB_ICONINFORMATION);
		return FALSE;
	}
	
	ULONG idClass = pobj->GetClassID();

	CDialogEditLink dlg;
	dlg.m_nHelpID = IDD_CHANGE_OBJECT_CLASS;
	if (dlg.DoModalLinkSimple("Change Class", "&Select the new class for the object.", 
					rootClass, idClass, idClass, app.m_lngExcludeFlags, FALSE) == IDOK) {
		// Check if class changed
		OBJID idNewClass = dlg.m_lngSelectedID;
		if (idClass != idNewClass && idNewClass != 0) {
			// Change the object's class
			// This will handle converting name data type if necessary, etc.,
			// and also will set modified flag and tell views of change
			pobj->SetClassID(idNewClass);
		}

	}
	return FALSE;

}




// Let user change class of children for the specified object
//. don't let user change system objects!
BOOL BDoc::UIChangeObjectContents(BObject* pobj) {
	ASSERT_VALID(pobj);
	
	//, handle in doc not ui
	OBJID idClass = pobj->GetPropertyLink(propDefaultClass);
	if (idClass == 0)
		idClass = classPaper;

	CDialogEditLink dlg;
	dlg.m_nHelpID = IDD_CHANGE_OBJECT_CONTENTS;
	if (dlg.DoModalLinkSimple("Change Contents", 
				"&Select the class of object that will be added to this object by default.",
				rootClass, idClass, idClass, app.m_lngExcludeFlags) == IDOK) {
		// Check if class changed
		OBJID idNewClass = dlg.m_lngSelectedID;
		if (idClass != idNewClass && idNewClass != 0) {
			BObject* pobjNewClass = dlg.m_pobjSelected;
			ASSERT_VALID(pobjNewClass);
			// This will set document modified flag and update views
			pobj->SetPropertyLink(propDefaultClass, pobjNewClass->id);
			return TRUE;
		}

	}
	return FALSE;

}



// Change the icon for the class of the specified object.
BOOL BDoc::UIChangeClassIcon(BObject *pobj) {
	ASSERT_VALID(pobj);

	// Get object's class
	BObject* pobjClass = pobj->GetClassObject();
	ASSERT_VALID(pobjClass);

	// Get current icon and default icon
	OBJID idIcon = pobjClass->GetIconID();
	OBJID idDefaultIcon = pobjClass->GetDefaultIconID();

	CDialogEditLink dlg;
	dlg.m_nHelpID = IDD_CHANGE_CLASS_ICON;
	if (dlg.DoModalLinkSimple("Change Class Icon", "&Select an icon from the list below, or click on Import to import a new icon.", 
									folderIcons, idIcon, idDefaultIcon, app.m_lngExcludeFlags) == IDOK) {
		// Check if icon changed
		OBJID idNewIcon = dlg.m_lngSelectedID;
		if (idIcon != idNewIcon && idNewIcon != 0) {
			// This will set doc modified flag and update views
			pobjClass->SetIconID(idNewIcon);
			return TRUE;
		}

	}
	return FALSE;
}




//x
/*
// Rename the current object, bringing up the appropriate dialog.
// Returns True if user hit OK, False if Cancel.
// This is called by F4 handlers.
BOOL BDoc::UIRenameObject(BObject* pobj) {
	//, we don't really need this routine...
//x	return pobj->UIEditValue(propName);
	return app.ui.EditValue(pobj, propName);
}
*/


// Export the objects specified in the BDataLink object to a file
//void BDoc::Export(BDataLink &datLink)
//void BDoc::Export(BObject* pobj, BOOL bRecurse, BOOL bSystem, CString strFormat, CFilename strFilename)
void BDoc::Export(BObject* pobj, BOOL bRecurse, BOOL bSystem, eFileFormat nFormat, CFilename strFilename) {
	ASSERT_VALID(pobj);

	//, this shouldn't be part of doc. 
	app.SetStatusBarText(_T("Exporting file..."));

	// open the file
//	CStdioFile file;
	CFileText file;
//	file.SetFormat(strFormat);
	file.SetFormat(nFormat);

	// Open does not throw exceptions. great. but Write does. 
	// cstdiofile - Text mode provides special processing for carriage 
	// return/linefeed pairs. When you write a newline character (0x0A) 
	// to a text-mode CStdioFile object, the byte pair (0x0A, 0x0D) is sent to 
	// the file. When you read, the byte pair (0x0A, 0x0D) is translated to a 
	// single 0x0A byte.
//	if (file.Open(strFilename, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
//	if (file.Open(strFilename, CFile::modeCreate | CFile::modeWrite))
	if (file.Open(strFilename, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) {

		// ideally, just create a different type of archive, then do serialize,
		// and each archive will write the information in the appropriate format!
		// hmm, but each item would need to write the full set of properties out
		// for certain formats, eg csv. so could store that info in the archive...
//		CArchive ar(&file, CArchive::store);
//		CArchiveXml ar(&file, CArchive::store);
//		CArchiveCsv ar(&file, CArchive::store);
//		this->Serialize(ar);

		BDataLink datProps;
		int nProps = GetProperties(datProps, pobj);

		// Initialize the progress bar and show it
		int nObjects = pobj->GetChildCount(TRUE) + 1;
		app.GetProgressBar().SetRange(0, nObjects);
		app.GetProgressBar().SetPos(0);
		app.GetProgressBar().ShowWindow(SW_SHOW);
		app.SetStatusBarText(_T("Exporting data..."));

		// export header
		file.WriteHeader(datProps);

		// recurse over objects, exporting them
		pobj->Export(file, bRecurse, datProps);
		
		// export footer
		file.WriteFooter();

		// close the archive and file
//		ar.Close();
		file.Close();

		// update status bar
		app.GetProgressBar().SetPos(0);
		app.GetProgressBar().ShowWindow(SW_HIDE);
		app.SetStatusBarText(_T("Data exported"));

//		CString s = _T("Finished exporting data to file \"") + strFilename + _T("\".");
//		AfxMessageBox(s, MB_ICONINFORMATION);

	}
	else {
		CString s = _T("Unable to open file \"") + strFilename + _T("\" for writing.");
		AfxMessageBox(s, MB_ICONEXCLAMATION);
	}


	//, this shouldn't be part of doc. 
//	app.SetStatusBarText();

/*
	//, doc enums or app enums
	const filetypeRtf = 1;
	const filetypeText = 2;
	const filetypeNeoMem = 3;
	switch (nFileType) {
	case filetypeRtf:
		strText = pobjFirst->GetPropertyString(propRtfText);
		break;
	case filetypeText: {
			// Get the rtf text and convert it to plain text
			LPCTSTR pszRtf = pobjFirst->GetPropertyString(propRtfText);
			app.ConvertRtfToPlain(pszRtf, strText);
			break;
		}
	}
*/



}



// Add a new property def bobject.
// Returns a pointer to the new bobject or 0 if user hit cancel.
BObject* BDoc::UIAddNewPropertyDef() {

	CDialogEditProperty dlg;
	
	if (dlg.DoModalAdd()) {

		// Add the new property and tell views

		// Create the new propertydef
		BObject& objProp = BObject::New(*this, classProperty, dlg.m_strName, folderProperties);
//,		BObject& objProp = BPropDef::New(*this, dlg.m_strName, dlg.m_pobjPropertyType->id, dlg.m_strDescription);

		objProp.SetPropertyLink(propPropertyType, dlg.m_pobjPropertyType->id, FALSE, FALSE);
		objProp.SetPropertyString(propDescription, dlg.m_strDescription);

		// Link source
		if (dlg.m_pobjLinkSource)
			objProp.SetPropertyLink(propLinkSource, dlg.m_pobjLinkSource->id, FALSE, FALSE);

		// Additional display property
		if (dlg.m_bAdditionalProperty && dlg.m_pobjAdditionalProperty != 0)
			objProp.SetPropertyLink(propAdditionalDisplayProperty, dlg.m_pobjAdditionalProperty->id, FALSE, FALSE);

		// Display link hierarchy
		if (dlg.m_bDisplayHierarchy)
			objProp.SetPropertyLong(propDisplayLinkHierarchy, 1, FALSE, FALSE);

		// Limit links
		if (dlg.m_bLimitLinks)
			objProp.SetPropertyLong(propLimitNumberOfLinks, 1, FALSE, FALSE);

		// Units
//		if (dlg.m_pobjUnits)
//			hobjProperty->SetPropertyLink(propDefaultUnits, dlg.m_pobjUnits, FALSE, FALSE);

		//, number format

		return &objProp;
	}

	return 0;
}





// Edit this property def bobject using the edit property dialog,
// save any changes, and notify all views of any changes.
// Returns True if user hit OK.
BOOL BDoc::UIEditPropertyDef(BObject* pobjPropertyDef) {

	// Check if property is marked NoModify
//	if (pobjPropertyDef->GetFlag(flagNoModify)) {
//		AfxMessageBox("This property is a system property and cannot be modified.", MB_ICONINFORMATION);
//		return FALSE;
//		AfxMessageBox("Note: This is a system property - you will only be able to modify the name and description.", MB_ICONINFORMATION);
//	}

	// Get original values
	// Note: These are used after the dialog returns as well
	CString strName = pobjPropertyDef->GetPropertyString(propName);
	CString strDescription = pobjPropertyDef->GetPropertyString(propDescription);
	OBJID idPropertyType = pobjPropertyDef->GetPropertyLink(propPropertyType);
	OBJID idLinkSource = pobjPropertyDef->GetPropertyLink(propLinkSource);
	// Note: Link source may be zero
//	BObject* pobjUnits = GetPropertyLink(propDefaultUnits);

	OBJID idAdditionalProperty = pobjPropertyDef->GetPropertyLink(propAdditionalDisplayProperty);
	BOOL bAdditionalProperty = (idAdditionalProperty != 0);
	BOOL bLimitLinks = pobjPropertyDef->GetPropertyLong(propLimitNumberOfLinks);
	BOOL bDisplayHierarchy = pobjPropertyDef->GetPropertyLong(propDisplayLinkHierarchy);
	BOOL bSystemProperty = (pobjPropertyDef->GetFlag(flagNoModify)); 

	BObject* pobjPropertyType = this->GetObject(idPropertyType); // Must have a property type
	BObject* pobjLinkSource = this->GetObjectNull(idLinkSource); // may be zero
	BObject* pobjAdditionalProperty = this->GetObjectNull(idAdditionalProperty); // may be zero

	// Set up and show dialog
	CDialogEditProperty dlg;
	dlg.m_nMode = CDialogEditProperty::modeEditProperty;
	dlg.m_strName = strName;
	dlg.m_strDescription = strDescription;
	dlg.m_pobjPropertyDef = pobjPropertyDef;
	dlg.m_pobjPropertyType = pobjPropertyType;
	dlg.m_pobjLinkSource = pobjLinkSource;
//	dlg.m_pobjUnits = pobjUnits;
	dlg.m_pobjAdditionalProperty = pobjAdditionalProperty;
	dlg.m_bAdditionalProperty = bAdditionalProperty;
	dlg.m_bDisplayHierarchy = bDisplayHierarchy;
	dlg.m_bLimitLinks = bLimitLinks;
	dlg.m_bSystemProperty = bSystemProperty; // if True, user can only modify name and desc.

	if (dlg.DoModal() == IDOK) {
//	if (dlg.DoModalEdit(
		CWaitCursor cw;

		// Update this propertydef bobject with changes and notify views also

		// If changed property type, attempt a conversion of all existing property values.
		//, might want to show progress dialog
		if (pobjPropertyType != dlg.m_pobjPropertyType) {

			// Create a temporary propdef object and assign new properties to it - 
			// some bdata's use propdef on get and some use it on set, so we need a temporary go-between
			// to store the new propdef properties.
			//, is there a better way to handle this? maybe. but this deals with existing bdata methods.
			//, this is somewhat bad as doc needs to know all the properties of a property def
			// ie if you add a new one, you need to remember to add it here also

			BObject& objTemp = BObject::New(*this, classProperty, "Temp", folderProperties, 0, flagTemp); //, will need to avoid alerting ui?

			objTemp.SetPropertyLink(propPropertyType, dlg.m_pobjPropertyType->id, FALSE, FALSE);
			objTemp.SetPropertyLink(propLinkSource, dlg.m_pobjLinkSource->id, FALSE, FALSE);
			objTemp.SetPropertyLink(propAdditionalDisplayProperty, dlg.m_pobjAdditionalProperty->id, FALSE, FALSE); //, what if zero?
			if (dlg.m_bLimitLinks) objTemp.SetPropertyLong(propLimitNumberOfLinks, 1, FALSE, FALSE);
			if (dlg.m_bDisplayHierarchy) objTemp.SetPropertyLong(propDisplayLinkHierarchy, 1, FALSE, FALSE);
			OBJID idNewPropertyType = dlg.m_pobjPropertyType->GetObjectID();

			// Start at the top object and work down
			m_pobjRoot->ChangePropertyType(pobjPropertyDef, &objTemp, idNewPropertyType);

			// Now delete our temporary object
			//, not in the db so don't need to delete it
			//, um, yeah we do. we have a pointer to this thing, need to delete it
			//, test
			objTemp.DeleteObject(FALSE, FALSE);
			//, objTemp now refers to a deleted object. oo. is this allowed? must be last line of block, or no other use of ref
//			hobjTemp = NULL;
		}

		// Name
		pobjPropertyDef->SetPropertyString(propName, dlg.m_strName);
		pobjPropertyDef->SetPropertyString(propDescription, dlg.m_strDescription);
		
		// Note: If you set link with 0, it will delete the property bobject (which is what we want).
		pobjPropertyDef->SetPropertyLink(propPropertyType, dlg.m_pobjPropertyType->id);
		pobjPropertyDef->SetPropertyLink(propLinkSource, dlg.m_pobjLinkSource->id);
		if (dlg.m_pobjAdditionalProperty) //x added this - okay? 
			pobjPropertyDef->SetPropertyLink(propAdditionalDisplayProperty, dlg.m_pobjAdditionalProperty->id); //x bombed here

//		ULONG lngLinks = dlg.m_bLimitLinks ? 1 : 0;
//		pobjPropertyDef->SetPropertyLong(propLimitNumberOfLinks, lngLinks);
		if (dlg.m_bLimitLinks)
			pobjPropertyDef->SetPropertyLong(propLimitNumberOfLinks, 1);
		else
			pobjPropertyDef->DeleteProperty(propLimitNumberOfLinks);

		if (dlg.m_bDisplayHierarchy)
			pobjPropertyDef->SetPropertyLong(propDisplayLinkHierarchy, 1);
		else
			pobjPropertyDef->DeleteProperty(propDisplayLinkHierarchy);		

		//, number format

//		pobjPropertyDef->SetPropertyLink(propDefaultUnits, dlg.m_pobjUnits);

		// Make all views refresh themselves
		UpdateAllViewsEx(NULL, hintRefresh);

		return TRUE;
	}

	return FALSE;
}




// BObject Manipulation
//----------------------------------------------------------------------------------------------------


// Create a BData object based on the property type associated with the given classdef or propertydef
// So you have some object of a certain class, or a property of a certain propertydef,
// and this routine will create the bdata object associated with the class or propertydef.
// For an object, the bdata object will store the object's name, so for a paper object it would create
// a BDataString object, and for a person object it would create a BDataPersonName object.
// For properties, different property types get different BData types, so a measurement property
// would get a BDataNumber object, and a link would get a BDataLink object, etc.
// Returns a pointer to a new BData object.
//, might want to leave this in doc, and leave CreateBObject also
// ie the document is the factory for these objects...
BData* BDoc::CreateBData(OBJID idClassOrProperty) {
	ASSERT_VALID(this);

	// Get the property type associated with this classdef or propertydef
	// Note: pobjClassDef is either the classdef or propertydef
	BObject* pobjClassDef = GetObjectNull(idClassOrProperty);
	OBJID idPropertyType = 0;
	if (pobjClassDef) {
//x		OBJID idPropType = 0;

		// For classdefs, get from "object name property type" property,
		// for propertydefs, get from "property type" property.
		if (pobjClassDef->GetClassID() == classClass)
			// Note: For now this will be BDataString, BDataPersonName, or BDataDate
			idPropertyType = pobjClassDef->GetPropertyLink(propObjectNamePropertyType);
		else
			idPropertyType = pobjClassDef->GetPropertyLink(propPropertyType);
	}

	// Create BData object and return pointer to it.
	// If prop type is zero will create string bdata.
	BData* pdat = CreateBDataFromPropertyType(idPropertyType);
	ASSERT_VALID(pdat);
	return pdat;
}




// Create a new BData object of the class associated with the given property type.
// E.g. a number property type gets a BDataNumber object.
// If prop type is zero, will return a string object.
BData* BDoc::CreateBDataFromPropertyType(OBJID idPropertyType) {
	// idproptype can be zero

	BData* pdat = NULL;

	switch (idPropertyType) {
		case 0: // string is the default if no property type specified
		case proptypeString:
			pdat = new BDataString;
			break;
		case proptypeRtf:
			pdat = new BDataString;
			//, set some flag in the bdata so we know it's rtf?
			break;
		case proptypeLink:
			//, note how this is defaulting to singular, but
			// what about props that are multiple links? ie that need a call to SetMultiple() here? 
			//, or simpler to just store them all the same. 
			pdat = new BDataLink;
			break;
		case proptypeNumber:
			pdat = new BDataNumber;
//			((BDataNumber*) pdat)->m_lngUnitID = 0; //default
			break;
		case proptypePersonName:
			pdat = new BDataPersonName;
			break;
		case proptypeDate:
			pdat = new BDataDate;
			break;
		case proptypeIcon:
			pdat = new BDataIcon;
			break;
		case proptypeLong:
			pdat = new BDataLong;
			break;
		case proptypeCurrency:
			pdat = new BDataNumber;
			// set the units for the number to currency
			(DYNAMIC_DOWNCAST(BDataNumber, pdat))->SetUnitID(unitCurrency);
			break;
		case proptypeFlags:
			pdat = new BDataFlags;
			break;
		case proptypeColumns:
			pdat = new BDataColumns;
			break;
		case proptypeBoolean:
			pdat = new BDataBoolean;
			break;
		case proptypeHyperlink: // deprecated
			pdat = new BDataHyperlink;
			break;
		case proptypeEmail:
			pdat = new BDataEmail;
			break;
		case proptypeWebsite:
			pdat = new BDataWebsite;
			break;
		case proptypeFile: 
			pdat = new BDataFile;
			break;
		case proptypeFolder:
			pdat = new BDataFolder;
			break;
//		case proptypeViews:
//			pData = new BDataViews;
//			break;
/*
		case proptypeTimeInterval:
			pdat = new BDataNumber;
			// set the units for the number to seconds
			((BDataNumber*) pdat)->m_lngUnitID = unitSeconds;
			break;
		case proptypeCalculated:
			pdat = new BDataNumber;
			break;
		case proptypeDatedMeasurement:
			break;
		case proptypeAddress:
			break;
			*/
		default:
			AfxMessageBox("Need handler for property type in BDoc::CreateBDataFromPropertyType!");
			break;
	}
	ASSERT_VALID(pdat);
	return pdat;
}




// Return the next available ObjectID
ULONG BDoc::GetNextObjectID() {

	ASSERT_VALID(this);
	m_idNextObject++;
	// If in user mode make sure ID is in UserID space
//	if (!app.m_bAdmin && m_idNextObject < lngUserIDStart)
	// Make sure ID is in UserID space
	if (m_idNextObject < lngUserIDStart) {
		m_idNextObject = lngUserIDStart;
	}
	return m_idNextObject;
}








/*
// either handle this here or in the child frame
void BDoc::OnCmdFilePrintPreview() {
	// here we have access to the current object
	// pass on to the rtf view?
	// must ensure that the rtf view is actually loaded?
	// think of it as an object that handles the formatting of the rtf text for the printer
	// pass the value of the selected text only flag
	// what we want to do is route the print commands to the rtf view
}
*/



// This is triggered if user clicks on the close button for the child frame.
BOOL BDoc::CanCloseFrame(CFrameWnd* pFrame) {
	// Make sure the current object is saved
	if (m_pobjCurrent) {
		ASSERT_VALID(m_pobjCurrent);
		UpdateAllViewsEx(NULL, hintSave, m_pobjCurrent);
	}

	return CDocument::CanCloseFrame(pFrame);
}




// Show properties for document
void BDoc::OnCmdFileProperties() {
	//, ideally this will bring up the standard windows property page showing
	// the contents of the SummaryInformation and/or DocumentSummaryInformation property set
	// we'd need to use structured storage
	// and write the property set
	// See The Summary Information Property Set in msdn help

	CPropertySheetEx2 sheet;
	sheet.m_psh.dwFlags |= PSH_NOAPPLYNOW; // turn off apply button
	CPageFileGeneral page;
	sheet.AddPage(&page);
	sheet.SetTitle(_T("Document"), PSH_PROPTITLE);
	page.m_pDoc = this;
	if (sheet.DoModal() == IDOK) {
		// onapply will have already applied changes to the object and notified the views
	}

}







// Find references to the specified Find object, starting at the start object and recursing downwards.
// Will fill array with objects that reference the Find object, and return the number of references.
// If no Start object is passed, will start at rootMain
//, rename to GetObjectReferences
int BDoc::GetReferences(BObject *pobjFind, CObArray &aRefs, BObject *pobjStart /* = 0 */, BOOL bRecurse /* = TRUE */) {
	ASSERT_VALID(pobjFind);
	if (pobjStart == 0)
		pobjStart = GetObject(rootMain);

	return pobjStart->GetReferences(pobjFind, aRefs, bRecurse);
}






// Bring up the class wizard to add, edit, or delete a class
//, this belongs in the ui layer. child frame? 
void BDoc::OnCmdEditClasses() {
	CSheetWizard sh;
	if (sh.DoModalParameters(CSheetWizard::modeAddOrEdit) == ID_WIZFINISH) {
		// don't need to do anything
	}
}


// Bring up the list of properties to add, edit or delete
void BDoc::OnCmdPropertyWizard() {
	// Note: We don't care about return value since user always uses Close button in Edit mode
//	CDialogEditLink	dlg;
//	dlg.m_nHelpID = IDD_PROPERTY_WIZARD;
//	dlg.DoModalEdit("Properties", "You can add a new property, or edit or delete an existing property here.", 
//									folderProperties, app.m_lngExcludeFlags);
	CDialogSelectProperty dlg;
	dlg.m_nMode = IDD_PROPERTY_WIZARD;
	dlg.DoModal();
}



// Create a template file from the current file (delete all user objects)
void BDoc::OnCmdFileDeleteAll() {
	if (IDYES == AfxMessageBox("Warning: This will delete all user objects from this file. "
		"All classes and properties will be left in the file. This will let you create a new template file. "
		"Do you want to continue?", MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2)) {
		// get all home child objects and delete them silently,
		// what about references?
		// property/classdefs might reference user objects (eg link source property, etc)
		// could traverse tree, delete objects at bottom, and don't delete if they're referenced
		// that way, would leave things like Author folder that was linked to ?
	}
}





// Add a new class using the class wizard.
// Returns pointer to new class bobject, or 0 if user hit Cancel.
BObject* BDoc::UIAddNewClass() {
	CSheetWizard sh;
	if (sh.DoModalParameters(CSheetWizard::modeAddOnly) == ID_WIZFINISH) {
		return sh.m_pobj;
	}
	return 0;
}


// Edit the specified class in the class wizard.
// Returns True if user hit Finish.
BOOL BDoc::UIEditClass(BObject *pobjClass) {
	CSheetWizard sh;
	if (sh.DoModalParameters(CSheetWizard::modeEditOnly, pobjClass) == ID_WIZFINISH) {
		return TRUE;
	}
	return FALSE;
}





// Add a new object, allowing user to select class and name in a dialog
// Bring up a dialog to enter information about the new item
// (select class from a combobox, enter in the text representation, 
// select icon, etc.).
// Depending on the class selected, could enter different properties also.
// If parent is not specified will add to current object.
// Bug: Had the bool parameters switched around from the definition
BObject* BDoc::UIAddNewObject(
						BObject* pobjParent /* = 0 */, 
						BOOL bSelectNewObject /* = TRUE */
						) {
	ASSERT_VALID(this);

	// Use current object as parent as none specified
	if (pobjParent == NULL)
		pobjParent = m_pobjCurrent;
	ASSERT_VALID(pobjParent);

	// The parent object should specify what type of object to add as the default,
	// eg if on a Book Folder then Book class would be the default.
	// This will check the object for this property - if not there, will look up the class chain.
	OBJID idClass = pobjParent->GetPropertyLink(propDefaultClass);
	BObject* pobjClass = this->GetObject(idClass);

	// Get name of new object from the default class (eg "New Book")
	//, (or could be a classdef property if need to customize more)
	CString strName;
	pobjClass->GetClassDefNewName(strName);

	// Show dialog
	CDialogEditObject dlg;
	if (dlg.DoModalAddObject(strName, pobjParent, pobjClass, 0) == IDOK) {
		// Get class, name, and location (parent) that user selected
		ULONG idClass = dlg.m_lngClassID;
		if (idClass) {
			// bug 6748 v1.2c: another one of these crashes caused by nothing selected 
			//   in the dialog (giving idClass=0). so just add an if around it. 
			ASSERT(idClass);
			strName = dlg.m_strName;
			pobjParent = dlg.m_pobjParent;

			// Add a new object to the document
			BObject& objNew = BObject::New(*this, idClass, strName, pobjParent->id);

			//, this is really kludgy being here - move into bobj::new
			// Special properties for folder objects
			if (idClass == classFolder) {
				// Default class
				BObject* pobjDefaultClass = dlg.m_pobjDefaultClass;
				if (pobjDefaultClass != 0) {
					ASSERT_VALID(pobjDefaultClass);
//,					BClassDef& objClassDef = (BClassDef&) objNew; //?
					objNew.SetPropertyLink(propDefaultClass, pobjDefaultClass->id, FALSE, FALSE);
					// Initialize column array based on default class
					objNew.SetColumnsBasedOnClass(pobjDefaultClass);
				}
			}
			
			// Now select the object as the current object if specified by the 
			// parameter bSelectNewObject.
			// Also set focus to viewText.
			if (bSelectNewObject) {
				SetCurrentObject(&objNew, NULL);
				// Now set focus to text view if available
				CFrameChild* pFrame = GetMDIChildFrame();
				pFrame->ShowView(viewText, TRUE);
			}

			return &objNew;
		}
		else {
			AfxMessageBox("No class selected for new object. No action taken.", MB_ICONINFORMATION);
		}
	}

	return NULL;
}




// Edit the specified object in the edit object dialog.
// Returns True if user hit OK.
BOOL BDoc::UIEditObject(BObject *pobj) {	
	ASSERT_VALID(this);
	ASSERT_VALID(pobj);
	ASSERT_VALID(pobj->GetData());

	// Check if object has a person name, in which case, just bring up the edit person name dialog.
	// If editing an icon, just let user modify name!
	ULONG idClass = pobj->GetClassID();
	BOOL bIcon = (idClass == classIcon);
	//, bad way to check this - should be a pobj property
	BOOL bPersonName = (pobj->GetData()->IsKindOf(RUNTIME_CLASS(BDataPersonName)));
//	ULONG idClass = pobj->GetClassID();
//	BObject* pobjClass = m_pDoc->GetObject(idClass);
//	BObject* pobjNamePropType = pobjClass->GetPropertyLink(propObjectNamePropertyType);
//	OBJID idNamePropType = pobjNamePropType->GetObjectID();
//	if (lngNamePropTypeID == proptypePersonName)
	if (bPersonName || bIcon) {
//x		if (pobj->GetData()->UIEditValue(pobj, 0)) {
//x		if (pobj->GetData()->UIEditValue(pobj, 0, app.ui)) {
		if (pobj->GetData()->UIEditValue(app.ui, pobj, 0)) {
			CHint h;
			h.pobjObject = pobj;
			h.idProperty = propName;
			UpdateAllViewsEx(0, hintPropertyChange, &h);
			return TRUE;
		}
		else {
			return FALSE;
		}
	}

	// Get object properties
	CString strName = pobj->GetPropertyString(propName);
	BObject* pobjClass = GetObjectNull(idClass);
	OBJID idDefaultClass = pobj->GetPropertyLink(propDefaultClass);
	BObject* pobjDefaultClass = this->GetObjectNull(idDefaultClass);
	BObject* pobjParent = pobj->GetParent();

	// Initialize dialog
	CDialogEditObject dlg;
	if (IDOK == dlg.DoModalEditObject(strName, pobjParent, pobjClass, pobjDefaultClass)) {
		// Name
		if (strName != dlg.m_strName)
			pobj->SetPropertyString(propName, dlg.m_strName);

		// Class
		if (idClass != dlg.m_lngClassID)
			pobj->SetClassID(dlg.m_lngClassID);

		// Contents (Default Class)
		if (pobjDefaultClass != dlg.m_pobjDefaultClass)
			pobj->SetPropertyLink(propDefaultClass, dlg.m_pobjDefaultClass->id);
		
		return TRUE;
	}

	return FALSE;
}



void BDoc::OnCloseDocument() {
	// Delete the AutoRecover file (not needed since we're closing down normally)
	CString strAutoRecover = GetModifiedName(m_strPathName, _T("_AutoRecover"));
	if (Library::FileExists(strAutoRecover)) {
		TRACE("BDoc::OnCloseDocument - deleting %s\n", (LPCTSTR) strAutoRecover);
		CFile::Remove(strAutoRecover);
	}

	// The default implementation of this function destroys all of the frames used for viewing 
	// the document, closes the view, cleans up the document's contents, and then calls the 
	// DeleteContents member function to delete the document’s data. 
	CDocument::OnCloseDocument();

	// Show welcome dialog if no other files are open
	app.ShowWelcomeDialog();
}





// Check if the specified bobject address is valid and that the bobject is valid
BOOL BDoc::IsBObjectValid(BObject *pobj) {
	OBJID idTest;
	BObject* pobjTest;
	POSITION pos = m_mapObjects.GetStartPosition();
	while (pos) {
		m_mapObjects.GetNextAssoc(pos, idTest, pobjTest);
		if (pobj == pobjTest)
			return TRUE;
	}
	// Searched through the whole map and didn't find the bobject, so must have been deleted!
	return FALSE;
}



// Back
void BDoc::OnNavigateBack() {
	BObject* pobj = DYNAMIC_DOWNCAST(BObject, m_objHistory.GoBack());
	if (pobj && IsBObjectValid(pobj)) // check that bobject is still valid first!
		SetCurrentObject(pobj, NULL, TRUE);
}

// Forward
void BDoc::OnNavigateForward() {
	BObject* pobj = DYNAMIC_DOWNCAST(BObject, m_objHistory.GoForward());
	if (pobj && IsBObjectValid(pobj)) // check that bobject is still valid first!
		SetCurrentObject(pobj, NULL, TRUE);
}


void BDoc::OnUpdateNavigateBack(CCmdUI* pCmdUI) {
	pCmdUI->Enable(m_objHistory.IsBackValid());
}

void BDoc::OnUpdateNavigateForward(CCmdUI* pCmdUI) {
	pCmdUI->Enable(m_objHistory.IsForwardValid());
}



// Set the specified object as the command target, which will be acted on by any
// ID_OBJ_xxx command handlers.
// Pass 0 for new object.(?)
//, should be a prop of ui 
void BDoc::SetTargetObject(BObject *pobj) {
//	ASSERT(pobj != (BObject*) (CListCtrlEx::keyDummyRow)); //, catch this
//	m_datTargetObjects.RemoveAll();
//	m_datTargetObjects.AddLink(pobj);
	//, for now, just one target object allowed
	m_pobjTarget = pobj;
	//if (pobj)
		//trace("SetTargetObject to %s\n", pobj->GetPropertyString(propName));
	//else
		//trace("SetTargetObject to <null>\n");
}


// Returns True if there is only one target object.
BOOL BDoc::IsTargetSingle() {
//,	return m_datTarget.IsSingle();
//	return TRUE;
	return (m_pobjTarget != NULL);
}


// Returns pointer to current target object - may be 0!
BObject* BDoc::GetTargetObject() {
	return m_pobjTarget;
}



void BDoc::OnObjEditInDialog() {
	// First make sure there's just one object for the target
	if (IsTargetSingle()) {
//x		UIRenameObject(m_pobjTarget);
//x		app.ui.EditValue(m_pobjTarget, propName);
		m_pobjTarget->UIEditValue(propName, app.ui);
	}	
}


void BDoc::OnUpdateObjEditInDialog(CCmdUI* pCmdUI) {
	//, bad behavior - tbr button is disabled, but if you click on it, setfocus selects first cell in 
	// grid, enabling the button, so it enables it and brings up the dialog. so just set true for now
//	pCmdUI->Enable(m_pDoc->IsTargetSingle());
	pCmdUI->Enable(TRUE);
}





// Display properties for the current target object.
// Alt+Enter
void BDoc::OnObjProperties() {
	if (m_pobjTarget) {
		// Save the current object, in case it is the target
		UpdateAllViewsEx(NULL, hintSave, m_pobjCurrent);

		ASSERT_VALID(m_pobjTarget);
		m_pobjTarget->DisplayProperties();
	}
}

void BDoc::OnUpdateObjProperties(CCmdUI* pCmdUI) {
}




// Toggle high priority flag for item
void BDoc::OnObjPriorityHigh() {
	if (m_pobjTarget) {
		ASSERT_VALID(m_pobjTarget);
		BOOL bCurrent = m_pobjTarget->GetFlag(flagHighPriority);
		m_pobjTarget->SetFlag(flagHighPriority, !bCurrent);
		
		// Resort views if autosort is on for this item's parent
		if (m_pobjTarget->IsParentSorted()) {
			BObject* pobjParent = m_pobjTarget->GetParent();
			UpdateAllViewsEx(NULL, hintResortChildren, pobjParent);
		}
	}
}



//x
/*
// Change the default contents for the target object's class.
void BDoc::OnObjChangeClassContents() {
	if (m_pobjTarget) {
		ASSERT_VALID(m_pobjTarget);
		UIChangeClassContents(m_pobjTarget);
	}
}

// Change the default contents for the class of the specified object.
BOOL BDoc::UIChangeClassContents(BObject *pobj) {
	ASSERT_VALID(pobj);

	// Get object's class
	pobj = pobj->GetClassObject();
	ASSERT_VALID(pobj);

	//, move out of ui
	OBJID idClass = pobj->GetPropertyLink(propObjectDefaultClass);
	if (idClass == 0)
		idClass = classPaper;


	CDialogEditLink dlg;
	dlg.m_nHelpID = IDD_CHANGE_CLASS_CONTENTS;
	CString strInstructions;
	strInstructions.Format("&Select the class that will be added to objects of the class '%s' by default.", 
		(LPCTSTR) pobj->GetPropertyString(propName));
	if (dlg.DoModalLinkSimple("Change Class Contents", strInstructions,
					rootClass, idClass, idClass, app.m_lngExcludeFlags) == IDOK) {
		// Check if class changed
		OBJID idNewClass = dlg.m_lngSelectedID;
		if (idClass != idNewClass && idNewClass != 0) {
			BObject* pobjNewClass = dlg.m_pobjSelected;
			ASSERT_VALID(pobjNewClass);
			// This will set document modified flag and update views
			pobj->SetPropertyLink(propObjectDefaultClass, pobjNewClass->id);
			return TRUE;
		}

	}
	return FALSE;
}
*/


// Change the default icon for the target object's class.
void BDoc::OnObjChangeClassIcon() {
	if (m_pobjTarget) {
		ASSERT_VALID(m_pobjTarget);
		UIChangeClassIcon(m_pobjTarget);
	}
}


// Change the class of the target object.
void BDoc::OnObjChangeObjectClass() {
	// Bring up dialog with treeview of all classes, let user choose one
	if (m_pobjTarget) {
		ASSERT_VALID(m_pobjTarget);
		UIChangeObjectClass(m_pobjTarget);
	}
}


// Change the default class of children for the target object.
void BDoc::OnObjChangeObjectContents() {
	// Bring up dialog with treeview of all classes, let user choose one
	if (m_pobjTarget) {
		ASSERT_VALID(m_pobjTarget);
		UIChangeObjectContents(m_pobjTarget);
	}
}

//x
/*
// Select the icon to be associated with the target object.
void BDoc::OnObjChangeObjectIcon() {
	// Bring up dialog with list of all icons in file, let user choose one
	if (m_pobjTarget) {
		ASSERT_VALID(m_pobjTarget);
		UIChangeObjectIcon(m_pobjTarget);
	}
}
*/


// Delete the target object.
//, Move the target object to the recycle folder.
void BDoc::OnObjDelete() {
	if (m_pobjTarget) {
		ASSERT_VALID(m_pobjTarget);
		UIDeleteObject(m_pobjTarget);
	}
}



// Move the current object down relative to its siblings
void BDoc::OnObjMoveDown() {
	if (m_pobjTarget) {
		ASSERT_VALID(m_pobjTarget);
		m_pobjTarget->MoveDown();
	}
}


// Move the current object up relative to its siblings
void BDoc::OnObjMoveUp() {
	if (m_pobjTarget) {
		ASSERT_VALID(m_pobjTarget);
		m_pobjTarget->MoveUp();
	}
}


void BDoc::OnObjOpen() {
	if (m_pobjTarget) {
		// Select item in treeview
		// This will broadcast all the update messages necessary (save, select, load)
		SetCurrentObject(m_pobjTarget);
	}		
}




// Toggle autosort setting for the target object.
void BDoc::OnObjAutosort() {
	if (m_pobjTarget) {
		ASSERT_VALID(m_pobjTarget);
		BOOL bAutosort = !m_pobjTarget->IsSorted(); // toggle value
		m_pobjTarget->SetFlag(flagNoAutosort, !bAutosort); // store opposite since flag is for NoAutosort!
		if (bAutosort) {
			// If autosort is being turned on, need to sort the children in the treeview and other views alphabetically.
			UpdateAllViewsEx(NULL, hintResortChildren, m_pobjTarget);
		}
		else {
			// If autosort is being turned off, need to sort the children physically so that they start off sorted.			
			m_pobjTarget->SortChildren();
			// Also make sure the children are displayed alphabetically sorted in all views 
			// (eg in contents view might be in some other order).
			UpdateAllViewsEx(NULL, hintResortChildren, m_pobjTarget);
		}
	}	
}




// See if the current object can be moved up or down
// If the parent object is sorted, then disable move up and down
void BDoc::OnUpdateObjMoveDown(CCmdUI* pCmdUI) {
	ASSERT_VALID(m_pobjCurrent);
	BOOL bEnable = m_pobjCurrent->IsMoveUpDownValid(FALSE);
	pCmdUI->Enable(bEnable);
}

void BDoc::OnUpdateObjMoveUp(CCmdUI* pCmdUI) {
	ASSERT_VALID(m_pobjCurrent);
	BOOL bEnable = m_pobjCurrent->IsMoveUpDownValid(TRUE);
	pCmdUI->Enable(bEnable);
}


// Set check on or off depending on state of autosort flag for target object
void BDoc::OnUpdateObjAutosort(CCmdUI* pCmdUI) {
	if (m_pobjTarget) {
		ASSERT_VALID(m_pobjTarget);
		pCmdUI->SetCheck(m_pobjTarget->IsSorted() ? 1 : 0);
	}
}


// currently this is just handled in data view
void BDoc::OnObjSortChildren() {
}


// Enable sort children item if this item is NOT autosorted
void BDoc::OnUpdateObjSortChildren(CCmdUI* pCmdUI) {
	if (m_pobjTarget) {
		ASSERT_VALID(m_pobjTarget);
		pCmdUI->Enable(!m_pobjTarget->IsSorted());
	}
}


// Show/hide the contents view for all objects of the current object's class.
void BDoc::OnViewToggleContents() {
	ASSERT_VALID(m_pobjCurrent);

	OBJID idView = viewContents;
	CFrameChild* pframe = GetMDIChildFrame();
	if (pframe) {
		ASSERT_VALID(pframe);
		BOOL bViewVisible = pframe->IsViewVisible(idView);
		CString strShowHide = bViewVisible ? "hide" : "show";
		CString strViewName = "Contents";
		CString strClassName = m_pobjCurrent->GetPropertyString(propClassName);
		CString strMsg;
		strMsg.Format("This will %s the %s View for all objects of this class (%s). Continue?",
					strShowHide,
					strViewName,
					strClassName
					);
		if (IDYES == AfxMessageBox(strMsg, MB_ICONQUESTION + MB_YESNO)) {
			BObject* pobjView = GetObjectNull(idView);
			ULONG lngHint = bViewVisible ? hintRemoveView : hintAddView;
			UpdateAllViewsEx(NULL, lngHint, pobjView);
		}
	}
}


// Update menu text
void BDoc::OnUpdateViewToggleContents(CCmdUI* pCmdUI) {
	OBJID idView = viewContents;
	CFrameChild* pframe = GetMDIChildFrame();
	if (pframe) {
		ASSERT_VALID(pframe);
		BOOL bViewVisible = pframe->IsViewVisible(idView);
		LPCTSTR pszMenuText = bViewVisible ? "Hide &Contents View..." : "Show &Contents View...";
		pCmdUI->SetText(pszMenuText);
	}
}





// Virtual function called by framework before a modified document is to be closed. 
// The default implementation of this function displays a message box asking the user whether 
// to save the changes to the document, if any have been made. Override this function if your 
// program requires a different prompting procedure. This is an advanced overridable.
//` code copied from MFC
BOOL BDoc::SaveModified() {

	//` If still loading the document don't do the save! (might happen if autosave happens during long file load)
	if (m_bLoadingFile)
		return FALSE;

//	// We need to send hintSave message to all views to be sure the document is up to date.
//	// Actually, we can't do this here because framework won't know data in view has changed.
//	// So we put the hintSave code in NeoMem::SaveAllModified!
	// Actually we have to put this here otherwise user can say File/Close and the doc won't be flagged as dirty!
	if (m_pobjCurrent) {
		ASSERT_VALID(m_pobjCurrent);
		UpdateAllViewsEx(NULL, hintSave, m_pobjCurrent);
	}

	// If doing AutoRecover don't ask user
	if (app.m_bDoingAutoRecover)
		return DoFileSave();

	return CDocument::SaveModified();
}



// Save this document if it's modified, WITHOUT asking user if they want to save it.
// This is used by crash code to save backup of document. 
// Returns False if file save failed. 
// Note: This code is taken from CDocument::SaveModified() in doccore.cpp.
BOOL BDoc::SaveModifiedBackup() {
	if (!IsModified())
		return TRUE;        // ok to continue

/* // only needed for message box
	// get name/title of document
	CString strName;
	if (m_strPathName.IsEmpty()) {
		// get name based on caption
		strName = m_strTitle;
		if (strName.IsEmpty())
			VERIFY(strName.LoadString(AFX_IDS_UNTITLED));
	}
	else {
		// get name based on file title of path name
		strName = m_strPathName;
		if (afxData.bMarked4) {
			AfxGetFileTitle(m_strPathName, strName.GetBuffer(_MAX_PATH), _MAX_PATH);
			strName.ReleaseBuffer();
		}
	}
*/

//	CString prompt;
//	AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, strName);
//	switch (AfxMessageBox(prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE)) {
//	case IDCANCEL:
//		return FALSE;       // don't continue

//	case IDYES:
		// If so, either Save or Update, as appropriate
		if (!DoFileSave())
			return FALSE;       // don't continue
//		break;

//	case IDNO:
//		// If not saving changes, revert the document
//		break;
//
//	default:
//		ASSERT(FALSE);
//		break;
//	}
	return TRUE;    // keep going
}





// Get the document path name with the appended text and .neo extension.
// Eg call with "_Backup" will return "C:\\...\Stuff_Backup.neo"
CString BDoc::GetModifiedName(LPCTSTR szFileName, LPCTSTR szAppendText) {

	CString strModifiedName;
//	CString strOriginalName = m_strPathName;
	CString strOriginalName = szFileName;

	// If string is empty then file hasn't been saved yet - use the doc title (Document1)
	if (strOriginalName.IsEmpty())
		strOriginalName = app.m_strDocumentFolder + "\\" + GetTitle();

	//, use CPath stuff?
	int nPos = strOriginalName.ReverseFind('.');
	if (nPos != -1)
		strModifiedName = strOriginalName.Left(nPos) + szAppendText + app.m_strFileExtension;
	else
		strModifiedName = strOriginalName + szAppendText + app.m_strFileExtension;

	return strModifiedName;
}




//. was moving this to cframechild

// Add an object. 
// This is the default ID_OBJ_ADD handler.
void BDoc::OnObjAdd() {
	if (m_pobjTarget) {
		BObject* pobj = UIAddNewObject(m_pobjTarget);
	}
}



//-----------------------------------------------------------------------------------------------------------------




void BDoc::OnNavigateGoto() {
//	BObject* pobj = (BObject*) m_objHistory.GoBack();
//	if (pobj && IsBObjectValid(pobj)) // check that bobject is still valid first
//		SetCurrentObject(pobj, NULL, TRUE);

	CDialogGetLocation dlg;
	dlg.m_strInstructions = "Select the object you want to goto and hit OK. Use right and left arrows to expand and collapse items.";
	dlg.m_strCaption = "Goto Object...";
	dlg.m_pobj = m_pobjCurrent;
	dlg.m_nHelpID = IDD_GOTO_OBJECT;
	if (IDOK == dlg.DoModal()) {
		SetCurrentObject(dlg.m_pobj);
	}
}



// Open the specified document.
// override of virtual function
// 1.1 Handles AutoRecover and ErrorBackup
BOOL BDoc::OnOpenDocument(LPCTSTR lpszPathName) {
	m_bLoadingFile = TRUE;

	//trace("BDoc::OnOpenDocument(%s)\n", lpszPathName);

	CWaitCursor cw;

	// Reset the next object id counter
	m_idNextObject = lngUserIDStart;

	CString strLoadFileName = lpszPathName;

	CString strErrorBackupName = GetModifiedName(lpszPathName, _T("_ErrorBackup"));
	CString strAutoRecoverName = GetModifiedName(lpszPathName, _T("_AutoRecover"));
	BOOL bErrorBackupAvail = Library::FileExists(strErrorBackupName);
	BOOL bAutoRecoverAvail = Library::FileExists(strAutoRecoverName);

	// Check if there is an _ErrorBackup copy of this file available.
	// ErrorBackup takes precedence over AutoRecover, because ErrorBackup will be more recent. 
	if (bErrorBackupAvail) {
		CString strMsg;
		CString strFileName = Library::GetFileName(lpszPathName);
		CString strOldVersion = GetModifiedName(strFileName, _T("_ErrorBackupOldVersion"));
		strMsg.Format(
				"Error Recovery information for the file \"%s\" exists, which means that NeoMem must have "
				"closed unexpectedly. NeoMem will now load the Error Recovery file, and rename the existing "
				"file to \"%s\". ", 
				(LPCTSTR) strFileName, 
				(LPCTSTR) strOldVersion);
		AfxMessageBox(strMsg, MB_ICONINFORMATION + MB_OK);

		// Rename existing file to _ErrorBackupOldVersion
		strOldVersion = GetModifiedName(lpszPathName, _T("_ErrorBackupOldVersion")); // get full path version now!
		_tremove(strOldVersion); // Delete any existing _ErrorBackupOldVersion first
		_trename(strFileName, strOldVersion);

		// Rename _ErrorBackup to filename and continue
		_trename(strErrorBackupName, strFileName);

		// Delete any existing AutoRecover file also (which would otherwise be loaded next time)
		//. but if errorbackup file is corrupted, would want the autorecover file
		// maybe rename it to something else?
		_tremove(strAutoRecoverName);
	}
	// Check if there is an AutoRecover copy of this file available
	else if (bAutoRecoverAvail) {
		CString strMsg;
		CString strFileName = Library::GetFileName(lpszPathName);
		CString strOldVersion = GetModifiedName(strFileName, _T("_AutoRecoverOldVersion"));
//		strMsg.Format("AutoRecover information for the file \"%s\" exists - would you like to load it?", lpszPathName);
		strMsg.Format(
				"AutoRecover information for the file \"%s\" exists, which means that NeoMem must have "
				"closed unexpectedly. NeoMem will now load the AutoRecover information, and rename the "
				"existing file to \"%s\". ", 
				(LPCTSTR) strFileName, 
				(LPCTSTR) strOldVersion);
//		if (IDYES == AfxMessageBox(strMsg, MB_ICONQUESTION + MB_YESNO))
		AfxMessageBox(strMsg, MB_ICONINFORMATION + MB_OK);

		// Rename existing file to _AutoRecoverOldVersion
		strOldVersion = GetModifiedName(lpszPathName, _T("_AutoRecoverOldVersion")); // get full path version now
		_tremove(strOldVersion); // Delete any existing _AutoRecoverOldVersion first
		_trename(lpszPathName, strOldVersion);

		// Rename _AutoRecover to filename and continue
		_trename(strAutoRecoverName, lpszPathName);
	}


	// Open the file and Deserialize all its data into m_pobjRoot, and mark document as clean
//	if (!CDocument::OnOpenDocument(lpszPathName))
//	if (!OnOpenDocumentEx(lpszPathName))
	if (!OnOpenDocumentEx(strLoadFileName))
		return FALSE;

	// Give message if no root object (if it's a zero length file, OnOpenDocument skips serialization
	// so you don't get the archive exception)
	BObject* pobj = GetObjectNull(rootMain);
	if (pobj == 0) {
		AfxMessageBox("Invalid file format - no root object found.", MB_ICONEXCLAMATION);
		return FALSE;
	}

	// Set up default columns
	//, do this here for now but eventually could store with the document
	m_datColumnsDefault.RemoveColumns();
	m_datColumnsDefault.InsertColumn(propName, this, 200);
	m_datColumnsDefault.InsertColumn(propClassName, this, 200);

	// Don't let user modify home object's class
//	BObject* pobjHome = GetObject(rootUser);
//	pobjHome->m_lngFlags |= flagNoModifyClass;

	// Walk through entire document recursively, converting old types to new ones
//	BObject* pobjStart = GetObject(rootMain);
//	UpdateDocument(pobjStart);

/*
	// Change the property type for propColumnInfoArray and propObjectColumnInfoArray from
	// proptypeArray to proptypeColumns
	// Add the new proptype first
	BObject* pobj = GetObjectNull(folderPropertyTypes);
	HOBJECT* hobjPropType = CreateObject(classPropertyType, "Columns", pobj, proptypeColumns, 0, flagSystem | flagNoDelete | flagAdminOnly);
	AddObject(hobjPropType);

	pobj = GetObject(propColumnInfoArray);
	BObject* pobjPropTypeColumns = GetObjectNull(proptypeColumns);
	pobj->SetPropertyLink(propPropertyType, pobjPropTypeColumns);
	pobj = GetObject(propObjectColumnInfoArray);
	pobj->SetPropertyLink(propPropertyType, pobjPropTypeColumns);
*/

/*
	BObject* pobjClassRoot = GetObject(rootClass);
	BDataLinks* paProperties = new BDataLinks;
	paProperties->m_apobj.Add(GetObject(propName));
	paProperties->m_apobj.Add(GetObject(propClassName));
	paProperties->m_apobj.Add(GetObject(propDescription));
	paProperties->m_apobj.Add(GetObject(propFlags));
	paProperties->m_apobj.Add(GetObject(propParentName));
	paProperties->m_apobj.Add(GetObject(propObjectID));
	pobjClassRoot->SetPropertyData(propObjectProperties, paProperties);
*/

/*
	// Set up column arrangement that will be the default for all objects
	BDataArray* paCols = new BDataArray;

	BDataColumnInfo* pColInfo;
	pColInfo = new BDataColumnInfo;
	pColInfo->idProperty = propName;
	pColInfo->m_nColWidth = 200;
	paCols->m_apdat.Add(pColInfo);

	pColInfo = new BDataColumnInfo;
	pColInfo->idProperty = propClassName;
	pColInfo->m_nColWidth = 90;
	paCols->m_apdat.Add(pColInfo);

	pColInfo = new BDataColumnInfo;
	pColInfo->idProperty = propDescription;
	pColInfo->m_nColWidth = 220;
	paCols->m_apdat.Add(pColInfo);

	pobjClassRoot->SetPropertyData(propObjectColumnInfoArray, paCols);
*/

//	BObject* pobjClassPaper = GetObject(classPaper);
//	pobjClassPaper->DeleteProperty(propObjectColumnInfoArray);

//	pobjItem = CreateObject(classProperty, "Property Type", pobj, propPropertyType, 0, flagSystem | flagNoDelete);
//	pobjItem->SetPropertyLink(propPropertyType, GetObject(proptypeLong));
//  AddObject(pobjItem);

//	BObject* pobj = GetObject(propPropertyType);
//	pobj->SetPropertyLink(propPropertyType, GetObject(proptypeLink));

/*
	// Set default view arrangement for all classes
	BObject* pobjClass = GetObject(rootClass);
	BDataViews* pdatViews = new BDataViews;
	pdatViews->InsertView(0, 0, viewText, 0);
	pdatViews->InsertView(1, 0, viewProperties, 0);
	pobjClass->SetPropertyData(propObjectViewArrangement, pdatViews);

	// Set default view arrangement for folders
	pobjClass = GetObject(classFolder);
	pdatViews = new BDataViews;
	pdatViews->InsertView(0, 0, viewText, 0);
	pdatViews->InsertView(0, 1, viewContents, 0);
	pdatViews->InsertView(1, 0, viewProperties, 0);
	pobjClass->SetPropertyData(propObjectViewArrangement, pdatViews);
*/

	// Note: CViewTabs OnInitialUpdate happens AFTER this routine (OnOpenDocument) is finished
	// so the treeview has not even been loaded at this point.
	// Therefore, we don't need to use SetCurrentObject to broadcast the hints.
	// Splitter positions are set in overridden document template class.
	//trace("    exiting OnOpenDocument\n");

	m_bLoadingFile = FALSE;

	return TRUE;
}











// Advanced overridable - overridden to prevent letting user select to 
// overwrite a read-only file
// This is called by OnFileSave and OnFileSaveAs
BOOL BDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace) {
	// Save the document data to a file
	// lpszPathName = path name where to save document file
	// if lpszPathName is NULL then the user will be prompted (SaveAs)
	// note: lpszPathName can be different than 'm_strPathName'
	// if 'bReplace' is TRUE will change file name if successful (SaveAs)
	// if 'bReplace' is FALSE will not change path name (SaveCopyAs)
	CString newName = lpszPathName;
	if (newName.IsEmpty()) {
		CDocTemplate* pTemplate = GetDocTemplate();
		ASSERT(pTemplate != NULL);

		newName = m_strPathName;
		if (bReplace && newName.IsEmpty()) {
			newName = m_strTitle;
			// check for dubious filename
			int iBad = newName.FindOneOf(_T(" #%;/\\"));
			if (iBad != -1)
				newName.ReleaseBuffer(iBad);

			// append the default suffix if there is one
			CString strExt;
			if (pTemplate->GetDocString(strExt, CDocTemplate::filterExt) &&
			  !strExt.IsEmpty()) {
				ASSERT(strExt[0] == '.');
				newName += strExt;
			}
		}

		// All the call to the App does is pass the call along to the Doc Manager, 
		// so it's safe to replace this call with one of our own, which will let 
		// us pass more interesting parameters. (see AppDlg.cpp)
//		if (!AfxGetApp()->DoPromptFileName(newName, bReplace ? AFX_IDS_SAVEFILE : AFX_IDS_SAVEFILECOPY,
//			OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOREADONLYRETURN, FALSE, pTemplate)) //` Added OFN_NOREADONLYRETURN to the flags
//			return FALSE;       // don't even attempt to save
		CDocManagerEx* pDocMgr = STATIC_DOWNCAST(CDocManagerEx, app.m_pDocManager); // from CDocManager
		ASSERT_VALID(pDocMgr);
		if (!pDocMgr->DoPromptFileName2(newName, bReplace ? AFX_IDS_SAVEFILE : AFX_IDS_SAVEFILECOPY, 
			OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOREADONLYRETURN, //` added OFN_NOREADONLYRETURN
			FALSE, pTemplate, this)) //` added this pointer
			return FALSE;       // don't even attempt to save
	}

	CWaitCursor wait;

	//` this will call OnSaveDocumentEx
	if (!OnSaveDocument(newName)) {
		if (lpszPathName == NULL) {
			// be sure to delete the file
			TRY {
				CFile::Remove(newName);
			}
			CATCH_ALL(e) {
				TRACE0("Warning: failed to delete file after failed SaveAs.\n");
				DELETE_EXCEPTION(e);
			}
			END_CATCH_ALL
		}
		return FALSE;
	}

	// reset the title and change the document name
	if (bReplace)
		SetPathName(newName);

	return TRUE;        // success
}







// Add a folder. 
// This is the default ID_OBJ_ADD_FOLDER handler.
void BDoc::OnObjAddFolder() {
	if (m_pobjTarget) {
		BObject* pobj = UIAddNewFolder(m_pobjTarget);
	}	
}



// Add a new folder to the default location
BObject* BDoc::UIAddNewFolder(BObject* pobjParent /* = 0 */, 
						BOOL bSelectNewObject /* = TRUE */
						) {
	ASSERT_VALID(this);

	CString strName;
	CString strDescription;
	BObject* pobjDefaultClass = 0;

	// Get location for new folder if not specified
	if (pobjParent == 0) {
		// Doc remembers the last location for adding a folder
		if (m_idDefaultFolderLocation != 0) {
			pobjParent = GetObjectNull(m_idDefaultFolderLocation);
		}
		// If location is unspecified or no longer valid, use the root object as add location
		if (pobjParent == 0) {
			ASSERT_VALID(m_pobjRoot);
			pobjParent = m_pobjRoot;
			m_idDefaultFolderLocation = m_pobjRoot->GetObjectID(); // remember this in document
		}
	}
	ASSERT_VALID(pobjParent);

	// Get name of new object from the default class (eg "New Book")
	//. (or could be a classdef property if need to customize more)
	BObject* pobjClass = GetObject(classFolder);
	pobjClass->GetClassDefNewName(strName); // eg "New Folder"
	pobjDefaultClass = GetObject(classPaper);

	// Show dialog
	CDialogEditFolder dlg;
	if (dlg.DoModalAddFolder(strName, strDescription, pobjParent, pobjDefaultClass) == IDOK) {
		// Get class, name, and location (parent) that user selected
//		ULONG idClass = dlg.m_lngClassID;
//		ASSERT(idClass);
		strName = dlg.m_strName;
		strDescription = dlg.m_strDescription;
		pobjParent = dlg.m_pobjParent;
		ULONG idClass = classFolder;
	
		// Add a new folder to the document
		BObject& objNew = BObject::New(*this, idClass, strName, pobjParent->id);

		// Set description
		objNew.SetPropertyString(propDescription, strDescription, FALSE, FALSE);

		//, kludgy
		// Set default class and default columns
		BObject* pobjDefaultClass = dlg.m_pobjDefaultClass;
		if (pobjDefaultClass != 0) {
			ASSERT_VALID(pobjDefaultClass);
			objNew.SetPropertyLink(propDefaultClass, pobjDefaultClass->id, FALSE, FALSE);
			// Initialize column array based on default class
			objNew.SetColumnsBasedOnClass(pobjDefaultClass);
		}
		
		// Now select the object as the current object if specified by the parameter bSelectNewObject
		// Also set focus to viewText
		if (bSelectNewObject) {
			SetCurrentObject(&objNew, NULL);
			// Now set focus to text view if available
			CFrameChild* pFrame = GetMDIChildFrame();
			pFrame->ShowView(viewText, TRUE);
		}

		return &objNew;
	}
	else
		return NULL;
}



// Import a file
void BDoc::Import() {
	
	// used to have try/catch around this, but the framework will catch it and throw it
	// to our error dialog anyway...

	//,
//	CDialogImport dlgImport;
//	if (dlgImport.DoModal()) {
//	}
//	return;

	// Get import filename
//	CString strFilename = _T("");
//	CFileDialogEx dlg(TRUE, _T("rtf"), strFilename, OFN_HIDEREADONLY, app.m_strImportFilter);
//x	CFileDialogEx dlg(TRUE, _T("rtf"), _T(""), OFN_HIDEREADONLY, app.m_strImportFilter, AfxGetMainWnd());
	CFileDialog dlg(TRUE, _T("rtf"), _T(""), OFN_HIDEREADONLY, app.m_strImportFilter, AfxGetMainWnd());
	dlg.m_ofn.lpstrTitle = _T("Import File");
	if (dlg.DoModal() == IDOK) {
		CString strFilename = dlg.GetPathName();
		CString strFileTitle = dlg.GetFileTitle();

		CString strText;
//		CStdioFile f(strFilename, CFile::modeRead | CFile::typeText);
//		f.ReadString(strText);
		CFile f(strFilename, CFile::modeRead); // | CFile::typeBinary);

		const int nBufferSize = 4096;
		TCHAR szBuffer[nBufferSize + 1]; // add one for nullchar
		szBuffer[nBufferSize] = '\0';
		while (TRUE) {
			int nChars = f.Read(szBuffer, nBufferSize);
			if (nChars < nBufferSize)
				szBuffer[nChars] = '\0'; // make sure buffer terminates with nullchar
			strText += szBuffer;
			if (nChars < nBufferSize)
				break;
		}


		// Convert plaintext to rtf
//		BOOL bPlaintext = (strText.Left(4) != "{\\rt");
//		if (bPlaintext) {
//			CString strRtf;
//			app.ConvertPlainToRtf(strText, strRtf);
//			strText = strRtf;
//		}

		BObject* pobjCurrent = GetCurrentObject();

		// Create the new object as child of the current one
		BObject& objNew = BObject::New(*this, classPaper, strFileTitle, pobjCurrent->id);

		// Set its rtf property
		objNew.SetPropertyString(propRtfText, strText, FALSE, FALSE);

//LPCTSTR t = hobjNew->GetPropertyString(propRtfText);//..

		// Select the new object
		SetCurrentObject(&objNew, NULL);
	}


/*	// Open the datafile
	CFile file;
	try {
		file.Open(strFilename, CFile::modeReadWrite | CFile::shareExclusive);
	}

	// hook up to archive
	bool bReading = true;
	UINT uMode = (bReading ? CArchive::load : CArchive::store);
	try {
		CArchive ar(&file, uMode);
	}
	catch (CException* pException) {
		// handle error
		return;
	}

	// serialize data
	pobj->Serialize();

	ar.Close();
	file.Close();
*/
}





// Set security options for current file
void BDoc::OnFileSecurity() {
	UISetEncryption();
}






// Override so we can call OnSaveDocumentEx instead of the base 
// class's OnSaveDocument. And to do some other stuff.
BOOL BDoc::OnSaveDocument(LPCTSTR lpszPathName) {
	CWaitCursor cw;

	// Make sure current object is saved
//	UpdateAllViewsEx(NULL, hintSave, m_pobjCurrent);
	if (m_pobjCurrent) {
		ASSERT_VALID(m_pobjCurrent);
		UpdateAllViewsEx(NULL, hintSave, m_pobjCurrent);
	}

	// Save current splitter position
	m_lngSplitterPos = GetMDIChildFrame()->GetSplitterPos();

	// Now call the base class and exit
//	return CDocument::OnSaveDocument(lpszPathName);

	// Now call our extension of the base class's version of OnSaveDocument and exit
	return OnSaveDocumentEx(lpszPathName);
}



// This is a copy of CDocument's version of OnSaveDocument, modified 
// to use our own CCryptoFile instead of CFile, to allow for encryption.
// Have to call base class function versions explicitly. 
BOOL BDoc::OnSaveDocumentEx(LPCTSTR lpszPathName) {
	CFileException fe;

	CFile* pFile = NULL;
	pFile = GetFile(lpszPathName, CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive, &fe);
//	CCryptoFile* pFile = NULL;
//	pFile = (CCryptoFile*) GetFile(lpszPathName, CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive, &fe);

	if (pFile == NULL) {
		ReportSaveLoadException(lpszPathName, &fe, TRUE, AFX_IDP_INVALID_FILENAME); 
		return FALSE;
	}

	// Note: Buffer size of archive is the default 4096 bytes
	// Note use of CCryptoArchive which extends CArchive
	CCryptoArchive saveArchive(pFile, CArchive::store | CArchive::bNoFlushOnDelete);
	saveArchive.m_pDocument = this;
	saveArchive.m_bForceFlat = FALSE;
	TRY {
		CWaitCursor wait;
		Serialize(saveArchive);     // save me 
		saveArchive.Close();
		ReleaseFile(pFile, FALSE); 
	}
	CATCH_ALL(e) {
		ReleaseFile(pFile, TRUE); 

		TRY {
			ReportSaveLoadException(lpszPathName, e, TRUE, AFX_IDP_FAILED_TO_SAVE_DOC); 
		}
		END_TRY
		DELETE_EXCEPTION(e);
		return FALSE;
	}
	END_CATCH_ALL

	//` Don't clear this flag if we're doing AutoRecover save!
	if (app.m_bDoingAutoRecover == FALSE)
		SetModifiedFlag(FALSE);     // back to unmodified

	return TRUE;        // success
}





// A virtual member function
// Override to use CCryptoFile instead of CFile
CFile* BDoc::GetFile(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError) {
//	CMirrorFile* pFile = new CMirrorFile;
	CMirrorFileEx* pFile = new CMirrorFileEx;
	ASSERT(pFile != NULL);
	if (!pFile->Open(lpszFileName, nOpenFlags, pError)) {
		delete pFile;
		pFile = NULL;
	}
	return pFile;
}





// Serialize the document to/from the given archive, which is associated with a file. 
// To verify the password, we create the hash, then generate the session key, then get the hash 
// of that and compare it with the stored hash. Can't just use hash of the password because session 
// keys are generated from the password hash, so storing the password hash wouldn't be secure. 
// We'll always have access to the password, even though we never store it in the file, because the 
// user must enter the correct password to open the file!
void BDoc::Serialize(CArchive& ar) {
	//trace("BDoc::Serialize\n");

	ASSERT_VALID(this);

	// Note: WORD = unsigned short, ULONG = unsigned long
	WORD wReserved = 0;
	ULONG lngMagic = 0x38af34ad; // Magic bytes to recognize NeoMem file ("NeoMem" in zork bytes: 3 chars in 2 bytes)

	// Downcast CArchive variable to our extension class
	//,, this cast looks weird - valid? 
	CCryptoArchive* parex = (CCryptoArchive*) &ar; // can't use dynamic or static downcast here

	// Create encryption/decryption object
	CCrypto objCrypto;

	// Call base class first (does nothing)
	CDocument::Serialize(ar);

	if (ar.IsStoring()) {
		if (app.m_bDoingAutoRecover)
			app.SetStatusBarText(_T("Saving AutoRecover information..."));
		else
			app.SetStatusBarText(_T("Saving file..."));

		ar << lngMagic; // magic number to identify a NeoMem file
		ar << m_nVersionFileStructure; // always write file with newest (current) version
		ar << m_nVersionDataModel; // always write file with newest (current) version (assume was updated on load if necessary)
		ar << m_bUnicode; // using unicode? might be part of data model version
		ar << m_nEncryptionType; // type of encryption used on the file
		ar << m_strSessionKeyHash; // save hash of session key based on user's password (this was m_strPassword in v1)

		// Turn encryption on at this point
		if (m_nEncryptionType) {
			objCrypto.Init();
			objCrypto.MakeKey(m_strPassword);
			parex->SetCryptoOn(&objCrypto);
		}

//		ar << m_pobjCurrent;
		ar << m_lngSplitterPos;
		ASSERT(m_idNextObject > lngUserIDStart); // next objectid must be in user space
		ar << m_idNextObject;
		// Encrypt the string array. Would have been better to put this last so it would be
		// encrypted automatically but easier to just live with this file structure. 
		//, don't need this yet because we don't store the search strings
//		if (m_nEncryptionType)
//		{
//			objCrypto.Init();
//			objCrypto.MakeKey(m_strPassword); // use password to encrypt rest of file
//			objCrypto.EncryptStringList(m_lstSearches);
//		}
		m_lstSearches.Serialize(ar); // can't use << for this

		m_datViewsLeft.Serialize(ar);
		m_datColumnsSearchView.Serialize(ar);
		m_datColumnsIndexView.Serialize(ar);
		m_datColumnsPropertyView.Serialize(ar);
		m_datColumnsClassFolder.Serialize(ar);
		m_datColumnsPropertyFolder.Serialize(ar);
		m_datColumnsIconFolder.Serialize(ar);
		m_datColumnsViewFolder.Serialize(ar);

		// Reserved space for future expansion
//		ar << wReserved;
//		ar << wReserved;
		ar << m_idDefaultFolderLocation; // 1.1 added
//		ar << wReserved;
//		ar << wReserved;
		m_idStartObject = m_pobjCurrent->GetObjectID();
		ar << m_idStartObject; // 1.2c added
		ar << wReserved;
		ar << wReserved;
		ar << wReserved;
		ar << wReserved;

		// Get and write total number of objects here so can display progress bar on writing AND reading!
		ULONG nObjects = m_pobjRoot->GetChildCount(TRUE, TRUE) + 1; // add one for root object itself!
		ar << nObjects;

		// Get and write number of objects with ObjectID's
		ULONG nObjectIDs = m_pobjRoot->GetChildCount(TRUE, FALSE) + 1; // add one for root object itself!
		ar << nObjectIDs;

		// Initialize the progress bar and show it
		app.GetProgressBar().SetRange32(0, nObjects);
		app.GetProgressBar().SetStep(1);
		app.GetProgressBar().SetPos(0);
		app.GetProgressBar().ShowWindow(SW_SHOW);

		// This is what saves the entire file recursively!
		ar << m_pobjRoot;

		// Flush remaining data to the file
		// Bug: Had to add this here because otherwise CCryptoFile::Write wasn't being called for the last
		// part of the file and so that part wasn't being encrypted (not sure why MFC wouldn't have 
		// called it as expected - kind of strange). Hard to track this one down.
		ar.Flush();

		// Turn progress bar off
		app.GetProgressBar().ShowWindow(SW_HIDE);

		// Turn encryption off
		if (m_nEncryptionType)
			parex->SetCryptoOff();

		if (app.m_bDoingAutoRecover)
			app.SetStatusBarText(_T("AutoRecover information saved"));
		else
			app.SetStatusBarText(_T("File saved"));

	}

	else
	
	// Read File
	{
//		int nFileVersion = ar.GetObjectSchema(); // -1 for cdocument

		ULONG lngMagicTest;
		ar >> lngMagicTest;
		if (lngMagicTest != lngMagic) { // magic number to identify a NeoMem file
			AfxMessageBox(_T("This file does not appear to be a NeoMem file."), MB_ICONINFORMATION);
			AfxThrowUserException();
		}

		// Read file structure version and data model version
		ar >> m_nVersionFileStructure;
		ar >> m_nVersionDataModel;
		TRACE("  File structure version %d (.exe version %d)\n", m_nVersionFileStructure, versionFileStructure);
		TRACE("  Data model version %d (.exe version %d)\n", m_nVersionDataModel, versionDataModel);

		// If data model or file structure version of the file is greater than this code can handle then 
		// tell the user to upgrade.
		// This will happen if the user is still using NeoMem 1.0 and tries to open a file created with 
		// v1.1 for example. 
		// If you tried to go ahead with the file you'd run into problems with things like views which have
		// no code associated with them - would crash the program. 
		if (m_nVersionDataModel > versionDataModel || m_nVersionFileStructure > versionFileStructure) {
			AfxMessageBox(_T("Can't read this file - it was created by a more recent version of NeoMem. You will need to upgrade to a newer version of NeoMem in order to read this file. Visit www.neomem.org to download the latest version. "), MB_ICONINFORMATION);
			AfxThrowUserException();
		}
		// Note: Check for upgrading the file structure or data model is below, AFTER the check 
		// for a valid password

		app.SetStatusBarText(_T("Loading file..."));
		ar >> m_bUnicode; // using unicode? might be part of data model version in future
		ar >> m_nEncryptionType; // type of encryption used on file
		ar >> m_strSessionKeyHash; // hash of session key based on password (hexascii) - used to verify password
		TRACE("  Encryption type %d\n", m_nEncryptionType);
		TRACE("  SessionKeyHash: %s\n", m_strSessionKeyHash);

		// If this file is encrypted, ask user for password
		if (m_nEncryptionType > 0) {
			CDialogEditName dlg;
			CString str;
			dlg.m_strCaption = "Enter Password";
			str.Format("Enter the password for the file \"%s\":", (LPCTSTR) ar.GetFile()->GetFileName());
			dlg.m_strInstructions = str;
			dlg.m_bPassword = TRUE;
			if (dlg.DoModal() == IDCANCEL) {
				// User hit cancel so exit
				AfxThrowUserException();
			}
			CString strTestPassword = dlg.m_strName;

			// Test [hash of session key based on test password] with [hash of session key based on true password].
			objCrypto.Init();
			CString strTestHash = objCrypto.GetSessionKeyHash(strTestPassword);
			TRACE("  TestHash: %s\n", strTestHash);
			if (strTestHash != m_strSessionKeyHash) {
				AfxMessageBox("Invalid password entered.", MB_ICONINFORMATION);
				AfxThrowUserException();
			}
			// User must have entered the correct password, so we can continue.
			// The actual password isn't stored in the document, so set it here. 
			m_strPassword = strTestPassword;

			// Use the password to create a session key for decrypting the file. 
			objCrypto.MakeKey(m_strPassword);

			// Turn on decryption at this point
			parex->SetCryptoOn(&objCrypto);
		}


		// Check if file being read needs to be upgraded.
//		m_nVersionDataModel = 0;
		m_bUpgradeFileStructure = (m_nVersionFileStructure < versionFileStructure);
		m_bUpgradeDataModel = (m_nVersionDataModel < versionDataModel);
		if (m_bUpgradeFileStructure || m_bUpgradeDataModel) {
			// File being read is older than currently expected file structure or data model,
			// so ask user if they want to upgrade file.
			UpgradeFile(ar);
	  }

			
//		ar >> m_pobjCurrent;
		ar >> m_lngSplitterPos;
		ar >> m_idNextObject;
		//trace("  NextObjectID as stored in file is %d\n", m_idNextObject);
		m_lstSearches.Serialize(ar); // can't use >> for this
		// Decrypt the search strings
		//, don't need this yet because we don't store the search strings
//		if (m_nEncryptionType)
//			objCrypto.DecryptStringList(m_lstSearches);

		m_datViewsLeft.Serialize(ar);
		m_datColumnsSearchView.Serialize(ar);
		m_datColumnsIndexView.Serialize(ar);
		m_datColumnsPropertyView.Serialize(ar);
		m_datColumnsClassFolder.Serialize(ar);
		m_datColumnsPropertyFolder.Serialize(ar);
		m_datColumnsIconFolder.Serialize(ar);
		m_datColumnsViewFolder.Serialize(ar);


/*
		// create left views object!
		BDataViews& v = m_datLeftViews;
		v.m_avi.SetSize(6);
		v.m_avi[0].m_lngViewID = viewData;
		v.m_avi[0].m_lngViewHeight = 0;
		v.m_avi[1].m_lngViewID = 0;
		v.m_avi[1].m_lngViewHeight = 0;

		v.m_avi[2].m_lngViewID = viewSearch;
		v.m_avi[2].m_lngViewHeight = 0;
		v.m_avi[3].m_lngViewID = 0;
		v.m_avi[3].m_lngViewHeight = 0;

		v.m_avi[4].m_lngViewID = viewIndex;
		v.m_avi[4].m_lngViewHeight = 0;
		v.m_avi[5].m_lngViewID = 0;
		v.m_avi[5].m_lngViewHeight = 0;
*/

		// Reserved space for future expansion
//		ar >> wReserved;
//		ar >> wReserved;
		ar >> m_idDefaultFolderLocation; // 1.1a added
		//, could add encryption used for password, m_nEncryption encryption version, etc
//		ar >> wReserved;
//		ar >> wReserved;
		ar >> m_idStartObject; // 1.2c added
		ar >> wReserved;
		ar >> wReserved;
		ar >> wReserved;
		ar >> wReserved;

		// Read total number of objects in file so can update progress bar correctly
		ULONG nObjects;
		ar >> nObjects;
		//trace("  Total number of objects in file: %d\n", nObjects);

		// Read number of objects with ObjectID's
		ULONG nObjectIDs;
		ar >> nObjectIDs;
		//trace("  Total number of objectIDs in file: %d\n", nObjectIDs);

		// Initialize the hash table to accommodate the required number of objects (maps from ObjectID to pobj).
		// For best performance need to use a prime number, so get from table of primes.
		ULONG nHashSize = nObjectIDs * 2;
		nHashSize = GetNextPrime(nHashSize);
		//trace("  Initializing Hash Table to %d positions\n", nHashSize);
		m_mapObjects.InitHashTable(nHashSize);

		// Initialize the progress bar and show it
		app.GetProgressBar().SetRange32(0, nObjects);
		app.GetProgressBar().SetStep(1);
		app.GetProgressBar().SetPos(0);
		app.GetProgressBar().ShowWindow(SW_SHOW);

		// This is what reads all the objects into memory, recursing from the root object!
		TRY {
			ar >> m_pobjRoot; 
		}
		CATCH_ALL(e) {
			TRACE0("Error in serializing file.\n");
			DELETE_EXCEPTION(e);
		}
		END_CATCH_ALL
		ASSERT_VALID(m_pobjRoot);

		// Stop decrypting file
		if (m_nEncryptionType)
			parex->SetCryptoOff();

		// Get current object 
		if (m_idStartObject)
			m_pobjCurrent = GetObject(m_idStartObject);
		else
			m_pobjCurrent = GetObject(rootUser);

		// Initialize the history list with the current object 
		//, put here because of selchange probs in treeview etc
		m_objHistory.SetCurrent(m_pobjCurrent);

		// initialize column objects
//		m_datClassColumns.InsertColumn(propName, this);
//		m_datPropertyColumns.RemoveColumns();
//		m_datPropertyColumns.InsertColumn(propName, this);
//		m_datPropertyColumns.InsertColumn(propPropertyType, this);
//		m_datPropertyColumns.InsertColumn(propLinkSource, this);
////		m_datPropertyColumns.InsertColumn(propDescription, this);
//		m_datIconColumns.InsertColumn(propName, this);

		// index view
//		rdat.InsertColumn(propName, m_pDoc, 90);
//		rdat.InsertColumn(propClassName, m_pDoc, 50);

		// property view
//		rdat.InsertColumn(propName, m_pDoc, 140);
//		rdat.InsertColumn(propValue, m_pDoc, 240); // pseudo property

		// search view
//		rdat.InsertColumn(propName, m_pDoc, 90);
//		rdat.InsertColumn(propClassName, m_pDoc, 50);


//		m_datColumnsViewFolder.RemoveColumns();
//		m_datColumnsViewFolder.InsertColumn(propName, this, 120);
//		m_datColumnsViewFolder.InsertColumn(propDescription, this, 320);

		// set classdef default
//		BObject* pobj = GetObject(rootClass);
//		BObject* pobj2 = GetObject(classClass);
//		pobj->SetPropertyLink(propDefaultClass, pobj2, FALSE, FALSE);

		// Modify Paper views
//		BObject* pobjClass = GetObject(classPaper);
//		BData* pdat = pobjClass->GetPropertyData(propObjectViewArrangement);
//		BDataViews* pdatViews = DYNAMIC_DOWNCAST(BDataViews, pdat);
//		pdatViews->InsertView(1, 0, viewProperties, 0);

		// Modify Home views
//		BObject* pobjClass = GetObject(classHome);
//		BData* pdat = pobjClass->GetPropertyData(propObjectViewArrangement);
//		BDataViews* pdatViews = DYNAMIC_DOWNCAST(BDataViews, pdat);
//		pdatViews->InsertView(1, 0, viewProperties, 0);

		// Turn progress bar off
		app.GetProgressBar().ShowWindow(SW_HIDE);

		app.SetStatusBarText(_T("File loaded"));
	}

	// Hide progress bar
	app.GetProgressBar().SetPos(0);
	app.GetProgressBar().ShowWindow(SW_HIDE);

}



// Let user set security options for this file in a dialog
void BDoc::UISetEncryption() {

	CDialogSaveOptions dlg;
	dlg.m_nEncryptionType = m_nEncryptionType;
	dlg.m_strPassword = m_strPassword; // we'll always have this because user had to enter it to open the file
	if (dlg.DoModal() == IDOK) {
		// Set modified flag if changed encryption type or password
		if (m_nEncryptionType != dlg.m_nEncryptionType)
			SetModifiedFlag();
		if (m_strPassword != dlg.m_strPassword)
			SetModifiedFlag();

		m_nEncryptionType = dlg.m_nEncryptionType;
		m_strPassword = dlg.m_strPassword; // we'll need this later when actually saving the file, to generate the session key

		// Get the hash of the session key based on the entered password, and store it with the document. 
		// This will be used to verify password on opening the file in the future.
		CCrypto objCrypto;
		objCrypto.Init();
		m_strSessionKeyHash = objCrypto.GetSessionKeyHash(m_strPassword);
	}
}



// Move the currently selected object to a new location.
// Bring up dialog with treeview of entire file, let user choose new location.
// Returns new location of target.
BObject* BDoc::UIMoveObjectTo() {

	if (m_pobjTarget) {
		// Get parent of first object in array (kind of arbitrary as they could all have different parents)
//		BObject* pobj = (BObject*) m_aObjects.GetAt(0);
//		ASSERT_VALID(pobj);
//		ASSERT_VALID(pobj->m_pobjParent);
//		OBJID idOldParent = pobj->m_pobjParent->GetObjectID();
//		BObject* pobjTarget = m_pobjTarget; //, must save this to local because of focus changing the value
		ASSERT_VALID(m_pobjTarget);
		BObject* pobjParent = m_pobjTarget->GetParent();
		ASSERT_VALID(pobjParent);
//		OBJID idOldParent = pobjTarget->m_pobjParent->GetObjectID();
		OBJID idOldParent = pobjParent->GetObjectID();

//		CDialogEditLink dlg;
//		if (IDOK == dlg.DoModalSelect("Move Object to New Location", strMsg, rootUser, lngOldParentID, lngOldParentID, app.m_lngExcludeFlags, TRUE))
		CDialogGetLocation dlg;
		CString strMsg;
		strMsg.Format("Select the new location for the %s and hit OK. "
			"Use right and left arrows to expand and collapse items.", (LPCTSTR) m_pobjTarget->GetName(TRUE));
		dlg.m_strInstructions = strMsg;
		dlg.m_strCaption = "Move Object To...";
		dlg.m_pobj = pobjParent;
		dlg.m_nHelpID = IDD_MOVE_OBJECT_TO;
		if (IDOK == dlg.DoModal()) {
			BObject* pobjNewParent = dlg.m_pobj;
			if (pobjNewParent) {
				// Move objects to new parent, set doc modified flag and notify views
				ASSERT_VALID(pobjNewParent);
				//trace("Move %s to %s\n", m_pobjTarget->GetName(TRUE), pobjNewParent->GetName(TRUE));
				m_pobjTarget->MoveTo(pobjNewParent);
				return pobjNewParent;
			}
		}	
	}
	return NULL;
}





// Advanced overridable DoFileSave from adapted from MFC DocCore.cpp
// Called by OnFileSave
// 1.0 Added read-only message.
// 1.1 Added AutoBackup and AutoRecover handling.
BOOL BDoc::DoFileSave() {

	// Handle AutoRecover save separately
	if (app.m_bDoingAutoRecover)
		return DoFileAutoRecoverSave();

	// Check if file exists and is read-only
	DWORD dwAttrib = GetFileAttributes(m_strPathName);
	BOOL bFileExists = Library::FileExists(m_strPathName);
	BOOL bFileReadOnly = dwAttrib & FILE_ATTRIBUTE_READONLY;

	// If file exists and it is read-only give user a message
	if (bFileExists && bFileReadOnly) { // file is marked read-only
		CString str;
		CString strFileTitle = GetTitle();
		str.Format("The file \"%s\" is marked read-only, which means that in order to save\n"
						  "your changes you will need to give it a new filename.", (LPCTSTR) strFileTitle);
		AfxMessageBox(str, MB_ICONINFORMATION);
		if (!DoSave(NULL)) { // NULL = get new filename
			TRACE0("Warning: File save with new name failed.\n");
			return FALSE;
		}
	}
	else if (bFileExists == FALSE) { // file does not exist
		// If file does not exist save it with a new name
		if (!DoSave(NULL)) {
			TRACE0("Warning: File save with new name failed.\n");
			return FALSE;
		}
	}
	else { // file already exists - can do autobackup 
		// AutoBackup
		CString strBackup;
		CString strBackup2;
		if (app.m_bAutoBackup) {
			// Get filenames
			strBackup = GetModifiedName(m_strPathName, _T("_Backup"));
			strBackup2 = GetModifiedName(m_strPathName, _T("_Backup__Old"));
			// If there's already a _Backup file, remove it
			if (Library::FileExists(strBackup)) {
				_tremove(strBackup2); // delete this in case it was left behind somehow
				_trename(strBackup, strBackup2); // keep temporarily in case save fails
			}
			// Now rename the existing file to _Backup
			if (bFileExists)
				_trename(m_strPathName, strBackup);
		}

		// Save file
		if (!DoSave(m_strPathName)) {
			TRACE0("Warning: File save failed.\n");
			return FALSE;
		}

		// Finish AutoBackup
		if (app.m_bAutoBackup) {
			// Delete last _Backup file 
			//. could save some of these occasionally, like once a week or once a month...
			_tremove(strBackup2);
		}
	}

	// AutoRecover 
	// Delete the last AutoRecover file because it's not needed anymore (just saved the file normally).
	// Also reset the timer so won't immediately do AutoRecover after this normal file save.
	if (app.m_bAutoRecover) {
		CString strAutoRecoverName = GetModifiedName(m_strPathName, _T("_AutoRecover"));
		TRACE("Deleting %s\n", (LPCTSTR) strAutoRecoverName);
		_tremove(strAutoRecoverName);
		app.ResetAutoRecoverTimer(); // this will make sure that AutoRecover is actually specified
		m_bAutoRecoverDirty = FALSE; // clear flag
	}

	return TRUE;
}




// Save AutoRecover copy of file
BOOL BDoc::DoFileAutoRecoverSave() {
	
	// Update flag if user has made changes to file
//	if (IsModified()) 
//		m_bAutoRecoverDirty = TRUE; 

	// Exit if the file has not changed since last the AutoRecover or normal save.
	if (m_bAutoRecoverDirty == FALSE) {
		TRACE("No changes made since last AutoRecover save\n");
		return TRUE;
	}

	// Get AutoRecover filename
	CString strAutoRecoverName = GetModifiedName(m_strPathName, _T("_AutoRecover"));

	// Save file as copy
	if (!DoSave(strAutoRecoverName, FALSE)) { // FALSE = save as copy
		TRACE0("Warning: File save failed.\n");
		return FALSE;
	}

	// Reset the flag so we don't do more AutoRecover saves than necessary
	m_bAutoRecoverDirty = FALSE;

	return TRUE;
}



// Override of virtual method so we can keep track of AutoRecover saves also.
void BDoc::SetModifiedFlag(BOOL bModified /* = TRUE */) {
	m_bModified = bModified;
	m_bAutoRecoverDirty = bModified;
}


// Get size of file as a string
CString BDoc::GetFileSizeString() {
	CString str;
	LPCTSTR pszFile = GetPathName();
	CFileException e;
	CFile* f = GetFile(pszFile, CFile::shareDenyNone | CFile::modeRead, &e);
	if (f) {
		ULONGLONG ullLength = f->GetLength();
		str.Format("%s Bytes", (LPCTSTR) Library::fc(ullLength)); // format thousands
		f->Close();
		delete f;
	}
	else {
		str = _T("(File has not been saved yet)");
	}
	return str;
}



// Get number of objects in this document, as a string.
CString BDoc::GetNumberOfObjectsString() {
	CString str;
	BObject* pobjRoot = m_pobjRoot;
	BObject* pobjSystem = GetObject(rootSystem);
	UINT nSystem = pobjSystem->GetChildCount(TRUE) + 2; // 1 for rootsys and 1 for rootmain
	UINT nTotal = pobjRoot->GetChildCount(TRUE) + 1; // 1 for root object
	UINT nUser = nTotal - nSystem;
//	str.Format("%d (%d User, %d System)", nTotal, nUser, nSystem);
//,	str.FormatThousands("%t (%t User, %t System)", nTotal, nUser, nSystem);
//	m_txtObjects.SetWindowText(str);
//	::GetNumberFormat(LOCALE_USER_DEFAULT, 0, str, NULL, szFormatted, nChars);
//	m_txtObjects.SetWindowText(szFormatted);
	str.Format("%s (%s User, %s System)", (LPCTSTR) Library::fc(nTotal), (LPCTSTR) Library::fc(nUser), (LPCTSTR) Library::fc(nSystem));
	return str;
}





// This is a copy of MFC's CDocument's version of OnOpenDocument, 
// modified to use our own CCryptoFile instead of CFile, to allow for encryption.
BOOL BDoc::OnOpenDocumentEx(LPCTSTR lpszPathName) {

	//trace("BDoc::OnOpenDocumentEx(%s)\n", lpszPathName);

	if (IsModified())
		TRACE0("Warning: OnOpenDocument replaces an unsaved document.\n");

	CFileException fe;
	//` We overrode GetFile so it returns a CCryptoFile (can treat as a CFile though)
	CFile* pFile = GetFile(lpszPathName, CFile::modeRead|CFile::shareDenyWrite, &fe);
	if (pFile == NULL) {
		ReportSaveLoadException(lpszPathName, &fe, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		return FALSE;
	}

	DeleteContents();
	SetModifiedFlag();  // dirty during de-serialize

	// Note: Buffer size of archive is the default 4096 bytes
	// Note use of CCryptoArchive which extends CArchive
	CCryptoArchive loadArchive(pFile, CArchive::load | CArchive::bNoFlushOnDelete);
	loadArchive.m_pDocument = this;
	loadArchive.m_bForceFlat = FALSE;
	TRY {
		CWaitCursor wait;
		if (pFile->GetLength() != 0)
			Serialize(loadArchive);     // load me
		loadArchive.Close();
		ReleaseFile(pFile, FALSE);
	}
	CATCH_ALL(e) {
		ReleaseFile(pFile, TRUE); 
		DeleteContents();   // remove failed contents  

		TRY {
			ReportSaveLoadException(lpszPathName, e, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC); 
		}
		END_TRY
		DELETE_EXCEPTION(e);
		return FALSE;
	}
	END_CATCH_ALL


	// After serializing data, finish handling upgrade of file structure and/or data model.
	if (m_bUpgradeFileStructure || m_bUpgradeDataModel) {
		// But don't if we're in the process of upgrading the template file itself (strictly admin stuff here)
		if (app.m_strTemplatePath != lpszPathName) {
			// Synchronize file with Template.neo if data model needs to be upgraded
			if (m_bUpgradeDataModel) {
				// Open the template file and load all its data
				BDoc* pdocTemplate = new BDoc();
//				if (!pdocTemplate->OnOpenDocumentEx(app.m_strTemplatePath))
				if (!pdocTemplate->CreateTemplate()) { // will throw exception if fails
//					CString s;
//					s.Format("Unable to open template file. The file '%s' should be in the NeoMem application folder.", (LPCTSTR) app.m_strTemplateFileName);
//					AfxMessageBox(s, MB_ICONEXCLAMATION);
					return FALSE;
				}
				// Synchronize this document with the template document
				Synchronize(pdocTemplate);
				ASSERT_VALID(this);
				ASSERT_VALID(pdocTemplate);
				// Delete the template document contents
				pdocTemplate->DeleteContents();
				delete pdocTemplate;
			}

			// Rename old file to _BeforeUpgrade
			CString strBeforeUpgrade = GetModifiedName(lpszPathName, _T("_BeforeUpgrade"));
			_trename(lpszPathName, strBeforeUpgrade);

			// Save new version of file under old name (overwriting the existing file).
			//. gives a confusing message if it fails
			// fixed? bomb here - some links still soft
			if (!this->OnSaveDocumentEx(lpszPathName)) {
				//. trigger an error in error dialog instead
				AfxMessageBox("Unable to save new version of file. Internal problem encountered in conversion to new format. ", MB_ICONEXCLAMATION); 
				return FALSE;
			}
		}
	}

	SetModifiedFlag(FALSE);     // start off with unmodified

	return TRUE;
}





// Synchronize this document with the template file. 
// Walk through template file system objects, 
// Add, edit, delete to synchronize this file with it.
// Set all links in template file to soft links so can copy over to this file.
// After copying over, convert back to hard links. 
void BDoc::Synchronize(BDoc* pdocTemplate) {
	ASSERT_VALID(this);
	ASSERT_VALID(pdocTemplate);

	// Get template file's system object
	BObject* pobjTemplate = pdocTemplate->GetObject(rootSystem);

	//, get number of system objects so can update progress bar

	// Convert all template file system object links to soft links
	pobjTemplate->ConvertToSoftLinks(TRUE);

	// Recurse through template file system objects and synchronize this file with them (add objects)
	SynchronizeRecurse(pobjTemplate);

	// Now recurse through objects and update their properties
	SynchronizeRecurseProps(pobjTemplate);

	// Now get this document's system object
	BObject* pobjThis = GetObject(rootSystem);

	// Now check for objects to delete
	// Walk through this file's system objects and see if they exist in the template file - if not there 
	// then should be deleted.
	//, for now, don't do this - assume that we never delete objects, will just disable them or something.
	// currently this brings up messagebox if the object is referenced in the file.
//	SynchronizeDelete(pobjThis, docTemplate);

	// Convert all of this document's system object soft links to hard links.
	// Note: Any user-defined classes and properties, etc. will already be hard links - only the
	// objects transferred from the template file will have soft links at this point.
	pobjThis->ConvertToHardLinks(TRUE);

//	AfxMessageBox("Your file has been updated.", MB_ICONINFORMATION);
}


// Synchronize 
void BDoc::SynchronizeRecurse(BObject* pobjTemplate) {
	// note: doing this object then children so that if a new folder has been added 
	// the parent will be added first. if this creates problems may need to convert parent link
	// to a soft link also. 

	ASSERT_VALID(this);
	ASSERT_VALID(pobjTemplate);

	// See if this object exists in this file already
	ULONG nID = pobjTemplate->GetObjectID();
	ASSERT(nID < lngUserIDStart); // should be a system object
	BObject* pobjThis = this->GetObjectNull(nID);
	if (pobjThis == NULL) {
		// Doesn't exist - we need to add it
		pobjThis = new BObject();
		pobjThis->SetDoc(this);
//		pobjThis->CopyFrom(pobjTemplate);
		pobjThis->SetObjectID(nID);
		this->AddObjectToIndex(pobjThis);
	}
	else {
		// Exists - update it
		// prob: if this object links to an object that has not been synchronized yet, this will bomb.
		// ie need to add objects before synchronizing their properties...
//		pobjThis->CopyFrom(pobjTemplate);
	}

	// Walk through any child objects and recurse
	BObjects* paChildren = pobjTemplate->GetChildren();
	if (paChildren) {
		ASSERT_VALID(paChildren);
		int nItems = paChildren->GetSize();
		for (int i = 0; i < nItems; i++) {
			BObject* pobjChild = DYNAMIC_DOWNCAST(BObject, paChildren->GetAt(i));
			ASSERT_VALID(pobjChild);
			SynchronizeRecurse(pobjChild);
		}
	}

	// Now synchronize the object's properties... 
//	pobjThis->CopyFrom(pobjTemplate);

}



// Synchronize all properties in this file to those in the specified object.
void BDoc::SynchronizeRecurseProps(BObject* pobjTemplate) {
	// Find object in this document
	ULONG nID = pobjTemplate->GetObjectID();
	ASSERT(nID < lngUserIDStart); // should be a system object
	BObject* pobjThis = this->GetObject(nID);
	ASSERT(pobjThis); // must already have been added if new

	// Now update its properties
	pobjThis->CopyFrom(pobjTemplate);

	// Walk through any child objects and recurse
	BObjects* paChildren = pobjTemplate->GetChildren();
	if (paChildren) {
		ASSERT_VALID(paChildren);
		int nItems = paChildren->GetSize();
		for (int i = 0; i < nItems; i++) {
			BObject* pobjChild = DYNAMIC_DOWNCAST(BObject, paChildren->GetAt(i));
			ASSERT_VALID(pobjChild);
			SynchronizeRecurseProps(pobjChild);
		}
	}

}



// walk through system objects and see if they are in the template document also - 
// if not, delete them. 
void BDoc::SynchronizeDelete(BObject *pobjThis, BDoc* pdocTemplate) {
	ASSERT_VALID(this);
	ASSERT_VALID(pobjThis);
	ASSERT_VALID(pdocTemplate);

	ULONG nID = pobjThis->GetObjectID();
	BObject* pobjTemplate = pdocTemplate->GetObjectNull(nID);
	if (pobjTemplate == NULL) {
		// This object is no longer in the template file, so we need to delete it
		TRACE("Deleting system object that is no longer in template file: %s\n", pobjThis->GetName(TRUE));
		//, if this object is referenced in the file, will bring up a messagebox. not good here
		pobjThis->DeleteObject(FALSE, FALSE);
	}
	
	// Recurse through children
	// Walk through any child objects and recurse
	BObjects* paChildren = pobjThis->GetChildren();
	if (paChildren) {
		ASSERT_VALID(paChildren);
		int nItems = paChildren->GetSize();
		for (int i = 0; i < nItems; i++) {
			BObject* pobjChild = DYNAMIC_DOWNCAST(BObject, paChildren->GetAt(i));
			ASSERT_VALID(pobjChild);
			SynchronizeDelete(pobjChild, pdocTemplate);
		}
	}

}




void BDoc::UpdateAllViewsEx(CView *pSender, LPARAM lHint /*= 0L*/, CObject* pHint /*= NULL*/) {
	//trace("UpdateAllViewsEx - %s\n", app.GetHintName(lHint)); 
	UpdateAllViews(pSender, lHint, pHint);
}


// Handle upgrading the current file (after asking user)
void BDoc::UpgradeFile(CArchive& ar) {
	// Show file upgrade message
	CString strBackupName = GetModifiedName(ar.GetFile()->GetFileName(), "_BeforeUpgrade");
	CString str;
//			str.Format("The file \"%s\" needs to be upgraded to a new file structure in order to be read. \n\n"
//							"NeoMem will upgrade the file so it can be used with the current version, "
//							"and keep a backup copy of the old version as \"%s\". \n\n"
//							"Click OK to continue or Cancel if you do not want to upgrade this file.",
//							(LPCTSTR) ar.GetFile()->GetFileName(),
//							(LPCTSTR) strBackupName
//							);
	str.Format("You have selected a file created in a previous version of NeoMem. \n"
					  "Click OK to convert the file so it can be read, or Cancel to leave it as is. \n\n"
					"(If you click OK the old version will be kept with the name \"%s\").",
//							(LPCTSTR) ar.GetFile()->GetFileName(),
					(LPCTSTR) strBackupName
					);
	if (IDCANCEL == AfxMessageBox(str, MB_ICONQUESTION + MB_OKCANCEL)) {
		// User hit cancel so exit without message
		AfxThrowUserException();
	}

/*
	// Show file upgrade dialog
	CDialogUpgradeFile dlg;
	dlg.m_strFile = ar.GetFile()->GetFileName();
	if (IDCANCEL == dlg.DoModal())
	{
		// User hit cancel so exit without message
		AfxThrowUserException();
	}
*/

	// - read file into memory, modifying as necessary
	// - rename file to _BeforeUpgrade
	// - save as old name

	// Update File Structure
	// Individual Serialize routines will handle updating the file structure automatically - 
	// they should check the nFileVersion variable and fill in whatever variables are missing 
	// with default values.
	// So we just need to update the file structure version variable here.
	// On file save, all Serialize code will write the new file structure.
	if (m_bUpgradeFileStructure) {
		m_nVersionFileStructure = versionFileStructure;
	}

	// Update data model
	// this will require synchronizing this file with the new template.neo file:
	// open template.neo, walk through all system objects, ADD, EDIT, DELETE to synchronize files
	// walk recursively through all objects in template file
	// if system object, see if it's in the userfile
	//		if it is, make sure they have the same values and property values
	//		if it isn't, add it to the userfile and copy all the values and property values
	// then do reverse - 
	// walk recursively through all objects in userfile
	// if system object, make sure it's in the template file
	//		if it isn't, delete it from the userfile (what if cascade delete though?)
	if (m_bUpgradeDataModel) {
		// Update data model variable now
		m_nVersionDataModel = versionDataModel;
	}

	// After this Serialize routine finishes, the calling function will synchronize this file
	// with the new Template.neo file if necessary, rename the old file to _BeforeUpgrade,
	// and resave this file under the old name. 
}





void BDoc::UITest() {
	// Test this document and the UI
}



// get list of properties for export - start with id, class, name, desc, etc.
// then add everything else.
// actually, you want to recurse through the specified objects, 
// gather the props to include,
int BDoc::GetProperties(BDataLink &datProps, BObject* pobj/*=NULL*/) {
	ASSERT_VALID(this);
	ASSERT_VALID(&datProps);

	datProps.SetMultiple();
	datProps.AddLinkID(propObjectID, this);
//	datProps.AddLinkID(propClassID, this);
	datProps.AddLinkID(propClassName, this);
	datProps.AddLinkID(propName, this);
//	datProps.AddLinkID(propParentID, this); 
	datProps.AddLinkID(propParentName, this);
	datProps.AddLinkID(propDescription, this);

	// get list of all properties, and add them to the list 
	// (duplicates will be ignored)
	BObject* pobjProps = this->GetObject(folderProperties);
	ASSERT_VALID(pobjProps->GetChildren());
	int nProps = pobjProps->GetChildren()->GetSize();
	for (int i = 0; i < nProps; i++) {
		BObject* pobjProp = DYNAMIC_DOWNCAST(BObject, pobjProps->GetChildren()->GetAt(i));
		ASSERT_VALID(pobjProp);
		// just user properties
		if (!(pobjProp->GetFlag(flagAdminOnly))) {
			OBJID idProp = pobjProp->GetObjectID();
			// and not propRtfText (yet)
			if ((idProp != propRtfText) && (idProp != propPlainText)) {
				datProps.AddLink(pobjProp);
			}
		}
	}
	// now add propRtfText, so it's the last one
//	datProps.AddLinkID(propRtfText, this); // yucky. leave out. 
	datProps.AddLinkID(propPlainText, this);


	//. if pobj specified, recurse downwards from there and gather props actually used.
	// then use this list to remove unused properties from datProps. 
	if (pobj) {

	}

	nProps = datProps.GetLinkCount();
	return nProps;
}




// 1.3 use this instead of opendocumentex on template file
BOOL BDoc::CreateTemplate() {
	//. should throw exception if fails

	// wrap this for now
	//. maybe easier to switch to an xml format? but icons...
	return OnOpenDocument(app.m_strTemplatePath);

}



void BDoc::RemoveObjectFromIndex(OBJID idObject)
{
	// "Looks up the map entry corresponding to the supplied key; then, if the key is found, removes the entry."
	m_mapObjects.RemoveKey(idObject);
}




// Prompt user for filename if not specified
//void BDoc::Save(LPCTSTR pszFilename, BOOL bReplace) {
void BDoc::Save(BOOL bReplace) {
//	if (pszFilename)
//		DoSave(pszFilename, bReplace);
//	else
//		OnFileSave();

	// uh, this always prompts for filename? 
	// then get filename from this-> 
	// check if it's uninitialized, call dosave or onfilesave
	// and breplace?
	OnFileSave();

}


// Prompt user for filename
void BDoc::SaveAs(LPCTSTR pszFilename, BOOL bReplace) {
	DoSave(pszFilename, bReplace);
}


// this just provides access to cdoc's protected method
void BDoc::OnFileSave()
{
	CDocument::OnFileSave();
}



// void BDoc::SetEncryption(EncryptionTypes nEncryption, LPCTSTR pszPassword = NULL) {
void BDoc::SetEncryption(EncryptionTypes nEncryption, LPCTSTR pszPassword) {
		m_nEncryptionType = nEncryption;
//?	if (pszPassword)
		m_strPassword = pszPassword;
}



