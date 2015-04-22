///////////////////////////////////////////////////////////////////////////////
// File: gcmacro.h
// Description:
// inheritance macro

#ifndef _GC_MACRO_H
#define _GC_MACRO_H

#define GC_CONNECT_OBJECT(__GC_SELF,__GC_BASE)\
    public:\
    template<class ...Args>\
    __GC_SELF(Args... args) : __GC_BASE(gc_delegate) {\
        gcConnectObject connect_object(this);\
        gc_create(args...);\
    }\
    ~__GC_SELF() override {\
        gcDisconnectObject disconnect_object(this);\
        gc_destroy();\
    }\
    __GC_SELF(gc_delegate_t):__GC_BASE(gc_delegate){}\

#endif // _GC_MACRO_H

