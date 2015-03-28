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

using namespace std;
using namespace collector;


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



// A class that doesn't have any pointer member dont't have any special configuration

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

    virtual ~X() {
        print("X" + to_string(n) + " deleted");
    }

    void say_hi() {
        print("hi " + to_string(n) + "!");
    }
};

void test_1() {

    gcPointer<XBase> a = new X(1);
    gcPointer<X> b = new X(2);

    a = (gcPointer<XBase>) b;

    // Wait some time until garbage collector do cleaning
    // so we can see if it was done correctly
    this_thread::sleep_for(chrono::milliseconds(1000));
    print("X1 deleted?");

    // Local pointers are destroyed
}

// ----------------------------------------------------------------------

// A class with pointer members must be configured like this template

class X_gc : public gcObject {
public:

    gcPointer<X_gc> p;
    int n = 0;

    X_gc(int a) {

		gcConnectObject do_it(this);

        n = a;
        print("X_gc" + to_string(n) + " created");
	}

    ~X_gc() {

		gcDisconnectObject do_it(this);

        print("X_gc" + to_string(n) + " deleted");
	}

	void say_hi() {
        print("hi " + to_string(n) + "!");
	}

};

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

    // Local pointers are destroyed
}

void test_3(){


    // A list of pointers
    gcListPointer<X_gc> plist = new gcList<X_gc>;

    for (int n = 0; n < 5; n++)
        plist->push_back( new X_gc(n) );

    gcList<X_gc>::Iterator i = plist->begin();
    *(++i) = new X_gc(10000);

    for (auto p : *plist) {
        print(to_string(p->n));
    }

    // Wait some time until garbage collector do cleaning
    // so we can see if it was done correctly
    this_thread::sleep_for(chrono::milliseconds(1000));
    print("X_gc1 deleted?");

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
    print("X_gc1 , X_gc4 deleted?");

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

    print("exiting thread");
}

int main()
{
    /* Init a garbage collected program */
	gcCollector new_collector;

    void (*tst_tbl[])() = {test_0, test_1, test_2, test_3, test_4, test_5};

    for(int tst_num=0; tst_num<6; tst_num++)
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
    cout << "--------------------------------------" << "\n";

	thread new_thread[THREAD_MAX];

    for (int n = 0; n < THREAD_MAX; n++){
        new_thread[n] = thread(&thread_fn);
    }

    for (int n = 0; n < THREAD_MAX; n++){
        new_thread[n].join();
    }

    return 0;
}

