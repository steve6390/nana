/*
 *	A Tooltip Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/tooltip.hpp
 */

#ifndef NANA_GUI_WIDGETS_TOOLTIP_HPP
#define NANA_GUI_WIDGETS_TOOLTIP_HPP
#include "widgets/widget.hpp"

namespace nana{ namespace gui
{
	class tooltip
	{
	public:
		tooltip();
		~tooltip();

		void set(window, const nana::string&);
		void show(window, int x, int y, const nana::string&);
		void close();
	};//class tooltip

}//namespace gui
}//namespace nana

#endif

