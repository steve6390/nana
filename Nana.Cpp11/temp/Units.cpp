#include <../temp/Units.hpp>
#include <iostream> 



double ide(double ori){return ori;}

CUnit::nonLinealFunction    CUnit::_identity(ide);
CUnit::units                CUnit::_Units ;
CUnit::magnitudes           CUnit::_Magnitudes;
bool                        CUnit::_initialized = CUnit::Initialize();
const double                CUnit::No=6.02e23;


std::ostream& operator<<( std::ostream& o, const CUnit& u)
{
   o << "Unit: "<< u.name << " = "<< u.conv.c << " "  ;

   if (! u.conv.lineal)        o << " x nonlineal function of " ;

   o <<u.base;

   if (u.conv.s)              o << " + " << u.conv.s ;

   o << "    " << u.magnitude ;

   return o;
}

void CUnit::add()
{
    if(name=="")
    {    
        std::cerr << std::endl << "We need a new unit name in order to create a new unit. ";
        return;
    }
    if (unit_exist(name))         // this Unit name allready exist
    {    
        if (unit_exist(base))     // this base allready exist too
        {
            std::cerr << std::endl << name << " allready defined as " << _Units[name] ; 
            return;
        }                   // the unti exist, but the base dont: asume we are actualy defining the base
        if (! conv.lineal) 
        {
            std::cerr << std::endl << "Unable to define " << base << " reverting the non-lineal definition of "<< name ; 
            return;
        }
        conv.s=-conv.s/conv.c; 
        conv.c=1/conv.c;
        name.swap(base);           
    } 
    if (unit_exist(base))   // La base ya existe
    {
        if(_Units[base].magnitude != magnitude)          
            if (magnitude=="")
                magnitude=_Units[base].magnitude;    // definimos la nueva unidad en la magnitud de la base
            else 
            {    
                std::cerr << std::endl << "The new unit and the base need to be of the same magnitude." << std::endl;
                std::cerr << std::endl << base << " allready defined as " 
                            << _Units[base]  ; 
                return;
            }                 // todo normal !!
    }   else                                // La base no existe
        if (magnitude=="")
        {    
            std::cerr << std::endl << "We need a new magnitude name in order to create the new base unit: " << name   << std::endl;
            return;
        } else
            if(base=="")
            { 
                base=name;
                conv=conversion(1);
            } else 
                if (base!=name)  
                    CUnit(base,1,base,magnitude);// Creo Primero la base, como base

        // Luego la nueva Unit
    _Units[name]=*this;
    _Magnitudes[magnitude].insert(name);

    error=false ;
}
