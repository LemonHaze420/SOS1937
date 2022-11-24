# Microsoft Developer Studio Project File - Name="archive" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (WCE SH4) Static Library" 0x8604

CFG=archive - Win32 (WCE SH4) Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "archive.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "archive.mak" CFG="archive - Win32 (WCE SH4) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "archive - Win32 (WCE SH4) Release" (based on "Win32 (WCE SH4) Static Library")
!MESSAGE "archive - Win32 (WCE SH4) Debug" (based on "Win32 (WCE SH4) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "archive"
# PROP Scc_LocalPath "."
# PROP WCE_FormatVersion "6.0"
CPP=shcl.exe

!IF  "$(CFG)" == "archive - Win32 (WCE SH4) Release"

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
# ADD CPP /nologo /MC /W3 /Zi /O2 /Ob2 /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "NDEBUG" /D "SHx" /D "SH4" /D "_SH4_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /D "_MBCS" /D "_UNICODE" /YX /Qsh4r7 /Qs /Qfast /Qgvp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"f:\sosgd\track5\archive.lib"

!ELSEIF  "$(CFG)" == "archive - Win32 (WCE SH4) Debug"

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
# ADD CPP /nologo /MC /W3 /Zi /Oi /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "DEBUG" /D "SHx" /D "SH4" /D "_SH4_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /D "_MBCS" /D "_UNICODE" /YX /Qsh4r7 /Qs /Qfast /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"f:\sosgd\track5\archive.lib"

!ENDIF 

# Begin Target

# Name "archive - Win32 (WCE SH4) Release"
# Name "archive - Win32 (WCE SH4) Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\archive.cpp
DEP_CPP_ARCHI=\
	".\archive.h"\
	".\arcstack.h"\
	".\tarc.h"\
	".\tarcf.h"\
	{$(INCLUDE)}"archiveIface.h"\
	{$(INCLUDE)}"boundBox.h"\
	{$(INCLUDE)}"BS2all.h"\
	{$(INCLUDE)}"BS2configuration.h"\
	{$(INCLUDE)}"BS2consts.h"\
	{$(INCLUDE)}"BS2features.h"\
	{$(INCLUDE)}"BS2simTypes.h"\
	{$(INCLUDE)}"BS2utils.h"\
	{$(INCLUDE)}"camera.h"\
	{$(INCLUDE)}"collide.h"\
	{$(INCLUDE)}"debugger.h"\
	{$(INCLUDE)}"debugmemlib.h"\
	{$(INCLUDE)}"display.h"\
	{$(INCLUDE)}"edge.h"\
	{$(INCLUDE)}"engine.h"\
	{$(INCLUDE)}"errorlib.h"\
	{$(INCLUDE)}"FloatMathLib.h"\
	{$(INCLUDE)}"Font.h"\
	{$(INCLUDE)}"grdebug.h"\
	{$(INCLUDE)}"hunk.h"\
	{$(INCLUDE)}"intrecip.h"\
	{$(INCLUDE)}"linkToTriangle.h"\
	{$(INCLUDE)}"material.h"\
	{$(INCLUDE)}"matrix3x4.h"\
	{$(INCLUDE)}"mesh.h"\
	{$(INCLUDE)}"model.h"\
	{$(INCLUDE)}"modelStore.h"\
	{$(INCLUDE)}"nongeodata.h"\
	{$(INCLUDE)}"object.h"\
	{$(INCLUDE)}"outLists.h"\
	{$(INCLUDE)}"outTriangle.h"\
	{$(INCLUDE)}"pointdata2d.h"\
	{$(INCLUDE)}"pointdata3d.h"\
	{$(INCLUDE)}"pool.h"\
	{$(INCLUDE)}"quaternion.h"\
	{$(INCLUDE)}"racingLine.h"\
	{$(INCLUDE)}"RboundBox.h"\
	{$(INCLUDE)}"rgraphics.h"\
	{$(INCLUDE)}"rollOverNumber.h"\
	{$(INCLUDE)}"shintr.h"\
	{$(INCLUDE)}"shsgintr.h"\
	{$(INCLUDE)}"SineTable.h"\
	{$(INCLUDE)}"sky.h"\
	{$(INCLUDE)}"sort.h"\
	{$(INCLUDE)}"sortlistentry.h"\
	{$(INCLUDE)}"Sphere.h"\
	{$(INCLUDE)}"spline.h"\
	{$(INCLUDE)}"spriteFX.h"\
	{$(INCLUDE)}"spriteFXentry.h"\
	{$(INCLUDE)}"tarcman.h"\
	{$(INCLUDE)}"textureStore.h"\
	{$(INCLUDE)}"TimeStats.h"\
	{$(INCLUDE)}"timing.h"\
	{$(INCLUDE)}"triangle.h"\
	{$(INCLUDE)}"vector2P.h"\
	{$(INCLUDE)}"vector3.h"\
	{$(INCLUDE)}"vertex.h"\
	{$(INCLUDE)}"world.h"\
	{$(INCLUDE)}"Xmulder.h"\
	
# End Source File
# Begin Source File

SOURCE=.\arcstack.cpp
DEP_CPP_ARCST=\
	".\archive.h"\
	".\arcstack.h"\
	".\tarc.h"\
	".\tarcf.h"\
	{$(INCLUDE)}"archiveIface.h"\
	{$(INCLUDE)}"boundBox.h"\
	{$(INCLUDE)}"BS2all.h"\
	{$(INCLUDE)}"BS2configuration.h"\
	{$(INCLUDE)}"BS2consts.h"\
	{$(INCLUDE)}"BS2features.h"\
	{$(INCLUDE)}"BS2simTypes.h"\
	{$(INCLUDE)}"BS2utils.h"\
	{$(INCLUDE)}"camera.h"\
	{$(INCLUDE)}"collide.h"\
	{$(INCLUDE)}"debugger.h"\
	{$(INCLUDE)}"debugmemlib.h"\
	{$(INCLUDE)}"display.h"\
	{$(INCLUDE)}"edge.h"\
	{$(INCLUDE)}"engine.h"\
	{$(INCLUDE)}"errorlib.h"\
	{$(INCLUDE)}"FloatMathLib.h"\
	{$(INCLUDE)}"Font.h"\
	{$(INCLUDE)}"grdebug.h"\
	{$(INCLUDE)}"hunk.h"\
	{$(INCLUDE)}"intrecip.h"\
	{$(INCLUDE)}"linkToTriangle.h"\
	{$(INCLUDE)}"material.h"\
	{$(INCLUDE)}"matrix3x4.h"\
	{$(INCLUDE)}"mesh.h"\
	{$(INCLUDE)}"model.h"\
	{$(INCLUDE)}"modelStore.h"\
	{$(INCLUDE)}"nongeodata.h"\
	{$(INCLUDE)}"object.h"\
	{$(INCLUDE)}"outLists.h"\
	{$(INCLUDE)}"outTriangle.h"\
	{$(INCLUDE)}"pointdata2d.h"\
	{$(INCLUDE)}"pointdata3d.h"\
	{$(INCLUDE)}"pool.h"\
	{$(INCLUDE)}"quaternion.h"\
	{$(INCLUDE)}"racingLine.h"\
	{$(INCLUDE)}"RboundBox.h"\
	{$(INCLUDE)}"rgraphics.h"\
	{$(INCLUDE)}"rollOverNumber.h"\
	{$(INCLUDE)}"shintr.h"\
	{$(INCLUDE)}"shsgintr.h"\
	{$(INCLUDE)}"SineTable.h"\
	{$(INCLUDE)}"sky.h"\
	{$(INCLUDE)}"sort.h"\
	{$(INCLUDE)}"sortlistentry.h"\
	{$(INCLUDE)}"Sphere.h"\
	{$(INCLUDE)}"spline.h"\
	{$(INCLUDE)}"spriteFX.h"\
	{$(INCLUDE)}"spriteFXentry.h"\
	{$(INCLUDE)}"tarcman.h"\
	{$(INCLUDE)}"textureStore.h"\
	{$(INCLUDE)}"TimeStats.h"\
	{$(INCLUDE)}"timing.h"\
	{$(INCLUDE)}"triangle.h"\
	{$(INCLUDE)}"vector2P.h"\
	{$(INCLUDE)}"vector3.h"\
	{$(INCLUDE)}"vertex.h"\
	{$(INCLUDE)}"world.h"\
	{$(INCLUDE)}"Xmulder.h"\
	
# End Source File
# Begin Source File

SOURCE=.\formats.c
DEP_CPP_FORMA=\
	".\tarcf.h"\
	
# End Source File
# Begin Source File

SOURCE=.\param.cpp
DEP_CPP_PARAM=\
	".\archive.h"\
	".\arcstack.h"\
	".\param.h"\
	".\tarcf.h"\
	
# End Source File
# Begin Source File

SOURCE=.\tarcr.c
DEP_CPP_TARCR=\
	".\tarc.h"\
	
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\archive.h
# End Source File
# Begin Source File

SOURCE=.\arcstack.h
# End Source File
# Begin Source File

SOURCE=.\param.h
# End Source File
# Begin Source File

SOURCE=.\tarc.h
# End Source File
# Begin Source File

SOURCE=.\tarcf.h
# End Source File
# Begin Source File

SOURCE=.\tarcman.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# End Target
# End Project
