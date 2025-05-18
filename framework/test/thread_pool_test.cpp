#include <iostream> // for prints
#include <future> // for future
#include <chrono> // for chrono time

#include "thread_pool.hpp"

#define RED     "\033[0;31m"
#define BRED    "\033[1;31m"
#define BGRN    "\033[1;32m"
#define RESET   "\033[0m"

static const size_t g_numOfComands = 90;
static const size_t g_numOfTreads = (g_numOfComands / 2) + 1;
static const size_t g_sleep_time_in_mil = (g_numOfComands / g_numOfTreads) + 1;
static size_t g_tests_failed = 0;

using namespace ilrd;
std::queue<std::string> g_file;
std::queue<std::string> g_compareToFile;

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
void PrintTestResult(std::string fail_message, comparable actual, comparable expected, int line_num)
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

static bool Writer(std::string& str)
{
	g_file.push(str);
	return true;
}

static bool Reader()
{
	g_compareToFile.push(g_file.front());
	g_file.pop();
	return true;
}

bool TestCreate()
{
	{
		ThreadPool* pool = Singleton<ThreadPool>::GetInstance();
	}
	for (size_t i = 1; i < g_numOfTreads; ++i)
	{
		{
			ThreadPool* pool = Singleton<ThreadPool>::GetInstance();
		}
	}
	
	return true;
}
class WriterTask : public ThreadPool::ITask
{
private:
	bool (*m_func)(std::string&);
	std::string& m_str;
public:
	WriterTask(bool (*func)(std::string&), std::string& str) : m_func(func), 	
																	m_str(str){}
	~WriterTask(){}
	virtual void operator()()
	{
		m_func(m_str);
	}
};

bool PushWriters(ThreadPool* pool)
{
	for (size_t i = 0; i < g_numOfComands; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(g_sleep_time_in_mil));
		std::string str(std::to_string(i));
		std::shared_ptr<WriterTask> ptr(new WriterTask(Writer, str));
		pool->Add(ptr);
	}
	return true;
}
class ReaderTask : public ThreadPool::ITask
{
public:
	ReaderTask(bool (*func)(void)) : m_func(func) {}
	void operator()(){m_func();}
private:
	bool (*m_func)(void);
};

bool PushReaders(ThreadPool* pool)
{
	for (size_t i = 0; i < g_numOfComands; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(g_sleep_time_in_mil));
		std::string str(std::to_string(i));
		std::shared_ptr<ThreadPool::ITask> ptr(new ReaderTask(Reader));
		pool->Add(ptr);
	}
	return true;
}

bool CompareResults(std::string& func)
{
	
	for (size_t i = 0; i < g_numOfComands; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(g_sleep_time_in_mil));
		if (g_compareToFile.empty())
		{
			return true;
		}
		PrintTestResult(func, g_compareToFile.front(), std::to_string(i), __LINE__);
		g_compareToFile.pop();
	}
	return 0 == g_tests_failed;
}

bool TestAdd()
{
	std::string fail_message("TestAdd");
	ThreadPool* pool = Singleton<ThreadPool>::GetInstance();
	if (PushWriters(pool) && PushReaders(pool) && CompareResults(fail_message))
	{
		std::cout << "TestAdd:                   Passed!!!!!!!!!!!!!" <<std::endl;
		return true;
	}
	return false;
}

static bool TestStop()
{
	std::string fail_message("TestStop");
	ThreadPool* pool = Singleton<ThreadPool>::GetInstance();
	if (PushWriters(pool) && PushReaders(pool) && CompareResults(fail_message))
	{
		size_t size = g_compareToFile.size();
		pool->Stop();
		std::this_thread::sleep_for(std::chrono::milliseconds(3 * g_sleep_time_in_mil));
		if (PushWriters(pool) && PushReaders(pool) && CompareResults			
							(fail_message) && (g_compareToFile.size() != size))
		{
			return false;
		}
		return true;
	}
	return false;
}

static bool TestSimplePuseAndResume()
{
	std::string fail_message("TestSimplePuseAndResume");
	ThreadPool* pool = Singleton<ThreadPool>::GetInstance();
	if (PushWriters(pool) && PushReaders(pool) && CompareResults(fail_message))
	{
		std::this_thread::sleep_for(std::chrono::seconds(5 * g_sleep_time_in_mil));
		
	// 	PushWriters(pool);
		
	// 	pool.Pause();
	// std::cout << "TestSimplePuseAndResume:   failed" <<std::endl;
		
	// 	std::this_thread::sleep_for(std::chrono::seconds(g_sleep_time_in_mil));
	// 	PushReaders(pool);
	// 	std::this_thread::sleep_for(std::chrono::seconds(g_sleep_time_in_mil));
	// 	if (!g_compareToFile.empty())
	// 	{
	// 		put(RED);
	// 		std::cout << "TestSimplePuseAndResume:   failed" <<std::endl;
	// 		put(RESET);
	// 		return false;
	// 	}
	// 	// std::this_thread::sleep_for(std::chrono::seconds(1));
	// 	pool.Resume();
	// 	std::this_thread::sleep_for(std::chrono::seconds( 3 * g_sleep_time_in_mil));
	// 	if (g_compareToFile.empty() || (!CompareResults(fail_message)))
	// 	{
	// 		put(RED);
	// 		std::cout << "TestSimplePuseAndResume:   failed" <<std::endl;
	// 		put(RESET);
	// 		return false;
	// 	}
	// 	std::cout << "TestSimplePuseAndResume:   Passed!!!!!!!!!!!!!" <<std::endl;
	}
	return true;
}

static bool TestPuseAndResume()
{
	if(TestSimplePuseAndResume())
	{
		std::cout << "TestPuseAndResume:         Passed!!!!!!!!!!!!!" <<std::endl;
		return true;
	}
	return false;
}

static bool TestIncreaseThreads(ThreadPool* pool)
{
	pool->SetNumThread(2 * g_numOfTreads);
	return true;
}

static bool TestDecreaseThreads(ThreadPool* pool)
{
	pool->SetNumThread(g_numOfComands);
	return true;
}

static bool TestSetNumOfThreads()
{
	std::string fail_message("TestSetNumOfThreads");
	ThreadPool* pool = Singleton<ThreadPool>::GetInstance();
	if (PushWriters(pool) && PushReaders(pool) && CompareResults(fail_message))
	{}
	TestIncreaseThreads(pool);
	if (PushWriters(pool) && PushReaders(pool) && CompareResults(fail_message))
	{}
	TestDecreaseThreads(pool);
	if (PushWriters(pool) && PushReaders(pool) && CompareResults(fail_message))
	{}
	std::cout << "TestSetNumOfThreads:       Passed!!!!!!!!!!!!!" <<std::endl;
	return true;
}

void ShimonsTest()
{
	if(TestCreate() && TestAdd() && TestStop() && TestPuseAndResume()  && 
									TestSetNumOfThreads()  && (!g_tests_failed))
	{
		put(BGRN);
		std::cout << "        Passed All Shimons Test!!!!!" << std::endl;
		put(RESET);
		return;
	}
	std::cout << "Num Of Tests Failed: " << g_tests_failed <<std::endl;
}

const size_t g_numTestLoops = 50;

std::mutex g_printSync; //### Remove this later
void ContextPrint(std::string str, std::string context = "", int line = 0)
{
    std::lock_guard lock(g_printSync);

    if (0 != line)
    {
        std::cout << "line:" << line;
    }
    if ("" != context)
    {
        std::cout << " (" << context << ") ";
    }

    std::cout << str << std::endl;
}

void CountTo(size_t limit)
{
    size_t i = 0;
    for(; i < limit; ++i)
    {
    }
}

class CountTask: public ThreadPool::ITask
{
public:
    explicit CountTask(size_t limit, bool* wasInvoked);
    void operator()();
private:
    size_t m_limit;
    bool* m_wasInvoked;
};

CountTask::CountTask(size_t limit, bool* wasInvoked)
: m_limit(limit), m_wasInvoked(wasInvoked)
{
}

void CountTask::operator()()
{
    *m_wasInvoked = true;

    CountTo(m_limit);
}

int TestAddAfterStop(size_t numThreads)
{
    int faultCount = 0;
    size_t i = 0;
    std::string context = "FAIL: add after stop";

    while(i++ < g_numTestLoops)
    {
        bool wi1 = false;
        bool wi2 = false;
        bool wi3 = false;
        bool wi4 = false;

        {
           	ThreadPool* tp = Singleton<ThreadPool>::GetInstance();

            tp->Add(std::make_shared<CountTask>(9999999, &wi1));
            tp->Add(std::make_shared<CountTask>(1000000, &wi2));

            tp->Stop();

            tp->Add(std::make_shared<CountTask>(1, &wi3), 
            ThreadPool::Priority::HIGH);
            tp->Add(std::make_shared<CountTask>(2, &wi4), 
            ThreadPool::Priority::LOW);
			std::this_thread::sleep_for(std::chrono::milliseconds(g_sleep_time_in_mil));
        }
		
        if ((!wi1) || (!wi2))
        {
            ContextPrint("Tasks added before Stop() were not invoked", context, 
            __LINE__);
            
            ++faultCount;
        }
        if (wi3 || wi4)
        {
            ContextPrint("Tasks added after Stop() were executed", context, 
            __LINE__);

            ++faultCount;
        }

        if (0 < faultCount)
        {
            break;
        }
    }

    return faultCount;
}

std::mutex g_sync; //### temp - remove

class PriorityCheck: public ThreadPool::ITask
{
public:
    PriorityCheck(size_t* sharedCounter, size_t* orderOfArrival, 
    std::mutex* syncCounter);
    void operator()();
private:
    size_t* m_sharedCounter;
    size_t* m_orderOfArrival;
    std::mutex* m_syncCounter;
};

PriorityCheck::PriorityCheck(size_t* sharedCounter, size_t* orderOfArrival, 
std::mutex* syncCounter)
: m_sharedCounter(sharedCounter), m_orderOfArrival(orderOfArrival), 
m_syncCounter(syncCounter)
{   
}

void PriorityCheck::operator()()
{
    std::lock_guard lock(*m_syncCounter);
	
    ++(*m_sharedCounter);
    *m_orderOfArrival = *m_sharedCounter;
}

int TestPriority(void)
{
    int faultCount = 0;
    std::string context = "Fail: priority";
    size_t i = 0;

    while (i++ < g_numTestLoops)
    {
        size_t counter = 0;
        size_t one = 0;
        size_t two = 0;
        size_t three = 0;
        
        {
            ThreadPool* tp = Singleton<ThreadPool>::GetInstance();
            std::mutex sync;

            tp->Pause();

            tp->Add(std::make_shared<PriorityCheck>(&counter, &three, &sync), 
            ThreadPool::Priority::LOW);
            tp->Add(std::make_shared<PriorityCheck>(&counter, &two, &sync), 
            ThreadPool::Priority::MID);
            tp->Add(std::make_shared<PriorityCheck>(&counter, &one, &sync), 
            ThreadPool::Priority::HIGH);

            tp->Resume();

            tp->Stop(); // make sure tasks finish
			    std::this_thread::sleep_for(std::chrono::milliseconds(g_sleep_time_in_mil));

        }

        if ((1 != one) || (2 != two) || (3 != three))
        {
            std::cout << "one = " << one << ", two = " << two << ", three = " << three << std::endl;

            ContextPrint("low priority executed before high priority", context);
            
            ++faultCount;

            break;
        }
    }

    return faultCount;
}

void TestPauseBeforDtor(void)
{
    std::future<int> res = std::async(std::launch::async, []()
    {
       	ThreadPool* tp = Singleton<ThreadPool>::GetInstance();

        tp->Pause();

        return 1;
    });

    if (std::future_status::timeout == 
    res.wait_for(std::chrono::milliseconds(10)))
    {
        ContextPrint("dedededeDeadlockkkkk", "Fail: Dtor() after Pause()");
    }
}

void TestThreadPool()
{
    int faultCount = 0;
    size_t numThreads = 16;

    faultCount += TestAddAfterStop(numThreads);
    faultCount += TestPriority();
    
    //TestPauseBeforDtor(); // test for deadlock

    if (0 == faultCount)
    {
        ContextPrint("I got a feeling\nThat tonight-'s gonna be a good night\n\
That tonight-'s gonna be a good good night");
    }
}

int main()
{
	std::cout << "Pleas start by writing a simple test first!!!!" << std::endl;	
	//ShimonsTest();
	TestThreadPool();
	return 0;
}
