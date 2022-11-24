# Microsoft Developer Studio Project File - Name="boris2a" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (WCE SH4) Static Library" 0x8604

CFG=boris2a - Win32 (WCE SH4) Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "boris2a.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "boris2a.mak" CFG="boris2a - Win32 (WCE SH4) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "boris2a - Win32 (WCE SH4) Release" (based on "Win32 (WCE SH4) Static Library")
!MESSAGE "boris2a - Win32 (WCE SH4) Debug" (based on "Win32 (WCE SH4) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "boris2a"
# PROP Scc_LocalPath "."
# PROP WCE_FormatVersion "6.0"
CPP=shcl.exe

!IF  "$(CFG)" == "boris2a - Win32 (WCE SH4) Release"

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

!ELSEIF  "$(CFG)" == "boris2a - Win32 (WCE SH4) Debug"

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
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "boris2a - Win32 (WCE SH4) Release"
# Name "boris2a - Win32 (WCE SH4) Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\archiveIface.cpp
DEP_CPP_ARCHI=\
	".\archiveIface.h"\
	".\boundBox.h"\
	".\bs2all.h"\
	".\BS2consts.h"\
	".\bs2utils.h"\
	".\camera.h"\
	".\display.h"\
	".\engine.h"\
	".\material.h"\
	".\matrix3x4.h"\
	".\mesh.h"\
	".\model.h"\
	".\modelStore.h"\
	".\nongeodata.h"\
	".\object.h"\
	".\pointdata3d.h"\
	".\sphere.h"\
	".\textureStore.h"\
	".\timing.h"\
	".\triangle.h"\
	".\vector3.h"\
	".\vertex.h"\
	".\xmulder.h"\
	{$(INCLUDE)}"archive.h"\
	{$(INCLUDE)}"arcstack.h"\
	{$(INCLUDE)}"rgraphics.h"\
	{$(INCLUDE)}"shsgintr.h"\
	{$(INCLUDE)}"tarc.h"\
	{$(INCLUDE)}"tarcf.h"\
	{$(INCLUDE)}"tarcman.h"\
	
# End Source File
# Begin Source File

SOURCE=.\bs2utils.cpp
DEP_CPP_BS2UT=\
	".\archiveIface.h"\
	".\boundBox.h"\
	".\bs2all.h"\
	".\BS2consts.h"\
	".\bs2utils.h"\
	".\camera.h"\
	".\display.h"\
	".\engine.h"\
	".\material.h"\
	".\matrix3x4.h"\
	".\mesh.h"\
	".\model.h"\
	".\modelStore.h"\
	".\nongeodata.h"\
	".\object.h"\
	".\pointdata3d.h"\
	".\sphere.h"\
	".\textureStore.h"\
	".\timing.h"\
	".\triangle.h"\
	".\vector3.h"\
	".\vertex.h"\
	".\xmulder.h"\
	{$(INCLUDE)}"archive.h"\
	{$(INCLUDE)}"arcstack.h"\
	{$(INCLUDE)}"rgraphics.h"\
	{$(INCLUDE)}"shsgintr.h"\
	{$(INCLUDE)}"tarc.h"\
	{$(INCLUDE)}"tarcf.h"\
	{$(INCLUDE)}"tarcman.h"\
	
# End Source File
# Begin Source File

SOURCE=.\camera.cpp
DEP_CPP_CAMER=\
	".\archiveIface.h"\
	".\boundBox.h"\
	".\bs2all.h"\
	".\BS2consts.h"\
	".\bs2utils.h"\
	".\camera.h"\
	".\display.h"\
	".\engine.h"\
	".\material.h"\
	".\matrix3x4.h"\
	".\mesh.h"\
	".\model.h"\
	".\modelStore.h"\
	".\nongeodata.h"\
	".\object.h"\
	".\pointdata3d.h"\
	".\sphere.h"\
	".\textureStore.h"\
	".\timing.h"\
	".\triangle.h"\
	".\vector3.h"\
	".\vertex.h"\
	".\xmulder.h"\
	{$(INCLUDE)}"archive.h"\
	{$(INCLUDE)}"arcstack.h"\
	{$(INCLUDE)}"rgraphics.h"\
	{$(INCLUDE)}"shsgintr.h"\
	{$(INCLUDE)}"tarc.h"\
	{$(INCLUDE)}"tarcf.h"\
	{$(INCLUDE)}"tarcman.h"\
	
# End Source File
# Begin Source File

SOURCE=.\engine.cpp
DEP_CPP_ENGIN=\
	".\archiveIface.h"\
	".\boundBox.h"\
	".\bs2all.h"\
	".\BS2consts.h"\
	".\bs2utils.h"\
	".\camera.h"\
	".\display.h"\
	".\engine.h"\
	".\material.h"\
	".\matrix3x4.h"\
	".\mesh.h"\
	".\model.h"\
	".\modelStore.h"\
	".\nongeodata.h"\
	".\object.h"\
	".\pointdata3d.h"\
	".\sphere.h"\
	".\textureStore.h"\
	".\timing.h"\
	".\triangle.h"\
	".\vector3.h"\
	".\vertex.h"\
	".\xmulder.h"\
	{$(INCLUDE)}"archive.h"\
	{$(INCLUDE)}"arcstack.h"\
	{$(INCLUDE)}"rgraphics.h"\
	{$(INCLUDE)}"shsgintr.h"\
	{$(INCLUDE)}"tarc.h"\
	{$(INCLUDE)}"tarcf.h"\
	{$(INCLUDE)}"tarcman.h"\
	
# End Source File
# Begin Source File

SOURCE=.\matrix3x4.cpp
DEP_CPP_MATRI=\
	".\archiveIface.h"\
	".\boundBox.h"\
	".\bs2all.h"\
	".\BS2consts.h"\
	".\bs2utils.h"\
	".\camera.h"\
	".\display.h"\
	".\engine.h"\
	".\material.h"\
	".\matrix3x4.h"\
	".\mesh.h"\
	".\model.h"\
	".\modelStore.h"\
	".\nongeodata.h"\
	".\object.h"\
	".\pointdata3d.h"\
	".\sphere.h"\
	".\textureStore.h"\
	".\timing.h"\
	".\triangle.h"\
	".\vector3.h"\
	".\vertex.h"\
	".\xmulder.h"\
	{$(INCLUDE)}"archive.h"\
	{$(INCLUDE)}"arcstack.h"\
	{$(INCLUDE)}"rgraphics.h"\
	{$(INCLUDE)}"shsgintr.h"\
	{$(INCLUDE)}"tarc.h"\
	{$(INCLUDE)}"tarcf.h"\
	{$(INCLUDE)}"tarcman.h"\
	
# End Source File
# Begin Source File

SOURCE=.\mesh.cpp
DEP_CPP_MESH_=\
	".\archiveIface.h"\
	".\boundBox.h"\
	".\bs2all.h"\
	".\BS2consts.h"\
	".\bs2utils.h"\
	".\camera.h"\
	".\display.h"\
	".\engine.h"\
	".\material.h"\
	".\matrix3x4.h"\
	".\mesh.h"\
	".\model.h"\
	".\modelStore.h"\
	".\nongeodata.h"\
	".\object.h"\
	".\pointdata3d.h"\
	".\sphere.h"\
	".\textureStore.h"\
	".\timing.h"\
	".\triangle.h"\
	".\vector3.h"\
	".\vertex.h"\
	".\xmulder.h"\
	{$(INCLUDE)}"archive.h"\
	{$(INCLUDE)}"arcstack.h"\
	{$(INCLUDE)}"rgraphics.h"\
	{$(INCLUDE)}"shsgintr.h"\
	{$(INCLUDE)}"tarc.h"\
	{$(INCLUDE)}"tarcf.h"\
	{$(INCLUDE)}"tarcman.h"\
	
# End Source File
# Begin Source File

SOURCE=.\model.cpp
DEP_CPP_MODEL=\
	".\archiveIface.h"\
	".\boundBox.h"\
	".\bs2all.h"\
	".\BS2consts.h"\
	".\bs2utils.h"\
	".\camera.h"\
	".\display.h"\
	".\engine.h"\
	".\material.h"\
	".\matrix3x4.h"\
	".\mesh.h"\
	".\model.h"\
	".\modelStore.h"\
	".\nongeodata.h"\
	".\object.h"\
	".\pointdata3d.h"\
	".\sphere.h"\
	".\textureStore.h"\
	".\timing.h"\
	".\triangle.h"\
	".\vector3.h"\
	".\vertex.h"\
	".\xmulder.h"\
	{$(INCLUDE)}"archive.h"\
	{$(INCLUDE)}"arcstack.h"\
	{$(INCLUDE)}"rgraphics.h"\
	{$(INCLUDE)}"shsgintr.h"\
	{$(INCLUDE)}"tarc.h"\
	{$(INCLUDE)}"tarcf.h"\
	{$(INCLUDE)}"tarcman.h"\
	
# End Source File
# Begin Source File

SOURCE=.\modelstore.cpp
DEP_CPP_MODELS=\
	".\archiveIface.h"\
	".\boundBox.h"\
	".\bs2all.h"\
	".\BS2consts.h"\
	".\bs2utils.h"\
	".\camera.h"\
	".\display.h"\
	".\engine.h"\
	".\material.h"\
	".\matrix3x4.h"\
	".\mesh.h"\
	".\model.h"\
	".\modelStore.h"\
	".\nongeodata.h"\
	".\object.h"\
	".\pointdata3d.h"\
	".\sphere.h"\
	".\textureStore.h"\
	".\timing.h"\
	".\triangle.h"\
	".\vector3.h"\
	".\vertex.h"\
	".\xmulder.h"\
	{$(INCLUDE)}"archive.h"\
	{$(INCLUDE)}"arcstack.h"\
	{$(INCLUDE)}"rgraphics.h"\
	{$(INCLUDE)}"shsgintr.h"\
	{$(INCLUDE)}"tarc.h"\
	{$(INCLUDE)}"tarcf.h"\
	{$(INCLUDE)}"tarcman.h"\
	
# End Source File
# Begin Source File

SOURCE=.\object.cpp
DEP_CPP_OBJEC=\
	".\archiveIface.h"\
	".\boundBox.h"\
	".\bs2all.h"\
	".\BS2consts.h"\
	".\bs2utils.h"\
	".\camera.h"\
	".\display.h"\
	".\engine.h"\
	".\material.h"\
	".\matrix3x4.h"\
	".\mesh.h"\
	".\model.h"\
	".\modelStore.h"\
	".\nongeodata.h"\
	".\object.h"\
	".\pointdata3d.h"\
	".\sphere.h"\
	".\textureStore.h"\
	".\timing.h"\
	".\triangle.h"\
	".\vector3.h"\
	".\vertex.h"\
	".\xmulder.h"\
	{$(INCLUDE)}"archive.h"\
	{$(INCLUDE)}"arcstack.h"\
	{$(INCLUDE)}"rgraphics.h"\
	{$(INCLUDE)}"shsgintr.h"\
	{$(INCLUDE)}"tarc.h"\
	{$(INCLUDE)}"tarcf.h"\
	{$(INCLUDE)}"tarcman.h"\
	
# End Source File
# Begin Source File

SOURCE=.\sphere.cpp
DEP_CPP_SPHER=\
	".\archiveIface.h"\
	".\boundBox.h"\
	".\bs2all.h"\
	".\BS2consts.h"\
	".\bs2utils.h"\
	".\camera.h"\
	".\display.h"\
	".\engine.h"\
	".\material.h"\
	".\matrix3x4.h"\
	".\mesh.h"\
	".\model.h"\
	".\modelStore.h"\
	".\nongeodata.h"\
	".\object.h"\
	".\pointdata3d.h"\
	".\sphere.h"\
	".\textureStore.h"\
	".\timing.h"\
	".\triangle.h"\
	".\vector3.h"\
	".\vertex.h"\
	".\xmulder.h"\
	{$(INCLUDE)}"archive.h"\
	{$(INCLUDE)}"arcstack.h"\
	{$(INCLUDE)}"rgraphics.h"\
	{$(INCLUDE)}"shsgintr.h"\
	{$(INCLUDE)}"tarc.h"\
	{$(INCLUDE)}"tarcf.h"\
	{$(INCLUDE)}"tarcman.h"\
	
# End Source File
# Begin Source File

SOURCE=.\texturestore.cpp
DEP_CPP_TEXTU=\
	".\archiveIface.h"\
	".\boundBox.h"\
	".\bs2all.h"\
	".\BS2consts.h"\
	".\bs2utils.h"\
	".\camera.h"\
	".\display.h"\
	".\engine.h"\
	".\material.h"\
	".\matrix3x4.h"\
	".\mesh.h"\
	".\model.h"\
	".\modelStore.h"\
	".\nongeodata.h"\
	".\object.h"\
	".\pointdata3d.h"\
	".\sphere.h"\
	".\textureStore.h"\
	".\timing.h"\
	".\triangle.h"\
	".\vector3.h"\
	".\vertex.h"\
	".\xmulder.h"\
	{$(INCLUDE)}"archive.h"\
	{$(INCLUDE)}"arcstack.h"\
	{$(INCLUDE)}"rgraphics.h"\
	{$(INCLUDE)}"shsgintr.h"\
	{$(INCLUDE)}"tarc.h"\
	{$(INCLUDE)}"tarcf.h"\
	{$(INCLUDE)}"tarcman.h"\
	
# End Source File
# Begin Source File

SOURCE=.\timing.cpp
DEP_CPP_TIMIN=\
	".\archiveIface.h"\
	".\boundBox.h"\
	".\bs2all.h"\
	".\BS2consts.h"\
	".\bs2utils.h"\
	".\camera.h"\
	".\display.h"\
	".\engine.h"\
	".\material.h"\
	".\matrix3x4.h"\
	".\mesh.h"\
	".\model.h"\
	".\modelStore.h"\
	".\nongeodata.h"\
	".\object.h"\
	".\pointdata3d.h"\
	".\sphere.h"\
	".\textureStore.h"\
	".\timing.h"\
	".\triangle.h"\
	".\vector3.h"\
	".\vertex.h"\
	".\xmulder.h"\
	{$(INCLUDE)}"archive.h"\
	{$(INCLUDE)}"arcstack.h"\
	{$(INCLUDE)}"rgraphics.h"\
	{$(INCLUDE)}"shsgintr.h"\
	{$(INCLUDE)}"tarc.h"\
	{$(INCLUDE)}"tarcf.h"\
	{$(INCLUDE)}"tarcman.h"\
	
# End Source File
# Begin Source File

SOURCE=.\vector3.cpp
DEP_CPP_VECTO=\
	".\archiveIface.h"\
	".\boundBox.h"\
	".\bs2all.h"\
	".\BS2consts.h"\
	".\bs2utils.h"\
	".\camera.h"\
	".\display.h"\
	".\engine.h"\
	".\material.h"\
	".\matrix3x4.h"\
	".\mesh.h"\
	".\model.h"\
	".\modelStore.h"\
	".\nongeodata.h"\
	".\object.h"\
	".\pointdata3d.h"\
	".\sphere.h"\
	".\textureStore.h"\
	".\timing.h"\
	".\triangle.h"\
	".\vector3.h"\
	".\vertex.h"\
	".\xmulder.h"\
	{$(INCLUDE)}"archive.h"\
	{$(INCLUDE)}"arcstack.h"\
	{$(INCLUDE)}"rgraphics.h"\
	{$(INCLUDE)}"shsgintr.h"\
	{$(INCLUDE)}"tarc.h"\
	{$(INCLUDE)}"tarcf.h"\
	{$(INCLUDE)}"tarcman.h"\
	
# End Source File
# Begin Source File

SOURCE=.\xmulder.cpp
DEP_CPP_XMULD=\
	".\archiveIface.h"\
	".\boundBox.h"\
	".\bs2all.h"\
	".\BS2consts.h"\
	".\bs2utils.h"\
	".\camera.h"\
	".\display.h"\
	".\engine.h"\
	".\material.h"\
	".\matrix3x4.h"\
	".\mesh.h"\
	".\model.h"\
	".\modelStore.h"\
	".\nongeodata.h"\
	".\object.h"\
	".\pointdata3d.h"\
	".\sphere.h"\
	".\textureStore.h"\
	".\timing.h"\
	".\triangle.h"\
	".\vector3.h"\
	".\vertex.h"\
	".\xmulder.h"\
	{$(INCLUDE)}"archive.h"\
	{$(INCLUDE)}"arcstack.h"\
	{$(INCLUDE)}"rgraphics.h"\
	{$(INCLUDE)}"shsgintr.h"\
	{$(INCLUDE)}"tarc.h"\
	{$(INCLUDE)}"tarcf.h"\
	{$(INCLUDE)}"tarcman.h"\
	
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\archiveIface.h
# End Source File
# Begin Source File

SOURCE=.\boundBox.h
# End Source File
# Begin Source File

SOURCE=.\bs2all.h
# End Source File
# Begin Source File

SOURCE=.\BS2consts.h
# End Source File
# Begin Source File

SOURCE=.\bs2utils.h
# End Source File
# Begin Source File

SOURCE=.\camera.h
# End Source File
# Begin Source File

SOURCE=.\display.h
# End Source File
# Begin Source File

SOURCE=.\engine.h
# End Source File
# Begin Source File

SOURCE=.\material.h
# End Source File
# Begin Source File

SOURCE=.\matrix3x4.h
# End Source File
# Begin Source File

SOURCE=.\mesh.h
# End Source File
# Begin Source File

SOURCE=.\model.h
# End Source File
# Begin Source File

SOURCE=.\modelStore.h
# End Source File
# Begin Source File

SOURCE=.\nongeodata.h
# End Source File
# Begin Source File

SOURCE=.\object.h
# End Source File
# Begin Source File

SOURCE=.\pointdata3d.h
# End Source File
# Begin Source File

SOURCE=.\sphere.h
# End Source File
# Begin Source File

SOURCE=.\textureStore.h
# End Source File
# Begin Source File

SOURCE=.\timing.h
# End Source File
# Begin Source File

SOURCE=.\triangle.h
# End Source File
# Begin Source File

SOURCE=.\vector3.h
# End Source File
# Begin Source File

SOURCE=.\vertex.h
# End Source File
# Begin Source File

SOURCE=.\xmulder.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# End Target
# End Project
