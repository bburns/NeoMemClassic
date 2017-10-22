
#include "precompiled.h"

#include "Test.h"


#include "NeoMem.h"

#include "Brooklyn.h"
#include "ConstantsDatabase.h" //. should be part of brooklyn, mostly
//#include "BDoc.h"
//#include "BDataDate.h"
//#include "BDataIcon.h"
//#include "BFolder.h"
//#include "BClass.h"

#include "BDataEmail.h"

//, ui stuff
//#include "GUI.h"
#include "FrameChild.h"




#include <iostream>
using namespace std;





CTest::CTest() {
}

CTest::~CTest() {
}


//void CTest::TestFolder() {
//}




// Run all Unit Tests
// this is called from InitInstance if in test mode (ie run with /t option).
//. and for now, by ctrl+alt+shift+T command. 

// to turn on nm testing,
// project / properties...
// configuration properties / debugging
// command arguments: /test





// static
void CTest::DoTests(CNeoMem& app) {

	// must wrap in try/catch because we're not inside a windows message handler!
	// see ProcessWndProcException
	try {
		// each module could have a test routine
		// the test command would know about these and call them explicitly
		// could also do various integrative tests
//		BDoc* pdoc = BDoc::GetDoc();
//		BDataBoolean::Test(pdoc);
//		BDataDate::Test(pdoc);
//		BDataHyperlink::Test(pdoc);
//		BDataLink::Test(pdoc);
//		BDataString::Test(pdoc);
//		BObject::Test(pdoc);
//		CRichEditCtrlEx::Test();


		// trace macro
		if (0) {
	
			// trace("CViewTabs");
			// gives output something like - 
			// [..\code\ViewTabs.cpp(80):] CViewTabs
			// (vs doesn't like the [] though)
			#define trace(str) TRACE("%s(%d): " str "\n", __FILE__, __LINE__)


			// why couldn't this just be a fn? 
			// trace(s, ...)
			// cstring str(s, ...)
			// s+= __file__ +...
			// TRACE(s)


			// test 
			trace("hi");
	//		trace("say %s alfie", "frog!"); //, bomb

			// so
	//		trace("here is %d cows", 5);
	//		becomes
	//		TRACE("[%s(%d):] " 
	//			"say %s alfie", "frog!"
	//			"\n",
	//			__FILE__, __LINE__);

	//		want nested evaluation
	//			maybe a fn
	//		ie fn uses printf to get string, then calls TRACE with that string

		}


		// maybe simplest to create a new document, 
		// throw ALL commands at it (esp flaky/suspicious ones), 
		// then check that it's in the proper state.

		BDoc& doc = BDoc::New(); 


		//, should document really know about current object? no, that's a ui thing. 
		// there could be multiple ui's looking at the db. 
		// it's pretty entwined in BDoc though. 
//		currentobject


		// add folder for fish
		BFolder& folderFish = BFolder::New(doc, "fish");

		// check class
		//, propClassID vs propClass??
		OBJID idClass = folderFish.GetPropertyLink(propClassID);
		ASSERT(idClass == classFolder);

		// check location
		OBJID idParent = folderFish.GetPropertyLink(propParent);
		ASSERT(idParent == doc.GetCurrentObject()->id);


		// set description
		CString strDesc("fish i'm thinking of getting");
		folderFish.SetPropertyString(propDescription, strDesc);
		CString str = folderFish.GetPropertyString(propDescription);
		ASSERT(str==strDesc);


		// set name
		CString strName("my fish");
		folderFish.SetPropertyString(propName, strName);
		ASSERT(folderFish.GetPropertyString(propName) == strName);



		// add a fish class
		BClass& classFish = BClass::New(doc, "Fish");

		// check location
		OBJID idLoc = classFish.GetPropertyLink(propParent);
		ASSERT(idLoc == rootClass);


		// set description
		{
		CString strDesc("a thing that swims in the water");
		classFish.SetPropertyString(propDescription, strDesc);
		CString str = classFish.GetPropertyString(propDescription);
		ASSERT(str==strDesc);
		}


		// id is a read-only variable!
//		OBJID idfoo = classFish.id;
//		classFish.id = 2; // fails



		// create a fish, add it to fish folder
//		pdoc->UIAddNewObject(); //, adapt this so can pass params to it...
		//, add desc to New?
		BObject& objPlecy = BObject::New(doc, classFish.id, "plecy", folderFish.id);
		objPlecy.SetPropertyString(propDescription, "plecostomus");



		// set folder default to fish class
		// This will set document modified flag and update views?
//,		pdoc->UIChangeObjectContents(hobjFishClass);
		folderFish.SetPropertyLink(propDefaultClass, classFish.id);
		{
		OBJID id = folderFish.GetPropertyLink(propDefaultClass);
		ASSERT(id == classFish.id);
		}



		// add a new property 'price'
//		pdoc->UIAddNewPropertyDef(); //, adapt this
//,		BObject& objPrice = BPropDef::New(doc, "price", proptypeCurrency, "how much it costs");
		BObject& objPrice = BObject::New(doc, classProperty, "price", folderProperties);
		objPrice.SetPropertyLink(propPropertyType, proptypeCurrency);// reqd
		objPrice.SetPropertyString(propDescription, "how much it costs");
		// check
		BObject* pobj = doc.GetObject(objPrice.id);
		ASSERT(pobj->GetPropertyLink(propPropertyType) == proptypeCurrency);


		// add to fish class
		classFish.SetPropertyLinksAdd(propObjectProperties, objPrice.id);
		
		// see if the new property is in the fish class properties
		{
		// get list of property values
		//,, should be bobjects, or some lightweight recordset
		//, and not need to delete pa at end!
//		ObjIDArray a;
//		classFish.GetPropertyLinks(propObjectProperties, a);
		CObArray* pa = classFish.GetPropertyLinks(propObjectProperties);

		// should be ..& a = classFish.GetPropertyLinks(..);
		// with the reference being to the m_a
		// but... we want COPIES of data, not the original.
		// so have to return a pointer. right? 

		// check first value
		int x = pa->GetSize();
		ASSERT(pa->GetSize() == 1);
		ASSERT(pa->GetAt(0) == &objPrice);
		//, check (add FindItem)
//		ObjIDArray a;
//		classFish.GetPropertyLinks(propObjectProperties, a);
//		ASSERT(a.FindItem(objPrice.id));
//		delete pa;
		pa->RemoveAll();
		delete pa;
		}


		// add another property (will exercise different code)
		BObject& objSize = BObject::New(doc, classProperty, "size", folderProperties);
		objSize.SetPropertyLink(propPropertyType, proptypeNumber);//reqd
		objSize.SetPropertyString(propDescription, "size in inches");

		classFish.SetPropertyLinksAdd(propObjectProperties, objSize.id);

		// check
		{
		CObArray* pa = classFish.GetPropertyLinks(propObjectProperties);
		ASSERT(pa->GetSize() == 2);
		ASSERT(pa->GetAt(0) == &objPrice);
		ASSERT(pa->GetAt(1) == &objSize);
		delete pa;
		}


		// set plecy's price
		objPlecy.SetPropertyString(objPrice.id, "$1.34");
		// check
		ASSERT(objPlecy.GetPropertyString(objPrice.id) == "$1.34"); // works? why? must have good == method
		ASSERT(objPlecy.GetPropertyString(objPrice.id) == CString("$1.34")); 

		




		// add some text to fish folder
		{
		CString str("what am i going to do with all these fish?");
		//, can't use propPlainText - not symmetric. fix it. 
		folderFish.SetPropertyString(propRtfText, str);
		ASSERT(folderFish.GetPropertyString(propRtfText) == str);
		}

		// add another fish
		BObject& objGlassfish = BObject::New(doc, classFish.id, "glassfish", folderFish.id);

		//, check count of fish in db
		//, no way to do a simple query like that!

		objGlassfish.SetPropertyString(objPrice.id, "$2.54");

		//, convert the price prop to a number (instead of currency)
//		objPrice.SetPropertyLink(propPropType, proptypeNumber); // link or long?
//		ASSERT(objGlassFish.GetPropertyData();


		// import a new icon
		CStringEx strTestFolder = app.m_strApplicationFolder + "\\..\\test\\files\\";
		CStringEx strFilename = strTestFolder + "fish.ico";

		//, this should be cleaner
		// put in a BIcon class? ie pass filename to new etc
//,		BIcon& iconFish = BIcon::New(doc, "Fish", strFilename);
		{
		BObject& objIcon = BObject::New(doc, classIcon, "Fish", folderIcons);
		BDataIcon* pdatIcon = new BDataIcon();
		pdatIcon->LoadFile(strFilename);
		objIcon.SetPropertyData(propIconData, pdatIcon);
		delete pdatIcon;

		// set fish class icon
		classFish.SetIconID(objIcon.id);
		}

		//, this is right, but maybe a command would be better. 
		// eg classFish.SetObjectIcon or something. 
		// ie something to mimic the user command. 

		//, next?
//		BIcon& objIcon = BIcon::New(doc, "Fish", strFilename);
//		classFish.SetIconID(objIcon.id);




		// test name functions
		//, note getname is slower though...
		BObject& objOctopus = BObject::New(doc, classFish.id, NULL, folderFish.id);
		ASSERT(objOctopus.GetName() == CString("New Fish")); // default name
//		ASSERT(objOctopus.GetName() == "New Fish"); //, bombs - fix
		ASSERT(objOctopus.GetPropertyString(propName) == CString("New Fish"));
		ASSERT(objOctopus.GetPropertyString(propName) == "New Fish");
		ASSERT(objOctopus.GetPropertyString(propName) == objOctopus.GetName()); 
		objOctopus.SetName("octopus");
		ASSERT(objOctopus.GetName(true) == CString("fish \"octopus\"")); // extended name




		// cool, this casting works!
		// as long as you don't add more vars etc to the subclasses!
//		BFolder* folder = (BFolder*) &folderFish;
//		BFolder& f2 = (BFolder&) folderFish;






		// add price column
		// Get column information from object or from object's classdef
		// this is a copy so we're responsible for it

		// what if gpd().ToColumns() <- does ddcast and assertvalid
		// BDataColumns* pdatColumns = folderFish.GetPropertyData(propColumnInfoArray).ToColumns();
		// much nicer

//		BDataColumns* pdatColumns = DYNAMIC_DOWNCAST(BDataColumns, folderFish.GetPropertyData(propColumnInfoArray));
//		ASSERT_VALID(pdatColumns);
//		folderFish.SetPropertyData(propColumnInfoArray, pdatColumns); // will send hint
//		delete pdatColumns;

		//, not working
//		folder.GetColumns().AddColumn(objPrice.id);
//		BFolder& folder = (BFolder&) folderFish; 
//		BDataColumns& cols = folder.GetColumns();
//		cols.InsertColumn(objSize.id,&doc);
//		folder.PutColumns(cols); //, ugh will delete cols

		// that's hideous. fix it
		// folderFish.GetColumns().InsertColumn(objSize.id,&doc).PutColumns();
		// better but still ugh

//		BDataColumns* pdatColumns = folderFish.GetPropertyColumns(propColumnInfoArray);
//		pdatColumns->InsertColumn(objPrice.id, &doc);
//		folderFish.SetPropertyData(propColumnInfoArray, pdatColumns); // will send hint
//		delete pdatColumns;

		//, can mixup with AddProperty. what does that do again?
		folderFish.AddColumn(objPrice.id);


		// add size column (old fashioned way)
/*
		{
		BDataColumns* pdatColumns = folderFish.GetPropertyColumns(propColumnInfoArray);
		pdatColumns->InsertColumn(objSize.id, &doc, 100);
		folderFish.SetPropertyData(propColumnInfoArray, pdatColumns); // will send hint
		delete pdatColumns;
		}
*/
		folderFish.AddColumn(objSize.id, 100);




		// narrow description column
		{
		BDataColumns* pdatColumns = DYNAMIC_DOWNCAST(BDataColumns, folderFish.GetPropertyData(propColumnInfoArray));
		int nCol = pdatColumns->GetColumnIndex(propDescription);
		int nWidth = pdatColumns->GetColumnWidth(nCol);
		pdatColumns->SetColumnWidth(nCol, int(nWidth * 0.60));
		folderFish.SetPropertyData(propColumnInfoArray, pdatColumns); // sends hint
		delete pdatColumns;
		}

		// narrow price column
		{
		BDataColumns* pdat = DYNAMIC_DOWNCAST(BDataColumns, folderFish.GetPropertyData(propColumnInfoArray));
		int nCol = pdat->GetColumnIndex(objPrice.id);
		int nWidth = pdat->GetColumnWidth(nCol);
		pdat->SetColumnWidth(nCol, int(nWidth * 0.6));
		folderFish.SetPropertyData(propColumnInfoArray, pdat); // sends hint
		delete pdat;
		}

		// int nWidth = obj.GetColumns().GetColumnWidth(nCol);
		// obj.GetColumns().SetColumnWidth(nCol, nWidth*0.6);

		//, but search SetPropertyData(propObjectColumnInfoArray
		
//		folderFish.SetColumnWidth();
		//,, but you could end up duplicating the entire bdatacolumns interface
		// in the bfolder interface. 
		// need a better way. 
		// save for phase two




		// make a category for fish
		BClass& classCategory = BClass::New(doc, "Category");
		BFolder& folderCategories = BFolder::New(doc, "Categories", rootUser);
		BObject& objSalt = BObject::New(doc, classCategory.id, "Saltwater", folderCategories.id);
		BObject& objFresh = BObject::New(doc, classCategory.id, "Freshwater", folderCategories.id);
		
		// make category property
		BObject& propCategory = BObject::New(doc, classProperty, "Category", folderProperties);
		propCategory.SetPropertyLink(propPropertyType, proptypeLink); //, note, this is REQUIRED, but not enforced here... should be
		propCategory.SetPropertyString(propDescription, "oh, fish category you know");
		propCategory.SetPropertyLink(propLinkSource, folderCategories.id); // where to find values

		// add property to the fish class
		classFish.SetPropertyLinksAdd(propObjectProperties, propCategory.id);
		//, note, there is 
//		objTest.ClassDefAddProperty(objSize.id);
		// which does the same thing, because it's not finished. contains just:
//		SetPropertyLinksAdd(propObjectProperties, lngPropertyID);


		// add property to fish folder
/*		{
		BDataColumns* pdatColumns = folderFish.GetPropertyColumns(propColumnInfoArray);
		pdatColumns->InsertColumn(propCategory.id, &doc, 0, 2);
		folderFish.SetPropertyData(propColumnInfoArray, pdatColumns); // will send hint
		delete pdatColumns; //, bombs
		}
*/
		folderFish.AddColumn(propCategory.id, 0, 2);


		// add squid
		BObject& objSquid = BObject::New(doc, classFish.id, "squid", folderFish.id);

		// set single and multiple values 
		{
		objPlecy.SetPropertyLink(propCategory.id, objFresh.id);
		objGlassfish.SetPropertyLink(propCategory.id, objFresh.id);

		// add array
//		CObArray a;
//		a.Add(&objFresh);
//		a.Add(&objSalt);
//		objOctopus.SetPropertyLinks(propCategory.id, &a);

		// simpler
		objSquid.SetPropertyLinksAdd(propCategory.id, objFresh.id);
		objSquid.SetPropertyLinksAdd(propCategory.id, objSalt.id);
		}



		// test GetPropertyLinks and SetPropertyLinksAdd
		{
		BObject& objTest = BObject::New(doc, classClass, "test", folderFish.id);
		CObArray* pa = objTest.GetPropertyLinks(propObjectProperties); // could be null
		ASSERT(pa == NULL);
		delete pa;
		objTest.SetPropertyLinksAdd(propObjectProperties, objSize.id);
		pa = objTest.GetPropertyLinks(propObjectProperties);
		ASSERT(pa);
		ASSERT(pa->GetCount() == 1);
		ASSERT(pa->GetAt(0) == &objSize);
		delete pa;
		objTest.DeleteObject();
		}


		// test ClassDefAddProperty method
		//, move to bclass
		{
		BObject& objTest = BObject::New(doc, classClass, "test", folderFish.id);
		CObArray* pa = objTest.GetPropertyLinks(propObjectProperties); // could be null
		ASSERT(pa == NULL);
		delete pa;
		objTest.ClassDefAddProperty(objSize.id);
		pa = objTest.GetPropertyLinks(propObjectProperties);
		ASSERT(pa->GetCount() == 1);
		ASSERT(pa->GetAt(0) == &objSize);
		delete pa;
		objTest.DeleteObject();
		}


		// GetObject / GetObjectNull
		ASSERT(doc.GetObject(objPlecy.id) == &objPlecy);
//		doc.GetObject(832427); // should bomb - not found
		ASSERT(doc.GetObjectNull(832427) == 0); // okay



		// ** search
		//, do lots more, after adding a bunch of fish with random specs and names
		BObjects a;
		doc.GetObjects(&folderFish, 0, "plec", a); // a query! rather simple
		ASSERT(a.GetCount() == 1);
		ASSERT(a.GetAt(0) == &objPlecy);



		// add a date property
		BObject& propDate = BObject::New(doc, classProperty, "Date Discovered", folderProperties);
		propDate.SetPropertyLink(propPropertyType, proptypeDate);

		// add to class
		classFish.SetPropertyLinksAdd(propObjectProperties, propDate.id);

		// add to folder
/*		{
		BDataColumns* pdatColumns = folderFish.GetPropertyColumns(propColumnInfoArray);
		pdatColumns->InsertColumn(propDate.id, &doc, 100);
		folderFish.SetPropertyData(propColumnInfoArray, pdatColumns); // will send hint
		delete pdatColumns;
		}
*/
		folderFish.AddColumn(propDate.id, 100);


		// set a date value

		//, should be
//		objPlecy.SetPropertyDate(propid, 2012,10,2);
//		objPlecy.SetValue(propid, new BDataDate(2012,10,2));

		// 1. by bdata
		{	
		// first time i've run into the need to create a temporary bdata object. 
		//,, a hassle. why not always create one? ie in getpropertydata
//		BDataDate* pdat = DYNAMIC_DOWNCAST(BDataDate, objPlecy.GetPropertyData(propDate.id));
//		ASSERT(pdat); // failed - propvalue didn't exist!

		BDataDate* pdat = DYNAMIC_DOWNCAST(BDataDate, objPlecy.GetPropertyData(propDate.id, TRUE)); // note TRUE
		pdat->SetDate(2012,10,2,19,39,23);
		objPlecy.SetPropertyData(propDate.id, pdat);
		delete pdat; //, yuck

			// check
			// too verbose
			{
			pdat = DYNAMIC_DOWNCAST(BDataDate, objPlecy.GetPropertyData(propDate.id, TRUE));
			COleDateTime odt;
			odt.SetDateTime(2012, 10, 2, 19, 39, 23);
			COleDateTime odtPlecy = pdat->GetDate();
			ASSERT(odt == odtPlecy);
	//		ASSERT(pdat->GetDate() == odt.SetDateTime(2012, 10, 2, 19, 39, 23)); // why crash? odt.setdt returns an int
			}

			// better
			{
			COleDateTime odtPlecy = objPlecy.GetPropertyDate(propDate.id);
			COleDateTime odt = BDataDate::NewDate(2012, 10, 2, 19, 39, 23);
			ASSERT(odtPlecy == odt);
			}

		}


		// 2. by odd string date
		//, brings up ui dialog - bad
		//, also asserts oddly

		{
		objGlassfish.SetPropertyDate(propDate.id, "ktbday");
		//, return a variant type? string or odt? or the bdata? which is sort of a variant
//		BDataDate* pdat = DYNAMIC_DOWNCAST(BDataDate, objGlassfish.GetPropertyDate(propDate.id));
//		ASSERT(pdat->IsStringDate());
//		ASSERT(pdat->GetBDataText(&doc, propDate.id) == "ktbday");
		//, if had overloading based on return type...
		CString strFish = objGlassfish.GetPropertyString(propDate.id);
		ASSERT(strFish == "ktbday");
		}


		// 3. by numeric date
		{
//		objSquid.SetPropertyDate(propDate.id, 1995, 03, 25);
		}


		// 4. by string date
		{
		objSquid.SetPropertyString(propDate.id, "3/5/1959");
		COleDateTime odt = objSquid.GetPropertyDate(propDate.id);
		ASSERT(odt == COleDateTime(1959,3,5,0,0,0));
		}

		// 5. by odd string dates
		{
		objSquid.SetPropertyString(propDate.id, "afTer fALL 1972  ? ");
		CString strResult = objSquid.GetPropertyString(propDate.id);
		ASSERT(strResult == "After Autumn 1972?");
		}

		{
		objOctopus.SetPropertyString(propDate.id, "jun 2012");
		CString strResult = objOctopus.GetPropertyString(propDate.id);
		ASSERT(strResult == "June 2012");
		}

		//, eh, this is all a bit extraneous to the main goal. 
		// ie adding GetPropertyXXX for missing types, etc.
		// good for api ultimately, and testing, 
		// but not for immediate goal of cleaning up the existing code. 



		// test various properties: email, website, file, folder, hyperlink
		//, very overkill to have all those
		//------------------------------------------------------------------

		// add class
		BClass& classPerson = BClass::New(doc, "Person", "a homo sapiens");
		ASSERT(classPerson.GetPropertyString(propDescription) == "a homo sapiens");

		//, have to set this property to get a person name. weird api. descend from classPerson instead? 
		classPerson.SetPropertyLink(propObjectNamePropertyType, proptypePersonName);

		// add folder
		BFolder& folderPeople = BFolder::New(doc, "people", rootUser, classPerson.id);
		ASSERT(folderPeople.GetPropertyLink(propDefaultClass) == classPerson.id);

		// add people
		BObject& objKate = BObject::New(doc, classPerson.id, "Kate", folderPeople.id); // austen
		BObject& objAnna = BObject::New(doc, classPerson.id, "Anna Lucia Cortez", folderPeople.id);


		// add email property
		BProperty& propEmail = BProperty::New(doc, "Email", "email address", proptypeEmail);

		// add it to person class
		classPerson.SetPropertyLinksAdd(propObjectProperties, propEmail.id);

		// add it to folder
/*		BDataColumns* pdatColumns = folderPeople.GetPropertyColumns(propColumnInfoArray);
		pdatColumns->InsertColumn(propEmail.id, &doc);
		folderPeople.SetPropertyData(propColumnInfoArray, pdatColumns); // will send hint
		delete pdatColumns;
*/
		folderPeople.AddColumn(propEmail.id);


		// set email
		objKate.SetPropertyString(propEmail.id, "kateb@gmail.com");

		// switch to people folder
		doc.SetCurrentObject(&folderPeople);


		// add another person 
		BObject& objJack = BObject::New(doc, classPerson.id, "Jack Shephard", folderPeople.id);

		// add file property
		BProperty& propFile = BProperty::New(doc, "File", "file", proptypeFile);
		folderPeople.AddColumn(propFile.id);
		objJack.SetPropertyString(propFile.id, "neomem.hlp");

		// add folder property
		BProperty& propFolder = BProperty::New(doc, "Folder", "folder", proptypeFolder);
		folderPeople.AddColumn(propFolder.id);
		objJack.SetPropertyString(propFolder.id, "C:\\");

		// add hyperlink property
		BProperty& propHyperlink = BProperty::New(doc, "Hyperlink", "a hyperlink", proptypeHyperlink);
		folderPeople.AddColumn(propHyperlink.id);
		objKate.SetPropertyString(propHyperlink.id, "http://gooogle.com");

		// add website property
		BProperty& propWebsite = BProperty::New(doc, "Website", "wwwww", proptypeWebsite);
//x		folderPeople.SetPropertyColumnsAdd(propColumnInfoArray, propWebsite.id);
		folderPeople.AddColumn(propWebsite.id);
		objJack.SetPropertyString(propWebsite.id, "www.cnn.com");


		// hide description column
		folderPeople.RemoveColumn(propDescription);


		// add boolean property
		BProperty& propBoolean = BProperty::New(doc, "Alien?", "is he/she from mars?", proptypeBoolean);
		folderPeople.AddColumn(propBoolean.id, 100, 2);
		objJack.SetPropertyString(propBoolean.id, "yes");







		// doc.SaveAs();


		//, test file encryption, password function

//x		TestEncryption();

		// so we have this file, and we want to encrypt it when we next save it. 
		// so we set the encryption properties and save it. simplest api. 
		// but why is password optional? um. eh leave it required for now. 

		//, cf uisetencryption

		// enum EncryptionTypes {encryptNone = 0, encryptRC4 = 1};
		// then password
		doc.SetEncryption(BDoc::encryptRC4, "hello"); 
		doc.SaveAs("TestEncryption.neo");
		doc.SetEncryption(BDoc::encryptNone); 


//		doc.Close();
//		eh?

//		doc = BDoc::Open(); // w/password?




		//, take code out of db into ui, eg bdoc.uiaddnewobject
//		BObject* pobjnew = doc.UIAddNewObject();


		//, test odd dates (before jan 210)
		//, go through view classes
		//, go through dialog classes, incl propsheet
		//, keep taking code out of ui into bdoc
		//, and out of ui into cui eg standard dialogs
		//, try to clean up api - getting/setting/saving values. macros?
		//, better query api


		//? test bobject.setpropertydata
//		BObject& obj = objPlecy;
//		BData* pdat = obj.GetPropertyData(propClassName);
//		obj.SetPropertyData(propClassName, pdat);

		//, test pasting garbage into richedit, lots of data, etc.
		//, figure out unicode stuff
		//, check file upgrade process
		//, tests for import/export
		//, fix memory leaks (do later, after new api)
		//, test file compression - oh not there yet
		//, add simple password protection, no encryption





		// ui stuff - put into ui level

		// specific bdata cmds would be in the bdata files. self-contained tests? or separate file. eh. self-contained.
		// test all the methods. 
		{
		CString s = "sculley@fbi.gov";
		BDataEmail* pdat = new BDataEmail();
		pdat->SetBDataText(s);
		ASSERT(pdat->GetBDataText() == s);

		CStringArray& sa = pdat->UIGetCommands();
		ASSERT(sa.GetSize() == 1);
		ASSERT(sa[0] == "&Send Email...");
	
//		pdat->UIEditValue(app.ui); // works


		// set current and target objects. or just target i guess. 
		// set to a folder
		// get access to current view and call this on it? 
		// in ui, pick fish class
//		pview->OnObjChangeClassContents();
		// then here check things work.
		// first that fish class is new value
		// then add something to a folder
		// check that it's a fish

		// so only user intervention needed is to pick fish
		// but still, not tenable for long term use. 
		// hm but want how can we move some of this to db?
		// 



		}


/*
		CUI& ui = app.ui;





		// select the fish folder
		doc.SetCurrentObject(&folderFish); //, memory leak - carray 40bytes and bdataviews.createcopy
		// check
		ASSERT(doc.GetCurrentObject() == &folderFish);

		// this will do a switch on proptype, bring up appropriate dialog, 
		// write the new value back to the object
		ui.EditValue(&objPlecy, propDescription);
		ui.EditValue(&objPlecy, propDate.id);

		// select the search view
		CFrameChild* pframe = app.GetChildFrame();
		pframe->ShowView(viewSearch);
//,?	app.ShowView(viewSearch);
*/





		// test making copies of bobjects etc
//		BObject* pobj = doc.GetObject(objPlecy.id);
//		pobj->GetPropertyLong(


		// save doc
//		doc.Save(); // if modified brings up saveas dialog etc
//		doc.Save("../test/files/TestRun.neo");
		doc.SaveAs("../test/files/TestRun.neo");
		doc.Save(); // should not bring up dialog. 



	}
	catch (CException* e) {
		//?
		const MSG msg = {0};
		AfxGetThread()->ProcessWndProcException(e, &msg);
	}


//, beeps too loud		AfxMessageBox("All tests done.");

}




void TestEncryption() {


	// load file, turn on encryption, save it new name, load it, turn off encryption, save as 3rd name,
	// then compare first and third files

	


}





