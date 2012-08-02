/*
 *	A date chooser Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/date_chooser.cpp
 */

#include <nana/gui/widgets/date_chooser.hpp>
#include <nana/paint/gadget.hpp>
#include <nana/system/platform.hpp>

namespace nana{ namespace gui{

	namespace drawerbase
	{
		namespace date_chooser
		{
			//class trigger: public gui::drawer_trigger

				trigger::trigger()
					: widget_(nullptr), chose_(false), page_(PageDate), pos_(WhereNone)
				{
					const nana::string ws[] = {STR("S"), STR("M"), STR("T"), STR("W"), STR("T"), STR("F"), STR("S")};
					const nana::string ms[] = {STR("January"), STR("February"), STR("March"), STR("April"), STR("May"), STR("June"), STR("July"), STR("August"), STR("September"), STR("October"), STR("November"), STR("December")};

					for(int i = 0; i < 7; ++i)	weekstr_[i] = ws[i];
					for(int i = 0; i < 12; ++i) monthstr_[i] = ms[i];

					nana::date d;
					chdate_.year = chmonth_.year = d.read().year;
					chdate_.month = chmonth_.month = d.read().month;
					chdate_.day = d.read().day;
				}

				bool trigger::chose() const
				{
					return chose_;
				}

				nana::date trigger::read() const
				{
					return nana::date(chdate_.year, chdate_.month, chdate_.day);
				}

				void trigger::week_name(unsigned index, const nana::string& str)
				{
					if(0 <= index && index < 7)
						this->weekstr_[index] = str;
				}

				void trigger::month_name(unsigned index, const nana::string& str)
				{
					if(0 <= index && index < 12)
						this->monthstr_[index] = str;
				}

				void trigger::_m_init_color()
				{
					color_.selected = 0x2F3699;
					color_.highlight = 0x4D56C8;
					color_.normal = 0x0;
					color_.bkcolor = 0x88C4FF;
				}

				int trigger::_m_pos_where(trigger::graph_reference graph, int x, int y)
				{
					int xend = static_cast<int>(graph.width()) - 1;
					int yend = static_cast<int>(graph.height()) - 1;
					if(0 < y && y < static_cast<int>(topbar_height))
					{
						if(static_cast<int>(border_size) < x && x < xend)
						{
							if(x < border_size + 16)
								return WhereLeftButton;
							else if(xend - border_size - 16 < x)
								return WhereRightButton;
							else
								return WhereTopbar;
						}
						else
							return WhereNone;
					}
					else if(topbar_height < y && y < yend)
					{
						trace_pos_.x = x;
						trace_pos_.y = y;
						return WhereTextArea;
					}
					return WhereNone;
				}

				void trigger::_m_draw(trigger::graph_reference graph)
				{
					_m_init_color();

					const unsigned width = graph.width() - 2;

					graph.rectangle(0xB0B0B0, false);
					graph.rectangle(1, 1, width, topbar_height, 0xFFFFFF, true);

					_m_draw_topbar(graph);

					if(graph.height() > 2 + topbar_height)
					{
						nana::point refpos(1, static_cast<int>(topbar_height) + 1);

						unsigned height = graph.height() - 2 - topbar_height;
						nana::paint::graphics gbuf(width, height);
						gbuf.rectangle(0xF0F0F0, true);

						switch(page_)
						{
						case PageDate:
							_m_draw_days(refpos, gbuf);
							break;
						case PageMonth:
							_m_draw_months(refpos, gbuf);
							break;
						}

						graph.bitblt(refpos.x, refpos.y, gbuf);
					}
				}

				void trigger::_m_draw_topbar(trigger::graph_reference graph)
				{
					int ypos = (topbar_height - 16) / 2 + 1;

					const nana::color_t color = color_.normal;

					nana::paint::gadget::arrow_16_pixels(graph, border_size, ypos, (pos_ == WhereLeftButton ? color_.highlight : color), 1, nana::paint::gadget::directions::to_west);
					nana::paint::gadget::arrow_16_pixels(graph, graph.width() - (border_size + 16 + 1), ypos, (pos_ == WhereRightButton ? color_.highlight : color), 1, nana::paint::gadget::directions::to_east);

					if(graph.width() > 32 + border_size * 2)
					{
						std::stringstream ss;
						ss<<chmonth_.year;
						nana::string str;
						if(page_ == PageDate)
						{
							str += monthstr_[chmonth_.month - 1];
							str += STR("  ");
						}
						str += nana::charset(ss.str());

						nana::size txt_s = graph.text_extent_size(str);

						ypos = (topbar_height - txt_s.height) / 2 + 1;

						int xpos = (graph.width() - txt_s.width) / 2;
						if(xpos < border_size + 16) xpos = 16 + border_size + 1;

						graph.string(xpos, ypos, (pos_ == WhereTopbar ? color_.highlight : color), str);
					}
				}

				void trigger::_m_make_drawing_basis(trigger::drawing_basis& dbasis, trigger::graph_reference graph, const nana::point& refpos)
				{
					dbasis.refpos = refpos;
					const unsigned width = graph.width();
					const unsigned height = graph.height();

					if(PageDate == page_)
					{
						dbasis.line_s = height / 7.0;
						dbasis.row_s = width / 7.0;
					}
					else if(PageMonth == page_)
					{
						dbasis.line_s = height / 3.0;
						dbasis.row_s = width / 4.0;
					}

					dbasis_ = dbasis;
				}

				void trigger::_m_draw_pos(trigger::drawing_basis & dbasis, trigger::graph_reference graph, int x, int y, const nana::string& str, bool primary, bool sel)
				{
					nana::point refpos;
					refpos.x = static_cast<int>(x * dbasis.row_s);
					refpos.y = static_cast<int>(y * dbasis.line_s);

					int width = static_cast<int>(dbasis.row_s);
					int height = static_cast<int>(dbasis.line_s);

					nana::color_t color = color_.normal;

					nana::point tpos = trace_pos_;
					tpos.x -= dbasis.refpos.x;
					tpos.y -= dbasis.refpos.y;

					if(pos_ == WhereTextArea
						&& refpos.x <= tpos.x
						&& tpos.x < refpos.x + width
						&& refpos.y <= tpos.y
						&& tpos.y < refpos.y + height)
					{
						if(page_ != PageDate || y)
						{
							color = color_.highlight;
							graph.rectangle(refpos.x, refpos.y, width, height, color_.bkcolor, true);
						}
					}

					if(sel)
					{
						color = color_.highlight;
						graph.rectangle(refpos.x, refpos.y, width, height, color_.bkcolor, true);
						graph.rectangle(refpos.x, refpos.y, width, height, color_.selected, false);
					}

					x = refpos.x + width / 2;
					y = refpos.y + height / 2;

					nana::size txt_s = graph.text_extent_size(str);
					x -= txt_s.width / 2;
					y -= txt_s.height / 2;

					if(primary == false)
						color = 0xB0B0B0;

					graph.string(x, y, color, str);
				}

				void trigger::_m_draw_pos(trigger::drawing_basis & dbasis, trigger::graph_reference graph, int x, int y, int number, bool primary, bool sel)
				{
					std::stringstream ss;
					ss<<number;
					_m_draw_pos(dbasis, graph, x, y, nana::charset(ss.str()), primary, sel);
				}

				void trigger::_m_draw_ex_days(trigger::drawing_basis & dbasis, trigger::graph_reference graph, int begx, int begy, bool before)
				{
					int x = nana::date::day_of_week(chmonth_.year, chmonth_.month, 1);
					int y = (x ? 1 : 2);

					if(before)
					{
						int year = chmonth_.year;
						int month = chmonth_.month - 1;
						if(month == 0)
						{
							--year;
							month = 12;
						}
						bool same = (chdate_.year == year && chdate_.month == month);
						int days = nana::date::month_days(year, month);

						int size = (x ? x : 7);
						int beg = days - size + 1;

						for(int i = 0; i < size; ++i)
						{
							this->_m_draw_pos(dbasis, graph, i, 1, beg + i, false, same && (chdate_.day == beg + i));
						}
					}
					else
					{
						int year = chmonth_.year;
						int month = chmonth_.month + 1;
						if(month == 13)
						{
							++year;
							month = 1;
						}
						bool same = (chdate_.year == year && chdate_.month == month);

						int day = 1;
						x = begx;
						for(y = begy; y < 7; ++y)
						{
							for(; x < 7; ++x)
							{
								_m_draw_pos(dbasis, graph, x, y, day, false, same && (chdate_.day == day));
								++day;
							}
							x = 0;
						}
					}
				}

				void trigger::_m_draw_days(const nana::point& refpos, trigger::graph_reference graph)
				{
					drawing_basis dbasis;
					_m_make_drawing_basis(dbasis, graph, refpos);

					for(int i = 0; i < 7; ++i)
						_m_draw_pos(dbasis, graph, i, 0, weekstr_[i], true, false);

					int day = 1;
					int x = nana::date::day_of_week(chmonth_.year, chmonth_.month, 1);
					int y = (x ? 1 : 2);

					//draw the days that before the first day of this month
					this->_m_draw_ex_days(dbasis, graph, 0, 0, true);
					//
					int days = static_cast<int>(nana::date::month_days(chmonth_.year, chmonth_.month));

					bool same = (chdate_.year == chmonth_.year && chdate_.month == chmonth_.month);
					while(day <= days)
					{
						for(; x < 7; ++x)
						{
							_m_draw_pos(dbasis, graph, x, y, day, true, (same && chdate_.day == day));
							if(++day > days) break;
						}
						if(day > days) break;
						y++;
						x = 0;
					}

					++x;
					if(x >= 7)
					{
						x = 0;
						++y;
					}

					this->_m_draw_ex_days(dbasis, graph, x, y, false);
				}

				void trigger::_m_draw_months(const nana::point& refpos, trigger::graph_reference graph)
				{
					drawing_basis dbasis;
					_m_make_drawing_basis(dbasis, graph, refpos);

					for(int y = 0; y < 3; ++y)
						for(int x = 0; x < 4; ++x)
						{
							int index = x + y * 4;
							_m_draw_pos(dbasis, graph, x, y, monthstr_[index], true, (chmonth_.year == chdate_.year) && (index + 1 == chdate_.month));
						}
				}

				bool trigger::_m_get_trace(int x, int y, int & res)
				{
					x -= dbasis_.refpos.x;
					y -= dbasis_.refpos.y;

					int lines;
					int rows;
					if(page_ == PageDate)
					{
						lines = rows = 7;
					}
					else if(page_ == PageMonth)
					{
						lines = 3;
						rows = 4;
					}

					int width = static_cast<int>(dbasis_.row_s * rows);
					int height = static_cast<int>(dbasis_.line_s * lines);

					if(0 <= x && x < width && 0 <= y && y < height)
					{
						x = static_cast<int>(x / dbasis_.row_s);
						y = static_cast<int>(y / dbasis_.line_s);

						int n = y * rows + x + 1;
						if(page_ == PageDate)
						{
							if(n < 8) return false; //Here is week title bar
							n -= 7;

							int dw = nana::date::day_of_week(chmonth_.year, chmonth_.month, 1);
							if(dw)
								n -= dw;
							else
								n -= 7;
						}
						res = n;
						return true;
					}
					return false;
				}

				void trigger::_m_perf_transform(int tfid, nana::paint::graphics& graph,  nana::paint::graphics& dirtybuf, nana::paint::graphics& newbuf, const nana::point& refpos)
				{
					const int sleep_time = 15;
					const int count = 20;
					double delta = dirtybuf.width() / double(count);
					double delta_h = dirtybuf.height() / double(count);
					double fade = 1.0 / count;

					if(tfid == TransformToRight)
					{
						for(int i = 1; i < count; ++i)
						{
							int off_x = static_cast<int>(delta * i);
							graph.bitblt(refpos.x + off_x, refpos.y, dirtybuf.width() - off_x, dirtybuf.height(), dirtybuf, 0, 0);
							graph.bitblt(refpos.x, refpos.y, off_x, newbuf.height(), newbuf, dirtybuf.width() - off_x, 0);

							nana::gui::API::update_window(widget_->handle());
							nana::system::sleep(sleep_time);
						}
					}
					else if(tfid == TransformToLeft)
					{
						double delta = dirtybuf.width() / double(count);

						for(int i = 1; i < count; ++i)
						{
							int off_x = static_cast<int>(delta * i);
							int less_w = dirtybuf.width() - off_x;
							graph.bitblt(refpos.x, refpos.y, less_w, dirtybuf.height(), dirtybuf, off_x, 0);
							graph.bitblt(refpos.x + less_w, refpos.y, off_x, newbuf.height(), newbuf, 0, 0);

							nana::gui::API::update_window(widget_->handle());
							nana::system::sleep(sleep_time);
						}
					}
					else if(tfid == TransformLeave)
					{
						nana::paint::graphics dzbuf(newbuf.width(), newbuf.height());
						nana::paint::graphics nzbuf(newbuf.width(), newbuf.height());

						for(int i = 1; i < count; ++i)
						{
							int zo_width = static_cast<int>(newbuf.width() - delta * i);
							int zo_height = static_cast<int>(newbuf.height() - delta_h * i);
							int dx = (static_cast<int>(newbuf.width()) - zo_width) / 2;
							int dy = (static_cast<int>(newbuf.height()) - zo_height) / 2;

							dzbuf.rectangle(0xFFFFFF, true);
							dirtybuf.stretch(dzbuf, nana::rectangle(dx, dy, zo_width, zo_height));

							zo_width = static_cast<int>(newbuf.width() + delta * (count - i));
							zo_height = static_cast<int>(newbuf.height() + delta_h * (count - i));
							dx = (static_cast<int>(newbuf.width()) - zo_width) / 2;
							dy = (static_cast<int>(newbuf.height()) - zo_height) / 2;
							newbuf.stretch(nzbuf, nana::rectangle(dx, dy, zo_width, zo_height));

							nzbuf.blend(dzbuf, 0, 0, fade * (count - i));
							graph.bitblt(refpos.x, refpos.y, dzbuf);

							nana::gui::API::update_window(widget_->handle());

							nana::system::sleep(sleep_time);
						}
					}
					else if(tfid == TransformEnter)
					{
						nana::paint::graphics dzbuf(newbuf.width(), newbuf.height());
						nana::paint::graphics nzbuf(newbuf.width(), newbuf.height());

						for(int i = 1; i < count; ++i)
						{
							int zo_width = static_cast<int>(newbuf.width() + delta * i);
							int zo_height = static_cast<int>(newbuf.height() + delta_h * i);
							int dx = (static_cast<int>(newbuf.width()) - zo_width) / 2;
							int dy = (static_cast<int>(newbuf.height()) - zo_height) / 2;
							dirtybuf.stretch(dzbuf, nana::rectangle(dx, dy, zo_width, zo_height));

							zo_width = static_cast<int>(newbuf.width() - delta * (count - i));
							zo_height = static_cast<int>(newbuf.height() - delta_h * (count - i));
							dx = (static_cast<int>(newbuf.width()) - zo_width) / 2;
							dy = (static_cast<int>(newbuf.height()) - zo_height) / 2;
							nzbuf.rectangle(0xFFFFFF, true);
							newbuf.stretch(nzbuf, nana::rectangle(dx, dy, zo_width, zo_height));

							nzbuf.blend(dzbuf, 0, 0, fade * (count - i));
							graph.bitblt(refpos.x, refpos.y, dzbuf);

							nana::gui::API::update_window(widget_->handle());

							nana::system::sleep(sleep_time);
						}
					}

					graph.bitblt(refpos.x, refpos.y, newbuf.width(), newbuf.height(), newbuf, 0, 0);
				}

				void trigger::refresh(trigger::graph_reference graph)
				{
					_m_draw(graph);
				}

				void trigger::bind_window(trigger::widget_reference wd)
				{
					widget_ = &wd;
				}

				void trigger::attached(trigger::graph_reference graph)
				{
					window wd = widget_->handle();
					using namespace API::dev;
					make_drawer_event<nana::gui::events::mouse_move>(wd);
					make_drawer_event<nana::gui::events::mouse_leave>(wd);
					make_drawer_event<nana::gui::events::mouse_down>(wd);
					make_drawer_event<nana::gui::events::mouse_up>(wd);
				}

				void trigger::detached()
				{
					API::dev::umake_drawer_event(widget_->handle());
				}

				void trigger::mouse_move(trigger::graph_reference graph, const nana::gui::eventinfo& ei)
				{
					int pos = this->_m_pos_where(graph, ei.mouse.x, ei.mouse.y);
					if(pos == pos_ && pos_ != WhereTextArea) return;
					pos_ = pos;
					_m_draw(graph);
					nana::gui::API::lazy_refresh();
				}

				void trigger::mouse_leave(trigger::graph_reference graph, const nana::gui::eventinfo&)
				{
					if(WhereNone == pos_) return;
					pos_ = WhereNone;
					_m_draw(graph);
					nana::gui::API::lazy_refresh();
				}

				void trigger::mouse_up(trigger::graph_reference graph, const nana::gui::eventinfo& ei)
				{
					bool redraw = true;
					int pos = this->_m_pos_where(graph, ei.mouse.x, ei.mouse.y);
					int tfid = TransformNone;

					if(pos == WhereTopbar)
					{
						switch(page_)
						{
						case PageDate:
							page_ = PageMonth;
							tfid = TransformLeave;
							break;
						default:
							redraw = false;
						}
					}
					else if(pos == WhereTextArea)
					{
						int ret = 0;
						switch(page_)
						{
						case PageDate:
							if(_m_get_trace(ei.mouse.x, ei.mouse.y, ret))
							{
								if(ret < 1)
								{
									if(--chmonth_.month == 0)
									{
										--chmonth_.year;
										chmonth_.month = 12;
									}
									tfid = TransformToRight;
								}
								else
								{
									int days = nana::date::month_days(chmonth_.year, chmonth_.month);
									if(ret > days)
									{
										if(++chmonth_.month == 13)
										{
											++chmonth_.year;
											chmonth_.month = 1;
										}
										tfid = TransformToLeft;
									}
									else //Selecting a day in this month
									{
										chdate_.year = chmonth_.year;
										chdate_.month = chmonth_.month;
										chdate_.day = ret;
										chose_ = true;
									}
								}
							}
							break;
						case PageMonth:
							if(_m_get_trace(ei.mouse.x, ei.mouse.y, ret))
								chmonth_.month = ret;
							page_ = PageDate;
							tfid = TransformEnter;
							break;
						default:
							redraw = false;
						}
					}
					else if(pos == WhereLeftButton || pos == WhereRightButton)
					{
						int end_m;
						int beg_m;
						int step;
						if(pos == WhereLeftButton)
						{
							end_m = 1;
							beg_m = 12;
							step = -1;
							tfid = TransformToRight;
						}
						else
						{
							end_m = 12;
							beg_m = 1;
							step = 1;
							tfid = TransformToLeft;
						}
						switch(page_)
						{
						case PageDate:
							if(chmonth_.month == end_m)
							{
								chmonth_.month = beg_m;
								chmonth_.year += step;
							}
							else
								chmonth_.month += step;
							break;
						case PageMonth:
							chmonth_.year += step;
							break;
						default:
							redraw = false;
						}
					}

					if(redraw)
					{
						if(tfid != TransformNone)
						{
							const unsigned width = graph.width() - 2;
							nana::point refpos(1, static_cast<int>(topbar_height) + 1);

							unsigned height = graph.height() - 2 - topbar_height;
							nana::paint::graphics dirtybuf(width, height);
							dirtybuf.bitblt(0, 0, width, height, graph, refpos.x, refpos.y);

							_m_draw(graph);

							nana::paint::graphics gbuf(width, height);
							gbuf.bitblt(0, 0, width, height, graph, refpos.x, refpos.y);

							this->_m_perf_transform(tfid, graph, dirtybuf, gbuf, refpos);
						}
						else
							_m_draw(graph);

						nana::gui::API::lazy_refresh();
					}
				}
			//end class trigger
		}//end namespace date_chooser
	}//end namespace drawerbase

	//class date_chooser
		date_chooser::date_chooser()
		{}

		date_chooser::date_chooser(window wd, bool visible)
		{
			create(wd, rectangle(), visible);
		}

		date_chooser::date_chooser(window wd, const nana::rectangle& r, bool visible)
		{
			this->create(wd, r, visible);
		}

		bool date_chooser::chose() const
		{
			return this->get_drawer_trigger().chose();
		}

		nana::date date_chooser::read() const
		{
			return this->get_drawer_trigger().read();
		}

		void date_chooser::weekstr(unsigned index, const nana::string& str)
		{
			this->get_drawer_trigger().week_name(index, str);
			nana::gui::API::refresh_window(this->handle());
		}

		void date_chooser::monthstr(unsigned index, const nana::string& str)
		{
			this->get_drawer_trigger().month_name(index, str);
			nana::gui::API::refresh_window(this->handle());
		}


	//end class date_chooser
}//end namespace gui
}//end namespace nana