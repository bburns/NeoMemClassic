
// BObject



#include "precompiled.h"

#include "BData.h"
#include "BDataLink.h"
#include "BDataLong.h"
#include "BDataPersonName.h"
#include "BDataString.h"
#include "BObject.h"
#include "ConstantsDatabase.h"
#include "BDoc.h"


//. get rid of these dependencies
#include "NeoMem.h"
#include "Constants.h"
#include "DialogEditProperty.h"
#include "Hint.h"
#include "PageObjectGeneral.h"
#include "PropertySheetEx2.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// last parameter is version number
IMPLEMENT_SERIAL(BObject, CObject, VERSIONABLE_SCHEMA | versionFileStructure)


// Construction/Destruction
//--------------------------------------------------------------------------



BObject::BObject(BDoc& doc, OBJID idClass, LPCTSTR pszName, OBJID idParent, OBJID idIcon, ULONG lngFlags) :
	m_lngObjectID(0), id(m_lngObjectID)
{
	ASSERT_VALID(&doc);

	Init();

	// set doc
	m_pDoc = &doc;

	// default class
	if (idClass == 0)
		idClass = classPaper;

	BObject* pobjClassDef = doc.GetObject(idClass);

	// default name, eg "New Fish"
	CString strName; // don't move into brackets - needs to stay alive till end
	if (pszName == 0) {
		pobjClassDef->GetClassDefNewName(strName);
		pszName = (LPCTSTR) strName; 
	}

	// default location - current object, with exceptions
	if (idParent == 0) {
		//, should be a classdef property. just do a switch here for now
		if (idClass == classClass)
			idParent = folderClasses;
		else
			idParent = doc.GetCurrentObject()->id;
	}

	// default flags
	// The default flags come from the classdef.
	if (lngFlags == 0) {
//x		BDataFlags* pdatFlags = DYNAMIC_DOWNCAST(BDataFlags, pobjClassDef->GetPropertyData(propObjectFlags));
//		if (pdatFlags) {
//			lngFlags = pdatFlags->GetFlags();
//			delete pdatFlags;
//		}
		lngFlags = pobjClassDef->GetPropertyFlags(propObjectFlags);
	}



	// set properties
	m_lngClassID = idClass;
	
	//, use lpctstr. 
	//, make #define PSZ and PCSZ
	SetName(pszName);

	BObject* pobjParent = doc.GetObject(idParent);
	SetParent(pobjParent); //, api should take id not pobj

	SetFlags(lngFlags);
	SetIconID(idIcon); //, leave as direct ref for now (too confusing yet!)


//,
/*
	// Special properties for folder objects
	//, put in bfolder? but ui may say new bobject(class=folder)... hmm...
	// oh, but bfolder calls bobjcect
	// but here we don't get default class. shit. 
	if (idClass == classFolder) {
		// Default class
		BObject* pobjDefaultClass = dlg.m_pobjDefaultClass;
		if (pobjDefaultClass != 0) {
			ASSERT_VALID(pobjDefaultClass);
//,			BClassDef& objClassDef = (BClassDef&) objNew; //?
			objNew.SetPropertyLink(propDefaultClass, pobjDefaultClass->id, FALSE, FALSE);
			// Initialize column array based on default class
			objNew.SetColumnsBasedOnClass(pobjDefaultClass);
		}
	}
*/

	// Validate object
//	ASSERT_VALID(&obj);

	// Add object to database (and tell views)
	doc.AddObject(this); 

}



// better to use member initialization lists - copy and paste,
// because calling another routine and assigning values would be so much slower
// ^ oh, i see - i didn't see this text and changed them to an Init routine, 
// because the init lists had gotten out of sync, and thought it was dangerous. 
// you would want it fast because this is BObject. 
BObject::BObject() : 
	m_lngObjectID(0), id(m_lngObjectID) 
{
	Init();
}


BObject::BObject(OBJID idClass) : 
	m_lngObjectID(0), id(m_lngObjectID) 
{
	Init();
	m_lngClassID = idClass;
}


void BObject::Init() {
	m_lngObjectID = 0;
	m_lngClassID = 0;
	m_lngFlags = 0;
	m_lngIconID = 0;
	m_paProperties = NULL;
	m_paChildren = NULL;
	m_pobjParent = NULL;
	m_pdat = NULL;
	m_pDoc = NULL;
	m_bytViewHeight = 50; // 50% default
}


// BObject factory
// Only doc is required
// static
BObject& BObject::New(BDoc& doc, OBJID idClass, LPCTSTR pszName, OBJID idParent, OBJID idIcon, ULONG lngFlags) {
	BObject* pobj = new BObject(doc, idClass, pszName, idParent, idIcon, lngFlags);
	ASSERT(pobj);
	return *pobj;
}



BObject::~BObject()
{
	// Clear the object, release all its children and properties recursively, and release
	// the collection objects themselves.

	// Release object data
	if (m_pdat)
	{
		ASSERT_VALID(m_pdat);
		delete m_pdat; // destructor for bdata objects will release any heap memory
	}

	// Release child objects recursively (destructor recursively deletes child objects)
	if (m_paChildren)
	{
		ASSERT_VALID(m_paChildren);
		delete m_paChildren;
	}

	// Release property objects recursively (descructor recursively deletes property objects)
	if (m_paProperties)
	{
		delete m_paProperties;
	}
}


/*
// Copy constructor (will be good for right click Copy...)
BObject::BObject( const BObject& a )
{
	mlngClassID = a.mlngClassID;
	mstrText = a.mstrText;
}
BObject& BObject::operator=( const BObject& a )
{
    m_years = a.m_years; return *this;
}
BOOL BObject::operator==(BObject a)
{
    return m_years == a.m_years;
}
*/




// Save to or load object from file.
void BObject::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);    // Always call base class Serialize.

	// m_myob.Serialize(ar);    // Call Serialize on embedded member.
	// m_pOther->Serialize(ar);    // Call Serialize on objects of known exact type.
	// use << for dynamic pointer for which the exact type is not known (eg m_pdat)

	if (ar.IsStoring())
	{
		// Keep track of old versions here for reference

		// Save to file
		// Note: We don't serialize m_pobjParent because that is reconstructed in loading the file
		ar << m_lngObjectID;
		ar << m_lngClassID;
		ar << m_lngIconID;
		ar << m_lngFlags;
		ar << m_pdat;
		ar << m_paChildren;
		ar << m_paProperties;
		ar << m_bytViewHeight; //, remove in v2

		// Update progress bar
		app.GetProgressBar().StepIt();
	}

	else

	// Read object from file
	{
		// Store pointer to document in this bobject
		m_pDoc = DYNAMIC_DOWNCAST(BDoc, ar.m_pDocument);
		ASSERT_VALID(m_pDoc);

		// Get version of object as stored in file
		// (not actually necessary until the schema changes)
//		int nFileVersion = ar.GetObjectSchema();
//		switch (nFileVersion)
//		{
//			case 1:
				// Read in current version of this object
				// Load from file
				ar >> m_lngObjectID;
				ar >> m_lngClassID;
//				if (m_pDoc->m_nObject==372 && m_lngClassID==500336)
//					TRACE("hi\n");
//				TRACE("reading obj %d (id %d, classid %d):\n", m_pDoc->m_nObject, m_lngObjectID, m_lngClassID);
				ar >> m_lngIconID;
				ar >> m_lngFlags;
				ar >> m_pdat;
				ar >> m_paChildren;
				ar >> m_paProperties;
				ar >> m_bytViewHeight; //, remove in v2
				
//				break;
//			case 2:
//				// read in future version of this object
//				break;
//			default:
//				// report unknown version of this object
//				break;
//		}

		// Make sure we're marked as owning the children and property objects
		if (m_paChildren) m_paChildren->SetOwnership(TRUE);
		if (m_paProperties) m_paProperties->SetOwnership(TRUE);

		//, Clear rtf position property?
		// could eventually define this prop to be non-serializable
//		if (m_lngClassID == propRtfInsertionPoint)
//		{
//			ASSERT(m_pdat->IsKindOf(RUNTIME_CLASS(BDataLong)));
//			BDataLong* pdatLong = (BDataLong*) m_pdat;
//			pdatLong->m_lngValue = 0;
//		}

		// Remove high priority flag if any (temp)
//		m_lngFlags &= ~flagHighPriority;




/*
		// Convert from BDataArray/BDataColumnInfo to BDataColumns!
		// if the classid of this object is propColumnInfoArray or propObjectColumnInfoArray, do this.
		if (m_lngClassID == propColumnInfoArray || m_lngClassID == propObjectColumnInfoArray)
		{
			// m_pdat should be a bdataarray object containing bdatacolumninfo objects
			// verify this
			ASSERT(m_pdat->IsKindOf(RUNTIME_CLASS(BDataArray)));
			BDataArray* pdatArray = (BDataArray*) m_pdat;
			// walk through the array, getting the column info and adding it to a bdatacolumns object
			BDataColumns* pdatCols = new BDataColumns;
			ASSERT_VALID(pdatCols);

			int nCols = pdatArray->m_apdat.GetSize();
			for (int i = 0; i < nCols; i++)
			{
				BDataColumnInfo* pc = DYNAMIC_DOWNCAST(BDataColumnInfo, pdatArray->m_apdat.GetAt(i));
				ASSERT_VALID(pc);
				OBJID lngPropertyID = pc->idProperty;
				int nWidth = pc->m_nColWidth;
//				int nAlignment = pc->m_nColAlignment;
				pdatCols->InsertColumn(lngPropertyID, m_pDoc);
				pdatCols->SetColumnWidth(i, nWidth);
			}

			// now delete the old bdataarray and replace it with our new bdatacolumns object
			delete m_pdat;
			m_pdat = pdatCols;

		}
*/

/*
		// Convert from BDataArray/BDataArray/BDataView to BDataViews
		// If the classid of this object is propViewArrangement or propObjectViewArrangement, do this.
		if (m_lngClassID == propViewArrangement || m_lngClassID == propObjectViewArrangement)
		{
			// m_pdat should be a bdataarray object containing bdataarray objects
			// verify this
			ASSERT(m_pdat->IsKindOf(RUNTIME_CLASS(BDataArray)));
			BDataArray* pdatArray = (BDataArray*) m_pdat;

			int nIndex = 0;

			// walk through the array, getting the view arrays, and adding the views to a bdataviews object
			BDataViews* pdatViews = new BDataViews;
			ASSERT_VALID(pdatViews);
			pdatViews->m_avi.SetSize(10); // good enough for now

			// walk through tabs
			int nTabs = pdatArray->m_apdat.GetSize();
			for (int nTab = 0; nTab < nTabs; nTab++)
			{
				// for each tab, get the view array
				BDataArray* pdatViewArray = DYNAMIC_DOWNCAST(BDataArray, pdatArray->m_apdat[nTab]);
				ASSERT_VALID(pdatViewArray);
				int nViews = pdatViewArray->m_apdat.GetSize();

				// walk through view array, adding each view to our new array
				for (int nView = 0; nView < nViews; nView++)
				{
					BDataView* pv = DYNAMIC_DOWNCAST(BDataView, pdatViewArray->m_apdat[nView]);
					ASSERT_VALID(pv);
//					pdatViews->InsertView(nTab, nView, lngViewID, lngViewHeight);
					ViewInfo& rvi = pdatViews->m_avi[nIndex];
					rvi.m_lngViewID = pv->m_lngViewID;
					rvi.m_lngViewHeight = pv->m_lngViewHeight;
					nIndex++;
				}
				// at the end of each tab, add a 0 0 viewinfo object
				ViewInfo& rvi = pdatViews->m_avi[nIndex];
				rvi.m_lngViewID = 0;
				rvi.m_lngViewHeight = 0;
				nIndex++;
			}

			// Truncate array
			pdatViews->m_avi.SetSize(nIndex);

			// Now delete the old bdataarray and replace it with our new bdataviews object
			// note: this will delete all contained arrays and bdataview objects
			delete m_pdat;
			m_pdat = pdatViews;

		}
*/

/*
		// Convert from bdataname to bdatapersonname
		// If the classid of this object is person, do this.
		if (m_lngClassID == classPerson)
		{
			ASSERT(m_pdat->IsKindOf(RUNTIME_CLASS(BDataName)));
			BDataName* pdatOld = (BDataName*) m_pdat;

			BDataPersonName* pdatNew = new BDataPersonName;
			pdatNew->m_strFirst = pdatOld->m_strFirst;
			pdatNew->m_strMiddle = pdatOld->m_strMiddle;
			pdatNew->m_strLast = pdatOld->m_strLast;
			pdatNew->m_strSuffix = pdatOld->m_strSuffix;
			pdatNew->m_strNickname = pdatOld->m_strNickname;

			delete m_pdat;
			m_pdat = pdatNew;
		}
*/

/*
		// Convert from bdatalinks to bdatalink
		if (m_pdat->IsKindOf(RUNTIME_CLASS(BDataLinks)))
		{
			BDataLinks* pdatLinks = (BDataLinks*) m_pdat;
			BDataLink* pdatLink = new BDataLink;
			pdatLink->SetMultiple();
			// walk through array
			int nItems = pdatLinks->m_apobj.GetSize();
			for (int i = 0; i < nItems; i++)
			{
				BObject* pobj = (BObject*) pdatLinks->m_apobj.GetAt(i);
				ASSERT_VALID(pobj);
				pdatLink->AddLink(pobj);
			}
			delete pdatLinks;
			m_pdat = pdatLink;
		}
*/

		// If this is an object (as opposed to a property), add it to the document's index
		if (m_lngObjectID)
		{
			// Add the object to the index
			// Note: If for some reason the ObjectID is already occupied, this routine will
			// try getting the next valid ID until it finds an empty space
			m_pDoc->AddObjectToIndex(this);

			// Also walk through the children and set their parent property to point to this object
			// BUG:: Old version of SetParent just set m_pobjParent, then I changed it for some other
			// part of the program, and this wound up creating a copy of each object on load.
			// Problem was semantics - should have created another routine called Move.
			// Er, this is BObjects.SetParent, not BObject. But ditch that routine,
			// since this is the only place it's used. 
			if (m_paChildren) {
//				m_paChildren->SetParent(this);
				ASSERT_VALID(m_paChildren);
				int nItems = m_paChildren->GetSize();
				for (int i = 0; i < nItems; i++)
				{
					BObject* pobj = DYNAMIC_DOWNCAST(BObject, m_paChildren->GetAt(i));
					ASSERT_VALID(pobj);
					// Set the object's parent to the new parent
					pobj->m_pobjParent = this;
					
				}
			}

		}

		if (m_lngObjectID)
			//trace("    Read ObjectID %d: \"%s\"\n", m_lngObjectID, (LPCTSTR) GetPropertyString(propName));

		// Update progress bar
		app.GetProgressBar().StepIt();
	}

}


// Diagnostics
//---------------------------------------------------------------------------------------------------

#ifdef _DEBUG

void BObject::AssertValid() const
{
	CObject::AssertValid();
//	ASSERT(m_years > 0);
}

void BObject::Dump(CDumpContext& dc) const
{
    CObject::Dump(dc);
    //dc << m_years;
}

#endif //_DEBUG




// Methods
//---------------------------------------------------------------------------------------------------

// Get the icon used to represent this object, looking up the class chain if necessary
//, could store the looked up nIndex also for speed - just don't serialize it
OBJID BObject::GetIconID()
{
	ASSERT_VALID(this);

	// Return custom icon, if defined
	if (m_lngIconID) return m_lngIconID;

	// If the IconID is zero, then we get the IconID from the object's classdef and class chain
	return GetDefaultIconID();
}


// Get the default icon for this object, looking up the class chain if necessary
OBJID BObject::GetDefaultIconID()
{
	ASSERT_VALID(this);

	// Special case: if object is an Icon, then display itself as the icon
	if (m_lngClassID == classIcon) return m_lngObjectID;

	// If we're not on a classdef, get the object's classdef
	BObject* pobjClassDef = 0;
	if (m_lngClassID != classClass)
	{
		// First look to the object's classdef
		pobjClassDef = m_pDoc->GetObject(m_lngClassID);
		if (pobjClassDef->m_lngIconID) // leave these as direct refs, not method calls. 
			return pobjClassDef->m_lngIconID; // found icon, return it
	}
	else
		// We're already on a classdef, so just look up the class chain
		pobjClassDef = this;

	// If not there, then walk up through class chain till reach the root class
	do
	{
		pobjClassDef = pobjClassDef->GetParent();
		if (pobjClassDef == NULL) return iconDefault; // just in case
		ASSERT_VALID(pobjClassDef);
		if (pobjClassDef->m_lngIconID)
			return pobjClassDef->m_lngIconID; // found icon finally, return it
		if (pobjClassDef->GetObjectID() == rootSystem)
			return iconDefault; // reached the end of the line and no icon defined, so return default
	} while (TRUE);

}


// Gets the index for the icon associated with this object
// Will return the icon associated with the object's class if the
// object has not specified a custom icon.
int BObject::GetIconIndex()
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pDoc);
	OBJID lngIconID = GetIconID();
	// Return the index of the IconID in the image list
	return m_pDoc->GetIconIndex(lngIconID);
}





// Set the bobject's text representation (name)
// Different objects may use different properties to represent their text name 
// (eg person object parses this text into first middle last names, etc, stored in bdatapersonname object)
void BObject::SetName(const CString& strText)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pDoc);

	// If no bdata object exists yet we'll need to create one appropriate for this class.
	if (m_pdat == NULL)
	{
		BData* pData = m_pDoc->CreateBData(m_lngClassID);
		SetData(pData);
	}
	ASSERT_VALID(m_pdat);

	// Pass the property def of the name property so the BData object will know how to handle parsing.
	BObject* pobjPropDef = m_pDoc->GetObject(m_lngClassID);
	m_pdat->SetBDataText(strText, pobjPropDef);
}



// Sets object's underlying data, i.e. transfer ownership of the BData object to this BObject. 
// So don't delete pdat after calling this routine!
// Will dispose of any existing BData object attached to this BObject. 
BOOL BObject::SetData(BData* pdat)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pdat); 

	// Delete any existing data object first.
	// But note: if user is passing the same data as already exists, we don't want to delete it!
	if (m_pdat != NULL && m_pdat != pdat)
		delete m_pdat;

	m_pdat = pdat;

	return TRUE;
}




// Add an object to the list of children for this BObject.
// Child's parent should be NULL on entry.
// Will create a child list if none exists.
// Will optionally check the child list for the new child before adding it so no
// duplicate entries are formed. 
// Returns True if successful, False if not.
//, might want to insert alphabetically - pass param
//, or could set a flag indicating that the list is unsorted
BOOL BObject::AddChild(BObject *pobjChild, BOOL bCheckForDuplicates) {

	ASSERT_VALID(this);
	ASSERT_VALID(pobjChild);
	ASSERT(pobjChild->m_pobjParent == NULL);

	// Create new child list if not already there
	if (m_paChildren == NULL) {
		m_paChildren = new BObjects;
		m_paChildren->SetOwnership(TRUE); // this array will own the bobjects it points to
	}
	ASSERT_VALID(m_paChildren);
	
	// Make sure the child is not already in the array.
	// Optional because it's slow - currently only needed with synchronize routines. 
	BOOL bAlreadyThere = FALSE;
	if (bCheckForDuplicates) {
		bAlreadyThere = (m_paChildren->FindObject(pobjChild, FALSE) != -1);
	}

	// Now add the new child object to the child array, if it's not already there. 
	if (!bAlreadyThere)
		m_paChildren->Add(pobjChild); // may throw memory exception

	// Now set the parent of the child object
	pobjChild->m_pobjParent = this;

	// Make sure everything is set correctly
	ASSERT(pobjChild->m_pobjParent == this);
	ASSERT(this->m_paChildren->FindObject(pobjChild, FALSE) != -1);

	return TRUE;
}


// Remove the specified object from this object's children collection and set its parent to NULL.
// Returns True if successful.
BOOL BObject::RemoveChild(BObject* pobjChild)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pobjChild);
	ASSERT_VALID(pobjChild->m_pobjParent);
	ASSERT(pobjChild->m_pobjParent == this);

	if (m_paChildren)
	{
		ASSERT_VALID(m_paChildren);
		int nIndex = m_paChildren->FindObject(pobjChild);
		ASSERT(nIndex >= 0);
		if (nIndex >= 0)
		{
			m_paChildren->RemoveAt(nIndex);
			pobjChild->m_pobjParent = NULL;
			//, might want to delete the child collection if we're down to zero
			return TRUE;
		}
	}
	return FALSE;
}





// Routines
//-----------------------------------------------------------------------------------

// Add a property to the list of properties for this BObject.
// Will create a property list if none exists.
// Returns True if successful, False if not.
// Note: This is only called by FindProperty.
// Note: If you want to add a property to the class of an object, use something like 
//   classFish.SetPropertyLinksAdd(propObjectProperties, propCategory.id);
//, could pass parameters here so can specify position to add new property at
BOOL BObject::AddProperty(BObject *pobjProperty)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pobjProperty);

	// Create new property list if not already there
	if (m_paProperties == NULL)
	{
		m_paProperties = new BObjects;
		m_paProperties->SetOwnership(TRUE); // this array will own the bobjects it points to
	}
	ASSERT_VALID(m_paProperties);

	// Now add the new property to the properties list
	//, depending on parameters, use AddTail, AddHead, InsertBefore, or InsertAfter
//	POSITION pos = m_paProperties->AddTail(pProperty);
	m_paProperties->Add(pobjProperty); // may throw memory exception

	return TRUE;
}



// Delete the specified property value from the collection of properties.
// This will also set the document modified flag and update all views.
// Also will ask user if bAskUser is True.
// Note: This will not let pseudo properties be deleted (eg classname, classid, objectid, etc)
// Returns True if successful.
BOOL BObject::DeleteProperty(OBJID lngPropertyID, BOOL bSetModifiedFlag /* = TRUE */, 
											BOOL bUpdateViews /* = TRUE */, BOOL bAskUser /* = FALSE */)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pDoc);

	//, fall out if a pseudoprop


	if (m_paProperties)
	{
		ASSERT_VALID(m_paProperties);
		int nIndex = m_paProperties->FindObjectClassID(lngPropertyID);
		if (nIndex != -1)
		{
			// Ask user
			if (bAskUser)
			{
				CString str;
				CString strValue = GetPropertyString(lngPropertyID);
				strValue.Remove('\n');
				strValue.Remove('\r');
				if (strValue.GetLength() > 40)
					strValue = strValue.Left(40) + _T("...");
				str.Format("Are you sure you want to delete the property value '%s'?", (LPCTSTR) strValue);
				if (IDNO == AfxMessageBox(str, MB_ICONQUESTION + MB_YESNO))
				{
					return FALSE;
				}
			}

			// Bug: Forgot to delete the bobject also! Caused memory leak
			BObject* pobjProp = DYNAMIC_DOWNCAST(BObject, m_paProperties->GetAt(nIndex));
			ASSERT_VALID(pobjProp);

			// Remove the item from the array
			//, might want to override and delete the array if down to zero also
			m_paProperties->RemoveAt(nIndex);

			// Now nobody owns this bobject, so we can delete it
			delete pobjProp;

			// Set document modified flag
			if (bSetModifiedFlag)
				m_pDoc->SetModifiedFlag();

			// Update views
			//, make different methods for the different types of hint - will keep me from messing up CHint objects
			if (bUpdateViews)
			{
				CHint h;
				h.pobjObject = this;
				h.idProperty = lngPropertyID;
				m_pDoc->UpdateAllViewsEx(NULL, hintPropertyChange, &h);
			}

			return TRUE;
		}
	}
	return FALSE;
}



//, need to clean all this code up at some point - print it all out, etc.


// Find the specified property in this object's properties collection.
// Returns a pointer to the property object, or NULL if not found.
// If the property is an inheritable property, then will search through the classdef properties
// and up the class chain.
// If bAddIfNotFound is True, then will add the property to this object's property collection. 
// Also if the property is not already in this object's classdef's associated properties, then 
// it will add it there also.
//, what about pseudo properties?
BObject* BObject::FindProperty(OBJID lngPropertyID, BOOL bAddIfNotFound)
{
	ASSERT_VALID(this);

	//, handle pseudo properties here?

	// Search through this object's properties collection first.
	if (m_paProperties)
	{
		ASSERT_VALID(m_paProperties);
		int nItems = m_paProperties->GetSize();
		for (int i = 0; i < nItems; i++)
		{
			BObject* pobjPropertyValue = DYNAMIC_DOWNCAST(BObject, m_paProperties->GetAt(i));
			ASSERT_VALID(pobjPropertyValue);
			if (pobjPropertyValue->GetClassID() == lngPropertyID)
			{
				// Found, return the property value bobject
				return pobjPropertyValue;
			}
		}
	}

	// Property was not found in this object, so look to its class or up the class chain.
	// Do this for certain special properties which inherit their values from the class chain.
	// eg propDefaultClass -> propObjectDefaultClass
	// These are hardcoded for now.
	// NOTE: There is a relationship between the ObjectID's for the object version and the
	// classdef version of the property - add 500 to get to the classdef version
	BOOL bSearchClassChain = FALSE;
	BOOL bGetClassDef = FALSE;
	if (!bAddIfNotFound)
	{
		switch (lngPropertyID)
		{
		case propDefaultClass:
		case propColumnInfoArray:
		case propViewArrangement:
			lngPropertyID += 500; // convert to classdef version of the property
			bSearchClassChain = TRUE; // search up the class chain
			bGetClassDef = TRUE; // get the object's classdef
			break;
		case propObjectDefaultClass:
		case propObjectColumnInfoArray:
		case propObjectViewArrangement:
			bSearchClassChain = TRUE; // search up the class chain
			break;
		}
		//, see which version produces better code...
/*		if ((lngPropertyID >= propDefaultClass) && (lngPropertyID <= propObjectViewArrangement))
		{
			bSearchClassChain = TRUE;
			// Convert to classdef version of the property
			if (lngPropertyID < propObjectDefaultClass)
				lngPropertyID += 500;
		}
*/
		// If we need to search up the class chain for an inherited value, do so now.
		if (bSearchClassChain)
		{
			if (bGetClassDef)
			{
				// Get the object's classdef initially
				BObject* pobjClass = m_pDoc->GetObject(m_lngClassID);
				if (pobjClass)
					return pobjClass->FindProperty(lngPropertyID, bAddIfNotFound);
			}
			else
			{
				// Get the classdef's parent recursively until you reach the end of the line
				BObject* pobjParent = this->GetParent();
				if (pobjParent)
				{
					ASSERT_VALID(pobjParent);
					// Exit if you've reached the system root - property was never found
					if (pobjParent->GetObjectID() == rootSystem)
						return NULL;
					return pobjParent->FindProperty(lngPropertyID, bAddIfNotFound);
				}
			}
		}
	}

	// The property was not found, so add it to this object's property collection, 
	// if parameter specifies this.
	if (bAddIfNotFound) {

		BObject* pobjPropertyValue = new BObject(lngPropertyID);
		ASSERT_VALID(pobjPropertyValue);
		pobjPropertyValue->SetDoc(m_pDoc); // set document pointer

		// Create a bdata object as appropriate for this property, if specified
		// (If you're calling FindProperty in order to Set a value, you don't need this)
		// don't need this as we now create a copy of the bdata found wherever it's from
//		if (bCreateBData) {
//			BData* pdat = m_pDoc->CreateBData(lngPropertyID);
//			ASSERT_VALID(pdat);
//			pobjPropertyValue->m_pdat = pdat; // store it in the property object we just created
//		}
		
		// Add property to this object's property collection.
		// Note: This will create a properties collection if none exists.
		AddProperty(pobjPropertyValue);

		// Also add the property to the object's classdef's associated properties if not already there.
		BObject* pobjClass = GetClassObject();
		ASSERT_VALID(pobjClass);
		pobjClass->ClassDefAddProperty(lngPropertyID);
		
		return pobjPropertyValue;
	}

	return NULL;
}




COleDateTime& BObject::GetPropertyDate(OBJID lngPropertyID) {
	ASSERT_VALID(this);
	ASSERT(lngPropertyID);
	ASSERT_VALID(m_pDoc);

	BDataDate* pdat = DYNAMIC_DOWNCAST(BDataDate, GetPropertyData(lngPropertyID, TRUE));
	//. check for string
	// or always return pdat? 
	return pdat->GetDate();


/*

	// getpropdata
	BObject* pobjPropertyValue = FindProperty(lngPropertyID, FALSE);
	if (pobjPropertyValue) {
		ASSERT_VALID(pobjPropertyValue);
		BData* pdat = pobjPropertyValue->GetData()->CreateCopy();
		ASSERT_VALID(pdat);
		return pdat;
	}
	// Property not found - create a temporary bdata object if requested. 
	// Don't always want to create a temp bdata object, eg GetProperties calls this with propObjectProperties.
	// Same for icons - does GetPropertyData to test if has icon data or not - needs NULL.
	if (bCreateTempBDataIfNotFound) {
		// If no BData exists in the object or up the class chain, then create a temporary one 
		// as appropriate for the property and save it
		BData* pdat = m_pDoc->CreateBData(lngPropertyID);
		ASSERT_VALID(pdat);
		return pdat;
	}


	// getpropstring
	// Find the specified property in the property collection, and return its text representation.
	// For some properties, we want to get a pointer to the class or base class's property object,
	// ie get the inherited (default) value.
	BObject* pobjPropertyValue = FindProperty(lngPropertyID, FALSE);
	if (pobjPropertyValue) {
		// Rather than calling this routine again, could duplicate the code here.
		// Also, that way we could pass lngpropid to the bdata gettext, which sometimes needs it.
		ASSERT_VALID(pobjPropertyValue);
		if (pobjPropertyValue->GetData() == NULL) {
			// No bdata object exists yet - we need to create one appropriate for this class
			pobjPropertyValue->m_pdat = m_pDoc->CreateBData(pobjPropertyValue->GetClassID());
		}
		ASSERT_VALID(pobjPropertyValue->GetData());
		return pobjPropertyValue->GetData()->GetBDataText(m_pDoc, lngPropertyID);
	}
	else
		// Property was not found - need to return zero-length string.
		// we're using lpctstr so this should be okay
		return "";

*/

}




BOOL BObject::SetPropertyDate(OBJID lngPropertyID, LPCTSTR pszText, BOOL bSetModifiedFlag, BOOL bUpdateViews) {

	ASSERT_VALID(this);
	ASSERT(lngPropertyID);
	ASSERT(pszText); //? or could pass zero to clear value?
	ASSERT_VALID(m_pDoc);

//	BDataDate* pdat = new BDataDate(pszText);
//	SetData(pdat);

	//, darn this was so nice..., even though all wrong
//	SetData(new BDataDate(pszText)); //, won't work because temp obj will be deleted, i think
	


	//. Get the property definition and property type so we know what to do.
	// eg some properties are relationships (two way) and
	// we need to know what the mirror property is so we can set that as well.
//	BObject* pobjPropertyType = pobjPropertyDef->GetPropertyLink(propPropertyType);

	// If new string is empty, just delete the property
	if (strlen(pszText) == 0) {
		// why wouldn't you mark doc as changed and alert ui? oh, because it's set below
		DeleteProperty(lngPropertyID, FALSE, FALSE);
	}
	else {
//x		BObject* pobjPropertyValue = FindProperty(lngPropertyID, TRUE);
//x		ASSERT_VALID(pobjPropertyValue);
//x		pobjPropertyValue->SetName(pszText);

		BDataDate* pdat = DYNAMIC_DOWNCAST(BDataDate, GetPropertyData(lngPropertyID, TRUE));
		pdat->SetBDataText(pszText, NULL, FALSE); // setpropdate doesn't have a bshowerrormsg flag, so just set to false
		SetPropertyData(lngPropertyID, pdat, FALSE, FALSE); // flags set below
		// no need to delete pdat, as setpropdat takes ownership of it
	}

	// Set document modified flag if specified
	if (bSetModifiedFlag)
		m_pDoc->SetModifiedFlag(TRUE);

	// Update all views if specified
	if (bUpdateViews) {
		CHint h;
		h.pobjObject = this;
		h.idProperty = lngPropertyID;
		m_pDoc->UpdateAllViewsEx(NULL, hintPropertyChange, &h);
	}

	return TRUE;
}




// Set a property value, adding a property object if necessary.
// Returns True if property value set successfully or False if not.
// (Eg might be parsing something and format is invalid, so returns False).
// Return False for read-only properties. 
BOOL BObject::SetPropertyString(OBJID lngPropertyID, LPCTSTR pszText, 
					BOOL bSetModifiedFlag /* = TRUE */, BOOL bUpdateViews /* = TRUE */)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pDoc);
	ASSERT(pszText); // 2012-10 new

	// Check if property is read-only and give message.
	BObject* pobjPropertyDef = m_pDoc->GetObject(lngPropertyID);
	if (pobjPropertyDef->GetFlag(flagPropValueReadOnly))
	{
		//,, move this out of db into ui
		AfxMessageBox("This property value is read-only and cannot be changed.", MB_ICONINFORMATION);
		return FALSE;
	}


	// Handle pseudo properties first
	switch (lngPropertyID)
	{
		case propName:
			// Set the name (text representation) of the object
			SetName(pszText);
			break;

		case propObjectID:
			{
				// Parse string into long integer
				OBJID lngNewObjectID = atol(pszText);
				//, we'll need some special handling to make sure we don't overwrite an existing objectid
				// eg say bValid = SetObjectID(pdat->m_lng);
				m_lngObjectID = lngNewObjectID;
				break;
			}

		case propSize:
			{
				// read-only property
				return FALSE;
				break;
			}

		case propClassName:
			{
				// Classname is a string pseudo property, want to handle with bdatalink object.
				// Create a temporary bdata object to handle the parsing (since that's what bdatalink settext does).

				//, for error checking, could say
				// bValid = SetClassID(pobjClass->GetObjectID());
				// this code would make sure the id exists, that it's a class, and handle
				// converting any special properties to the new class (?)

				// Parse string into link to a class object using a temporary BDataLink object
				BDataLink* pdatLink = new BDataLink();
				ASSERT_VALID(pdatLink);

				// The propertydef will tell the BDataLink object where to start searching for matches
//				BObject* pobjPropertyDef = m_pDoc->GetObject(propClassName);
				if (pdatLink->SetBDataText(pszText, pobjPropertyDef))
				{
					// Bug: Was setting m_lngClassID instead of calling SetClassID!
//					m_lngClassID = pdatLink->GetLinkObjectID();
					OBJID lngNewClassID = pdatLink->GetLinkObjectID();
					SetClassID(lngNewClassID);
					delete pdatLink;
				}
				else
				{
					// Unable to parse text - return False
					delete pdatLink;
					return FALSE;
				}
				break;
			}

		case propFlags:
			{
				m_lngFlags = BDataFlags::StringToFlags(pszText);
				break;
			}

		default:
			{
				//. Get the property definition and property type so we know what to do.
				// eg some properties are relationships (two way) and
				// we need to know what the mirror property is so we can set that as well.
//				BObject* pobjPropertyType = pobjPropertyDef->GetPropertyLink(propPropertyType);

				// If new string is empty, just delete the property
				if (strlen(pszText) == 0)
				{
					DeleteProperty(lngPropertyID, FALSE, FALSE);
				}
				else
				{
					BObject* pobjPropertyValue = FindProperty(lngPropertyID, TRUE);
					ASSERT_VALID(pobjPropertyValue);
					pobjPropertyValue->SetName(pszText);
				}
				break;
			}
	}

	// Set document modified flag if specified
	if (bSetModifiedFlag)
		m_pDoc->SetModifiedFlag(TRUE);

	// Update all views if specified
	if (bUpdateViews)
	{
		CHint h;
		h.pobjObject = this;
		h.idProperty = lngPropertyID;
		m_pDoc->UpdateAllViewsEx(NULL, hintPropertyChange, &h);
	}

	return TRUE;
}



/*
BOOL BObject::SetDescription(LPCTSTR pszText) {
	return this->SetPropertyString(propDescription, pszText);
}
*/



// Get text property value.
// pass true for bMachineValue to get machine-readable version of data. used by export. 
// Warning: Since this uses m_strTextCache, you can't string a bunch of these calls 
// on one line, eg in a CString Format call
//xLPCTSTR BObject::GetPropertyString(OBJID lngPropertyID, BOOL bCreateTempBDataIfNotFound)
CString BObject::GetPropertyString(OBJID lngPropertyID)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pDoc);
	
	// Handle pseudo properties first
	switch (lngPropertyID)
	{

	case propName: 
		{
			// Return the text representation of the object (the name)
			if (m_pdat == NULL)
			{
				// No bdata object exists yet - we need to create one appropriate for this class
				m_pdat = m_pDoc->CreateBData(m_lngClassID);
			}
			ASSERT_VALID(m_pdat);
			return m_pdat->GetBDataText(m_pDoc, lngPropertyID);
/*			// could handle read-only summary here...
			LPCTSTR psz = m_pdat->GetBDataText(m_pDoc, lngPropertyID);
			if (*psz == 0)
				return "hi!";
			else
				return psz;
*/
		}
		break;
	
	case propSize:
		{
			ULONG lngSize = GetMemoryUsed(FALSE);
			m_strTextCache.FormatBytes(lngSize);
			return m_strTextCache;
		}
		break;

	case propClassID:
		{
			ASSERT (m_lngClassID);
			m_strTextCache.Format("%d", m_lngClassID);
			return m_strTextCache;
		}
		break;

	case propClassName:
		{
			// Return the name of the object's class
//			if (m_lngClassID)
//			{
				ASSERT (m_lngClassID);
				BObject* pobjClassDef = m_pDoc->GetObject(m_lngClassID);
				return pobjClassDef->GetPropertyString(propName);
//			}
//			else
//				return "ClassID is 0";
		}
		break;

	case propParentID:
		{
			BObject* pobjParent = GetParent();
			if (pobjParent)
			{
				ASSERT_VALID(pobjParent);
				m_strTextCache.Format("%d", pobjParent->GetObjectID());
			}
			else
			{
				m_strTextCache = CString("0");
			}
			return m_strTextCache;		
		}
		break;

	case propParentName:
		{
			BObject* pobjParent = GetParent();
			// Return the name of the object's parent (location)
			if (pobjParent)
			{
				ASSERT_VALID(pobjParent);
				return pobjParent->GetPropertyString(propName);
			}
			else
				// Return reference to an empty string (can't return "" because would 
				// return a reference to a temporary object)
				// we're using lpctstr so this should be okay
				return "";
		}
		break;


	case propObjectID: 
		// how do we handle this?
		// ie for this object, say it's the Icons folder,
		// we're displaying its properties
		// this method wants to return a reference to a cstring
		// but what cstring? this is a pseudo property
		// and you can't return a reference to a local cstring because it will get deleted
		// well for now, let's use the text cache (same as used by name though)
		{
			m_strTextCache.Format("%d", m_lngObjectID);
			return m_strTextCache;
		}
		break;

	case propPlainText:
		{
			// get plain text version of rtf text contents
			CString strRtf = GetPropertyString(propRtfText);
			app.ConvertRtfToPlain(strRtf, m_strTextCache);
			return m_strTextCache;
		}
		break;

	case propFlags:
		{
			return BDataFlags::FlagsToString(m_lngFlags);
		}
		break;


	default:
		{
			// Find the specified property in the property collection, and return its text representation.
			// For some properties, we want to get a pointer to the class or base class's property object,
			// ie get the inherited (default) value.
			BObject* pobjPropertyValue = FindProperty(lngPropertyID, FALSE);
			if (pobjPropertyValue)
			{
				// Rather than calling this routine again, could duplicate the code here.
				// Also, that way we could pass lngpropid to the bdata gettext, which sometimes needs it.
				ASSERT_VALID(pobjPropertyValue);
				if (pobjPropertyValue->GetData() == NULL)
				{
					// No bdata object exists yet - we need to create one appropriate for this class
					BData* pdatNew = m_pDoc->CreateBData(pobjPropertyValue->GetClassID());
					pobjPropertyValue->SetData(pdatNew);
				}
				ASSERT_VALID(pobjPropertyValue->GetData());
				return pobjPropertyValue->GetData()->GetBDataText(m_pDoc, lngPropertyID);
			}
			else
				// Property was not found - need to return zero-length string.
				// we're using lpctstr so this should be okay
				return "";
		}
		break;
	}
}






//xULONG BObject::GetPropertyFlags(OBJID idProperty, BOOL bCreateTempBDataIfNotFound)
ULONG BObject::GetPropertyFlags(OBJID idProperty)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pDoc);
	
	// Handle pseudo properties first
	switch (idProperty)
	{

	case propFlags:
		{
//			return BDataFlags::FlagsToString(m_lngFlags);
			return m_lngFlags;
		}
		break;


	default:
		{
			// Find the specified property in the property collection, and return its flag value.
			// For some properties, we want to get a pointer to the class or base class's property object,
			// ie get the inherited (default) value.
			BObject* pobjPropertyValue = FindProperty(idProperty, FALSE);
			if (pobjPropertyValue)
			{
				// Rather than calling this routine again, could duplicate the code here.
				// Also, that way we could pass lngpropid to the bdata gettext, which sometimes needs it.
				ASSERT_VALID(pobjPropertyValue);
				if (pobjPropertyValue->GetData() == NULL)
				{
					// No bdata object exists yet - we need to create one appropriate for this class
					pobjPropertyValue->m_pdat = m_pDoc->CreateBData(pobjPropertyValue->GetClassID());
				}
				BDataFlags* pdat = DYNAMIC_DOWNCAST(BDataFlags, pobjPropertyValue->GetData());
				ASSERT_VALID(pdat);
				ULONG lngFlags = pdat->GetFlags();
				return lngFlags;

			}
			else
				// Property was not found
				return 0;
		}
		break;
	}
}


//x
/*
BDataColumns* BObject::GetPropertyColumns(OBJID idProperty) {
	ASSERT_VALID(this);
	ASSERT_VALID(m_pDoc);
	
	// Find the specified property in the property collection, and return its value.
	// For some properties, we want to get a pointer to the class or base class's property object,
	// ie get the inherited (default) value.
	BObject* pobjPropertyValue = FindProperty(idProperty, FALSE);
	if (pobjPropertyValue)
	{
		// Rather than calling this routine again, could duplicate the code here.
		// Also, that way we could pass lngpropid to the bdata gettext, which sometimes needs it.
		ASSERT_VALID(pobjPropertyValue);
		if (pobjPropertyValue->GetData() == NULL)
		{
			// No bdata object exists yet - we need to create one appropriate for this class
			pobjPropertyValue->m_pdat = m_pDoc->CreateBData(pobjPropertyValue->GetClassID());
		}
		//, sheesh. anyway to clear this up?
		BDataColumns* pdat = DYNAMIC_DOWNCAST(BDataColumns, pobjPropertyValue->GetData());
		ASSERT_VALID(pdat);
		BData* pcopy = pdat->CreateCopy();
		ASSERT_VALID(pcopy);
		BDataColumns* pcopy2 = DYNAMIC_DOWNCAST(BDataColumns, pcopy);
		ASSERT_VALID(pcopy2);
		return pcopy2;
	}
	else
		// Property was not found
		return NULL;
}
*/



// Set the underlying data for a property.
// Makes a COPY of the BData object and saves it. 
// Each property BObject stores its data in the m_pdat member, which is a pointer to a BData object.
BOOL BObject::SetPropertyData(OBJID lngPropertyID, BData *pdatOrig, BOOL bSetModifiedFlag /* = TRUE */, BOOL bUpdateViews /* = TRUE */)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pDoc);
	ASSERT_VALID(pdatOrig);

	// Make copy of the BData object
	BData* pdat = pdatOrig->CreateCopy();

	// Handle pseudo properties first
	switch (lngPropertyID)
	{
	case propName:
		{
			// Save bdata to bobject.
			// This will work for both normal names (ie stored in bdatastring object) and people names
			// (stored in bdatapersonname object).
			if (m_pdat)
				delete m_pdat;
			m_pdat = pdat;
			break;
		}
	case propClassName:
		{
			// Note: pdat is a pointer to a temporary bdata object.
			BDataLink* pdatLink = DYNAMIC_DOWNCAST(BDataLink, pdat);
			ASSERT_VALID(pdatLink);
			// Bug: Was setting m_lngClassID directly instead of calling SetClassID, so name type wasn't being changed
//			m_lngClassID = pdatLink->GetLinkObjectID();
			OBJID lngNewClassID = pdatLink->GetLinkObjectID();
			SetClassID(lngNewClassID);
			//, memory leak? test
//			delete pdat;
			break;
		}
	default:
		{
			// For most properties, we just set the pointer to the new BData object.
			// If property doesn't already exist, it will be created.
			BObject* pobjPropertyValue = FindProperty(lngPropertyID, TRUE);
			ASSERT_VALID(pobjPropertyValue);
			pobjPropertyValue->SetData(pdat);
			break;
		}
	}

	// Set document modified flag if specified
	if (bSetModifiedFlag)
		m_pDoc->SetModifiedFlag(TRUE);

	// Update all views if specified
	if (bUpdateViews)
	{
		CHint h;
		h.pobjObject = this;
		h.idProperty = lngPropertyID;
		m_pDoc->UpdateAllViewsEx(NULL, hintPropertyChange, &h);
	}

	return TRUE;
}



// Get a COPY of the underlying data object associated with a property.
// Be sure to delete the bdata object when done with it. 
// To modify a value, write it back to the object with SetPropertyData.
BData* BObject::GetPropertyData(OBJID lngPropertyID, BOOL bCreateTempBDataIfNotFound)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pDoc);

	//.. bad - this same code is in GetPropertyString - put in FindProperty?
//	BObject* pobjPropertyValue = FindProperty(lngPropertyID, FALSE);

	// Handle pseudo properties first
	switch (lngPropertyID)
	{

	case propName: 
		{
			// Return the text representation of the object (the name)
			// Create a new one if necessary
			// Note: Object name BData's are always stored in the BObject's m_pdat pointer.
			if (m_pdat == 0)
			{
				// No bdata object exists yet - we need to create one appropriate for this class
				// (eg BDataString or BDataPersonName)
				m_pdat = m_pDoc->CreateBData(m_lngClassID);
			}
			ASSERT_VALID(m_pdat);

			// create copy
			BData* pdat = m_pdat->CreateCopy();

			ASSERT_VALID(pdat);
			return pdat;
			break;
		}

	case propClassName:
		{
			//. how could you return a bdata object for classname?
			// it would be a bdatalink
			// create a temporary bdata object of type bdatalink (as determined by propClassName's property type of proptypeLink)
			// the value of the link is the object's classdef
			// so would need to initialize the bdatalink object to point to the m_lngClassID object
			// user selects a different class
			// uieditvalue would change the bdatalink's m_pobj pointer to point to the new class object
			// and would broadcast the hintPropertyChange to all views

			// but how would that change get written back to the actual m_lngClassID variable?
			// the UIEditValue could handle that - ie check to see if the propertydef is a pseudo property
			// if so, do any special handling required

			// now what about F2 handler?
			// ie user is on classname pseudo property
			// and hits F2
			// types in a new class name, say "Tape"
			// hits enter
			// ui code calls SetPropertyString(propClassName, "Tape")
			// SetPropertyString code will pass the string to the appropriate bdata object for parsing

			// Create a temporary BData object appropriate for the propertydef (based on its property type)
			BData* pdat = m_pDoc->CreateBData(lngPropertyID);
			ASSERT_VALID(pdat);
			BDataLink* pdatLink = DYNAMIC_DOWNCAST(BDataLink, pdat);
			ASSERT_VALID(pdatLink);

			// Set link to point to this object's classdef
			BObject* pobjClass = m_pDoc->GetObject(m_lngClassID);
			pdatLink->SetLink(pobjClass);

			// Return a pointer to the temporary bdata object
			ASSERT_VALID(pdatLink);
			return pdatLink;

			break;
		}

	case propParentName:
		{
			// but how would that change get written back to the actual m_lngParentID variable?
			// the UIEditValue could handle that - ie check to see if the propertydef is a pseudo property
			// if so, do any special handling required

			// Create a temporary BData object appropriate for the propertydef (based on its property type)
			BData* pdat = m_pDoc->CreateBData(lngPropertyID);
			ASSERT_VALID(pdat);
			BDataLink* pdatLink = DYNAMIC_DOWNCAST(BDataLink, pdat);
			ASSERT_VALID(pdatLink);

			// Set link to point to this object's parent (may be zero if it's the root object)
			BObject* pobjParent = GetParent();
			if (pobjParent)
				ASSERT_VALID(pobjParent);
			pdatLink->SetLink(pobjParent); // zero is okay here

			// Return a pointer to the temporary bdata object
			ASSERT_VALID(pdatLink);
			return pdatLink;

			break;
		}

	case propObjectID:
		{
			// Create a temporary BData object appropriate for the propertydef (based on its property type)
			BData* pdat = m_pDoc->CreateBData(lngPropertyID);
			ASSERT_VALID(pdat);
			BDataLong* pdatLong = DYNAMIC_DOWNCAST(BDataLong, pdat);
			ASSERT_VALID(pdatLong);

			pdatLong->SetValue(m_lngObjectID);

			// Return a pointer to the temporary bdata object
			ASSERT_VALID(pdatLong);
			return pdatLong;

			break;
		}

	case propPlainText:
		{

			// Create a temporary BData object 
			BDataString* pdat = new BDataString;
			ASSERT_VALID(pdat);

			// get plain text version of rtf text contents, and store it in the bdata
			CString strRtf = GetPropertyString(propRtfText);
			app.ConvertRtfToPlain(strRtf, m_strTextCache);
			pdat->SetBDataText(m_strTextCache, 0, FALSE);
//			app.ConvertRtfToPlain(pszRtf, pdat->m_strText); // protected member
			
			// Return a pointer to the temporary bdata object
			ASSERT_VALID(pdat);
			return pdat;

			break;
		}

	default:
		{
			BObject* pobjPropertyValue = FindProperty(lngPropertyID, FALSE);
			if (pobjPropertyValue)
			{
				ASSERT_VALID(pobjPropertyValue);

				// this code was in getpropstring, and seemed good to have here also,
				//, though bobject should ensure mpdoc is not null
				if (pobjPropertyValue->GetData() == NULL) {
					// No bdata object exists yet - we need to create one appropriate for this class
					BData* pdatNew = m_pDoc->CreateBData(pobjPropertyValue->GetClassID());
					pobjPropertyValue->SetData(pdatNew);
				}
				ASSERT_VALID(pobjPropertyValue->GetData());

				BData* pdat = pobjPropertyValue->GetData()->CreateCopy();
				ASSERT_VALID(pdat);
				return pdat;
			}
		}
	}


	// how can this be ever reached if the default case returns from this fn?
	// oh, there's an if...

	// Property not found - create a temporary bdata object if requested. 
	// Don't always want to create a temp bdata object, eg GetProperties calls this with propObjectProperties.
	// Same for icons - does GetPropertyData to test if has icon data or not - needs NULL.
	if (bCreateTempBDataIfNotFound)
	{
		// If no BData exists in the object or up the class chain, then create a temporary one 
		// as appropriate for the property and save it
		BData* pdat = m_pDoc->CreateBData(lngPropertyID);
		ASSERT_VALID(pdat);
		return pdat;
	}

	return NULL;
}




// Set long property value.
BOOL BObject::SetPropertyLong(OBJID lngPropertyID, ULONG lngValue, 
										BOOL bSetModifiedFlag /* = TRUE */, BOOL bUpdateViews /* = TRUE */)
{
	ASSERT_VALID(this);

	//. handle pseudo properties here

	// Find/create property
	BObject* pobjPropertyValue = FindProperty(lngPropertyID, TRUE);
	ASSERT_VALID(pobjPropertyValue);

	// Create new data object to hold data and initialize it
	//, more efficient to use existing bdata object
//	if (pobjPropertyValue->m_pdat == NULL)
//	{
//		pobjPropertyValue->m_pdat = new BDataLong;
//	}
	BDataLong* pdat = new BDataLong;
	ASSERT_VALID(pdat);
	pdat->SetValue(lngValue);

	// Store the data in the property object
	pobjPropertyValue->SetData(pdat);

	// Set document modified flag if specified
	if (bSetModifiedFlag)
		m_pDoc->SetModifiedFlag(TRUE);

	// Update all views if specified
	if (bUpdateViews)
	{
		CHint h;
		h.pobjObject = this;
		h.idProperty = lngPropertyID;
		m_pDoc->UpdateAllViewsEx(NULL, hintPropertyChange, &h);
	}

	return TRUE;
}


// GetPropertyLong
//, how do we return NULL? need a variant type?
//xULONG BObject::GetPropertyLong(OBJID lngPropertyID, BOOL bCreateTempBDataIfNotFound)
ULONG BObject::GetPropertyLong(OBJID lngPropertyID)
{
	ASSERT_VALID(this);
	ASSERT(lngPropertyID);
//x	ASSERT(bCreateTempBDataIfNotFound == FALSE); // for now

	// Handle pseudo properties first
	switch (lngPropertyID)
	{
		case propObjectID:
			return m_lngObjectID;
		case propSize:
			return GetMemoryUsed(FALSE);
	}

	BObject* pobjPropertyValue = FindProperty(lngPropertyID, FALSE);
	if (pobjPropertyValue)
	{
		ASSERT_VALID(pobjPropertyValue);
		BDataLong* pdat = DYNAMIC_DOWNCAST(BDataLong, pobjPropertyValue->GetData());
		ASSERT_VALID(pdat);
		return pdat->GetValue();
	}
/*
	else
	// Set default values here, since no property value was found
	{
		switch (lngPropertyID)
		{
			// By default, any link property should allow multiple links.
			case propAllowedNumberOfLinks:			
				return -1;
		}
	}
*/
	return 0; 
}



// Set object that the specified property links to.
// Note: If idObj is zero, will delete the property bobject.
BOOL BObject::SetPropertyLink(OBJID idProperty, OBJID idObj, BOOL bSetModifiedFlag, BOOL bUpdateViews) {
	ASSERT_VALID(this);
	ASSERT(idProperty);

	if (idObj) {

		// Find/create property value bobject
		BObject* pobjPropertyValue = FindProperty(idProperty, TRUE);
		ASSERT_VALID(pobjPropertyValue);

		// Create new data object to hold data and initialize it
		//, more efficient to use existing bdata object
//		if (pobjPropertyValue->m_pdat == NULL) {
//			pobjPropertyValue->m_pdat = new BDataLong;
//		}
		BDataLink* pdat = new BDataLink;
		ASSERT_VALID(pdat);

		BObject* pobj = m_pDoc->GetObject(idObj);
		pdat->SetLink(pobj); //,, memory leak, no? need to delete what's already there

		// Store the data in the property object
		pobjPropertyValue->SetData(pdat);
	}
	else {
		// Link object is zero, so delete the property bobject.
		DeleteProperty(idProperty, FALSE, FALSE);
	}

	// Set document modified flag if specified
	if (bSetModifiedFlag)
		m_pDoc->SetModifiedFlag(TRUE);

	// Update all views if specified
	if (bUpdateViews) {
		CHint h;
		h.pobjObject = this;
		h.idProperty = idProperty;
		m_pDoc->UpdateAllViewsEx(NULL, hintPropertyChange, &h);
	}

	return TRUE;
}


BOOL BObject::SetPropertyLinks(OBJID idProperty, CObArray* pa, BOOL bSetModifiedFlag, BOOL bUpdateViews) {
	ASSERT_VALID(this);
	ASSERT(idProperty);

	if (pa) {
		ASSERT_VALID(pa);

		// Find/create property value bobject
		BObject* pobjPropertyValue = FindProperty(idProperty, TRUE);
		ASSERT_VALID(pobjPropertyValue);


		BDataLink* pdat = DYNAMIC_DOWNCAST(BDataLink, pobjPropertyValue->GetData());
		if (!pdat) {
			// Create new data object to hold data and initialize it
			pdat = DYNAMIC_DOWNCAST(BDataLink, m_pDoc->CreateBData(idProperty));
			pdat->SetMultiple();
			// store in pobj
			pobjPropertyValue->SetData(pdat);
		}
		ASSERT(pdat);
		ASSERT(pdat->IsMultiple()); //, eh

		//, ugh merge bdatalink with bobjects and standardize all this
		CObArray* paObject = pdat->GetLinkArray();
		ASSERT_VALID(paObject); // always true? 
		paObject->Append(*pa);

	}
	else {
		// Link object is zero, so delete the property bobject.
		DeleteProperty(idProperty, FALSE, FALSE);
	}



	// Set document modified flag if specified
	if (bSetModifiedFlag)
		m_pDoc->SetModifiedFlag(TRUE);

	// Update all views if specified
	if (bUpdateViews) {
		CHint h;
		h.pobjObject = this;
		h.idProperty = idProperty;
		m_pDoc->UpdateAllViewsEx(NULL, hintPropertyChange, &h);
	}

	return TRUE;
}


//x
/*
BOOL BObject::SetPropertyColumnsAdd(OBJID idProperty, OBJID idObj, BOOL bSetModifiedFlag, BOOL bUpdateViews) {
	ASSERT_VALID(this);
	ASSERT(idProperty);
	ASSERT(idObj);

	BDataColumns* pdatColumns = this->GetPropertyColumns(idProperty);
//x	pdatColumns->InsertColumn(idObj, &doc);
	pdatColumns->InsertColumn(idObj, m_pDoc);
	this->SetPropertyData(idProperty, pdatColumns, bSetModifiedFlag, bUpdateViews);
	delete pdatColumns;

	return TRUE;
}
*/


// Add an object to a multilink property
BOOL BObject::SetPropertyLinksAdd(OBJID idProperty, OBJID idObj, BOOL bSetModifiedFlag, BOOL bUpdateViews) {
	ASSERT_VALID(this);
	ASSERT(idProperty);
	ASSERT(idObj);

	// Find/create property value bobject
	//,, make a BPropValue class? 
	BObject* pobjPropertyValue = FindProperty(idProperty, TRUE);
	ASSERT_VALID(pobjPropertyValue);

	// similarly the bdata may or may not exist yet, but there's
	// no similar function to get/add one?

//x	pobjPropertyValue->SetPropertyData(idProperty, pdat
	BDataLink* pdat = DYNAMIC_DOWNCAST(BDataLink, pobjPropertyValue->GetData());
	if (!pdat) {
		pdat = DYNAMIC_DOWNCAST(BDataLink, m_pDoc->CreateBData(idProperty));
		pdat->SetMultiple();
		// store in pobj
		pobjPropertyValue->SetData(pdat);
	}
	ASSERT(pdat);
	ASSERT(pdat->IsMultiple());

	pdat->AddLinkID(idObj, m_pDoc);

	// Set document modified flag if specified
	if (bSetModifiedFlag)
		m_pDoc->SetModifiedFlag(TRUE);

	// Update all views if specified
	if (bUpdateViews) {
		CHint h;
		h.pobjObject = this;
		h.idProperty = idProperty;
		m_pDoc->UpdateAllViewsEx(NULL, hintPropertyChange, &h);
	}

	return TRUE;
}




//xOBJID BObject::GetPropertyLink(OBJID lngPropertyID, BOOL bCreateTempBDataIfNotFound)
OBJID BObject::GetPropertyLink(OBJID lngPropertyID)
{
	ASSERT_VALID(this);
	ASSERT(lngPropertyID);
//x	ASSERT(bCreateTempBDataIfNotFound == FALSE); // not used

	switch (lngPropertyID)
	{
		case propClassID:
			return m_lngClassID;
//,		case propIconID:
//			return m_lngIconID; // but could be zero...
		case propLocation: 
			return m_pobjParent->id;
	}

	BObject* pobjPropertyValue = FindProperty(lngPropertyID, FALSE);
	if (pobjPropertyValue)
	{
		ASSERT_VALID(pobjPropertyValue);
		BDataLink* pdat = DYNAMIC_DOWNCAST(BDataLink, pobjPropertyValue->GetData());
		if (pdat)
		{
			ASSERT_VALID(pdat);
			BObject* pobjLink = pdat->GetLink();
			if (pobjLink) {
				ASSERT_VALID(pobjLink);
				return pobjLink->id;
			}
		}
	}
	return 0; 
}


//x
/*
//ObjIDArray BObject::GetPropertyLinks(OBJID lngPropertyID, BOOL bCreateTempBDataIfNotFound)
void BObject::GetPropertyLinks(OBJID lngPropertyID, ObjIDArray& a)
//ObjIDArray* BObject::GetPropertyLinks(OBJID lngPropertyID)
{
	ASSERT_VALID(this);
	ASSERT(lngPropertyID);
//x	ASSERT(bCreateTempBDataIfNotFound == FALSE); // not used

	BObject* pobjPropertyValue = FindProperty(lngPropertyID, FALSE);
	if (pobjPropertyValue)
	{
		ASSERT_VALID(pobjPropertyValue);
		BDataLink* pdat = DYNAMIC_DOWNCAST(BDataLink, pobjPropertyValue->GetData());
		if (pdat)
		{
			ASSERT_VALID(pdat);
			ASSERT(pdat->IsMultiple());

//			ObjIDArray* pida = new ObjIDArray();
//			CObArray* pp = pdat->GetLinkArray();

			CUIntArray b;
			int nObjs = pdat->GetObjectIDArray(b);
			//, use this and delete the above method
//			CObArray* pp = pdat->GetLinkArray();

			//, better way?
			a.SetSize(b.GetSize());
			for (int i = 0; i < b.GetSize(); i++)
				a.SetAt(i, b.GetAt(i));

//			pida->SetSize(pp->GetSize());
//			for (int i = 0; i < pp->GetSize(); i++)
//				pida->SetAt(i, pp->GetAt(i));

//			return pida;
//			return a;
		}
	}
}
*/




// Returns COPY of link array - caller must delete it
CObArray* BObject::GetPropertyLinks(ULONG lngPropertyID) {
	ASSERT_VALID(this);
	ASSERT(lngPropertyID);

	BObject* pobjPropertyValue = FindProperty(lngPropertyID, FALSE);
	if (pobjPropertyValue) {
		ASSERT_VALID(pobjPropertyValue);
		BDataLink* pdat = DYNAMIC_DOWNCAST(BDataLink, pobjPropertyValue->GetData());
		if (pdat) {
			ASSERT_VALID(pdat);
			ASSERT(pdat->IsMultiple());

			CObArray* plocal = pdat->GetLinkArray();
			ASSERT_VALID(plocal);

			// MFC: no copy constructor? gives weird cobject errors
//			CObArray* pcopy = new CObArray(*plocal);
			CObArray* pcopy = new CObArray();
			pcopy->Append(*plocal);
			return pcopy;
		}
	}
	return NULL;
}






// Get number of children, recursively if specified, and including properties if specified.
int BObject::GetChildCount(BOOL bRecurse /* = FALSE */, BOOL bIncludeProperties /* = FALSE */) const
{
	ASSERT_VALID(this);
	int nChildren = 0;

	if (m_paChildren)	
	{
		ASSERT_VALID(m_paChildren);

		// Add number of children to count
		int nItems = m_paChildren->GetSize();
		nChildren += nItems;

		// If recursive, walk through the children, summing the number of their children also
		if (bRecurse)
		{
			for (int i = 0; i < nItems; i++)
			{
				BObject* pobjChild = DYNAMIC_DOWNCAST(BObject, m_paChildren->GetAt(i));
				ASSERT_VALID(pobjChild);
				nChildren += pobjChild->GetChildCount(bRecurse, bIncludeProperties);
			}
		}
	}

	if (bIncludeProperties && m_paProperties)
	{
		ASSERT_VALID(m_paProperties);
		// Add number of properties to count
		int nItems = m_paProperties->GetSize();
		nChildren += nItems;
	}

	return nChildren;
}



// Returns 1+ if has children, 0 if doesn't.
int BObject::HasChildren() const
{
	ASSERT_VALID(this); 
	if (m_paChildren)
	{
		ASSERT_VALID(m_paChildren);
		return m_paChildren->GetSize();
	}
	return 0;
}



// Display properties of object.
void BObject::DisplayProperties()
{
	ASSERT_VALID(this);

	//, add these to page:
	// Flags (checkboxes or checklistbox)
	// Icon (display and let user change)
	// Children (count and size, recursive)
	// Properties (listview with values)
	// maybe the page could load a cviewproperties object?
	// i think eventually that would be the best - then could modify properties for any object
	// from anywhere in the app
	// eg in a family tree, or scrapbook, or map...

	CPropertySheetEx2 sh;
	CPageObjectGeneral pg;
	sh.m_psh.dwFlags |= PSH_NOAPPLYNOW; // turn off apply button
	sh.AddPage(&pg);
	CString strName = GetPropertyString(propName);
	sh.SetTitle(strName, PSH_PROPTITLE);
	pg.m_pobj = this;
	if (sh.DoModal() == IDOK)
	{
		// Note: OnApply will have already applied changes to the object and notified the views.
	}
}




// Check if an object is a child or grandchild etc of another object.
// see also IsChildOf
BOOL BObject::IsChild(BObject* pobjPossibleParent) const
{
	ASSERT_VALID(this);
	const BObject* pobj = this;
	// Look up through object's parent chain for the possible parent object
	while (pobj->m_pobjParent)
	{
		pobj = pobj->m_pobjParent;
		if (pobj == pobjPossibleParent)
			return TRUE;
	}
	return FALSE;
}


// Check if object is in child collection of specified parent object.
// see also IsChild
BOOL BObject::IsChildOf(BObject* pobjPossibleParent, BOOL bRecurse) const
{
	ASSERT_VALID(this);
	ASSERT_VALID(pobjPossibleParent);
	if (pobjPossibleParent->GetChildren())
	{
		ASSERT_VALID(pobjPossibleParent->GetChildren());
		if (pobjPossibleParent->GetChildren()->FindObject(this, bRecurse) != -1)
			return TRUE;
	}
	return FALSE;
}




// Get the size of the object in bytes, including itself, all its properties, and optionally 
// its children recursively.
ULONG BObject::GetMemoryUsed(BOOL bRecurse) const
{
	ASSERT_VALID(this);

	ULONG nBytes = 0;
	nBytes += sizeof(BObject);

	//, add size of string cache?
//	CString m_strTextCache; // This is a cache for the text-representation for this data object

	// Add size of bdata object
	if (m_pdat)
	{
		ASSERT_VALID(m_pdat);
		nBytes += m_pdat->GetMemoryUsed(TRUE);
	}

	// Walk through properties, adding their sizes
	if (m_paProperties)
	{
		ASSERT_VALID(m_paProperties);
		nBytes += sizeof(BObjects); // Add size of array object
		int nProperties = m_paProperties->GetSize();
		for (int i = 0; i < nProperties; i++)
		{
			BObject* pobjProperty = DYNAMIC_DOWNCAST(BObject, m_paProperties->GetAt(i));
			ASSERT_VALID(pobjProperty);
			nBytes += pobjProperty->GetMemoryUsed(TRUE);
		}
	}
	
	// Walk through children, getting their sizes also
	if (bRecurse)
	{
		if (m_paChildren)
		{			
			ASSERT_VALID(m_paChildren);
			nBytes += sizeof(BObjects); // Add size of array object
			int nChildren = m_paChildren->GetSize();
			for (int i = 0; i < nChildren; i++)
			{
				BObject* pobjChild = DYNAMIC_DOWNCAST(BObject, m_paChildren->GetAt(i));
				ASSERT_VALID(pobjChild);
				nBytes += pobjChild->GetMemoryUsed(TRUE);
			}
		}
	}

	return nBytes;
}





// Get array of PropertyDef objects that are applicable to this object.
// Walks up class chain to get inherited properties, filling array with pointers 
// to propdefs.
// If bInheritedOnly is True, will only get propertydefs as inherited by this 
// class's class hierarchy.
// If bThisIsAClass is True, will treat this object as a class. Note this could be a 
// class but you want to treat is as an object, as when viewing Property View for 
// a classdef. 
// Returns number of propertydefs in array.
int BObject::GetPropertyDefs(CObArray& aPropertyDefs, BOOL bInheritedOnly, 
							 BOOL bThisIsAClass)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pDoc);

	//, make global const, enforce this
	const int nMaxClassDepth = 10; // Maximum depth of class hierarchy

	// Initialize array
	CObArray apClasses;
	apClasses.SetSize(nMaxClassDepth);
	int nClasses = 0;

	// If this object is itself a classdef, then we need to add it to the array first,
	// otherwise, the first class is the class of this object.
	// Note: Only do this if requesting inherited propdefs only, which is done in class wizard properties page.
	BObject* pobjClass = 0;
//	if (m_lngClassID == classClass)
//	if (bInheritedOnly && (m_lngClassID == classClass))
	if (bThisIsAClass && (m_lngClassID == classClass))
		pobjClass = this;
	else
		pobjClass = m_pDoc->GetObject(m_lngClassID);

	// Walk up the class chain, pushing classdef pointers to a stack
	// when reach the root, add its propObjectProperties array to our array
	// then walk down the stack, adding propObjectProperties arrays to our array
	// at end of stack, add the object's property array to our array, eliminating duplicate properties
	do
	{
		ASSERT_VALID(pobjClass);
		// Save the pointer to the class in our "stack"
		apClasses.SetAt(nClasses, pobjClass);
		nClasses++;
		// Get the class's parent class
		pobjClass = pobjClass->GetParent();
	}
	// Exit when reach the system root
	// NOTE: This assumes that class root is always located in the system root
	while (pobjClass->GetObjectID() != rootSystem);

	// Walk through class chain from top to bottom, adding propObjectProperties to our array.
	// If user just wants inherited props, don't include the object's classdef props.
	int nFirst = bInheritedOnly ? 1 : 0; 
	for (int i = nClasses - 1; i >= nFirst; i--)
	{
		pobjClass = DYNAMIC_DOWNCAST(BObject, apClasses.GetAt(i));
		ASSERT_VALID(pobjClass);

		CObArray* pa = pobjClass->GetPropertyLinks(propObjectProperties);
		if (pa)
			aPropertyDefs.Append(*pa);
//x
/*
		BDataLink* pdatLink = DYNAMIC_DOWNCAST(BDataLink, pobjClass->GetPropertyData(propObjectProperties));
//		CObArray* pa = pobjClass->GetPropertyLinks(propObjectProperties);
		if (pdatLink)
		{
			ASSERT_VALID(pdatLink);
			// Append the array of pobj's to the list of property defs.
			// Might need to exclude some properties, eg exclude flagSystemProperty if not in admin mode,
			// but i think it's better to just do that in the property view.
			CObArray* pa = pdatLink->GetLinkArray();
			if (pa)
			{
				CObArray& ra = *pa;
				aPropertyDefs.Append(ra);
			}
			delete pdatLink;
		}
		*/
	}

	// Add any custom properties not in the preceding collections to the end.
	// Need to walk through current property collection, and if a prop is not in the current array, add it.
	
	//, ie 
	// m_paProperties.Remove(aProperties); // remove the intersection of the arrays
	// aProperties.Append(m_paProperties); // now append them
	// Would do this now but would add another layer of inheritance and not sure how that would 
	// affect performance at this point (ie derive our own array class from CObArray).
	// How would that affect performance? maybe it wouldn't except in constructor and destructor?
	// ie the vtable would point to the correct routines anyway?

	if (m_paProperties && !bInheritedOnly)
	{
		ASSERT_VALID(m_paProperties);
		int nProps = m_paProperties->GetSize();
		int nPropsTotal = aPropertyDefs.GetSize();
		for (int i = 0; i < nProps; i++)
		{
			// Get property value, then property def from it
			BObject* pobjPropValue = DYNAMIC_DOWNCAST(BObject, m_paProperties->GetAt(i));
			ASSERT_VALID(pobjPropValue);
			OBJID lngPropertyID = pobjPropValue->GetClassID();
			BObject* pobjPropDef = m_pDoc->GetObjectNull(lngPropertyID);
			if (!pobjPropDef)
			{
				CString str;
				str.Format("Found a property that has no propertydef (propid = %d) - delete it (recommended)?", lngPropertyID);
				if (IDYES == AfxMessageBox(str, MB_YESNO))
				{
					m_paProperties->RemoveAt(i);
					i--;
					nProps--;
				}
			}
			else
			{
				ASSERT_VALID(pobjPropDef);
				// Walk through array and see if this property is already there.
				// If not, add it to the array.
				BOOL bNotThere = TRUE;
				for (int j = 0; j < nPropsTotal; j++)
				{
					if (aPropertyDefs.GetAt(j) == DYNAMIC_DOWNCAST(CObject, pobjPropDef))
					{
						bNotThere = FALSE;
						break;
					}
				}
				if (bNotThere)
					aPropertyDefs.Add(pobjPropDef);
			}
		}
	}
	return aPropertyDefs.GetSize();
}



/*
// see getreferences
// Search through document, looking for links to this object.
// Returns total number of links.
//. add objects that link to this object to the array
//, move this to BDoc - like Search
int BObject::GetLinks(BObjects &aObjects, BObject* pobjStart)
{
	ASSERT_VALID(this);
	ASSERT_VALID(&aObjects);
	
	// Start at main root object
	if (pobjStart == NULL)
		pobjStart = m_pDoc->GetRoot();
	ASSERT_VALID(pobjStart);

	// See if the start object has any references to this object
	pobjStart->m_lngClassID;
	pobjStart->m_lngIconID;
	pobjStart->m_pobjParent;
	pobjStart->m_pdat->GetLinks(this);

	// walk through properties, searching for references to this object
	if (pobjStart->m_paProperties)
	{
		int nItems = pobjStart->m_paProperties->GetSize();
		for (int i = 0; i < nItems; i++)
		{
			BObject* pobjProp = (BObject*) pobjStart->m_paProperties->GetAt(i);
			GetLinks(aObjects, pobjProp);
		}
	}

	// Recurse through all objects, searching for links to this object
	if (pobjStart->GetChildren())
	{
		// walk through children
		int nItems = pobjStart->GetChildren()->GetSize();
		for (int i = 0; i < nItems; i++)
		{
			BObject* pobj = (BObject*) pobjStart->GetChildren()->GetAt(i);
			GetLinks(aObjects, pobj);
		}
	}

	return aObjects.GetSize();
}
*/



//, move to BProperty
// Get the alignment (left, right, center) associated with this propertydef bobject.
// Numbers are right aligned, strings are left aligned.
// Returns LVCFMT_LEFT, LVCFMT_RIGHT, or LVCFMT_CENTER.
// This is used by CViewContents.
int BObject::GetPropertyDefAlignment()
{
	ASSERT_VALID(this);
	OBJID idPropType = GetPropertyLink(propPropertyType);
	ASSERT(idPropType);
	int nAlignment = LVCFMT_LEFT; // default is left-aligned
	switch (idPropType)
	{
		case proptypeNumber:
		case proptypeCurrency:
		case proptypeTimeInterval:
		case proptypeCalculated:
		case proptypeLong:
			nAlignment = LVCFMT_RIGHT;
			break;
	}
	return nAlignment;
}



// Return True if the parent of this object is set for autosort
BOOL BObject::IsParentSorted()
{
	ASSERT_VALID(this);
	BObject* pobjParent = GetParent();
	if (pobjParent)
	{
		ASSERT_VALID(pobjParent);
		return (!(pobjParent->GetFlag(flagNoAutosort)));
	}
	return FALSE;
}


// Return True if the object is set to have its children autosorted
BOOL BObject::IsSorted()
{
	ASSERT_VALID(this);
	return (!(this->GetFlag(flagNoAutosort)));
}



// Move this object up relative to its siblings.
// Sets document modified flag and updates views.
//, using our own cobarray class we could have an Exchange method
BOOL BObject::MoveUp()
{
	ASSERT_VALID(this);
	if (m_pobjParent)
	{
		BObjects* pa = m_pobjParent->GetChildren();
		ASSERT_VALID(pa);
		// Switch the positions of the two pointers
		int nIndex = pa->FindObject(this);
		if (nIndex != -1)
		{
			if (nIndex > 0)
			{
				int nIndexOther = nIndex - 1;
				BObject* pobjOther = DYNAMIC_DOWNCAST(BObject, pa->GetAt(nIndexOther));
				ASSERT_VALID(pobjOther);
				pa->SetAt(nIndexOther, this);
				pa->SetAt(nIndex, pobjOther);

				// Set document modified flag
				m_pDoc->SetModifiedFlag();

				// Now tell views
				// basically a move to the same parent, with a different index?
				// in future, might have drag drop to new location
				// we have the two objects that are exchanging positions
				// tree needs pobj, pobjdest, bAfter
				// in this case we're moving it before the given item
				CHint h;
				h.pobjObject = this;
				h.pobjTarget = pobjOther;
				h.bAfter = FALSE;
				m_pDoc->UpdateAllViewsEx(NULL, hintReposition, &h);
				return TRUE;
			}
		}
	}
	return FALSE;
}


// Move this object down relative to its siblings.
// Sets document modified flag and updates views.
BOOL BObject::MoveDown()
{
	ASSERT_VALID(this);
	if (m_pobjParent)
	{
		BObjects* pa = m_pobjParent->GetChildren();
		ASSERT_VALID(pa);
		// Switch the positions of the two pointers
		int nIndex = pa->FindObject(this);
		int nItems = pa->GetSize();
		if (nIndex != -1)
		{
			if (nIndex < nItems - 1)
			{
				int nIndexOther = nIndex + 1;
				BObject* pobjOther = DYNAMIC_DOWNCAST(BObject, pa->GetAt(nIndexOther));
				ASSERT_VALID(pobjOther);
				pa->SetAt(nIndexOther, this);
				pa->SetAt(nIndex, pobjOther);

				// Set document modified flag
				m_pDoc->SetModifiedFlag();
				
				// Now tell views
				CHint h;
				h.pobjObject = this;
				h.pobjTarget = pobjOther;
				h.bAfter = TRUE;
				m_pDoc->UpdateAllViewsEx(NULL, hintReposition, &h);
				return TRUE;
			}
		}
	}
	return FALSE;
}







// Edit the value associated with the given property in a dialog.
// Returns True if user hit OK.
// Note: This will set document modified flag and tell all views about any property change also.
//xBOOL BObject::UIEditValue(OBJID lngPropertyID)
BOOL BObject::UIEditValue(OBJID lngPropertyID, CUI& ui)
{
	ASSERT_VALID(this);
	ASSERT(lngPropertyID);

	CWaitCursor cw;

	// Get propertydef object for the property
	BObject* pobjPropertyDef = m_pDoc->GetObject(lngPropertyID);

	// Check if property is read-only
	if (pobjPropertyDef->GetFlag(flagPropValueReadOnly))
	{
		AfxMessageBox("This property value is read-only and cannot be changed.", MB_ICONINFORMATION);
		return FALSE;
	}

	// Get a copy of BData associated with object's property.
	// If user says OK, then we write the edited bdata object back to the object, which will
	// always write it to the bobject, not up the class chain.
	BOOL ret = FALSE;
	BData* pdat = GetPropertyData(lngPropertyID, TRUE);
	if (pdat) {
		ASSERT_VALID(pdat);

		// UIEditValue will bring up a dialog box that lets user modify the value stored in the BData object.
		// Need to pass object and property so it knows the context.
		// It will return True if user said OK in dialog.
//x		ret = pdat->UIEditValue(this, pobjPropertyDef);
//x		ret = pdat->UIEditValue(this, pobjPropertyDef, ui);
		ret = pdat->UIEditValue(ui, this, pobjPropertyDef);
		if (ret) {
			// User said OK, so let's set the BData copy with the new value to the object.
			// This also sets the document modified flag and updates views.
			SetPropertyData(lngPropertyID, pdat);
		}
		delete pdat; 
	}
	return ret;
}



// Get the default column width for this propertydef, in pixels. 
// This is used by CViewContents when you insert a new column.
int BObject::GetPropertyDefWidth()
{
	//, hardcode for now, later could make a property to store this info in each propertydef
	int nWidth = 120; // default in pixels
	switch (m_lngObjectID)
	{
	case propDescription:
		nWidth = 200;
		break;
	}
	return nWidth;
}



// Get the default name for a new object of this class, eg "New Paper".
//, might make a property eventually (ie you could use something different for some classes).
BOOL BObject::GetClassDefNewName(CString& strName)
{
	strName = CString(_T("New ")) + GetPropertyString(propName);
	return TRUE;
}



// Check if the object can be moved up or down relative to its siblings.
// This can only be true for objects which parents have autosort turned off (they have flagNoAutosort).
// For the most part, if flagNoAutosort is set for an object, we want to be able to move 
// its children up and down, EXCEPT for the Home, Reference and System objects.
// Could handle with another flag, but for now hardcode this.
BOOL BObject::IsMoveUpDownValid(BOOL bMoveUp)
{
	ASSERT_VALID(this);

	// If parent has autosort off, then bEnable will be True
	BOOL bEnable = !IsParentSorted();

	if (m_pobjParent)
	{
		// Exception for children of the main root object (eg Home, Reference).
		// (they have NoAutosort but you don't want user to move them around).
		ASSERT_VALID(m_pobjParent);
		if (m_pobjParent->GetObjectID() == rootMain)
		{
			// let admin move items up and down (note: no handling for first or last item)
			if (app.m_bAdmin)
				bEnable = TRUE;
			else
				bEnable = FALSE;
		}
		else if (bEnable)
		{
			// Check if first or last among siblings
			BObjects* paSiblings = m_pobjParent->GetChildren();
			ASSERT_VALID(paSiblings);
			int nIndex = paSiblings->FindObject(this);
			int nItems = paSiblings->GetSize();

			// Bug: Had these if statements without the brackets, which screwed up the logic! be careful about that.
			if (bMoveUp)
			{
				// If first item in list, disable moveup
				if (nIndex == 0) bEnable = FALSE;
			}
			else
			{
				// If last item in list, disable movedown
				if (nIndex == nItems - 1) bEnable = FALSE;
			}
		}
	}
	// If object has no parent then it must be the main root object, so disable
	else
		bEnable = FALSE;

	return bEnable;
}






// Change this object's class, handling name data type change, also updating views.
// This should always be used rather than setting m_lngClassID directly.
BOOL BObject::SetClassID(OBJID lngNewClassID)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pdat);
	ASSERT_VALID(m_pDoc);
	ASSERT(lngNewClassID);

	// See if we need to handle name data type change - 
	// compare existing bdata type and new bdata type.
	CRuntimeClass* prcOld = m_pdat->GetRuntimeClass();

	// Get the new name bdata as required by the new class.
	// This will be BDataString or BDataPersonName (for now)
	BData* pdatNew = m_pDoc->CreateBData(lngNewClassID);
	CRuntimeClass* prcNew = pdatNew->GetRuntimeClass();

	// If the name bdata objects are the same type, we can leave it as it is, so delete the new one.
	if (prcOld == prcNew)
	{
		delete pdatNew;
		pdatNew = 0;
	}
	else
	{
		// The name bdata types are different, so we need to convert between them and store the 
		// new one with this bobject.
		CString strName = m_pdat->GetBDataText(m_pDoc, propName);
		pdatNew->SetBDataText(strName);
		
		// Delete the old bdata object
		delete m_pdat;
		
		// Store the new bdata object
		m_pdat = pdatNew;
	}

	// Set class and document modified flag
	m_lngClassID = lngNewClassID;

	// Set modified flag
	m_pDoc->SetModifiedFlag(TRUE);

	// Inform views of change
	CHint h;
	h.pobjObject = this;
	h.idProperty = propClassID;
	m_pDoc->UpdateAllViewsEx(NULL, hintPropertyChange, &h);

	// Also broadcast hints for all dependent properties
	//, need generic way of handling this
	h.idProperty = propClassName;
	m_pDoc->UpdateAllViewsEx(NULL, hintPropertyChange, &h);

	h.idProperty = propIconID;
	m_pDoc->UpdateAllViewsEx(NULL, hintPropertyChange, &h);

	return TRUE;
}




// Fill an array with this object's parents, stopping at the specified object.
// Can specify to include this object and the stopping object.
// Returns number of items in array.
int BObject::GetParents(BObjects &aParents, BObject *pobjStopAt, BOOL bIncludeThisObject /* = TRUE */, 
										BOOL bIncludeStopObject /* = FALSE */)
{
	ASSERT_VALID(this);
	ASSERT_VALID(&aParents);
	ASSERT(bIncludeStopObject == FALSE); //, for now

	// Clear array
	aParents.RemoveAll();

	// Get starting object
	BObject* pobj = 0;
	if (bIncludeThisObject)
		pobj = this;
	else
		pobj = m_pobjParent;

	do
	{
		if (pobj == 0) break;
		ASSERT_VALID(pobj);
		aParents.Add(pobj);
		pobj = pobj->m_pobjParent;
	}
	while (pobj != pobjStopAt);

	return aParents.GetSize();	
}




// Set or clear a flag for this object, recursing through children if specified.
// Possible flags are flagExpanded, flagNoDelete, flagTemp, flagFilter, flagDisabled, etc.
// This will set document modified flag if it's an important flag.
void BObject::SetFlag(ULONG lngFlag, BOOL bValue /*=TRUE*/, BOOL bRecurse /* = FALSE */) {

	ASSERT_VALID(this);
	ASSERT_VALID(m_pDoc);

	if (bValue)
		m_lngFlags |= lngFlag;
	else
		m_lngFlags &= ~lngFlag;

	if (bRecurse) {
		// Walk through children and call this routine recursively
		int nChildren = GetChildCount(FALSE);
		for (int i = 0; i < nChildren; i++) {
			BObject* pobj = DYNAMIC_DOWNCAST(BObject, m_paChildren->GetAt(i));
			ASSERT_VALID(pobj);
			pobj->SetFlag(lngFlag, bValue, bRecurse);
		}
	}
	
	// Changing the flags listed here will not set the document modified flag
	const ULONG lngImportantFlags = ~(flagExpanded | flagTemp | flagFilter | flagDisabled);

	// Set document modified if it's an important flag
//	if (lngFlag != flagExpanded && lngFlag != flagTemp && lngFlag != flagFilter)
	if (lngFlag & lngImportantFlags)
		m_pDoc->SetModifiedFlag();
}


void BObject::ClearFlag(ULONG lngFlag) {
	SetFlag(lngFlag, FALSE);
}


// Get the value of the specified flag for this object. (flagNoDelete, flagHighPriority, etc).
// inline BOOL BObject::GetFlag(ULONG lngFlag) // removed inline because caused link errors
BOOL BObject::GetFlag(ULONG lngFlag)
{
	return (m_lngFlags & lngFlag);
}



// Call this method to send a message to this bobject and optionally all of its children recursively.
// For now just implements msgResetData, which will cause any bdata text cache's to be reset.
// (just does it for name property for now)
int BObject::SendMessage(ULONG lngMsg, BOOL bRecurse)
{
	ASSERT_VALID(this);

	switch (lngMsg)
	{
		case msgResetData:
			{
				// Walk through all property bdatas and reset them.
				// Also reset m_pdat.
				//, for now, this is enough since we just use it to reset names.
				if (m_pdat)
				{
					ASSERT_VALID(m_pdat);
					m_pdat->ResetData();
				}
				break;
			}
	}

	// Now walk through children and call this method recursively
	if (m_paChildren)
	{
		ASSERT_VALID(m_paChildren);
		int nChildren = m_paChildren->GetSize();
		for (int i = 0; i < nChildren; i++)
		{
			BObject* pobj = DYNAMIC_DOWNCAST(BObject, m_paChildren->GetAt(i));
			ASSERT_VALID(pobj);
			pobj->SendMessage(lngMsg, bRecurse);
		}
	}
	return 0;
}



// Set icon and document modified flag, and update views
//, validate iconid
BOOL BObject::SetIconID(OBJID lngIconID)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pDoc);

	// If icon is default for the object, store it as zero
	if (lngIconID == GetDefaultIconID())
		m_lngIconID = 0;
	else
		m_lngIconID = lngIconID;

	// Set document flag
	m_pDoc->SetModifiedFlag(TRUE);

	// Inform views of change
	CHint h;
	h.pobjObject = this;
	h.idProperty = propIconID;
	m_pDoc->UpdateAllViewsEx(NULL, hintPropertyChange, &h);

	// If this is a classdef object, refresh all visible icons
	// bug: used = instead of == and wound up converting all objects to classes
	if (m_lngClassID == classClass)
		m_pDoc->UpdateAllViewsEx(NULL, hintRefreshAllIcons);

	return TRUE;
}


//x
/*
//, move to bfolder
BOOL BObject::SetColumns(BDataColumns& cols) {
	ASSERT_VALID(this);
	BOOL bResult = this->SetPropertyData(propColumnInfoArray, &cols); // sends hint
	delete &cols;
	return bResult;
}
*/


// For this folder object, initialize the column array (propColumnInfoArray) 
// to reflect the properties used by the default class.
//, move to bfolder
void BObject::SetColumnsBasedOnClass(BObject *pobjDefaultClass) {
	
	BDataColumns* pdatCols = new BDataColumns;
	ULONG lngExcludeFlags = flagAdminOnly; // always exclude admin only props (eg ObjectID)
	BObjects aProps;
	int nProps = pobjDefaultClass->GetPropertyDefs(aProps, FALSE, TRUE); // get props associated with class
	for (int i = 0; i < nProps; i++) {
		BObject* pobjProp = DYNAMIC_DOWNCAST(BObject, aProps.GetAt(i));
		ASSERT_VALID(pobjProp);
		if (!(pobjProp->GetFlag(lngExcludeFlags))) {
			OBJID lngPropertyID = pobjProp->GetObjectID();
			//. kludgy: don't add the Size property by default, though it's available to all objects. 
			if (lngPropertyID != propSize)
				pdatCols->InsertColumn(lngPropertyID, m_pDoc);
		}
	}
	SetPropertyData(propColumnInfoArray, pdatCols, FALSE, FALSE);
	delete pdatCols;
}





// Change the property type for the specified property to the new property type,
// recursing downwards through child objects.
// See also ChangeNamePropertyType
//, pobjPropertyDef is required for some SetDataText's, unfortunately!
void BObject::ChangePropertyType(BObject* pobjPropertyDef, BObject* pobjNewPropertyDef, 
							OBJID lngNewPropertyTypeID)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pobjPropertyDef);
	ASSERT_VALID(pobjNewPropertyDef);

	OBJID lngPropertyID = pobjPropertyDef->GetObjectID();

	// Walk through properties looking for property in question
	if (m_paProperties)
	{
		ASSERT_VALID(m_paProperties);
		int nProperties = m_paProperties->GetSize();
		for (int i = 0; i < nProperties; i++)
		{
			BObject* pobj = DYNAMIC_DOWNCAST(BObject, m_paProperties->GetAt(i));
			ASSERT_VALID(pobj);
			if (pobj->m_lngClassID == lngPropertyID)
			{
				BData* pdatOld = pobj->GetData();

				// Create the new bdata type object
				BData* pdatNew = m_pDoc->CreateBDataFromPropertyType(lngNewPropertyTypeID);

				// Convert between the types using a string as the go-between.
				// Don't display any error messages.
				CString strText = pdatOld->GetBDataText(m_pDoc, lngPropertyID);
//				pdatNew->SetBDataText(strText, pobjPropertyDef);
//				pdatNew->SetBDataText(strText, pobjNewPropertyDef);
				pdatNew->SetBDataText(strText, pobjNewPropertyDef, FALSE);

				// Delete the old bdata object
				delete pdatOld;

				// Store the new bdata object
				pobj->m_pdat = pdatNew;

			}
		}
	}

	// Now walk through children and call this method recursively
	if (m_paChildren)
	{
		ASSERT_VALID(m_paChildren);
		int nChildren = m_paChildren->GetSize();
		for (int i = 0; i < nChildren; i++)
		{
			BObject* pobj = DYNAMIC_DOWNCAST(BObject, m_paChildren->GetAt(i));
			ASSERT_VALID(pobj);
//			pobj->ChangePropertyType(pobjPropertyDef, lngPropertyID, lngNewPropertyTypeID);
			pobj->ChangePropertyType(pobjPropertyDef, pobjNewPropertyDef, lngNewPropertyTypeID);
		}
	}
}




// Recurse downwards through objects and change the name bdata property type for
// objects of the specified class.
// See also ChangePropertyType
void BObject::ChangeNamePropertyType(OBJID lngClassID, OBJID lngNewPropertyTypeID)
{
	ASSERT_VALID(this);

	// If this object is of the specified class, change its name bdata object
	if (m_lngClassID == lngClassID)
	{
		BData* pdatOld = m_pdat;

		// Create the new bdata type object
		BData* pdatNew = m_pDoc->CreateBDataFromPropertyType(lngNewPropertyTypeID);

		// Convert between the types using a string as the go-between.
		// Don't display any error messages.
		//. For now this just occurs between BDataString and BDataPersonName, so we don't
		// have to worry about propertydefs, but in future version might need to handle them!
		CString strName = pdatOld->GetBDataText(m_pDoc, 0);
		pdatNew->SetBDataText(strName, 0, FALSE);

		// Delete the old bdata object
		delete pdatOld;

		// Store the new bdata object
		m_pdat = pdatNew;
	}

	// Now walk through children and call this method recursively
	if (m_paChildren)
	{
		ASSERT_VALID(m_paChildren);
		int nChildren = m_paChildren->GetSize();
		for (int i = 0; i < nChildren; i++)
		{
			BObject* pobj = DYNAMIC_DOWNCAST(BObject, m_paChildren->GetAt(i));
			ASSERT_VALID(pobj);
			pobj->ChangeNamePropertyType(lngClassID, lngNewPropertyTypeID);
		}
	}
}



// Find references in this object and its properties to the Find object.
// If recurse is specified, will search recursively through any child objects also.
//. eventually add an object that includes the pobj referencing it and the propid.
int BObject::GetReferences(BObject *pobjFind, CObArray &aRefs, BOOL bRecurse) {

	ASSERT_VALID(this);
	ASSERT_VALID(pobjFind);
	ASSERT_VALID(&aRefs);

	BOOL bReferenced = FALSE;
	OBJID lngFindID = pobjFind->GetObjectID();

	// Exit if objectid is zero (can happen if it's a temporary object, not in the db)
	if (lngFindID == 0)
		return aRefs.GetSize();

	// Search in attributes
	if (m_lngClassID == lngFindID)
		bReferenced = TRUE;
	if (m_lngIconID == lngFindID)
		bReferenced = TRUE;

	// Search in this object's bdata
	if (m_pdat) {
		ASSERT_VALID(m_pdat);
		if (m_pdat->FindReferences(pobjFind))
			bReferenced = TRUE;
	}

	// If we haven't found a reference yet, search through this object's properties' bdata objects 
	// until you find a reference.
	// Also check each propertyid.
	if (!bReferenced && m_paProperties) {
		ASSERT_VALID(m_paProperties);
		int nProps = m_paProperties->GetSize();
		for (int i = 0; i < nProps; i++) {
			BObject* pobjProp = DYNAMIC_DOWNCAST(BObject, m_paProperties->GetAt(i));
			ASSERT_VALID(pobjProp);
			// Check if this property value is using the find object as its propertydef
			if (pobjProp->GetClassID() == lngFindID) {
				bReferenced = TRUE;
				break;
			}
			// Check if the property value data references the find object
			ASSERT_VALID(pobjProp->GetData());
			if (pobjProp->GetData()->FindReferences(pobjFind)) {
				bReferenced = TRUE;
				break;
			}
		}
	}

	// If there was a reference to the Find object in this object, 
	// add this object to the list of references
	if (bReferenced)
		aRefs.Add(this);

	// Search through children (if recurse specified)
	if (bRecurse && m_paChildren) {
		ASSERT_VALID(m_paChildren);
		int nChildren = m_paChildren->GetSize();
		for (int i = 0; i < nChildren; i++) {
			BObject* pobjChild = DYNAMIC_DOWNCAST(BObject, m_paChildren->GetAt(i));
			ASSERT_VALID(pobjChild);
			pobjChild->GetReferences(pobjFind, aRefs, bRecurse);
		}
	}

	return aRefs.GetSize();
}



// Remove references to an object, or replace references with a new object if specified, 
// optionally recursing downwards.
void BObject::ReplaceReferences(BObject* pobjFind, BObject* pobjNew /* = 0 */, BOOL bRecurse /* = TRUE */)
{
	//trace("BObject::ReplaceReferences\n");

	ASSERT_VALID(this);
	ASSERT_VALID(pobjFind);
	
	CHint h;
	h.pobjObject = this;

	OBJID lngFindID = pobjFind->GetObjectID();
	OBJID lngNewID = 0;
	if (pobjNew) 
	{
		ASSERT_VALID(pobjNew);
		lngNewID = pobjNew->GetObjectID();
	}

	// Check class
	if (m_lngClassID == lngFindID)
	{
		// If new id is 0, we'll want to delete this object entirely, as it's a property value?
		// Actually, this should never happen, as we check if it's a classdef and pass classPaper. 
		ASSERT(lngNewID);
//		if (lngNewID == 0)
//		{
//		}
//		else
//		{
			SetClassID(lngNewID);
			// Tell views
//			h.idProperty = propClass;
//			m_pDoc->UpdateAllViewsEx(0, hintPropertyChange, &h);
			h.idProperty = propClassName;
			m_pDoc->UpdateAllViewsEx(0, hintPropertyChange, &h);
//		}
	}

	// Check icon
	if (m_lngIconID == lngFindID)
	{
		m_lngIconID = lngNewID;
		// Tell views
//		h.idProperty = propIconID;
//		m_pDoc->UpdateAllViewsEx(0, hintPropertyChange, &h);
	}

	// Check this object's bdata
	if (m_pdat)
	{
		ASSERT_VALID(m_pdat);
		if (m_pdat->ReplaceReferences(pobjFind, pobjNew))
		{
			// Tell views
			h.idProperty = m_lngClassID;
			m_pDoc->UpdateAllViewsEx(0, hintPropertyChange, &h);
		}
	}

	// Check property values, if any.
	if (m_paProperties)
	{
		ASSERT_VALID(m_paProperties);
		int nProps = m_paProperties->GetSize();
		for (int i = 0; i < nProps; i++)
		{
			BObject* pobjPropertyValue = DYNAMIC_DOWNCAST(BObject, m_paProperties->GetAt(i));
			ASSERT_VALID(pobjPropertyValue);
			// If the property def of this property value is the object we're looking for, delete the property value.
			//, note this doesn't handle replace for property values yet - might need to adjust bdata objects also
			if (pobjPropertyValue->GetClassID() == lngFindID)
			{
				// Bug: Called DeleteProperty on the property value instead of on this bobject
//				pobjPropertyValue->DeleteProperty(lngFindID);
				DeleteProperty(lngFindID);
				// adjust indexes so can continue through array
				i--;
				nProps--;
			}
			else
			{
				// Remove/replace any references contained in the property value's bdata.
				ASSERT_VALID(pobjPropertyValue->GetData());
				if (pobjPropertyValue->GetData()->ReplaceReferences(pobjFind, pobjNew))
				{
					// Tell views
					h.idProperty = pobjPropertyValue->GetClassID();
					m_pDoc->UpdateAllViewsEx(0, hintPropertyChange, &h);
				}
			}
		}
	}

	// Search through children (if recurse was specified)
	if (bRecurse && m_paChildren)
	{
		ASSERT_VALID(m_paChildren);
		int nChildren = m_paChildren->GetSize();
		for (int i = 0; i < nChildren; i++)
		{
			BObject* pobjChild = DYNAMIC_DOWNCAST(BObject, m_paChildren->GetAt(i));
			ASSERT_VALID(pobjChild);
			pobjChild->ReplaceReferences(pobjFind, pobjNew, bRecurse);
		}
	}
}




// Sort the children of this object (physically) in alphabetical order.
BOOL BObject::SortChildren()
{
	if (m_paChildren)
	{
		ASSERT_VALID(m_paChildren);
		m_paChildren->Sort(propName);
	}
	return TRUE;
}




// Delete this object and any children recursively. 
// Will ask to remove references if any exist, and if user says No, will return False. 
// If object (and descendents) is deleted successfully, will return True.
// This will tell all views about deletion, remove object from doc's index, 
// remove object from parent's child list, and set document modified flag.
// You can tell it to not set document modified flag, and to not update views.
BOOL BObject::DeleteObject(BOOL bSetModifiedFlag /* = TRUE */, BOOL bUpdateViews /* = TRUE */) {

	ASSERT_VALID(this);
	ASSERT_VALID(m_pDoc);

	// Check children recursively also
	// Note: We do this first so that children are deleted before their parents (important!)
	if (m_paChildren) {
		ASSERT_VALID(m_paChildren);
		while (m_paChildren->GetSize() > 0) {
			BObject* pobjChild = DYNAMIC_DOWNCAST(BObject, m_paChildren->GetAt(0));
			ASSERT_VALID(pobjChild);
			// Attempt to delete child - if failed, return False
			if (!pobjChild->DeleteObject())
				return FALSE;
		}
	}

	// Check for links to <this> bobject recursively through entire document
	BObjects aReferences;
	int nLinks = m_pDoc->GetReferences(this, aReferences);
	if (nLinks) {

		// Get object's class name (lowercase)
		CString strClassName = GetPropertyString(propClassName);
		strClassName.MakeLower();

		// Ask the user if they want to remove all references to the object and delete it
		CString strMsg;
		strMsg.Format(_T("The %s \"%s\" is referenced by the following object(s): %s. "
						"Do you want to remove all references to the object and then delete it?"), 
						(LPCTSTR) strClassName, 
						(LPCTSTR) GetPropertyString(propName), 
						(LPCTSTR) aReferences.GetText()
						);
		if (IDYES == AfxMessageBox(strMsg, MB_ICONQUESTION | MB_YESNO)) {

			// If we're deleting a class, we'll want to replace all references with classPaper.
			BObject* pobjNew = 0; // default is to just remove all references
			if (m_lngClassID == classClass)
				pobjNew = m_pDoc->GetObject(classPaper);
			// Remove/replace all references recursively and return true.
			m_pDoc->GetRoot()->ReplaceReferences(this, pobjNew);
		}
		else
			// Did not delete object - return False
			return FALSE;
	}

	// Now we're clear to delete <this> bobject..

	// Tell all views about deletion.
	// Note: Need to do this BEFORE actually deleting the objects so view code can utilize object props, etc.
	// Note: If we deleted the current object, this is where treeview will remove item and select the next item.
//	if (bUpdateViews) {
//		CHint objHint;
//		BObjects aObjects;
//		aObjects.Add(this);
//		objHint.m_paObjects = &aObjects;
//		m_pDoc->UpdateAllViewsEx(NULL, hintDelete, &objHint);
//	}

	// Remove the object from the Index
	m_pDoc->RemoveObjectFromIndex(m_lngObjectID);

	// Remove the object from its parent's children collection and set parent to NULL.
	// Note: We only need to do this for the top level objects we are deleting - 
	// we don't care about any children or grandchildren lists of those objects.
	// Also, it screws things up because you wind up removing items from the array 
	// that you are currently walking through.
	ASSERT_VALID(m_pobjParent);
	m_pobjParent->RemoveChild(this);

	// Set document flag
	if (bSetModifiedFlag)
		m_pDoc->SetModifiedFlag();

	// Tell all views about deletion.
	if (bUpdateViews) {
		CHint objHint;
		BObjects aObjects;
		aObjects.Add(this);
		objHint.paObjects = &aObjects;
		m_pDoc->UpdateAllViewsEx(NULL, hintDelete, &objHint);
	}

	// Now delete the actual BObject
	// Note: Destructor recursively deletes children and properties and bdata.
	// Note: We only need to do this for the top level objects since this is recursive.
	delete this;

	return TRUE;
}




// Get pointer to the specified child, or 0 if invalid index.
BObject* BObject::GetChild(int nChild)
{
	ASSERT_VALID(this);
	ASSERT(nChild >= 0);

	if (m_paChildren)
	{
		ASSERT_VALID(m_paChildren);
		int nChildren = m_paChildren->GetSize();
		if (nChild < nChildren)
		{
			BObject* pobjChild = DYNAMIC_DOWNCAST(BObject, m_paChildren->GetAt(nChild));
			ASSERT_VALID(pobjChild);
			return pobjChild;
		}
	}
	return 0;
}



// Returns a pointer to the class object for this object
BObject* BObject::GetClassObject()
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pDoc);
	BObject* pobjClass = m_pDoc->GetObject(m_lngClassID);
	ASSERT_VALID(pobjClass);
	return pobjClass;
}


//x
/*
BDataColumns& BObject::GetColumns() {
	ASSERT_VALID(this);
	BDataColumns* pdatColumns = DYNAMIC_DOWNCAST(BDataColumns, this->GetPropertyData(propColumnInfoArray));
	ASSERT_VALID(pdatColumns);
	return *pdatColumns;
}
*/



// Move this BObject to a new parent if possible.
// This will handle removing from old parent list, adding to new parent list. 
// This will set document modified flag and update all views if specified. 
// See also SetParent (which should just be for initialization?)
BOOL BObject::MoveTo(BObject *pobjNewParent, BOOL bSetModifiedFlag /* = TRUE */, BOOL bUpdateViews /* = TRUE */, BOOL bDisplayMessages /* = TRUE */)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pobjNewParent);
	ASSERT_VALID(m_pDoc);

	// Exit if moving onto current parent (ie no change)
	if (pobjNewParent == m_pobjParent)
		return FALSE; //. or true?

	// Check if move is valid
	// Displays message if not allowed to drop any of the objects on the target.
//	if (!IsMoveValid(pobjNewParent, TRUE))
	if (!IsMoveValid(pobjNewParent, bDisplayMessages))
		return FALSE;

	// Remove the object from its current parent's child collection and set parent to NULL.
	ASSERT_VALID(m_pobjParent);
	m_pobjParent->RemoveChild(this);
	
	// Add the object to the new parent's child collection.
	// Note: This will set the object's parent pointer also.
	pobjNewParent->AddChild(this, TRUE);

	// Make sure relationship is set correctly (parent is a two-way relationship)
	ASSERT(this->m_pobjParent == pobjNewParent);
	ASSERT(this->IsChildOf(pobjNewParent, FALSE));

	// Set document modified
	if (bSetModifiedFlag)
		m_pDoc->SetModifiedFlag();

	// Now tell all views about the move
	if (bUpdateViews)
	{
//		CHint h;
//		h.m_paObjects = &aObjects;
//		h.m_pobjObject = this;
//		h.m_pobjParent = pobjNewParent;
//		m_pDoc->UpdateAllViewsEx(NULL, hintMove, &h);
		m_pDoc->UpdateAllViewsEx(NULL, hintMoveObject, this);
	}

	return TRUE;
}



// Check if this object can be moved onto the specified target.
// Displays a message and returns false if move is invalid.
BOOL BObject::IsMoveValid(BObject *pobjTarget, BOOL bDisplayMessages)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pobjTarget);

	int nError = 0;

	// Make sure an item is not being dropped onto itself
	if (this == pobjTarget)
		nError = IDS_ERROR_MOVE_SELF;
	
	// Make sure an item is not being dropped onto its parent
	if (m_pobjParent == pobjTarget)
		nError = IDS_ERROR_MOVE_PARENT;

	// Make sure an item is not being dropped onto a descendent
	if (pobjTarget->IsChild(this))
		nError = IDS_ERROR_MOVE_CHILD;

	// Display message if rule broken and return False
	if (nError)
	{
		if (bDisplayMessages)
			AfxMessageBox(nError, MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	// Move is valid
	return TRUE;
}



// Add the specified property to this classdef's list of associated properties, checking
// first if the property is already included in the class chain.
// Returns True if successful.
//, This function is not finished - doesn't look up the class chain etc. 
BOOL BObject::ClassDefAddProperty(OBJID lngPropertyID)
{
	ASSERT_VALID(this);

	//, First see if the property is associated with any parent classes - if so we don't need to add it to
	// the object's classdef.
//	CObArray aProps;
//	int nProps = GetPropertyDefs(aProps, TRUE, TRUE);

	//,, temporary - just hardcode the inherited props for now - simpler and faster at the moment.
	switch (lngPropertyID)
	{
	case propName:
	case propClassName:
	case propDescription:
		return TRUE;
	}

	// Note: Add will not add duplicate objects

	SetPropertyLinksAdd(propObjectProperties, lngPropertyID);
//x
/*
	// duplicate code in 3 places
	// Note: pdat might be zero if class has no properties assigned to it
	// Note: AddLinkID will not add duplicate objects (just returns -1 if already there)
	BDataLink* pdatLinks = DYNAMIC_DOWNCAST(BDataLink, GetPropertyData(propObjectProperties));
	if (pdatLinks)
	{
		ASSERT_VALID(pdatLinks);
		pdatLinks->AddLinkID(lngPropertyID, m_pDoc);
	}
	else
	{
		// No bdata yet - create a new one and store it with the class def
		pdatLinks = new BDataLink;
		pdatLinks->SetMultiple();
		pdatLinks->AddLinkID(lngPropertyID, m_pDoc);
	}
	SetPropertyData(propObjectProperties, pdatLinks);
	delete pdatLinks;
*/

	return TRUE;
}


// Get the name for this object, optionally including its class name 
// eg:      book "The Lord of the Rings" 
//, trim to reasonable # chars, add ... if necess
//, why does this use m_strTextCache? slows it down. eg can't use it to replace GetPropertyString(propName)
LPCTSTR BObject::GetName(BOOL bIncludeClassName)
{
	ASSERT_VALID(this);

//	ASSERT(bIncludeClassName); //, for now
	if (bIncludeClassName)
	{
		CString strClassName = GetPropertyString(propClassName);
		strClassName.MakeLower();
		m_strTextCache.Format("%s \"%s\"", 
//					(LPCTSTR) GetPropertyString(propClassName), 
					(LPCTSTR) strClassName, 
					(LPCTSTR) GetPropertyString(propName)
					);
	}
	else
	{
		m_strTextCache.Format("%s", 
					(LPCTSTR) GetPropertyString(propName)
					);
	}
	return m_strTextCache;
}





// Add rich text to the specified property.
// Used in drag/drop of text to different items and Move Text To... command.
// This will set the document modified flag.
// Note: This uses the app's hidden rtf control, because the target rtf is not necessarily 
// visible in the main rtf control.
//, could pass a param for position to insert at (beginning, end, stored insertion point)
BOOL BObject::AddRtf(OBJID lngPropertyID, CString& strRtf)
{
	ASSERT_VALID(this);

	// Get text (rtf) of target object.
	CString strOldText = GetPropertyString(lngPropertyID);

	// Add existing text (rtf) to the dummy rtf control.
	app.m_rtf.SetRtf(strOldText);
//traceString("%s\n", pszOldText);
//traceToFile("_1.rtf", pszOldText);

	// Select the last character.
	long nChars = app.m_rtf.GetTextLength();
	app.m_rtf.SetSel(nChars, nChars + 1); // bug: if nchars was 0 was leaving default system font at very end. fixed by using nchars+1 for end of range.

	// Replace the selection with the new selection.
	app.m_rtf.SetRtf((LPCTSTR) strRtf, TRUE);

	// Get the new text (rtf)
//,	GetRtfConst()?	
	CString strNewText = app.m_rtf.GetRtf(FALSE);
//traceString("%s\n", (LPCTSTR) strNewText);
//traceToFile("_2.rtf", (LPCTSTR) strNewText);

	// Assign new text (rtf) back to object.
	// Set doc modified flag and tell all views.
	SetPropertyString(lngPropertyID, (LPCTSTR) strNewText, TRUE, TRUE);

	// Clear rtf contents to save memory.
	app.m_rtf.SetWindowText("");

	return TRUE;
}







// Convert all subproperties to soft links (ie ObjectIDs)
void BObject::ConvertToSoftLinks(BOOL bRecurse)
{
	ASSERT_VALID(this);

	// Walk through all properties and convert to soft links
	if (m_paProperties)
	{
		ASSERT_VALID(m_paProperties);
		int nProps = m_paProperties->GetSize();
		for (int i = 0; i < nProps; i++)
		{
			BObject* pobjProp = DYNAMIC_DOWNCAST(BObject, m_paProperties->GetAt(i));
			ASSERT_VALID(pobjProp);
			BData* pdat = pobjProp->GetData();
			if (pdat)
			{
				OBJID lngPropertyID = pobjProp->GetClassID();
				TRACE("converting %s.[%d] to soft links\n", (LPCTSTR) this->GetName(TRUE), lngPropertyID);
				pdat->ConvertToSoftLinks();
			}
		}
	}

	// Recurse through all child objects also, if specified
	if (bRecurse)
	{
		if (m_paChildren)
		{
			int nItems = m_paChildren->GetSize();
			for (int i = 0; i < nItems; i++)
			{
				BObject* pobj = DYNAMIC_DOWNCAST(BObject, m_paChildren->GetAt(i));
				ASSERT_VALID(pobj);
				pobj->ConvertToSoftLinks(bRecurse);
			}
		}
	}
}


// Convert all subproperties from soft to hard links
void BObject::ConvertToHardLinks(BOOL bRecurse)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pDoc);
	BDoc* pDoc = this->GetDoc();

	// Walk through all properties and convert to hard links.
	// Note: Some properties may already be hard links. 
	if (m_paProperties)
	{
		ASSERT_VALID(m_paProperties);
		int nProps = m_paProperties->GetSize();
		for (int i = 0; i < nProps; i++)
		{
			BObject* pobjProp = DYNAMIC_DOWNCAST(BObject, m_paProperties->GetAt(i));
			ASSERT_VALID(pobjProp);
			BData* pdat = pobjProp->GetData();
			if (pdat)
			{
				OBJID lngPropertyID = pobjProp->GetClassID();
				//trace("converting %s.[%d] to hard links\n", (LPCTSTR) this->GetName(TRUE), lngPropertyID);
				pdat->ConvertToHardLinks(pDoc);
			}
		}
	}

	// Recurse through all child objects also, if specified
	if (bRecurse)
	{
		if (m_paChildren)
		{
			int nItems = m_paChildren->GetSize();
			for (int i = 0; i < nItems; i++)
			{
				BObject* pobj = DYNAMIC_DOWNCAST(BObject, m_paChildren->GetAt(i));
				ASSERT_VALID(pobj);
				pobj->ConvertToHardLinks(bRecurse);
			}
		}
	}
}



// Copy the bdata object associated with the specified BObject and property value, if there.
// Also returns a pointer to the new bdata object, or 0 if none.
void BObject::CopyPropertyDataFrom(BObject *pobjSource, OBJID lngPropertyID)
{
	ASSERT_VALID(this);
	BData* pdat = pobjSource->GetPropertyData(lngPropertyID);
	if (pdat) {
		this->SetPropertyData(lngPropertyID, pdat, FALSE, FALSE);
		delete pdat;
	}

	// else

	// If bdata can't be found, make sure it doesn't exist in this bobject as well!
	// bug: forgot to include this step!
	this->DeleteProperty(lngPropertyID, FALSE, FALSE);
}




// Copy a BObject's contents onto this one. 
void BObject::CopyFrom(BObject* pobjSource)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pDoc);
	ASSERT_VALID(pobjSource);
AfxMessageBox("copyfrom!"); //x
	// Copy properties
	this->m_lngFlags = pobjSource->GetFlags();
	this->m_lngIconID = pobjSource->m_lngIconID; // leave as direct ref
	this->m_bytViewHeight = pobjSource->m_bytViewHeight;
	this->m_lngClassID = pobjSource->GetClassID();
	this->SetObjectID(pobjSource->GetObjectID());

	// Leave this->pDoc as is

	// Find the parent of the source object and get its ID. 
	// Parent is essentially a link property - so can treat as a soft link also.
	// This will make sure if we're copying from one document to another that
	// the parent will be set properly.
	//. Note: Assumes that the parent already exists, ie that synchronization is
	// done in top down recursive order
	BObject* pobjSourceParent = pobjSource->GetParent();
	ASSERT_VALID(pobjSourceParent);
	OBJID nSourceParentID = pobjSourceParent->GetObjectID();
	
	// Lookup the parent object in THIS document, and make this bobject a child. 
	// This will handle moving the object if its parent has changed for some reason
	// (ie remove from old parent list, add to new parent list). 
	// Use setparent instead of moveto because moveto assumes that the object already has a valid parent.
	BObject* pobjThisParent = m_pDoc->GetObjectNull(nSourceParentID);
//	this->MoveTo(pobjThisParent, TRUE, TRUE, FALSE); 
	this->SetParent(pobjThisParent);

	// Copy BData (usually contains name of this BObject)
	if (pobjSource->GetData())
	{
		this->SetData(pobjSource->GetData()->CreateCopy());
	}

	// Delete all existing property subobjects in case this object has some that the new object doesn't. 
	// Release property objects recursively (descructor recursively deletes property objects)
	if (this->m_paProperties)
	{
		delete this->m_paProperties;
		this->m_paProperties = 0; // Bug: Didn't have this here and some code used this to see if had props
	}
	
	// Copy all property bdata objects
	BObjects* pobjSourceProps = pobjSource->m_paProperties;
	if (pobjSourceProps)
	{
		ASSERT_VALID(pobjSourceProps);
		int nProps = pobjSourceProps->GetSize();
		for (int i = 0; i < nProps; i++)
		{
			BObject* pobjSourceProp = DYNAMIC_DOWNCAST(BObject, pobjSourceProps->GetAt(i));
			ASSERT_VALID(pobjSourceProp);
			OBJID lngPropertyID = pobjSourceProp->GetClassID();
			BData* pdatSource = pobjSourceProp->GetData();
			if (pdatSource)
			{
				ASSERT_VALID(pdatSource);
				BData* pdatCopy = pdatSource->CreateCopy();
				this->SetPropertyData(lngPropertyID, pdatCopy, FALSE, FALSE);
				delete pdatCopy;
			}
		}
	}
	
}


// Set the parent for this object.
// See also MoveTo, AddChild
void BObject::SetParent(BObject *pobjNewParent)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pobjNewParent);
	ASSERT_VALID(m_pDoc);

	//, Exit if move is not valid
	// skip for now - bombs if setting to existing parent
//	if (!IsMoveValid(pobjNewParent, FALSE))
//	{
//		ASSERT(0);
//		return;
//	}

	// Exit if moving onto current parent (ie no change)
//	if (pobjNewParent == m_pobjParent)
//		return FALSE; //. or true?

	// Remove the object from its current parent's child collection and set its parent to NULL.
	if (m_pobjParent)
	{
		ASSERT_VALID(m_pobjParent);
		m_pobjParent->RemoveChild(this);
	}
	
	// Add the object to the new parent's child collection
	// Note: This will set this object's parent pointer also
	//   ie this->m_pobjParent = pobjNewParent;
	pobjNewParent->AddChild(this, TRUE);

	// Make sure relationship is set correctly (parent is a two-way relationship)
	ASSERT(this->m_pobjParent == pobjNewParent);
	ASSERT(this->IsChildOf(pobjNewParent, FALSE));

}



void BObject::Export(CFileText &file, BOOL bRecurse, BDataLink& datProps)
{
	ASSERT_VALID(this);
	ASSERT_VALID(&datProps);

	static int nIndent = 0;
	CString strIndent;
	strIndent.Format("%*s", nIndent * 2, ""); 

	switch (file.m_nFormat)
	{
	case ffCsv:
	case ffTab:
		{
			// Walk through properties
			int nProps = datProps.GetLinkCount();
			LPCTSTR psz = NULL;
			for (int i = 0; i < nProps; i++)
			{
				BObject* pobjProp = datProps.GetLinkAt(i);
				ASSERT_VALID(pobjProp);
				OBJID lngPropertyID = pobjProp->GetObjectID();
		//		CString str = this->GetPropertyString(lngPropertyID);
		//		file.WriteValue(str); // will add quotes, etc
				BData* pdat = this->GetPropertyData(lngPropertyID);
				if (pdat)
					psz = pdat->GetBDataText(m_pDoc, lngPropertyID);
				else
					psz = "";
				file.WriteValue(psz); // will add quotes, etc
				// output machine values of certain property types also
				if (pobjProp->PropertyDefHasMachineVersion())
				{
					if (pdat)
						psz = pdat->GetBDataText(m_pDoc, lngPropertyID, TRUE);
					else
						psz = "";
					file.WriteDelim();
					file.WriteValue(psz);
				}

				// add delimiter
				if (i < nProps-1)
				{
					file.WriteDelim();
				}

				delete pdat;
			}
			file.WriteNewline();
		}
		break;
	case ffRtf:
		{
			CString str;
			// format
			// \s1\ql \outlinelevel0 
			str.Format("\\s%d\\ql \\outlinelevel%d ",
				nIndent+1, nIndent
				);
			file.WriteString(str);
			// name
			str.Format("%s\r\n\\par \r\n\r\n",
				(LPCTSTR) this->GetName()
				);
			file.WriteString(str);
			// contents
			CStringEx strText = this->GetPropertyString(propPlainText);
			strText.Trim();
			if (!strText.IsEmpty())
			{
				strText.Replace("\r\n","\r\n\\par ");
				str.Format("\\pard\\plain \\ql\r\n"
					"%s\r\n"
					"\\par\r\n\r\n",
					(LPCTSTR) strText
					);
				file.WriteString(str);
			}
		}
		break;
	case ffText:
		{
			// no indentation
			CString str;
			str.Format("%s%s\r\n",
//				(LPCTSTR) strIndent,
				"",
				(LPCTSTR) this->GetName()
				);
			file.WriteString(str);
			str.Format("%s%s\r\n",
//				(LPCTSTR) strIndent
				"",
				"-------------------------------------------------------"
				);
			file.WriteString(str);
			str.Format("%s%s\r\n\r\n",
//				(LPCTSTR) strIndent,
				"",
				(LPCTSTR) this->GetPropertyString(propPlainText)
				);
			file.WriteString(str);
		}
		break;
	case ffOpml:
		// eg <outline text="Heart of Glass.mp3" type="song" f="Blondie - Heart of Glass.mp3">
		// <outline text=(rtf contents) />
		{
			// yes these format statements must be separate because they use m_strTextCache!
			CString str;
			
			str.Format("%s<outline text=\"%s\" type=\"%s\">\r\n",
				(LPCTSTR) strIndent,
				(LPCTSTR) this->GetName(),
				(LPCTSTR) this->GetPropertyString(propClassName)
				);
			file.WriteString(str);
			// get plain text, and convert all linefeeds to &#10;
			CStringEx strText = this->GetPropertyString(propPlainText);
			strText.Trim();
			if (!strText.IsEmpty())
				strText.Replace("\r\n","&#10;");
			str.Format("%s  <outline text=\"%s\"/>\r\n",
				(LPCTSTR) strIndent,
				(LPCTSTR) strText
				);
			file.WriteString(str);
		}
		break;
	case ffXml:
		// eg <object id=15> 
		//  <type type_id=23>fish</type>
		//  <name>plecy</name>
		{
			// yes these format statements must be separate because they use m_strTextCache!
			CString str;
			// no indent for xml - exporting flat list of objects
//			str.Format("%s<object id=\"%d\">\r\n",
//				(LPCTSTR) strIndent,
//				(LPCTSTR) this->GetObjectID()
//				);
			str.Format("  <object id=\"%d\">\r\n",
				(LPCTSTR) this->GetObjectID()
				);
			file.WriteString(str);

/*			// get plain text
			CStringEx strText = this->GetPropertyString(propPlainText);
			str.Format("    <text><![CDATA[%s]]></text>\r\n",
				(LPCTSTR) strText
				);
			file.WriteString(str);
*/

			// Walk through properties
			int nProps = datProps.GetLinkCount();
			LPCTSTR pszValue = NULL;
			CStringEx strPropName;
			for (int i = 0; i < nProps; i++)
			{
				BObject* pobjProp = datProps.GetLinkAt(i);
				ASSERT_VALID(pobjProp);
				strPropName = pobjProp->GetName();
				OBJID lngPropertyID = pobjProp->GetObjectID();
				BData* pdat = this->GetPropertyData(lngPropertyID);
				if (pdat)
				{
					// proptype. 
					// convert to valid element name. ie limit to a-z and underscore. 
					strPropName.RemoveBadChars();
					pszValue = pdat->GetBDataText(m_pDoc, lngPropertyID);
					str.Format("    <%s><![CDATA[%s]]></%s>\r\n",
						(LPCTSTR) strPropName, 
						pszValue, 
						(LPCTSTR) strPropName
						);
					file.WriteString(str);
					// output machine values of certain property types also
					if (pobjProp->PropertyDefHasMachineVersion())
					{
						strPropName = pobjProp->GetPropertyDefMachineVersionName();
						strPropName.RemoveBadChars();
						pszValue = pdat->GetBDataText(m_pDoc, lngPropertyID, TRUE); // get machine version
						str.Format("    <%s><![CDATA[%s]]></%s>\r\n",
							(LPCTSTR) strPropName, 
							pszValue, 
							(LPCTSTR) strPropName
							);
					}

					delete pdat;
				}

			}


			str.Format("  </object>\r\n"
				);
			file.WriteString(str);
		}
		break;
	}

	// Update progress bar
	app.GetProgressBar().StepIt();

	nIndent++;
	// Now walk through children and call this routine recursively
	if (bRecurse && m_paChildren)
	{
		ASSERT_VALID(m_paChildren);
		int nChildren = m_paChildren->GetSize();
		for (int i = 0; i < nChildren; i++)
		{
			BObject* pobjChild = DYNAMIC_DOWNCAST(BObject, m_paChildren->GetAt(i));
			ASSERT_VALID(pobjChild);
//			pobjChild->Export(file, bRecurse, pobjProps);
			pobjChild->Export(file, bRecurse, datProps);
		}
	}
	nIndent--;

	// write object footer, if any
	switch (file.m_nFormat)
	{
	case ffOpml:
		{
			CString str;
			str.Format("%s</outline>\r\n",
				(LPCTSTR) strIndent
				);
			file.WriteString(str);
		}
		break;
/*	case ffXml:
		{
			CString str;
			str.Format("  </object>\r\n"
				);
			file.WriteString(str);
		}
		break;*/
	}
}



// return true if this property def stores both human and machine-readable info.
// used by export, so can know to export the machine-readable version as well.
BOOL BObject::PropertyDefHasMachineVersion()
{
	ASSERT_VALID(this);
	ASSERT(m_lngClassID == classProperty);

	OBJID idPropType = GetPropertyLink(propPropertyType);
	switch (idPropType)
	{
		case proptypeLink:
		case proptypeDate:
		case proptypeCurrency:
			return TRUE;
			break;
	}
	return FALSE;
}



CString BObject::GetPropertyDefMachineVersionName()
{
	// get property def name, add "ID" or "_value"
	// eg "ClassID", "ParentID"
	switch (this->GetObjectID())
	{
	case propClassID:
		return _T("ClassID");
	case propParentName:
		return _T("ParentID");
	}
	// else
	CString s = this->GetName();
	s += _T("_value");
	return s;
}


