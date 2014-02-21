/*
 *	A List Box Implementation
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/listbox.hpp
 *
 */

#ifndef NANA_GUI_WIDGETS_LISTBOX_HPP
#define NANA_GUI_WIDGETS_LISTBOX_HPP
#include "widget.hpp"
#include <nana/pat/cloneable.hpp>
#include <nana/concepts.hpp>
#include <cassert>

namespace nana{ namespace gui{
	class listbox;
	namespace drawerbase
	{
		namespace listbox
		{
			typedef std::size_t size_type;
			struct index_pair
			{
				size_type cat, item;
				index_pair(size_type cat_=0 , size_type item_=0 ): cat(cat_),item(item_){}
				bool operator == (index_pair idx) const {return cat==idx.cat && item == idx.item;}
				bool operator != (index_pair idx) const {return cat!=idx.cat || item != idx.item;}
			};
			struct super_index_pair: index_pair
			{
				super_index_pair(size_type cat_ = npos, size_type item_ = npos): index_pair(cat_ , item_){}
				super_index_pair(index_pair i): index_pair(i){}
				bool empty() const {return cat==npos ;}
				bool isCat() const {return item==npos && !empty();}
				bool isItem() const{return !empty() && item!=npos ;}
				operator index_pair(){ assert( isItem() ); return {cat,item};}
				bool operator == (super_index_pair idx) const 
				{
					return  cat==idx.cat && ( item == idx.item || isCat() && idx.isCat() )
						 || empty() && idx.empty()   ;
				}
				bool operator != (super_index_pair idx) const {return !operator == (idx) ;}
			};

			using selection       = std::vector<      index_pair>;   ///<A container type for \a items.
			using super_selection = std::vector<super_index_pair>;   ///<A container type for \a items AND \a categories.

			//struct essence_t
			//@brief:	this struct gives many data for listbox,
			//			the state of the struct does not effect on member funcions, therefore all data members are public.
			struct essence_t;
			class drawer_header_impl;
			class drawer_lister_impl;

			class trigger: public drawer_trigger
			{
			public:
				typedef listbox::size_type size_type;  // once again? = nana::gui::drawerbase::listbox::size_type

				trigger();
				~trigger();
				essence_t& essence();
				essence_t& essence() const;
				void draw();
			private:
				void _m_draw_border();
			private:
				void bind_window(widget_reference);

				void attached(graph_reference);
				void detached();
				void typeface_changed(graph_reference);
				void refresh(graph_reference);
				void mouse_move(graph_reference, const eventinfo&);
				void mouse_leave(graph_reference, const eventinfo&);
				void mouse_down(graph_reference, const eventinfo&);
				void mouse_up(graph_reference, const eventinfo&);
				void mouse_wheel(graph_reference, const eventinfo&);
				void dbl_click(graph_reference, const eventinfo&);
				void resize(graph_reference, const eventinfo&);
				void key_down(graph_reference, const eventinfo&);
				void key_char(graph_reference, const eventinfo&);  // I want " " to check/uncheck selected items, and Ctrl-A to select all
			private:
				essence_t * essence_;
				drawer_header_impl *drawer_header_;
				drawer_lister_impl *drawer_lister_;
			};//end class trigger

		    /// An interface that performances a translation between the object of T and an item of listbox.
			template<typename T>
			class resolver_interface
			{
			public:
				
				typedef T target;               ///< The type that will be resolved.

				/// The destructor
				virtual ~resolver_interface(){}

				virtual nana::string decode( size_type, const target&) const = 0;
				virtual void encode(target&, size_type, const nana::string&) const = 0;
			};

			template<typename T>
			struct resolver_proxy
			{
				pat::cloneable<resolver_interface<T>> res;

				resolver_proxy()
				{}

				resolver_proxy(const resolver_proxy& rhs)
					: res(rhs.res)
				{}

				~resolver_proxy()
				{
				}
			};


			class item_proxy 
				: public std::iterator<std::input_iterator_tag, item_proxy> , public index_pair
			{
			public:
				item_proxy();
				item_proxy(essence_t* ess, size_type cat, size_type pos);
				item_proxy(essence_t* ess, index_pair idx);

				bool empty() const;

				item_proxy & check(bool ck);
				bool checked() const;

				item_proxy & select(bool);
				bool selected() const;

				item_proxy & bgcolor(nana::color_t);
				nana::color_t bgcolor() const;

				item_proxy& fgcolor(nana::color_t);
				nana::color_t fgcolor() const;

				index_pair pos() const;

				size_type columns() const;
				item_proxy & text(size_type col, const nana::string&);
				item_proxy & text(size_type col, nana::string&&);
				nana::string text(size_type col) const;

				template<typename T>
				item_proxy & resolve_from(const T& t)
				{
					auto proxy = _m_resolver().template get<resolver_proxy<T> >();
					if(nullptr == proxy)
						throw std::invalid_argument("Nana.Listbox.ItemProxy: the type passed to value() does not match the resolver.");
					
					auto * res = proxy->res.get();
					const size_type headers = columns();

					for(size_type i = 0; i < headers; ++i)
						text(i, res->decode(i, t));
					
					return *this;
				}

				template<typename T>
				void resolve_to(T& t) const
				{
					auto proxy = _m_resolver().template get<resolver_proxy<T> >();
					if(nullptr == proxy)
						throw std::invalid_argument("Nana.Listbox.ItemProxy: the type passed to value() does not match the resolver.");
					
					auto * res = proxy->res.get();
					const size_type headers = columns();
					for(size_type i = 0; i < headers; ++i)
						res->encode(t, i, text(i));
				}

				template<typename T>
				T* value_ptr() const
				{
					auto * pany = _m_value();
					return (pany ? pany->template get<T>() : nullptr);
				}

				template<typename T>
				T & value() const
				{
					auto * pany = _m_value();
					if(nullptr == pany)
						throw std::runtime_error("treebox::item_proxy.value<T>() is empty");

					T * p = pany->template get<T>();
					if(nullptr == p)
						throw std::runtime_error("treebox::item_proxy.value<T>() invalid type of value");
					return *p;
				}

				template<typename T>
				item_proxy & value(const T& t)
				{
					*_m_value(true) = t;
					return *this;
				}

				/// Behavior of Iterator's value_type
				bool operator==(const nana::string& s) const;
				bool operator==(const char * s) const;
				bool operator==(const wchar_t * s) const;

				/// Behavior of Iterator
				item_proxy & operator=(const item_proxy&);

				/// Behavior of Iterator
				item_proxy & operator++();

				/// Behavior of Iterator
				item_proxy	operator++(int);

				/// Behavior of Iterator
				item_proxy& operator*();

				/// Behavior of Iterator
				const item_proxy& operator*() const;

				/// Behavior of Iterator
				item_proxy* operator->();

				/// Behavior of Iterator
				const item_proxy* operator->() const;

				/// Behavior of Iterator
				bool operator==(const item_proxy&) const;

				/// Behavior of Iterator
				bool operator!=(const item_proxy&) const;

				//Undocumented method
				essence_t * _m_ess() const;
			private:
				const nana::any & _m_resolver() const;
				nana::any * _m_value(bool alloc_if_empty);
				const nana::any * _m_value() const;
			private:
				essence_t * ess_;
				//std::size_t cat_;
				//std::size_t pos_;
			};

			class cat_proxy
				: public std::iterator<std::input_iterator_tag, cat_proxy>
			{
			public:
				cat_proxy();
				cat_proxy(essence_t * ess, size_type cat_pos);

				/// Append an item at end of the category
				template<typename T>
				item_proxy append(const T& t)
				{
					auto proxy = _m_resolver().template get<resolver_proxy<T> >();
					if(proxy)
					{
						auto & res = proxy->res;
						index_pair idx( pos_ , size()    );                // an iterator to the next created item
						push_back(res->decode( 0, t)  );                   // Create a new item and Set txt in first column
						item_proxy ip(ess_, idx  );                        // an iterator to the last created item
						const size_type headers = columns();
						for(size_type i = 1; i < headers; ++i) // Set txt in the rest of the columns
							ip.text(i, res->decode(i, t));
						return ip;
					}
					return item_proxy();
				}

				std::size_t columns() const;

				/// Behavior of a container
				void push_back(const nana::string&);
				void push_back(nana::string&&);

				item_proxy begin() const;
				item_proxy end() const;
				item_proxy cbegin() const;
				item_proxy cend() const;

				item_proxy at( size_type pos) const;
				item_proxy back() const;

				size_type size() const;      // how many cat?

				/// Behavior of Iterator
				cat_proxy& operator=(const cat_proxy&);

				/// Behavior of Iterator
				cat_proxy & operator++();

				/// Behavior of Iterator
				cat_proxy	operator++(int);

				/// Behavior of Iterator
				cat_proxy& operator*();

				/// Behavior of Iterator
				const cat_proxy& operator*() const;

				/// Behavior of Iterator
				cat_proxy* operator->();

				/// Behavior of Iterator
				const cat_proxy* operator->() const;

				/// Behavior of Iterator
				bool operator==(const cat_proxy&) const;

				/// Behavior of Iterator
				bool operator!=(const cat_proxy&) const;
			private:
				const nana::any & _m_resolver() const;
			private:
				essence_t * ess_;
				nana::gui::drawerbase::listbox::size_type pos_;
			};

			struct extra_events
			{
				nana::fn_group<void(item_proxy, bool)> checked;
				nana::fn_group<void(item_proxy, bool)> selected;
			};
		}
	}//end namespace drawerbase
/*!\class nana::gui::listbox
\brief A rectangle containing a list of strings from which the user can select. This widget contain a list of \a categories, with in turn contain \a items. 
A category is a text with can be \a selected, \a checked and \a expanded to show the items.
An item is formed by \a column-fields, each corresponding to one of the \a headers. 
An item can be \a selected and \a checked.
The user can \a drag the header to \a reisize it or to \a reorganize it. 
By \a clicking on a header the list get \a reordered, first up, and then down alternatively,

\example listbox_Resolver.cpp

*/
	class listbox
		:	public widget_object<category::widget_tag, drawerbase::listbox::trigger>,
			public concepts::any_objective<drawerbase::listbox::size_type, 2>
	{
	public:
		typedef drawerbase::listbox::size_type size_type;
		typedef drawerbase::listbox::index_pair index_pair;
		typedef drawerbase::listbox::super_index_pair super_index_pair;
		typedef drawerbase::listbox::selection selection;                ///<A container type for items.
		typedef drawerbase::listbox::super_selection super_selection;  	 ///<A container type for items AND categories.

		typedef std::pair<size_type, size_type>	index_pair_t;

		typedef drawerbase::listbox::extra_events ext_event_type;
		typedef drawerbase::listbox::cat_proxy	cat_proxy;
		typedef drawerbase::listbox::item_proxy item_proxy;
		//typedef std::vector<index_pair_t> selection;

		      /// An interface that performances a translation between the object of T and an item of listbox.
		template<typename T>
		class resolver_interface
			: public drawerbase::listbox::resolver_interface<T>
		{
		};
	public:
		listbox();
		listbox(window, bool visible);
		listbox(window, const rectangle& = rectangle(), bool visible = true);

		ext_event_type& ext_event() const;

		void auto_draw(bool);                                ///<Set state: Redraw automatically after an operation?

		void append_header(const nana::string &header_txt, unsigned width = 120);///<Appends a new column with a header text and the specified width at the end

		cat_proxy append(const nana::string& text);          ///<Appends a new category at the end
		cat_proxy at(size_type pos) const;
		item_proxy at(size_type pos, size_type index) const;
		item_proxy at( index_pair idx) const{return at(idx.cat, idx.item);} // npos???

		void insert(size_type cat, size_type index, const nana::string&first_col_txt);///<Insert a new item with a text in the first column.
		// npos???
		void insert( index_pair idx, const nana::string& col1){return insert(idx.cat, idx.item, col1);}///<Insert a new item with a text in the first column.

		void checkable(bool);
		selection checked() const;                         ///<Returns the items which are checked.                       

		void clear(size_type cat);                         ///<Removes all the items from the specified category
		void clear();                                      ///<Removes all the items from all categories
		void erase(size_type cat);                         ///<Erases a category
		void erase();                                      ///<Erases all categories.
		item_proxy erase(item_proxy);

		template<typename Resolver>
		void resolver(const Resolver & res)
		{
			drawerbase::listbox::resolver_proxy<typename Resolver::target> proxy;
			proxy.res = pat::cloneable<drawerbase::listbox::resolver_interface<typename Resolver::target> >(res);
			_m_resolver(nana::any(proxy));
		}

		            ///Sets a strick weak ordering comparer for a column
		void set_sort_compare(size_type sub, std::function<bool(const nana::string&, nana::any*, 
			                                                    const nana::string&, nana::any*, bool reverse)> strick_ordering);
		void sort_col(size_type col, bool reverse = false);
		std::size_t sort_col() const;
		void unsort();
		bool freeze_sort(bool freeze);

		selection selected() const;                         ///<Get the indexs of all the selected items

		void show_header(bool);
		bool visible_header() const;
		void move_select(bool upwards);                     ///<Selects an item besides the current selected item.
		void icon(size_type cat, size_type index, const nana::paint::image&);
		void icon(index_pair idx, const nana::paint::image&);

		nana::paint::image icon(size_type cat, size_type index) const;
		nana::paint::image icon(index_pair idx) const;

		size_type size_categ() const;                      ///<Get the number of categories
		size_type size_item() const;                       ///<The number of items in the default category 
		size_type size_item(size_type cat) const;          ///<The number of items in category "cat"
	private:
		nana::any* _m_anyobj(size_type cat, size_type index, bool allocate_if_empty) const;
		nana::any* _m_anyobj(index_pair idx, bool allocate_if_empty) const;

		void _m_resolver(const nana::any&);
		const nana::any & _m_resolver() const;
		size_type _m_headers() const;
	};
}//end namespace gui
}//end namespace nana

#endif
