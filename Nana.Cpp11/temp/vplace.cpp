/*
 *	An Alternative Implementation of Place for Layout
 *  (
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/place.cpp
 */
#include <sstream>
#include <cfloat>
#include <cmath>
#include <unordered_set>
#include <cassert>
//#include <unordered_map>
#include <map>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <../temp/vplace.hpp>
#include <nana/gui/wvl.hpp>
#include <nana/gui/programming_interface.hpp>
#include <iostream>    // temp, for debugging
#include <nana/gui/widgets/label.hpp>

std::ostream& operator<<(std::ostream& o,const nana::rectangle &r)
{ o<<" rect("<<r.x<<","<<r.y<<","<<r.width <<","<<r.height <<")\n"; return o;}

namespace nana{	namespace gui {

namespace vplace_impl
{
    typedef vplace::minmax  minmax;
    typedef vplace::field_t field_t;

    struct adj{unsigned weigth,min,count_adj; adj():weigth(0),min(0),count_adj(0){}  };

    struct IField  :minmax 
    {
                                IField      (){}
                                IField      (unsigned min_,unsigned max_):minmax  (min_,max_){}
            virtual adj         pre_place   (unsigned t_w,      adj& fixed=adj()                    )=0;
            virtual adj         end_place   (unsigned t_w,const adj& fixed,       adj& adj_min=adj())=0;
            virtual unsigned    weigth      (unsigned t_w,const adj& fixed, const adj& adj_min      )=0;
            virtual            ~IField      (){}
            virtual rectangle   cells       ()   const                              =0;
            virtual void        collocate   (const rectangle& r)   =0;
            virtual window      window_handle()const   =0;
            virtual void        populate_children(	implement*   place_impl_)  =0;
    };
 	class  division : public IField
	{
	  public:
        std::vector <std::string> field_names;
		std::vector<IField*>      children;   //  std::vector<div*> 
		std::vector<window>       fastened_in_div;   //  
		std::unique_ptr<IField>   gap;        //  
	  public:
        virtual int&      weigth_c(rectangle& r )=0;
        virtual unsigned& weigth_s(rectangle& r )=0;
        virtual int&       fixed_c(rectangle& r )=0;
        virtual unsigned&  fixed_s(rectangle& r )=0;

        window    window_handle    () const  override     { return nullptr; }
        rectangle cells            () const  override     {return rectangle(-1,-1,1,1);}             
                    /// populate childen in the same order in with they were introduced in the div layout str,
                    /// and then in the order in with they were added to the field
        void populate_children(	implement*   place_impl_);
        /// add field names in the same order in with they are introduced in the div layout str
		virtual void collocate( const rectangle& r) 
		{   
                                     //std::cerr<< "\ncollocating div in: "<<r; // debugg
			rectangle area (r);

            adj pre_adj, end_adj; auto t_w=weigth_s(area);
			for(auto child: children)                            
                child->pre_place(  t_w , pre_adj );  
			for(auto child: children)                            
                child->end_place(  t_w , pre_adj, end_adj );
       
			for(auto child : children)                          
			{
			    rectangle child_area (area);
                weigth_s(child_area) = child->weigth(  t_w , pre_adj, end_adj )   ;
                weigth_c(area)      += weigth_s(child_area);
                weigth_s(area)      -= weigth_s(child_area);
                                    //std::cerr<< "\ncollocating child in: "<<child_area; // debugg
                child->collocate(child_area);
			}
			for(auto & fsn: fastened_in_div)
			{	
                API::move_window(fsn, r);   // si alguien habia cerrado la w fsn despues del populate children, tendremos problemas? make unload fastened erase?
                API::show_window(fsn, API::visible(fsn));
            }
		}
        virtual ~division() 	{ }
	};

    struct Gap_field   : IField                     
    { 
        void     collocate    (const rectangle& r)override   {}
        window   window_handle() const override              { return nullptr; }
        void     populate_children(implement*   place_impl_) override {}
        rectangle   cells         () const override          {return rectangle(-1,-1,0,0);}             

        virtual  ~Gap_field (){}
    };
    struct Widget_field :  Gap_field                     
    { 
        //Widget_field    (window handle_):handle(handle_){}
        window          handle; 
        Widget_field    ():handle(nullptr){}
        virtual        ~Widget_field    (){}
        void            init (window handle_){handle=handle_;}

        window          window_handle  () const override          { return handle; }
        rectangle       cells          () const override          {return rectangle(-1,-1,1,1);}             
        void            collocate      (const rectangle& r)override
        {  
            API::move_window (handle,r ); 
            API::show_window(handle, true);
        }
    };
    struct Cell_field: virtual Widget_field    
    { 
        unsigned   row,column;
        rectangle  cells         () const override          {return rectangle(column,row,1,1);}             
        void       init(window handle_, unsigned row_,unsigned column_)
        {  Widget_field::init( handle_); row=row_ ; column=column_ ; }
    };
    struct Room_field: virtual Widget_field    
    { 
        unsigned   rows,columns;
        rectangle  cells         () const override          {return rectangle(-1,-1,columns,rows);}             
        void       init(window handle_,unsigned rows_,unsigned columns_)
        {  Widget_field::init( handle_);   rows=rows_;   columns=columns_;   }
    };
    struct Cells_field:  Cell_field, Room_field  
    { 
        rectangle  cells         () const override          {return rectangle(column,row,columns,rows);}             
        void       init(window handle_, rectangle r)
        { handle= handle_; row=r.y ; column=r.x ; rows=r.height ; columns=r.width ; }
    };
	class  div_h 	: public division
	{
	  public:
         int&      weigth_c(rectangle& r )override{return r.x;}
         unsigned& weigth_s(rectangle& r )override{return r.width;}
         int&       fixed_c(rectangle& r )override{return r.y;}
         unsigned&  fixed_s(rectangle& r )override{return r.height;}
	};
	class  div_v 	: public division
	{
	  public:
         int&      weigth_c(rectangle& r )override{return r.y;}
         unsigned& weigth_s(rectangle& r )override{return r.height;}
         int&       fixed_c(rectangle& r )override{return r.x;}
         unsigned&  fixed_s(rectangle& r )override{return r.width;}
   	};
	class  div_grid	: public div_h
	{
	  public:
        std::string name; ///< field name to be refered in the field(name)<<room instr.
        unsigned rows, columns;      ///< w=rows and h=columns   dim; 
		void Init(const std::string& name_, size dim_)	
        {  name=name_; rows=dim_.height ; columns =dim_.width ;}

		void Init(const std::string& name_, unsigned rows_, unsigned columns_)	
        {  name=name_; rows=rows_ ; columns =columns_;}

        virtual void collocate(const rectangle& r) override
	    {
            if(! r.width || ! r.height ) return;

            for(auto & fsn: fastened_in_div)
			    API::move_window(fsn, r);

            if(children.empty() ) return;

                                    /// by now asept only one field name in grid div
            assert (  field_names.size() ==1  ); 

            rectangle area(r);
		    if((rows<=1 && columns<=1) || !rows || !columns)
		    {
			    unsigned min_n_of_cells = _m_number_of_cells();

			    unsigned edge=1;
                while (edge*edge <= min_n_of_cells )
                    ++edge;
                rows=columns=edge;
		    }
		    double block_w = /*weigth_s( area )*/  area.width / double(columns );    // TODO: adapt to vert
		    double block_h = /*fixed_s( area )*/   area.height/ double(rows );
            std::vector<std::vector<size>> table(columns,std::vector<size>(rows,size(1,1)));  

		    unsigned field=0;
            size d_c, d_f;
			for(unsigned rw = 0; rw < rows ; ++rw)
		      for(unsigned clm = 0; clm < columns ; ++clm)
              {
				d_c=table[clm][rw];
                if( ! d_c.height || ! d_c.width  )       /// pass collapsed cells
						continue;

                d_f = children[field]->cells();  
                while ( ! d_f.height || ! d_f.width)      /// pass non dimension fields, the gaps
                {
                    if (++field >= children.size () )
                        return;
                    d_f = children[field]->cells(); 
				}
				if(clm + d_f.width  >= columns )
						                            d_f.width = columns - clm;    /// adjust to borders
				if(rw  + d_f.height >= rows    )
						                            d_f.height = rows - rw;

				for(unsigned cc = 0; cc < d_f.width ; ++cc)
					for(unsigned cr = 0; cr < d_f.height ; ++cr)
					{
                        size&ccell ( table[clm+cc][rw+cr] );
                        if ( ccell.height &&  ccell.width)          // cell usable? have height and width ?
                            ccell=size(0,0);                        // make it un-usable
                        else                                        // colision with allready used cells
                        {
                            d_f.width = cc;                         // limit the current room
                            break;                                  // error: allready marked as used are not unmarked !!!
                        }
					}
                table[clm][rw]=d_f;
                rectangle cell(area);
                cell.x     = area.x + static_cast<int>(clm * block_w);
                cell.y     = area.y + static_cast<int>(rw  * block_h);
                cell.width =  static_cast<unsigned>(block_w * d_f.width);
                cell.height =  static_cast<unsigned>(block_h * d_f.height);

                adj pre_adj_x, end_adj_x,  pre_adj_y, end_adj_y;
                        children[field]->pre_place(  cell.width , pre_adj_x ); 
                        children[field]->pre_place(  cell.height, pre_adj_y ); 
                if(gap) {           gap->pre_place(  cell.width , pre_adj_x ); 
                                    gap->pre_place(  cell.height, pre_adj_y );      }

                        children[field]->end_place(  cell.width , pre_adj_x, end_adj_x  ); 
                        children[field]->end_place(  cell.height, pre_adj_y, end_adj_y  ); 
                if(gap) {           gap->end_place(  cell.width , pre_adj_x, end_adj_x  ); 
                                    gap->end_place(  cell.height, pre_adj_y, end_adj_y  ); }
			    rectangle child_area (cell);
                child_area.width = children[field]->weigth(   cell.width  ,pre_adj_x, end_adj_x  )   ;
                child_area.height= children[field]->weigth(   cell.height ,pre_adj_y, end_adj_y  )   ;
                children[field]->collocate(child_area);

                if (++field >= children.size () )
                    return;
            }
    }
	  private:
		unsigned _m_number_of_cells() const
		{
			unsigned n = 0;
			for(auto & child : children)
                n += child->cells().width * child->cells().height ;      
			return n;
		}
	};//end class div_grid
	
    template <class Base> struct IAdjust : Base
    {    
            IAdjust(){}
            IAdjust(unsigned min_,unsigned max_=MAX) {MinMax(min_, max_);} 
            virtual ~IAdjust(){}
    };
    template <class Base> struct IFixed:  IAdjust<Base>     
    { 
        unsigned weight_; 

        IFixed(unsigned weight)                             : weight_(weight) {}
        IFixed(unsigned weight, unsigned min_,unsigned max_): weight_(weight),IAdjust<Base>(min_,max_){}

        adj   pre_place(unsigned t_w,                          adj&   fixed = adj() ) override    
                    {   fixed.weigth   += weigth_adj(t_w) ;   return  fixed;        }

        adj   end_place(unsigned t_w,const adj& fixed = adj(), adj& adj_min = adj() ) override    
                    {   adj_min.weigth += weigth_adj(t_w) ;   return  adj_min;      }   

        unsigned weigth(unsigned t_w,const adj& fixed,const adj& adj_min )  override              
                    {   return weigth_adj(t_w);      }    

        virtual unsigned weigth_adj(unsigned t_w )
        {   
            if ( weight_  < min )    {return min; }
            if ( weight_  > max )    {return max; }
                
            return  weight_;          
        }
    };
    template <class Base> struct IPercent: IFixed<Base> 
    { 
        IPercent(double percent_)                            
            :IFixed<Base>(static_cast<unsigned>(100*percent_))          {}

        IPercent(double percent_,unsigned min_,unsigned max_)
            :IFixed<Base>(static_cast<unsigned>(100*percent_),min_,max_){}
        unsigned weigth_adj(unsigned t_w )override
        {   
            if ( t_w * weight_ /100.0  < min )    {return min; }
            if ( t_w * weight_ /100.0  > max )    {return max; }
                
            return  t_w * weight_ /100.0;          
        }
    };     
    template <class Base> struct IAdjustable  :  IAdjust<Base>                      
    { 
        IAdjustable(         ){}
        IAdjustable(unsigned min_,unsigned max_):IAdjust<Base>  (min_,max_){}

        adj   pre_place(unsigned t_w,                        adj& fixed = adj() ) override    
                    {  
                        ++fixed.count_adj;     
                        fixed.min += min;    
                        return  fixed;        
                    }
        adj   end_place(unsigned t_w,const adj& fixed = adj(), adj& adj_min = adj() ) override    
        {   
            if ( t_w      <   fixed.weigth + fixed.min         )  
                                                            { adj_min.weigth += min; return adj_min; }
            if ( t_w <    min * fixed.count_adj + fixed.weigth )
                                                            { adj_min.weigth += min; return adj_min; }
            if ( t_w >    max * fixed.count_adj + fixed.weigth )
                                                            { adj_min.weigth += max; return adj_min; }

            adj_min.min += min; 
            ++adj_min.count_adj;   return  adj_min;        
        }
        unsigned weigth(unsigned t_w,const adj& fixed,const adj& adj_min )override
        {   
            if ( t_w      <   fixed.weigth + fixed.min         )   
                                                                        {return min; }
            if ( t_w <    min * fixed.count_adj   + fixed.weigth   )   
                                                                        {return min; }
            if ( t_w >    max * fixed.count_adj   + fixed.weigth   )   
                                                                        {return max; }
            if ( t_w <    min * adj_min.count_adj + adj_min.weigth )   
                                                                        {return min; }
            if ( t_w >    max * adj_min.count_adj + adj_min.weigth )   
                                                                        {return max; }

            return  (t_w - adj_min.weigth) / adj_min.count_adj  ;        
        }
    };
   
    struct adj_gap:   IAdjustable<Gap_field> 
    { 
        adj_gap( )                                                   {}
        adj_gap( unsigned min_,unsigned max_):IAdjustable<Gap_field> (min_,max_){}
    };
    struct adj_widget:    IAdjustable<Widget_field>
    { 
        adj_widget(window handle_)                      
                {Widget_field::init (handle_);}
        adj_widget(window handle_, unsigned min_,unsigned max_)
            :IAdjustable<Widget_field>(min_,max_) {Widget_field::init (handle_);}
    };
    struct adj_room: IAdjustable<Room_field>   
    { 
        adj_room(window handle_,unsigned rows_,unsigned columns_)                             
                {Room_field::init(handle_, rows_, columns_);}
        adj_room(window handle_,unsigned rows_,unsigned columns_, unsigned min_,unsigned max_)
                :IAdjustable<Room_field>(min_,max_){Room_field::init(handle_, rows_, columns_);}
    };
	struct adj_div_h : public IAdjustable<div_h> 
    {
        adj_div_h()                     {}
        adj_div_h(unsigned min_,unsigned max_):IAdjustable<div_h> (min_,max_){}
    };
	struct adj_div_v : public IAdjustable<div_v>  
    {
        adj_div_v()                     {}
        adj_div_v(unsigned min_,unsigned max_):IAdjustable<div_v>(min_,max_){}
    };
	struct adj_div_grid : public IAdjustable<div_grid> 
    {
        adj_div_grid(const std::string& name_, unsigned rows_, unsigned columns_)   
                                                           { Init(name_,rows_,columns_);}

        adj_div_grid(const std::string& name_, size dim_ , unsigned min_ ,  unsigned max_     )    
            :  IAdjustable<div_grid>(min_   ,max_       )  {Init(name_,dim_); }

        adj_div_grid(const std::string& name_, size dim_      )    
                                                           {Init(name_,dim_); }

        adj_div_grid(const std::string& name_, unsigned rows_, unsigned columns_,
                                               unsigned min_,unsigned max_)   
            :    IAdjustable<div_grid>(min_        ,max_       ) { Init(name_,rows_,  columns_);}
    };

    struct fixed_gap: IFixed<Gap_field> 
    { 
        fixed_gap(unsigned weight_                             ):IFixed<Gap_field> (weight_)          {}
        fixed_gap(unsigned weight_, unsigned min_,unsigned max_):IFixed<Gap_field> (weight_,min_,max_){}
    };
    struct fixed_widget:   IFixed<Widget_field>
    { 
        fixed_widget(window handle_,unsigned weight_                             )
            :IFixed<Widget_field>(weight_          ){Widget_field::init (handle_);}
        fixed_widget(window handle_,unsigned weight_, unsigned min_,unsigned max_)
            :IFixed<Widget_field>(weight_,min_,max_){Widget_field::init (handle_);}
    };
    struct fixed_room:  IFixed<Room_field>   
    { 
        fixed_room(window handle_, unsigned weight_  , 
                   unsigned rows_, unsigned columns_  )
            :IFixed<Room_field>   (weight_)              {Room_field::init(handle_, rows_, columns_);}
        fixed_room(window handle_, unsigned weight_  ,
                   unsigned rows_,unsigned columns_,    unsigned min_,unsigned max_)
            :IFixed<Room_field>   (weight_,min_,max_)    {Room_field::init(handle_, rows_, columns_);}
    };
	struct fixed_div_h : public IFixed<div_h>
    {
        fixed_div_h(unsigned weight_                             )  :IFixed<div_h> (weight_ )        {}
        fixed_div_h(unsigned weight_ , unsigned min_,unsigned max_):IFixed<div_h> (weight_ ,min_,max_){}
    };
	struct fixed_div_v : public IFixed<div_v> 
    {
        fixed_div_v(unsigned weight_                             ):IFixed<div_v>(weight_   )       {}
        fixed_div_v(unsigned weight_, unsigned min_,unsigned max_):IFixed<div_v>(weight_,min_,max_){}
    };
	struct fixed_div_grid : public IFixed<div_grid> 
    {
        fixed_div_grid(const std::string& name_,unsigned weight_ ,  unsigned rows_, unsigned columns_)  
            :IFixed<div_grid>(weight_)   { Init(name_,rows_,  columns_);}
        fixed_div_grid(const std::string& name_,unsigned weight_  , size dim_)
            :IFixed<div_grid>(weight_ )  {Init(name_,dim_); }

        fixed_div_grid(const std::string& name_,unsigned weight_ ,  unsigned rows_, 
                                                unsigned columns_, unsigned min_,unsigned max_)  
            :IFixed<div_grid>(weight_,min_,max_) { Init(name_,rows_,  columns_);}
        fixed_div_grid(const std::string& name_,unsigned weight_  , size dim_, 
                                                 unsigned min_,unsigned max_)
            :IFixed<div_grid>(weight_,min_,max_) {Init(name_,dim_); }
    };

    struct percent_gap: IPercent<Gap_field> 
    { 
        percent_gap(double   percent_):IPercent<Gap_field>(percent_){}
        percent_gap(double   percent_,unsigned min_,unsigned max_):IPercent<Gap_field>(percent_,min_,max_){}
    };     
    struct percent_widget: IPercent<Widget_field>
    { 
        percent_widget(window handle_,double   percent_)
            :IPercent<Widget_field>(percent_){Widget_field::init (handle_);}
        percent_widget(window handle_,double   percent_, unsigned min_,unsigned max_)
            :IPercent<Widget_field>(percent_,min_,max_){Widget_field::init (handle_);}
    };   
    struct percent_room:  IPercent<Room_field>   
    { 
        percent_room(window handle_, double   percent_,
                     unsigned rows_, unsigned columns_  )
              :IPercent<Room_field>  (percent_          ) {Room_field::init(handle_, rows_, columns_);}

        percent_room(window handle_, double   percent_,
                     unsigned rows_, unsigned columns_,   unsigned min_, unsigned max_ )
              :IPercent<Room_field>(percent_,min_,max_)   {Room_field::init(handle_, rows_, columns_);}
    };     
    struct percent_div_h : public IPercent<div_h> 
    {
        percent_div_h(double   percent_)  :IPercent<div_h> ( percent_   )                 {}
        percent_div_h(double   percent_,unsigned min_,unsigned max_)
                         :IPercent<div_h> ( percent_ ,min_,max_){}
    };
	struct percent_div_v : public IPercent<div_v> 
    {
        percent_div_v(double   percent_)   :IPercent<div_v> ( percent_   )                    {}
        percent_div_v(double   percent_,unsigned min_,unsigned max_)
                                 :IPercent<div_v>( percent_,min_,max_){}
    };
	struct percent_div_grid : public IPercent<div_grid>  
    {
        percent_div_grid(const std::string& name_,double   percent_, unsigned rows_, unsigned columns_)   
            :IPercent<div_grid> ( percent_    )        { Init(name_,rows_,  columns_);}
        percent_div_grid(const std::string& name_,double   percent_, size dim_)
            :IPercent<div_grid> ( percent_)            {Init(name_,dim_); }

        percent_div_grid(const std::string& name_,double   percent_, unsigned rows_, unsigned columns_, 
                                                                     unsigned min_,unsigned max_)   
            :IPercent<div_grid> ( percent_ ,min_,max_   )   { Init(name_,rows_,  columns_);}
        percent_div_grid(const std::string& name_,double   percent_, size dim_, 
                                                                     unsigned min_,unsigned max_)
            :IPercent<div_grid> ( percent_,min_,max_)       {Init(name_,dim_); }
    };

    class number_t
	{	//number_t is used to store a number type variable
	    //such as integer, real and percent. Essentially, percent is a typo of real.
	public:
		enum class kind{/*empty, */integer, real, percent};
		number_t() 			: kind_(kind::integer)		{	value_.integer = 0;		}

		kind kind_of() const		{			return kind_;		}

		int integer() const
		{
           //        if( kind::empty  == kind_ )				//return 0;
            if(kind::integer == kind_ )				return value_.integer;
			return static_cast<int>(value_.real);
		}
		double real() const
		{
            //        if( kind::empty  == kind_ )				//return 0;
			if(kind::integer == kind_  )				return value_.integer;
			return value_.real;
		}
		void assign      (int i)      { kind_ = kind::integer;  value_.integer = i; }
		void assign    (double d)     { kind_ = kind::real; 	value_.real    = d; }
		void assign_percent(double d) { kind_ = kind::percent; value_.real = d / 100; }
        void clear(){*this = number_t(); 	}
    private:
		kind kind_;
		union valueset 	{ int integer; double real; }value_;
	};//end class number_t

	class tokenizer
	{
	public:
		enum class token
		{
			div_start, div_end,
			identifier, vertical, horizontal, grid, number, array,
			weight, gap, min, max,
			equal,
			eof, error
		};

		tokenizer(const char* p)	: div_str(p), divstr_(div_str.c_str() ), sp_(div_str.c_str())	{}

		const std::string& idstr() const		{			return idstr_;		}
		number_t           number() const		{			return number_;		}
		std::vector<number_t>& array()		{			return array_;		}
		token read()
		{
			sp_ = _m_eat_whitespace(sp_);

			std::size_t readbytes = 0;
			switch(*sp_)
			{
			case '\0':				                    return token::eof;
			case '=':		        ++sp_;				return token::equal;
			case '<':				++sp_;				return token::div_start;
			case '>':				++sp_;				return token::div_end;
			case '[':
				array_.clear();
				sp_ = _m_eat_whitespace(sp_ + 1);
				if(*sp_ == ']')		{	++sp_;		return token::array;	}

				while(true)
				{
					sp_ = _m_eat_whitespace(sp_);
					if(token::number != read())
						_m_throw_error("invalid array element");

					array_.push_back(number_);

					sp_ = _m_eat_whitespace(sp_);
					char ch = *sp_++;

					if(ch == ']')						break;
					if(ch != ',')						_m_throw_error("invalid array");
				}
				return token::array;
			case '.': case '-':
				if(*sp_ == '-')
				{
					readbytes = _m_number(sp_ + 1, true);
					if(readbytes)			++ readbytes;
				}
				else 					readbytes = _m_number(sp_, false);
						
				if(readbytes){  sp_ += readbytes; 	return token::number;}
				else    					        _m_throw_error(*sp_);
				break; 
			default:
				if(isdigit (*sp_))
				{
					readbytes = _m_number(sp_, false);
					if(readbytes) 	{	sp_ += readbytes; return token::number;		}
				}
				break;
			}

			if('_' == *sp_ || isalpha(*sp_))
			{
				const char * idstart = sp_++;

				while('_' == *sp_ || isalpha(*sp_) || isalnum(*sp_)) 	++sp_;

				idstr_.assign(idstart, sp_);

				     if(idstr_ == "weight"){_m_attr_number_value();	return token::weight;}
				else if(idstr_ == "gap")   {_m_attr_number_value();	return token::gap;   }
				else if(idstr_ == "min")   {_m_attr_number_value(); return token::min;   }
				else if(idstr_ == "max")   {_m_attr_number_value();	return token::max;   }
				else if(idstr_ == "vertical") 		   	 	 	    return token::vertical;
				else if(idstr_ == "grid")       					return token::grid;
                        				                            return token::identifier;
			}

			return token::error;
		}
	private:
		void _m_throw_error(char err_char)
		{
			std::stringstream ss;
			ss<<"place: invalid character '"<<err_char<<"' at "<<static_cast<unsigned>(sp_ - divstr_);
			throw std::runtime_error(ss.str());
		}

		void _m_attr_number_value()
		{
			if(token::equal != read())
				_m_throw_error("an equal sign is required after \'"+ idstr_ +"\'");

			const char* p = sp_;
			for(; *p == ' '; ++p);

			std::size_t len = 0;
			if(*p == '-')
			{
				len = _m_number(p + 1, true);
				if(len)	++len;
			}
			else
				len = _m_number(p, false);

			if(0 == len)
				_m_throw_error("the \'" + idstr_ + "\' requires a number(integer or real or percent)");
						
			sp_ += len + (p - sp_);
		}

		void _m_throw_error(const std::string& err)
		{
			std::stringstream ss;
			ss  <<"NANA::vplace layout syntax error:\n\t"      <<  err 
                <<" at "<<static_cast<unsigned>(sp_ - divstr_) 
                <<" of:\n" << divstr_ ;
            std::cerr<< ss.str();
			throw std::runtime_error(ss.str());
		}

		const char* _m_eat_whitespace(const char* sp)
		{
			while (*sp && !isgraph(*sp))
				++sp;
			return sp;
		}

		std::size_t _m_number(const char* sp, bool negative)
		{
			const char* allstart = sp;
			sp = _m_eat_whitespace(sp);

			number_.assign(0);

			bool gotcha = false;
			int integer = 0;
			double real = 0;
			//read the integral part.
			const char* istart = sp;
			while(isdigit (*sp))	{	integer = integer * 10 + (*sp - '0');	++sp;		}
			const char* iend = sp;

			if('.' == *sp)
			{
				double div = 1;
				const char* rstart = ++sp;
				while(isdigit (*sp)){	real += (*sp - '0') / (div *= 10);		++sp;		}

				if(rstart != sp)
				{
					real += integer;
					number_.assign(negative ? -real : real);
					gotcha = true;
				}
			}
			else if(istart != iend)
			{
				number_.assign(negative ? - integer : integer);
				gotcha = true;
			}

			if(gotcha)
			{
				for(;*sp == ' '; ++sp);
				if('%' == *sp)
				{   
                    number_.assign_percent(number_.real());
					return sp - allstart + 1;
				}
				return sp - allstart;
			}
			return 0;
		}
	private:
        std::string div_str;   // for the future. kip a copy of the layout
		const char* divstr_;
		const char* sp_;
		std::string idstr_;
		number_t number_;
		std::vector<number_t> array_;
	};	//end class tokenizer

    struct implement           //struct implement
	{
        std::unique_ptr<field_t>        temp_field_t;
        window                          parent_window_handle{nullptr};
        event_handle                    event_size_handle{nullptr};
		std::unique_ptr<division>       root_division;
        std::unordered_set<std::string> names;     ///<  All the names defines. Garant no repited name.
        unsigned                        div_numer{ 0 }; ///<  Used to generate unique div name.
        bool                            recollocate{ true };

            /// All the fields defined by user with field(name)<<IField, 
            /// plus the div. find from the layot in div()
		std::multimap<std::string, std::unique_ptr<IField>> fields;    
		std::multimap<std::string,    window              > fastened;
        std::vector/*<std::unique_ptr */<nana::gui::label*>/*>*/     widgets;                                 
			
       ~implement() 	{ API::umake_event(event_size_handle);	    }

        void              collocate();
		void              div(const char* s);
        bool              bind     (window parent_widget);
		division *        scan_div       (tokenizer&);

        /// Generate and registre in the set a new unique div name from the current layout
        std::string   add_div_name  ()  
        {
            std::string name= std::to_string (div_numer++);
            names.insert (name);
            return name; 
        }
        bool          add_field_name(const std::string& n)  /// is unique? (registre it)
        {
            if (names.insert (n).second)
                return true; 
            else return false;//trow name repit;
        }
	};	      //struct implement

    void division::populate_children(	implement*   place_impl_)
    {   fastened_in_div.clear ();
        children.clear ();             /// .clear(); the children or it is empty allways ????
        for (const auto &name : field_names)      /// for all the names in this div
        {                             /// find in the place global list of fields all the fields attached to it
            auto r= place_impl_->fields.equal_range(name);     
            for (auto fi=r.first ; fi != r.second ; ++fi)      
            {
                children.push_back (fi->second.get () );       /// to form the div children
                fi->second->populate_children (place_impl_);
                if (gap) 
                    children.push_back (gap.get() );
            }
            auto f= place_impl_->fastened.equal_range(name);
            for (auto fi=f.first ; fi != f.second ; ++fi)
                fastened_in_div.push_back (fi->second );
        }
    }  

    void implement::collocate()
	{
		if(root_division && parent_window_handle)
		{
			rectangle r(API::window_size(this->parent_window_handle));  //debugg
            //if(r.width && r.height ) 
            {      
               if (recollocate) root_division->populate_children (this);
               recollocate = false;
               root_division->collocate(r/*=API::window_size(this->parent_window_handle)*/);
            }
            //			                                //rectangle r; // debugg
            //root_division->collocate(r/*=API::window_size(parent_window_handle)*/);
            ////std::cerr<< "\ncollocating root div  [ "<<this->parent_window_handle<<" ]) with area: "<<r; // debugg
		}
	}

    class field_impl 		:	public field_t , public minmax
	{
	public:
		std::string name;

	private:
		implement * place_impl_;
	 public:
		//typedef std::vector<std::unique_ptr<IField>>::const_iterator const_iterator;

         field_impl(implement * p, const std::string& name_):	place_impl_(p),   name(name_){}

	  public:
        IField * create_field(window    wd                                      )
                                      {return new adj_widget     ( wd )                   ;}
        IField * create_field(unsigned gap                                      )
                                      {return new fixed_gap      ( gap )                  ;}
        IField * create_field(window handle_,unsigned weight_                   )
                                      {return new fixed_widget   ( handle_, weight_)      ;}
        IField * create_field(window handle_,double   percent_                  )
                                      {return new percent_widget( handle_, percent_)      ;}
        IField * create_field(window handle_,unsigned rows_,unsigned columns_   )
                                      {return new adj_room    ( handle_, rows_, columns_ );}
	 private:
         field_t& add(IField * fld)
		{
			fld->MinMax (*this);
            place_impl_->fields.emplace(name,std::unique_ptr<IField>( fld));
            place_impl_->recollocate = true;
			_m_make_destroy(fld->window_handle());
			return *this;
		}

        field_t& operator<<(const std::string&  txt)	override 
        {   
			//return *this;  //  ttttttttttttttttttttttttttttttttttttttttttt
            return add_label( nana::charset (txt)  );
        };
        field_t& operator<<(const std::wstring&  txt)	override 
        {   
			//return *this;    //  ttttttttttttttttttttttttttttttttttttttttttt
            return add_label( nana::charset (txt)  );
        };
        field_t&  add_label(const nana::string& txt )
        {            
            //std::unique_ptr <nana::gui::label> lab(new nana::gui::label(place_impl_->parent_window_handle, txt  ));
            //API::
            nana::gui::label*lab = std::addressof(nana::gui::form_loader <nana::gui::label>()(place_impl_->parent_window_handle, txt));
            place_impl_->widgets.push_back (lab);/*newstd::addressof (nana::gui::form_loader <nana::gui::label>()(place_impl_->parent_window_handle, txt  ))*/ 
            //place_impl_->widgets.emplace_back (new nana::gui::label(place_impl_->parent_window_handle, txt  ));
            add(create_field( *place_impl_->widgets.back () ));
            auto pi = place_impl_;
			auto dtr = API::make_event<events::destroy>(*lab, [pi](const eventinfo& ei)
			{
				for (auto f=pi->widgets.begin(); f!=pi->widgets.end(); ++f)
                    if (/*->get()*/(*f)->handle() ==  ei.window )
                    {
                        pi->widgets.erase(f);    // delete ???
				        pi->recollocate = true;
				        pi->collocate();
                        break;
                    }
			});
            //API::make_event<events::destroy>(*lab, [dtr](const eventinfo& ei)
            //{
            //    API::umake_event(dtr); 
            //});	

			return *this;
        }
        field_t& operator<<(minmax Size_range)	override { MinMax(Size_range) ;return *this;};
        field_t& operator<<(IField * fld)       override {return add(fld);                  }
        field_t& operator<<(window   wd)        override {return add(create_field(wd));		}
		field_t& operator<<(unsigned gap)       override {return add(create_field(gap));    }
		field_t& fasten(window wd)              override
		{
			place_impl_->fastened.emplace (name , wd); 
            place_impl_->recollocate = true;
			//Listen to destroy of a window. The deleting a fastened window
			//does not change the layout.
            auto pi = place_impl_;
			auto dtr = API::make_event<events::destroy>(wd, [pi](const eventinfo& ei)
			{
				for (auto f=pi->fastened.begin(); f!=pi->fastened.end(); ++f)
                    if (f->second ==  ei.window )
                    {
                        pi->fastened.erase(f);    // delete ???
				        pi->recollocate = true;
                        break;
                    }
			});	
            //API::make_event<events::destroy>(wd, [dtr](const eventinfo& ei)
            //{
            //    API::umake_event(dtr); 
            //});	
			return *this;
		}
		field_t& operator<<(const fixed_widget& fx)    {return add(new fixed_widget(fx));	 }
    	field_t& operator<<(const percent_widget& pcnt){return add(new percent_widget(pcnt));}
		field_t& operator<<(const adj_room& r) 
		{
			adj_room x = r;
			if(x.rows == 0)   //  sure ????
				x.rows = 1;
			if(x.columns == 0)
				x.columns = 1;
			return add(new adj_room(x));
		}
 
        //Listen to destroy of a window
		//It will delete the element and recollocate when the window destroyed.
		void _m_make_destroy(window wd)
		{
            auto pi = place_impl_;
            auto dtr = API::make_event<events::destroy>(wd, [pi](const eventinfo& ei)
			{
				for (auto f=pi->fields.begin(); f!=pi->fields.end(); ++f)
                    if (f->second->window_handle() ==  ei.window )
                    {
                        pi->fields.erase(f);    // delete ???
				        pi->recollocate = true;
				        pi->collocate();
                        break;
                    }
			});
            //API::make_event<events::destroy>(wd, [dtr](const eventinfo& ei)
            //{
            //    API::umake_event(dtr); 
            //});	
		}
	};//end class field_impl

	division* implement::scan_div(tokenizer& tknizer)
	{
		typedef tokenizer::token token;

		token       div_type = token::eof;
		number_t    weight , gap;
        bool        have_gap=false, have_weight=false;
        minmax      w; 
        std::string gr_name;
 
        std::vector<number_t>    array;
		std::vector<std::string> field_names_in_div;
 
        for(token tk = tknizer.read(); tk != token::eof && tk!=token::div_end ; tk = tknizer.read())
		{
			switch(tk)
			{
			    case token::div_start:	    
                    {
                       std::string div_name(add_div_name ());
                       fields.emplace(div_name,std::unique_ptr<IField>(scan_div(tknizer)));
                       field_names_in_div.push_back(div_name);				    break;
                                                                               
                    }
			    case token::array:		    tknizer.array().swap(array);   		break;
			    case token::identifier:		
                {
                    std::string field_name(tknizer.idstr());
                    if (add_field_name (field_name))
                       field_names_in_div.push_back(field_name );			    
                    else ;     /* trow repeated name in layout !!!!!!! */       break;	     
                }
			    case token::horizontal:
			    case token::vertical:    	div_type = tk;		   		        break;
			    case token::grid:			
                    div_type = tk;
                    gr_name=field_names_in_div.back();                          break;

			    case token::weight:	weight = tknizer.number();have_weight=true; break;
			    case token::gap:	   gap = tknizer.number();have_gap=true;    break;
			    case token::min:		    
                    {
                        if(tknizer.number().kind_of() != number_t::kind::percent   )
					        w.min = tknizer.number().integer();                                
                         /*else trow no min percent possible */                 break;
                    }
			    case token::max:		    
                    {
                        if(tknizer.number().kind_of() != number_t::kind::percent   )
					        w.max = tknizer.number().integer();                                
                         /*else trow no max percent possible */                 break;
                    }
			    default:	break;
			}
		}
        division *div;
		unsigned rows=1, columns=1;		

        if (div_type == token::grid)
		{
			if(array.size())
			{
				if(array[0].kind_of() != number_t::kind::percent)
					columns = array[0].integer();
                if(array.size() > 1)
			    {
				    if(array[1].kind_of() != number_t::kind::percent)
					    rows = array[1].integer();
			    }
			}
			if( ! rows )    				rows    = 1;
			if( ! columns )			  	    columns = 1;
            if(gr_name.empty())             gr_name =field_names_in_div.back();  
		} 
        if (weight.kind_of () == number_t::kind::percent && weight.real() > 0 )
        {
            double perc=weight.real ();
            switch(div_type)
            {
                case token::eof:
		        case token::horizontal:		div = new percent_div_h(perc,w.min,w.max);			     break;
		        case token::vertical:		div = new percent_div_v(perc,w.min,w.max);			     break;
                case token::grid:  div = new percent_div_grid(gr_name,perc,rows,columns,w.min,w.max);break;
                default:
                    throw std::runtime_error("nana.place: invalid division type.");
		    }
        } else 
            {   unsigned fixed=weight.integer();
                if (fixed)
                    switch(div_type)
                    {
                        case token::eof:
		                case token::horizontal:		div = new fixed_div_h(fixed,w.min,w.max);		break;
		                case token::vertical:		div = new fixed_div_v(fixed,w.min,w.max);		break;
                        case token::grid:           div = new fixed_div_grid(gr_name,fixed,
                                                             rows,columns,   w.min,w.max);          break;
                        default:
                            throw std::runtime_error("nana.place: invalid division type.");
		            }
                else
                    switch(div_type)
                    {
                        case token::eof:
		                case token::horizontal:			div = new adj_div_h(w.min,w.max);			break;
		                case token::vertical:			div = new adj_div_v(w.min,w.max);			break;
                        case token::grid:               div = new adj_div_grid(gr_name,
                                                             rows,columns,   w.min,w.max);          break;
                        default:
                            throw std::runtime_error("nana.place: invalid division type.");
		            }
        }
        if (have_gap)
        {
            if (gap.kind_of() == number_t::kind::percent && gap.real () > 0 )
                div->gap.reset (new percent_gap(gap.real ()) );
		    else 
                if (gap.integer())          div->gap.reset (new fixed_gap(gap.integer()) );
                else                        div->gap.reset (new adj_gap  () );
        }
		div->field_names.swap(field_names_in_div);
		return div;
	} // scan_div

    void implement::div(const char* s)
    {
        names.clear ();
        while(div_numer)
                fields.erase (std::to_string (--div_numer));

		tokenizer tknizer(s);
        root_division.reset();
		root_division.reset( scan_div(tknizer));
        recollocate = true;
    }

} // namespace place_impl

	vplace::vplace(window wd)	: impl_(new implement)   {bind(wd);}
	vplace::vplace()		    : impl_(new implement)	 {         }
	vplace::~vplace()		{			delete impl_;		}
    void        vplace::div      (const std::string& s)	{   impl_->div(s.c_str()); 	    }
	void        vplace::collocate(         ) 	{	impl_->collocate();	}
	vplace::field_reference vplace::field  (const std::string& name)
	{
         impl_->temp_field_t.reset (new vplace_impl::field_impl(this->impl_,name)); 
        return  *impl_->temp_field_t.get();
	}

	//IField*         vplace::fixed  (const std::wstring& txt ,  unsigned size)
	//{
	//	return new vplace_impl::fixed_widget(txt, size);
	//}
	//IField*         vplace::fixed  (const std::string&  txt ,  unsigned size)
	//{
	//	return new vplace_impl::fixed_widget(nana::charset( txt), w);
	//}

	IField*         vplace::fixed  (window wd, unsigned size)
	{
		return new vplace_impl::fixed_widget(wd, size);
	}


	IField*         vplace::percent(window wd, double per, minmax w)
	{
		return new vplace_impl::percent_widget(wd, per, w.min ,w.max );
	}
	IField*         vplace::room   (window wd, unsigned width, unsigned height)
	{
		return new vplace_impl::adj_room(wd, height, width);
	}
    vplace::minmax::minmax (unsigned Min, unsigned Max) : min(Min), max(Max){}
    vplace::field_t::~field_t(){}

	void vplace::bind(window wd)
	{
		if( impl_->bind(wd))
			throw std::runtime_error("place.bind: it has already binded to a window.");
    }
    bool implement::bind (window wd) 
    {
		if (parent_window_handle) return true; 
        parent_window_handle = wd;
     		//  rectangle r;  //debugg
            //  r=API::window_size(this->parent_window_handle);  //debugg
            //  std::cerr<< "\nplace(parent_widget [ "<<parent_widget<<" ]) with area: "<<r;  //debugg

        event_size_handle = API::make_event<events::size>(parent_window_handle, [this](const eventinfo&ei)
		{
            this->collocate(); 
    //        //std::cerr<< "\nResize: collocating root div ??:[ "<<this->parent_window_handle<<" ]) with event :[ "//debug
    //        //         <<ei.window <<" ]) ";  //debug
    //        if(this->root_division)
    //        {
				//rectangle r(API::window_size(this->parent_window_handle));  //debugg
    //            //if(r.width && r.height ) 
    //               this->root_division->collocate(r/*=API::window_size(this->parent_window_handle)*/);
    //            //std::cerr<< "\ncollocating root div  [ "<<this->parent_window_handle<<" ]) with area: "<<r;  //debugg
    //        }
		});
        return false;
	}

}//end namespace gui
}//end namespace nana

