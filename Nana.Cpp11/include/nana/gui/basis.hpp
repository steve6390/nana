/*
 *	Basis Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/basis.hpp
 *
 *	This file provides basis class and data structrue that required by gui
 */

#ifndef NANA_GUI_BASIS_HPP
#define NANA_GUI_BASIS_HPP

#include "../basic_types.hpp"
#include "../traits.hpp"	//metacomp::fixed_type_set

namespace nana
{
namespace gui
{
	namespace detail
	{
		struct native_window_handle_impl{};
		struct window_handle_impl{};
		struct event_handle_impl{};
	}

	namespace category
	{
		enum class flags
		{
			super,
			widget = 0x1,
			lite_widget = 0x3,
			root = 0x5,
			frame = 0x9
		};
		//wait for constexpr
		struct widget_tag{ static const flags value = flags::widget; };
		struct lite_widget_tag : widget_tag{ static const flags value = flags::lite_widget;};
		struct root_tag : widget_tag{ static const flags value = flags::root; };
		struct frame_tag: widget_tag{ static const flags value = flags::frame; };
	}// end namespace category

	typedef detail::native_window_handle_impl * native_window_type;
	typedef detail::window_handle_impl*	window;
	typedef detail::event_handle_impl*	event_handle;

	struct keyboard
	{
		enum keys
		{
			select_all = 0x1,
			alt = 18,
			backspace = 0x8, tab = 0x9,
			esc = 0x1B, enter = 0xD,
			cancel = 0x3, sync = 0x16,
			pageup = 33, pagedown,
			left = 37, up, right, down,
			insert = 45, del,
		};
	};

	struct mouse
	{
		enum t
		{
			any_button, left_button, middle_button, right_button
		};
	};

	namespace color
	{
		enum
		{
			white = 0xFFFFFF,
			button_face_shadow_start = 0xF5F4F2,
			button_face_shadow_end = 0xD5D2CA,
			button_face = 0xD4D0C8,
			dark_border	= 0x404040,
			gray_border	= 0x808080,
			highlight = 0x1CC4F7,
		};
	};

	struct z_order_action
	{
		enum inner_t
		{
			none, bottom, top, topmost, foreground
		};
	};

	//Window appearance structure
	struct appearance
	{
		bool taskbar;
		bool floating;
		bool no_activate;

		bool minimize;
		bool maximize;
		bool sizable;

		bool decoration;

		appearance();
		appearance(bool has_decaration, bool taskbar, bool floating, bool no_activate, bool min, bool max, bool sizable);
	};

	struct appear
	{
		struct minimize{};
		struct maximize{};
		struct sizable{};
		struct taskbar{};
		struct floating{};
		struct no_activate{};

		template<typename Minimize = null_type,
					typename Maximize = null_type,
					typename Sizable = null_type,
					typename Floating = null_type,
					typename NoActive = null_type>
		struct decorate
		{
			typedef metacomp::fixed_type_set<Minimize, Maximize, Sizable, Floating, NoActive> set_type;

			operator appearance() const
			{
				return appearance(	true, true,
									set_type::template count<floating>::value,
									set_type::template count<no_activate>::value,
									set_type::template count<minimize>::value,
									set_type::template count<maximize>::value,
									set_type::template count<sizable>::value
									);
			}
		};

		template<typename Taskbar = null_type, typename Floating = null_type, typename NoActive = null_type, typename Minimize = null_type, typename Maximize = null_type, typename Sizable = null_type>
		struct bald
		{
			typedef metacomp::fixed_type_set<Taskbar, Floating, NoActive, Minimize, Maximize, Sizable> set_type;

			operator appearance() const
			{
				return appearance(	false,
									set_type::template count<taskbar>::value,
									set_type::template count<floating>::value,
									set_type::template count<no_activate>::value,
									set_type::template count<minimize>::value,
									set_type::template count<maximize>::value,
									set_type::template count<sizable>::value
									);
			}
		};

		template<bool HasDecoration = true, typename Sizable = null_type, typename Taskbar = null_type, typename Floating = null_type, typename NoActive = null_type>
		struct optional
		{
			typedef metacomp::fixed_type_set<Taskbar, Floating, NoActive> set_type;

			operator appearance() const
			{
				return appearance(HasDecoration,
									set_type::template count<taskbar>::value,
									set_type::template count<floating>::value,
									set_type::template count<no_activate>::value,
									true, true,
									set_type::template count<sizable>::value);
			}
		};
	};//end namespace apper
}//end namespace gui
}//end namespace nana

#endif

