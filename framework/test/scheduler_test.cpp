//Developer: Shimon Schwartz
// File: 	   scheduler_test.cpp
// Reviewer:    
// Date: 	   26 december 2024
// Last Update :      29 december 2024 22:18

#include <thread> // for threads
#include <iostream> // for prints

#include "scheduler.hpp"

//__________________________ Shimons tests___________________________________
// TODO      UPDATED ON: 29 dec 22:18

#define RESET   "\033[0m"
#define RED     "\033[0;31m"
#define BRED    "\033[1;31m"
#define GREEN   "\033[1;32m"
#define BGREEN  "\033[1;32m"
#define YELLOW  "\033[0;33m"
#define BYELLOW "\033[1;33m"
#define BLU     "\033[0;34m"
#define BBLU    "\033[1;34m"
#define PURPLE  "\033[0;35m"
#define BPURPLE  "\033[1;35m"
#define GRAY  "\033[0;2m"
#define BGRAY "\033[1;2m"




// TODO README: U can torn on/off the tests in the array as described here.
// TODO 										and adjust num Of Tasks below
static const bool tests[6] =
{
/* testCreate()					   	 		  */ true,
/* TestAddReadersBeforeWriters() 		 	  */ true,
/* TestAddReadersBeforeWriters()			  */ true,
/* PushTasksThroughATask() 					  */ true,
/* NoamTests();					  			  */ true,
/* Get printed infomation for Tests you Passed*/ false

};

static const size_t g_numOfComands = 34; // todo adjust num Of Tasks
// todo                              (and length sleep time as as a result)

static const size_t g_sleep_time_in_mil = 10;
static size_t g_tests_failed = 0;
std::mutex g_mutex;

std::thread* g_testingMSGTread;
bool g_IsTesting;

std::queue<std::string> g_file;
std::queue<std::string> g_compareToFile;

//-------------------- utilities -----------------------------



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
    else if (tests[5])
    {
        put(BGRAY);
		put("Test Passed on line - ");  Put(line_num);
		put("Success message: "); 		Put(fail_message);
		put("actual value: "); 			Put(actual);
		put("expected value: "); 		Put(expected);
		put(RESET);
    }
}

static void PrintHeaderMSG()
{
	put(PURPLE);
	std::cout << "\n\nPleas start by writing at least a simple test yourself pleas!!!!\n" << std::endl;
	put(BLU);
	std::cout << "\n\nBefore running The tests pleas add your one Tests\nto this file by changing your main() to a function\nwith yore name and adding a call to it from the main()\n\n" << std::endl;
	put(BBLU);
	std::cout << "After doing so, Read the instructions in the head of this file.\n\n" << std::endl;
	put(RESET);
}


static void TestingMSGTreadFunc()
{
	while (g_IsTesting && 0 == g_tests_failed)
	{
		put(BGRAY);
		std::cout << "\n\n Still Testing Pleas be patient....!!!!\n" << std::endl;
		put(RESET);
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
}

static bool Writer(std::string str)
{
	g_IsTesting = true;
	std::lock_guard<std::mutex> lock(g_mutex);
    size_t size = g_file.size();
	g_file.push(str);
	return g_file.size() == size + 1 && g_tests_failed == 0;
}

static bool Reader()
{
	g_IsTesting = true;
	std::lock_guard<std::mutex> lock(g_mutex);
	size_t file_size = g_file.size();
	size_t compareToFile_size = g_compareToFile.size();
	g_compareToFile.push(g_file.front());
	g_file.pop();
	return g_file.size() == file_size - 1
	&&
	g_compareToFile.size() == compareToFile_size + 1
	&& 
	g_tests_failed == 0;
}

using namespace ilrd;

bool TestCreate()
{
	g_IsTesting = true;
	Scheduler* sched = Singleton<Scheduler>::GetInstance();
	return NULL != sched && g_tests_failed == 0;
}

class WriterTask : public Scheduler::ISchedTask
{
private:
	bool (*m_func)(std::string);
	std::string m_str;
public:
	WriterTask(bool (*func)(std::string), std::string& str) : m_func(func), 	
																	m_str(str){}
	~WriterTask(){}
	virtual void Execute()
	{
		g_IsTesting = true;
		g_tests_failed += !m_func(m_str);
	}
};

bool PushWriters(Scheduler& sched, const size_t executeDelay)
{
	g_IsTesting = true;
	std::string fail_message("TestAddsWritersBeforeReaders");
	for (size_t i = 1; i < g_numOfComands + 1; i++)
	{
		std::string str(std::to_string(i));
		std::shared_ptr<WriterTask> ptr(new WriterTask(Writer, str));
		sched.AddTask(ptr, std::chrono::milliseconds(executeDelay));
		
	}
	return g_tests_failed == 0;
}

class ReaderTask : public Scheduler::ISchedTask
{
public:
	ReaderTask(bool (*func)(void)) : m_func(func) {}
	virtual void Execute(){g_IsTesting = true; g_tests_failed += !m_func();}
private:
	bool (*m_func)(void);
};

bool PushReaders(Scheduler& sched, const size_t executeDelay)
{
	g_IsTesting = true;
	for (size_t i = 1; i < g_numOfComands + 1; i++)
	{
		std::string str(std::to_string(i));
		std::shared_ptr<Scheduler::ISchedTask> ptr(new ReaderTask(Reader));
		sched.AddTask(ptr, std::chrono::milliseconds(3  +  (executeDelay * (g_numOfComands))));
	}
	return g_tests_failed == 0;
}

bool CompareResults(std::string& func, const size_t executeDelay)
{
	g_IsTesting = true;
	while(g_compareToFile.size() < g_numOfComands){}
	std::this_thread::sleep_for(std::chrono::milliseconds((executeDelay * (g_numOfComands + 1))));
	while(g_compareToFile.size() < g_numOfComands){}
	for (size_t i = 1 ; i < g_numOfComands + 1; i++)
	{
		PrintTestResult(func, g_compareToFile.front(), std::to_string(i), __LINE__);
		g_compareToFile.pop();
	}
	g_IsTesting = false;
	return 0 == g_tests_failed;
}

bool TestAddsWritersBeforeReaders(Scheduler& sched)
{
	g_IsTesting = true;
	std::string fail_message("TestAddsWritersBeforeReaders");
	if (PushWriters(sched, 1) && PushReaders(sched, 2) && CompareResults(fail_message, 9))
	{
		std::cout << "TestAddsWritersBeforeReaders: Passed!!!!!!!!!!!!!" <<std::endl;
		return g_tests_failed == 0;
	}

	Put(BRED);
	std::cout << "TestAddsWritersBeforeReaders: Failed" <<std::endl;
	Put(RESET);
	return g_tests_failed == 0;
}

bool TestAddReadersBeforeWriters(Scheduler& sched)
{
	g_IsTesting = true;
    std::string fail_message("TestAddReadersBeforeWriters");
	if (PushReaders(sched, 3) && PushWriters(sched, 1) &&  CompareResults(fail_message,  9))
	{
		std::cout << "TestAddReadersBeforeWriters: Passed!!!!!!!!!!!!!" <<std::endl;
		return g_tests_failed == 0;
	}
	Put(BRED);
	std::cout << "TestAddReadersBeforeWriters: Failed" <<std::endl;
	Put(RESET);
	return g_tests_failed == 0;
}

class TasksThroughATask : public Scheduler::ISchedTask
{
private:
	Scheduler& m_sched;
	bool (*m_func)(Scheduler& sched);
public:
	TasksThroughATask(bool (*func)(Scheduler& sched), Scheduler& sched) : m_sched(sched), m_func(func) {}
	~TasksThroughATask(){}
	virtual void Execute()
	{
		g_IsTesting = true;
		g_tests_failed += !m_func(m_sched);
	}
};

static bool PushTasks(Scheduler& sched)
{
	std::string fail_message("TestAddReadersBeforeWriters");
	if (PushReaders(sched, 3) && PushWriters(sched, 1)){}
	return g_tests_failed == 0;
}
static bool PushTasksThroughATask(Scheduler& sched)
{
	g_IsTesting = true;
	std::string fail_message("PushTasksThroughATask(TestAddReadersBeforeWriters())");
	std::shared_ptr<Scheduler::ISchedTask> ptr(new TasksThroughATask(PushTasks, sched));
	sched.AddTask(ptr, std::chrono::milliseconds(1));
	if (CompareResults(fail_message,  9))
	{
		while (g_IsTesting){if(g_tests_failed){return false;}}
		std::cout << "PushTasksThroughATask: Passed!!!!!!!!!!!!!" << std::endl;
	}
	return g_tests_failed == 0;
}

void ShimonsTest()
{
	put(YELLOW);
	std::cout << "Starting Shimons Tests... Pleas wait patiently..." << std::endl;
	put(RESET);
	g_IsTesting = true;
	std::thread thread(TestingMSGTreadFunc);
	Scheduler* sched = Singleton<Scheduler>::GetInstance();
	if((!tests[0] || TestCreate())
	&&
	(!tests[1] || TestAddsWritersBeforeReaders(*sched)) && (!tests[2] || TestAddReadersBeforeWriters(*sched)) && (!tests[3] || PushTasksThroughATask(*sched)) && (!g_tests_failed))
	{
		put(BGREEN);
		std::cout << "        Passed All Shimons Test!!!!!" << std::endl;
		put(RESET);
		g_IsTesting = false;
		thread.join();
		return;
	}
	g_IsTesting = false;
	thread.join();
	put(BRED);
	std::cout << "Num Of Shimons Tests Failed: " << g_tests_failed <<std::endl;
	put(RESET);
}
// ------------------------ Noams Tests -------------------------------------- 
class TestTask : public Scheduler::ISchedTask
{
public:
    TestTask(int n, int delay) : m_n(n), m_delay(delay){} 
    void Execute(){std::cout << "executed taskNum: " <<  m_n << ", with delay: " << m_delay << ", at time: " << std:: chrono::system_clock::now()  + std::chrono::hours{2}<< std::endl;}
private:
    int m_n;
	std::chrono::milliseconds m_delay;
};

void ThreadFunc(Scheduler *sched)
{
    std::shared_ptr<TestTask> a = std::make_shared<TestTask>(3, 2000);
    sched->AddTask(a, std::chrono::milliseconds(2000));
    std::shared_ptr<TestTask> b = std::make_shared<TestTask>(1, 10);
    sched->AddTask(b, std::chrono::milliseconds(10));
    std::shared_ptr<TestTask> c = std::make_shared<TestTask>(2, 1000);
    sched->AddTask(c, std::chrono::milliseconds(1000));
}

int NoamTests()
{
	put(YELLOW);
	std::cout << "Starting Noams Tests... Pleas wait patiently..." << std::endl;
	put(RESET);
    Scheduler* sched = Singleton<Scheduler>::GetInstance();
    std::shared_ptr<TestTask> a = std::make_shared<TestTask>(4, 4000);
    sched->AddTask(a, std::chrono::milliseconds(5000));
    std::thread t1(ThreadFunc, sched);
    t1.join();
	std::shared_ptr<TestTask> b = std::make_shared<TestTask>(5, 5000);
    sched->AddTask(b, std::chrono::milliseconds(6000));
    sleep(11);    
    return 0;
}

int main()
{
	PrintHeaderMSG();
	ShimonsTest();
	if (!tests[4] || !NoamTests())
	{
		put(YELLOW);
		std::cout << "For Noams Tests: Check if the above Tasks are executed by the right  order {1, 2,..., 5}" << std::endl;
		put(RESET);
	}
	return 0;
}