# Microsoft Developer Studio Project File - Name="SOS1937FrontEndCE" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (WCE SH4) Application" 0x8601

CFG=SOS1937FrontEndCE - Win32 (WCE SH4) Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sos1937frontendce.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sos1937frontendce.mak" CFG="SOS1937FrontEndCE - Win32 (WCE SH4) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SOS1937FrontEndCE - Win32 (WCE SH4) Release" (based on "Win32 (WCE SH4) Application")
!MESSAGE "SOS1937FrontEndCE - Win32 (WCE SH4) Debug" (based on "Win32 (WCE SH4) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "SOS1937FrontEndCE"
# PROP Scc_LocalPath "Dreamcast"
# PROP WCE_FormatVersion "6.0"
CPP=shcl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SOS1937FrontEndCE - Win32 (WCE SH4) Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WCESH4Rel"
# PROP BASE Intermediate_Dir "WCESH4Rel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WCESH4Rel"
# PROP Intermediate_Dir "WCESH4Rel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MC /W3 /Zi /Ox /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "NDEBUG" /D "SHx" /D "SH4" /D "_SH4_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /D "_MBCS" /D "_UNICODE" /YX /Qsh4r7 /Qs /Qfast /Qgvp /c
# ADD CPP /nologo /MC /W3 /Zi /Ox /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "NDEBUG" /D "SHx" /D "SH4" /D "_SH4_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /D "_MBCS" /D "_UNICODE" /YX /Qsh4r7 /Qs /Qfast /Qgvp /c
# ADD BASE RSC /l 0x809 /r /d "SHx" /d "SH4" /d "_SH4_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "NDEBUG"
# ADD RSC /l 0x809 /r /d "SHx" /d "SH4" /d "_SH4_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "NDEBUG"
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 coredll.lib /nologo /machine:SH4 /nodefaultlib:"$(CENoDefaultLib)" /subsystem:$(CESubsystem) /STACK:65536,4096
# ADD LINK32 coredll.lib /nologo /machine:SH4 /nodefaultlib:"$(CENoDefaultLib)" /subsystem:$(CESubsystem) /STACK:65536,4096

!ELSEIF  "$(CFG)" == "SOS1937FrontEndCE - Win32 (WCE SH4) Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WCESH4Dbg"
# PROP BASE Intermediate_Dir "WCESH4Dbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WCESH4Dbg"
# PROP Intermediate_Dir "WCESH4Dbg"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MC /W3 /Zi /Od /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "DEBUG" /D "SHx" /D "SH4" /D "_SH4_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /D "_MBCS" /D "_UNICODE" /YX /Qsh4r7 /Qs /Qfast /c
# ADD CPP /nologo /MC /W3 /Zi /Od /D "KATANA" /D "DREAMCAST" /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "DEBUG" /D "SHx" /D "SH4" /D "_SH4_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /D "_MBCS" /D "_UNICODE" /YX /Qsh4r7 /Qs /Qfast /c
# ADD BASE RSC /l 0x809 /r /d "SHx" /d "SH4" /d "_SH4_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "DEBUG"
# ADD RSC /l 0x809 /r /d "SHx" /d "SH4" /d "_SH4_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "DEBUG"
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 coredll.lib /nologo /debug /machine:SH4 /nodefaultlib:"$(CENoDefaultLib)" /subsystem:$(CESubsystem) /STACK:65536,4096
# ADD LINK32 coredll.lib ddraw.lib dinput.lib dxguid.lib mapledev.lib /nologo /debug /machine:SH4 /nodefaultlib:"$(CENoDefaultLib)" /subsystem:$(CESubsystem) /STACK:65536,4096
# SUBTRACT LINK32 /incremental:no /nodefaultlib

!ENDIF 

# Begin Target

# Name "SOS1937FrontEndCE - Win32 (WCE SH4) Release"
# Name "SOS1937FrontEndCE - Win32 (WCE SH4) Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\main.cpp
NODEP_CPP_MAIN_=\
	".\acingLine.h"\
	".\amera.h"\
	".\apledev.h"\
	".\ar.h"\
	".\arbody.h"\
	".\arc.h"\
	".\arcf.h"\
	".\arcman.h"\
	".\arwheel.h"\
	".\aterial.h"\
	".\atrix3x4.h"\
	".\bject.h"\
	".\dge.h"\
	".\eader\Alphabet.h"\
	".\eader\Boris2Emulation.h"\
	".\eader\Buttons.h"\
	".\eader\ConsoleRegistry.h"\
	".\eader\FrontEndObject.h"\
	".\eader\IOFunctions.h"\
	".\eader\main.h"\
	".\eader\Screen.h"\
	".\eader\Slider.h"\
	".\eader\Word.h"\
	".\ebugger.h"\
	".\ector2P.h"\
	".\ector3.h"\
	".\eg.h"\
	".\erstore.h"\
	".\ertex.h"\
	".\esh.h"\
	".\extureStore.h"\
	".\graphics.h"\
	".\hintr.h"\
	".\hsgintr.h"\
	".\imeStats.h"\
	".\iming.h"\
	".\ineTable.h"\
	".\inkToTriangle.h"\
	".\isplay.h"\
	".\itstop.h"\
	".\ky.h"\
	".\LLcar.h"\
	".\mulder.h"\
	".\ngine.h"\
	".\ntrecip.h"\
	".\odel.h"\
	".\odelStore.h"\
	".\ointdata2d.h"\
	".\ointdata3d.h"\
	".\ollide.h"\
	".\ollOverNumber.h"\
	".\ongeodata.h"\
	".\onsoleFrontLib.h"\
	".\ont.h"\
	".\ool.h"\
	".\orld.h"\
	".\ort.h"\
	".\ortlistentry.h"\
	".\os1937\ChooseOptionsScreen.h"\
	".\OS1937\ControllerOptionsScreen.h"\
	".\OS1937\EnterPlayerName.h"\
	".\os1937\FrontScreen.h"\
	".\OS1937\GlobalStructure.h"\
	".\OS1937\MemoryCardScreen.h"\
	".\os1937\OptionsScreen.h"\
	".\OS1937\PlayerDetailsScreen.h"\
	".\os1937\RaceType.h"\
	".\os1937\SelectCarScreen.h"\
	".\os1937\SelectTrackScreen.h"\
	".\OS1937\SingleRace.h"\
	".\os1937\SOS_Index.h"\
	".\oundBox.h"\
	".\phere.h"\
	".\pline.h"\
	".\priteFX.h"\
	".\priteFXentry.h"\
	".\rchive.h"\
	".\rchiveIface.h"\
	".\rcstack.h"\
	".\rdebug.h"\
	".\riangle.h"\
	".\rrorlib.h"\
	".\S2all.h"\
	".\S2configuration.h"\
	".\S2consts.h"\
	".\S2features.h"\
	".\S2macros.h"\
	".\S2simTypes.h"\
	".\S2utils.h"\
	".\uaternion.h"\
	".\unk.h"\
	".\utLists.h"\
	".\utTriangle.h"\
	
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Screens"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# End Target
# End Project
