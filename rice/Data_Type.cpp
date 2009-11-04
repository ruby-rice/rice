#include "Data_Type.hpp"

Rice::Data_Type_Base::Casters * Rice::Data_Type_Base::casters_ = 0;

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

Rice::Data_Type_Base::Casters &
Rice::Data_Type_Base::
casters()
{
  // Initialize the casters_ if it is null
  if (!casters_)
  {
    // First, see if it has been previously registered with the
    // interpreter (possibly by another extension)
    Class object(rb_cObject);
    Object casters_object(object.attr_get("__rice_casters__"));

    if (casters_object.is_nil())
    {
      // If it is unset, then set it for the first time
      Data_Object<Casters> casters(
          new Casters,
          rb_cObject);
      object.iv_set("__rice_casters__", casters);
      casters_ = casters.get();
    }
    else
    {
      // If it is set, then use the existing value
      Data_Object<Casters> casters(
          casters_object);
      casters_ = casters.get();
    }
  }

  return *casters_;
}

