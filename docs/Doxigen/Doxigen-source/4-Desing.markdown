# 4. Think about the Design #
Assuming the design of a framework that generates some data and it requires a module 
for representing these data in GUI. In general, we just introduce an interface for outputting these data, for example: 

	struct data { std::string url; }; 
	class uiface 
	{ public: 
		virtual ~uiface() = 0; 
		virtual void create_ui_element(const data&) = 0; 
	}; 
	uiface::~uiface(){} 
	class framework 
	{ public: 
		framework(): uiface_(0) 
		{ 
			data dat; 
			dat.url = "stdex.sf.net"; 
			cont_.push_back(dat); 
			dat.url = "nanaproject.wordpress.com"; 
			cont_.push_back(dat); 
		} 
		void set(uiface * uif) { uiface_ = uif; } 
		void work() 
		{ if(uiface_) 
		  { for(std::vector<data>::const_iterator i = cont_.begin(); i != cont_.end();   ++i) 
			{ uiface_->create_ui_element(*i); 
			} 
		  } 
		} 
	  private: 
		uiface * uiface_; 
		std::vector<data> cont_; 
	}; 

Now we need a GUI and it creates some buttons for representing the data, and when a buttons 
is clicked, it outputs the data that button represents. And then, let’s define a GUI for that requirement.
 
	namespace ui 
	{ 
		using namespace nana::gui; 
		class bar : public form, public uiface 
		{ public: 
			bar() { gird_.bind(*this); } 
		  private: 
			typedef std::pair<std::shared_ptr<button>, data> btn_pair; 
				   //Now we implement the virtual function that declared by uiface 
			virtual void create_ui_element(const data& dat) 
			{ 
				btn_pair p(std::shared_ptr<button>(new button(*this)), dat); 
							  //Make the click event 
				p.first->make_event<events::click>(*this, &bar::_m_click); 
				p.first->caption(nana::stringset_cast(dat.url)); 
				gird_.push(*(p.first), 0, 22); 
				ui_el_.push_back(p); 
			} 
			  private: 
			void _m_click(const eventinfo& ei) 
			{ 
				//Check which button is clicked 
				for(std::vector<btn_pair>::iterator i = ui_el_.begin(); i != ui_el_.end();++i) 
				{ if(i->first->handle() == ei.window) 
				  { 	//Show data 
					i->second; 
					std::cout<<"open "<<i->second.url<<std::endl; 
					break; 
 				  } 
				} 
			} 
			 private: 
			gird gird_; 
			std::vector<btn_pair> ui_el_; 
		}; 
	} 

It’s done for implementing the GUI. Let’s make the framework work. 

	#include <nana/gui/wvl.hpp> 
	#include <nana/gui/widgets/button.hpp> 
	#include <nana/gui/layout.hpp> 
	#include <iostream> 
	include definition of framework… 
	int main() 
	{ 
	ui::bar bar; 
	bar.show(); 
	framework fw; 
	fw.set(&bar); 
	fw.work(); 
	nana::gui::exec(); 
	} 

Run the program. Refer to figure 2.2, when we click a button, the program would output the data that is represented by button. 

![Figure 2.2 Design of a framework] (framework.jpg)
 
Let’s rethink the implementation of class bar. It is a bit complicated, because a check of 
which button is clicked is needed. In fact, we can reduce the complexity of previous design 
by employing function object. Think about the following implementation. 

	class bar : public form, public uiface 
	{   struct command 
		{  data dat; 
		   command(const data& d): dat(d) {} 
			void operator()() { std::cout<<"open "<<dat.url<<std::endl; } 
		}; 
	  public: 
		bar() { gird_.bind(*this); } 
	  private: 
							 //Now we implement the virtual function declared by uiface 
		virtual void create_ui_element(const data& dat) 
		{ 
		  std::shared_ptr<button> p(new button(*this)); 
										 //Make the click event 
		  p->make_event<events::click>(command(dat)); 
		  p->caption(nana::stringset_cast(dat.url)); 
		  gird_.push(*p, 0, 22); 
		  ui_el_.push_back(p); 
		} 
	  private: 
		gird gird_; 
		std::vector<std::shared_ptr<button> > ui_el_; 
	}; 

As you see it, the new implementation is more convenient. The check of which button is clicked 
and the pair structure are removed. In C++11, the standard library provides a bind function. 
Generating a function object by using std::bind() instead of giving a explicit definition of 
a class that used for a function. Let’s remove the definition of struct bar::command and reimplement 
the create_ui_element(). 

	virtual void create_ui_element(const data& dat) 
	{ 
		std::shared_ptr<button> p(new button(*this)); 
		 //Make the click event 
		typedef nana::functor<void()> fun; //or std::function<void()> 
		p->make_event<events::click> ( fun (std::bind( &bar::_m_click, this, dat) )); 
		p->caption(nana::stringset_cast(dat.url)); 
		gird_.push(*p, 0, 22); 
		ui_el_.push_back(p); 
	} 
	void _m_click(const data& dat) 
	{ 
	   std::cout<<"open "<<dat.url<<std::endl; 
	} 

The member function _m_click() is very tiny, we can remove it with lambda expression in C++11. 

The main idea of this section is binding the execution context at some place in code 
and retaining the context for future use. As above code shown, we bind the data with a 
function object, and when the function object is called, we could easily refer to the data. 
By using the reasonable technical idioms, we can make the code more slighter, more expressive, 
the benefits are: flexible application, less comment and easy maintenance. 

