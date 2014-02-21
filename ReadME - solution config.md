# Install myNana
1. go to [myNana master](https://nanapro.codeplex.com/SourceControl/network/forks/qPCR4vir/myNana/latest?branch=myNana_master)
2. Download/clone the branch myNana_master into your working directory of choise (for example: C:\Prog\ExtLib\Nana) 
3. Copy the whole dir Nana.Cpp11\build\vc2012\PropertySheet into the directory two level before the nana directory 
(to get somethig like: C:\Prog\PropertySheet). Two level because I use this PropertySheets for both my projects and for external projects or libraries.
4. Open C:\Prog\ExtLib\Nana\Nana.Cpp11\build\vc2012\nana.sln with MSVS2013. Now you can start to modify nana, or you can add a new project to write programs using nana.


Click on your solution in the solution explorer and go to:

VIEW/Others Windows/Property Manager

Here we have a list of all projects in the solution. 
"Inside" each project you see a list of all "named" configurations (MTrelease, etc.) in combination with a Platform (x64, Win32). 
"Inside" each of these combination we see an ordered list of "Property Sheet". Most of then are "built-in", defined by MSVS, but the higher are user defined (created and added by the user). These user defined Sheet are very useful, because you save it once and you can add it to any other configuration. Now you will need to change only one file or Sheet, and the change will correctly propagate for all configuration. 


Here I use:

PlatfDependName.PropertySheet :  (C:\Prog\PlatfDependName.PropertySheet.props)

base for any other of my Sheet (all other inherit it). It define a Macro:
 
 PlatfDependNameMod:      _VC$(PlatformToolset)_$(Configuration)_$(Platform)

it aims to Modify the output file name (the final result of the compilation), making it unique for each possible configuration. For example:

nana.C11_VCv120_MTrelease_x64.lib

Here:	nana.C11	- project name (modificable),
	_VC		- compiled using VC
	v120		- ToolSet, that is, the compiler version. Here the one with come with VC2013
	_MTrelease	- build configuration. Here Multi-threaded (/MT), static linked with the std
			  cpp library and without debug information, optimized for release. 
	_x64		- the platform.

The pros are: the linker can pick the correct lib to link, and will compile potentially faster, because don’t need to recompile all each time you just change the configuration. Also, I keep some of the version of the .exe to compare it. The contra is that some file/dir get big, and you will need to decide not to put it under Git or at least not to sent it to the remote repository.
Key changed:
Target Name: 			$(ProjectName)$(PlatfDependNameMod)
Intermediate Directory: 	..\..\..\temp\$(ProjectName)\$(Configuration)\$(Platform)\
Include Directories:		..\..\include;$(IncludePath)
Source Directories:		..\..\scr;$(SourcePath)


Lib.PropertySheet:   (C:\Prog\Lib.PropertySheet.props)
Key changed:
Output Directory:		..\..\lib\

Exe.PropertySheet.props	C:\Prog\Exe.PropertySheet.props
Key changed:
Output Directory:		..\..\bin\
Addnana.C11Lib.PropertySheet.props	C:\Prog\ExtLib\Addnana.C11Lib.PropertySheet.props
Key changed:
Include Directories:		..\..\..\ExtLib\Nana\Nana.Cpp11\include;$(IncludePath)
Source Directories:		..\..\..\ExtLib\Nana\Nana.Cpp11\source;$(SourcePath)
Library Directories:		..\..\..\ExtLib\Nana\lib;$(LibraryPath)
Additional Dependencies	nana.C11$(PlatfDependNameMod).lib;%(AdditionalDependencies)

C:\Prog\ThDySec\AddThDySevLib.PropertySheet.props
Key changed:
Source Directories:		..\..\scr\ThDySec;..\..\scr\ThDy_programs;$(SourcePath)
Library Directories:		..\..\lib;$(LibraryPath)
Additional Dependencies	ThDySec$(PlatfDependNameMod).lib;ThDy_programs$(PlatfDependNameMod).lib;%(AdditionalDependencies)





La solución tiene un juego de config.
Cada project tiene un juego de config. que se puede editar.
En el nombre de la config estoy poniendo el tipo de link (MD-dynamic, MT-static), release or debbug, y el ToolSet (version del compilador)
A cada projecto pener un dir temp y otro de bin. 
Ajustar los nombres del output y poner las lib de que depende. 


Temporales de mis projects    (exe & lib)   estan en C:\Prog\temp
Temporales de ExtLib (exe & lib, NANA,etc.) estan en C:\Prog\ExtLib\temp

Proj de lib NANA esta en C:\Prog\ExtLib\Nana\Nana.Cpp11\build\vc2012
  su temp debe estar en: C:\Prog\ExtLib\temp\nana\Debug\x64

Revisar Config Prop --> C/C++ --> CodeGenerat --> Run Time Lib: MD  multi-treaded DLL (d)
                                              -->               MT  multi-tread   (static) (d) 

For All configuration and all Platforms:





COnfiguration properties-->General:

Output Directory:
..\..\lib\
..\..\bin\
 
Intermediate Directory:
..\..\..\temp\$(ProjectName)\$(Configuration)\$(Platform)\
..\..\..\..\temp\$(ProjectName)\$(Configuration)\$(Platform)\    // Para NANA

Target Name:
$(ProjectName)_VC$(PlatformToolset)_$(Configuration)_$(Platform)   	



COnfiguration properties-->Debugging:

Command:
$(TargetPath)

Working Directory:

$(ProjectDir)				// lib




COnfiguration properties-->VC++ Directories:

Include dir:
..\..\include;..\..\..\ExtLib\Nana\Nana.Cpp11\include;$(IncludePath)

Lib dir:
..\..\lib\;..\..\..\ExtLib\Nana\Nana.Cpp11\build\lib;$(LibraryPath)

Source Dir:
..\..\..\ExtLib\Nana\Nana.Cpp11\source;..\..\scr\ThDySec;..\..\scr\ThDy_programs;$(SourcePath)



BORRADO POR SER "ADDITIONAL"

COnfiguration properties-->Librarian / Linker -->General:

Output File:
$(OutDir)$(TargetName)$(TargetExt)

Add Link Dep:
ThDySec_$(Configuration)_$(Platform).lib;ThDy_programs_$(Configuration)_$(Platform).lib;%(AdditionalDependencies)
..\..\lib\VC$(PlatformToolset);..\..\..\ExtLib\Nana\nana.win32\build\bin\vc11;$(LibraryPath)


..\..\..\ExtLib\Nana\Nana.Cpp11\build\lib;..\..\lib\

Output File
$(OutDir)$(TargetName)$(TargetExt)
