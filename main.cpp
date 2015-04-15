//
// RULE 0:      WRITE gcCollector new_collector; AT main()
//
// RULE 1:      IN ORDER TO BE MANAGED BY GARBAGE COLLECTOR,
//              OBJECTS ARE CREATED WITH 'new' INSIDE OF ASSIGNAMET OF A gcPointer
//
// RULE 2:      USE gcPointer's CUSTOMIZED CONTAINERS: gcList, gcListPointer, etc
//
// RULE 3:      WRITE gcConnectThread doit; AT THE BEGINING OF THREADS OTHER THAN MAIN
//
// RULE 4.1:    NEW CLASSES THAT HAVE gcPointer MEMBERS MUST BE DERIVED OF gcObject OR
//              ANY DERIVED FROM IT BUT ONLY ONE AT ONCE
//
// RULE 4.2:    CALL MACRO _GC_DECLARE( `this_class`, `base_class` ) WHERE `base_class`
//              CAN BE gcObject OR ANY DERIVED FROM IT
//
// RULE 4.2:    USE gc_create AND gc_destroy INSTEAD CONSTRUCTORS AND DESTRUCTORS
//
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

class RockBase {
public:
    int n;
    virtual ~RockBase(){}
};

class Rock : public RockBase
{
public:
    Rock(int a) {
        n = a;
        print("Rock" + to_string(n) + " created");
    }

    ~Rock() override {
        print("Rock" + to_string(n) + " deleted");
    }
};

void test_1() {

    gcPointer<RockBase> a = new Rock(1);
    gcPointer<Rock> b = new Rock(2);

    a = (gcPointer<RockBase>) b; // no error check yet

    // Wait some time until garbage collector do cleaning
    // so we can see if it was done correctly
    this_thread::sleep_for(chrono::milliseconds(1000));
    print("Rock1 deleted?");

    // Local pointers are destroyed
}

// ----------------------------------------------------------------------

// A class with pointer members must be
// configured like this template

class Dog : public gcObject
{
    _GC_DECLARE(Dog,gcObject)

    gcPointer<Dog> dog_ptr;


    void gc_create(int c ) {

        while(c-->0) print("Guau!");
    }

    void gc_destroy() {
        print("Grr!");
    }
};

class TalkInterface {
public:
    virtual void say_hi()=0;
};

class TalkingDog : public  Dog, public TalkInterface
{
    _GC_DECLARE(TalkingDog, Dog)

    gcPointer<TalkingDog> p;
    int n;

    void gc_create(int a) {
        n = a;
        print("TalkingDog" + to_string(n) + " created");
        if (n == 1000){ this->Dog::gc_create(1000); }
    }

    void gc_destroy() {
        print("TalkingDog" + to_string(n) + " deleted");
    }

    void say_hi() override {
        print("hi " + to_string(n) + "!");
        dog_ptr = p;
    }
};

void test_2_sub1(const gcPointer<TalkingDog>& c3){

    // Rock normal pointer
    gcPointer<TalkingDog> nonscoped_c3 = c3;

    // this function does nothing
}

void test_2_sub2( const gcPointer<TalkingDog>& c4){

    // Rock scoped pointer
    gcScopedPointer<TalkingDog> scoped_c4 = c4;

    // TalkingDog2000 will be deleted now since it is scoped here
}

void test_2()
{
    gcPointer<TalkingDog> p;

    p = new TalkingDog(1);
    p->p = new TalkingDog(2);
    p->p->p = new TalkingDog(3);

    p->p->p->p = p->p;			// circular link
    p->p = new TalkingDog(4);         // unlink objects 2 and 3 from root

    p->say_hi();                // method calls
    p->p->say_hi();


    // Wait some time until garbage collector do cleaning
    // so we can see if it was done correctly
    this_thread::sleep_for(chrono::milliseconds(1000));
    print("TalkingDog2 TalkingDog3 deleted?");

    // This is a gcObject pseudo rvalue
    TalkingDog *rval = new TalkingDog(5);

    // TalkingDog5 now is lvalue and finalizable
    gcPointer<TalkingDog> c1 = rval;

    // Add a reference
    gcPointer<TalkingDog> c2 = rval;

    // make TalkingDog5 not finalizable
    c1.gc_make_nonfinalizable();

    // Remove references
    c1 = 0;
    c2 = p;

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("TalkingDog5 NOT deleted?");

    // Delete object
    c1 = rval;
    c1.gc_make_finalizable();
    c1.gc_deallocate();

    // test a scoped pointer
    gcPointer<TalkingDog> c3 = new TalkingDog(1000);
    gcPointer<TalkingDog> c4 = new TalkingDog(2000);

    test_2_sub1(c3);
    test_2_sub2(c4);

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("TalkingDog2000 deleted?");

    print(to_string(c4 == 0));

    gcUniquePointer<TalkingDog> c5 = new TalkingDog(4000);
    c5 = new TalkingDog(5000);

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("TalkingDog4000 deleted?");

    // Local pointers are destroyed
}

void test_3(){

    // Rock list of pointers
    gcListPointer<TalkingDog> plist = new gcList<TalkingDog>;

    for (int n = 0; n < 5; n++)
        plist->push_back( new TalkingDog(n) );

    gcList<TalkingDog>::iterator i = plist->begin();
    *(++i) = new TalkingDog(10000);

    for (auto p : *plist) {
        print(to_string(p->n));
    }

    // Wait some time until garbage collector do cleaning
    // so we can see if it was done correctly
    this_thread::sleep_for(chrono::milliseconds(1000));
    print("TalkingDog1 deleted?");

    *plist = {{new TalkingDog(50),new TalkingDog(60)}};

    // Local pointers are destroyed
}

void test_4(){

    gcMapPointer<string,TalkingDog> pmap = new gcMap<string,TalkingDog>;

    pmap["KEY"] = new TalkingDog(1);
    pmap["KEY"] = new TalkingDog(2);

    // Wait some time until garbage collector do cleaning
    // so we can see if it was done correctly
    this_thread::sleep_for(chrono::milliseconds(1000));
    print("TalkingDog1 deleted?");

    // Local pointers are destroyed
}

void test_5(){

    gcPointer<TalkingDog> p1 = new TalkingDog(1), p2 = new TalkingDog(2);

    gcSetPointer<TalkingDog> pset = new gcSet<TalkingDog>;

    pset->insert(p1);
    pset->insert(p2);
    pset->insert(p2);

    for (auto p : *pset) {
        print(to_string(p->n));
    }
    print("Set has TalkingDog1 and TalkingDog2 one time only?");

    // Local pointers are destroyed
}

void test_6(){

    gcPointer<TalkingDog> p1 = new TalkingDog(1), p2 = new TalkingDog(2);

    gcArrayPointer<TalkingDog,2> parr = new gcArray<TalkingDog,2>;

    if(parr == parr){
    parr[0]=p1;
    parr[1]=p2;}

    for (gcPointer<TalkingDog> p : *parr) {
        print(to_string(p->n));
    }

    // Local pointers are destroyed
}

void test_7(){

    gcPointer<TalkingDog> p1 = new TalkingDog(1);
    gcWeakPointer<TalkingDog> wp = p1;

    p1 = 0;

    // Wait some time until garbage collector do cleaning
    // so we can see if it was done correctly
    this_thread::sleep_for(chrono::milliseconds(1000));
    print("TalkingDog1 deleted?");
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
    print("TalkingDog1 , TalkingDog4, TalkingDog1000 deleted?");

    test_3();
    print("exit test_3");

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("TalkingDog0 , TalkingDog1000 , TalkingDog2, TalkingDog3, TalkingDog4 deleted?");

    test_4();
    print("exit test_4");

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("TalkingDog2 deleted?");

    test_5();
    print("exit test_5");

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("TalkingDog1 , TalkingDog2 deleted?");

    test_6();
    print("exit test_6");

    this_thread::sleep_for(chrono::milliseconds(1000));
    print("TalkingDog1 , TalkingDog2 deleted?");

    test_7();
    print("exit test_7");

    test_8();

    print("exiting thread");
}

int main()
{           
    /* Init a garbage collected program */
    gcCollector new_collector;

    // Time between each mark-sweep event = 300
    // new_collector.sleep_time = 300 (If you do it, make a lock)

    void (*tst_tbl[])() = {test_0, test_1, test_2, test_3, test_4, test_5, test_6, test_7, test_8};

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

