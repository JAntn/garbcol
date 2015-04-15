#ifndef _GC_MACRO
#define _GC_MACRO

#define GC_CONNECT_OBJECT(__GC_SELF,__GC_BASE)\
    public:\
    template<class ...Args>\
    __GC_SELF(Args... args) : __GC_BASE(gc_delegate) {\
        gcConnectObject doit(this);\
        gc_create(args...);\
    }\
    ~__GC_SELF() override {\
        gcDisconnectObject doit(this);\
        gc_destroy();\
    }\
    __GC_SELF(gc_delegate_t):__GC_BASE(gc_delegate){}\

#endif // GCMACRO

