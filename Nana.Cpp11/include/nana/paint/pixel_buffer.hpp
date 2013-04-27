/*
 *	Pixel Buffer Implementation
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/paint/pixel_buffer.hpp
 */

#ifndef NANA_PAINT_PIXEL_BUFFER_HPP
#define NANA_PAINT_PIXEL_BUFFER_HPP

#include <nana/gui/basis.hpp>
#include <memory>

namespace nana{	namespace paint
{
	class pixel_buffer
	{
		struct pixel_buffer_storage;
	public:
		pixel_buffer();
		pixel_buffer(drawable_type, const nana::rectangle& want_rectangle);
		pixel_buffer(drawable_type, std::size_t top, std::size_t lines);
		pixel_buffer(std::size_t width, std::size_t height);

		~pixel_buffer();

		bool open(drawable_type);
		bool open(drawable_type, const nana::rectangle& want_rectangle);
		bool open(std::size_t width, std::size_t height);

		void close();

		bool empty() const;

		operator const void*() const;

		std::size_t bytes() const;
		nana::size size() const;
		
		pixel_rgb_t * raw_ptr() const;
		pixel_rgb_t * raw_ptr(std::size_t row) const;
		void put(const unsigned char* rawbits, std::size_t width, std::size_t height, std::size_t bits_per_pixel, std::size_t bytes_per_line, bool is_negative);
		
		void line(const std::string& name);
		void line(const nana::point& pos_beg, const nana::point& pos_end, nana::color_t color, double fade_rate);

		void rectangle(const nana::rectangle&, nana::color_t, double fade_rate, bool solid);
		void shadow_rectangle(const nana::rectangle&, nana::color_t beg, nana::color_t end, double fade_rate, bool vertical);
		
		pixel_rgb_t pixel(int x, int y) const;
		void pixel(int x, int y, pixel_rgb_t);

		void paste(drawable_type, int x, int y) const;
		void paste(const nana::rectangle& src_r, drawable_type, int x, int y) const;
		void paste(gui::native_window_type, int x, int y) const;
		void stretch(const std::string& name);
		void stretch(const nana::rectangle& src_r, drawable_type, const nana::rectangle& r) const;
		void blend(const std::string& name);
		void blend(const nana::point& s_pos, drawable_type dw_dst, const nana::rectangle& d_r, double fade_rate) const;

	private:
		std::shared_ptr<pixel_buffer_storage> storage_;
	};
}//end namespace paint
}//end namespace nana

#endif
