/*
 *	A Scroll Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/scroll.hpp
 */
#ifndef NANA_GUI_WIDGET_SCROLL_HPP
#define NANA_GUI_WIDGET_SCROLL_HPP

#include "widget.hpp"
#include <nana/paint/gadget.hpp>
#include <nana/gui/timer.hpp>

namespace nana{ namespace gui{

	namespace drawerbase
	{
		namespace scroll
		{
			enum class buttons
			{
				none, forward, backward, scroll, first, second
			};

			struct metrics_type
			{
				typedef std::size_t size_type;

				size_type peak;
				size_type range;
				size_type step;
				size_type value;

				buttons what;
				bool pressed;
				size_type	scroll_length;
				int			scroll_pos;
				int			scroll_mouse_offset;

				metrics_type();
			};

			class drawer
			{
			public:
				struct states
				{
					enum{none, highlight, actived, selected};
				};

				typedef nana::paint::graphics& graph_reference;
				const static unsigned fixedsize = 16;

				drawer(metrics_type& m);
				void set_vertical(bool);
				buttons what(graph_reference, int x, int y);
				void scroll_delta_pos(graph_reference, int);
				void auto_scroll();
				void draw(graph_reference, buttons);
			private:
				bool _m_check() const;
				void _m_adjust_scroll(graph_reference);
				void _m_background(graph_reference);
				void _m_button_frame(graph_reference, int x, int y, unsigned width, unsigned height, int state);
				void _m_draw_scroll(graph_reference, int state);
				void _m_draw_button(graph_reference, int x, int y, unsigned width, unsigned height, buttons what, int state);
			private:
				metrics_type &metrics_;
				bool	vertical_;
			};

			template<bool Vertical>
			class trigger
				: public nana::gui::drawer_trigger
			{
			public:
				typedef metrics_type::size_type size_type;

				trigger()
					: graph_(nullptr), drawer_(metrics_)
				{
					drawer_.set_vertical(Vertical);
				}

				const metrics_type& metrics() const
				{
					return metrics_;
				}

				void peak(size_type s)
				{
					if(graph_ && (metrics_.peak != s))
					{
						metrics_.peak = s;
						API::refresh_window(widget_->handle());
					}
				}

				void value(size_type s)
				{
					if(graph_ && (metrics_.value != s))
					{
						metrics_.value = s;
						API::refresh_window(widget_->handle());
					}
				}

				void range(size_type s)
				{
					if(graph_ && (metrics_.range != s))
					{
						metrics_.range = s;
						API::refresh_window(widget_->handle());
					}
				}

				void step(size_type s)
				{
					metrics_.step = s;
				}

				bool make_step(bool forward, unsigned multiple)
				{
					if(graph_)
					{
						size_type step = (multiple > 1 ? metrics_.step * multiple : metrics_.step);
						size_type value = metrics_.value;
						if(forward)
						{
							size_type maxv = metrics_.peak - metrics_.range;
							if(metrics_.peak > metrics_.range && value < maxv)
							{
								if(maxv - value >= step)
									value += step;
								else
									value = maxv;
							}
						}
						else if(value)
						{
							if(value > step)
								value -= step;
							else
								value = 0;
						}
						size_type cmpvalue = metrics_.value;
						metrics_.value = value;
						return (value != cmpvalue);
					}
					return false;
				}
			private:
				void bind_window(widget_reference widget)
				{
					widget_ = &widget;
					widget.caption(STR("Nana Scroll"));

					using namespace API::dev;
					window wd = widget.handle();
					make_drawer_event<events::mouse_enter>(wd);
					make_drawer_event<events::mouse_move>(wd);
					make_drawer_event<events::mouse_down>(wd);
					make_drawer_event<events::mouse_up>(wd);
					make_drawer_event<events::mouse_leave>(wd);
					make_drawer_event<events::mouse_wheel>(wd);
					make_drawer_event<events::size>(wd);

					timer_.make_tick(nana::make_fun(*this, &trigger::_m_tick));
					timer_.enable(false);
				}

				void attached(graph_reference graph)
				{
					graph_ = &graph;
				}

				void detached()
				{
					API::dev::umake_drawer_event(widget_->handle());
					graph_ = nullptr;
				}

				void refresh(graph_reference graph)
				{
					drawer_.draw(graph, metrics_.what);
				}

				void resize(graph_reference graph, const nana::gui::eventinfo& ei)
				{
					drawer_.draw(graph, metrics_.what);
					API::lazy_refresh();
				}

				void mouse_enter(graph_reference graph, const nana::gui::eventinfo& ei)
				{
					metrics_.what = drawer_.what(graph, ei.mouse.x, ei.mouse.y);
					drawer_.draw(graph, metrics_.what);
					API::lazy_refresh();
				}

				void mouse_move(graph_reference graph, const nana::gui::eventinfo& ei)
				{
					bool redraw = false;
					if(metrics_.pressed && (metrics_.what == buttons::scroll))
					{
						redraw = true;
						drawer_.scroll_delta_pos(graph, (Vertical ? ei.mouse.y : ei.mouse.x));
					}
					else
					{
						buttons what = drawer_.what(graph, ei.mouse.x, ei.mouse.y);
						if(metrics_.what != what)
						{
							redraw = true;
							metrics_.what = what;
						}
					}
					if(redraw)
					{
						drawer_.draw(graph, metrics_.what);
						API::lazy_refresh();
					}
				}

				void mouse_down(graph_reference graph, const nana::gui::eventinfo& ei)
				{
					if(ei.mouse.left_button)
					{
						metrics_.pressed = true;
						metrics_.what = drawer_.what(graph, ei.mouse.x, ei.mouse.y);
						switch(metrics_.what)
						{
						case buttons::first:
						case buttons::second:
							this->make_step(metrics_.what == buttons::second, 1);
							timer_.interval(1000);
							timer_.enable(true);
							break;
						case buttons::scroll:
							API::capture_window(widget_->handle(), true);
							metrics_.scroll_mouse_offset = (Vertical ? ei.mouse.y : ei.mouse.x) - metrics_.scroll_pos;
							break;
						case buttons::forward:
						case buttons::backward:
							drawer_.auto_scroll();
							break;
						default:	//Ignore buttons::none
							break;
						}
						drawer_.draw(graph, metrics_.what);
						API::lazy_refresh();
					}
				}

				void mouse_up(graph_reference graph, const nana::gui::eventinfo& ei)
				{
					timer_.enable(false);
					
					API::capture_window(widget_->handle(), false);

					metrics_.pressed = false;

					metrics_.what = drawer_.what(graph, ei.mouse.x, ei.mouse.y);
					drawer_.draw(graph, metrics_.what);
					API::lazy_refresh();
				}

				void mouse_leave(graph_reference graph, const nana::gui::eventinfo& ei)
				{
					if(metrics_.pressed) return;

					metrics_.what = buttons::none;
					drawer_.draw(graph, buttons::none);
					API::lazy_refresh();
				}

				void mouse_wheel(graph_reference graph, const nana::gui::eventinfo& ei)
				{
					if(this->make_step(ei.wheel.upwards == false, 3))
					{
						drawer_.draw(graph, metrics_.what);
						API::lazy_refresh();
					}
				}
			private:
				void _m_tick()
				{
					this->make_step(metrics_.what == buttons::second, 1);
					API::refresh_window(widget_->handle());
					timer_.interval(100);
				}
			private:
				nana::gui::widget * widget_;
				nana::paint::graphics * graph_;
				metrics_type metrics_;
				drawer	drawer_;
				nana::gui::timer timer_;
			};
		}//end namespace scroll
	}//end namespace drawerbase

	template<bool Vertical>
	class scroll
		: public widget_object<category::widget_tag, drawerbase::scroll::trigger<Vertical> >
	{
		typedef widget_object<category::widget_tag, drawerbase::scroll::trigger<Vertical> > base_type;
	public:
		typedef std::size_t size_type;

		scroll(){}

		scroll(window wd, bool visible)
		{
			this->create(wd, rectangle(), visible);
		}

		scroll(window wd, const rectangle& r, bool visible = true)
		{
			this->create(wd, r, visible);
		}

		size_type amount() const
		{
			return this->get_drawer_trigger().metrics().peak;
		}

		void amount(size_type Max)
		{
			return this->get_drawer_trigger().peak(Max);
		}

		size_type range() const
		{
			return this->get_drawer_trigger().metrics().range;
		}

		void range(size_type r)
		{
			return this->get_drawer_trigger().range(r);
		}

		size_type value() const
		{
			return this->get_drawer_trigger().metrics().value;
		}

		void value(size_type s)
		{
			return this->get_drawer_trigger().value(s);
		}

		size_type step() const
		{
			return this->get_drawer_trigger().metrics().step;
		}

		void step(size_type s)
		{
			return this->get_drawer_trigger().step(s);
		}

		bool make_step(bool forward)
		{
			if(this->get_drawer_trigger().make_step(forward, 1))
			{
				API::refresh_window(this->handle());
				return true;
			}
			return false;
		}

		bool make_scroll(bool forward)
		{
			if(this->get_drawer_trigger().make_step(forward, 3))
			{
				API::refresh_window(this->handle());
				return true;
			}
			return false;
		}
	};//end class scroll

}//end namespace gui
}//end namespace nana

#endif