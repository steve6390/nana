# 1- Nana C++ library. #     {#mainpage}  

## What is Nana C++ Library? ##

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

## Introduction     #
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
### Threading###
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

### RAII ###
There is a important feature as shown in above examples, as soon as a `form` object is created, 
its corresponding window is created, and the window is invisible till the `show()` is called 
to the `form` object, as soon as the `form` object is destructed, its corresponding window is 
closed, it conforms with the C++ object life-time concept.

### Cross-Platform Programming ###
Nana C++ Library is designed to be used for cross-platform programming. 
Its first release is under Windows. Now, the library is basiclly ported to Linux(X11).

The Most Important Feature: Free

Nana C++ Library is an open-source; it's free for both commercial and non-commercial use.




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


