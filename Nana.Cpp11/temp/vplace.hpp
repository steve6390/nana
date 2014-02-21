/*
 *	A "virtual" Implementation of Place for Layout
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/place.cpp
 */

#ifndef NANA_GUI_vPLACE_HPP
#define NANA_GUI_vPLACE_HPP
#include <utility>
#include <nana/gui/basis.hpp>
#include <limits>
#include <nana\gui\programming_interface.hpp>
#include <string>


namespace nana {namespace gui
{
	namespace vplace_impl
    {
        struct IField ;
        struct implement;
    }
    using  vplace_impl::IField ;
    using  vplace_impl::implement ;

    class vplace		: noncopyable
	{
	  public:
        struct minmax
        {
            minmax(unsigned Min=MIN, unsigned Max=MAX);
            unsigned min,max /*, Min(), Max()*/;
            void     MinMax      (minmax Size_Range) 
                {
                    min=Size_Range.min; 
                    max=Size_Range.max;
                };
            void     MinMax      (unsigned min_,unsigned max_=MAX) {min=min_; max=max_;} 
            minmax   MinMax      (                 )  { return *this;};
            static const unsigned MIN=0,MAX=1000000;
        };
		struct field_t
		{
			virtual field_t& operator<<(minmax              Size_range)	= 0;
            virtual field_t& operator<<(IField *            fld)		= 0;
            virtual field_t& operator<<(const std::wstring& txt)		= 0;
            virtual field_t& operator<<(const std::string&  txt)		= 0;
			virtual field_t& operator<<(window              wd)		= 0;    
			virtual field_t& operator<<(unsigned            gap)  	= 0;
			virtual field_t& fasten(window wd)	    = 0;
			virtual ~field_t()                      = 0;
		};

		typedef field_t &             field_reference;

		 vplace(window parent_widget);
		 vplace();
		/** @brief Bind to a window
		 *	@param handle	A handle to a window which the place wants to attach.
		 *	@remark	It will throw an exception if the place has already binded to a window.
		 */
		void               bind     (window parent_widget);
		~vplace();

		void               div      (const ::std::string & layout);
		void               collocate();
		field_reference    field   (const ::std::string & name);    /// TODO: Add min and max

  //      struct element{};
  //      vplace& operator ()  (const ::std::string & name){ Set_def_field(name); return *this; }
		//vplace& operator<<(minmax              Size_range)	;
  //      vplace& operator<<(const std::wstring& txt)	;
  //      vplace& operator<<(const std::string&  txt)	;
		//vplace& operator<<(window              wd)	;    
		//vplace& operator<<(unsigned            gap) ;
		//vplace& fasten(window wd)	    ;
  //      vplace& operator<<(element             e )	;
		//element    Fixed   (window wd, unsigned size         );
		//element    Percent (window wd, double   percent_ , minmax MinMax=minmax()    );
		//element    Room    (window wd, unsigned width, unsigned height);/// TODO: Add min and max
                                                        /// Use room (wd,w,h) in combination with a <Table grid[W,H]>


		static IField*     fixed   (window wd                    , unsigned size    );
		static IField*     fixed   (const std::wstring& txt      , unsigned size    );
		static IField*     fixed   (const std::string&  txt      , unsigned size    );
		static IField*     percent (window wd, double   percent_ , minmax MinMax=minmax()    );
        /// Use room (wd,w,h) in combination with a <Table grid[W,H]>
		static IField*     room    (window wd, unsigned width, unsigned height);/// TODO: Add min and max

	  private:
		implement * impl_;
        //void Set_def_field  (const ::std::string & name);   
	};
}//end namespace gui
}//end namespace nana

#endif //#ifndef NANA_GUI_PLACE_HPP
