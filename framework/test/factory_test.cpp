#define HANDLETON

#include <iostream> // for prints
#include <future> // for future
#include <chrono> // for chrono time

// #include <gtest/gtest.h>
#include <memory>
#include <unordered_map>
#include <functional>
#include <string>
#include <stdexcept>
#include "factory.hpp"

// #define RED     "\033[0;31m"
// #define BRED    "\033[1;31m"
// #define BGRN    "\033[1;32m"
// #define RESET   "\033[0m"

static const char* RED =   "\033[0;31m";
static const char* RESET = "\033[0m";

static size_t g_tests_failed = 0;

template <typename printable>
void Put(printable p)
{
	std::cout << p << std::endl;
}

template <typename printable>
void put(printable p)
{
	std::cout << p;
}

template<typename comparable>
void PrintTestResult(std::string& fail_message, comparable actual, comparable expected, int line_num)
{
	if(actual != expected)
	{
		put(RED);
		put("Test Failed on line - ");  Put(line_num);
		put("failure message: "); 		Put(fail_message);
		put("actual value: "); 			Put(actual);
		put("expected value: "); 		Put(expected);
		put(RESET);
		++g_tests_failed;
	}
}



struct Base{virtual ~Base(){} virtual std::string GetType() const = 0;};

struct DerivedA : public Base
{
	DerivedA(int num) : m_num(num){}
    std::string GetType() const override {return  "DerivedA";}
	bool operator==(DerivedA rhs) {return m_num == rhs.m_num;}
	// I know its bad but it's just a test (Ammos!!!! !!תרגע) 
private:
	int m_num;
};

struct DerivedB : public Base
{
	DerivedB(int num1, int num2): m_num1(num1), m_num2(num2){}
    std::string GetType() const override {return "DerivedB";}
	bool operator==(DerivedB rhs) {return m_num1 == rhs.m_num1 &&
														m_num2 == rhs.m_num2;}
private:
	int m_num1;
	int m_num2;
};

struct DerivedCInheritsDerivedB : public DerivedB
{
	DerivedCInheritsDerivedB(int num1, int num2, int num1ForB, int num2ForB) :
					DerivedB(num1ForB, num2ForB), m_num1(num1), m_num2(num2){}
    std::string GetType() const override {return "DerivedCInheritsDerivedB";}
	bool operator==(DerivedCInheritsDerivedB rhs)
						{return DerivedB::operator==(rhs) && m_num1 == rhs.m_num1 &&m_num2 == rhs.m_num2;}
private:
	int m_num1;
	int m_num2;
};

struct DerivedNoArgs : public Base
{
    std::string GetType() const override {return "DerivedNoArgs";}
};

static std::vector<Base> g_BaseObjects; 

using namespace ilrd;

static bool TestFactoryCreate()
{
	Factory<std::string, Base, int>* fac = 
					  Singleton<Factory<std::string, Base, int>>::GetInstance();

	fac->Add("A",
	[](int num) -> std::shared_ptr<Base> {return std::make_shared<DerivedA>(num);});

	fac->Add("B",
	[](int) -> std::shared_ptr<Base>
	{return std::make_shared<DerivedB>(1, 2);});

	fac->Add("C",
	[](int) -> std::shared_ptr<Base>
	{return std::make_shared<DerivedCInheritsDerivedB>(1, 2, 3, 4);});

	Factory<std::string, Base>* noArgFac = 
						   Singleton<Factory<std::string, Base>>::GetInstance();
	noArgFac->Add("NoArgs", []() -> std::shared_ptr<Base> {
        return std::make_shared<DerivedNoArgs>();
    });



	auto objA = fac->Create("A", 42);
	std::string msg("Create(\"A\", 42)") ;
    PrintTestResult(msg, objA != NULL, true, __LINE__);
	PrintTestResult(msg, objA->GetType(), std::string("DerivedA"), __LINE__);

    auto objB = fac->Create("B", 24);
	msg = "Create(\"B\", 24)";
	PrintTestResult(msg, objB != NULL, true, __LINE__);
	PrintTestResult(msg, objB->GetType(), std::string("DerivedB"), __LINE__);

	auto objC = fac->Create("C", 24);
	msg = "Create(\"C\", 24)";
	PrintTestResult(msg, objC != NULL, true, __LINE__);
	PrintTestResult(msg, objC->GetType(), std::string("DerivedCInheritsDerivedB"), __LINE__);

	auto obj = noArgFac->Create("NoArgs");
	msg = "Create(\"NoArgs\")";
	PrintTestResult(msg, obj != NULL, true, __LINE__);
	PrintTestResult(msg, obj->GetType(), std::string("DerivedNoArgs"), __LINE__);
	std::cout << "TestFactoryCreate:       Passed!!!!!!!!!!!!!" <<std::endl;
	return true;
}

// ================================== Evyatar's tests ==================================

const int PASSED = 0;
const int FAILED = 1;
const int OFF = 0;
const int ON = 1;
int g_errors = 0;

namespace Color
{

const int BOLD = 1;

enum Code
{
	FG_RED      = 31,
	FG_GREEN    = 32,
	FG_BLUE     = 34,
	FG_DEFAULT  = 39,
	BG_RED      = 41,
	BG_GREEN    = 42,
	BG_BLUE     = 44,
	BG_DEFAULT  = 49,
	RESET       = 0
};

class Modifier
{
public:
	Modifier(Code code, bool bold = false);
	
	std::string GetCode() const;
	
public:
	Code m_code;
	bool m_bold;
};

std::ostream& operator<<(std::ostream& os, const Modifier& mod) {
	return os << "\033[" << mod.GetCode() << "m";
}

Modifier::Modifier(Code code, bool bold)
	: m_code(code)
	, m_bold(bold)
{
	// empty
}

std::string Modifier::GetCode() const
{
	if (m_bold && m_code < 40) // Only apply bold to foreground colors
	{
		return std::to_string(BOLD) + ";" + std::to_string(m_code);
	}
	
	return std::to_string(m_code);
}

} // Color namespace

Color::Modifier bold_red(Color::FG_RED, Color::BOLD);
Color::Modifier bold_green(Color::FG_GREEN, Color::BOLD);
Color::Modifier def(Color::RESET);

static bool IntTest(const char* function_name, int line, int func_return_val
					, int expected);


class A
{
public:
	A(int i = 10) : m_i(i) {};
	operator std::string() const { return std::to_string(m_i); }
	void Set(int num) { m_i = num; }
	int Get() { return m_i; }
	static std::shared_ptr<A> MakeSharedA() { return std::make_shared<A>(); }

private:
	int m_i;
};

class B : public A
{
public:
	B(int i = 202) : A(i) {};
	static std::shared_ptr<A> MakeSharedB() { return std::make_shared<B>(); }
};

static void TestFactory()
{
	Factory<int, A>* factory = Singleton<Factory<int, A>>::GetInstance();
	factory->Add(0, &A::MakeSharedA);
	bool failCount = IntTest("Add", __LINE__, factory->Create(0)->Get(), 10);

	factory->Add(1, &B::MakeSharedB);
	failCount |= IntTest("Add", __LINE__, factory->Create(1)->Get(), 202);
	failCount |= IntTest("Add", __LINE__, factory->Create(0)->Get(), 10);

	try
	{
		failCount |= IntTest("Add", __LINE__, factory->Create(550)->Get(), 10);
	}
	catch(const std::out_of_range& e)
	{
		// empty
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		failCount |= IntTest("Add", __LINE__, 1, 0);
	}
	

	g_errors += failCount;
}

void EvyatarsTest()
{
	TestFactory();	
	
	if (!g_errors)
	{
		std::cout << bold_green << "\n\n\tpassed Evyatar's tests!!!\n" << def << std::endl;
	}
}

static bool IntTest(const char* function_name, int line, int func_return_val
					, int expected)
{
	if (func_return_val != expected)
	{
		std::cout << bold_red << "\nTest fail" << def << " in function "
			<< bold_red << function_name << def
			<< ", line " << line << "! expected " << expected 
			<< ", but returned " << func_return_val << std::endl;

		return FAILED;
	}
	
	return PASSED;
}

// ========================== end of Evyatar's tests ===========================


// ========================== begin of Yoni's tests ============================

class Y1
{
	public:

	Y1(int a = 2):m_a(a)
	{

	}

	static std::shared_ptr<Y1> CreateY1()
	{
		return std::make_shared<Y1>();	
	}

	static std::shared_ptr<Y1> CreateY1custom()
	{
		return std::make_shared<Y1>();	
	}

	private:

	int m_a;	
};

/*
	std::function is all type of callable function.
	callble function can be:
    1) pointer function.
	2) lambda
	3) functor
	4)
*/
static void YoniTest()
{
	Factory<int, Y1>* fY1 = Singleton<Factory<int, Y1>>::GetInstance();

	fY1->Add(1, &Y1::CreateY1);
	

} 	


int main()
{
	TestFactoryCreate();
    EvyatarsTest();
	YoniTest();

	return 0;
}
