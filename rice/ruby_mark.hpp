#ifndef ruby_mark__hpp
#define ruby_mark__hpp

//! Default function to call to mark a data object.
/*! This function can be specialized for a particular type to override
 *  the default behavior (which is to not mark any additional objects).
 */
template<typename T>
void ruby_mark(T * /* obj */)
{
}

#endif // ruby_mark__hpp
