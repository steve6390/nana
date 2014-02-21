
#   How to implement a button  

## 1 Introduction  
This article explain how to implement a button widget and some basic knowledge about nana::gui::button. 
It is a simple widget and a good start point to learn the implementation of a widget. The implementation 
of nana::gui::button is defined in "include/nana/gui/widgets/button.hpp" and "source/gui/widgets/button.cpp".

Nana.GUI provides the framework for implementing a widget. A GUI-widget is made of a window manipulator and a 
drawer trigger, so, to implement a widget we need to implement these two parts. 

Window Manipulator

This class provides some operations that make the widget visible for a user. For example nana::gui::button is a manipulator. 

Drawer Trigger

A drawing operation is driven by the drawer trigger. It provied the internal of a widget, reacting 
to events and is invisible for an user. During the life time of a window manipulator and a drawer 
trigger, Nana.GUI does not involve creation and destruction of a drawer trigger.

## 2. Implementation ##

### 2.1 Window Manipulator ###

	template<typename DrawerTrigger>
	class basic_button: public widget_object<category::widget_tag, DrawerTrigger>
	{public:
	basic_button(){}
	basic_button(nana::gui::widget& widget, int x, int y, unsigned width,
 				unsigned height)
	{ this->create(widget.handle(), x, y, width, height);
	}
	};

This code defines the window manipulator of the button.

	template<typename DrawerTrigger>

The template parameter DrawerTrigger is used to specify a drawer trigger.

basic_button inherit from widget_object<category::widget_tag, DrawerTrigger>. 
The widget_tag indicates the button is a Widget Window. Although all widgets are 
inherited from nana::gui::widget, the widgets just can be inherited from the template class widget_object<>.

The default constructor does not create the widget unless the create() method is called. 
The second constructor will create the widget by calling the create() method. Nana.GUI guarantees the safety if an operation on a widget before its creation.

### 2.2. Drawer Trigger ###

A drawer trigger is more complicate than a window manipulator; it reacts to mouse and keyboard events. 
In this implementation of the button, the code for drawing the button is written inside the drawer trigger 
directly. Now, let's dip into the definition of the drawer trigger.

A drawer trigger must be inherited from std::gui::drawer_trigger.

	using namespace nana;
	class button_drawer: public nana::gui::drawer_trigger<cagetory::widget_tag>
	{public:
		button_drawer ();
		void bind_window(nana::gui::widget & widget);
	 private:
		void attached(nana::paint::graphics &gra );
		void detached();
		void normal	(paint::graphics &gra);
		void mouse_leave(paint::graphics &gra, const eventinfo &ei);
		void mouse_down	(paint::graphics &gra, const eventinfo &ei);
		void mouse_up	(paint::graphics &gra, const eventinfo &ei);
		void focus		(paint::graphics &gra, const eventinfo &ei);
	 private:
		void _m_draw_title(paint::graphics&, bool is_mouse_down, bool is_focus);
		void _m_draw_background(paint::graphics&, bool is_mouse_down);
		void _m_draw_border(paint::graphics&, bool is_mouse_down);

		nana::gui::widget* widget_ ;
		bool 		is_ms_down_ ;
		bool 		is_focus_ ;
	};

This button_drawer makes the constructor and a bind_window() member function  public, and others private. 

void bind_window(nana::gui::widget& widget){widget_ = &widget;}

Nana.GUI creates a widget and tells the drawer trigger the widget through bind_window().

The private member functions with don’t start with _m_ are virtual functions that are defined by nana::gui::drawer_trigger. 
These virtual functions are invoked inside the Nana.GUI. The name of some of these virtual functions starts with mouse_ because 
they are used for answering to mouse operations. You need to override these functions if you want some special capacities.

The private member functions with start with _m_ are defined by the button drawer trigger. They implements the drawing 
of the button. As will be readily seen, when a mouse event is raised, the drawer trigger calls these member functions for drawing the widget.

	nana::gui::widget * widget_;

This data member refers to a window manipulator which is connecting with the drawer trigger.

	bool is_ms_down_;

This indicates the current status of the left mouse button (is pressed down?).

Now, let's return to the details of member functions with start with _m_.

	button_drawer::button_drawer()
	:widget_(0), is_ms_down_(false), is_focus_(false)
	{}

The default constructor initializes the data members. is_ms_down_ will be modified by mouse_down() and by mouse_up(). is_focus_ will be modified by focus().

	void button_drawer::set(nana::gui::widget& widget)
	{  if(widget_ == 0) widget_ = &widget;
	}

Sets widget_, this function is invoked by window manipulator's get_drawer_trigger().
The following member functions are the essential of the drawer trigger.

	void button_drawer::attached(nana::paint::graphics&)
	{
		using namespace nana::gui::API;
		is_ms_down_ = false;
		nana::gui::window window = widget_->handle();
		make_drawer_event<events::mouse_leave>(window);
		make_drawer_event<events::mouse_down>(window);
		make_drawer_event<events::mouse_up>(window);
		make_drawer_event<events::focus>(window);
	}

When a widget is creating, Nana.GUI is responsible for attaching the drawer trigger which is returned 
by the window manipulator to the Window Manager of Nana.GUI, at the same time, Nana.GUI calls the attach() 
method provided by the drawer trigger, in other words, the attach() method is the first method of a drawer 
trigger that Nana.GUI calls. When the drawer trigger is attached to the Window Manager, the connection is 
created between the drawer trigger and window manipulator that the drawer trigger will receive the callback 
of an event from a widget. In this member function, button_drawer registers the events through the handle of 
window manipulator. The drawer trigger has a special function for registering an event(it is different with 
nana::gui::API::register_event and make_event() of a window manipulator). Register an event, and Nana.GUI calls 
back the corresponding function for answering the event. In button_drawer's attach(), it registers mouse_leave, 
mouse_down and mouse_up, you can register more events if you want button more specially good effect. The attach() 
has a parameter, it refers to a nana::paint::graphics object, this object is the off-screen buffer of the widget, 
any drawing operation on this object, it will be displayed on the monitor. This object is created and destroyed by 
Nana.GUI. There is not any operation on this object. 

	void button_drawer::detached()
	{ nana::gui::API::unregister_drawer_event(widget_->handle()); }

When a drawer trigger is detached from the Window Manager, Nana.GUI will callback the detach() method. In the button_drawer implementation, we just unregister the events that we registered in attach(). nana::GUI::API::unregister_drawer_event is responsible for destroying the all registered events for the specified window. 

	void button_drawer::normal(paint::graphics& ghc)
	{
		_m_draw_background	(ghc, is_ms_down_);
		_m_draw_border		(ghc, is_ms_down_);
		_m_draw_title		(ghc, is_ms_down_, is_focus);
	}

The normal() method defined by a drawer trigger will be invoked after a widget is created completely, and nana::gui::API::refresh_window() will also invoke this normal() method. In this implementation, the normal() method calls the member functions start with _m_ for operating the graphics object.

	void button_drawer::mouse_leave(paint::graphics& ghc, const eventinfo&)
	{
		_m_draw_background	(ghc, false);
		_m_draw_border		(ghc, false);
		_m_draw_title		(ghc, false, is_focus_);
		nana::gui::API::lazy_refresh();
	}

When the mouse leaves the widget, Nana.GUI will call the mouse_leave, where the second parameter refers to an eventinfo object which provides some event information. In this implementation we don't take care about this parameter. It's worth noting that nana::gui::API::lazy_refresh() is invoked last, it will let the graphics object display on monitor. lazy_refresh() only works in an event callback, hence we can't find it in normal() method.
mouse_down(), mouse_up() and focus() are similar to mouse_leave(). Refer to "source/gui/widgets/button.cpp" for the details. Refer to Nana.Paint for the details of nana::paint::graphics.

## The tabstop in Nana.GUI #

A tabstop is used for switching the keyboard focus between two widgets by pressing the Tab. Nana.GUI defines 3 states for tabstop.

Every Nana.GUI window defaultly does not take care about the tabstop, but when implement a widget, programmers must pay attention to the tabstop. If a widget owns the tabstop, the widget would catch the keyboard focus when the Tab key is pressed. If a widget is eating-tab, the widget can deal with the tab in the key_char event, and the focus could not change. A textbox is eating-tab. If a widget does not take care about the tabstop, it will ignore the message.

There are two functions about the tabstop defined in nana::gui::API. tabstop is used for setting a widget owns a tabstop, eat_tabstop is used for setting a widget as eating tabstop. A widget can be set both tabstop and eat_tabstop . For example, for a textbox that has been set both tabstop and eat_tabstop, if the textbox is not focused, a user can press the Tab key to switch the focus on it, and when the user press a Tab key again, a Tab character is inputted into the textbox and the focus is not changed.

