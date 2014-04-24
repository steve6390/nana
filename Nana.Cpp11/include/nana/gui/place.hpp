/*
 *	An Implementation of Place for Layout
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/place.cpp
 */

#ifndef NANA_GUI_PLACE_HPP
#define NANA_GUI_PLACE_HPP
#include <utility>
#include <nana/gui/basis.hpp>

namespace nana
{
namespace gui
{
    ///  Layout managment - an object of class place is attached to a widget, and it automatically positions and resizes the children widgets.
	class place
		: nana::noncopyable
	{
		typedef std::pair<window, unsigned>	fixed_t;
		typedef std::pair<window, int>	percent_t;
		typedef std::pair<window, std::pair<unsigned, unsigned> > room_t;

		struct implement;

		class field_t
		{
		public:
			virtual ~field_t() = 0;
			virtual field_t& operator<<(window wd)		= 0;
			virtual field_t& operator<<(unsigned gap)	= 0;
			virtual field_t& operator<<(const fixed_t& f)	= 0;
			virtual field_t& operator<<(const percent_t& p)	= 0;
			virtual field_t& operator<<(const room_t& r)	= 0;
			virtual field_t& fasten(window wd)	= 0;
		};
	public:
        ///  reference to a field manipulator which refers to a field object created by place 
		typedef field_t & field_reference;

		place();
		place(window);///< Attaches to a specified widget.
		~place();

		/** @brief Bind to a window
		 *	@param handle	A handle to a window which the place wants to attach.
		 *	@remark	It will throw an exception if the place has already binded to a window.
		 */
		void bind(window handle);
        
		void div(const char* s);              ///< Divides the attached widget into fields.
		field_reference field(const char* name);///< Returns a field with the specified name.
		void collocate();                     ///< Layouts the widgets.

		static fixed_t fixed(window wd, unsigned size);  ///< Generates a fixed object to send it to the field.
		static percent_t percent(window wd, int per);    ///< Generates a percent object to send it to the field.
		static room_t room(window wd, unsigned w, unsigned h); ///< Generates a room object to send it to the field.
	private:
		implement * impl_;
	};
}//end namespace gui
}//end namespace nana

#endif //#ifndef NANA_GUI_PLACE_HPP
