# Install myNana
1. go to [myNana master](https://nanapro.codeplex.com/SourceControl/network/forks/qPCR4vir/myNana)
2. Download or clone the branch master into your working directory of choise (for example: C:\Prog\ExtLib\Nana) 
3. Copy the whole dir Nana.Cpp11\build\PropertySheet into the directory two level before the nana directory 
(to get something like: C:\Prog\PropertySheet). Two level because I use this PropertySheets for both my projects and for external projects or libraries.
4. Open C:\Prog\ExtLib\Nana\Nana.Cpp11\build\vc2012\nana.sln with MSVS2013. 
5. Now you can start to modify nana, or you can: 
6. Add a new empty project to write programs using nana in a directory at the same level as for nana (for example: C:\Prog\ExtLib\Nana\demo_projects\demo_projects.vcxproj). 
6. Go to VIEW/Others Windows/Property Manager and add to the new project the two PropertySheets: 
	 \PropertySheet\Addnana.C11Lib.PropertySheet.props    (this must be up)
	 \PropertySheet\Exe.PropertySheet.props        (this first. If you want a lib add instead: Lib.PropertySheet.props)
6. Install [Doxygen GUI](https://sourceforge.net/projects/doxygen/files/). Generate Nana reference documentation with 
[C:\Prog\ExtLib\Nana\docs\Doxigen\Nana.Cpp11.forDevelopers.Doxyfile] or [C:\Prog\ExtLib\Nana\docs\Doxigen\Nana.Cpp11.forUsers.Doxyfile].

(..\..\ExtLib\Nana\docs\Doxigen\Nana.Cpp11.forDevelopers.Doxyfile) 
or(..\..\ExtLib\Nana\docs\Doxigen\Nana.Cpp11.forUsers.Doxyfile).

