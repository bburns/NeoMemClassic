
// CNeoMem


#include "precompiled.h"


// Application
#include "NeoMem.h"
#include <eh.h> // for set_terminate
#include "CommandLineInfoEx.h"
#include "Constants.h"
#include "DocManagerEx.h"
#include "Email.h"
#include "FileFormat.h"
#include "FileDialogEx.h"
#include "Hint.h"
#include "IconCache.h"
#include "NeoDocTemplate.h"
#include "PropertySheetEx2.h"
#include "Strings.h"
#include "HelpIDs.h"

#include "Path.h"
using namespace nsPath;

// Database
//,#include "Brooklyn.h"
#include "BDataPersonName.h" // for constants
#include "BDoc.h"
#include "ConstantsDatabase.h"
#include "BDataIcon.h"


// UI
//,#include "Gui.h"
#include "DialogAbout.h"
#include "DialogEditName.h"
#include "DialogError.h"
#include "DialogMessage2.h"
#include "DialogNotice.h"
#include "DialogWelcome.h"
#include "FrameMain.h"
#include "FrameChild.h"
#include "PageOptionsDisplay.h"
#include "PageOptionsFile.h"
#include "SheetOptions.h"
#include "ViewRtf.h"
#include "ViewTabs.h"


#include "ViewContents.h" // for testing
#include "Test.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif





// Header defaults
const COLORREF clrHeaderBackgroundDefault = 0x80c0b8;
const COLORREF clrHeaderForegroundDefault = 0x00ffffff;
const int nHeaderFontSizeDefault = 160;
const LPCTSTR szHeaderFontNameDefault = _T("Arial");





// Function headers
//typedef void (*TERMINATE_FN)();
//TERMINATE_FN old_terminate = NULL;

// Handle an exception that was thrown outside of a message loop.
//, doesn't get called
void TerminateRoutine() {
	AfxMessageBox("Unhandled Exception", MB_ICONEXCLAMATION);
	// cleanup
	// call old terminate routine
//	if (old_terminate)
//		old_terminate();
	// Exit 
	exit(-1);
}



BEGIN_MESSAGE_MAP(CNeoMem, CWinApp)
	//{{AFX_MSG_MAP(CNeoMem)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HEADER, OnUpdateViewHeader)
	ON_COMMAND(ID_APP_ABOUT, OnCmdHelpAbout)
	ON_COMMAND(ID_VIEW_OPTIONS, OnCmdViewOptions)
	ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
	ON_COMMAND(ID_FILE_PAGE_SETUP, OnFilePageSetup)
	ON_COMMAND(ID_VIEW_HEADER, OnCmdViewHeader)
	ON_COMMAND(ID_VIEW_HEADER_FONT, OnCmdViewHeaderFont)
	ON_COMMAND(ID_VIEW_HEADER_COLOR, OnCmdViewHeaderColor)
	ON_COMMAND(ID_VIEW_HEADER_RESET, OnCmdViewHeaderReset)
	ON_COMMAND(ID_HELP_WEBSITE, OnCmdHelpWebsite)
	ON_COMMAND(ID_HELP_EMAIL, OnCmdHelpEmail)
	ON_COMMAND(ID_HELP_KEYBOARD, OnCmdHelpKeyboard)
	ON_COMMAND(ID_HELP_TIP_OF_THE_DAY, OnCmdHelpTipOfTheDay)
	ON_COMMAND(ID_HELP_SHOW_WELCOME, OnCmdHelpShowWelcome)
	ON_COMMAND(ID_HELP_WHAT_IS, OnCmdHelpWhatIs)
	ON_COMMAND(ID_HELP_WHATS_NEW, OnHelpWhatsNew)
	//}}AFX_MSG_MAP

//	ON_COMMAND(CG_IDS_TIPOFTHEDAY, ShowTipOfTheDay)

	// just use cframemain oncontexthelp instead - cwinapp's has been superceded by cframewnd's
//	ON_COMMAND(ID_CONTEXT_HELP, CWinApp::OnContextHelp);

	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
//	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen) // use our own version of OnFileOpen

	// Standard print setup command
//	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup) // already listed above
END_MESSAGE_MAP()



// Constructor
CNeoMem::CNeoMem() : 

//	m_pMainWnd(0),
	m_bAdmin(FALSE),
	m_bAutoStart(FALSE),
	m_bDisplayGridlines(FALSE),
	m_bDisplayHeader(FALSE),
	m_bOptionsLoaded(FALSE),
	m_bShowWelcome(FALSE),
	m_bAppShuttingDown(FALSE),
	m_bWindowsShuttingDown(FALSE),
	m_bAutoLoad(FALSE),
	m_cfObjects(0),
	m_cfRtf(0),
	m_hCursorHand(0),
	m_lngExcludeFlags(0),
	m_lngSearchExcludeFlags(0),
	m_nNameFormat(0),
	m_nTip(0),
	m_nUnits(0),
	m_bAutoBackup(FALSE),
	m_bAutoRecover(FALSE), 
	m_nAutoRecoverMinutes(5),
	m_bDoingAutoRecover(FALSE),
	m_bPrintHeaderFooter(TRUE),
	m_bIgnoreLoadHint(FALSE),
	m_bRegisterFileType(TRUE),
	m_lngPrintViewID(0)

{

	m_sizePaper = CSize(8*1440 + 720, 11*1440); // default size 8.5x11"
//. ole  	m_pRedoc = 0; // created/destroyed in framemain

	EnableHtmlHelp();
  
	// in vs10, some errors get caught at the wrong location, so add this
	// see http://connect.microsoft.com/VisualStudio/feedback/details/563622/mfc-default-exception-handling-causes-problems-with-activation-context
	// AfxSetAmbientActCtx(FALSE);
}



// Register application manually - using MFC's you would get the 
// new file and also print commands registered. 
void CNeoMem::RegisterApplication() {

//		[HKEY_CLASSES_ROOT\.neo]
//			@="Neomem.Document"
//		[HKEY_CLASSES_ROOT\Neomem.Document]
//			@="NeoMem Document"
//		[HKEY_CLASSES_ROOT\Neomem.Document\DefaultIcon]
//			@="E:\\PROJECTS\\NEOMEM\\DEBUG\\NEOMEM.EXE,1"
//		[HKEY_CLASSES_ROOT\Neomem.Document\shell]
//		[HKEY_CLASSES_ROOT\Neomem.Document\shell\open]
//		[HKEY_CLASSES_ROOT\Neomem.Document\shell\open\ddeexec]
//			@="[open(\"%1\")]"
//		[HKEY_CLASSES_ROOT\Neomem.Document\shell\open\command]
//			@="E:\\PROJECTS\\NEOMEM\\DEBUG\\NEOMEM.EXE /dde"

/*
	TCHAR pszExeName[MAX_PATH];
	if (::GetModuleFileName(NULL, pszExeName, MAX_PATH)) {
		CRegKey r;
		CString str;

		r.Create(HKEY_CLASSES_ROOT, ".neo2");
		r.SetValue("NeoMem2.Document", NULL);

		r.Create(HKEY_CLASSES_ROOT, "NeoMem2.Document");
		r.SetValue("NeoMem Document", NULL);
		str.Format("%s,1", pszExeName);
		r.SetValue((LPCTSTR) str, "DefaultIcon");
		r.Create(r.m_hKey, "shell");
		r.Create(r.m_hKey, "open");
		HKEY hKey = r.m_hKey;
		r.Create(hKey, "ddeexec");
		r.SetValue("[open(\"%1\")]", NULL);
		r.Create(hKey, "command");
		str.Format("%s /dde", pszExeName);
		r.SetValue((LPCTSTR) str, NULL);
	}
*/

	RegisterShellFileTypes(TRUE); // True for win95
	// Remove the ShellNew key so our doc type doesn't show up in File/New
	// MFC has set HKEY_CLASSES_ROOT\.neo\ShellNew\NullFile = ""
	// also remove print and printto
	RegDeleteKey(HKEY_CLASSES_ROOT, ".neo\\ShellNew");
	RegDeleteKey(HKEY_CLASSES_ROOT, "NeoMem.Document\\shell\\print");
	RegDeleteKey(HKEY_CLASSES_ROOT, "NeoMem.Document\\shell\\printto");
//	HKEY hkey;
//	if (RegOpenKey(HKEY_CLASSES_ROOT,".neo\\ShellNew",&hkey)= =ERROR_SUCCESS) {
//		RegDeleteValue(hkey, "NullFile");
//		CString s = "SCRIBBLE.EXE /ShellNew";
//		DWORD len = s.GetLength() + 1;
//		RegSetValueEx(hkey, "Command", 0, REG_SZ, (BYTE*)(LPCSTR)s, len);
//		RegCloseKey(hkey);
//	}

}


// InitInstance is called by AfxWinMain, which then enters the thread's message 
// loop via Run. see mfc/src/WinMain.cpp and ThrdCore.cpp
BOOL CNeoMem::InitInstance() {

	TRACE("InitInstance\n");

	// Turn on memory overwrite checking
	// The debug memory allocator puts guard bytes around all memory allocations. However, 
	// these guard bytes don't do any good unless you check whether they have been changed 
	// (which would indicate a memory overwrite). Otherwise, this just provides a buffer that might 
	// in fact allow you to get away with a memory overwrite.
	// By turning on the checkAlwaysMemDF, you will force MFC to make a call to the AfxCheckMemory 
	// function every time a call to new or delete is made. If a memory overwrite was detected, 
	// it will generate a TRACE message that looks similar to the following:
	// Damage Occurred! Block=0x5533
	#if defined(_DEBUG) && !defined(_AFX_NO_DEBUG_CRT)
		afxMemDF |= checkAlwaysMemDF;
	#endif

//	afxTraceFlags |= traceCmdRouting; // Trace command routing
//	afxTraceFlags |= traceWinMsg; // Trace windows messages
//	AfxSetAllocHook(AllocHook); // Hook into memory allocation
//	if (_heapchk() != _HEAPOK) DebugBreak(); // can do in debug and release mode

	// If you need to detect whether another instance already exists, create a uniquely named 
	// mutex using the CreateMutex function. CreateMutex will succeed even if the mutex 
	// already exists, but the GetLastError function will return ERROR_ALREADY_EXISTS. This indicates 
	// that another instance of your application exists, because it created the mutex first. 
	// Create Mutex to see if instance is already running
//	HANDLE hMutex = ::CreateMutex(NULL, TRUE, "NeoMemMutex");
//	if (::GetLastError() == ERROR_ALREADY_EXISTS)
//	{
//		AfxMessageBox("Only one instance of NeoMem allowed at a time!");
//		return FALSE;
//	}

	// Make sure we're not running on NT or 2000 (won't work with unicode strings yet)
//    IDS_ERROR_NT            "NeoMem will not run on Windows NT or 2000!"
//	if (m_objInfo.Unicode())
//	{
//		AfxMessageBox(IDS_ERROR_NT, MB_ICONINFORMATION);
//		return FALSE;
//	}

	// Set the terminate routine which catches unhandled exceptions not caught in a message loop.
	// Bug: Converted TerminateRoutine from a class method to a global function but forgot to 
	// remove the definition in the .h file, so couldn't get this to compile
	set_terminate(TerminateRoutine);

	// Make sure we are running on a decent version of the common controls - if not,
	// tell user to upgrade
	if (!UpdateCommonControls())
		return FALSE;

	// Check which version of rtf control the user has
	// Before creating a rich edit control, you should call the LoadLibrary function to verify which version 
	// of Rich Edit is installed. 
	// Microsoft Windows®  2000 includes Rich Edit 3.0 with a Rich Edit 1.0 emulator.
	// Microsoft® Windows NT® version 4.0 includes Rich Edit 1.0 and 2.0. 
	// Windows 98 includes Rich Edit 1.0 and 2.0. 
	// Windows 95 OSR2 includes Rich Edit 2.0
	// Windows 95 includes only Rich Edit 1.0. However, Riched20.dll is compatible with Windows 95 
	// and may be installed if an application that uses Rich Edit 2.0 has been installed. 
	// 2.0 included with win95 osr2, vb 5, vc 5, visual studio 6, office 97
	// 3.0 included with office 2000, visual studio.net, windows 2000, windows xp, windows me 
	// 4.0 included with 
	float fVersion = m_objInfo.GetRichEditVersionNumber();
//	float fVersion = 1.0;
	if (fVersion < 5.0f) {
		// user doesn't have RichEd20.dll on their system
		AfxMessageBox("Notice: NeoMem requires version 2.0 or 3.0 of the RichEdit control, which does "
			"not appear to be on your system. This control is included in various Microsoft products - the "
			"easiest way to obtain it is by installing the latest Word Viewer from Microsoft. See the NeoMem "
			"readme file for more information. ", MB_ICONINFORMATION);
		return FALSE;
	}

	//. Make sure we have CryptoAPI available?
	// Windows 95, Windows NT 3.5x, and Windows NT 4.0 do not support Microsoft's CryptoAPI 
	// standard out of the box. For Windows 95, Internet Explorer (IE) 3.02 or higher installs crypto.
	// For Windows NT 4.0, Service Pack 3 installs it. Windows NT 3.51 does not support CryptoAPI.
	// will bomb if try to use crypto functions and they're not available

	//, Make sure we are running on 16-bit displays at least
//	if (m_b

	// Save the current (app) directory so we know where to find the template and example files.
//,	DWORD dwLength = MAX_PATH;
	DWORD dwLength = 2048; // MAX_PATH;
	LPTSTR szPath = m_strApplicationFolder.GetBuffer(dwLength);
	// bug: current directory isn't always where the .exe is for some reason - sometimes it's C:\
//	::GetCurrentDirectory(dwLength, szPath);
//	_tcscpy(szPath, m_pszExeName); // this is just "NEOMEM"
	::GetModuleFileName(NULL, szPath, dwLength); // full path and file name
	m_strApplicationFolder.ReleaseBuffer();
	m_strApplicationFolder = m_strApplicationFolder.LeftFrom('\\'); // remove the file name
	m_strIniFolder = m_strApplicationFolder;
	// 1.2 delete the template1.1.neo file, if it exists (moving back to just template.neo)
	CString strOldTemplate = m_strApplicationFolder + _T("\\Template1.1.neo");
	if (Library::FileExists(strOldTemplate))
		_tremove(strOldTemplate);

	// 1.1 if app is in debug or release folder, use the main documents folder to find template file
	// (prevents having 3 copies of it lying around...)
	// Just go up one folder to main project folder then down to files folder (was Documents)
	// 1.2 moved this here from LoadOptions
	if ((m_strApplicationFolder.Right(5) == "Debug") || (m_strApplicationFolder.Right(7) == "Release"))
		m_strApplicationFolder = m_strApplicationFolder.LeftFrom('\\') + _T("\\files"); 

	// Locate the help file (when files are in final configuration)
	//. check for final version
	//. free psz memory first
//	m_pszHelpFilePath = "NeoMem.chm";

	// Get system colors to static variables
	Library::LoadColors();

	// Create global fonts to use in various controls
//	m_fontControls.CreatePointFont(80, _T("MS Sans Serif"));
	m_fontControls.CreateStockObject(ANSI_VAR_FONT);

	// Create bold version
	LOGFONT lf;
	m_fontControls.GetLogFont(&lf);
	lf.lfWeight = 700; // bold
	m_fontControlsBold.CreateFontIndirect(&lf);

	// Load cursors
	m_hCursorHand = LoadCursor(IDC_CURSOR_HAND);

	// Initialize global NUMBERFMT structure used by fc and ::GetNumberFormat.
	// Need to do this in order to override the default of always having 2 digits after decimal.
	g_nf.NumDigits = 0;
	g_nf.LeadingZero = 0; // number of digits after decimal (default is 2)
	g_nf.Grouping = 3;
	g_nf.lpDecimalSep = ".";
	g_nf.lpThousandSep = ",";
	g_nf.NegativeOrder = 1;

	// Initialize OLE 2.0 libraries
	TRACE("initialize ole libraries\n");
	if (!AfxOleInit()) {
		AfxMessageBox(IDP_AFXOLEINIT_FAILED);
//		return FALSE;
	}


	// Register our clipboard formats
	TRACE("register clipboard formats\n");
	m_cfObjects = ::RegisterClipboardFormat(_T("NeoMem Objects"));
	m_cfRtf = ::RegisterClipboardFormat(_T("Rich Text Format"));
	//, put in resource file?
//	CString strFormat;
//	strFormat.LoadString(IDS_CLIPBOARD_FORMAT_NEOMEM_OBJECTS);
//	m_cfObjects = ::RegisterClipboardFormat(strFormat);
//	strFormat.LoadString(IDS_CLIPBOARD_FORMAT_RTF);
//	m_cfRtf = ::RegisterClipboardFormat(strFormat);

	// Load tips from resource
//	LoadTips();

	// Call this function in your application object's InitInstance function to 
	// enable support for containment of OLE controls. (allows it to be a container)
	TRACE("enable ole containment\n");
	AfxEnableControlContainer();

	// Enable use of rich edit control
	//* Note: This must be called before any richedit 2.0 dialog is loaded!!!
	TRACE("initialize rich edit control\n");
	AfxInitRichEdit();
//	AfxInitRichEditEx(); // Loads 2.0/3.0
	// no message needed here - already have above
//	{
//		AfxMessageBox("Unable to load Rich Edit 2.0", MB_OK | MB_ICONEXCLAMATION);
//		return FALSE;
//	}

	// Load application image list (incl up down icons for listview header)
	TRACE("load image list\n");
	LoadImageList();

	// Make list of file formats for import/export
	m_ffCsv.Init(ffCsv, "Comma Separated Values","","csv");
	m_ffTab.Init(ffTab, "Tab Delimited Values","","txt");
	m_ffRtf.Init(ffRtf, "RTF Outline","","rtf");
	m_ffText.Init(ffText, "Plain Text","","txt");
	m_ffXml.Init(ffXml, "XML","","xml");
	m_ffHtml.Init(ffHtml, "HTML","","htm");
	m_ffOpml.Init(ffOpml, "OPML Outline","Outline Processor Markup Language","opml");
	m_listExportFormats.Add(&m_ffCsv);
	m_listExportFormats.Add(&m_ffTab);
	m_listExportFormats.Add(&m_ffRtf);
	m_listExportFormats.Add(&m_ffText);
	m_listExportFormats.Add(&m_ffXml);
//	m_listExportFormats.Add(&m_ffOpml);
	m_listImportFormats.Add(&m_ffRtf);
	m_listImportFormats.Add(&m_ffText);

	// make filters for file dialogs
	// eg "Rich Text Format Files (*.rtf)|*.rtf|Text Files (*.txt)|*.txt|All Files (*.*)|*.*||"
	m_strExportFilter = m_listExportFormats.GetFilter();
	m_strImportFilter = m_listImportFormats.GetFilter();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need.
	// no longer needed for vs2010
//	TRACE("enable 3d controls\n");
//	#ifdef _AFXDLL
//		Enable3dControls();			// Call this when using MFC in a shared DLL
//	#else
//		Enable3dControlsStatic();	// Call this when linking to MFC statically
//	#endif


	// 1.2 use ini file instead of registry (feature 394)
/*	// Set registry key
	// this tells mfc to use the registry for settings
	TRACE("set registry key\n");
	#ifdef _DEBUG
		SetRegistryKey(szRegKeyDebug);
	#else
		SetRegistryKey(szRegKey);
	#endif
*/
	CString sInifile = m_strIniFolder + _T("\\") + CString(m_pszAppName) + _T(".ini");
	MoveRegistryToIniFile(sInifile);
	UseIniFile(sInifile);

	// Load standard INI file options (including MRU)
	LoadStdProfileSettings(8);

	// Load options from registry
	TRACE("load options\n");
	LoadOptions();

	// Load some strings
	TRACE("load strings\n");
	LoadStrings();

	// Use extended CDocManager
	// Add before you create any doc templates!
	m_pDocManager = new CDocManagerEx;

	// Register the application's document templates.  
	TRACE("register doc template\n");
	// Document templates serve as the connection between documents, frame windows and views.
	// The application object stores a pointer to each document template in its template 
	// list and provides an interface for adding document templates. 
	// The document template stores pointers to the CRuntimeClass objects for the document, 
	// view, and frame window classes. These CRuntimeClass objects are specified when constructing 
	// a document template.
	// Because CDocTemplate is an abstract class, you cannot use the class directly. 
	// A typical application uses one of the two CDocTemplate-derived classes provided by the 
	// Microsoft Foundation Class Library: CSingleDocTemplate, which implements SDI, and 
	// CMultiDocTemplate, which implements MDI. 
	// why is just viewtree here? Because the creation of the child frame is overridden
	// to create more than one view object
//	CMultiDocTemplate* pDocTemplate;
//	pDocTemplate = new CMultiDocTemplate(
	BDocTemplate* pDocTemplate;
	pDocTemplate = new BDocTemplate(
		IDR_NEOMEM_TYPE,
		RUNTIME_CLASS(BDoc),	// Document
		RUNTIME_CLASS(CFrameChild),	// Custom MDI child frame
		RUNTIME_CLASS(CViewTabs));	// View
	AddDocTemplate(pDocTemplate);

	// The document template contains the ID of the resources used with the document type 
	// (such as menu, icon, or accelerator table resources). The document template also 
	// has strings containing additional information about its document type. These include 
	// the name of the document type (for example, “Worksheet”) and the file extension 
	// (for example, “.xls”). Optionally, it can contain other strings used by the 
	// application’s user interface, the Windows File Manager, and OLE support.
	pDocTemplate->GetDocString(m_strFileExtension, CDocTemplate::filterExt);

	// This example accesses the doc template object to construct
	// a default document name such as SHEET.XLS, where "sheet"
	// is the base document name and ".xls" is the file extension
	// for the document type.
/*
	CString strDefaultDocName, strBaseName, strExt;
	CDocTemplate* pDocTemplate = GetDocTemplate();
	if (!pDocTemplate->GetDocString(strBaseName, CDocTemplate::docName)
	  || !pDocTemplate->GetDocString(strExt, CDocTemplate::filterExt))
	{
	  AfxThrowUserException(); // These doc template strings will
		 // be available if you created the application using AppWizard
		 // and specified the file extension as an option for
		 // the document class produced by AppWizard.
	}
	strDefaultDocName = strBaseName + strExt;
*/

	// Register file type
	// This eliminates the need to ship a .REG file with your application.
	// Note: If the registration database already associates a given filename extension with another 
	// file type, no new association is created. 
	// Only do this if we're running in release mode, otherwise when you double click a file it'll run in debug mode
#ifndef DEBUG
	if (m_bRegisterFileType) {
		TRACE("register file type\n");
		EnableShellOpen(); 
		RegisterApplication();
	}
#endif

	// Create the main MDI Frame window
	TRACE("create main frame\n");
	CFrameMain* pMainFrame = new CFrameMain;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// This code replaces the MFC created menus with the Ownerdrawn versions. // bcmenu
	TRACE("replace mfc menus with bcmenus\n");
	pDocTemplate->m_hMenuShared = pMainFrame->NewMenu();
	pMainFrame->m_hMenuDefault = pMainFrame->NewDefaultMenu();

	// This simulates a window being opened if you don't have a default window displayed at startup. // bcmenu
	TRACE("simulate window being opened(?)\n");
	pMainFrame->OnUpdateFrameMenu(pMainFrame->m_hMenuDefault);

	// Parse command line for standard shell commands, DDE, file open.
	// The CCommandLineInfo class aids in parsing the command line at application startup.
	// An MFC application will typically create a local instance of this class in the InitInstance 
	// function of its application object. This object is then passed to CWinApp::ParseCommandLine, 
	// which repeatedly calls ParseParam to fill the CCommandLineInfo object. The CCommandLineInfo 
	// object is then passed to CWinApp::ProcessShellCommand to handle the command-line arguments and flags.
	// 1.1c use our extended version to handle /test and /admin
//	CCommandLineInfo cmdInfo;
	CCommandLineInfoEx cmdInfo;

	// Call this member function to parse the command line and send the parameters, 
	// one at a time, to CCommandLineInfo::ParseParam, which will check each one and set 
	// flags and variables in cmdInfo as necessary.
	TRACE("parse command line\n");
	ParseCommandLine(cmdInfo);

	// Admin / Object Filter
	// If the value includes flagSystem, then the tree will start with the user root as the root item.
	// If the value does not include flagSystem, then the tree will start with the system root as the root item.
	// 1.1c handle admin mode (/admin). this will replace modifying the registry key Admin.
	m_bAdmin = cmdInfo.IsAdmin();
	if (m_bAdmin) {
//		m_lngSearchExcludeFlags = 0; // Don't exclude anything from searches
//		m_lngExcludeFlags = 0; // Don't exclude any classes, properties, property types, etc.
		m_lngSearchExcludeFlags = flagTemp; // Don't exclude anything from searches
		m_lngExcludeFlags = flagTemp; // Don't exclude any classes, properties, property types, etc.
	}
	else {
//		m_lngSearchExcludeFlags = flagSystem; // Exclude system objects from search results
//		m_lngExcludeFlags = flagAdminOnly; // Exclude certain classes, properties, property types, etc. from various dialogs presented to the user
		m_lngSearchExcludeFlags = flagSystem | flagTemp; // Exclude system objects from search results (, and index!)
		m_lngExcludeFlags = flagAdminOnly | flagTemp; // Exclude certain classes, properties, property types, etc. from various dialogs presented to the user
	}

	// Set some flags
	BOOL bCreateNewFile = (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew);
	BOOL bOpenExistingFile = (cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen) ||
												(cmdInfo.m_nShellCommand == CCommandLineInfo::FileDDE);

	// If we're going to show the welcome dialog and there was nothing on the command line, 
	// switch to FileNothing so that no new document is created.
	if (m_bShowWelcome && bCreateNewFile) 
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing; // don't open a new document

	// If we're going to autoload a file on startup, switch to FileNothing so that no new document is created.
	// Only do this if user did not double click on a file to open it (ie bCreateNewFile must be true also).
	if (m_bAutoLoad && bCreateNewFile) 
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing; // don't open a new document

	// 1.1c handle test mode (/test)
	m_bTestMode = cmdInfo.IsTest();
	if (m_bTestMode) {
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing; // don't open a document, new or old
		m_bAutoLoad = FALSE; // make sure we don't autoload a file either
		m_bShowWelcome = FALSE; // and no welcome dialog. 
	}

	// Process command - ie calls File New etc based on settings in cmdInfo object
	// Dispatch commands specified on the command line.
	// If user is just running the application, cmdInfo will be New file, so this is 
	// where the new CDocument object is created.
	// If user double clicked on a filename then cmdInfo will be Open file, which will
	// create a new CDocument object and then load it from the file.
	// actually, it will be DDE (specified by "/dde" on command line, which means wait for a dde command which
	// will be send to your main window)
	TRACE("process command line\n");
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Maximize the child mdi window before it is displayed
//	CMDIChildWnd* pChild = pMainFrame->MDIGetActive();
//	if (pChild != NULL)
//		pMainFrame->MDIMaximize(pChild);

	//. Get this from the registry
//	m_nCmdShow = SW_SHOWMAXIMIZED;

	// The main window has been initialized, so show and update it.
	TRACE("show main window\n");
	if (!pMainFrame->LoadWindowPlacement(Strings::szSettings, Strings::szWindowPosition))
		pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Create invisible rtf control 
	TRACE("create invisible rtf control\n");
	m_rtf.Create(WS_CHILD | ES_MULTILINE, CRect(0,0,0,0), AfxGetMainWnd(), 100);
	// 1.1 Set max length
	// Bug: Text was being truncated on moving text to another object - turned out text was limited
	// to 32k by default. Had never noticed this - only tested on short text views.
	m_rtf.LimitText(0xffffff);  // 16MB // this sends EM_LIMITTEXT message

	// Show Tip of the Day
	// this is a problem if using dde to open a file (ie from explorer)
	// dde command gets sent to the active window or something
	// so need to open this window AFTER dde is finished opening the file, somehow
	// need to find right place in framework to do this
//	if (m_bShowTips)
//		OnCmdHelpTipOfTheDay();

	// Show welcome dialog, ONLY IF not in dde mode (dialog screws it up),
	// AND if we're not going to reload a file on startup.
	if (m_bShowWelcome && (cmdInfo.m_nShellCommand != CCommandLineInfo::FileDDE) && !m_bAutoLoad) {
		ShowWelcomeDialog();
	}

	// AutoLoad file, if specified in Options dialog, and user did not double click on a file
	TRACE("autoload file\n");
	if (m_bAutoLoad && !bOpenExistingFile) {

		// Check for blank file name
		if (m_strAutoLoadFileName.IsEmpty()) {
			// do nothing
		}
		// Check if autoload file exists...
		else if (!Library::FileExists(m_strAutoLoadFileName)) {
			CString str;
			str.Format("Could not open the AutoLoad file '%s'.", (LPCTSTR) m_strAutoLoadFileName);
			AfxMessageBox(str, MB_ICONINFORMATION);
			// Turn off startup option
			// don't need to do this - presumably user will load a real file now and will
			// want it to come up next time they run the program
//			m_bAutoLoad = FALSE;
		}
		else {
			// This gives a messagebox if fails, but not very clear...
			if (!OpenDocumentFile(m_strAutoLoadFileName))
			{
				// failed
			}
		}
	}

	// Start AutoRecover timer, if specified
	if (m_bAutoRecover) {
		TRACE("start autorecover timer\n");
		GetMainFrame()->SetAutoRecoverTimer(m_nAutoRecoverMinutes);
	}

	// why would this interfere with dde? the main window has been loaded and is visible at this point
//	AfxMessageBox("InitInstance show welcome dialog here?");

	// CG: This line inserted by 'Tip of the Day' component.
//	ShowTipAtStartup();

	// If in test mode, start the unit tests
	if (m_bTestMode)
//		DoTests();
		CTest::DoTests(*this);

	

	// Windows Explorer sends a WM_DDE_EXECUTE message with command string 
	// "[open("E:\Projects\NeoMem\NeoMem\Template.neo")]" to the frame window.
	// Frame window calls OnDDEExecute in Winfrm.cpp, which acknowledges the message 
	// by sending WM_DDE_ACK to the sender, and then calls
	// AfxGetApp()->OnDDECommand(szCommand);
	// so maybe if we include this in our welcome dialog, we can still handle the dde msg?
	// ON_MESSAGE(WM_DDE_EXECUTE, OnDDEExecute)
	// no go!! i don't know why!

	return TRUE;
}



// Override ExitInstance if you need special cleanup processing, such as freeing graphics 
// device interface (GDI) resources or deallocating memory used during program execution. 
// Cleanup of standard items such as documents and views, however, is provided by the 
// framework, with other overridable functions for doing special cleanup specific to those 
// objects.
// Note: At this point, the windows have all been destroyed, so can't save Window positions here.
int CNeoMem::ExitInstance() {
	//trace("CNeoMem::ExitInstance\n");

	// Set flag
//	m_bAppShuttingDown = TRUE;

	// Call CoUninitialize() in CDBApp::ExitInstance.
	// I think AfxOleInit takes care of this automatically
//	::OleUninitialize();

	// Detach memory block from memory file
//	if (m_memTips.GetLength() > 0)
//		m_memTips.Detach();

	// Save options to ini file if they were loaded
	if (m_bOptionsLoaded)
		SaveOptions();

	// Destroy rtf window if it was created
	if (::IsWindow(m_rtf.m_hWnd))
		m_rtf.DestroyWindow();

	// BUG:: this is where a crash happened.
	// The problem is, MFC puts m_nShowCmd into m_pCmdInfo temporarily,
	// and is supposed to remove it on receiving the DDE message, but 
	// the DDE message was never received.
	// This was due to short .exe name not being used. This was fixed but problem persisted.
	// It turns out it was because I had left some msgboxes in InitInstance, which were
	// somehow interfering with the DDE messaging, so the symptoms remained the same.
	// Lesson: MsgBoxes are BAD FOR DEBUGGING!
	if (m_pCmdInfo) {
		if (!AfxIsValidAddress(m_pCmdInfo, sizeof(CCommandLineInfo))) {
			CString str;
			str.Format("Warning: m_pCmdInfo was invalid (%d) - setting to 0.", m_pCmdInfo);
			AfxMessageBox(str);
			m_pCmdInfo = 0;
		}
	}

	return CWinApp::ExitInstance();
}



// This will move information from the registry to an ini file
void CNeoMem::MoveRegistryToIniFile(CString sInifile) {

	// if registry key exists, read info from there, and set flag. 
	CRegKey r;
	CString sCompanyName = CString(Strings::szRegKey);
	CString sKey = _T("Software\\") + sCompanyName + _T("\\NeoMem");
	if (r.Open(HKEY_CURRENT_USER, sKey) == ERROR_SUCCESS) {
		TRACE("registry key found - moving to ini file\n");

		// Set registry key
		// this tells mfc to use the registry for settings
		TRACE("set registry key\n");
		SetRegistryKey(sCompanyName);
		r.Close();

		// now read info from the registry

		// Load standard INI file options (including MRU)
		LoadStdProfileSettings(8);

		// Load options from registry or ini file
		TRACE("load options\n");
		LoadOptions();

		// tell mfc to use ini file
		UseIniFile(sInifile);

		// Now save options
		TRACE("saving info to ini file...\n");
		SaveOptions();

		// now delete the registry info
		TRACE("deleting registry info...\n");
		CRegKey r;
		if (r.Open(HKEY_CURRENT_USER, "Software") == ERROR_SUCCESS) {
			r.RecurseDeleteKey(sCompanyName);
			r.Close();
		}
	}
	else {
		TRACE("registry key NOT found - will just use ini file\n");
	}
}


// Tell MFC to use the specified ini file, instead of the registry,
// to store profile information. 
void CNeoMem::UseIniFile(CString sInifile) {
	// clear this string - this is what mfc uses to see which it should use (reg/ini)
	free((void*)m_pszRegistryKey);
	m_pszRegistryKey = NULL;

	// specify ini file name
	free((void*)m_pszProfileName);
	m_pszProfileName = _tcsdup((LPCTSTR)sInifile);
}




// Load settings from registry, or set defaults
void CNeoMem::LoadOptions() {
  
	// Used to get various binary settings
	BYTE* pData = 0;
	UINT nBytes = 0;

	// Get measurement system from registry
	// 0 is metric, 1 is english
	TCHAR buf[2];
	buf[0] = NULL;
	GetLocaleInfo(GetUserDefaultLCID(), LOCALE_IMEASURE, buf, 2);
	int nDefUnits = buf[0] == '1' ? 0 : 1;
//	SetUnits(GetProfileInt(szSection, szUnits, nDefUnits));
	m_nUnits = nDefUnits;

	// Load page margins
	// set default: (twips)
	int nDefaultMargin = 1440 * 3 / 4; // .75 inch
	m_rectPageMargins.left = GetProfileInt(Strings::szSettings, Strings::szMarginLeft, nDefaultMargin);
	m_rectPageMargins.right = GetProfileInt(Strings::szSettings, Strings::szMarginRight, nDefaultMargin);
	m_rectPageMargins.top = GetProfileInt(Strings::szSettings, Strings::szMarginTop, nDefaultMargin);
	m_rectPageMargins.bottom = GetProfileInt(Strings::szSettings, Strings::szMarginBottom, nDefaultMargin);

	m_bPrintHeaderFooter = GetProfileInt(Strings::szSettings, Strings::szPrintHeaderFooter, TRUE);

	// Header View
	m_bDisplayHeader = GetProfileInt(Strings::szSettings, Strings::szDisplayHeader, TRUE);
	// get font for header view
	if (GetProfileBinary(Strings::szSettings, Strings::szHeaderFont, &pData, &nBytes) && pData) {
		m_fontHeader.CreateFontIndirect((LOGFONT*) pData);
		delete pData;
	}
	else {
		// Create default of 16pt Arial
		// bug: pt size is in 10ths of a point - MFC's param name is misleading
		m_fontHeader.CreatePointFont(nHeaderFontSizeDefault, szHeaderFontNameDefault); // eg 160 and "Arial"
	}
	// Get header colors
	m_clrHeaderBackground = GetProfileInt(Strings::szSettings, Strings::szHeaderBackgroundColor, clrHeaderBackgroundDefault);
	m_clrHeaderForeground = GetProfileInt(Strings::szSettings, Strings::szHeaderForegroundColor, clrHeaderForegroundDefault);

	// Get custom colors used in the various color dialogs
	if (GetProfileBinary(Strings::szSettings, Strings::szCustomColors, &pData, &nBytes) && pData) {
		// Copy data into array - cool!
		memcpy(&m_clrCustomColors[0], pData, sizeof(COLORREF) * 16);
		// bug: forgot to delete pData - 64 byte memory leak
		// Make sure that you check for memory leaks everytime you add new functionality,
		// otherwise you'll never know what's causing it without tracking it down the hard way.
		delete pData;
	}
	else {
		// Set up default custom colors
		// 0x00bbggrr
		m_clrCustomColors[0] = 0x00ffb080;
		m_clrCustomColors[1] = 0x00d7b3c1;
		m_clrCustomColors[2] = 0x0091d299;
		m_clrCustomColors[3] = 0x00d1eb83;
		m_clrCustomColors[4] = 0x00a5b593;
		m_clrCustomColors[5] = 0x00c5d8ed;
		m_clrCustomColors[6] = 0x00f4f8c9;
		m_clrCustomColors[7] = 0x00f0d9b7;

		m_clrCustomColors[8] = 0x00d5b3f0;
		m_clrCustomColors[9] = 0x00a36386;
		m_clrCustomColors[10] = 0x00b4bf91;
		m_clrCustomColors[11] = 0x00ffd7ff;
		m_clrCustomColors[12] = 0x00e4f9d0;
		m_clrCustomColors[13] = 0x00ddc6c7;
		m_clrCustomColors[14] = 0x00c100c1;
		m_clrCustomColors[15] = 0x0028d7c1; // turquoise
	}

	// Gridlines
	m_bDisplayGridlines = GetProfileInt(Strings::szSettings, Strings::szDisplayGridlines, TRUE);

	// Get default font for Text View
	if (GetProfileBinary(Strings::szSettings, Strings::szRtfDefaultFont, &pData, &nBytes) && pData) {
		m_fontRtfDefault.CreateFontIndirect((LOGFONT*) pData);
		delete pData;
	}
	else {
		// Create default of 8pt Arial
		m_fontRtfDefault.CreatePointFont(80, _T("Arial"));
	}

	// Tip of the Day
//,	m_bShowTips = GetProfileInt(szSettings, szShowTips, TRUE);
	m_nTip = GetProfileInt(Strings::szSettings, Strings::szLastTip, 0);

	// Name format
	m_nNameFormat = GetProfileInt(Strings::szSettings, Strings::szNameFormat, BDataPersonName::nfFirstLast);

//	m_nDateFormat = GetProfileInt(szSettings, szDateFormat, 0);

	// Admin mode
	// 1.1d should have commented this out in 1.1c (switched to cmd line)
//	m_bAdmin = GetProfileInt(szSettings, szAdmin, FALSE);

	// Get document folder
	m_strDocumentFolder = GetProfileString(Strings::szSettings, Strings::szDocumentFolder, "");
	if (m_strDocumentFolder.IsEmpty()) {
		// Note: SHGetFolderPath requires certain DLL's to be installed, so screw it. Just get from registry:
		// HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders
		// Note: Can't use GetProfileString because you can't really use it except with mycompany/myapp key.
		CRegKey r;
		r.Open(HKEY_CURRENT_USER, Strings::szShellFolders);
		DWORD dwCount = MAX_PATH;
		LPTSTR szPath = m_strDocumentFolder.GetBuffer(dwCount);
		r.QueryStringValue(szPath, _T("Personal"), &dwCount); // was QueryValue
		r.Close();
		m_strDocumentFolder.ReleaseBuffer();
		// To find shell folders such as My Documents, use SHGetFolderPath to get the path. 
		// SHGetFolderPath can work everywhere SHFOLDER.dll is installed.
		// You can use it on Windows 95, Windows 98, or any Windows platform, and it will 
		// get you to the right place.
		// Conflicting info from help!!: 
		// Important: SHGetFolderPath is new to the Windows 2000 API. If you call 
		// SHGetFolderPath from an application that can be installed on a previous version of 
		// Windows, then you will need to redistribute the file SHFolder.dll with your application.
//		HMODULE hModSHFolder = LoadLibrary("shfolder.dll");
//		if (hModSHFolder) {
//			TCHAR szPath[MAX_PATH];
//			(*(FARPROC*)&pfnSHGetFolderPath = GetProcAddress(hModSHFolder, "SHGetFolderPathA"));
//			pfnSHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, NULL, szPath);
//			m_strDocumentFolder = szPath;
//		}
	}
	// By setting the current directory here, any File Open or Save dialog will use that as the base folder
	::SetCurrentDirectory(m_strDocumentFolder);

	// Welcome dialog
	m_bShowWelcome = GetProfileInt(Strings::szSettings, Strings::szShowWelcome, TRUE);

	// Startup file
	m_bAutoLoad = GetProfileInt(Strings::szSettings, Strings::szAutoLoad, TRUE); // 1.2a made default true
	m_strAutoLoadFileName = GetProfileString(Strings::szSettings, Strings::szAutoLoadFileName, "");

	//, Add Object Dialog options
//	m_bAddAsSibling = GetProfileInt(szSettings, szAddAsSibling, FALSE);
//	m_bSelectNewObject = GetProfileInt(szSettings, szSelectNewObject, TRUE);

	// Home View colors
	// difference for 2 lines (hex) 08 20 28
	// tan: 0x0090a8b8   0x0098c8e0
	// green: #CCFF66 #66CC33
	// 0x00bbggrr
	m_clrHomeTopBack = 0x0098c0c8; // tan
	m_clrHomeTopBack2 = 0x0080a8b0;
	m_clrHomeTopFore = 0x00ffffff;

	m_clrHomeMiddleBack = 0x00ffffff;
	m_clrHomeMiddleFore = 0x0;
	m_clrHomeMiddleFore2 = 0x00808080;

	m_clrHomeBottomBack = 0x007888a0;
	m_clrHomeBottomBack2 = 0x00a0a8b8;
	m_clrHomeBottomFore = 0x00ffffff;

	m_clrHomeDescriptionFore = 0x0020a020; // green
	

	// Template file
	CString strTemplateFile;
	strTemplateFile.LoadString(IDS_TEMPLATE_FILE); // "Template.neo" by default
	m_strTemplateFileName = strTemplateFile;
	m_strTemplatePath = m_strApplicationFolder + _T("\\") + m_strTemplateFileName;

	// AutoStart
	m_bAutoStart = GetProfileInt(Strings::szSettings, Strings::szAutoStart, FALSE);

	// AutoRecover
	m_bAutoRecover = GetProfileInt(Strings::szSettings, Strings::szAutoRecover, TRUE);
	m_nAutoRecoverMinutes = GetProfileInt(Strings::szSettings, Strings::szAutoRecoverMinutes, 5);

	// AutoBackup
	m_bAutoBackup = GetProfileInt(Strings::szSettings, Strings::szAutoBackup, TRUE);

	// Text View
	m_clrTextViewForecolor = GetProfileInt(Strings::szSettings, Strings::szTextViewForecolor, Library::clrWindowText);
	m_clrTextViewBackcolor = GetProfileInt(Strings::szSettings, Strings::szTextViewBackcolor, Library::clrWindow);

	// Export folder and format
	m_strExportFolder = GetProfileString(Strings::szSettings, Strings::szExportFolder, m_strDocumentFolder);
	m_strExportFormat = GetProfileString(Strings::szSettings, Strings::szExportFormat, "csv");

	// Register .neo file type?
	m_bRegisterFileType = GetProfileInt(Strings::szSettings, Strings::szRegisterFileType, TRUE);
	
	// Set flag so ExitInstance knows it can call SaveOptions
	m_bOptionsLoaded = TRUE;
}



// Save current settings in registry
void CNeoMem::SaveOptions() {

	// Margins
	WriteProfileInt(Strings::szSettings, Strings::szMarginLeft, m_rectPageMargins.left);
	WriteProfileInt(Strings::szSettings, Strings::szMarginRight, m_rectPageMargins.right);
	WriteProfileInt(Strings::szSettings, Strings::szMarginTop, m_rectPageMargins.top);
	WriteProfileInt(Strings::szSettings, Strings::szMarginBottom, m_rectPageMargins.bottom);

	WriteProfileInt(Strings::szSettings, Strings::szPrintHeaderFooter, m_bPrintHeaderFooter);

	// Header View
	WriteProfileInt(Strings::szSettings, Strings::szDisplayHeader, m_bDisplayHeader);
	LOGFONT lf;
	if (m_fontHeader.GetLogFont(&lf))
		WriteProfileBinary(Strings::szSettings, Strings::szHeaderFont, (LPBYTE) &lf, sizeof(LOGFONT));
	WriteProfileInt(Strings::szSettings, Strings::szHeaderBackgroundColor, m_clrHeaderBackground);
	WriteProfileInt(Strings::szSettings, Strings::szHeaderForegroundColor, m_clrHeaderForeground);

	// Save custom colors
	WriteProfileBinary(Strings::szSettings, Strings::szCustomColors, (LPBYTE) &m_clrCustomColors[0], sizeof(COLORREF) * 16);

	// Gridlines
	WriteProfileInt(Strings::szSettings, Strings::szDisplayGridlines, m_bDisplayGridlines);

	// Save default rtf font
	if (m_fontRtfDefault.GetLogFont(&lf))
		WriteProfileBinary(Strings::szSettings, Strings::szRtfDefaultFont, (LPBYTE) &lf, sizeof(LOGFONT));

	// Tips
//	WriteProfileInt(Strings::szSettings, Strings::szShowTips, m_bShowTips);
	WriteProfileInt(Strings::szSettings, Strings::szLastTip, m_nTip);

	// Name and date formats
	WriteProfileInt(Strings::szSettings, Strings::szNameFormat, m_nNameFormat);
//	WriteProfileInt(Strings::szSettings, Strings::szDateFormat, m_nDateFormat);

	// Document folder
	WriteProfileString(Strings::szSettings, Strings::szDocumentFolder, m_strDocumentFolder);

	// Welcome
	WriteProfileInt(Strings::szSettings, Strings::szShowWelcome, m_bShowWelcome);

	// Add Object Dialog options
//	WriteProfileInt(Strings::szSettings, Strings::szAddAsSibling, m_bAddAsSibling);
//	WriteProfileInt(Strings::szSettings, Strings::szSelectNewObject, m_bSelectNewObject);

	//, write Home view colors

	// Template file
	//, not really using this now
	WriteProfileString(Strings::szSettings, Strings::szTemplateFileName, m_strTemplateFileName);

	// AutoLoad
	WriteProfileInt(Strings::szSettings, Strings::szAutoLoad, m_bAutoLoad);
	WriteProfileString(Strings::szSettings, Strings::szAutoLoadFileName, m_strAutoLoadFileName);

	// AutoStart
	WriteProfileInt(Strings::szSettings, Strings::szAutoStart, m_bAutoStart);

	// AutoRecover
	WriteProfileInt(Strings::szSettings, Strings::szAutoRecover, m_bAutoRecover);
	WriteProfileInt(Strings::szSettings, Strings::szAutoRecoverMinutes, m_nAutoRecoverMinutes);

	// AutoBackup
	WriteProfileInt(Strings::szSettings, Strings::szAutoBackup, m_bAutoBackup);

	// Text View
	WriteProfileInt(Strings::szSettings, Strings::szTextViewForecolor, m_clrTextViewForecolor);
	WriteProfileInt(Strings::szSettings, Strings::szTextViewBackcolor, m_clrTextViewBackcolor);

	// Export folder and format
	WriteProfileString(Strings::szSettings, Strings::szExportFolder, m_strExportFolder);
	WriteProfileString(Strings::szSettings, Strings::szExportFormat, m_strExportFormat);

}




// Initialize the application image list
void CNeoMem::LoadImageList() {
	m_imlApp.Create(16, 16, ILC_MASK, 10, 10);
	m_nIconArrowUp = m_imlApp.Add(LoadIcon(IDI_ARROW_UP));
	m_nIconArrowDown = m_imlApp.Add(LoadIcon(IDI_ARROW_DOWN));
//	m_nIconDocument = m_imlApp.Add(LoadIcon(IDR_NEOMEM_TYPE));
}



// Message Handlers
//---------------------------------------------------------------------------

// App command to run the dialog
void CNeoMem::OnCmdHelpAbout() {
	CWaitCursor wc;
	CDialogAbout dlg;
	dlg.DoModal();
}



// Bring up options dialog and let user change settings
void CNeoMem::OnCmdViewOptions() {

	// Before calling options, save any current data, if any
	BDoc* pDoc = BDoc::GetDoc();
	if (pDoc) {
		ASSERT_VALID(pDoc);
		pDoc->UpdateAllViewsEx(NULL, hintSave, pDoc->GetCurrentObject());
	}

	// Note: can turn off help button if you want
	CSheetOptions sh(_T("Options"), AfxGetMainWnd());
	CPageOptionsDisplay pgDisplay;
	CPageOptionsFile pgFile;

	sh.m_psh.dwFlags |= PSH_NOAPPLYNOW; // turn off apply button
	sh.AddPage(&pgDisplay);
	sh.AddPage(&pgFile);

	m_fontRtfDefault.GetLogFont(&pgDisplay.m_lf);
//	pgDisplay.m_bShowTips = m_bShowTips;
	pgDisplay.m_nNameFormat = m_nNameFormat - 1; // Note: Must adjust by 1 to match combo items
//	pgDisplay.m_nDateFormat = m_nDateFormat; // 0 or 1 = short or long

	pgFile.m_strDocumentFolder = m_strDocumentFolder;
	pgFile.m_bAutoLoad = m_bAutoLoad;
//	pgFile.m_strAutoLoadFileName = m_strAutoLoadFileName;
	pgFile.m_bAutoStart = m_bAutoStart;
	pgFile.m_bAutoBackup = m_bAutoBackup;
	pgFile.m_bAutoRecover = m_bAutoRecover;
	pgFile.m_nAutoRecoverMinutes = m_nAutoRecoverMinutes;

	pgFile.m_strCurrentFile = "";
	if (pDoc) {
		pgFile.m_strCurrentFile = pDoc->GetPathName();
	}

	if (sh.DoModal() == IDOK) {
		// Apply changes

		// Update default rtf font
		// Tell rtf view to update default font
//		CFrameMain* pMain = (CFrameMain*) this->m_pMainWnd;
//		ASSERT_VALID(pMain);
//		CFrameChild* pChild = (CFrameChild*) pMain->GetActiveFrame();
//		ASSERT_VALID(pChild);
//		CViewRtf* pView = (CViewRtf*) pChild->GetView(viewText);
//		ASSERT_VALID(pView);
//		pView->SetDefaultFont(m_fontRtfDefault);
		if (pgDisplay.m_bFontChanged) {
			m_fontRtfDefault.DeleteObject();
			m_fontRtfDefault.CreateFontIndirect(&pgDisplay.m_lf);
			// Tell all open document views to get the new font
			UpdateAllDocumentViews(hintDefaultFontChanged);
		}

		// Tip of the day
//		m_bShowTips = pgDisplay.m_bShowTips;

		// Name Format
		// If this changed, need to refresh any name objects being displayed
		UINT nNewNameFormat = (UINT) pgDisplay.m_nNameFormat + 1; // Note: Must adjust by 1 to match combo items
		if (m_nNameFormat != nNewNameFormat) {
			m_nNameFormat = nNewNameFormat;
			// Now need to reset all name bdata object flags so that they will know to recalculate themselves.
			UpdateAllDocumentObjects(msgResetData, TRUE);
			UpdateAllDocumentViews(hintResortAllChildren);
		}

		// Date Format
		// If this changed, need to refresh any date values being displayed
//		if (m_nDateFormat != (UINT) pgDisplay.m_nDateFormat)
//		{
//			m_nDateFormat = pgDisplay.m_nDateFormat;
//		}

		// Document Folder
		// If this changed, save it and set the current directory to it
		if (pgFile.m_strDocumentFolder != m_strDocumentFolder) {
			m_strDocumentFolder = pgFile.m_strDocumentFolder;
			// By setting the current directory here, any File Open or Save dialog will use that as the base folder
			::SetCurrentDirectory(m_strDocumentFolder);
		}

		// AutoLoad
//		if (pgFile.m_strAutoLoadFileName != m_strAutoLoadFileName) {
//			m_strAutoLoadFileName = pgFile.m_strAutoLoadFileName;
//		}
		m_bAutoLoad = pgFile.m_bAutoLoad;

		// AutoStart
		if (pgFile.m_bAutoStart != m_bAutoStart) {
			m_bAutoStart = pgFile.m_bAutoStart;

			// Since AutoStart was turned on or off, we need to add or delete a value in the Run registry key.
			// The value's name is "NeoMem" and the string is "D:\Applications\NeoMem\NeoMem.exe" or whatever.
			TCHAR szExeName[MAX_PATH];
			if (::GetModuleFileName(NULL, szExeName, MAX_PATH)) {
				CRegKey r;
				if (ERROR_SUCCESS == r.Open(HKEY_CURRENT_USER, Strings::szWindowsRun)) {
					if (m_bAutoStart)
						r.SetStringValue(szExeName, m_pszAppName); // was SetValue
					else
						r.DeleteValue(m_pszAppName);
				}
			}
		}

		// AutoRecover
		// 1.2a bug 3966: wasn't turning off autorecover timer
		if ((m_bAutoRecover != pgFile.m_bAutoRecover) || (m_nAutoRecoverMinutes != pgFile.m_nAutoRecoverMinutes)) {
			m_bAutoRecover = pgFile.m_bAutoRecover;
			m_nAutoRecoverMinutes = pgFile.m_nAutoRecoverMinutes;
			if (m_bAutoRecover) {
				GetMainFrame()->SetAutoRecoverTimer(m_nAutoRecoverMinutes);
			}
			else {
				GetMainFrame()->SetAutoRecoverTimer(0); // turn it off
			}
		}

		// AutoBackup
		m_bAutoBackup = pgFile.m_bAutoBackup;

		// Refresh all views, to catch any changes to name format, date format, etc.
		UpdateAllDocumentViews(hintRefresh);

	}
}





// Routines
//-------------------------------------------------------------------


void CNeoMem::OnFilePrintSetup() {
	CWinApp::OnFilePrintSetup();
}



// page setup support functions from wordapp.cpp MFC
//, move these?
inline int roundleast(int n) {
	int mod = n%10;
	n -= mod;
	if (mod >= 5)
		n += 10;
	else if (mod <= -5)
		n -= 10;
	return n;
}

static void RoundRect(LPRECT r1) {
	r1->left = roundleast(r1->left);
	r1->right = roundleast(r1->right);
	r1->top = roundleast(r1->top);
	r1->bottom = roundleast(r1->bottom);
}

static void MulDivRect(LPRECT r1, LPRECT r2, int num, int div) {
	r1->left = ::MulDiv(r2->left, num, div);
	r1->top = ::MulDiv(r2->top, num, div);
	r1->right = ::MulDiv(r2->right, num, div);
	r1->bottom = ::MulDiv(r2->bottom, num, div);
}



// code taken from wordapp.cpp MFC
// changed so that margins are set for the app, not for the document
void CNeoMem::OnFilePageSetup() {
	CPageSetupDialog dlg;
	PAGESETUPDLG& psd = dlg.m_psd;

//	BOOL bMetric = app.GetUnits() == 1; //centimeters
//	BOOL bMetric = (app.m_nUnits == 1);
	BOOL bMetric = (m_nUnits == 1);
	psd.Flags |= PSD_MARGINS | 
						(bMetric ? PSD_INHUNDREDTHSOFMILLIMETERS : PSD_INTHOUSANDTHSOFINCHES);
	int nUnitsPerInch = bMetric ? 2540 : 1000;
	MulDivRect(&psd.rtMargin, m_rectPageMargins, nUnitsPerInch, 1440);
	RoundRect(&psd.rtMargin);

	// Get the current device from the app
//	PRINTDLG pd;
//	pd.hDevNames = NULL;
//	pd.hDevMode = NULL;
//	GetPrinterDeviceDefaults(&pd);
//	psd.hDevNames = pd.hDevNames;
//	psd.hDevMode = pd.hDevMode;

	// Use default printer
	psd.hDevNames = NULL;
	psd.hDevMode = NULL;

	if (dlg.DoModal() == IDOK) {
		RoundRect(&psd.rtMargin);
		MulDivRect(m_rectPageMargins, &psd.rtMargin, 1440, nUnitsPerInch);

		m_sizePaper.cx = psd.ptPaperSize.x * 1440 / nUnitsPerInch;
		m_sizePaper.cy = psd.ptPaperSize.y * 1440 / nUnitsPerInch;

		SelectPrinter(psd.hDevNames, psd.hDevMode);
//		NotifyPrinterChanged();
	}
	
}


// Return a pointer to the application image list
CImageList* CNeoMem::GetAppImageList() {
	return &m_imlApp;
}




// Toggle display of header view
void CNeoMem::OnCmdViewHeader() {
	m_bDisplayHeader = !m_bDisplayHeader;
	// Refresh display
	// mainly just need to resize the header view with repaint TRUE
	BDoc::GetDoc()->UpdateAllViewsEx(NULL, hintResize);
}

void CNeoMem::OnUpdateViewHeader(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_bDisplayHeader ? 1 : 0);	
}


// Let user change font and foreground color for Header View
void CNeoMem::OnCmdViewHeaderFont() {
	LOGFONT lf;
	if (!m_fontHeader.GetLogFont(&lf))
		return;
	CFontDialog dlg(&lf); //, parent wnd?
	dlg.m_cf.rgbColors = m_clrHeaderForeground;
	if (dlg.DoModal() == IDOK) {
		m_clrHeaderForeground = dlg.m_cf.rgbColors;
		if (m_fontHeader.DeleteObject()) {
			m_fontHeader.CreateFontIndirect(&lf); //, check return value
			// this is supposed to redraw the window but it doesn't work
			// i think the child windows are excused because they are immune from being redrawn.
//			m_pMainWnd->Invalidate();
//			m_pMainWnd->UpdateWindow();
			// Refresh display
//			CHint h;
//			h.m_pobjObject = BDoc::GetDoc()->GetCurrentObject();
//			h.m_lngPropertyID = propName;
//			BDoc::GetDoc()->UpdateAllViewsEx(NULL, hintPropertyChange, &h);
			UpdateAllDocumentViews(hintRefresh);
		}
	}
}


// lpCustColors 
// Pointer to an array of 16 COLORREF values that contain red, green, blue (RGB) values for 
// the custom color boxes in the dialog box. If the user modifies these colors, the system 
// updates the array with the new RGB values. To preserve new custom colors between calls 
// to the ChooseColor function, you should allocate static memory for the array. 
void CNeoMem::OnCmdViewHeaderColor() {
	CColorDialog dlg(m_clrHeaderBackground); //, parent wnd?
	dlg.m_cc.lpCustColors = &m_clrCustomColors[0];
	if (dlg.DoModal() == IDOK) {
		m_clrHeaderBackground = dlg.GetColor();
//		m_pMainWnd->Invalidate();
		UpdateAllDocumentViews(hintRefresh);
	}
}




// Reset header to defaults
void CNeoMem::OnCmdViewHeaderReset() {
	m_clrHeaderBackground = clrHeaderBackgroundDefault;
	m_clrHeaderForeground = clrHeaderForegroundDefault;
	if (m_fontHeader.DeleteObject()) {
		m_fontHeader.CreatePointFont(nHeaderFontSizeDefault, szHeaderFontNameDefault);
	}

	UpdateAllDocumentViews(hintRefresh);
}




// Walk through all open documents and send hintSave.
// This is called before exiting.
// Virtual override.
BOOL CNeoMem::SaveAllModified() {
	// Get doc template
	POSITION pos = GetFirstDocTemplatePosition();
	CDocTemplate* pDocTemplate = GetNextDocTemplate(pos);
	if (pDocTemplate) {
		// Walk through open documents
		pos = pDocTemplate->GetFirstDocPosition();
		while (pos) {
			// Send hintSave to document - will set document modified flag if any
			// changes are saved to the document.
			BDoc* pDoc = DYNAMIC_DOWNCAST(BDoc, pDocTemplate->GetNextDoc(pos));
			ASSERT_VALID(pDoc);
			BObject* pobjCurrent = pDoc->GetCurrentObject();
			ASSERT_VALID(pobjCurrent);
			pDoc->UpdateAllViewsEx(NULL, hintSave, pobjCurrent);
		}
	}

	// The default implementation of this member function calls the CDocument::SaveModified 
	// member function in turn for all modified documents within the application.
	return CWinApp::SaveAllModified();
}







BOOL CNeoMem::OnDDECommand(LPTSTR lpszCommand) {
	//trace("CNeoMem::OnDDECommand(%s)\n", lpszCommand);

//	CString str;
//	str.Format("OnDDECommand: %s", lpszCommand);
//	AfxMessageBox(str);

	// Call base class - handles open command. Returns True if command was handled.
	if (CWinApp::OnDDECommand(lpszCommand))
		return TRUE;

	// Handle any DDE commands recognized by your application
	// and return TRUE.  See implementation of CWinApp::OnDDEComand
	// for example of parsing the DDE command string.

	// Return FALSE for any DDE commands you do not handle.
	return FALSE;
}





// Help Functions
//----------------------------------------------------------------------------------------------------


// Bring up the neomem website
void CNeoMem::OnCmdHelpWebsite() {
	GotoWebsite();
}


void CNeoMem::OnCmdHelpEmail() {
//	SendEmail();
	CEmail email;
	email.AskSend();
}



// Show the tip of the day dialog
void CNeoMem::OnCmdHelpTipOfTheDay() {
//	CDialogTip dlg;
//	dlg.m_bShowTips = m_bShowTips;
//	dlg.m_nTip = m_nTip;
//	dlg.DoModal();	
	// Save settings in case user changed them
//	m_bShowTips = dlg.m_bShowTips; 
//	m_nTip = dlg.m_nTip;
}





// Set text of status bar
// Pass 0 or () to show the default string ("Ready")
void CNeoMem::SetStatusBarText(LPCTSTR lpszText /* = 0 */) {
	ASSERT_VALID(m_pMainWnd);
	CString str;
	if (lpszText == 0) {
		str.LoadString(AFX_IDS_IDLEMESSAGE); // "Ready"
		lpszText = (LPCTSTR) str;
	}
	GetMainFrame()->GetStatusBar().SetWindowText(lpszText);
	GetMainFrame()->GetStatusBar().Resize();
}


// Get a reference to the progress bar control
CProgressCtrl& CNeoMem::GetProgressBar() {
	CProgressCtrl& rprog = GetMainFrame()->GetStatusBar().GetProgressBar();
	return rprog;
}


// Override this to register short .exe name instead of long name
// See Q148806 in Knowledge Base (MFC bug)
void CNeoMem::EnableShellOpen() {
	ASSERT(m_atomApp == NULL && m_atomSystemTopic == NULL); // do once

	// Get .exe's short name
	CString strShortName;
	TCHAR szLongPathName[_MAX_PATH];
	::GetModuleFileName(m_hInstance, szLongPathName, _MAX_PATH);
	if (::GetShortPathName(szLongPathName, strShortName.GetBuffer(_MAX_PATH), _MAX_PATH) == 0) {
		// Rare failure case (especially on not-so-modern file systems)
		strShortName = szLongPathName;
	}
	strShortName.ReleaseBuffer();
	int nPos = strShortName.ReverseFind('\\');
	if (nPos != -1)
		strShortName = strShortName.Right(strShortName.GetLength()- nPos-1);
	nPos = strShortName.ReverseFind('.');
	if (nPos != -1)
		strShortName = strShortName.Left(nPos);
	m_atomApp = ::GlobalAddAtom(strShortName);
	m_atomSystemTopic = ::GlobalAddAtom(_T("system"));
} 





/* // 1.1
void CNeoMem::OnCmdEnterKey() {
	EnterRegistrationKey();
}


// Compare specified key for the given name with the calculated key.
// Returns True if they match.
BOOL CNeoMem::VerifyRegistrationKey(CString& strName, CString& strKey) {
	char* pszName = strName.GetBuffer(0);

	CString strCalcKey;
	const int nKeyChars = 100;
	char* pszCalcKey = strCalcKey.GetBuffer(nKeyChars);

	const int nProgramID = 1; // NeoMem

	// Get registration key for given name into calc key buffer
	GetRegistrationKey(pszName, pszCalcKey, nProgramID);

	strName.ReleaseBuffer();
	strCalcKey.ReleaseBuffer();

	// Compare calc key with specified key
	BOOL bValid = (strKey == strCalcKey);
	return bValid;
}

*/


CDocument* CNeoMem::OpenDocumentFile(LPCTSTR lpszFileName) {
	// Add your specialized code here and/or call the base class	
	return CWinApp::OpenDocumentFile(lpszFileName);
}



// Return ObjectID of top object, either Main root or User root
ULONG CNeoMem::GetTopObjectID() {
	return m_bAdmin ? rootMain : rootUser;
}



// Advance tip counter to next available tip
void CNeoMem::GotoNextTip() {
	if (m_nTip == 0)
		m_nTip = IDS_TIP1;
	else
		m_nTip++;

	if (m_nTip > IDS_TIP_LAST)
		m_nTip = IDS_TIP1;

	m_strTip.LoadString(m_nTip);

}


// Reverse tip counter to previous tip
void CNeoMem::GotoPreviousTip() {
	if (m_nTip == IDS_TIP1)
		m_nTip = IDS_TIP_LAST;
	else
		m_nTip--;

	m_strTip.LoadString(m_nTip);
}


// Get pointer to current tip text
LPCTSTR CNeoMem::GetTip() {
	return m_strTip;
}









// This is where we handle any uncaught exceptions that occur during processing 
// of a Windows message. [but doesn't include those outside of that situation]
// Should give message with the exception, saying to save your work and close the program.
// This gets called from AfxCallWndProc, which catches any uncaught exceptions 
// within the message loop
// TRY {
//      lResult = pWnd->WindowProc(nMsg, wParam, lParam);
//   } CATCH_ALL(e) {
//      lResult = AfxGetThread()->ProcessWndProcException(e);
//		TRACE1("Warning: Uncaught exception in WindowProc (returning %ld).\n", lResult);
//   } END_CATCH_ALL 
// [from wincore.cpp]
//. write exceptions to logfile
// Don't delete the exception here because AfxCallWndProc (which called this routine) 
// does that.
// If you rethrow the exception from this routine, control would pass to the 
// terminate handler, which would just shut down the program.
LRESULT CNeoMem::ProcessWndProcException(CException* pe, const MSG* pMsg) {
//	e->ReportError();
//	return CWinApp::ProcessWndProcException(pe, pMsg);

	CError* pError = NULL;

	// if it's an error object, it already has lots of useful information. 
	// if not, create one. 
	if (pe->IsKindOf( RUNTIME_CLASS( CError ))) {
		pError = (CError*) pe;
	}
	else {
		// get error message from the exception. could be a fileexception, 
		// or memoryexception, etc
		// "for example, call GetErrorMessage to retrieve a string describing the 
		// error which caused MFC to throw a CFileException when writing to a CFile object."
		TCHAR szError[512];
		pe->GetErrorMessage(szError, 512);
		CString strError = szError;

		// make an error object to hold all this information
		pError = new CError(FALSE);
		pError->SetName(strError);
	}

	pError->SetMsg(pMsg); // parses message info
//	eError->ReportError();

	// Display message in dialog box, if bad let user send email, etc.
	BOOL bContinue = HandleError(*pError);

	// If you want to have the debugger kick in whenever you get here, uncomment this section.
//	#ifdef _DEBUG
//		DebugBreak();
//	#endif

	// This code is taken from CWinApp::ProcessWndProcException, but leaves out the msgbox.
	// Handle certain messages in CWinThread.
	LRESULT lResult  = 0; // sensible default return for most WndProc functions
	switch (pMsg->message) {
		case WM_CREATE:
		case WM_PAINT:
			lResult = CWinThread::ProcessWndProcException(pe, pMsg);
	}
	if (pMsg->message == WM_COMMAND)
		lResult = (LRESULT) TRUE; // pretend the command was handled


	if (!bContinue) {
		// If catastrophic error (or if user says to), terminate here.
		// You can call AfxAbort in the rare case when you encounter a catastrophic error from 
		// which you cannot recover.
		// Normally, an application should do everything it possibly can to clean up properly 
		// and _not_ call AfxAbort().
//		AfxAbort(); // terminate application without any cleanup (best for assertion failure?)
//		THROW_LAST(); // will call terminate()
		// maybe set a flag to shut down?
		// send wm_close message?
		// same as double-clicking on main window close box
//		ASSERT(AfxGetApp()->m_pMainWnd != NULL);
//		AfxGetApp()->m_pMainWnd->SendMessage(WM_CLOSE);
// 1.1		m_bCrashing = TRUE; // set flag so that nag dialog won't show up
		m_pMainWnd->SendMessage(WM_CLOSE);
	}

	return lResult;
}





// Bring up a dialog or messagebox with error message. If error is severe,
// let user email it to me, and if it's really severe, close the program.
// Returns True if user chose to Continue (or if not a severe error), or
// False if user chose to Shut Down the program.
BOOL CNeoMem::HandleError(CError& e) {
	// CException has a member GetErrorMessage, which could be overridden

	// If the error level is Low, just show the error message in a dialog.
	if (e.m_bSevere == FALSE) {
		AfxMessageBox(e.GetReportText(), MB_ICONINFORMATION);
		return TRUE; // Always continue from a non-severe error
	}
	else {
		// Otherwise try to save all open files, bring up dialog showing the message, 
		// and let the user email it to me.

		// save existing file(s) to _ErrorBackup
		CString strFilesSaved;
		SaveAllModifiedBackup(strFilesSaved);
		e.m_strFilesSaved = strFilesSaved; // add this info to our error object also

		CDialogError dlg;
		dlg.m_pe = &e;
		BOOL bContinue = dlg.DoModal();
		return bContinue;
	}
}




// Walk through all document templates and all open documents, sending
// the specified hint and object to all views.
void CNeoMem::UpdateAllDocumentViews(LPARAM lHint, CObject* pHint /* = 0 */) {
	TRACE("UpdateAllDocumentViews %s\n", GetHintName(lHint));

	// Walk through document templates
	POSITION posDocTemplate = GetFirstDocTemplatePosition();
	while (posDocTemplate) {
		CDocTemplate* pDocTemplate = GetNextDocTemplate(posDocTemplate);

		// Walk through documents
		POSITION posDoc = pDocTemplate->GetFirstDocPosition();
		while (posDoc) {
			CDocument* pDoc = pDocTemplate->GetNextDoc(posDoc);
			// Send message
			pDoc->UpdateAllViews(NULL, lHint, pHint);
		}
	}
}



// Walk through all document templates and all open documents, sending
// the specified hint and object to all bobjects.
void CNeoMem::UpdateAllDocumentObjects(ULONG lngMsg, BOOL bRecurse /* = TRUE */) {

	// Walk through document templates
	POSITION posDocTemplate = GetFirstDocTemplatePosition();
	while (posDocTemplate) {
		CDocTemplate* pDocTemplate = GetNextDocTemplate(posDocTemplate);

		// Walk through documents
		POSITION posDoc = pDocTemplate->GetFirstDocPosition();
		while (posDoc) {
			BDoc* pDoc = DYNAMIC_DOWNCAST(BDoc, pDocTemplate->GetNextDoc(posDoc));
			ASSERT_VALID(pDoc);
			// Send message to all bobjects
			BObject* pobj = pDoc->GetRoot();
			ASSERT_VALID(pobj);
			pobj->SendMessage(lngMsg, bRecurse);
		}
	}
}




void CNeoMem::GotoWebsite() {
	if (IDYES == AfxMessageBox("This will attempt to open the NeoMem.org website. Continue?", MB_ICONQUESTION + MB_YESNO)) {
		CString strURL;
		strURL.Format("http://%s", (LPCTSTR) app.m_strWebsite); // www.neomem.org
		CWaitCursor wc;
		HINSTANCE h = ::ShellExecute(NULL, "open", strURL, NULL, NULL, SW_SHOWNORMAL);	
		Library::HandleShellExecuteError(h);
	}
}





// Search the specified RTF string for the specified plain text.
// We use an invisible rtf control to convert the rtf to plain text. 
// Returns position of occurrence withing the string, or -1 if not found.
long CNeoMem::SearchRtfString(LPCTSTR pszRtfString, LPCTSTR pszSearchFor, BOOL bMatchCase, BOOL bWholeWord) {

	// Assign rtf text to search through
	m_rtf.SetRtf(pszRtfString);

	// Select all text
	CHARRANGE cr;
	cr.cpMin = 0;
	cr.cpMax = -1;

	// Specify search term and properties
	FINDTEXTEX ft;
	ft.chrg = cr;
	ft.lpstrText = pszSearchFor; // both are const char*
	DWORD dwSearchFlags = (bMatchCase ? FR_MATCHCASE : 0) | (bWholeWord ? FR_WHOLEWORD : 0);
	dwSearchFlags |= FR_DOWN; // This is needed for RichEdit 2.0 and later

	// Search for text
	// Bug:: redefined int nPos here - BAD!
	// Bug:: cstring and rtf returned int and long respectively, and i was treating them the same!
	long nPos = m_rtf.FindText(dwSearchFlags, &ft);
//	long nPos = m_rtf.FindText2(dwSearchFlags, &ft);

	return nPos;
}



// Update common control library, if necessary, and delete update file if not needed.
// Returns False if should abort application.
BOOL CNeoMem::UpdateCommonControls() {
	TRACE("UpdateCommonControls\n");

	float fVersionRequired = 5.80f;
	float fVersionExisting = m_objInfo.GetCommonControlsVersionNumber();

//	if (TRUE)
	if (fVersionExisting < fVersionRequired) {
		CString str;
		str.Format(
			"In order to run NeoMem, your Windows Common Control Library needs to be updated to a newer version."
			"\r\n\r\n"
			"A free update is available from Microsoft's website - click on the button below to go to Microsoft's download site. "
			"On the site, click on the link labelled \"Download 50comupd.exe (x86)\" and save the file to your hard drive. "
			"After the file downloads, double click on it to run the program and install the new version."
			"\r\n\r\n"
			"More Information: The Common Control Library resides in the Windows system file Comctl32.dll - NeoMem requires version %.2f or later, and your version is %.2f."
			"\r\n\r\n"
			,
			fVersionRequired,
			fVersionExisting
			);
		CDialogNotice dlg;
		dlg.m_strMessage = str;
		dlg.DoModal();

		return FALSE;
	}
	return TRUE;
}



// Load strings from string resource
void CNeoMem::LoadStrings() {
	TRACE("LoadStrings\n");

	m_strWebsite.LoadString(IDS_WEBSITE);
	m_strEmail.LoadString(IDS_EMAIL);
}



// Bring up a help topic
// Must add keyword to hlp/HelpIDs.h file
void CNeoMem::OnCmdHelpKeyboard() {
	// Show the custom help topic that has the context string "HID_KEYBOARD_SHORTCUTS"
	// in the help .RTF file, and which is mapped to the DWORD ID HID_KEYBOARD_SHORTCUTS
	// in the HelpIDs.h file, which is included in the [Map] section of the Neomem.hm file.

	// uCommand = HELP_SETWINPOS 
	// Displays the Help window, if it is minimized or in memory, and sets its size and position as specified. 
	// dwData =  Address of a HELPWININFO structure that specifies the size and position of either 
	// a primary or secondary Help window. 

	app.HtmlHelpA(HID_KEYBOARD_SHORTCUTS);
}



// Copy specified text to the clipboard.
// Returns True if successful.
BOOL CNeoMem::CopyToClipboard(CString &str) {
	TRACE("CopyToClipboard\n");
  
//	if (OpenClipboard())
	if (::OpenClipboard(NULL)) {
		::EmptyClipboard();

		// Copy the CString to global memory for the clipboard to use
		// Bug: Didn't check for 0 length, so sf.Detach failed with an assert
		//, make a cstringex savetoglobal method
		UINT nBytes = str.GetLength();
		if (nBytes != 0) {
			void* pData = (void*) (LPCTSTR) str;
			CSharedFile sf(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);
			sf.Write(pData, nBytes);
			HGLOBAL hGlobal = sf.Detach();

			::SetClipboardData(CF_TEXT, hGlobal);
			::CloseClipboard();
			return TRUE;
		}
	}
	return FALSE;
}




// Get text from clipboard (if any) and put it into the given cstring.
// Returns True if successful, or False if no text on clipboard.
BOOL CNeoMem::GetTextFromClipboard(CString &str) {
	BOOL bSuccess = FALSE;
	if (::IsClipboardFormatAvailable(CF_TEXT)) {
		if (::OpenClipboard(NULL)) {
			HGLOBAL hGlobal = ::GetClipboardData(CF_TEXT);
			if (hGlobal) {
				LPCTSTR psz = (LPCTSTR) ::GlobalLock(hGlobal);
				if (psz) {
					str = psz; // Copy global memory to cstring
					::GlobalUnlock(hGlobal);
					bSuccess = TRUE;
				} 
			}
			::CloseClipboard();
		}
	}
	return bSuccess;
	// This code didn't always work because the rtf control put text into a stream and so GetGlobalData
	// would return 0. Dealing with streams looked nasty, so just used the winapi functions.
/*
	COleDataObject odo;
	odo.AttachClipboard();
	if (odo.IsDataAvailable(CF_TEXT)) {
		// Get global memory block
		HGLOBAL hGlobal = odo.GetGlobalData(CF_TEXT);
		if (hGlobal) {
			// Get text from global memory
			LPCSTR pData = (LPCSTR) ::GlobalLock(hGlobal);
			if (pData) {
				str = pData;
				::GlobalUnlock(hGlobal);
				return TRUE;
			}
		}
//		else {
//			STGMEDIUM stgMedium;
//			if (odo.GetData(CF_TEXT, &stgMedium)) {
//				switch (stgMedium.tymed) {
//					case TYMED_ISTREAM:
//						stgMedium.pstg 
//				}
//			}
//		}
	}
	return FALSE;
*/
}



void CNeoMem::OnCmdHelpShowWelcome() {
	CDialogWelcome dlg;
	dlg.DoModal();
}


/*
void CNeoMem::ShowTipAtStartup(void) {
	// CG: This function added by 'Tip of the Day' component.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if (cmdInfo.m_bShowSplash) {
		CTipDlg dlg;
		if (dlg.m_bStartup)
			dlg.DoModal();
	}
}

void CNeoMem::ShowTipOfTheDay(void) {
	// CG: This function added by 'Tip of the Day' component.
	CTipDlg dlg;
	dlg.DoModal();
}
*/






void CNeoMem::LoadTips() {
/*
	// Open tip file from resource data
	// Note: Use NULL for hModule and it will load from current .exe's module
	HRSRC hResource = ::FindResource(NULL, MAKEINTRESOURCE(IDF_TIPS_FILE), "BINARYFILE");
	if (hResource) {
		// Note: hGlobal is only listed as HGLOBAL for backward compatibility - it's not really an HGLOBAL!
		HGLOBAL hGlobal = ::LoadResource(NULL, hResource);
		if (hGlobal) {
			// Note: The pointer returned by LockResource is valid until the module containing the resource 
			// is unloaded. It is not necessary to unlock resources because the system automatically deletes 
			// them when the process that created them terminates.
			LPVOID pdat = ::LockResource(hGlobal);
			int nBytes = _tcslen((const TCHAR *) pdat) * sizeof(TCHAR);
			if (pdat) {
				m_memTips.Attach((BYTE*) pdat, nBytes);
//				m_memTips.Read(
			}
		}
	}
*/
}



//x
/*
// Callback used by CNeoMem::BrowseFolder
int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
	TCHAR szDir[MAX_PATH];
	switch (uMsg) {
		// Indicates the browse dialog box has finished initializing. The lParam value is zero. 
		case BFFM_INITIALIZED:
			// BFFM_SETSELECTION selects the specified folder. 
			// To use a PIDL to specify the folder, set the message's lParam to the PIDL, 
			// and set wParam to FALSE. 
			// To specify the folder's path, set the message's lParam value to point to a 
			// NULL-terminated string with the path, and set wParam to TRUE. 
			if (::GetCurrentDirectory (sizeof(szDir) / sizeof(TCHAR), szDir))
			{
				::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM) szDir);
			}
			break;
                  
		// Indicates the selection has changed. The lParam parameter points to the item identifier list for the newly selected item. 
		case BFFM_SELCHANGED: 
			break;
		default:
			break;
	}
	return 0;
}
*/

//x
/*
// Browse for a folder. 
// Returns True if user hit OK, with name of folder in cstring.
// See also Browse for a Folder the Non-COM Way Brian Hart in MSDN
BOOL CNeoMem::BrowseFolder(LPCTSTR szInstructions, CString &strFolder) {
	BOOL bOK = FALSE;

	LPMALLOC pMalloc;
	// Get the Shell's default allocator
	if (::SHGetMalloc(&pMalloc) == NOERROR) {
		BROWSEINFO bi;
		char pszBuffer[MAX_PATH];

		// this doesn't work - how do you have it select a folder?
//		LPTSTR szFolder = strFolder.GetBuffer(0);
//		strcpy(pszBuffer, szFolder);
//		strFolder.ReleaseBuffer();

		LPITEMIDLIST pidl;
		// Get help on BROWSEINFO struct - it's got all the bit settings.
//		bi.hwndOwner = GetSafeHwnd();
		bi.hwndOwner = m_pMainWnd->GetSafeHwnd();
		bi.pidlRoot = NULL;
		bi.pszDisplayName = pszBuffer;
		bi.lpszTitle = szInstructions;
		bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
//		bi.lpfn = NULL;
		bi.lpfn = BrowseCallbackProc;
		bi.lParam = 0;
		// This next call issues the dialog box
		if ((pidl = ::SHBrowseForFolder(&bi)) != NULL) {
			if (::SHGetPathFromIDList(pidl, pszBuffer)) { 
				// At this point pszBuffer contains the selected path
				strFolder = pszBuffer;
				bOK = TRUE;
			}
			// Free the PIDL allocated by SHBrowseForFolder
			pMalloc->Free(pidl);
		}
		// Release the shell's allocator
		pMalloc->Release();
	}
	return bOK;
}

*/



// Show the welcome dialog if flag is set and no other files are open.
// Note: m_bAppShuttingDown is set in the main frame WM_CLOSE handler.
void CNeoMem::ShowWelcomeDialog() {
	BOOL bShow = m_bShowWelcome && // Option for welcome dialog
							(GetDocCount() == 0) && // No documents open
							(m_bAppShuttingDown == FALSE) && // App is not shutting down
							(m_bWindowsShuttingDown == FALSE); // Windows is not shutting down
	if (bShow) {
		CDialogWelcome dlg;
		dlg.DoModal();
	}
}




// Get a count of the number of open documents.
int CNeoMem::GetDocCount() {
	int nCount = 0;
	//, just one doc template
	POSITION pos = GetFirstDocTemplatePosition();
	CDocTemplate* pDocTemplate = GetNextDocTemplate(pos);
	if (pDocTemplate) {
		// walk through open documents
		pos = pDocTemplate->GetFirstDocPosition();
		while (pos) {
			nCount++;
			pDocTemplate->GetNextDoc(pos);
		}
	}
	return nCount;
}






// Global functions
//-----------------------------------------------------------------------------------------------------------------

//, move

// This hooks into the new operator memory allocation so you can find where memory is
// being allocated but not released.
#ifdef _DEBUG
static BOOL AFXAPI AllocHook(size_t nSize, BOOL bObject, LONG lRequestNumber) {
	TRACE("AllocHook size %d, object? %d, request %d\n", nSize, bObject, lRequestNumber);
	if (lRequestNumber == 52)
		TRACE("    gotcha!\n");
	return TRUE;
}
#endif




// This is called when a crash happens - all dirty files are saved to copies.
BOOL CNeoMem::SaveAllModifiedBackup(CString& strMsg) {
	// There is only one doc template
	POSITION pos = GetFirstDocTemplatePosition();
	CDocTemplate* pDocTemplate = GetNextDocTemplate(pos);
	if (pDocTemplate) {
		// Walk through open documents
		pos = pDocTemplate->GetFirstDocPosition();
		while (pos) {
			// Send hintSave to document - will set document modified flag if any
			// changes are saved to the document.
			try {
				BDoc* pDoc = DYNAMIC_DOWNCAST(BDoc, pDocTemplate->GetNextDoc(pos));
				ASSERT_VALID(pDoc);
				BObject* pobjCurrent = pDoc->GetCurrentObject();
				pDoc->UpdateAllViewsEx(NULL, hintSave, pobjCurrent);

				// If doc needs to be saved, try to save it to a backup file
				if (pDoc->IsModified()) {

					CString strOriginalName = pDoc->GetPathName();
//					// If string is empty then file hasn't been saved yet - use the doc title (Document1)
//					if (strOriginalName.IsEmpty())
//						strOriginalName = m_strDocumentFolder + "\\" + pDoc->GetTitle();
//					int nPos = strOriginalName.ReverseFind('.');
//					CString strNewName;
//					if (nPos != -1)
//						strNewName = strOriginalName.Left(nPos) + _T("_ErrorBackup.neo");
//					else
//						strNewName = strOriginalName + _T("_ErrorBackup.neo");
					CString strNewName = pDoc->GetModifiedName(strOriginalName, "_ErrorBackup");
					pDoc->SetPathName(strNewName, FALSE);

					// Save the file without prompting user (as SaveModified() would do).
					// Framework does NOT throw an exception - just returns False. 
					CString str;
					if (pDoc->SaveModifiedBackup()) {
						str.Format("The document \"%s\" was saved as \"%s\". NeoMem will look for this file the next time you open \"%s\". \r\n", 
							(LPCTSTR) strOriginalName, 
							(LPCTSTR) strNewName, 
							(LPCTSTR) strOriginalName); 
					}
					else {
						str.Format("Unable to save \"%s\".\r\n", 
							(LPCTSTR) strOriginalName);
						pDoc->SetModifiedFlag(FALSE); // clear this flag so framework won't ask user to save it again!
					}
					strMsg += str;
				}
			}
			catch (CException* pe) {
				pe->Delete();
				strMsg += "Unable to save remaining documents.\r\n";
				pos = 0; // exit loop
			}
		}
	}

	return TRUE;
}




// Get current child frame window, or NULL if none. 
CFrameChild* CNeoMem::GetChildFrame() {
	// CWnd* m_pMainWnd is set to CFrameMain* pMainFrame on InitInstance, 
	// so it's a CFrameMain*
	CFrameMain* pmain = STATIC_DOWNCAST(CFrameMain, m_pMainWnd);
	if (pmain) {
		ASSERT_VALID(pmain);
		CFrameChild* pchild = STATIC_DOWNCAST(CFrameChild, pmain->MDIGetActive());
		if (pchild) {
			ASSERT_VALID(pchild);
			return pchild;
		}
	}
	return NULL;
}


// Get current main frame window
CFrameMain* CNeoMem::GetMainFrame() {
	CFrameMain* pmain = STATIC_DOWNCAST(CFrameMain, m_pMainWnd); 
	ASSERT_VALID(pmain);
	return pmain;
}



// Returns True if the main window is maximized
//, this doesn't work - showCmd comes out SW_NORMAL even when it's maxed
BOOL CNeoMem::IsMaximized() {
	if (m_pMainWnd) {
		ASSERT_VALID(m_pMainWnd);
		WINDOWPLACEMENT wp;
		m_pMainWnd->GetWindowPlacement(&wp);
		return (wp.showCmd == SW_SHOWMAXIMIZED);
	}
	return FALSE;
}




// Overriding the base class version
void CNeoMem::OnFileOpen() {
	TRACE("CNeoMem::OnFileOpen\n");
	// In InitInstance, m_pDocManager = new CDocManagerEx, so ok to cast.
	CDocManagerEx* pDocMgr = STATIC_DOWNCAST(CDocManagerEx, m_pDocManager);
	ASSERT_VALID(pDocMgr);
	pDocMgr->OnFileOpen();
}





// Convert the specified rtf text into plain text.
// Returns True if successful.
BOOL CNeoMem::ConvertRtfToPlain(LPCTSTR pszRtf, CString &strPlain) {
	// Assign rtf text to our invisible rtf control, then get the plain text using the standard method.
	m_rtf.SetRtf(pszRtf);
	m_rtf.GetWindowText(strPlain);
	return TRUE;
}



//, Convert the specified plain text to rtf.
// Returns True if successful.
BOOL CNeoMem::ConvertPlainToRtf(CString& strPlain, CString& strRtf) {
	ASSERT(FALSE);
	// Assign plain text to our invisible rtf control, then get the rtf text.
//	m_rtf.SetRtf(pszRtf);
//	m_rtf.GetWindowText(strPlain);
//	m_rtf.SetWindowText(strPlain);
//	strRtf = m_rtf.GetRtf();
	// walk through, converting 
	return TRUE;
}



// Bring up help topic
//, couldn't get this to work in vs10, so hid the menu item
// What is &NeoMem?
// ID_HELP_WHAT_IS
// Brings up help page which explains NeoMem\nShow Help Topic
void CNeoMem::OnCmdHelpWhatIs() {
	app.HtmlHelpA(HID_BASE_RESOURCE + IDR_NEOMEM_TYPE); // shows default topic (overview)
//	app.HtmlHelpA(IDR_NEOMEM_TYPE); // tried this
//	app.HtmlHelpA(HID_WHAT_IS); // tried this
}


// Bring up help topic
// Must add keyword to hlp/HelpIDs.h file
void CNeoMem::OnHelpWhatsNew() {
	app.HtmlHelpA(HID_WHATS_NEW);
}


// Show a message to the user, giving them the option to turn it off in the future.
//. define the different messages better
UINT CNeoMem::ShowMessage(UINT nMessage) {
	CDialogMessage2 dlg;

	// Define captions, messages and their corresponding registry entries
	LPCTSTR szCaptions[] = {"Set Clock Format"};
	LPCTSTR szMessages[] = {"The clock uses the Long Date format set in the Windows Regional Settings Control Panel. To display the Control Panel now, click on OK, then select the Date tab to change the Long Date Format."};
	LPCTSTR szRegistryEntries[] = {"ShowMessageSetClockFormat"};

	UINT nMessages = sizeof(szMessages) / sizeof(LPCTSTR);
	ASSERT(nMessage < nMessages);

	LPCTSTR szRegistryEntry = szRegistryEntries[nMessage];
	LPCTSTR szMessage = szMessages[nMessage];
	LPCTSTR szCaption = szCaptions[nMessage];

	BOOL bShowMessage = GetProfileInt(Strings::szSettings, szRegistryEntry, TRUE);
	if (bShowMessage) {
		dlg.m_bShowMessage = TRUE;
		dlg.m_strMessage = szMessage;
		dlg.m_strCaption = szCaption;
		UINT nAnswer = dlg.DoModal();
		if (nAnswer == IDOK) {
			bShowMessage = dlg.m_bShowMessage;
			if (bShowMessage == FALSE)
				WriteProfileInt(Strings::szSettings, szRegistryEntry, bShowMessage);
		}
		return nAnswer;
	}
	return IDOK;
}



// called by neodoc on successful file save
void CNeoMem::ResetAutoRecoverTimer() {
	if (m_bAutoRecover) {
		GetMainFrame()->SetAutoRecoverTimer(m_nAutoRecoverMinutes);
	}
}



// Save AutoRecover information for all files
void CNeoMem::DoAutoRecoverSave() {
	m_bDoingAutoRecover = TRUE;
	SaveAllModified();
	m_bDoingAutoRecover = FALSE;
}	








void CNeoMem::Export(BObject *pobj) {
	// ask user what they want to export
	// ask what type of export they want (.rtf, .txt, .xml, .neo) 
	// outlook has wizard:
	// 1 select import/export type
	// 2 select folder to export
	// 3 select file to export to
	// might need something similar for printing also - 
	// select entire document, item, items, or selected text
	// user might have clicked on Home, selected a bunch of items in the contents view,
	// and clicked on export

	// First make sure the current item's text is saved
	BDoc* pdoc = BDoc::GetDoc();
	BObject* pobjCurrent = pdoc->GetCurrentObject();
	pdoc->UpdateAllViewsEx(NULL, hintSave, pobjCurrent);

/*
	// get current item name
	CString strItem = pobj->GetName();

	// Get first item name
//	BObject* pobjFirst = DYNAMIC_DOWNCAST(BObject, datLink.GetLinkAt(0));
//	CString strItemName = pobjFirst->GetPropertyString(propName);
	// Put object to export in a BDataLink array
//	BDataLink datExportObjects;
//	datExportObjects.SetMultiple();
//	datExportObjects.AddLink(pobjCurrent);
//	pDoc->Export(datExportObjects);

//	CString strFilename = "c:\\documents and settings\\bburns\\desktop\\_test.csv";
//	CFilename filename = CFilename(strItem);
//	filename.RemoveBadChars();
//	CString strFilename = app.m_strExportFolder + _T("\\") + filename;
//	CString strFilename = app.m_strExportFolder + _T("\\") + strItem + strExtension;
//	CString strFileTitle = ReplaceInvalid(strItem, '_');
//	CString strFolder = app.m_strExportFolder;
	// sorry, hardcoding for now - what a mess...
	CString strFilename = app.m_strExportFolder + _T("\\") + strItem + ".csv";
	CString strFormat = "csv"; // app.m_strExportFormat;
	BOOL bRecurse = TRUE;
	BOOL bSystem = FALSE;

	// bring up export dialog to get information
	CDialogExport dlg;
	dlg.m_pobj = pobj;
	dlg.m_strItem = strItem; // selected item
	dlg.m_bRecurse = bRecurse;
	dlg.m_bSystem = bSystem;
	dlg.m_strFormat = strFormat;
//	dlg.m_strFolder = strFolder;
//	dlg.m_strFileTitle = strFileTitle;
	dlg.m_strFilename = strFilename;
	if (dlg.DoModal() == IDOK) {
		pobj = dlg.m_pobj;
		bRecurse = dlg.m_bRecurse;
		bSystem = dlg.m_bSystem;
		strFormat = dlg.m_strFormat;
		strFilename = dlg.m_strFilename;

		CPath pathFull = CPath(strFilename);
		CPath pathFolder = pathFull.GetPath();
//		CString strFolder = filename.GetFolder();
		CStringEx strFolder = pathFolder.GetStr();
		strFolder = strFolder.LeftFrom('\\'); // remove trailing slash

		// remember the selected folder and format
		app.m_strExportFolder = strFolder;
		app.m_strExportFormat = strFormat;

		// do the export, handled by document
		CWaitCursor wc;
		pdoc->Export(pobj, bRecurse, bSystem, strFormat, strFilename);
	}
*/

	BOOL bRecurse = TRUE;
	BOOL bSystem = FALSE;

	// Get the filename and path to export to
	CStringEx strFolder = m_strExportFolder;
//	CString strExtension = m_strExportFormat;
	CString strExtension = "csv"; // hardcode as first in list, for now
	CStringEx strTitle;
	if (pobj->GetObjectID() == rootUser)
		strTitle = pdoc->GetTitle(); // eg test.neo
	else {
		strTitle = pobj->GetName();
		strTitle.Trim();
		strTitle = ReplaceInvalid(strTitle, '_');
	}
	CPath path = CPath(strFolder);
	path = path & strTitle;
	path = path.RenameExtension(strExtension);
	CString strFilename = path;

	// Initialize file dialog
//, switch to cfiledialog, or fix ex
	CFileDialogEx dlg(FALSE, strExtension, strFilename, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, (LPCTSTR) app.m_strExportFilter);
	CString strCaption = _T("Export to file");
	dlg.m_ofn.lpstrTitle = strCaption;
	dlg.m_bShowOptionButton = FALSE; // because exporting, don't need save options!
	
	// Get export filename
	if (dlg.DoModal() == IDOK) {
		strFilename = dlg.GetPathName(); // eg full/path/test.rtf
		int nFilterIndex = dlg.GetFilterIndex() - 1; // go from one-based to zero-based!
		eFileFormat nFormat = app.m_listExportFormats.GetFormatId(nFilterIndex);
		strExtension = dlg.GetFileExt(); // eg rtf
//		strFolder = dlg.GetFolderPath(); // bombs

		// remember the selected folder and format
		CPath path = CPath(strFilename);
		path = path.GetPath(); // exclude filename
		strFolder = path.GetStr();
		strFolder = strFolder.LeftFrom('\\'); // remove trailing slash

		m_strExportFolder = strFolder;
		m_strExportFormat = strExtension;

		// do the export, handled by document
		CWaitCursor wc;
		pdoc->Export(pobj, bRecurse, bSystem, nFormat, strFilename);

	}

}








// Import an .ico file and create a new icon bobject to store it.
// Returns a pointer to the new icon bobject, or 0 if failed.
//. move most of this to the dialog! or the ui object!
//xBObject* CNeoMem::UIImportIcon(CUI* pui, CString strFilename /* ="" */, CString strIconname /* ="" */) {
//xBObject* CNeoMem::UIImportIcon(CString strFilename /* ="" */, CString strIconname /* ="" */) {
BObject* CNeoMem::UIImportIcon(BDoc& doc, CString strFilename /* ="" */, CString strIconname /* ="" */) {

//	HOBJECT hobjIcon = NULL;

	// Get filename if not specified
	if (strFilename.IsEmpty()) {
//,		// Ask user for icon file
//		if (!pui->GetFileName("Import Icon", "ico", Strings::szIconFilter, strFilename))
//			return NULL;
		// Bring up file open dialog to choose .ico file
//x		CFileDialogEx dlg(TRUE, _T("ico"), _T(""), OFN_HIDEREADONLY, Strings::szIconFilter, AfxGetMainWnd());
		CFileDialog dlg(TRUE, _T("ico"), _T(""), OFN_HIDEREADONLY, Strings::szIconFilter, AfxGetMainWnd());
		dlg.m_ofn.lpstrTitle = _T("Import Icon");
		if (dlg.DoModal() == IDCANCEL)
			return NULL;
		strFilename = dlg.GetPathName();
	}

	// Try to import the icon file
//x	BDataIcon* pdat = new BDataIcon;
	BDataIcon dat;
	if (!dat.LoadFile(strFilename)) {
		AfxMessageBox(_T("Unable to load file as an icon."), MB_ICONEXCLAMATION);
//x		delete pdat;
		return NULL;
	}

	// Get iconname if not specified
	if (strIconname.IsEmpty()) {
		strIconname = CPath(strFilename).GetTitle();

		// Ask user for icon name
//,		if (!pui->GetString("Import Icon", "Enter the name for the new icon:", strIconname)) {
//			delete pdat;
//			return NULL;
//		}
		CDialogEditName dlg;
		dlg.m_strCaption = _T("Import Icon");
		dlg.m_strInstructions = _T("Enter the name for the new icon:");
		dlg.m_strName = strIconname;
		if (dlg.DoModal() == IDCANCEL) {
//x			delete pdat;
			return NULL;
		}
		strIconname = dlg.m_strName;
	}

	// Create the new icon bobject
	BObject& objIcon = BObject::New(doc, classIcon, strIconname, folderIcons);

//x	objIcon.SetPropertyData(propIconData, pdat, false, false); // makes a copy of dat
	objIcon.SetPropertyData(propIconData, &dat, false, false); // makes a copy of dat
//x	delete pdat;

	//, Set its file source property
//	hobjIcon->SetPropertyString(propSourceFile, strFilename, FALSE, FALSE);

	// objIcon is a reference to a bobject. basically, a pointer that's guaranteed to be zero. 
	// because New bombs rather than return zero. 
	// so the bobject is floating around in memory. 
	// and we return a pointer to it here.
//x	return hobjIcon;
	return &objIcon;
}













// Debug routines
//-------------------------------------------------------------------------------------------------------------
//, move/cleanup


#ifdef _DEBUG

// this is used to translate a windows notification code to a string. 
//, didn't we find a similar routine in mfc internals somewhere?
struct NotifyCodeString {
	UINT nNotifyCode;
	LPCTSTR szString;
};

// Note: The # is the stringizing operator, which returns the parameter, 
// code, in double quotes!
#define NCS(notifyCode) {notifyCode, #notifyCode},

// Get these from VC98\Include\Commctrl.h file
static const NotifyCodeString aNotifyCodes[] = {
//	{NM_CLICK, "NM_CLICK"},
//	{NM_RETURN, "NM_RETURN"},
//	{0, ""}

	// General
	NCS(NM_OUTOFMEMORY)
	NCS(NM_CLICK)
	NCS(NM_CHAR)
	NCS(NM_DBLCLK)
	NCS(NM_RETURN)
	NCS(NM_RCLICK)
	NCS(NM_RDBLCLK)
	NCS(NM_SETFOCUS)
	NCS(NM_KILLFOCUS)
	NCS(NM_CUSTOMDRAW)
	NCS(NM_HOVER)
	NCS(NM_NCHITTEST)
	NCS(NM_KEYDOWN)
	NCS(NM_RELEASEDCAPTURE)
	NCS(NM_SETCURSOR)
	NCS(NM_CHAR)

	// Header
	NCS(HDN_ITEMCHANGING)
	NCS(HDN_ITEMCHANGED)
	NCS(HDN_ITEMCLICK)
	NCS(HDN_ITEMDBLCLICK)
	NCS(HDN_DIVIDERDBLCLICK)
	NCS(HDN_BEGINTRACK)
	NCS(HDN_ENDTRACK)
	NCS(HDN_TRACK)
	NCS(HDN_GETDISPINFO)
	NCS(HDN_BEGINDRAG)
	NCS(HDN_ENDDRAG)

	// Toolbar
	NCS(TBN_GETBUTTONINFO)
	NCS(TBN_BEGINDRAG)
	NCS(TBN_ENDDRAG)
	NCS(TBN_BEGINADJUST)
	NCS(TBN_ENDADJUST)
	NCS(TBN_RESET)
	NCS(TBN_QUERYINSERT)
	NCS(TBN_QUERYDELETE)
	NCS(TBN_TOOLBARCHANGE)
	NCS(TBN_CUSTHELP)
	NCS(TBN_DROPDOWN)
	NCS(TBN_GETOBJECT)

	// Listview
	NCS(LVN_ITEMCHANGING)
	NCS(LVN_ITEMCHANGED)
	NCS(LVN_INSERTITEM)
	NCS(LVN_DELETEITEM)
	NCS(LVN_DELETEALLITEMS)
	NCS(LVN_BEGINLABELEDIT)
	NCS(LVN_ENDLABELEDIT)
	NCS(LVN_COLUMNCLICK)
	NCS(LVN_BEGINDRAG)
	NCS(LVN_BEGINRDRAG)
	NCS(LVN_ODCACHEHINT)
	NCS(LVN_ODFINDITEM)
	NCS(LVN_ITEMACTIVATE)
	NCS(LVN_ODSTATECHANGED)
	NCS(LVN_ODFINDITEM)
	NCS(LVN_HOTTRACK)
	NCS(LVN_GETDISPINFO)
	NCS(LVN_SETDISPINFO)
	NCS(LVN_GETINFOTIP)
	NCS(LVN_MARQUEEBEGIN)

	// RichEdit
	NCS(EN_CHANGE)
	NCS(EN_UPDATE)
	NCS(EN_SETFOCUS)
	NCS(EN_KILLFOCUS)
	NCS(EN_SELCHANGE)
	NCS(EN_DROPFILES)
	NCS(EN_PROTECTED)
	NCS(EN_STOPNOUNDO)
	NCS(EN_SAVECLIPBOARD)
	NCS(EN_LINK)
	NCS(EN_DRAGDROPDONE)

	// TreeView
	NCS(TVN_SELCHANGING)
	NCS(TVN_SELCHANGED)
	NCS(TVN_GETDISPINFO)
	NCS(TVN_ITEMEXPANDING)
	NCS(TVN_ITEMEXPANDED)
//	NCS(TVN_BEGINDRAW)
	NCS(TVN_BEGINDRAG)
	NCS(TVN_DELETEITEM)
	NCS(TVN_BEGINLABELEDIT)
	NCS(TVN_ENDLABELEDIT)
	NCS(TVN_KEYDOWN)
	NCS(TVN_GETINFOTIP)
	NCS(TVN_SINGLEEXPAND)

	NCS(0)
};


// Usage:

CString CNeoMem::GetNotifyCodeString(UINT nNotifyCode) {
	// array of codes and strings describing them
	// walk through array to find the code
	// return the string
	CString str = "unknown code";
	int i = 0;
	while (1) {
		// Exit if at end of array
		if (aNotifyCodes[i].nNotifyCode == 0)
			break;
		// Exit if found code
		if (nNotifyCode == aNotifyCodes[i].nNotifyCode) {
			str = aNotifyCodes[i].szString;
			break;
		}
		i++;
	}
	return str;
}





// Note: The # is the stringizing operator, which returns the parameter, code, in double quotes!
#define CODE(type, code) {type*1000 + code, #code},
struct CodeString {
	UINT nCode;
	LPCTSTR szString;
};
static const CodeString aCodes[] = {
//	{hintLoad, "hintLoad"},
//	{hintSave, "hintSave"},
//	{0, ""}

	// view update hints
	CODE(0, hintLoad)
	CODE(0, hintSave)
	CODE(0, hintAdd)
	CODE(0, hintDelete)
	CODE(0, hintMoveObject)
	CODE(0, hintLoadTree)
	CODE(0, hintSelect)
	CODE(0, hintResize)
	CODE(0, hintPropertyChange)
	CODE(0, hintReposition)
	CODE(0, hintRefresh) 
	CODE(0, hintDefaultFontChanged)  
	CODE(0, hintResortChildren) 
	CODE(0, hintRefreshAllIcons)
	CODE(0, hintAddView)
	CODE(0, hintRemoveView)
	CODE(0, hintMoveViewUp)
	CODE(0, hintResortAllChildren)
	CODE(0, 0)
};


LPCTSTR CNeoMem::GetCodeName(DWORD dwCodeType, DWORD nCodeID) {
	// array of codes and strings describing them
	// walk through array to find the code
	// return the string
	int i = 0;
	DWORD nID = dwCodeType * 1000 + nCodeID;
	while (1) {
		// Exit if at end of array
		if (aCodes[i].nCode == 0)
			break;
		// Exit if found code
		if (nID == aCodes[i].nCode)
			return aCodes[i].szString;
		i++;
	}
	return "(Unknown)";
}


#endif // _DEBUG
//-------------------------------------------------------------------------------------------------------------



// show an error message and let user continue or quit. 
// see also ThrowAssertion
BOOL CNeoMem::ShowError(CString strMsg) {

//	CAppException* pe = new CAppException(TRUE); // pass true if creating on the heap
	// Bug: can't just copy LPCTSTR pointer to string, because it might be a temporary object.
	// The error message was getting overwritten with FE EE's and didn't know what was going on - 
	// a CString was being passed here and then going out of scope because of the throw.
	// So use strcpy or CStrings.
//	pe->m_pszError = pszError; // store error message
//	pe->m_strError = "Assertion Failed";
//	throw pe;

//	CError e(_T("Assertion Failed"), TRUE);
//	e.SetFileLocation(lpszFilename, nLine, lpszExpression);
//	HandleError(e);

//	CError e(FALSE); // false because created on the stack
//	e.SetName(_T("Assertion Failed"));
//	e.SetSeverity(TRUE);
//	e.SetFileLocation(lpszFilename, nLine, lpszExpression);
//	throw &e;

/*	CError* pe = new CError(TRUE);
	if (lpszError==NULL)
		pe->SetName(_T("Assertion Failed"));
	else
		pe->SetName(lpszError);
	pe->SetSeverity(FALSE);
	pe->SetFileLocation(lpszFilename, nLine, lpszExpression);
	throw pe;
*/
	return TRUE;
}


void CNeoMem::WinHelp(DWORD dwData, UINT nCmd)
{
	CWinApp::WinHelp(dwData, nCmd);
/*
	switch (nCmd)
	{
		case HELP_CONTEXT: 
			HtmlHelp(NULL, m_pszHelpFilePath, HH_HELP_CONTEXT, dwData);
			break;
		case HELP_FINDER: 
			HtmlHelp(NULL, m_pszHelpFilePath, HH_DISPLAY_TOPIC, 0);
			break;	
	}
*/
}



void CNeoMem::NewFile() {
	OnFileNew();
}





// Globals
//-----------------------------------------------------------------------------------------------------------------


// The one and only CNeoMem object
CNeoMem app;

// ui alias 
CUI& ui(app.ui); 


// Globally available cstring objects
// Note: even if moved these into app class as static members, this code
// would still be here. eg CString CNeoMem::g_strSpace = _T(" ");
CString g_strSpace = _T(" ");
CString g_strCommaSpace = _T(", ");
CString g_strQuote = _T("\"");
CString g_strQuoteSpace = _T("\" ");




