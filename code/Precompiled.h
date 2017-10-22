
// precompiled.h (renamed from stdafx.h)
// This is the include file for standard system include files, or project 
// specific include files that are used frequently but changed infrequently.
// Note: this file must be included as the first non-comment thing done 
// in each file!
// precompiled.h was created by AppWizard to keep the list of header files 
// to be precompiled. 
// Note: Precompiled header (PCH) files speed up build times because they 
// don’t require recompiling.
// see also http://www.cygnus-software.com/papers/precompiledheaders.html
//-----------------------------------------------------------------------------------------------------

#pragma once

//#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
//#include <windows.h> // TrackMouseEvent


// Visual C++ no longer supports targeting Windows 95, Windows 98, Windows ME, or Windows NT. 
// http://msdn.microsoft.com/en-us/library/6sehtctf.aspx
// This targets the Windows 2000 operating system.
// #define WINVER 0x0500
// #define _WIN32_WINNT 0x0500
#define WINVER NTDDI_WINXP //0x05010000
#define _WIN32_WINNT NTDDI_WINXP


#ifdef _WIN32_WINNT
#if (_WIN32_WINNT >= 0x0400)
#pragma message("Functions Should Be Enabled")
#else
#pragma message("Operating System Version Too OLD")
#endif
#else
#pragma message("Computer is Not Win32 NT Based")
#endif


// Note: afx was the old name for mfc. 

#include <afxwin.h>     // MFC core and standard components

// Q195032: Incorrect Function Signatures May Cause Problems in Release
// http://support.microsoft.com/kb/195032
// To get a compile-time error when using message map macros, you 
// can re-define the macros after the "#include <afxwin.h>" line in 
// your Stdafx.h file. The code below uses the static_cast<> operator 
// to aid the compiler and developer by doing strict type conversion 
// on the function pointer:
#undef  ON_MESSAGE
#define ON_MESSAGE(message, memberFxn) \
	{ message, 0, 0, 0, AfxSig_lwl, \
	(AFX_PMSG) (AFX_PMSGW) (static_cast< LRESULT (AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM) > (&memberFxn)) },
#undef  ON_REGISTERED_MESSAGE
#define ON_REGISTERED_MESSAGE(nMessageVariable, memberFxn) \
	{ 0xC000, 0, 0, 0, (UINT)(UINT*)(&nMessageVariable), \
	(AFX_PMSG)(AFX_PMSGW)(static_cast<LRESULT (AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM)>(&memberFxn)) },
#undef  ON_THREAD_MESSAGE
#define ON_THREAD_MESSAGE(message, memberFxn) \
	{ message, 0, 0, 0, AfxSig_vwl, \
	(AFX_PMSG)(AFX_PMSGT)(static_cast<void (AFX_MSG_CALL CWinThread::*)(WPARAM, LPARAM)>(&memberFxn)) },
#undef  ON_REGISTERED_THREAD_MESSAGE
#define ON_REGISTERED_THREAD_MESSAGE(nMessageVariable, memberFxn) \
	{ 0xC000, 0, 0, 0, (UINT)(UINT*)(&nMessageVariable), \
	(AFX_PMSG)(AFX_PMSGT)(static_cast<void (AFX_MSG_CALL WinThread::*)(WPARAM, LPARAM)>(&memberFxn)) },


#include <afxext.h>     // MFC extensions
#include <afxcmn.h>		// MFC support for Windows Common Controls
#include <afxcview.h>	// MFC support for common control views
#include <afxdisp.h>    // MFC Automation classes
#include <afxdtctl.h>	// MFC support for Internet Explorer 4 Common Controls
#include <afxole.h>		// MFC Drag drop support
#include <afxadv.h>		// MFC CSharedFile support
#include <afxtempl.h>	// MFC Template support, for typedef CArray<...> etc
#include <afxpriv.h>    // MFC print preview support, includes CPreviewView
#include <afxrich.h>	// MFC support for richedit control - cricheditdoc etc
#include <afxodlgs.h>	// MFC support for olepropertydialog
#include <afxdao.h>		// MFC support for database access - for importing

#include <dde.h>	    // DDE suport (includes WM_DDE_EXECUTE definition - needed by CDialogWelcome)
#include <mapi.h>		// for email support
#include <richole.h>	// includes LPRICHEDITOLE etc
#include <shlwapi.h>	// Includes DLLGETVERSIONPROC
#include <stdlib.h>		// Standard something
#include <wincrypt.h>	// cryptography api functions


// If you copy source code from MFC routines some of them require this macro. 
// Defined in MFC's precompiled.h file:
#define DELETE_EXCEPTION(e) do { e->Delete(); } while (0)


#include "library.h"




//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

