# Microsoft Developer Studio Project File - Name="NeoMem" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=NeoMem - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NeoMem.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NeoMem.mak" CFG="NeoMem - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NeoMem - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "NeoMem - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NeoMem - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\code\builds\release"
# PROP Intermediate_Dir "..\code\builds\release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"precompiled.h" /FD /c
# ADD CPP /nologo /MT /W3 /WX /GR /GX /Zi /Od /Ob2 /I "..\resources" /I "..\help" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D _WIN32_WINNT=0x0400 /Fr /Yu"precompiled.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib advapi32.lib /nologo /version:0.0 /subsystem:windows /pdb:none /machine:I386
# SUBTRACT LINK32 /verbose /debug

!ELSEIF  "$(CFG)" == "NeoMem - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\code\builds\debug"
# PROP Intermediate_Dir "..\code\builds\debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"precompiled.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /WX /Gm /Gi /GR /GX /ZI /Od /I "..\resources" /I "..\help" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D _WIN32_WINNT=0x0400 /FAcs /Fr /Yu"precompiled.h" /FD /GZ /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib advapi32.lib /nologo /version:0.0 /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /verbose /profile

!ENDIF 

# Begin Target

# Name "NeoMem - Win32 Release"
# Name "NeoMem - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\code\BCMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\code\BData.cpp
# End Source File
# Begin Source File

SOURCE=..\code\BDataBinary.cpp
# End Source File
# Begin Source File

SOURCE=..\code\BDataBoolean.cpp
# End Source File
# Begin Source File

SOURCE=..\code\BDataColumns.cpp
# End Source File
# Begin Source File

SOURCE=..\code\BDataDate.cpp
# End Source File
# Begin Source File

SOURCE=..\code\BDataEmail.cpp
# End Source File
# Begin Source File

SOURCE=..\code\BDataFile.cpp
# End Source File
# Begin Source File

SOURCE=..\code\BDataFlags.cpp
# End Source File
# Begin Source File

SOURCE=..\code\BDataFolder.cpp
# End Source File
# Begin Source File

SOURCE=..\code\BDataHyperlink.cpp
# End Source File
# Begin Source File

SOURCE=..\code\BDataIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\code\BDataLink.cpp
# End Source File
# Begin Source File

SOURCE=..\code\BDataLong.cpp
# End Source File
# Begin Source File

SOURCE=..\code\BDataNumber.cpp
# End Source File
# Begin Source File

SOURCE=..\code\BDataPersonName.cpp
# End Source File
# Begin Source File

SOURCE=..\code\BDataString.cpp
# End Source File
# Begin Source File

SOURCE=..\code\BDataViews.cpp
# End Source File
# Begin Source File

SOURCE=..\code\BDataWebsite.cpp
# End Source File
# Begin Source File

SOURCE=..\code\BObject.cpp
# End Source File
# Begin Source File

SOURCE=..\code\BObjects.cpp
# End Source File
# Begin Source File

SOURCE=..\code\Clock.cpp
# End Source File
# Begin Source File

SOURCE=..\code\Color.cpp
# End Source File
# Begin Source File

SOURCE=..\code\ColorButton.cpp
# End Source File
# Begin Source File

SOURCE=..\code\ColourPopup.cpp
# End Source File
# Begin Source File

SOURCE=..\code\Command.cpp
# End Source File
# Begin Source File

SOURCE=..\code\CommandHistory.cpp
# End Source File
# Begin Source File

SOURCE=..\code\CommandLineInfoEx.cpp
# End Source File
# Begin Source File

SOURCE=..\code\Crypto.cpp
# End Source File
# Begin Source File

SOURCE=..\code\CryptoArchive.cpp
# End Source File
# Begin Source File

SOURCE=..\code\CryptoFile.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogAbout.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogAbout2.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogEditDate.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogEditFolder.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogEditLink.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogEditName.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogEditObject.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogEditObject2.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogEditPersonName.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogEditProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogEditRichText.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogEditString.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogError.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogEx.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogException.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogExport.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogGetLocation.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogImport.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogInsertLink.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogMessage.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogMessage2.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogNewFile.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogNewLinkSource.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogNotice.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogPassword.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogPrint.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogProperties.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogSaveOptions.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogSelectProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogTimer.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogTip.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogUpgradeFile.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogViews.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DialogWelcome.cpp
# End Source File
# Begin Source File

SOURCE=..\code\DocManagerEx.cpp
# End Source File
# Begin Source File

SOURCE=..\code\EditEx.cpp
# End Source File
# Begin Source File

SOURCE=..\code\EditInPlace.cpp
# End Source File
# Begin Source File

SOURCE=..\code\Email.cpp
# End Source File
# Begin Source File

SOURCE=..\code\Error.cpp
# End Source File
# Begin Source File

SOURCE=..\code\FileDialogEx.cpp
# End Source File
# Begin Source File

SOURCE=..\code\FileFormat.cpp
# End Source File
# Begin Source File

SOURCE=..\code\FileFormats.cpp
# End Source File
# Begin Source File

SOURCE=..\code\Filename.cpp
# End Source File
# Begin Source File

SOURCE=..\code\FileText.cpp
# End Source File
# Begin Source File

SOURCE=..\code\FileVersion.cpp
# End Source File
# Begin Source File

SOURCE=..\code\FontEx.cpp
# End Source File
# Begin Source File

SOURCE=..\code\FrameChild.cpp
# End Source File
# Begin Source File

SOURCE=..\code\FrameMain.cpp
# End Source File
# Begin Source File

SOURCE=..\code\GUI.cpp
# End Source File
# Begin Source File

SOURCE=..\code\Hint.cpp
# End Source File
# Begin Source File

SOURCE=..\code\History.cpp
# End Source File
# Begin Source File

SOURCE=..\code\IconCache.cpp
# End Source File
# Begin Source File

SOURCE=..\code\Info.cpp
# End Source File
# Begin Source File

SOURCE=..\code\ListCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=..\code\MDITrayFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\code\MirrorFileEx.cpp
# End Source File
# Begin Source File

SOURCE=..\code\ModuleVersion.cpp
# End Source File
# Begin Source File

SOURCE=..\code\NeoDoc.cpp
# End Source File
# Begin Source File

SOURCE=..\code\NeoDocTemplate.cpp
# End Source File
# Begin Source File

SOURCE=..\code\NeoMem.cpp
# End Source File
# Begin Source File

SOURCE=..\code\PageClassAdvanced.cpp
# End Source File
# Begin Source File

SOURCE=..\code\PageClassContents.cpp
# End Source File
# Begin Source File

SOURCE=..\code\PageClassIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\code\PageClassInheritance.cpp
# End Source File
# Begin Source File

SOURCE=..\code\PageClassIntro.cpp
# End Source File
# Begin Source File

SOURCE=..\code\PageClassName.cpp
# End Source File
# Begin Source File

SOURCE=..\code\PageClassProperties.cpp
# End Source File
# Begin Source File

SOURCE=..\code\PageClassSummary.cpp
# End Source File
# Begin Source File

SOURCE=..\code\PageClassType.cpp
# End Source File
# Begin Source File

SOURCE=..\code\PageClassViews.cpp
# End Source File
# Begin Source File

SOURCE=..\code\PageFileGeneral.cpp
# End Source File
# Begin Source File

SOURCE=..\code\PageObjectGeneral.cpp
# End Source File
# Begin Source File

SOURCE=..\code\PageOptionsDisplay.cpp
# End Source File
# Begin Source File

SOURCE=..\code\PageOptionsFile.cpp
# End Source File
# Begin Source File

SOURCE=..\code\Path.cpp
# End Source File
# Begin Source File

SOURCE=..\code\PreviewViewEx.cpp
# End Source File
# Begin Source File

SOURCE=..\code\PrintInfoMore.cpp
# End Source File
# Begin Source File

SOURCE=..\code\PropertyPageEx2.cpp
# End Source File
# Begin Source File

SOURCE=..\code\PropertySheetEx2.cpp
# End Source File
# Begin Source File

SOURCE=..\code\RegKey.cpp
# End Source File
# Begin Source File

SOURCE=..\code\RichEditCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=..\code\RichEditDocEx.cpp
# End Source File
# Begin Source File

SOURCE=..\code\RichEditStuff.cpp
# End Source File
# Begin Source File

SOURCE=..\code\RichEditView2.cpp
# End Source File
# Begin Source File

SOURCE=..\code\SheetOptions.cpp
# End Source File
# Begin Source File

SOURCE=..\code\SheetWizard.cpp
# End Source File
# Begin Source File

SOURCE=..\code\SplitterWndEx.cpp
# End Source File
# Begin Source File

SOURCE=..\code\StaticEx.cpp
# End Source File
# Begin Source File

SOURCE=..\code\StatusBarEx.cpp
# End Source File
# Begin Source File

SOURCE=..\code\StringEx.cpp
# End Source File
# Begin Source File

SOURCE=..\code\SystemTray.cpp
# End Source File
# Begin Source File

SOURCE=..\code\TabCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=..\code\TabOrder.cpp
# End Source File
# Begin Source File

SOURCE=..\code\Test.cpp
# End Source File
# Begin Source File

SOURCE=..\code\ToolBarCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=..\code\ToolbarRtf.cpp
# End Source File
# Begin Source File

SOURCE=..\code\TreeCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=..\code\UI.cpp
# End Source File
# Begin Source File

SOURCE=..\code\Undo.cpp
# End Source File
# Begin Source File

SOURCE=..\code\version.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\code\ViewBookmarks.cpp
# End Source File
# Begin Source File

SOURCE=..\code\ViewContents.cpp
# End Source File
# Begin Source File

SOURCE=..\code\ViewEx.cpp
# End Source File
# Begin Source File

SOURCE=..\code\ViewGrid.cpp
# End Source File
# Begin Source File

SOURCE=..\code\ViewHeader.cpp
# End Source File
# Begin Source File

SOURCE=..\code\ViewHome.cpp
# End Source File
# Begin Source File

SOURCE=..\code\ViewHomeInner.cpp
# End Source File
# Begin Source File

SOURCE=..\code\ViewIndex.cpp
# End Source File
# Begin Source File

SOURCE=..\code\ViewPictures.cpp
# End Source File
# Begin Source File

SOURCE=..\code\ViewProperties.cpp
# End Source File
# Begin Source File

SOURCE=..\code\ViewRtf.cpp
# End Source File
# Begin Source File

SOURCE=..\code\ViewSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\code\ViewTabs.cpp
# End Source File
# Begin Source File

SOURCE=..\code\ViewTree.cpp
# End Source File
# Begin Source File

SOURCE=..\code\WinMsg.cpp
# End Source File
# Begin Source File

SOURCE=..\code\XComboBoxEx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\code\BCMenu.h
# End Source File
# Begin Source File

SOURCE=..\code\BData.h
# End Source File
# Begin Source File

SOURCE=..\code\BDataBinary.h
# End Source File
# Begin Source File

SOURCE=..\code\BDataBoolean.h
# End Source File
# Begin Source File

SOURCE=..\code\BDataColumns.h
# End Source File
# Begin Source File

SOURCE=..\code\BDataDate.h
# End Source File
# Begin Source File

SOURCE=..\code\BDataEmail.h
# End Source File
# Begin Source File

SOURCE=..\code\BDataFile.h
# End Source File
# Begin Source File

SOURCE=..\code\BDataFlags.h
# End Source File
# Begin Source File

SOURCE=..\code\BDataFolder.h
# End Source File
# Begin Source File

SOURCE=..\code\BDataHyperlink.h
# End Source File
# Begin Source File

SOURCE=..\code\BDataIcon.h
# End Source File
# Begin Source File

SOURCE=..\code\BDataLink.h
# End Source File
# Begin Source File

SOURCE=..\code\BDataLong.h
# End Source File
# Begin Source File

SOURCE=..\code\BDataNumber.h
# End Source File
# Begin Source File

SOURCE=..\code\BDataPersonName.h
# End Source File
# Begin Source File

SOURCE=..\code\BDataString.h
# End Source File
# Begin Source File

SOURCE=..\code\BDataViews.h
# End Source File
# Begin Source File

SOURCE=..\code\BDataWebsite.h
# End Source File
# Begin Source File

SOURCE=..\code\BObject.h
# End Source File
# Begin Source File

SOURCE=..\code\BObjects.h
# End Source File
# Begin Source File

SOURCE=..\code\Clock.h
# End Source File
# Begin Source File

SOURCE=..\code\Color.h
# End Source File
# Begin Source File

SOURCE=..\code\ColorButton.h
# End Source File
# Begin Source File

SOURCE=..\code\ColourPopup.h
# End Source File
# Begin Source File

SOURCE=..\code\Command.h
# End Source File
# Begin Source File

SOURCE=..\code\CommandHistory.h
# End Source File
# Begin Source File

SOURCE=..\code\CommandLineInfoEx.h
# End Source File
# Begin Source File

SOURCE=..\code\ConstantsDatabase.h
# End Source File
# Begin Source File

SOURCE=..\code\Crypto.h
# End Source File
# Begin Source File

SOURCE=..\code\CryptoArchive.h
# End Source File
# Begin Source File

SOURCE=..\code\CryptoFile.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogAbout.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogAbout2.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogEditDate.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogEditFolder.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogEditLink.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogEditName.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogEditObject.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogEditObject2.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogEditPersonName.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogEditProperty.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogEditRichText.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogEditString.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogError.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogEx.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogException.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogExport.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogGetLocation.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogImport.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogInsertLink.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogMessage.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogMessage2.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogNewFile.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogNewLinkSource.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogNotice.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogPassword.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogPrint.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogProperties.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogSaveOptions.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogSelectProperty.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogTimer.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogTip.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogUpgradeFile.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogViews.h
# End Source File
# Begin Source File

SOURCE=..\code\DialogWelcome.h
# End Source File
# Begin Source File

SOURCE=..\code\DocManagerEx.h
# End Source File
# Begin Source File

SOURCE=..\code\EditEx.h
# End Source File
# Begin Source File

SOURCE=..\code\EditInPlace.h
# End Source File
# Begin Source File

SOURCE=..\code\Email.h
# End Source File
# Begin Source File

SOURCE=..\code\Error.h
# End Source File
# Begin Source File

SOURCE=..\code\FileDialogEx.h
# End Source File
# Begin Source File

SOURCE=..\code\FileFormat.h
# End Source File
# Begin Source File

SOURCE=..\code\FileFormats.h
# End Source File
# Begin Source File

SOURCE=..\code\Filename.h
# End Source File
# Begin Source File

SOURCE=..\code\FileText.h
# End Source File
# Begin Source File

SOURCE=..\code\FileVersion.h
# End Source File
# Begin Source File

SOURCE=..\code\FontEx.h
# End Source File
# Begin Source File

SOURCE=..\code\FrameChild.h
# End Source File
# Begin Source File

SOURCE=..\code\FrameMain.h
# End Source File
# Begin Source File

SOURCE=..\code\GUI.h
# End Source File
# Begin Source File

SOURCE=..\code\Hint.h
# End Source File
# Begin Source File

SOURCE=..\code\History.h
# End Source File
# Begin Source File

SOURCE=..\code\IconCache.h
# End Source File
# Begin Source File

SOURCE=..\code\Info.h
# End Source File
# Begin Source File

SOURCE=..\code\ListCtrlEx.h
# End Source File
# Begin Source File

SOURCE=..\code\LogFile.h
# End Source File
# Begin Source File

SOURCE=..\code\MDITrayFrame.h
# End Source File
# Begin Source File

SOURCE=..\code\MirrorFileEx.h
# End Source File
# Begin Source File

SOURCE=..\code\ModuleVersion.h
# End Source File
# Begin Source File

SOURCE=..\code\NeoDoc.h
# End Source File
# Begin Source File

SOURCE=..\code\NeoDocTemplate.h
# End Source File
# Begin Source File

SOURCE=..\code\PageClassAdvanced.h
# End Source File
# Begin Source File

SOURCE=..\code\PageClassContents.h
# End Source File
# Begin Source File

SOURCE=..\code\PageClassIcon.h
# End Source File
# Begin Source File

SOURCE=..\code\PageClassInheritance.h
# End Source File
# Begin Source File

SOURCE=..\code\PageClassIntro.h
# End Source File
# Begin Source File

SOURCE=..\code\PageClassName.h
# End Source File
# Begin Source File

SOURCE=..\code\PageClassProperties.h
# End Source File
# Begin Source File

SOURCE=..\code\PageClassSummary.h
# End Source File
# Begin Source File

SOURCE=..\code\PageClassType.h
# End Source File
# Begin Source File

SOURCE=..\code\PageClassViews.h
# End Source File
# Begin Source File

SOURCE=..\code\PageFileGeneral.h
# End Source File
# Begin Source File

SOURCE=..\code\PageObjectGeneral.h
# End Source File
# Begin Source File

SOURCE=..\code\PageOptionsDisplay.h
# End Source File
# Begin Source File

SOURCE=..\code\PageOptionsFile.h
# End Source File
# Begin Source File

SOURCE=..\code\Path.h
# End Source File
# Begin Source File

SOURCE=..\code\PreviewViewEx.h
# End Source File
# Begin Source File

SOURCE=..\code\PrintInfoMore.h
# End Source File
# Begin Source File

SOURCE=..\code\PropertyPageEx2.h
# End Source File
# Begin Source File

SOURCE=..\code\PropertySheetEx2.h
# End Source File
# Begin Source File

SOURCE=..\code\RegKey.h
# End Source File
# Begin Source File

SOURCE=..\code\RichEditCtrlEx.h
# End Source File
# Begin Source File

SOURCE=..\code\RichEditDocEx.h
# End Source File
# Begin Source File

SOURCE=..\code\RichEditStuff.h
# End Source File
# Begin Source File

SOURCE=..\code\RichEditView2.h
# End Source File
# Begin Source File

SOURCE=..\code\SheetOptions.h
# End Source File
# Begin Source File

SOURCE=..\code\SheetWizard.h
# End Source File
# Begin Source File

SOURCE=..\code\SplitterWndEx.h
# End Source File
# Begin Source File

SOURCE=..\code\StaticEx.h
# End Source File
# Begin Source File

SOURCE=..\code\StatusBarEx.h
# End Source File
# Begin Source File

SOURCE=..\code\StringEx.h
# End Source File
# Begin Source File

SOURCE=..\code\SystemTray.h
# End Source File
# Begin Source File

SOURCE=..\code\TabCtrlEx.h
# End Source File
# Begin Source File

SOURCE=..\code\TabOrder.h
# End Source File
# Begin Source File

SOURCE=..\code\Test.h
# End Source File
# Begin Source File

SOURCE=..\code\ToolBarCtrlEx.h
# End Source File
# Begin Source File

SOURCE=..\code\ToolbarRtf.h
# End Source File
# Begin Source File

SOURCE=..\code\TreeCtrlEx.h
# End Source File
# Begin Source File

SOURCE=..\code\UI.h
# End Source File
# Begin Source File

SOURCE=..\code\Undo.h
# End Source File
# Begin Source File

SOURCE=..\code\ViewBookmarks.h
# End Source File
# Begin Source File

SOURCE=..\code\ViewContents.h
# End Source File
# Begin Source File

SOURCE=..\code\ViewEx.h
# End Source File
# Begin Source File

SOURCE=..\code\ViewGrid.h
# End Source File
# Begin Source File

SOURCE=..\code\ViewHeader.h
# End Source File
# Begin Source File

SOURCE=..\code\ViewHome.h
# End Source File
# Begin Source File

SOURCE=..\code\ViewHomeInner.h
# End Source File
# Begin Source File

SOURCE=..\code\ViewIndex.h
# End Source File
# Begin Source File

SOURCE=..\code\ViewPictures.h
# End Source File
# Begin Source File

SOURCE=..\code\ViewProperties.h
# End Source File
# Begin Source File

SOURCE=..\code\ViewRtf.h
# End Source File
# Begin Source File

SOURCE=..\code\ViewSearch.h
# End Source File
# Begin Source File

SOURCE=..\code\ViewTabs.h
# End Source File
# Begin Source File

SOURCE=..\code\ViewTree.h
# End Source File
# Begin Source File

SOURCE=..\code\WinMsg.h
# End Source File
# Begin Source File

SOURCE=..\code\XComboBoxEx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\resources\icons\_Nebula.ico
# End Source File
# Begin Source File

SOURCE="..\resources\icons\_NeoMem Document.ico"
# End Source File
# Begin Source File

SOURCE=..\resources\icons\_NeoMem.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Apple.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\ArrowDown.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\ArrowUp.ico
# End Source File
# Begin Source File

SOURCE="..\resources\icons\Art Medium.ico"
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Art.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Art2.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Backcolor.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Bike.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Bird.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\bluesphere3.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Book.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Bookcase.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Box2.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Building.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\BulletBlue.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\bulletGreen.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Camera.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Cd.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Cellphone.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Chalkboard.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Chemicals.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Chip.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\circle_blue.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\City.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Clip03.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Clippers.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Clock.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Clock2.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Coffee.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Coin.ico
# End Source File
# Begin Source File

SOURCE="..\resources\icons\Computer Equipment.ico"
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Computer.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\CreditCard.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\diamondBlue.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\diamondBlue2.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\diamondBlueGreen.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\diamondGreen.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\diamondGreenDk.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\diamondPurple.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Doc.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\DriversLicense.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Envelope.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\FlowerRose.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Folder4.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Forecolor.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Forest.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Galaxy.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Guitar.ico
# End Source File
# Begin Source File

SOURCE=..\resources\cursors\Hand.cur
# End Source File
# Begin Source File

SOURCE=..\resources\icons\House.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Icon.ico
# End Source File
# Begin Source File

SOURCE="..\resources\icons\Intaglio green on black (256 color only).ico"
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Label.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Land.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Leaf.ico
# End Source File
# Begin Source File

SOURCE="..\resources\icons\Man andy.ico"
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Medicine.ico
# End Source File
# Begin Source File

SOURCE="..\resources\icons\Money 1.ico"
# End Source File
# Begin Source File

SOURCE="..\resources\icons\Movie Film.ico"
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Movie2.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Museum.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\New.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\newspaper.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Night.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Note.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Ocean.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Park.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Person.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Piano.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Present.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Printer.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Quote.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Record8aa.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\RedCross.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Room.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Salad.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Scanner.ico
# End Source File
# Begin Source File

SOURCE="..\resources\icons\Soccer ball.ico"
# End Source File
# Begin Source File

SOURCE=..\resources\icons\sphereWhite.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Systray.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Tape.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\TR2.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\trianglesBlue.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Videotape.ico
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Wallet.ico
# End Source File
# Begin Source File

SOURCE="..\resources\icons\woman jjl.ico"
# End Source File
# Begin Source File

SOURCE=..\resources\icons\Wrench.ico
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter "cnt;rtf"
# Begin Source File

SOURCE=..\help\helpIDs.h
# End Source File
# Begin Source File

SOURCE=..\help\NeoMem.cnt

!IF  "$(CFG)" == "NeoMem - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Copying contents file...
OutDir=.\..\code\builds\release
InputPath=..\help\NeoMem.cnt
InputName=NeoMem

"$(OutDir)\$(InputName).cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "..\help\$(InputName).cnt" $(OutDir)

# End Custom Build

!ELSEIF  "$(CFG)" == "NeoMem - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Copying contents file...
OutDir=.\..\code\builds\debug
InputPath=..\help\NeoMem.cnt
InputName=NeoMem

"$(OutDir)\$(InputName).cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "..\help\$(InputName).cnt" $(OutDir)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\help\NeoMem.hpj

!IF  "$(CFG)" == "NeoMem - Win32 Release"

# PROP Ignore_Default_Tool 1
USERDEP__NEOME="..\help\NeoMem.rtf"	"..\help\$(TargetName).hm"	
# Begin Custom Build - Making help file...
OutDir=.\..\code\builds\release
TargetName=NeoMem
InputPath=..\help\NeoMem.hpj
InputName=NeoMem

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcw /C /E /M "..\help\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "..\help\$(InputName).hlp" goto :Error 
	copy "..\help\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo ..\help\$(InputName).hpj(1) : error: 
	type "..\help\$(InputName).log" 
	:done 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "NeoMem - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__NEOME="..\help\NeoMem.rtf"	"..\help\$(TargetName).hm"	
# Begin Custom Build - Making help file...
OutDir=.\..\code\builds\debug
TargetName=NeoMem
InputPath=..\help\NeoMem.hpj
InputName=NeoMem

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcw /C /E /M "..\help\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "..\help\$(InputName).hlp" goto :Error 
	copy "..\help\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo ..\help\$(InputName).hpj(1) : error: 
	type "..\help\$(InputName).log" 
	:done 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\help\NeoMem.rtf
# End Source File
# End Group
# Begin Source File

SOURCE=..\code\Constants.h
# End Source File
# Begin Source File

SOURCE=..\code\Library.cpp
# End Source File
# Begin Source File

SOURCE=..\code\Library.h
# End Source File
# Begin Source File

SOURCE=..\code\macros.h
# End Source File
# Begin Source File

SOURCE=..\code\NeoMem.h
# End Source File
# Begin Source File

SOURCE=..\resources\NeoMem.rc
# End Source File
# Begin Source File

SOURCE=..\code\precompiled.cpp
# ADD CPP /Yc"precompiled.h"
# End Source File
# Begin Source File

SOURCE=..\code\precompiled.h
# End Source File
# Begin Source File

SOURCE=..\resources\Resource.h

!IF  "$(CFG)" == "NeoMem - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making help include file...
TargetName=NeoMem
InputPath=..\resources\Resource.h

"..\help\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"..\help\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"..\help\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 $(InputPath) >>"..\help\$(TargetName).hm" 
	echo. >>"..\help\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"..\help\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 $(InputPath) >>"..\help\$(TargetName).hm" 
	echo. >>"..\help\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"..\help\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 $(InputPath) >>"..\help\$(TargetName).hm" 
	echo. >>"..\help\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"..\help\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 $(InputPath) >>"..\help\$(TargetName).hm" 
	echo. >>"..\help\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"..\help\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 $(InputPath) >>"..\help\$(TargetName).hm" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "NeoMem - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making help include file...
TargetName=NeoMem
InputPath=..\resources\Resource.h

"..\help\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"..\help\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"..\help\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 $(InputPath) >>"..\help\$(TargetName).hm" 
	echo. >>"..\help\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"..\help\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 $(InputPath) >>"..\help\$(TargetName).hm" 
	echo. >>"..\help\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"..\help\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 $(InputPath) >>"..\help\$(TargetName).hm" 
	echo. >>"..\help\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"..\help\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 $(InputPath) >>"..\help\$(TargetName).hm" 
	echo. >>"..\help\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"..\help\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 $(InputPath) >>"..\help\$(TargetName).hm" 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\Strings.cpp
# End Source File
# Begin Source File

SOURCE=..\code\Strings.h
# End Source File
# End Target
# End Project
# Section NeoMem : {72ADFD54-2C39-11D0-9903-00A0C91BC942}
# 	1:17:CG_IDS_DIDYOUKNOW:833
# 	1:8:IDD_TIP1:829
# 	1:14:IDB_LIGHTBULB1:828
# 	1:22:CG_IDS_TIPOFTHEDAYMENU:832
# 	1:18:CG_IDS_TIPOFTHEDAY:830
# 	1:22:CG_IDS_TIPOFTHEDAYHELP:841
# 	1:19:CG_IDP_FILE_CORRUPT:838
# 	1:18:CG_IDS_FILE_ABSENT:836
# 	2:17:CG_IDS_DIDYOUKNOW:CG_IDS_DIDYOUKNOW
# 	2:7:CTipDlg:CTipDlg
# 	2:22:CG_IDS_TIPOFTHEDAYMENU:CG_IDS_TIPOFTHEDAYMENU
# 	2:18:CG_IDS_TIPOFTHEDAY:CG_IDS_TIPOFTHEDAY
# 	2:12:CTIP_Written:OK
# 	2:22:CG_IDS_TIPOFTHEDAYHELP:CG_IDS_TIPOFTHEDAYHELP
# 	2:2:BH:
# 	2:19:CG_IDP_FILE_CORRUPT:CG_IDP_FILE_CORRUPT
# 	2:7:IDD_TIP:IDD_TIP1
# 	2:8:TipDlg.h:TipDlg.h
# 	2:13:IDB_LIGHTBULB:IDB_LIGHTBULB1
# 	2:18:CG_IDS_FILE_ABSENT:CG_IDS_FILE_ABSENT
# 	2:10:TipDlg.cpp:TipDlg.cpp
# End Section
