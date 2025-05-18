#include "framework.hpp"
#include "key_args.hpp"  // wrapper for cmd data 

using namespace ilrd;

                                                                                /*
________________________________________________________________________________

                            minion example 
________________________________________________________________________________ 
                                                                                */
const size_t SIZE = 1000;
char buffer[SIZE] = "hey its minion nice meeting you";

                                                                                /*
________________________________________________________________________________
                                                                                */                                                                               


bool ai_func()
{
    static int counter = 5;



    std::cout << "shira" << counter << std::endl;
    if (counter--)
    {
        return false;
    }

    counter = 5;


    return true; 
}

class ReadCommandArgs: public IKeyArgs
{
    public:
        ReadCommandArgs(size_t num, size_t offset)
        :m_numBytesToRead(num), m_offset(offset)
        {
            // empty for now
        }

        uint64_t GetKey() const
        {
            return 1;
        }

        size_t GetOffset() const
        {
            return m_offset;
        }

        size_t GetNumOfBytesToRead() const
        {
            return m_numBytesToRead;
        }

    private:
        size_t m_numBytesToRead;
        size_t m_offset;
};

class ReadCommand: public ICmd
{
    std::pair<std::function<bool()>, const std::chrono::milliseconds>
                                   Execute(std::shared_ptr<IKeyArgs> keyArgsPtr) override
    {
        // Cast keyArgsPtr to ReadCommandArgs
        std::shared_ptr<ReadCommandArgs> data = 
            std::dynamic_pointer_cast<ReadCommandArgs>(keyArgsPtr);

        // Ensure the cast succeeded
        if (!data)
        {
            throw std::invalid_argument("Invalid keyArgsPtr passed to ReadCommand.");
        }

        size_t offset = data->GetOffset();

        // Read input from the user
        char buffer1[SIZE] = {'\0'};
        std::cin >> buffer1;
        std::cout << "User Input: " << buffer1 << std::endl;

        // Display buffer contents starting from the offset
        std::cout << "Buffer Contents: ";
        for (size_t i = offset; i < SIZE && buffer[i] != '\0'; ++i)
        {
            std::cout << buffer[i];
        }
        std::cout << std::endl;

        return {ai_func, std::chrono::milliseconds(10)};
    }
};

class CmdProxy: public IInputProxy
{
    public:

        std::shared_ptr<IKeyArgs> GetKeyArgs(int fd, Reactor::Mode mode)
        {
            (void)(fd);
            (void)(mode);

            size_t numOfBytesToRead = 10;
            size_t offset = 10;

            return std::make_shared<ReadCommandArgs>(numOfBytesToRead, offset);
        }
};

std::shared_ptr<ICmd> CmdCreator()
{
    return std::make_shared<ReadCommand>();
}

int main()
{
    std::vector<Framework::FdCallbacks> callbacks;
    callbacks.push_back({0, Reactor::READ, std::make_shared<CmdProxy>()});

    std::vector<Framework::CmdCreators> creators;
    creators.push_back({1, CmdCreator});

    Framework f(callbacks, creators, "./");
    f.RunReactor();

    return 0;
}
