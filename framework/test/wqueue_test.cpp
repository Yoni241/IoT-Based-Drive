#include <iostream>     // for prints
#include <queue>        // for queue, deque
#include <algorithm>    // for for_each
#include <thread>       // for threads
#include <atomic>       // for atomic

#include "wqueue.hpp"
#include "wpqueue.hpp"

using namespace ilrd;

int TestFunctional()
{
    int fault_count = 0;

    int data[5] = {1, 2, 3, 4, 5};
    
    WQueue<int, std::deque<int> > q;
    
    std::for_each(data, data + 5, [&q](int val) { q.Push(val); });

    int dest[5];

    for (size_t i = 0; !q.IsEmpty(); ++i)
    {
        int val;
        q.Pop(&val);
        dest[i] = val;
    }

    for(size_t i = 0; i < 5; ++i)
    {
        if (dest[i] != data[i])
        {
            ++fault_count;

            std::cout << "Push/Pop failed" << std::endl;
            
            break;
        }
    }

    return fault_count;
}

int TestTimeoutPop()
{
    int fault_count = 0;

    std::chrono::milliseconds timeout(2000);

    WQueue<int, std::deque<int>> q;
    
    int tmp;
    
    if (wqueue_status::TIME_OUT_EMPTY != q.Pop(&tmp, timeout))
    {
        ++fault_count;

        std::cout << "timeout pop failed - expected timeout empty queue" 
        << std::endl;
    }

    q.Push(1);
    if (wqueue_status::SUCCESS != q.Pop(&tmp, timeout))
    {
        ++fault_count;

        std::cout << "timeout pop failed - expected success" 
        << std::endl;
    }

    return fault_count;
}

template<class Container>
void Reader(WQueue<size_t, Container>* q, size_t* out_sum)
{
    size_t val;
    size_t grace = 5;
    size_t timeout_mili = 100;

    while (0 != grace)
    {
        if (wqueue_status::SUCCESS != q->Pop(&val, 
        std::chrono::milliseconds(timeout_mili)))
        {
            --grace;
        }
        else
        {
            *out_sum += val;
        }
    }
}

template<class Container>
void Writer(WQueue<size_t, Container>* q, size_t num)
{
    while (0 != num)
    {
        q->Push(num--);
    }
}

template<class Container>
int TestThreads()
{
    int fault_count = 0;

    WQueue<size_t, Container> q;
    
    size_t num = 1000;
    size_t num_half = num / 2;
    size_t partial_sum_t1 = 0;
    size_t partial_sum_t2 = 0;

    std::thread t1(Reader<Container>, &q, &partial_sum_t1);
    std::thread t2(Reader<Container>, &q, &partial_sum_t2);

    std::thread t3(Writer<Container>, &q, num_half);
    std::thread t4(Writer<Container>, &q, num_half);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    size_t expected_partial_sum = ((num_half) * (1 + num_half)); // N * (a1 + aN) / 2
    size_t res = partial_sum_t1 + partial_sum_t2;

    if (expected_partial_sum != res)
    {
        ++fault_count;

        std::cout << "test threads failed, expected res:" << 
        expected_partial_sum << ", but got :" << res << std::endl;
    }

    return fault_count;
}

void TestWQueue()
{
    int fault_count = 0;

    fault_count += TestFunctional();

    fault_count += TestTimeoutPop();

    fault_count += TestThreads<std::deque<size_t> >();

    fault_count += TestThreads<WPQueue<size_t> >();

    if (0 == fault_count)
    {
        std::cout << "Wow, ClAp clap, smiley face! nice job" << std::endl;
    }
}

int main(void)
{
    TestWQueue();

    return 0;
}
