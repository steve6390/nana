#ifndef NANA_GUI_Units_HPP
#define NANA_GUI_Units_HPP

#include <iostream> 
#include <map>
#include <set>
#include <functional>
#include <sstream>
#include <assert.h>


class CUnit;
 std::ostream& operator<<( std::ostream& o, const CUnit& u);

class CUnit
{
  public:   
    typedef std::string                             unit_name       ;
    typedef std::string                             magnitude_name  ;
    typedef std::set<unit_name>                     magnitude_t     ;
    typedef std::map<unit_name     , CUnit      >   units           ;
    typedef std::map<magnitude_name, magnitude_t>   magnitudes      ;
    typedef std::function <double(double)>          nonLinealFunction;
    struct conversion
    {
        double              c, s;
        bool                lineal;
        nonLinealFunction   nlc;
        conversion()   :c(1),s(0)  , lineal(true)
        {};
        conversion(double c_  , double s_=0)            :c(c_),s(s_), nlc(_identity), lineal(true)
        {};
        conversion(double c_  , double s_ ,const nonLinealFunction& nlc_):c(c_),s(s_), nlc(nlc_)    , lineal(false) 
        {};
        conversion operator*(const conversion& rc) const
        { 
            if (lineal && rc.lineal)
                return conversion (rc.c*c, c*rc.s+s);
            if (lineal )
                return conversion (rc.c*c, c*rc.s+s, rc.nlc);
            if (rc.lineal )
                return conversion (c,s, [rc,this](double b){return nlc(rc.c*b+rc.s);});

            return conversion (c,s, [rc,this](double b){return nlc(rc.c*rc.nlc(b)+rc.s);});
        }
        conversion inverted() const
        { 
            assert (lineal);
            return conversion (1/c, -s/c);
        }
        conversion invert() 
        { 
            assert (lineal);
            c =1/c; s=-s/c;
            return *this;
        }
        double operator()(double ori_val) const
        {
            if (lineal)
                return c*ori_val+s;
            return c*nlc(ori_val)+s;
        }
    } ;
    conversion      conv;
    unit_name       name, base;
    magnitude_name  magnitude;
    bool            error;
    static const magnitudes& MagnitudesDic() {return _Magnitudes;}
    static const units     & UnitsDic     () {return _Units     ;}

    static bool       unit_exist(const unit_name& n)
    {  
        return _Units.find(n)!= _Units.end();  
    }
    static bool       magnitude_exist(const unit_name& m)
    {  
        return _Magnitudes.find(m)!= _Magnitudes.end();  
    }
    std::string       to_string() const
    {
        std::ostringstream o;
        o << *this;
        return o.str();
    }
protected:
    static bool  _compatible(const unit_name& name_, const unit_name& base_)
    {
        return UnitsDic().at(name_ ).magnitude == UnitsDic().at(base_).magnitude ; 
    }
  private:
    static units                _Units ;
    static magnitudes           _Magnitudes;
    static nonLinealFunction    _identity;
    static bool                 _initialized;
    void add();

  public:
    CUnit(const unit_name& name_,  double   k_            ,   const unit_name& base_,   magnitude_name magnitude_="")
        : name(name_), base(base_), magnitude(magnitude_), error(true), conv(k_)
    {
        add();
    }
    CUnit(const unit_name& name_,  const conversion& conv_,   const unit_name& base_,   magnitude_name magnitude_="")
        : name(name_), base(base_), magnitude(magnitude_), error(true), conv(conv_)
    {
        add();
    }
    CUnit():error(true){}
    CUnit(const unit_name& from, const unit_name& to)   // convert from=name, to=base
        : name(from), base(to), error(true)
    {
        if (! (unit_exist(to) && unit_exist(from)) )
        {    
           std::cerr << std::endl << "We need two existing units in order to find a conversion. " << from
                     << " based on " << to << std::endl;
                return;
         }
        if (_Units[from].magnitude != _Units[to].magnitude)
        {   
            std::cerr << std::endl << "Units "<< from<< " (" << _Units[from].magnitude << ") and "
                     << to << " (" << _Units[to].magnitude<<") are not compatible. " <<  std::endl;
                return;
        }
        if (from==to)       {     error=false;     return ;       }

        unit_name  c_from=from, c_to, c_to_prev=c_from ;   // Unit Base = current to , and Unit Name = current from
        do                                                 // while it is not a basic unit (no more walking posible)
        {
            c_to_prev = c_to;
            c_to      =_Units[c_from].base; 
            conv      = conv * _Units[c_from].conv;
            if (c_to == to)       {     error=false;     return ;       } // We arrived !! we have U(from,to)

            c_from=c_to;               // search walking in the direction from->to, using to=U[from]

        }while(c_to_prev != c_to);     // while it is not a basic unit (no more walking posible)
                                       // we arrived to a basic unit. We have now: U( from , c_to = basic unit)

        //unit_name  c_from=from, c_to ;   // Unit Base = current to , and Unit Name = current from
        //do{                             //  This is a litter more eficient variant !!!
        //    if (c_from==to)
        //    {    
        //        error=false;
        //        return ;
        //    }
        //    conv=conv * _Units[c_from].conv;
        //    c_from=_Units[c_to=c_from].base;
        //}while(c_to!=c_from);


        conversion c2b(conv);       // direct Conversion to a basic unit: U( from , c_to = basic unit)
        conv=conversion();          // search walking in the direction to->from , using to=U[from]

       
        unit_name  c_rfrom=to, c_rto, c_rto_prev=c_rfrom ;   // Unit Base = current to , and Unit Name = current from
        do                                                 // while it is not a basic unit (no more walking posible)
        {
            c_rto_prev = c_rto;
            c_rto      =_Units[c_rfrom].base; 

            if (! _Units[c_rfrom].conv.lineal )   return;
            conv      =  conv * _Units[c_rfrom].conv.inverted() ;

            if (c_rto == from)       {     error=false;     return ;  } // We arrived !! we have U(to,from)

            c_rfrom=c_rto;               // search walking in the direction to->from, using to=U[from]

        }while(c_rto_prev != c_rto);     // while it is not a basic unit (no more walking posible)
                                       // we arrived to a basic unit. We have now: U( from , c_to = basic unit)

        //unit_name c_rfrom=to ,  c_rto ; /// Reverse Unit Name =  Reverse current from (to) , and Reverse Unit Base = Reverse current to (from)
        //do{ 
        //    if (c_rto==from)
        //    {    
        //        error=false; /*conv.invert()*/;
        //        return ;
        //    }
        //    conversion cv(_Units[c_rto].conv);
        //    if ( !cv.lineal)
        //    {    
        //        error=true;
        //        return ;
        //    }
        //    conv= conv * cv.inverted();
        //    c_rto=_Units[c_rfrom=c_rto].base;
        //}while(c_rto!=c_rfrom);

        if (c_to!=c_rto)
        {    
            std::cerr << std::endl << "Units "<< from<< " (" << _Units[from].magnitude << ") and "
                     << to << " (" << _Units[to].magnitude<<") have no conversion defined. " <<  std::endl;
            error=true;
            return ;
        }
        conv=c2b*conv;
        error=false;

        //        conversion c2b(conv);    // Conversion to a basic unit: U( from , c_to = basic unit)
        //conv=conversion();
        //unit_name rub, run=to ; /// Reverse Unit Name =  Reverse current from (to) , and Reverse Unit Base = Reverse current to (from)
        //do{ 
        //    if (run==from)
        //    {    
        //        error=false; /*conv.invert()*/;
        //        return ;
        //    }
        //    conversion cv(_Units[run].conv);
        //    if ( !cv.lineal)
        //    {    
        //        error=true;
        //        return ;
        //    }
        //    conv= conv * cv.inverted();
        //    run=_Units[rub=run].base;
        //}while(run!=rub);
        //if (c_to!=rub)
        //{    
        //    std::cerr << std::endl << "Units "<< from<< " (" << _Units[from].magnitude << ") and "
        //             << to << " (" << _Units[to].magnitude<<") have no conversion defined. " <<  std::endl;
        //    error=true;
        //    return ;
        //}
        //conv=c2b*conv;
        //error=false;


    }
        //for{ unit_name un=name ;  _Units[un].base != un ; un=_Units[un].base)
        //{
        //   conv=conv*_Units[un].conv;
        //   un=_Units[un].base;
        //if ( un== base)
        //{    
        //    *this= _Units[name];
        //    error=false;
        //    return ;
        //}
        //if (_Units[name].base== name)
        //CUnit u();
    
    static const double No;
    static bool Initialize()
    {
        CUnit("g"       , 0.001 , "kg"   , "Mass"       );
        CUnit("g"       , 1000  , "mg"                  );
        CUnit("mg"      , 1000  , "痢"                  );
        CUnit("痢"      , 1000  , "ng"                  );
        CUnit("ng"      , 1000  , "pg"                  );
        CUnit("pg"      , 1000  , "fg"                  );
        CUnit("tone"    , 1000  , "kg"                  );
        CUnit("A"       , 1     , ""     , "Current"    );
        CUnit("mA"      , 1000  , "A"                   );
        CUnit("mol"     , 1     , ""     , "Amount"     );
        CUnit("mol"     , No    , "cop"                 );
        CUnit("mmol"    , 0.001 , "mol"                 );
        CUnit("痠ol"    , 0.001 , "mmol"                );
        CUnit("nmol"    , 0.001 , "痠ol"                );
        CUnit("pmol"    , 0.001 , "nmol"                );
        CUnit("fmol"    , 0.001 , "pmol"                );
        CUnit("amol"    , 0.001 , "fmol"                );
        CUnit("zmol"    , 0.001 , "amol"                );
        CUnit("ymol"    , 0.001 , "zmol"                );
        CUnit("molecule", 1     , "cop"                 );
        CUnit("particle", 1     , "cop"                 );
        CUnit("cd"      , 1     , ""     , "Luminosity" );
        CUnit("K"       , 1     , ""     , "Temperature");
        CUnit("蚓",conversion(1, 273.15), "K"           );
        CUnit("蚓",conversion(1.8, 32),  "蚌"           );
        //CUnit("蚓"      , 1     , "grC"               );
        CUnit("s"       , 1     , "s"    , "Time"       );
        CUnit("s"      , 1000   , "ms"                  );
        CUnit("min"     , 60    , "s"                   );
        CUnit("h"       , 60    , "min"                 );
        CUnit("day"     , 24    , "h"                   );
        CUnit("week"    , 7     , "day"                 );
        CUnit("year"    , 365   , "day"                 );
        CUnit("L"       , 1     , ""    , "Volumen"     );
        CUnit("L"       , 1000  , "mL"                  );
        CUnit("mL"      , 1000  , "無"                  );
        CUnit("m3"      , 1000  , "L"                   );
        CUnit("dm3"     , 1     , "L"                   );
        CUnit("cm3"     , 1     , "mL"                  );
        CUnit("mm3"     , 1     , "無"                  );
        CUnit("m"       , 1     , ""    , "Length"      );
        CUnit("km"      , 1000  , "m"   , "Length"      );
        CUnit("dm"      , .1    , "m"                   );
        CUnit("cm"      , .1    , "dm"                  );
        CUnit("mm"      , .1    , "cm"                  );
        CUnit("rad"     , 1     , "m/m"    , "angle"    );
        CUnit("x"       , 1     , ""    , "fraction"    );
        CUnit("x"       , 100   , "%"                   );
        CUnit("sr"      , 1     , "m2/m2"  , "solidangle");
        CUnit("Hz"      , 1     , "s-1"    , "frequency");
        CUnit("N"       , 1     , "kg*m*s-2", "force"   );
        CUnit("J"       , 1     , "kg*m2*s-2", "energy" );
        CUnit("J"       , 1     , "N*m"      , "energy" );
        CUnit("cal"     , 4.184 , "J"                   );
        CUnit("kcal"    , 1000  , "cal"                 );
        CUnit("Wh"      , 3600  , "J"                   );
        CUnit("cal/mol" , 4.184 , "J/mol", "intr_energy");
        CUnit("kcal/mol", 1000  , "cal/mol"             );
        CUnit("kJ/mol"  , 1000  , "J/mol"               );
        CUnit("bp"      , 1     , "nt"    , "GeneLength");
        CUnit("kb"      , 1000  , "bp"    , "GeneLength");
        //CUnit("M"       , 1     , ""    , "molarity"  );
        CUnit("M"       , No    , "cop/L" ,"molarity"   );
        CUnit("cop/無"  , 1000  , "cop/L"               );
        CUnit("M"       , 1     , "mol/L"               );
        CUnit("M"       , 1000  , "mM"                  );
        CUnit("mM"      , 1000  , "然"                  );
        CUnit("然"      , 1     , "痠ol/L"              );
        CUnit("然"      , 1000  , "nM"                  );
        CUnit("nM"      , 1000  , "pM"                  );
        CUnit("pM"      , 1000  , "fM"                  );
      //CUnit("pM"      , 1     , "痠ol/L"              );
        CUnit("痠ol/L"  , 1     , "pmol/無"             );
        CUnit("mg/L"    , 1000  , "g/L" ,"concentration");
        CUnit("痢/L"    , 1000  , "mg/L"                );
        CUnit("g/mL"    , 1000  , "g/L"                 );
        CUnit("pg/無"   , 1     , "痢/L"                );

        return true;
    }
};

class Relation : public CUnit
{
public:
    Relation(const unit_name& name_,  double   k_            ,   const unit_name& base_ )
        /*: CUnit (name_,base_)*/
    {
        if ( error =  !unit_exist(name_) || !unit_exist(base_))
            return;
        //if ( error = _compatible (name_ , base_ ) && !CUnit (name_,base_).error ) 
        //    return;
        conv = conversion (k_);
        name=name_; base= base_ ;
    }
    Relation(const unit_name& name_,  const conversion& conv_,     const unit_name& base_ )
        /*: CUnit (name_,base_)*/
    {
        if ( error =  !unit_exist(name_) || !unit_exist(base_))
            return;
        //if ( error = _compatible (name_ , base_ ) && !CUnit (name_,base_).error ) 
        //    return;
        conv = conv_;
        name=name_; base= base_ ;
    }
    //Relation(const unit_name& name_,  const conversion& conv_,     const unit_name& base_ )
    //    : CUnit (name_,base_)
    //{
    //    conv = conv_;
    //    error = error ? !unit_exist(name) || !unit_exist(base) || _compatible (name , base ) : true;
    //}
    CUnit operator()(const unit_name& name_, const unit_name& base_)
    {
        CUnit u;
        u.name=name_; u.base=base_ ;
        if (!unit_exist (name_ ) || !unit_exist (base_ ))
            return u;
        //if ( _compatible(name_, base_) )
        //{
        //    u =CUnit(name_, base_);
        //    if (!u.error)
        //        return u;
        //} 
        if ( _compatible(name_, name) && _compatible(base,base_) )
        {
            CUnit l(name_, name), r(base,base_);
            if (!l.error && !r.error )
            {  
               u.conv=l.conv * conv * r.conv ;
               u.error = false;
               return u;
            }
        }
        if ( _compatible(name_, base ) && _compatible(name ,base_) )
        {
            CUnit rl(name_, base ), rr(name ,base_);
            if (!rl.error && !rr.error )
            {  
               u.conv=rl.conv * conv.inverted()  * rr.conv ;
               u.error = false;
            }
        }
        return u;
    }
};
class MW 
{
  Relation m,c;
public:
    MW(double mw):m("mol",mw,"g"),c("M",mw,"g/L"){}
    CUnit operator()(const CUnit::unit_name& name_, const CUnit::unit_name& base_)
    {
        CUnit r=m(name_,base_);
        if (!r.error)
            return r;
        return c(name_,base_);
    }
};



#endif 



