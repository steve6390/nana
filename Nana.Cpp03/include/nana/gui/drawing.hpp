/*
 *	A Drawing Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/drawing.hpp
 */
#ifndef NANA_GUI_DRAWING_HPP
#define NANA_GUI_DRAWING_HPP
#include "widgets/widget.hpp"
#include "../traits.hpp"
namespace nana
{
namespace gui
{
	
	class drawing
		:private nana::noncopyable
	{
	public:
		drawing(const nana::gui::widget& widget);
		//Just for polymorphism
		virtual ~drawing();

		bool empty() const;
		void update() const;
		void string(int x, int y, unsigned color, const nana::char_t* text);
		void bitblt(int x, int y, unsigned width, unsigned height, const nana::paint::graphics& source, int srcx, int srcy);
		void bitblt(int x, int y, unsigned width, unsigned height, const nana::paint::image& source, int srcx, int srcy);
		void line(int x, int y, int x2, int y2, unsigned color);
		void rectangle(int x, int y, unsigned width, unsigned height, unsigned color, bool issolid);
		void shadow_rectangle(int x, int y, unsigned width, unsigned height, nana::color_t beg, nana::color_t end, bool vertical);
		void stretch(const nana::rectangle & r_dst, const nana::paint::graphics& src, const nana::rectangle& r_src);
		void stretch(const nana::rectangle & r_dst, const nana::paint::image& src, const nana::rectangle& r_src);
		void clear();
	private:
		nana::gui::window window_;
	};//end class drawing

}//end namespace gui
}//end namespace nana
#endif
