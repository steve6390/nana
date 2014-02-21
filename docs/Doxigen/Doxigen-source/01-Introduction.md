# Nana C++ library.                {#index}  

[TOC]

\section WhatIs  What is Nana C++ Library? 

The Nana project is aimed at providing a software framework for cross-platform GUI programming, 
with commponents, such as graphics user interface and threads. Nana is a free/open-source 
library written in C++ with Object Oriented and Generic programing.

Nana is a C++ library, in Standard C++, that means it work great with modern C++ features, 
such as lambdas, concurrency and so on. It will give you wonderful experiences on GUI 
programming with C++11. Using its own widgets, drawing functions abstracted from the 
Nana abstract GUI system, it allows for writting programs on supported OS, such as Windows/Linux(X11).


Nana is a free/open-source library, it is distributed under *Boost Software License*, 
which is a lax, permissive non-copyleft free software license. Now, Nana is trying to 
build an active developer community to give help in using this library.

\section Install  Installation 

When you extract the library from its zip files, you have to install the library and configurate it before using. 
There is a general steps to install the library.
\subsection General General

#### Some terms for this instruction:
+ *NanaPath*: The path where Nana's files are located.
+ *IDEName*: The name of a certain Integrated Development Environments. Such as vc7.1, vc8.
+ *NanaStatic*: The generated Nana static linkage file, a *.lib/*.a file.
+ *BoostPath*: The path where Boost C++ Libraries are located.

#### Step One:
Start the IDE you use, set C++ include path and library path.
+ Include Path: *NanaPath*/include
+ Library Path: *NanaPath*/extrlib, *NanaPath*/build/bin/IDEName.

#### Step Two:
Create a static linkage library solution within a IDE which you use, and add all the files 
which are placed in *NanaPath*/source and in its all sub directiories to the solution. 
Then compile the solution and you will get a static linkage file *NanaStatic* in the path *NanaPath*/build/bin/IDEName.

#### Step Three:
Write a program and use Nana to create a GUI, and compile it with the *NanaStatic* file linked. 
Make sure that enable the compiler for supports of RTTI, MultiThreading and exception.

\subsection Microsoft Microsoft Visual C++
1. Set directories

	Add the Include Path *NanaPath*/include and the Library Path *NanaPath*/build/bin/IDEName and *NanaPath*/extrlib.
	#### Microsoft Visual C++ 2010/2012/2013
	Open the directory of "%UserProfile%/AppData/Local/Microsoft/MSBuild/v4.0", the %UserProfile% is a system environment variable. 
	Then you can find some files which are named like "Microsoft.CPP.XXX.user.props", choose one of them for your target platform, and edit it as follow:
	
	Insert "NanaPath/include" to the section <IncludePath>.
	
	Insert "NanaPath/build/bin/IDEName(e.g. vc10/vc2012/vc2013)" and "NanaPath/extrlib" to the section <LibraryPath>, and split them up with a semicolon.
	#### Microsoft Visual C++ 2003/2005/2008
	Click the menu *Tools->Options*, and then it shows a Options dialog. 
	
	Find  "Show directories for" which is in *Projects and Solutions->VC++ Directories*.

2. Compile Nana C++ Library
	
	Open "NanaPath/build/IDEName(e.g. vc7.1/vc8/vc9/vc10/vc2012/vc2013)/Nana.sln" with Visual C++. Click the menu Build->Rebuild All.  After building, a NanaStatic(e.g. nana_debug.lib, nana_release.lib) file should be generated in "NanaPath/build/bin/IDEName".

3. Programming with Nana C++ Library
	
	Create a Win32 Application Project, and make it as an empty project.
	
	Change properties of the project. Open the solution Properties Dialog.

	MDd and MTd for Debug Mode; MD and MT for Release Mode.
	Choosing the MD/MDd or MT/MTd depends on the property which is chose for building the Nana.

	Link the NanaStatic file.

	Input the name of NanaStatic file, such as nana_debug.lib or nana_release.lib.

\subsection Code Code::Blocks
1. Set directories
	
	Click menu Settings->Compiler... to open Global compiler settings dialog. Then, select Search directories->Compiler tab.

	Add "NanaPath/include".
	
	Add "/usr/include/freetype2" when it is in Linux. (The path of freetype2 may be different, it depends on your system).
	
	And then, switch to the "Linker" and add "NanaPath/build/codeblocks".

2. Compile Nana C++ Library

	MinGW
	Configure the Boost C++ Library for Nana.C++11(Skip this step if you are installing Nana.C++03)

	The Boost C++ Library is required due to <thread>, <mutex> and <condition_variable> being not supported by MinGW 4.7.x
	1. Insert the Boost includ directory to the "Settings->Compiler...->Search directories->Compiler".
	2. Append the MinGW path(e.g "C:\Program Files\CodeBlocks\MinGW\bin") to the "Path" which is one of Global Environment Variables.
	3. Open the "BoostPath/boost/thread/detail/config.hpp" header file and find the following lines.
    
			# if defined(BOOST_THREAD_PATH_WIN32)
			#      if defined(BOOST_MSVC) || defined(BOOST_INTEL_WIN)

		and modify

			# if defined(BOOST_THREAD_PATH_WIN32)
			#      if defined(BOOST_MSVC) || defined(BOOST_INTEL_WIN) || defined(__MINGW32__) || defined(MINGW32) || defined(BOOST_MINGW32)
	4. Compile the Boost.Thread
    
			bjam toolset=gcc --build-type=complete --with-thread
	5. Add the Boost C++ Libraries to the Code::Blocks.
		
		"Settings->Compiler...->Linker Settings" add libraries in the following order.
			
			libboost_system-mgwXX-mt-s-XXX.a
			libboost_thread-mgwXX-mt-s-XXX.a
			libboost_chrono-mgwXX-mt-s-XXX.a
		
		(Wrong order may cause "undefined reference" linker errors)

	Start to compile.
    
	Open the "NanaPath/build/codeblocks/nana.cbp" and build it. Don't forget 
	
		"-std=c++0x" or "-std=c++11" 
	
	when building the Nana.C++11.

3. Programming with Nana C++ Library
	Create a project, then open "Project->Build options...->Compiler settings->Compiler flags", and enable "-std=c++0x" or "-std=c++11" if it is Nana.C++11.
	Switch the tab to "Linker settings" in "Project build options", and link these static libraries in "Other linker options".
	##### Windows:
				- lnana -lgdi32 -lcomdlg32
	##### Linux:

				-nana -lX11 -lpthread -lrt -lXft
				-lpng (if the support of PNG is enabled)
				-lasound (if nana::audio::player is called)


\section Intro Introduction     
Nana is a C++ framework with provides GUI, threads and file system API. 
It provides many basic classes and functions for cross-platform programming.
Although C++ is a powerful and syntax-agile language, in fact, many programmers don't 
like to do GUI in C++, because it seems to be very difficult. Some existing C++ GUI 
frameworks define their own rule that forced you write some stiff code that always causes 
some problems, such as leaving your code in a deep inheritance hierarchy, making maintenance 
painful. Now, the Nana C++ Library is an alternative, a pure C++ library that will guide you 
in doing GUI with your immense and extensive C++ knowledge/skill/idioms. It makes a great 
progress in doing GUI in C++.

How easy is to create a Hello World program with Nana?

	#include <nana/gui/wvl.hpp>
	#include <nana/gui/widgets/label.hpp>

	int main()
	{
  		using namespace nana::gui;
  		form    fm;
  		label   lb(fm, fm.size());
 		lb.caption(STR("Hello, World"));
 		fm.show();
 		exec();
	}

Pretty easy, with understandable code. Nana brings very simple and reasonable concepts 
to keep it easy. Secondly, unlike other frameworks, that make you write stiff code due 
to name and syntax constraints, Nana can make your code more straightforward and readable. 
For example, answering an event.

	#include <nana/gui/wvl.hpp>
	#include <nana/gui/widgets/button.hpp>

	void clicked(const nana::gui::eventinfo&)
	{
		 //When the window  fm  is clicked, this function will be "called".
	}

	int main()
	{
 		using namespace nana::gui;
 		form fm;
 		fm.make_event<events::click>(clicked);
 		fm.show();
 		exec();
	}

The name of `clicked` function is not constrained. You can give it any other name you want. 
It is more straightforward than implementing an event handler from inheritance. 
In some situations, we don't need to care about the parameter of `clicked()` function, 
like in the example:

	void clicked() //NO parameter defined.
	{
 		//When the form is clicked, this function will be "called".
	}
	fm.make_event<events::click>(clicked);      //Nana allows!

Very flexible, and keep your code simple. And this feature can be applied with function object.

What makes Nana so flexible?

Nana C++ Library does not contain any "extra compilers" to parse "special syntax", 
Nana uses 100% C++ and the template techniques make this library very powerful and 
flexible. Nana is unlike other template-based library that causes a lot of code bloat 
and requires programmers have template-skilled, it's newbie-friendly.

Nana is a complete C++ style library that compile on Visual C++7.1/GCC 3.4 and later. 
If you are a C++ expert, Nana allows you to use lambda, a new feature of C++11, for event 
answering. Like this:

	fm.make_event<events::click>( []{	//When the form is clicked, the object  	
			           			        //created by this lambda will be "called".
 						            });

or

	fm.make_event<events::click>( [](const eventinfo& ei){ 
										//When the form is clicked, the object created
 										//by this lambda will be "called", and I can
 										//read the parameter.
 								   });

Additionally, Nana would make code flexible with `std::bind` in C++11.
\subsection Threading Threading 
Nana is a thread-safe library and accessing a widget between threads is normalized. 
This is a great feature that makes programmer deliver the event answer to other thread easier.

	#include <nana/gui/wvl.hpp>
	#include <nana/threads/pool.hpp>
	void foo()
	{
	  //This function will be "called" in other thread created by thread pool.
	}
	int main()
	{
	  using namespace nana::gui;
	  using namespace nana::threads;
 		pool thrpool;
 		form fm;
 		fm.make_event<events::click>(pool_push(thrpool, foo));
 		fm.make_event<events::click>(pool_push(thrpool, []{
 												  //A lambda is also allowed.
 										  }));
 		fm.show();
 		exec();
	}

\subsection RAII RAII  
There is a important feature as shown in above examples, as soon as a `form` object is created, 
its corresponding window is created, and the window is invisible till the `show()` is called 
to the `form` object, as soon as the `form` object is destructed, its corresponding window is 
closed, it conforms with the C++ object life-time concept.

\subsection Cross Cross-Platform Programming 
Nana C++ Library is designed to be used for cross-platform programming. 
Its first release is under Windows. Now, the library is basiclly ported to Linux(X11).

The Most Important Feature: Free

Nana C++ Library is an open-source; it's free for both commercial and non-commercial use.


\section Hello Hello World  

This lesson is indented to get you started programming with Nana.GUI. Let's read the simple HelloWorld code. 

	#include<nana/gui/wvl.hpp>
	#include <nana/gui/widgets/button.hpp>

	int main()
	 {
		 using namespace nana::gui;

		 form fm;
		 fm.caption(STR("Hello World"));
		 button btn(fm, nana::rectangle(20, 20, 150, 30));
		 btn.caption(STR("Quit"));
		 btn.make_event<events::click>(API::exit);
		 fm.show();
		 exec();
	 }

![Screenshot](Quit.jpg)

Walkthrough Line-by-line

	#include <nana/gui/wvl.hpp>

This line includes the Nana.GUI class definition. 

	#include <nana/gui/widgets/button.hpp>

This line includes the Nana.GUI.button class definition. 

	int main()
	{

The `main()` function is the entry point to the program. Almost always when using Nana.GUI, 
`main()` only needs to perform some kind of initialization before passing the control to the Nana.GUI 
library, which then tells the program about the user's actions via events.

    using namespace nana::gui;

Specify the nominated namespace nana::gui can be used in `main` function block scope. 
In this example, these names `form`, `button`, `events`, `API` and `exec` are defined in the namespace nana::gui.
With this using-directive, we could use these names directly in the main function scope.

	form fm;

This is the first piece of window-system code. A `form` is created while the variable `fm` is defined.
The `form` is a window with title bar and a sizable border frame, it's fundamental that you can put 
some widgets above it. 

	fm.caption(STR("Hello World"));

Set the `form` to display the text "Hello World" in its title bar. 

	button btn(fm, nana::rectangle(20, 20, 150, 30));

After the `form`, comes a button we created. In its constructor arg-list, the first argument tells 
the btn who the parent window is, and the following arguments describe position and size of btn. 

	btn.caption(STR("Quit"));

Set the btn to display the text "Quit". 

	btn.make_event<events::click>(API::exit);

make_event() is a method that every Nana.GUI widgets provide, you can register an event callback 
by using it. We want to exit the program while a mouse clicks on the btn. Now, register a callback
function on `click` event. 

	form.show();

A form widget is never visible when you create it. You must call show() to make it visible. 

	nana::gui::exec();

This is where the main() passes the control to Nana.GUI, and exec() will return when the 
application exists. In exec(), Nana.GUI processes the message loop and passes every event 
on to the appropriate widgets. 

	}

You should now save and compile the program.

Further

	STR("Hello World")

What is `STR`? `STR` is a macro that transforms a multi-byte string into wide-byte string 
if `NANA_UNICODE` is defined in *config.hpp*. With `STR` you can easier switch your program 
between multi-byte and wide-byte. 

	btn.make_event<nana::gui::events::click>(nana::gui::API::exit);

What is nana::gui::API::exit? This is an API provided by Nana.GUI. Its prototype is
void exit(). If exit() is called, Nana.GUI destroy all the windows you've created and 
the exec() will return. Member make_event() has a template argument. The argument can be a
function or a functor with a  `const nana::gui::eventinfo&` parameter or not. 

Is it right to invoke exit() in an event callback? 

It is right because Nana.GUI guarantees a program correctnets even when an invalid GUI objects handle is accesses .






\class nana::gui::listbox
	 
1. The resolver is used to resolute an object of the specified type for a listbox item.
2. The any_objective of listbox have a 2-Dimension indexing. The first dimension is for the category, and the second one is for the item of the specified category.

		int main()
		{
		   using namespace nana::gui;
		   form fm;
		   listbox lb(fm, nana::rectangle(10, 10, 280, 120));
		   lb.append_header(STR("Header"), 200);
		   lb.append_item(STR("int"));
		   lb.append_item(STR("double"));

		   lb.anyobj(0, 0, 10);
		   lb.anyobj(0, 1, 0.1);

		   int * pi = lb.anyobj<int>(0, 0); 	  // it returns a nullptr if there is not an int object specified.
		   double * pd = lb.anyobj<double>(0, 1); // it returns a nullptr if there is not an double object specified.

		   fm.show();
		   exec();
		 }

3. nana::gui::listbox creates the category 0 by default. The member functions without the categ parameter operate the items that belong to category 0.



\see nana::gui::drawerbase::listbox::cat_proxy
\see nana::gui::drawerbase::listbox::item_proxy








\class nana::gui::menu
\brief  a list of items that specify options or group of options for an application.

1. The widget sets the shortkey that is a character behind the first of &-character in text for the item. e.g. "File(&F)" or "&File".
2. The type item_proxy is used for callbacking. A programmer should not take care about the object item_proxy. It is created and destroyed by menu. The definition is

		class item_proxy: nana::noncopyable
		{
		  public:
			implementation-specified constructor
			void enabled(bool); 			//Sets the enable state of the item.
			bool enabled() const; 		//Gets the enable state of the item.
			std::size_t index() const; 	//Gets the index of the item.
		  private:
			//Private data members...
		};

3. There is a helper for automatically popping a menu.

		class menu_popuper
		{
		  public:
					//C++03
			menu_popuper(menu&, mouse::t = mouse::right_button);
			menu_popuper(menu&, window owner, const point& pos, 
			mouse::t =  mouse::right_button	);

					//C++11
			menu_popuper(menu&, mouse = mouse::right_button);
			menu_popuper(menu&, window owner, const point& pos, mouse = mouse::right_button);

			void operator()(const eventinfo&);
		private:
			//Implemented-Specified private members
		};

	Now let's use it. There is a button, it popups the menu when it is clicked.

		#include <nana/gui/wvl.hpp>
		#include <nana/gui/widgets/button.hpp>
		#include <nana/gui/widgets/menu.hpp>

		void on_menu_item(nana::gui::menu::item_proxy& ip)
		{
			 std::size_t index = ip.index(); //Get the index of the clicking item.
		}

		int main()
		{
			using namespace nana::gui;
			form fm;

			//We need a menu object
			menu mobj;
			mobj.append ( STR("Item 0"), &on_menu_item);
			mobj.append_splitter();
			mobj.append ( STR("Item 1"), &on_menu_item);

			//Now we need a button.
			button btn(fm, nana::rectangle(nana::point(10, 10), nana::size(100, 25)));
			btn.caption(STR("Popup Menu"));

			//Popup the menu when right clicking the button.
			btn.make_event<events::click>(menu_popuper(mobj));

			//Or popuping the menu with a specified coordinate when any mouse button is clicked.
			//btn.make_event<events::click> ( menu_popuper( mobj, btn, nana::point(0, 26),
			//                                mouse::any_button );

			fm.show();
			exec();
		}

		
\example subclass.cpp

\example calculator.cpp

\example demo.cpp11.cpp

