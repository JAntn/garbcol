//
// THIS FILE IS AN INTENSIVE TEST FOR THE GARBAGE COLLECTOR
// SEE README FOR MORE PLAIN DOCUMENTATION
//

// RULE 0:      WRITE gcCollector new_collector; AT main()
// RULE 1:      IN ORDER TO BE MANAGED BY GARBAGE COLLECTOR,
//              OBJECTS ARE CREATED WITH 'new' INSIDE OF ASSIGNAMET OF A gcPointer
// RULE 2:      USE gcPointer's CUSTOMIZED CONTAINERS: gcList, gcListPointer, etc
// RULE 3:      WRITE gcConnectThread connect_thread; AT THE BEGINING OF THREADS OTHER THAN MAIN
// RULE 4.1:    NEW CLASSES THAT HAVE gcPointer MEMBERS MUST BE DERIVED OF gcObject OR
//              ANY DERIVED FROM IT BUT ONLY ONE AT ONCE
// RULE 4.2:    CALL MACRO GC_CONNECT_OBJECT( `this_class`, `base_class` ) WHERE `base_class`
//              CAN BE gcObject OR ANY DERIVED FROM IT
// RULE 4.2:    USE gc_create AND gc_destroy AS CONSTRUCTOR AND DESTRUCTOR
// RULE 5:      POINTERS TO POINTERS AR NOT ALLOWED

#include <iostream>
#include <string>
#include "gc/gc.h"
#include "gc/gclist.h"
#include "gc/gcmap.h"
#include "gc/gcset.h"
#include "gc/gcarray.h"

using namespace std;
using namespace gcNamespace;

///////////////////////////////////////////////////////////////////////////////
// thread vars

thread_local int thread_x;
int thread_count = 0;
const int THREAD_MAX =4;
std::mutex a_mutex;

///////////////////////////////////////////////////////////////////////////////
// debugging tools

void print(string str) {
    cout << "thread " << thread_x << ": " << str << "\n";
}

void wait_enter_key()
{
    cout << "\n[Press <Enter> key to continue to next test]\n\n";
    char c = '\0';
    while (c!='\n'){
        c = getchar();
    }
}

///////////////////////////////////////////////////////////////////////////////
// simple examples

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

///////////////////////////////////////////////////////////////////////////////
// standard c++ class example
//
// a class that doesn't have any pointer member
// doesnt't need any special configuration

class Std_ClassBase {
public:
    int n;
    virtual ~Std_ClassBase(){}
};

class Std_Class : public Std_ClassBase
{
public:
    Std_Class(int a) {
        n = a;
        print("Std_Class" + to_string(n) + " created");
    }

    ~Std_Class() override {
        print("Std_Class" + to_string(n) + " deleted");
    }
};

void test_1() {

    gcPointer<Std_ClassBase> a = new Std_Class(1);
    gcPointer<Std_Class> b = new Std_Class(2);

    a = (gcPointer<Std_ClassBase>) b;

    // Wait some time until garbage collector do cleaning
    // so we can see if it was done correctly
    this_thread::sleep_for(chrono::milliseconds(1000));
    print("Std_Class1 deleted?");

    // Local pointers are destroyed
}

///////////////////////////////////////////////////////////////////////////////
// GC class example
//
// a class with pointer members must be configured like this template

class GC_BaseClass : public gcObject
{
    GC_CONNECT_OBJECT(GC_BaseClass,gcObject)

    gcPointer<GC_BaseClass> GC_BaseClass_ptr;

    void gc_create() {
        //print(string("create GC_BaseClass"));
    }

    void gc_destroy() {
        //print("destroy GC_BaseClass");
    }
};

///////////////////////////////////////////////////////////////////////////////
// interface

class MsgInterface {
public:
    virtual void print_msg()=0;
};

///////////////////////////////////////////////////////////////////////////////
// GC class example

class GC_Class : public  GC_BaseClass, public MsgInterface
{
    GC_CONNECT_OBJECT(GC_Class, GC_BaseClass)

    gcPointer<GC_Class> p;
    int n;

    void gc_create(int a) {
        n = a;
        print("GC_Class" + to_string(n) + " created");
        GC_BaseClass::gc_create();
    }

    void gc_destroy() {
        print("GC_Class" + to_string(n) + " deleted");
    }

    void print_msg() override {
        print("hi " + to_string(n) + "!");
        GC_BaseClass_ptr = p;
    }
};

void test_2_sub1(const gcPointer<GC_Class>& c3){

    // A normal pointer
    gcPointer<GC_Class> nonscoped_c3 = c3;

    // this function does nothing
}

void test_2_sub2( const gcPointer<GC_Class>& c4){

    // A scoped pointer
    gcScopedPointer<GC_Class> scoped_c4 = c4;

    // GC_Class2000 will be deleted now since it is scoped here
}

void test_2()
{
    gcPointer<GC_Class> p;

    p = new GC_Class(1);
    p->p = new GC_Class(2);
    p->p->p = new GC_Class(3);

    p->p->p->p = p->p;			// circular link
    p->p = new GC_Class(4);         // unlink objects 2 and 3 from root

    p->print_msg();                // method calls
    p->p->print_msg();


    // Wait some time until garbage collector do cleaning
    // so we can see if it was done correctly
    this_thread::sleep_for(chrono::milliseconds(1000));
    print("GC_Class2 GC_Class3 deleted?");

    // GC_Class5 now is lvalue and finalizable
    gcPointer<GC_Class> c1 = new GC_Class(5);

    // Add a reference
    gcPointer<GC_Class> c2 = c1;


    // make GC_Class5 not finalizable
    c1.gc_make_nonfinalizable();
    c1.gc_deallocate();


    this_thread::sleep_for(chrono::milliseconds(1000));
    print("GC_Class5 NOT deleted?");

    // Delete object
    c1.gc_make_finalizable();
    c1.gc_deallocate();

    // test a scoped pointer
    gcPointer<GC_Class> c3 = new GC_Class(1000);
    gcPointer<GC_Class> c4 = new GC_Class(2000);

    test_2_sub1(c3);
    test_2_sub2(c4);

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("GC_Class2000 deleted?");

    print(to_string(c4 == 0));

    gcUniquePointer<GC_BaseClass> c5 = new GC_Class(4000);

    //gcUniquePointer<GC_Class> c6(new GC_Class(5000));
    //c5 = c6; error: casting is forbbiden for unique pointers

    //gcUniquePointer<GC_BaseClass> c6(new GC_Class(5000));
    //c5 = c6; error: assignation from lvalue

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("GC_Class4000 deleted?");

    // Local pointers are destroyed
}

///////////////////////////////////////////////////////////////////////////////
// container examples

void test_3(){

    // A list of pointers
    gcListPointer<GC_Class> plist = new gcList<GC_Class>;

    for (int n = 0; n < 5; n++)
        plist->push_back( new GC_Class(n) );

    gcList<GC_Class>::iterator i = plist->begin();
    *(++i) = new GC_Class(10000);

    for (auto p : *plist) {
        print(to_string(p->n));
    }

    // Wait some time until garbage collector do cleaning
    // so we can see if it was done correctly
    this_thread::sleep_for(chrono::milliseconds(1000));
    print("GC_Class1 deleted?");

    *plist = {{new GC_Class(50),new GC_Class(60)}};

    // Local pointers are destroyed
}

void test_4(){

    gcMapPointer<string,GC_Class> pmap = new gcMap<string,GC_Class>;

    pmap["KEY"] = new GC_Class(1);
    pmap["KEY"] = new GC_Class(2);

    // Wait some time until garbage collector do cleaning
    // so we can see if it was done correctly
    this_thread::sleep_for(chrono::milliseconds(1000));
    print("GC_Class1 deleted?");

    // Local pointers are destroyed
}

void test_5(){

    gcPointer<GC_Class> p1 = new GC_Class(1), p2 = new GC_Class(2);

    gcSetPointer<GC_Class> pset = new gcSet<GC_Class>;

    pset->insert(p1);
    pset->insert(p2);
    pset->insert(p2);

    for (auto p : *pset) {
        print(to_string(p->n));
    }
    print("Set has GC_Class1 and GC_Class2 one time only?");

    // Local pointers are destroyed
}

void test_6(){

    gcPointer<GC_Class> p1 = new GC_Class(1), p2 = new GC_Class(2);

    gcArrayPointer<GC_Class,2> parr = new gcArray<GC_Class,2>;

    if(parr == parr){
    parr[0]=p1;
    parr[1]=p2;}

    for (gcPointer<GC_Class> p : *parr) {
        print(to_string(p->n));
    }

    // Local pointers are destroyed
}

///////////////////////////////////////////////////////////////////////////////
// weak pointer example

void test_7(){

    gcPointer<GC_Class> p1 = new GC_Class(1);
    gcWeakPointer<GC_Class> wp = p1;

    p1 = 0;

    // Wait some time until garbage collector do cleaning
    // so we can see if it was done correctly
    this_thread::sleep_for(chrono::milliseconds(1000));
    print("GC_Class1 deleted?");
}

///////////////////////////////////////////////////////////////////////////////
// benchmark

void test_8(){

    int time_tick_max = 0;
    int time_tick_min = 1000000;

    print("Please, wait. Testing GC interruptions time");

    ///////////////////////////////////////////////////////////////////////////////

    auto
    time_start = std::chrono::steady_clock::now();

    for (int n=0;n<30; n++)
    {
        auto
        time_tick_start = std::chrono::steady_clock::now();

        for (int i=0;  i<100;  i++)
        {
            gcPointer<int>  x = new int;
            for(int ii=0; ii<100; ii++) {
               gcPointer<int>  y = new int;
               x = y;
            }
        }

        auto
        time_tick_end = std::chrono::steady_clock::now();

        int
        time_tick_length =  std::chrono::duration_cast<std::chrono::microseconds>(
                time_tick_end - time_tick_start
                ).count();

        if (time_tick_length > time_tick_max){
            time_tick_max = time_tick_length;
        }

        if (time_tick_length < time_tick_min){
            time_tick_min = time_tick_length;
        }
    }

    auto
    time_end = std::chrono::steady_clock::now();

    ///////////////////////////////////////////////////////////////////////////////

    int
    time_length = std::chrono::duration_cast<std::chrono::microseconds>(
                time_end - time_start
                ).count();

    print(string("GC-RESULTS:"));
    print(string("total mcs  = ") + to_string(time_length));
    print(string("max tick   = ") + to_string(time_tick_max));
    print(string("min tick   = ") + to_string(time_tick_min));
    print(string("max - min  = ") + to_string(time_tick_max-time_tick_min));

    time_tick_max = 0;
    time_tick_min = 1000000;

    print("[Please, wait. Testing standard C++ time]");
    this_thread::sleep_for(chrono::milliseconds(1000));

    auto
    time_start_std = std::chrono::steady_clock::now();

    for (int n=0;n<30; n++) {

        // TICK PROCESS
        auto
        time_tick_start_std = std::chrono::steady_clock::now();

        for (int i=0;  i<100;  i++){
            int*  x = new int;
            for (int ii=0; ii<100; ii++) {
               int*  y = new int;
               delete y;
               y = x;
            }
            delete x;
        }

        auto
        time_tick_end_std = std::chrono::steady_clock::now();

        int
        time_tick_length_std =  std::chrono::duration_cast<std::chrono::microseconds>(
                    time_tick_end_std-time_tick_start_std
                    ).count();

        if (time_tick_length_std > time_tick_max){
            time_tick_max = time_tick_length_std;
        }

        if (time_tick_length_std < time_tick_min){
            time_tick_min = time_tick_length_std;
        }
    }

    auto
    time_end_std = std::chrono::steady_clock::now();

    time_length = std::chrono::duration_cast<std::chrono::microseconds>(
                time_end_std-time_start_std
                ).count();

    print(string("STD-RESULTS:"));
    print(string("total mcs  = ") + to_string(time_length));
    print(string("max tick   = ") + to_string(time_tick_max));
    print(string("min tick   = ") + to_string(time_tick_min));
    print(string("max - min  = ") + to_string(time_tick_max-time_tick_min));
}

///////////////////////////////////////////////////////////////////////////////
// thread fn

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
    print("GC_Class1 , GC_Class4, GC_Class1000 deleted?");

    test_3();
    print("exit test_3");

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("GC_Class0 , GC_Class1000 , GC_Class2, GC_Class3, GC_Class4 deleted?");

    test_4();
    print("exit test_4");

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("GC_Class2 deleted?");

    test_5();
    print("exit test_5");

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("GC_Class1 , GC_Class2 deleted?");

    test_6();
    print("exit test_6");

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("GC_Class1 , GC_Class2 deleted?");

    test_7();
    print("exit test_7");

    test_8();

    print("exiting thread");
}

int main()
{           
    // Init a garbage collected program
    gcCollector new_collector;

    // new_collector.sleep_time = 100

    void (*tst_tbl[])() = {test_0, test_1, test_2, test_3, test_4, test_5, test_6, test_7, test_8};

    for(int tst_num=0; tst_num<9; tst_num++)
    {
        cout << "TEST " << tst_num << "\n";
        cout << "....................................................." << "\n";

        tst_tbl[tst_num]();

        wait_enter_key();
    }

    cout << "MULTY THREAD TEST\n";
    cout << "A lot of mess will be shown\n";
    cout << "....................................................." << "\n";

    wait_enter_key();

    thread new_thread[THREAD_MAX];

    for (int n = 0; n < THREAD_MAX; n++){
        new_thread[n] = thread(&thread_fn);
    }

    for (int n = 0; n < THREAD_MAX; n++){
        while(true) {
            if( new_thread[n].joinable()) {
                new_thread[n].join();
                break;
            }
        }
    }

    cout << "TEST SUCCESFUL\n";
    cout << "....................................................." << "\n";

    return 0;
}

