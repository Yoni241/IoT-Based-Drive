#include <iostream>
#include <string>
#include <chrono> // for chrono time

#include "dir_monitor.hpp"
#include "dll_loader.hpp"


// TODO      UPDATED ON: 17 dec 14:33
#define RED     "\033[0;31m"
#define BRED    "\033[1;31m"
#define BGRN    "\033[1;32m"
#define RESET   "\033[0m"

// TODO README: U can torn on/off the tests in the array as described here.
static const bool tests[7] =
{
/* testCreateDirMonitor()*/         true,
/* TestDllMonitor()    	 */			true,
/* TestStop()   	     */			true,
/* TestPuseAndResume()   */		    true,
/* TestSetNumOfThreads   */         true,
/* Brooooolys tests()    */         true,
/* TestFuture() */                  true
};

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
bool PrintTestResult(std::string& fail_message, comparable actual, comparable expected, int line_num)
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
	return 0 == g_tests_failed;
}

using namespace ilrd;

static  DirMonitor* g_monitor = new DirMonitor("plugins");
static DllLoader* g_dllLoader = new DllLoader(g_monitor, [] (){std::cout << "Error loading shared object"  << std::endl;});

void func(void)
{
	std::cout << "Error loading shared object" << std::endl;
}

bool TestCreateDllLoader()
{
	DirMonitor monitor("plugins");
	DllLoader loader(&monitor, func);
	std::string msg("TestCreateDllLoader()");
	std::cout << msg << std::endl;
	return true;
}

static bool TestDllMonitor()
{
	std::string msg("TestDllMonitor()");
	std::cout << msg << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(30));
	return true;
}

static void ShimonsTests()
{
	if((!tests[0] || TestCreateDllLoader())
	&& 
	(!tests[1] || TestDllMonitor())
	/*&&
	(!tests[2] || TestStop())
	&&
	(!tests[3] ||TestPuseAndResume()) 
	&& 
	(!tests[4] || TestSetNumOfThreads())
	&&
	(!tests[5] || TestFuture()) */
	&&
	(!g_tests_failed))
	{
		put(BGRN);
		std::cout << "        Passed All Shimons Test!!!!!" << std::endl;
		put(RESET);
		return;
	}
	std::cout << "Num Of Tests Failed: " << g_tests_failed <<std::endl;
}  



int main()
{
	std::cout << "Pleas start by writing a simple test first!!!!" << std::endl;
	ShimonsTests();
	return 0;
}

