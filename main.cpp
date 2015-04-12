// RULE 0: WRITE gcCollector new_collector; AT main()
//
// RULE 1: IN ORDER TO BE MANAGED BY GARBAGE COLLECTOR,
//         OBJECTS ARE CREATED WITH 'new' INSIDE OF ASSIGNAMET OF A gcPointer
//
// RULE 2.1: NEW CLASSES THAT HAVE gcPointer MEMBERS MUST BE DERIVED OF gcObject
//
//
// RULE 2.2: WRITE gcConnectObject doit(this); AT OBJECT CONSTRUCTOR
//		   AND gcDisconnectObject doit(this); AT OBJECT DESTRUCTOR
//
//
// RULE 3: USE gcPointer's CUSTOMIZED CONTAINERS: gcList, gcListPointer, etc
//
// RULE 4: WRITE gcConnectThread doit; AT THE BEGINING OF NEW THREADS

#include <iostream>
#include <string>
#include "gc/gc.h"
#include "gc/gclist.h"
#include "gc/gcmap.h"
#include "gc/gcset.h"
#include "gc/gcarray.h"

using namespace std;
using namespace gcNamespace;


thread_local int thread_x;
int thread_count = 0;
const int THREAD_MAX =10;

// Simple debugging output tool

void print(string str) {
    cout << "thread " << thread_x << ": " << str << "\n";
}

// ----------------------------------------------------------------------

void test_0() {

    gcPointer<int> i = new int;
    *i = 3;

    cout << *i << '\n';

    gcPointer<string> s = new string[4];

    s[0] = "hello";
    s[1] = " ";
    s[2] = "world";
    s[3] = "\n";

    for(*i = 0; *i < 4; (*i)++ )
        cout << s[*i];

    s = new string("new ");         //OLD CONTENT WILL BE REMOVED
    s->append("hello world");

    print(*s);

    gcPointer<char*> c = new char*{"another hello"};
    print(*c);
}

// ----------------------------------------------------------------------



// A class that doesn't have any pointer member
// doesnt't need any special configuration

class XBase {
public:
    int n;
    virtual ~XBase(){}
};

class X : public XBase
{
public:
    X(int a) {
        n = a;
        print("X" + to_string(n) + " created");
    }

    ~X() override {
        print("X" + to_string(n) + " deleted");
    }

    void say_hi() {
        print("hi " + to_string(n) + "!");
    }
};

void test_1() {

    gcPointer<XBase> a = new X(1);
    gcPointer<X> b = new X(2);

    a = (gcPointer<XBase>) b; // no error check yet

    // Wait some time until garbage collector do cleaning
    // so we can see if it was done correctly
    this_thread::sleep_for(chrono::milliseconds(1000));
    print("X1 deleted?");

    // Local pointers are destroyed
}

// ----------------------------------------------------------------------

// A class with pointer members must be
// configured like this template

class X_gc : public gcObject {
public:

    gcPointer<X_gc> p;
    int n = 0;

    X_gc(int a) {

        gcConnectObject do_it(this);

        n = a;
        print("X_gc" + to_string(n) + " created");
    }

    ~X_gc() override {

        gcDisconnectObject do_it(this);

        print("X_gc" + to_string(n) + " deleted");
    }

    void say_hi() {
        print("hi " + to_string(n) + "!");
    }

};


void test_2_sub1(const gcPointer<X_gc>& c3){

    // A normal pointer
    gcPointer<X_gc> nonscoped_c3 = c3;

    // this function does nothing
}

void test_2_sub2( const gcPointer<X_gc>& c4){

    // A scoped pointer
    gcScopedPointer<X_gc> scoped_c4 = c4;
    // X_gc2000 will be deleted now since it is scoped here
}

void test_2()
{
    gcPointer<X_gc> p;

    p = new X_gc(1);
    p->p = new X_gc(2);
    p->p->p = new X_gc(3);

    p->p->p->p = p->p;			// circular link
    p->p = new X_gc(4);         // unlink objects 2 and 3 from root

    p->say_hi();                // method calls
    p->p->say_hi();


    // Wait some time until garbage collector do cleaning
    // so we can see if it was done correctly
    this_thread::sleep_for(chrono::milliseconds(1000));
    print("X_gc2 X_gc3 deleted?");

    // This is a gcObject pseudo rvalue
    X_gc *rval = new X_gc(5);

    // X_gc5 now is lvalue and finalizable
    gcPointer<X_gc> c1 = rval;

    // Add a reference
    gcPointer<X_gc> c2 = rval;

    // make X_gc5 not finalizable
    c1.gc_make_nonfinalizable();

    // Remove references
    c1 = 0;
    c2 = p;

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("X_gc5 NOT deleted?");

    // Delete object
    c1 = rval;
    c1.gc_make_finalizable();
    c1.gc_deallocate();

    // test a scoped pointer
    gcPointer<X_gc> c3 = new X_gc(1000);
    gcPointer<X_gc> c4 = new X_gc(2000);

    test_2_sub1(c3);
    test_2_sub2(c4);

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("X_gc2000 deleted?");

    print(to_string(c4 == 0));

    gcUniquePointer<X_gc> c5 = new X_gc(4000);
    c5 = new X_gc(5000);

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("X_gc4000 deleted?");

    // Local pointers are destroyed
}




void test_3(){

    // A list of pointers
    gcListPointer<X_gc> plist = new gcList<X_gc>;

    for (int n = 0; n < 5; n++)
        plist->push_back( new X_gc(n) );

    gcList<X_gc>::iterator i = plist->begin();
    *(++i) = new X_gc(10000);

    for (auto p : *plist) {
        print(to_string(p->n));
    }

    // Wait some time until garbage collector do cleaning
    // so we can see if it was done correctly
    this_thread::sleep_for(chrono::milliseconds(1000));
    print("X_gc1 deleted?");

    *plist = {{new X_gc(50),new X_gc(60)}};

    // Local pointers are destroyed
}

void test_4(){

    gcMapPointer<string,X_gc> pmap = new gcMap<string,X_gc>;

    pmap["KEY"] = new X_gc(1);
    pmap["KEY"] = new X_gc(2);

    // Wait some time until garbage collector do cleaning
    // so we can see if it was done correctly
    this_thread::sleep_for(chrono::milliseconds(1000));
    print("X_gc1 deleted?");

    // Local pointers are destroyed
}

void test_5(){

    gcPointer<X_gc> p1 = new X_gc(1), p2 = new X_gc(2);

    gcSetPointer<X_gc> pset = new gcSet<X_gc>;

    pset->insert(p1);
    pset->insert(p2);
    pset->insert(p2);

    for (auto p : *pset) {
        print(to_string(p->n));
    }
    print("Set has X_gc1 and X_gc2 one time only?");

    // Local pointers are destroyed
}

void test_6(){

    gcPointer<X_gc> p1 = new X_gc(1), p2 = new X_gc(2);

    gcArrayPointer<X_gc,2> parr = new gcArray<X_gc,2>;

    if(parr == parr){
    parr[0]=p1;
    parr[1]=p2;}

    for (gcPointer<X_gc> p : *parr) {
        print(to_string(p->n));
    }

    // Local pointers are destroyed
}

void test_7(){

    gcPointer<X_gc> p1 = new X_gc(1);
    gcWeakPointer<X_gc> wp = p1;

    p1 = 0;
    // Wait some time until garbage collector do cleaning
    // so we can see if it was done correctly
    this_thread::sleep_for(chrono::milliseconds(1000));
    print("X_gc1 deleted?");
}

void test_8(){

    int max = 0;
    int min = 1000000;
    print("Please, Wait. Testing GC interruptions time");

    auto tot_start = std::chrono::steady_clock::now();

    for(int n=0;n<30; n++) {

        // TICK PROCESS
        auto tick_start = std::chrono::steady_clock::now();

        for(int i=0;  i<100;  i++){
            gcPointer<int>  x = new int[20];
            for(int ii=0; ii<100; ii++) {
               gcPointer<int>  y = new int[20];
               x = y;
            }
        }

        auto tick_end = std::chrono::steady_clock::now();

        // UPDATE MAX/MIN
        int val =  std::chrono::duration_cast<std::chrono::microseconds>(tick_end-tick_start).count();

        if(val>max){
            max = val;
        }

        if(val<min){
            min = val;
        }
    }

    auto tot_end = std::chrono::steady_clock::now();

    int total = std::chrono::duration_cast<std::chrono::microseconds>(tot_end-tot_start).count();

    print(string("GC-RESULTS:"));
    print(string("total mcs  = ") + to_string(total));
    print(string("max tick  = ") + to_string(max));
    print(string("min tick  = ") + to_string(min));
    print(string("max - min = ") + to_string(max-min));

    max = 0;
    min = 1000000;
    print("Please, Wait. Testing standard C++");

    auto tot_start_std = std::chrono::steady_clock::now();

    for(int n=0;n<30; n++) {

        // TICK PROCESS
        auto tick_start_std = std::chrono::steady_clock::now();

        for(int i=0;  i<100;  i++){
            int*  x = new int[20];
            for(int ii=0; ii<100; ii++) {
               int*  y = new int[20];
               x = y;
               delete y;
            }
            delete x;
        }

        auto tick_end_std = std::chrono::steady_clock::now();

        // UPDATE MAX/MIN
        int val =  std::chrono::duration_cast<std::chrono::microseconds>(tick_end_std-tick_start_std).count();

        if(val>max){
            max = val;
        }

        if(val<min){
            min = val;
        }
    }

    auto tot_end_std = std::chrono::steady_clock::now();

    total = std::chrono::duration_cast<std::chrono::microseconds>(tot_end_std-tot_start_std).count();

    print(string("GC-RESULTS:"));
    print(string("total mcs  = ") + to_string(total));
    print(string("max tick  = ") + to_string(max));
    print(string("min tick  = ") + to_string(min));
    print(string("max - min = ") + to_string(max-min));
}

void thread_fn() {

    thread_count++;
    thread_x = thread_count;

    // Each thread Must be conneccted to grabage collector
    gcConnectThread doit;

    test_0();
    print("exit test_0");

    test_1();
    print("exit test_1");

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("X2 deleted?");

    test_2();
    print("exit test_2");

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("X_gc1 , X_gc4, X_gc1000 deleted?");

    test_3();
    print("exit test_3");

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("X_gc0 , X_gc1000 , X_gc2, X_gc3, X_gc4 deleted?");

    test_4();
    print("exit test_4");

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("X_gc2 deleted?");

    test_5();
    print("exit test_5");

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("X_gc1 , X_gc2 deleted?");

    test_6();
    print("exit test_6");

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("X_gc1 , X_gc2 deleted?");

    test_7();
    print("exit test_7");

    print("exiting thread");
}

int main()
{           
    /* Init a garbage collected program */
    gcCollector new_collector;

    // Time between each mark-sweep event = 300
    // new_collector.sleep_time = 300 (If you do it, make a lock)

    void (*tst_tbl[])() = {test_8, test_1, test_2, test_3, test_4, test_5, test_6, test_7, test_0};

    for(int tst_num=0; tst_num<9; tst_num++)
    {
        cout << "test " << tst_num << "\n";
        cout << "--------------------------------------" << "\n";

        tst_tbl[tst_num]();

        char c = '\0';
        while (c!='\n'){
            c = getchar();
        }
    }

    cout << "multy thread test\n";
    cout << "A lot of mess will be shown\n";
    cout << "--------------------------------------" << "\n";

    char c = '\0';
    while (c!='\n'){
        c = getchar();
    }

    thread new_thread[THREAD_MAX];

    for (int n = 0; n < THREAD_MAX; n++){
        new_thread[n] = thread(&thread_fn);
    }

    for (int n = 0; n < THREAD_MAX; n++){
        new_thread[n].join();
    }

    cout << "multy thread test succesfull\n";
    cout << "--------------------------------------" << "\n";

    return 0;
}

