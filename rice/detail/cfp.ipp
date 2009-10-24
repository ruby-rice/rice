namespace detail
{

struct rb_thread_struct
{
    VALUE self;
    void *vm;
    VALUE *stack;
    unsigned long stack_size;
    VALUE *cfp;
    /* ... */
};

typedef struct rb_thread_struct rb_thread_t;

} // namespace detail

extern "C" detail::rb_thread_t * ruby_current_thread;

inline
VALUE *
Rice::detail::
cfp()
{
  return ruby_current_thread->cfp;
}

inline
VALUE &
Rice::detail::
cfp_data_memo_node_and_pc(VALUE * cfp)
{
  return cfp[0];
}

inline
VALUE &
Rice::detail::
cfp_self(VALUE * cfp)
{
  return cfp[5];
}

inline
VALUE &
Rice::detail::
cfp_method_class(VALUE * cfp)
{
  return cfp[11];
}

