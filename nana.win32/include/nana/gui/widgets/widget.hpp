/*
 *	The fundamental widget class implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/widget.hpp
 */

#ifndef NANA_GUI_WIDGET_HPP
#define NANA_GUI_WIDGET_HPP
#include <nana/traits.hpp>
#include "../basis.hpp"
#include "../programming_interface.hpp"
#include <nana/functor.hpp>

namespace nana
{
namespace gui
{
	//class widget
	//@brief: this is a abstract class for defining the capacity interface.
	class widget
		: private nana::noncopyable
	{
		typedef void(*dummy_bool_type)(widget* (*)(const widget&));
	public:
		virtual ~widget();
		virtual window handle() const = 0;
		bool empty() const;
		void close();

		window parent() const;

		nana::string caption() const;
		void caption(const nana::string& str);

		void typeface(const nana::paint::font& font);
		nana::paint::font typeface() const;

		bool enabled() const;
		void enabled(bool);

		void show();
		void hide();
		bool visible() const;

		nana::size size() const;
		void size(unsigned width, unsigned height);
		
		nana::point pos() const;
		void move(int x, int y);
		void move(int x, int y, unsigned width, unsigned height);

		bool focused() const;

		void foreground(nana::color_t);
		nana::color_t foreground() const;
		void background(nana::color_t);
		nana::color_t background() const;

		template<typename Event, typename Function>
		event_handle make_event(Function function) const
		{
			if(traits::is_derived<Event, detail::event_type_tag>::value)
				return API::make_event<Event, Function>(this->handle(), function);
			else
				return 0;
		}

		template<typename Event, typename Class, typename Concept>
		event_handle make_event(Class& object, void (Concept::*memf)(const eventinfo&)) const
		{
			if(traits::is_derived<Event, detail::event_type_tag>::value)
				return API::make_event<Event>(this->handle(), functor<void(const eventinfo&)>(object, memf));
			else
				return 0;
		}

		template<typename Event, typename Class, typename Concept>
		event_handle make_event(Class& object, void (Concept::*memf)()) const
		{
			if(traits::is_derived<Event, detail::event_type_tag>::value)
				return API::make_event<Event>(this->handle(), functor<void()>(object, memf));
			else
				return 0;
		}

		template<typename Event, typename Function>
		event_handle bind_event(widget& wdg, Function function) const
		{
			if(traits::is_derived<Event, detail::event_type_tag>::value)
				return API::bind_event<Event, Function>(wdg.handle(), this->handle(), function);
			else
				return 0;
		}

		template<typename Event, typename Class, typename Concept>
		event_handle bind_event(widget& wdg, Class& object, void (Concept::*memf)(const eventinfo&)) const
		{
			if(traits::is_derived<Event, nana::gui::detail::event_type_tag>::value)
				return API::bind_event<Event>(wdg.handle(), this->handle(), functor<void(const eventinfo&)>(object, memf));
			else
				return 0;
		}

		void umake_event(event_handle eh) const;
		operator dummy_bool_type() const;
		operator nana::gui::window() const;
	protected:
		//protected members, a derived class must call this implementation if it overrides an implementation
		virtual void _m_complete_creation();

		virtual nana::string _m_caption() const;
		virtual void _m_caption(const nana::string& str);
		virtual void _m_close();
		virtual bool _m_enabled() const;
		virtual void _m_enabled(bool);
		virtual bool _m_show(bool);
		virtual bool _m_visible() const;
		virtual void _m_size(unsigned width, unsigned height);
		virtual void _m_move(int x, int y);
		virtual void _m_move(int x, int y, unsigned width, unsigned height);

		virtual void _m_typeface(const nana::paint::font& font);
		virtual nana::paint::font _m_typeface() const;

		virtual void _m_foreground(nana::color_t);
		virtual nana::color_t _m_foreground() const;
		virtual void _m_background(nana::color_t);
		virtual nana::color_t _m_background() const;
	};

	template<typename Category, typename DrawerTrigger>
	class widget_object: public widget
	{
	protected:		typedef DrawerTrigger drawer_trigger_t;
	public:
		 widget_object()			:handle_(0)	{}
		~widget_object()	{	if(handle_)	API::close_window(handle_);		}

		bool create(window wd, int x, int y, unsigned width, unsigned height)
													{		return create(wd, x, y, width, height, true);		}
		bool create(window wd)						{		return create(wd, 0, 0, 0, 0, true);		}
		bool create(window wd, bool visible)		{		return create(wd, 0, 0, 0, 0, visible);		}
		bool create(window wd, rectangle r=rectangle(), bool visible=true)
													{		return create(wd, r.x, r.y, r.width, r.height, visible);		}
		bool create(window wd, int x, int y, unsigned width, unsigned height, bool visible)
		{
			if(wd && this->empty())
			{
				handle_ = API::dev::create_widget(wd, x, y, width,height);
				API::dev::attach_signal		 (	handle_, *this, &widget_object::signal	);
				static_cast<drawer_trigger&> ( trigger_ ).bind_window (*this);
				API::dev::attach_drawer		 ( handle_, trigger_ );
				if(visible)		API::show_window( handle_, true );
				this->_m_complete_creation();
			}
			return (this->empty() == false);
		}

		window handle() const		{	return handle_;		}
	protected:
			  DrawerTrigger& get_drawer_trigger()		{			return trigger_;		}
		const DrawerTrigger& get_drawer_trigger() const	{			return trigger_;		}
	private:
		void signal(int message, const detail::signals& sig)
		{
			switch(message)
			{
				case detail::signals::caption:		this->_m_caption(sig.info.caption);				break;
				case detail::signals::destroy:		handle_ = 0;									break;
			}
		}
	private:
		window			handle_;
		DrawerTrigger	trigger_;
	};//end class widget_object

	template<typename DrawerTrigger>
	class widget_object<category::lite_widget_tag, DrawerTrigger>: public widget
	{
	protected:
		typedef DrawerTrigger drawer_trigger_t;
	public:

		widget_object()
			:handle_(0)
		{}

		~widget_object()
		{
			if(handle_)
				API::close_window(handle_);
		}

		bool create(window wd)
		{
			return create(wd, 0, 0, 0, 0, true);
		}

		bool create(window wd, bool visible)
		{
			return create(wd, 0, 0, 0, 0, visible);
		}

		bool create(window wd, int x, int y, unsigned width, unsigned height)
		{
			return create(wd, x, y, width, height, true);
		}

		bool create(window wd, int x, int y, unsigned width, unsigned height, bool visible)
		{
			if(wd && this->empty())
			{
				handle_ = API::dev::create_lite_widget(wd, x, y, width, height);
				if(visible)
					API::show_window(handle_, true);
				this->_m_complete_creation();
			}
			return (this->empty() == false);
		}

		window handle() const
		{
			return handle_;
		}
	private:
		void signal(int message, const detail::signals& sig)
		{
			switch(message)
			{
			case detail::signals::caption:
				this->_m_caption(sig.info.caption);
				break;
			case detail::signals::destroy:
				handle_ = 0; break;
			}
		}
	private:
		window handle_;
	};//end class widget_object

	template<typename DrawerTrigger>
	class widget_object<category::root_tag, DrawerTrigger>: public widget
	{
	protected:
		typedef DrawerTrigger drawer_trigger_t;
	public:

		widget_object()
			:handle_(API::dev::create_window(0, false, 0, 0, 300, 150, appearance()))
		{
			_m_bind_and_attach();
		}

		widget_object(int x, int y, unsigned width, unsigned height, const appearance& apr)
			:	handle_(API::dev::create_window(0, false, x, y, width, height, apr))
		{
			_m_bind_and_attach();
		}

		widget_object(window owner, bool nested)
			: handle_(API::dev::create_window(owner, nested, 0, 0, 300, 150, appearance()))
		{
			_m_bind_and_attach();
		}

		widget_object(window owner, bool nested, int x, int y, unsigned width, unsigned height, const appearance& apr)
			:	handle_(API::dev::create_window(owner, nested, x, y, width, height, apr))
		{
			_m_bind_and_attach();
		}

		~widget_object()
		{
			if(handle_)
				API::close_window(handle_);
		}

		window handle() const
		{
			return handle_;
		}

		window owner() const
		{
			return API::get_owner_window(handle_);
		}

		void icon(const nana::paint::image& ico)
		{
			API::window_icon(handle_, ico);
		}
	protected:
		DrawerTrigger& get_drawer_trigger()
		{
			return trigger_;
		}

		const DrawerTrigger& get_drawer_trigger() const
		{
			return trigger_;
		}
	private:
		void signal(int message, const detail::signals& sig)
		{
			switch(message)
			{
			case detail::signals::caption:
				this->_m_caption(sig.info.caption);
				break;
			case detail::signals::destroy:
				handle_ = 0; break;
			}
		}

		void _m_bind_and_attach()
		{
			API::dev::attach_signal(handle_, *this, &widget_object::signal);
			static_cast<drawer_trigger&>(trigger_).bind_window(*this);
			API::dev::attach_drawer(handle_, trigger_);	
		}
	private:
		window handle_;
		DrawerTrigger trigger_;
	};//end class widget_object<root_tag>

	template<typename Drawer>
	class widget_object<category::frame_tag, Drawer>: public widget{};

	template<>
	class widget_object<category::frame_tag, int>: public widget
	{
	protected:
		typedef int drawer_trigger_t;
	public:
		widget_object()
			:handle_(0)
		{}

		~widget_object()
		{
			if(handle_)
				API::close_window(handle_);
		}

		bool create(window wd)
		{
			return create(wd, 0, 0, 0, 0, true);
		}

		bool create(window wd, bool visible)
		{
			return create(wd, 0, 0, 0, 0, visible);
		}

		bool create(window wd, int x, int y, unsigned width, unsigned height)
		{
			return create(wd, x, y, width, height, true);
		}

		bool create(window wd, int x, int y, unsigned width, unsigned height, bool visible)
		{
			if(wd && this->empty())
			{
				handle_ = API::dev::create_frame(wd, x, y, width,height);
				API::dev::attach_signal(handle_, *this, &widget_object::signal);
				API::show_window(handle_, visible);
				this->_m_complete_creation();
			}
			return (this->empty() == false);
		}

		window handle() const
		{
			return handle_;
		}
	private:
		virtual drawer_trigger* get_drawer_trigger()
		{
			return 0;
		}

		void signal(int message, const detail::signals& sig)
		{
			switch(message)
			{
			case detail::signals::caption:
				this->_m_caption(sig.info.caption);
				break;
			case detail::signals::destroy:
				handle_ = 0; break;
			}
		}
	private:
		window handle_;
	};//end class widget_object<category::frame_tag>
}//end namespace gui
}//end namespace nana
#endif

