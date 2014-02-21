#ifndef NANA_GUI_Numer_HPP
#define NANA_GUI_Numer_HPP
#pragma warning(disable : 4996)
#include <../temp/EditableForm.hpp>
#include <../temp/Units.hpp>


namespace nana { namespace gui {

class NumberLabel : public label
{
  public:
    NumberLabel (   widget &parent,    
                    double val=0, 
                    unsigned decimals=2,
                    unsigned width=6)
        :	label(parent),
			_val(val),  _decimals(decimals), _width(width)
    {
        display();
    }
    double    _val;
    unsigned  _decimals, _width;
    double Value(          )const{                    return _val;}
    double Value(double val)     {_val=val; display(); return _val;}
void display()
{
    string val(50,0);
    swprintf(&val[0],val.size(), STR(" %*.*f"), _width, _decimals, _val );
    caption (val.c_str());
}
};


class NumberBox : public textbox
{
  public:
    NumberBox (   widget &parent,    
                    double val=0, 
                    unsigned decimals=2,
                    unsigned width=6)
        :	textbox(parent),
			_val(val),  _decimals(decimals), _width(width)
    {
        multi_lines(false);
        display();
        make_event <events::focus>([&](const eventinfo& ei)
                {  if (!ei.focus.getting) 
                        validate_edit( );
                }); 
    }
    double    _val;
    unsigned  _decimals, _width;
    double Value(          )const{                    return _val;}
    double Value(double val)     {_val=val; display(); return _val;}
void read()
{
    try    {  _val=std::stod (caption()  );     }
    catch (...)     {;     }
}
void validate_edit()
{
    read();
    display();
}
void display()
{
    string val(50,0);
    swprintf(&val[0],val.size(), STR(" %*.*f"), _width, _decimals, _val );
    caption (val.c_str());
}

};

class NumerUpDown : public  CompoWidget
{
  public:
    NumerUpDown (   widget &parent_,      const string &label, 
                    double val,           double min, double max, 
                    const string         &DefFileName=STR("NumUpDown.VertCenter.lay.txt"), 
                    double step=1,       unsigned width=6,    unsigned decimals=2  );
    double  Value    (          )const{                        return _val;}
    double  Min      (          )const{                        return _min;}
    double  Max      (          )const{                        return _max;}
    double  Step     (          )const{                        return _step;}
    unsigned Width    (          )const{                        return _width;}
    unsigned Decimals (          )const{                        return _decimals;}
    double  Value    (double val)     
    { 
        auto old_v = _val;
        changed=false;

        // add_validate     // 
        if     (val < _min)   val = _min;
        else if(val > _max)   val = _max;
        if ( _val != val )
        {
            _val=val; 
            if (! Validated())
                   _val=old_v;
            else 
             changed=true;
        }
        display () ;  
        return _val;
    }
    double  Min      (double val)     {_min=val; /*validate();*/ return _min;}
    double  Max      (double val)     {_max=val; /*validate(); */return _max;}
    double  Step     (double val)     {_step=val; /* ();*/return _step;}
    unsigned Width    (unsigned val)  {_width=val;/* display ();*/return _width;}
    unsigned Decimals (unsigned val)  {_decimals=val;/* display();*/return _decimals;}

    textbox _num;
  private:
    button  _up, _down;
    label   _label;
    double  _val, _min, _max, _step;
    unsigned _decimals, _width;
double read()
{
    try    
    {  
        return std::stod (_num.caption()  );
     }
    catch (...)     
    { 
        return _val;     
    }
}
void validate()
{
    if     (_val < _min)   _val = _min;
    else if(_val > _max)   _val = _max;
    //else return;
    display();
}
void validate_edit()
{
    Value (read());

    //double v=_val;
    //_val=read();
    //validate();
    //if (_val!=v) 
    //{
    //    changed=true;
    //    Validated();
    //}else
    //    changed=false;
}
void add(double step)
{
    Value(step+Value());
}
void display()
{
    string val(50,0);
    swprintf(&val[0],val.size(), STR(" %*.*f"), _width, _decimals, _val );
    _num.caption (val.c_str());

}

     void SetDefLayout       () override
    {
        _DefLayout= "  <       <vertical weight=60 <><label weight=15 gap=1><> >      \n"
                    "          <vertical weight=50 <><Num weight=19><> >   \n"
                    "          <vertical weight=30 <><UpDown weight=19><> > \n"
                    "  > " ;
    }
     void AsignWidgetToFields() override
    {
	    _place.field("Num"    ) << _num ;
	    _place.field("UpDown" ) << _up << _down ;
	    _place.field("label"  ) << _label;
    }

};

class UnitPicker : public combox
{
          CUnit::unit_name      _defUnit;
    const CUnit::magnitude_name  magnitude;
  public: 
    UnitPicker(widget &wd, const CUnit::unit_name& def)
          :combox(wd), _defUnit(def), magnitude(CUnit::UnitsDic().at(def).magnitude )
    {
        editable(false);
        for(const CUnit::unit_name& un : CUnit::MagnitudesDic().at(magnitude) )
            push_back (charset ( un /*CUnit::UnitsDic().at(un).to_string ()*/  ));
        caption(charset (def));
        //ext_event().selected=[&](combox& cb)
        //{
        //    _cb.caption(_cb.caption().substr(6, _cb.caption().find_first_of(STR(" ="),6)-6 )); 
        //};
    }

    double to_def   (double val) ///< Convert a value in user selected Unit into Default Unit 
    {
        return CUnit::CUnit(nana::charset (caption()) ,  _defUnit  ).conv (val);
    }
    double to_def   (double val, const CUnit::unit_name& un ) ///< Convert a value in Unit un into Default Unit 
    {
        return CUnit::CUnit(un ,  _defUnit  ).conv (val);
    }
    double from_def (double val) ///< Convert a value in Default Unit into user selected Unit 
    {
        return CUnit::CUnit( _defUnit, nana::charset (caption())  ).conv (val);
    }
    double from_def (double val, const CUnit::unit_name& un ) ///< Convert a value in Default Unit into Unit un 
    {
        return CUnit::CUnit( _defUnit, nana::charset (caption())  ).conv (val);
    }
    double convert_to(double val, const CUnit::unit_name& un) ///< Convert a value in user selected Unit into Unit un 
    {
        return CUnit::CUnit( nana::charset (caption()), un ).conv (val);
    }
    double convert_from(double val, const CUnit::unit_name& un) ///< Convert a value in Unit un into the user selected Unit
    {
        return CUnit::CUnit(un ,  nana::charset (caption()) ).conv (val);
    }

};

class NumUnitUpDown : public CompoWidget
{
    //double      _val;
    CUnit::unit_name _curr_un;
public:
    NumerUpDown _num; /// TODO: make private and provide a funtion to change the def lay, especialy the length of the label
    UnitPicker  _unit; /// TODO: make private and provide a funtion to change the def lay, especialy the length of the label
    NumUnitUpDown ( widget &wd,        
                    const string& label, 
                    double defVal,    double min,     double max,    
                    const CUnit::unit_name& def  , 
                    const string& DefLayFile =STR("NumUnitUpDonw.Lay.txt"),   
                    double step=1,   unsigned width=6, unsigned decimals=2)
        : CompoWidget (wd,label,STR("NumUnitUpDonw.Lay.txt")),
          _num(*this,label, defVal, min,max,STR("Vert-Invert.NumUpDonw.Lay.txt"),step,width,decimals),
          _unit(*this, def), _curr_un(def) //_val(defVal)
    {
        _unit.ext_event().selected=[&](combox& cb)
                                    {
                                        CUnit u(_curr_un , charset( _unit.caption() )/*CUnit::unit_name ( nana::charset(cb.option ()) ) */);
                                        if(u.error )
                                        {    _unit.caption (_unit.caption ()+STR("?"));
                                             return;
                                        }
                                        _num.Max   ( u.conv(_num.Max  () )  );
                                        _num.Min   ( u.conv(_num.Min  () )  );
                                        _num.Value ( u.conv(_num.Value() )  );
                                        if (u.conv.lineal )
                                          _num.Step( u.conv.c*_num.Step()   );
                                       
                                        _curr_un=charset(_unit.caption ());
                                    };
        InitMyLayout();
        SelectClickableWidget( _num);
        SelectClickableWidget( _unit);
        
        //_num.add_validated ( [&](){Validated ();}        );
    }
    virtual    void add_validated(const std::function<bool(void)>& v)
    {
        _num.add_validated (v); 
        //_validated.push_back (v); 
    }

     void SetDefLayout       () override
    {
        SetDefLayout       (60);
        //_DefLayout= "  <                        \n"
        //                        "  <vertical     min=125               Num                >           \n"
        //                        "  <vertical     min=50    <><vertical Unit weight=21><>  >           \n"
        //                 " > ";   

        //_num._DefLayout="  <                        \n"
        //                "                           \n"
        //                        "  <vertical weight=60       <><label weight=15 gap=1>    <>          >        \n"
        //                        "  <vertical weight=15       <><vertical UpDown weight=21><>          >        \n"
        //                        "  <vertical min=50          <><Num weight=21>            <>          >        \n"
        //                 " > ";
    }
     void SetDefLayout       (unsigned lab, unsigned n=50, unsigned unit=50)
    {
         // redefine layot of this complete widget: NumUnitUpDown

        char d[]= "  <                        \n"
                                "  <vertical     min=%u               Num                 >           \n"
                                "  <vertical     min=%u    <><vertical Unit weight=21><>  >           \n"
                         " > ";   
         // redefine layot of the NumUpDown part (inside the previus Num)
        char nd[]="  <                        \n"
                        "                           \n"
                                "  <vertical weight=%u       <><label weight=15 gap=1>     <>          >        \n"
                                "  <vertical weight=%u       <><vertical UpDown weight=21> <>          >        \n"
                                "  <vertical min=%u          <><Num weight=21>             <>          >        \n"
                         " > ";
   
             _DefLayout.resize ( std::strlen ( d)+20);
       _num. _DefLayout.resize ( std::strlen (nd)+20);
       sprintf(&      _DefLayout[0], d,lab+15+n,unit);
       sprintf(&_num. _DefLayout[0],nd,lab,15,n);
             _DefLayout.resize ( std::strlen (&      _DefLayout[0]));
       _num. _DefLayout.resize ( std::strlen (&_num. _DefLayout[0]));
     }
    void ResetLayout       (unsigned lab, unsigned n=50, unsigned unit=50)
    {
        SetDefLayout       (lab, n, unit);
        ResetDefLayout();  _num. ResetDefLayout();
        ReCollocate( );    _num.ReCollocate( );
    }

     void AsignWidgetToFields() override
    {
	    _place.field("Num"    ) << _num ;
	    _place.field("Unit"   ) << _unit ;
    }
     /// expresed in default Units
     double Value()
     {
         return _unit.to_def(_num.Value()); 
     }
     /// expresed in the especified "un" Units
     double Value(const CUnit::unit_name& un)
     {
         return _unit.convert_to(_num.Value(),un); 
     }
     /// expresed in default Units
     void   Value(double val_in_default_Units)
     {
         _num.Value(_unit.from_def(val_in_default_Units)); 
     }
     /// expresed in the especified "un" Units
     void   Value(double val, const CUnit::unit_name& un)
     {
         _num.Value(_unit.convert_from(val,un)); 
     }

};


}} // namespace nana { namespace gui {

        //cb.caption(_cb.caption().substr(6, _cb.caption().find_first_of(STR(" ="),6)-6 )); 

#endif 
