
# 3- Getting Started #
This chapter shows how to create a small graphical user interface (GUI) application with Nana C++ Library. 
## 3.1. Hello Nana ##
Let's start with a simple program, we will study it line by line. 

	1 #include <nana/gui/wvl.hpp> 
	2 #include <nana/gui/widgets/label.hpp> 
	3 int main() 
	4 { 
	5 	nana::gui::form form; 
	6 	nana::gui::label label(form, 0, 0, 100, 20); 
	7 	label.caption(STR("Hello Nana")); 
	8 	form.show(); 
	9 	nana::gui::exec(); 
	  } 

Lines 1 and 2 include the definitions of class `form` and class `label` in namespace nana::gui. 
Every GUI application written with Nana C++ Library must include the header file nana/gui/wvl.hpp. 

Line 5 defines a nana::gui::form object. It is a window used for placing a label widget in this example. 

Line 6 defines a nana::gui::label object to display a text string. The label object is created in the form. A widget is a visual element in the user interface. 

Line 7 sets the caption of label object. Every widget has a caption for displaying a title. In this line, there is a string within a macro named STR, 
this macro is a string wrapper used for easy switch the application between UNICODE and ASCII version. 

Line 8 makes the form visible. 

Line 9 passes the control of the application on to Nana. At this point, the program enters the event loop for waiting for
and receiving a user action, such as mouse move, mouse click and keyboard press. The function nana::gui::exec blocks till
form is destroyed, and the example demonstrate is that program exits when a user closes the window. 

![Figure 1.1 Hello Nana](HelloNana.jpg)

Now, you can run the program on your own machine. But firstly you should have Nana C++ Library 
installed in your system. A method to install is explained in Installation Library Documentation 
in Nana Programmer`s Guide. 

## 3.2. Making An Event ##
The second example shows how to respond a user action. To receive a user action, an event 
handler should be registered to a widget. Nana waits for a user action and invokes the event 
handler of corresponding event. The application consists of a button that the user can click to quit. 

	#include <nana/gui/wvl.hpp> 
	#include <nana/gui/widgets/button.hpp> 
	int main() 
	{ 
		using namespace nana::gui; 
		form fm; 
		button btn(fm, 0, 0, 100, 20); 
		btn.caption(STR("Quit")); 
		btn.make_event<events::click>(API::exit); 
		fm.show(); 
		exec(); 
	} 

This source code is similar to Hello Nana, except that we are using a button instead of a label, and 
we are making an event to respond a user click. Every widget class has a set of methods to make events, 
they are named "make_event". This is a function template. The first template parameter specifie what 
event must be given explicitly when calling the function, and the parameter of the member function is an 
event handler. As above code shown, we make the API::exit() as an event handler for the button's click event. 
The exit() function is an API in Nana C++ Library. It closes all windows in current GUI thread and terminates 
the event loop. It will be called when the user clicks the button. 

![Figure 1.2 Event Example](Quit.jpg)
 

# 3a.1 Function Objects #

This chapter shows two basic concepts: function object and lambda. Function object is a requirement to understand Nana C++ Library. 

A Function Object, or Functor (the two terms are synonymous) is simply any object that can be called as 
if it is a function, more generally, so is an object of a class that defines function call operator(). 

The function object is an impressive technology. A function object is a more general concept than a function 
because a function object can have state that persist across several calls and can be initialized and examined 
from outside the object, unlike a static local variable. For example: 

	class sum 
	{ public: 
		sum() : i_(0) {} 
		operator int() const volatile { return i_; } 
					   //this makes the objects of this class can be used like a function. 
		void operator()(int x) volatile { i_ += x; } 
	  private: 
		int i_; 
	}; 

	void foo(const std::vector<int>& v) 
	{ 			//gather the sum of all elements. 
	   std::cout<<std::for_each(v.begin(), v.end(), sum())<<std::endl; 
	} 

std::for_each() returns a copy of the object of sum, and we are able to retrieve the state or result. 
On the basis of this feature that function objects retain its own state, it is easy used for concurrency 
process, and it is extensively used for providing flexibility in the library implementation. 

Nana C++ Library uses a large number of function objects to make the framework work. 
To make the framework flexible enough, Nana C++ Library introduced a general functor class template: 

	template<typename Ftype> class nana::functor; 

The template parameter Ftype specify what function type the functor delegates. By using the functor 
class template, we are able to make Nana C++ Library get rid of types that involved. For example: 

	#include <nana/gui/wvl.hpp> 
	#include <iostream> 
	void foo() 
	{ 
		std::system("cls"); 
		std::cout<<"foo"<<std::endl; 
	} 
	void foo_with_eventinfo(const nana::gui::eventinfo& ei) 
	{ 
	  std::cout << "foo_with_eventinfo, mouse pos = ("
	   << ei.mouse.x  << ", "  <<  ei.mouse.y  << ")" << std::endl; 
	} 
	class click_stat 
	{ public: 
		click_stat(): n_(0) {} 
		void respond   ()   { std::cout<<"click_stat = "<<++n_<<std::endl; } 
		void respond_ei(const nana::gui::eventinfo& ei) 
		{ 
			 std::cout << "click_state width eventinfo = " << n_ 
			<< ", mouse pos = ("  <<ei.mouse.x<<", "<<ei.mouse.y<<")"
			<< std::endl; 
		} 
	 private: 
	   int n_; 
	}; 
	int main() 
	{ 
		using namespace nana::gui; 
		typedef nana::functor<void()> 				    fun_t; 
		typedef nana::functor<void(const eventinfo&)> 	fun_with_param_t; 
		form 		fm; 
		click_stat 	cs; 
		fun_t 		f(foo); 
		fm.make_event<events::click>(f); 
		f = fun_t(cs, &click_stat::respond); 
		fm.make_event<events::click>(f); 
		fun_with_param_t fp(foo_with_eventinfo); 
		fm.make_event<events::click>(fp); 
		fp = fun_with_param_t(cs, &click_stat::respond_ei); 
		fm.make_event<events::click>(fp); 
		fm.show(); 
		exec(); 
	} 

There are four different types of event handlers that can be processed by using nana::functor 
class template. It is flexible and reduce the complexity of study and use. 

![Figure 2.1 Various methods to make events to respond click.] (make_event.jpg)

In the previous example, we illustrated the use of nana::functor and the flexibility of using a 
function object. Practically, creating a nana::functor object is not required. Using these 
functions this way instead of creating a nana::functor: 

	int main() 
	{ 
		using namespace nana::gui; 
		form fm; 
		click_stat cs; 
		fm.make_event<events::click>(foo); 
		fm.make_event<events::click>(cs, &click_stat::respond); 
		fm.make_event<events::click>(foo_with_eventinfo); 
		fm.make_event<events::click>(cs, &click_stat::respond_ei); 
		fm.show(); 
		exec(); 
	} 

## 2.1, Predefined Function Objects ##

Nana C++ Library includes many different predefined function objects. Using these 
function objects together with function templates increases the expressive power of 
the library as well as making the resulting code much more efficient. For example, if
a C++ program wants to close the form when the form is being clicked. 

	form.make_event<events::click>(destroy(form)); 

Please include <nana/gui/functional.hpp> before using these function objects. 

	class destroy 
	{ public: 
		destroy(nana::gui::window wd); 
		void operator()() const; 
	}; 

Destroy the window. 

	class hide 
	{ public: 
		hide(nana::gui::window wd); 
		void operator()() const; 
	}; 

Hide the window.
 
	class show 
	{ public: 
		show(nana::gui::window wd); 
		void operator()() const; 
	}; 

Show the window. 

## 2.2, The Lambda Expression ##

A lambda expression is a mechanism for specifying a function object. The lambda 
is a new feature that introduced into C++ language recently, the primary use for a 
lambda is to specify a simple action to be performed by some functions. For example: 

	#include <nana/gui/wvl.hpp> 
	#include <iostream> 
	int main() 
	{ 
		nana::gui::form form; 
		form.make_event<nana::gui::events::click>( 
			[]{ std::cout<<"form is clicked"<<std::endl; } 
			); 
		form.show(); 
		nana::gui::exec(); 
	} 

The argument []{ std::cout<<"form is clicked"<<std::endl; } is a "lambda" 
(or "lambda function" or "lambda expression") in C++ language(C++11). A lambda starts with plain [],
and compound-state block {} defines a function body. In fact, A lambda defines an anonymous 
function object, and therefore a lambda could be invoked through a function-call syntax. 

	[]{ std::cout<<"hello, Nana"<<std::endl; }(); 

The use of lambda is creating an anonymous function object and so the arguments should be specified. For example: 

	form.make_event<nana::gui::events::click>( 
			[](const nana::gui::eventinfo& ei) 
			{ std::cout<<"mouse pos=("<<ei.mouse.x<<", "<<ei.mouse.y <<std::endl;} 
		); 

The lambda-declarator () is used like a parameter-list. Let’s stop the introduction to the lambda, if you want more details of lambda, please refer to other C++ books. 

