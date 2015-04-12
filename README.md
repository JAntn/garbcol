# Smart Pointers Garbage Collector

0.04.4

Smart pointers with mark&sweep garbage collector and multi-thread support.

This library provide tools for programming in C++11 with automatic memory management. It is written for a GNU compiler. 

Provides smart pointers whose contents are freed automatically, multi-thread compatibility, and a collection of containers that 'wrap' some STL classes.

### Initialization

It requires some basic rules. GC must be initiated in main function:

```C++

#include "gc/gc.h"

using namespace gcNamespace;

int main(int argc, char* argv[])
{
    gcCollector new_collector;
    
    //...
}

```
### Smart Pointers

Creating and using pointers is straight:

```C++

gcPointer<string> s;
    
s = new string("hello");         
s->append(" world");
cout << *s;

gcPointer<int> i = new int[3];
i[0] = 0;
i[1] = 1;
i[2] = 2;

i = new int; // previous array is freed
*i = 1;

```

You have access to the contents and member functions through operators * and -> . If the content is an array, you can acces to items with operator [].

Do not initialize pointers with static memory. 

Once the content is not referenced by any pointer, memory is freed automatically. Since it is a garbage collector of Mark&Sweep kind, the memory cleaning is effective even with circular references.

### Containers

Some STL containers have been adapted to work with pointers:

```C++

gcListPointer<int> plist = new gcList<int>;
    
for (int n = 0; n < 5; n++){
    plist->push_back( new int(n) ); 
}

gcList<int>::iterator it = plist->begin();
*(++it) = new int(1234);

for (auto p : *plist) {
    cout << *p;
}

gcMapPointer<string,int> pmap = new gcMap<string,int>;
pmap["KEY"] = new int(1);
pmap["KEY"] = new int(2);

gcPointer<int> p1 = new int(1), p2 = new int(2);
gcSetPointer<int> pset = new gcSet<int>;
pset->insert(p1);
pset->insert(p2);

```

You can find them in:

`gcarray.h, gcdeque.h, gclist.h, gcvector.h, gcmap.h, gcset.h`

The STL cannot be used with smart pointers directly. The garbcol library adapts the STL containers, but with a new appareance. Containers use their own allocator.

### New classes pattern

If a class has a smart pointer member, then it must have the following pattern:

```C++

class X : public gcObject 
{
public:

    gcPointer<X> p;

    X() {
        gcConnectObject doit(this);
	//...
    }

    ~X() override {
        gcDisconnectObject doit(this);
        //...
    }
};

```

Then, it can be used like normal classes:

```C++

gcPointer<X> p = new X(1);
p->p = new X(2); 
p->p->p = p;      // make a circular reference
p = new X(3);     // X(1) and X(2) are freed

```

This pattern is due to that all objects must be connected to a root node.

### Threads

If threads are used, they must connect to garbage collector first:

```C++

void thread_fn() {         
    gcConnectThread doit;
    //...
}

```

### Advanced

Weak pointers do not mark objects as reachable.

```C++

    gcPointer<int> strong = new int(5);
    gcWeakPointer<int> weak = strong;
    strong = 0;
    
    // 'new int(5)' will be deleted

```

Unique pointers protect data against multiple references to the same object.

```C++

    gcUniquePointer<int> unique = new int(5);
    // gcPointer<int> other = unique; Error    

```

Scoped pointers delete its contents at the end of scope anyway (even if it is shared by other active pointers).

```C++

void some_fn(){

    gcPointer<int> p = new int(1);
    some_fn2(p);
}

void some_fn2(const gcPointer<int>& p){

    gcScopedPointer<int> scoped = p;

    // 'new int(1)' will be deleted
}

```

Above, GC might leave temporally p with old value before setting it to zero.

All containers are shared pointers by default.

You can build other pointers using template parameters.

```C++

    // the same as gcPointer<int>
    gcPointer< int, gcSharedPointer_B_> shared_int;
    
    // the same as gcWeakPointer<int>
    gcPointer< int, gcWeakPointer_B_> weak_int;
    
    // scoped pointer building
    gcPointer< int, gcScopedPointer_B_<gcWeakPointer_B_>> weak_n_scoped_int;

    // unique pointers must be explicit
    gcUniquePointer<int> unique;
    
```

You can protect and object against be deleted.

```C++

int i = 1;
void some_fn() {

    gcPointer<int> p = &i;
    p.gc_make_nonfinalizable();
 
    // p <<content>> will not be deleted here. 
    // Can generate unreachable data.   
}

```

You can force deallocation.

```C++

    gcPointer<int> p = new int(1);
    p.gc_deallocate();     
    // 'new int(1)' will be deleted

```

(in)equality operators are implemented to compare pointer contents. gcNullPointer is defined, but you can use 0 or nullptr anyway.


### Limitations

Pointers cannot be builded in global scope (or be static). 

Be careful if you mix smart pointers and old ones for an object. Specially if it is a non-gcObject type.

```C++

void some_fn() {
    
    int *x = new int;
    
    gcPointer<int> p1 = x;
    gcPointer<int> p2 = x;
    
    // Crashing. 
    // p1 generates an intermediate object adapter for x.
    // p2 do the same for the same x
    // gc will try to delete x twice
}

```

gcObject derived classes are protected against the above behaviour.

A hundred times slower than c++ standard worse case (It is not optimized yet)

### Future
- bug fixing
- documentation
- pointer to pointer not implemented yet

It is intended that all features of this version (0.04.3) will be forward compatible unless it is not explicitly noticed.

### Experimental

The folder 'experimental' has the last code i'm working now. 

### License

MIT License

Joan Anton Viñals. 2015.
