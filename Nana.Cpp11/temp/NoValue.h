#ifndef _NO_VALUE_H
#define _NO_VALUE_H

#include "C:\Prog\ExtLib\std\limits_constexpr.h"

constexpr int i = 2;
constexpr int ixi(int i_){ return i_ * i_; }
constexpr int j= { STD::numeric_limits<int>::max() };


template <typename Num, Num NoValue = i+i /*= STD::numeric_limit<Num>::max()*/>
struct WithNoValue
{
    Num _i;


};
WithNoValue<int> I;

#endif


//using MIndex = unsigned char;
//
//const MIndex  Invalid_Menu_idx = std::numeric_limits<MIndex>::max();     // global ?? Could be another "logical" value, for example, just 100.
// 
//
////struct SmartMIndex
////{
//// MIndex  i {Invalid_Menu_idx};       
//// operator size_t(size_t idx)
////  {
////   if ( idx >= Invalid_Menu_idx )                                  // or if ( idx == npos)     ?????
////           return SmartMIndex{} ;                                    // not found
////   else
////          return SmartMIndex{  MIndex (t_idx)}  ;                          
////  }
////
////};
////SmartMIndex Invalid_SMenu_idx;        // global ??


