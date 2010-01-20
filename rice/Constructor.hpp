#ifndef Rice__Constructor__hpp_
#define Rice__Constructor__hpp_

// This is a generated file.  DO NOT EDIT!!


// This causes problems with certain C++ libraries
#undef TYPE

#include "to_from_ruby_defn.hpp"
#include "detail/method_data.hpp"
namespace Rice
{
  template<typename T, typename Arg0_T=void, typename Arg1_T=void, typename Arg2_T=void, typename Arg3_T=void, typename Arg4_T=void, typename Arg5_T=void, typename Arg6_T=void, typename Arg7_T=void, typename Arg8_T=void, typename Arg9_T=void, typename Arg10_T=void, typename Arg11_T=void, typename Arg12_T=void, typename Arg13_T=void, typename Arg14_T=void, typename Arg15_T=void>
  class Constructor
  {
  private:
    Constructor()
    {
    }
  };

  template<typename T>
  class Constructor<T, void, void, void, void, void, void, void, void, void, void, void, void, void, void, void>
  {
  public:
    static void construct(Object self)
    {
      DATA_PTR(self.value()) = new T();
    }
  };


  template<typename T>
  class Constructor<T, Object, void, void, void, void, void, void, void, void, void, void, void, void, void, void>
  {
  public:
    static void construct(Object self)
    {
      DATA_PTR(self.value()) = new T(self);
    }
  };

  
    template<typename T, typename Arg0_T>
    class Constructor<T, Arg0_T, void, void, void, void, void, void, void, void, void, void, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0)
        {
          DATA_PTR(self.value()) = new T(arg0);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T>
    class Constructor<T, Arg0_T, Arg1_T, void, void, void, void, void, void, void, void, void, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1)
        {
          DATA_PTR(self.value()) = new T(arg0, arg1);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T>
    class Constructor<T, Arg0_T, Arg1_T, Arg2_T, void, void, void, void, void, void, void, void, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2)
        {
          DATA_PTR(self.value()) = new T(arg0, arg1, arg2);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T>
    class Constructor<T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, void, void, void, void, void, void, void, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3)
        {
          DATA_PTR(self.value()) = new T(arg0, arg1, arg2, arg3);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T>
    class Constructor<T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, void, void, void, void, void, void, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4)
        {
          DATA_PTR(self.value()) = new T(arg0, arg1, arg2, arg3, arg4);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T>
    class Constructor<T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, void, void, void, void, void, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4, Arg5_T arg5)
        {
          DATA_PTR(self.value()) = new T(arg0, arg1, arg2, arg3, arg4, arg5);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T>
    class Constructor<T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, void, void, void, void, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4, Arg5_T arg5, Arg6_T arg6)
        {
          DATA_PTR(self.value()) = new T(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T>
    class Constructor<T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, void, void, void, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4, Arg5_T arg5, Arg6_T arg6, Arg7_T arg7)
        {
          DATA_PTR(self.value()) = new T(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T>
    class Constructor<T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, void, void, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4, Arg5_T arg5, Arg6_T arg6, Arg7_T arg7, Arg8_T arg8)
        {
          DATA_PTR(self.value()) = new T(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T>
    class Constructor<T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, void, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4, Arg5_T arg5, Arg6_T arg6, Arg7_T arg7, Arg8_T arg8, Arg9_T arg9)
        {
          DATA_PTR(self.value()) = new T(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T>
    class Constructor<T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4, Arg5_T arg5, Arg6_T arg6, Arg7_T arg7, Arg8_T arg8, Arg9_T arg9, Arg10_T arg10)
        {
          DATA_PTR(self.value()) = new T(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T>
    class Constructor<T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4, Arg5_T arg5, Arg6_T arg6, Arg7_T arg7, Arg8_T arg8, Arg9_T arg9, Arg10_T arg10, Arg11_T arg11)
        {
          DATA_PTR(self.value()) = new T(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T>
    class Constructor<T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4, Arg5_T arg5, Arg6_T arg6, Arg7_T arg7, Arg8_T arg8, Arg9_T arg9, Arg10_T arg10, Arg11_T arg11, Arg12_T arg12)
        {
          DATA_PTR(self.value()) = new T(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T>
    class Constructor<T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4, Arg5_T arg5, Arg6_T arg6, Arg7_T arg7, Arg8_T arg8, Arg9_T arg9, Arg10_T arg10, Arg11_T arg11, Arg12_T arg12, Arg13_T arg13)
        {
          DATA_PTR(self.value()) = new T(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T>
    class Constructor<T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4, Arg5_T arg5, Arg6_T arg6, Arg7_T arg7, Arg8_T arg8, Arg9_T arg9, Arg10_T arg10, Arg11_T arg11, Arg12_T arg12, Arg13_T arg13, Arg14_T arg14)
        {
          DATA_PTR(self.value()) = new T(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14);
        }
    };

    
    template<typename T, typename Arg0_T>
    class Constructor<T, Object, Arg0_T, void, void, void, void, void, void, void, void, void, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0)
        {
          DATA_PTR(self.value()) = new T(self, arg0);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T>
    class Constructor<T, Object, Arg0_T, Arg1_T, void, void, void, void, void, void, void, void, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1)
        {
          DATA_PTR(self.value()) = new T(self, arg0, arg1);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T>
    class Constructor<T, Object, Arg0_T, Arg1_T, Arg2_T, void, void, void, void, void, void, void, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2)
        {
          DATA_PTR(self.value()) = new T(self, arg0, arg1, arg2);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T>
    class Constructor<T, Object, Arg0_T, Arg1_T, Arg2_T, Arg3_T, void, void, void, void, void, void, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3)
        {
          DATA_PTR(self.value()) = new T(self, arg0, arg1, arg2, arg3);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T>
    class Constructor<T, Object, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, void, void, void, void, void, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4)
        {
          DATA_PTR(self.value()) = new T(self, arg0, arg1, arg2, arg3, arg4);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T>
    class Constructor<T, Object, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, void, void, void, void, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4, Arg5_T arg5)
        {
          DATA_PTR(self.value()) = new T(self, arg0, arg1, arg2, arg3, arg4, arg5);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T>
    class Constructor<T, Object, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, void, void, void, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4, Arg5_T arg5, Arg6_T arg6)
        {
          DATA_PTR(self.value()) = new T(self, arg0, arg1, arg2, arg3, arg4, arg5, arg6);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T>
    class Constructor<T, Object, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, void, void, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4, Arg5_T arg5, Arg6_T arg6, Arg7_T arg7)
        {
          DATA_PTR(self.value()) = new T(self, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T>
    class Constructor<T, Object, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, void, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4, Arg5_T arg5, Arg6_T arg6, Arg7_T arg7, Arg8_T arg8)
        {
          DATA_PTR(self.value()) = new T(self, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T>
    class Constructor<T, Object, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, void, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4, Arg5_T arg5, Arg6_T arg6, Arg7_T arg7, Arg8_T arg8, Arg9_T arg9)
        {
          DATA_PTR(self.value()) = new T(self, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T>
    class Constructor<T, Object, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, void, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4, Arg5_T arg5, Arg6_T arg6, Arg7_T arg7, Arg8_T arg8, Arg9_T arg9, Arg10_T arg10)
        {
          DATA_PTR(self.value()) = new T(self, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T>
    class Constructor<T, Object, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, void, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4, Arg5_T arg5, Arg6_T arg6, Arg7_T arg7, Arg8_T arg8, Arg9_T arg9, Arg10_T arg10, Arg11_T arg11)
        {
          DATA_PTR(self.value()) = new T(self, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T>
    class Constructor<T, Object, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, void, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4, Arg5_T arg5, Arg6_T arg6, Arg7_T arg7, Arg8_T arg8, Arg9_T arg9, Arg10_T arg10, Arg11_T arg11, Arg12_T arg12)
        {
          DATA_PTR(self.value()) = new T(self, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
        }
    };

    
    template<typename T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T>
    class Constructor<T, Object, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, void>
    {
      public:
        static void construct(Object self, Arg0_T arg0, Arg1_T arg1, Arg2_T arg2, Arg3_T arg3, Arg4_T arg4, Arg5_T arg5, Arg6_T arg6, Arg7_T arg7, Arg8_T arg8, Arg9_T arg9, Arg10_T arg10, Arg11_T arg11, Arg12_T arg12, Arg13_T arg13)
        {
          DATA_PTR(self.value()) = new T(self, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13);
        }
    };

    } // namespace Rice


#endif // Rice__Constructor__hpp_

