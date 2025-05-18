#include <iostream>

#include "logger.hpp"

using namespace ilrd;

int main()
{
	Logger* l = Singleton<Logger>::GetInstance();
	
	l->Log("debug", "Logger.txt", __LINE__, Logger::DEBUG);
	l->Log("info", "Logger.txt", __LINE__, Logger::INFO);
	l->Log("error", "Logger.txt", __LINE__, Logger::ERROR);

	l->SetLevel(Logger::INFO);
	l->Log("debug", "Logger.txt", __LINE__, Logger::DEBUG);
	l->Log("info", "Logger.txt", __LINE__, Logger::INFO);
	l->Log("error", "Logger.txt", __LINE__, Logger::ERROR);

	l->SetLevel(Logger::ERROR);
	l->Log("debug", "Logger.txt", __LINE__, Logger::DEBUG);
	l->Log("info", "Logger.txt", __LINE__, Logger::INFO);
	l->Log("error", "Logger.txt", __LINE__, Logger::ERROR);

	return 0;
}



