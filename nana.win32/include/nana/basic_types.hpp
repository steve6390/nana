/*
 *	Basic Types definition
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/basic_types.hpp
 */

#ifndef NANA_BASIC_TYPES_HPP
#define NANA_BASIC_TYPES_HPP

#include "macros.hpp"

namespace nana
{
	namespace detail
	{
		struct drawable_impl_type;	//declearation, defined in platform_spec.hpp
	}

	namespace paint
	{
		typedef nana::detail::drawable_impl_type*	drawable_type;
	}

	namespace gui
	{
		enum mouse_action_t{mouse_action_normal, mouse_action_over, mouse_action_pressed, mouse_action_end};
	}

	typedef unsigned		scalar_t;
	typedef unsigned char	uint8_t;
	typedef unsigned long	uint32_t;
	typedef unsigned		uint_t;
	typedef unsigned		color_t;

	const color_t null_color = 0xFFFFFFFF;

	struct pixel_rgb_t
	{
		union
		{
			struct element_tag
			{
				unsigned int blue:8;
				unsigned int green:8;
				unsigned int red:8;
				unsigned int alpha_channel:8;
			}element;

			color_t color;
		}u;
	};

	struct point
	{
		point();
		point(int x, int y);
		bool operator==(const point&) const;
		bool operator!=(const point&) const;
		bool operator<(const point&) const;
		bool operator<=(const point&) const;
		bool operator>(const point&) const;
		bool operator>=(const point&) const;

		int x;
		int y;
	};
	struct dpoint : point
	{   dpoint(int x_, int y_): point (x_,y_) {}
	    //dpoint operator -(point p1, point p2)
	};
	inline dpoint  operator -( point p1,  point p2){ return dpoint(p2.x-p1.x,p2.y-p1.y);}
	inline 	point  operator +(dpoint p1,  point p2){ return  point(p2.x+p1.x,p2.y+p1.y);}
	inline 	point  operator +( point p1, dpoint p2){ return  point(p2.x+p1.x,p2.y+p1.y);}

	struct upoint
	{
		typedef unsigned value_type;

		upoint();
		upoint(value_type x, value_type y);
		bool operator==(const upoint&) const;
		bool operator!=(const upoint&) const;
		bool operator<(const upoint&) const;
		bool operator<=(const upoint&) const;
		bool operator>(const upoint&) const;
		bool operator>=(const upoint&) const;

		value_type x;
		value_type y;
	};

	//struct dsize : size
	//{   dsize(int width_, int height_): size (width_,height_) {}
	//    //dsize operator -(size sz1, size sz2)
	//};
	//dsize operator -( size sz1,  size sz2){ return dsize(sz2.width-sz1.width,sz2.height-sz1.height);}
	//size  operator +(dsize sz1,  size sz2){ return  size(sz2.width+sz1.width,sz2.height+sz1.height);}
	//size  operator +( size sz1, dsize sz2){ return  size(sz2.width+sz1.width,sz2.height+sz1.height);}

	struct size
	{
		size();
		size(unsigned width, unsigned height);

		bool operator==(const size& rhs) const;
		bool operator!=(const size& rhs) const;

		unsigned width;
		unsigned height;
	};

	// 
	struct rectangle
	{
		rectangle();
		rectangle(int x, int y, unsigned width, unsigned height);
		rectangle(const size &);
		rectangle(const point& ori, const size&);
		rectangle(const point& p1, const point& p2)	:x(p1.x < p2.x ? p1.x : p2.x),
													 y(p1.y < p2.y ? p1.y : p2.y),
													 width (abs(p2.x-p1.x)),
													 height(abs(p2.y-p1.y)) {};
		bool operator==(const rectangle& rhs) const;
		bool operator!=(const rectangle& rhs) const;
		rectangle left_at (int    dx)const{return rectangle(point (x+width+dx, y), sz());}
		rectangle down_at (int    dy)const{return rectangle(point (x,y+height+dy), sz());}
		rectangle new_size(size   nz){return rectangle(		 ori()			, nz  );}
		//rectangle w (unsigned   nw){return rectangle(		 ori()			, size(nw, height)  );}
		rectangle&  w (unsigned   nw){ width= nw; return *this; }

		//rectangle shift(dpoint sh){return rectangle(point (x,y+height+dy), sz());}
		int x;
		int y;
		unsigned width;
		unsigned height;
		point ori() const {return point (x,y);}
		size  sz () const {return size  (width,height);}
	};

	struct arrange
	{
		enum t{unkown, horizontal, vertical, horizontal_vertical};
		t value;

		arrange();
		arrange(t);
		operator t() const;
		arrange& operator=(t);
		bool operator==(t) const;
		bool operator!=(t) const;
	};
}//end namespace nana

#endif


