#ifndef Rice__detail__Native__hpp_
#define Rice__detail__Native__hpp_

namespace Rice::detail
{
  class Native;

  class Resolved
  {
  public:
    inline bool operator<(Resolved other);
    inline bool operator>(Resolved other);

    Convertible convertible;
    double parameterMatch;
    Native* native;
  };

  class Native
  {
  public:
    static VALUE resolve(int argc, VALUE* argv, VALUE self);
  public:
    virtual ~Native() = default;
    VALUE call(int argc, VALUE* argv, VALUE self);

    virtual Resolved matches(int argc, VALUE* argv, VALUE self) = 0;
    virtual VALUE operator()(int argc, VALUE* argv, VALUE self) = 0;
  };
}

#endif // Rice__detail__Native__hpp_
