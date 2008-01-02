#include "Data_Type.hpp"

Rice::Data_Type_Base::Casters Rice::Data_Type_Base::casters_;

Rice::Data_Type_Base::
Data_Type_Base()
  : Module_impl<Class, Data_Type_Base>()
{
}

Rice::Data_Type_Base::
Data_Type_Base(VALUE v)
  : Module_impl<Class, Data_Type_Base>(v)
{
}

Rice::Data_Type_Base::
~Data_Type_Base()
{
}

