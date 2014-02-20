/*
 *	A Button Implementation
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/button.hpp
 */

#ifndef NANA_GUI_WIDGET_BUTTON_HPP
#define NANA_GUI_WIDGET_BUTTON_HPP
#include "widget.hpp"


namespace nana{namespace gui{
	namespace drawerbase
	{
		namespace button
		{
			     ///	Draw the button
			class trigger: public drawer_trigger
			{
			public:
				struct bgimage_tag;
				enum class state  ///< Defines the state of button actions.
				{
					normal, focused, highlight, pressed, disabled
				};
				
				trigger();
				~trigger();
				void icon(const nana::paint::image&);
				void image(const nana::paint::image&);
				bgimage_tag * ref_bgimage();
				bool enable_pushed(bool);
				bool pushed(bool);
				bool pushed() const;
				void omitted(bool);
				bool focus_color(bool);
			private:
				void bind_window(widget_reference);
				void attached(graph_reference);
				void detached();
				void refresh(graph_reference);
				void mouse_enter(graph_reference, const eventinfo&);
				void mouse_leave(graph_reference, const eventinfo&);
				void mouse_down(graph_reference, const eventinfo&);
				void mouse_up(graph_reference, const eventinfo&);
				void key_char(graph_reference, const eventinfo&);
				void key_down(graph_reference, const eventinfo&);
				void focus(graph_reference, const eventinfo&);
			private:
				void _m_draw(graph_reference);
				void _m_draw_title(graph_reference, bool enabled);
				void _m_draw_background(graph_reference);
				void _m_draw_border(graph_reference);
			private:
				nana::gui::widget* widget_;
				nana::paint::graphics* graph_;

				struct bgimage_tag* bgimage_;
				struct attr_tag
				{
					bool omitted;
					bool focused;
					bool pushed;
					bool keep_pressed;
					state act_state;
					bool enable_pushed;
					bool focus_color;
					paint::image * icon;
					color_t bgcolor;
					color_t fgcolor;
				}attr_;
			};
		}//end namespace button
	}//end namespace drawerbase

		                 /// define a button widget and provides the interfaces to be operational
		class button
			: public widget_object<category::widget_tag, drawerbase::button::trigger>
		{
			typedef widget_object<category::widget_tag, drawerbase::button::trigger> base_type;
		public:
			typedef drawerbase::button::trigger::state state;			///< Defines the state of button actions.

			button();
			button(window, bool visible);
			button(window, const nana::string& caption, bool visible = true);
			button(window, const nana::char_t* caption, bool visible = true);
			button(window, const nana::rectangle& = rectangle(), bool visible = true);
			button& icon(const nana::paint::image&);
			button& image(const char_t * filename);		    	         ///< Sets a background image.
			button& image(const nana::paint::image&);	    	         ///< Sets a background image.
			button& image_enable(state, bool);	    	         ///< Enables/disables a state. The button will be drawn defautly if the state is disabled.
			button& image_join(state target, state from);                ///< The state from joins the state to.
			button& image_stretch(nana::arrange, int beg, int end);      ///< Sets the stretch part of image. The image is stretched when the size of button is changed.
			button& image_valid_area(nana::arrange, const nana::rectangle&);  ///< Sets an area of the image which is painted as background.

			button& enable_pushed(bool);
			bool pushed() const;
			button& pushed(bool);
			button& omitted(bool);	    	               ///< Enables/Disables omitting displaying the caption if the text is too long.
			button& enable_focus_color(bool);              ///< Enables/Disables showing the caption with a special color to indicate the button is focused.
		private:
			void _m_shortkey();
			void _m_complete_creation();
			void _m_caption(const nana::string&);
		};
}//end namespace gui
}//end namespace nana
#endif

/*!   \class nana::gui::button

Notes

1. The background image of button horizontally split into 5 parts and each part indicates one state. 
If the background of button is some area in the image, the area can be specified by using image_valid_area(). 
![Background image of button horizontally split into 5 parts](background_button.jpg)
For example. There is an image of size 520 * 70 pixels, and a button background image is just 470 * 23 pixels at position (0, 0).

		int main()
		{
		  nana::gui::form form;
		  nana::gui::button btn(form, 5, 5, 94, 23);
		  btn.image(STR("button_image.bmp"));
		  btn.image_valid_area(nana::arrange::horizontal, nana::rectangle(0, 0, 94 * 5, 23));
		  form.show();
		  nana::gui::exec();
		}

The button splits the area of image into 5 parts for each state. The order of parts are 
same with the order of definition of elements of enum in type button::state.
![](btn_over.jpg)
2. An image state can be disabled by calling image_enable() method. If a state is disabled, the button would averagely 
split the area of image into parts for each enabled state.
*/