# garbcol

Smart pointers with Mark-Sweep garbage collector with multi-thread support.

The purpose of this library is to proportionate tools for programming in C++11 with automatic memory management. This libray is totally written in C++11 for a GNU compiler. In this first version, it is provided  with smart pointers whose contents are freed automatically, C++11 thread support, and a collection of containers that 'wrap' the STL original one.

Programming with this library require some basic rules. First, Garbage Collector must be initiated in main function:

```C++

#include "gc/gc.h"

int main(int argc, char* argv[])
{
    gcCollector new_collector;
    
    ...
}

```

Build and manage pointers is plain:

```C++

gcPointer<string> s;
    
s = new string("hello");         
s->append(" world");

cout << *s;

...

gcPointer<int> i = new int[4];

i[0] = 0;
i[1] = 1;
i[2] = 2;
i[3] = 3;

i = new int; // previous array is freed

*i = 1;

```

You have access to the content and member functions through operators * and -> respectively. If the content is an array, you can acces to items operator [].

These smart pointers are for dynamic memory. Do not initialize it with static memory. Once the content is not referenced in any pointer, it is freed automatically. Since it is a Garbage Collector of Mark-Sweep kind, the memory cleaning is effective even with circular reference structures.

Some STL containers have been adapted to work with pointers:

```C++

gcListPointer<int> plist = new gcList<int>;
    
for (int n = 0; n < 5; n++){

    // call to std::list<int>::push_back

    plist->push_back( new int(n) ); 
}

  
// get std::list<int>::iterator

gcList<int>::Iterator it = plist->begin();

*(++it) = new int(1234);


// output list

for (auto p : *plist) {
    cout << *p;
}

...

gcMapPointer<string,int> pmap = new gcMap<string,int>;

pmap["KEY"] = new int(1);
pmap["KEY"] = new int(2);

...

gcPointer<int> p1 = new int(1), 
               p2 = new int(2);

gcSetPointer<int> pset = new gcSet<int>;

pset->insert(p1);
pset->insert(p2);
pset->insert(p2);         // without effect, p2 is already in set
pset->insert(new int(2)); // insert a new pointer to set

```

You can find them in:

`gcarray.h, gcdeque.h, gclist.h, gcvector.h, gcmap.h, gcset.h`

The STL cannot be used with these smart pointers directly. The JV library adapts the containers in a manner that these containers are still usable, but with a new appareance. These containers use their own allocator. It is made an internal interface with wich it can traverse all nodes in the Mark-Sweep process.

If a class has a smart pointer member, then it must have the following template:

```C++

// gcObject is the base class for all objects 
// managed by GC

class X : public gcObject 
{
public:

    gcPointer<X> p;
    ...

    X(...) {
        gcConnectObject do_it(this);
	  ...
    }

    virtual ~X() {
        gcDisconnectObject do_it(this);
	  ...
    }

    gcPointer<X>& get_example(...){
	  return p;
    }

    void set_example(const gcPointer<X>& other){
	  p = other;
    }
};

```

Then, it can be used like other classes:

```C++
...

gcPointer<X> p = new X(1);

p->set_example(new X(2));

p->p->p = p;      // make a circular reference

p = new X(3);     // X(1) and X(2) are freed

...

//X* p = new X(0)   // **GC break!
//X obj(0)          // **GC break!

```

This pattern is due to that all objects must be connected to a root node. These links are internaly managed and require the smart pointers use.

If threads are used, they must connect to GC:

```C++

void thread_fn() {         
    gcConnectThread doit;
    ...
}

thread t(thread_fn);

```
J. Anton Viñals. 2015.
