#include "Logger.hpp"
#include "Datatypes.hpp"

int main()
{
    Core::Logger::Init();
    LOG(INFO) << "##### Routenplanung #####";

    Core::i32vec myVector;
    for(int i = 0; i < 1024 * 1024; i++)
    {
        myVector.push_back(i + 2);
    }

    LOG(INFO) << myVector[99];

    return 0;
}