#include <memory>
#include <iostream>

#include "dispatcher.hpp"

using namespace ilrd;

class Call1: public ICallback<int>
{
public:
    Call1() : m_a(++counter) {}
    void Update(const int* message) {m_a += *message;}
    int Get() const {return m_a;}

private:
    int m_a;
    static int counter;
};

int Call1::counter = 0;

class Call2: public ICallback<int>
{
public:
    Call2() : m_a(++counter) {}
    void Update(const int* message) {m_a += *message;}
    int Get() const {return m_a;}

private:
    int m_a;
    static int counter;
};
int Call2::counter = 99;

class Call3: public ICallback<int>
{
public:
    Call3() 
	: m_a(++counter) 
	{
		
	}
    void Update(const int* message) {m_a += *message;}
    int Get() const 
	{
		return m_a;
	}

private:
    int m_a;
    static int counter;
};
int Call3::counter = 999;

bool BasicTest()
{
    Dispatcher<int> dispatcher; 
    Call1 c1;
    dispatcher.Attach(&c1);
    if(c1.Get() != 1)
    {
        return false;
    }
    int tmp1 = 1;
    for (int i = 0; i < 100; ++i)
    {
        dispatcher.Notify(&i);
        if(c1.Get() != (tmp1 + i))
        {
            return false;
        }
        tmp1 = c1.Get();
    }

    Call2 c2;
    dispatcher.Attach(&c2);
    if(c2.Get() != 100)
    {
        return false;
    }

    int tmp2 = 100;
    for (int i = 0; i < 100; ++i)
    {
        dispatcher.Notify(&i);
        if(c1.Get() != (tmp1 + i))
        {
            return false;
        }
        tmp1 = c1.Get();

        if(c2.Get() != (tmp2 + i))
        {
            return false;
        }
        tmp2 = c2.Get();
    }


    Call3 c3;
    dispatcher.Attach(&c3);
    if(c3.Get() != 1000)
    {
        return false;
    }

    int tmp3 = 1000;
    for (int i = 0; i < 100; ++i)
    {
        dispatcher.Notify(&i);
        if(c1.Get() != (tmp1 + i))
        {
            return false;
        }
        tmp1 = c1.Get();

        if(c2.Get() != (tmp2 + i))
        {
            return false;
        }
        tmp2 = c2.Get();

        if(c3.Get() != (tmp3 + i))
        {
            return false;
        }
        tmp3 = c3.Get();
    }

    c1.Unsubscribe();

    for (int i = 0; i < 100; ++i)
    {
        dispatcher.Notify(&i);
        if(c1.Get() != (tmp1))
        {
            return false;
        }
        tmp1 = c1.Get();

        if(c2.Get() != (tmp2 + i))
        {
            return false;
        }
        tmp2 = c2.Get();

        if(c3.Get() != (tmp3 + i))
        {
            return false;
        }
        tmp3 = c3.Get();
    }

    c2.Unsubscribe();

    for (int i = 0; i < 100; ++i)
    {
        dispatcher.Notify(&i);
        if(c1.Get() != (tmp1))
        {
            return false;
        }
        tmp1 = c1.Get();

        if(c2.Get() != tmp2)
        {
            return false;
        }
        tmp2 = c2.Get();

        if(c3.Get() != (tmp3 + i))
        {
            return false;
        }
        tmp3 = c3.Get();
    }

    c3.Unsubscribe();

    for (int i = 0; i < 100; ++i)
    {
        dispatcher.Notify(&i);
        if(c1.Get() != (tmp1))
        {
            return false;
        }
        tmp1 = c1.Get();

        if(c2.Get() != tmp2)
        {
            return false;
        }
        tmp2 = c2.Get();

        if(c3.Get() != (tmp3))
        {
            return false;
        }
        tmp3 = c3.Get();
    }

    return true;
}

int main()
{
    if(BasicTest())
	{
		std::cout << "\n\tPass All Tests" << std::endl;
	}

    return 0;
}
