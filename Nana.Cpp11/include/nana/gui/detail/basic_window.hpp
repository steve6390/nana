/*
 *	A Basic Window Widget Definition
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/detail/basic_window.hpp
 */

#ifndef NANA_GUI_DETAIL_BASIC_WINDOW_HPP
#define NANA_GUI_DETAIL_BASIC_WINDOW_HPP
#include "drawer.hpp"
#include "../basis.hpp"
#include <nana/basic_types.hpp>
#include <nana/system/platform.hpp>
#include <nana/gui/effects.hpp>

namespace nana{	namespace gui{

namespace detail
{
	struct basic_window;

	class caret_descriptor
	{
	public:
		typedef basic_window core_window_t;

		caret_descriptor(core_window_t* wd, unsigned width, unsigned height);
		~caret_descriptor();
		void set_active(bool);
		core_window_t* window() const;
		void position(int x, int y);
		void effective_range(nana::rectangle);
		nana::point position() const;
		void visible(bool isshow);
		bool visible() const;
		nana::size size() const;
		void size(const nana::size&);
	private:
		void _m_visible(bool isshow);
		void _m_real_paint();
	private:
		core_window_t*	wd_;
		nana::point point_;
		nana::size	size_;
		nana::size	paint_size_;
		bool		visible_;
		bool		real_visible_state_;
		bool		out_of_range_;
		nana::rectangle effective_range_;
	};//end class caret_descriptor

	//tab_type
	//@brief: Define some constant about tab category, these flags can be combine with operator |
	struct tab_type
	{
		enum t
		{
			none,		//process by nana
			tabstop,	//move to the next tabstop window
			eating,		//process by current window
		};
	};

	//struct basic_window
	//@brief: a window data structure descriptor 
	struct basic_window
	{
		typedef std::vector<basic_window*> container_type;
		typedef std::vector<basic_window*> tabstop_container_type;

		struct root_context
		{
			bool focus_changed;
		};

		enum class update_state
		{
			none, lazy, refresh
		};

		struct edge_nimbus_action
		{
			basic_window * window;
			bool rendered;
		};

		typedef std::vector<edge_nimbus_action>	edge_nimbus_container;

		//basic_window
		//@brief: constructor for the root window
		basic_window(basic_window* owner, gui::category::root_tag**);

		template<typename Category>
		basic_window(basic_window* parent, const rectangle& r, Category**)
			: other(Category::value)
		{
			if(parent)
			{
				_m_init_pos_and_size(parent, r);
				this->_m_initialize(parent);
			}
		}

		~basic_window();

		//bind_native_window
		//@brief: bind a native window and baisc_window
		void bind_native_window(native_window_type, unsigned width, unsigned height, unsigned extra_width, unsigned extra_height, nana::paint::graphics&);

		void frame_window(native_window_type);
	private:
		void _m_init_pos_and_size(basic_window* parent, const rectangle&);
		void _m_initialize(basic_window* parent);
	public:
		typedef nana::color_t color_t;
		typedef std::vector<basic_window*> cont_type;

		nana::rectangle	rect;
		nana::size	min_track_size;
		nana::size	max_track_size;

		int	root_x;			//coordinate for root window
		int	root_y;			//coordinate for root window
		bool	visible;

		unsigned extra_width;
		unsigned extra_height;

		basic_window	*parent;
		basic_window	*owner;

		nana::string	title;
		nana::gui::detail::drawer	drawer;	//Self Drawer with owen graphics
		basic_window*		root_widget;	//A pointer refers to the root basic window, if the window is a root, the pointer refers to itself.
		paint::graphics*	root_graph;		//Refer to the root buffer graphics

		struct flags_type
		{
			bool enabled	:1;
			bool dbl_click	:1;
			bool capture	:1;	//if mouse button is down, it always receive mouse move even the mouse is out of its rectangle
			bool modal		:1;
			bool glass		:1;
			bool take_active:1;	//If take_active is false, other.active_window still keeps the focus.
			bool refreshing	:1;
			bool destroying	:1;
			bool dropable	:1; //Whether the window has make mouse_drop event.
			bool fullscreen	:1;	//When the window is maximizing whether it fit for fullscreen.
			unsigned Reserved: 22;
			unsigned char tab;		//indicate a window that can receive the keyboard TAB
			mouse_action	action;
		}flags;

		struct
		{
			caret_descriptor* caret;
		}together;

		struct
		{
			color_t foreground;
			color_t	background;
			color_t	active;
		}color;

		struct
		{
			effects::edge_nimbus_t	edge_nimbus;
		}effect;
		
		struct other_tag
		{
			struct	attr_frame_tag
			{
				native_window_type container;
				std::vector<native_window_type> attach;
			};

			struct	attr_root_tag
			{
				std::vector<basic_window*>	frames;	//initialization is null, it will be created while creating a frame widget. Refer to WindowManager::create_frame
				std::vector<basic_window*>	tabstop;
				std::vector<edge_nimbus_action> effects_edge_nimbus;
				basic_window	*focus;
				basic_window	*menubar;
				root_context	context;
				bool			ime_enabled;
			};

			category::flags category;

			basic_window *active_window;	//if flags.take_active is false, the active_window still keeps the focus,
											//if the active_window is null, the parent of this window keeps focus.
			paint::graphics glass_buffer;	//if flags.glass is true. Refer to window_layout::make_glass_background.
			update_state	upd_state;

			union
			{
				attr_root_tag * root;
				attr_frame_tag * frame;
			}attribute;

			other_tag(gui::category::flags);
			~other_tag();
		}other;

		native_window_type	root;		//root Window handle
		unsigned			thread_id;		//the identifier of the thread that created the window.
		unsigned			index;
		cont_type			children;
	};

}//end namespace detail
}//end namespace gui
}//end namespace nana
#endif
