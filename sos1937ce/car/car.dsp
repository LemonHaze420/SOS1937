# Microsoft Developer Studio Project File - Name="car" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (WCE SH4) Static Library" 0x8604

CFG=car - Win32 (WCE SH4) Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "car.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "car.mak" CFG="car - Win32 (WCE SH4) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "car - Win32 (WCE SH4) Release" (based on "Win32 (WCE SH4) Static Library")
!MESSAGE "car - Win32 (WCE SH4) Debug" (based on "Win32 (WCE SH4) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "car"
# PROP Scc_LocalPath "."
# PROP WCE_FormatVersion "6.0"
CPP=shcl.exe

!IF  "$(CFG)" == "car - Win32 (WCE SH4) Release"

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
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "car - Win32 (WCE SH4) Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WCESH4Dbg"
# PROP BASE Intermediate_Dir "WCESH4Dbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WCESH4Dbg"
# PROP Intermediate_Dir "WCESH4Dbg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MC /W3 /Zi /Od /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "DEBUG" /D "SHx" /D "SH4" /D "_SH4_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /D "_MBCS" /D "_UNICODE" /YX /Qsh4r7 /Qs /Qfast /c
# ADD CPP /nologo /MC /W3 /Zi /Od /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "DEBUG" /D "SHx" /D "SH4" /D "_SH4_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /D "_MBCS" /D "_UNICODE" /YX /Qsh4r7 /Qs /Qfast /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "car - Win32 (WCE SH4) Release"
# Name "car - Win32 (WCE SH4) Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\car.cpp
DEP_CPP_CAR_C=\
	".\ALLcar.h"\
	".\car.h"\
	".\carbody.h"\
	".\carwheel.h"\
	{$(INCLUDE)}"archive.h"\
	{$(INCLUDE)}"archiveIface.h"\
	{$(INCLUDE)}"arcstack.h"\
	{$(INCLUDE)}"boundBox.h"\
	{$(INCLUDE)}"bs2all.h"\
	{$(INCLUDE)}"BS2consts.h"\
	{$(INCLUDE)}"bs2utils.h"\
	{$(INCLUDE)}"display.h"\
	{$(INCLUDE)}"engine.h"\
	{$(INCLUDE)}"material.h"\
	{$(INCLUDE)}"matrix3x4.h"\
	{$(INCLUDE)}"mesh.h"\
	{$(INCLUDE)}"model.h"\
	{$(INCLUDE)}"modelStore.h"\
	{$(INCLUDE)}"nongeodata.h"\
	{$(INCLUDE)}"object.h"\
	{$(INCLUDE)}"pointdata3d.h"\
	{$(INCLUDE)}"rgraphics.h"\
	{$(INCLUDE)}"shsgintr.h"\
	{$(INCLUDE)}"sphere.h"\
	{$(INCLUDE)}"tarc.h"\
	{$(INCLUDE)}"tarcf.h"\
	{$(INCLUDE)}"tarcman.h"\
	{$(INCLUDE)}"textureStore.h"\
	{$(INCLUDE)}"timing.h"\
	{$(INCLUDE)}"triangle.h"\
	{$(INCLUDE)}"vector3.h"\
	{$(INCLUDE)}"vertex.h"\
	{$(INCLUDE)}"xmulder.h"\
	
# End Source File
# Begin Source File

SOURCE=.\carbody.cpp
DEP_CPP_CARBO=\
	".\ALLcar.h"\
	".\car.h"\
	".\carbody.h"\
	".\carwheel.h"\
	{$(INCLUDE)}"archive.h"\
	{$(INCLUDE)}"archiveIface.h"\
	{$(INCLUDE)}"arcstack.h"\
	{$(INCLUDE)}"boundBox.h"\
	{$(INCLUDE)}"bs2all.h"\
	{$(INCLUDE)}"BS2consts.h"\
	{$(INCLUDE)}"bs2utils.h"\
	{$(INCLUDE)}"display.h"\
	{$(INCLUDE)}"engine.h"\
	{$(INCLUDE)}"material.h"\
	{$(INCLUDE)}"matrix3x4.h"\
	{$(INCLUDE)}"mesh.h"\
	{$(INCLUDE)}"model.h"\
	{$(INCLUDE)}"modelStore.h"\
	{$(INCLUDE)}"nongeodata.h"\
	{$(INCLUDE)}"object.h"\
	{$(INCLUDE)}"pointdata3d.h"\
	{$(INCLUDE)}"rgraphics.h"\
	{$(INCLUDE)}"shsgintr.h"\
	{$(INCLUDE)}"sphere.h"\
	{$(INCLUDE)}"tarc.h"\
	{$(INCLUDE)}"tarcf.h"\
	{$(INCLUDE)}"tarcman.h"\
	{$(INCLUDE)}"textureStore.h"\
	{$(INCLUDE)}"timing.h"\
	{$(INCLUDE)}"triangle.h"\
	{$(INCLUDE)}"vector3.h"\
	{$(INCLUDE)}"vertex.h"\
	{$(INCLUDE)}"xmulder.h"\
	
# End Source File
# Begin Source File

SOURCE=.\carwheel.cpp
DEP_CPP_CARWH=\
	".\ALLcar.h"\
	".\car.h"\
	".\carbody.h"\
	".\carwheel.h"\
	{$(INCLUDE)}"archive.h"\
	{$(INCLUDE)}"archiveIface.h"\
	{$(INCLUDE)}"arcstack.h"\
	{$(INCLUDE)}"boundBox.h"\
	{$(INCLUDE)}"bs2all.h"\
	{$(INCLUDE)}"BS2consts.h"\
	{$(INCLUDE)}"bs2utils.h"\
	{$(INCLUDE)}"display.h"\
	{$(INCLUDE)}"engine.h"\
	{$(INCLUDE)}"material.h"\
	{$(INCLUDE)}"matrix3x4.h"\
	{$(INCLUDE)}"mesh.h"\
	{$(INCLUDE)}"model.h"\
	{$(INCLUDE)}"modelStore.h"\
	{$(INCLUDE)}"nongeodata.h"\
	{$(INCLUDE)}"object.h"\
	{$(INCLUDE)}"pointdata3d.h"\
	{$(INCLUDE)}"rgraphics.h"\
	{$(INCLUDE)}"shsgintr.h"\
	{$(INCLUDE)}"sphere.h"\
	{$(INCLUDE)}"tarc.h"\
	{$(INCLUDE)}"tarcf.h"\
	{$(INCLUDE)}"tarcman.h"\
	{$(INCLUDE)}"textureStore.h"\
	{$(INCLUDE)}"timing.h"\
	{$(INCLUDE)}"triangle.h"\
	{$(INCLUDE)}"vector3.h"\
	{$(INCLUDE)}"vertex.h"\
	{$(INCLUDE)}"xmulder.h"\
	
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ALLcar.h
# End Source File
# Begin Source File

SOURCE=.\car.h
# End Source File
# Begin Source File

SOURCE=.\carbody.h
# End Source File
# Begin Source File

SOURCE=.\carwheel.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# End Target
# End Project
