#include "Logger.hpp"
#include "Datatypes.hpp"
#include "Parser.hpp"

int main()
{
    Core::Logger::Init();
    LOG(INFO) << "##### Routenplanung #####";

    Core::o5mFile fileStatistics;
    Core::Parser::ReadIn_o5m(fileStatistics, "../Res/regbez-duesseldorf-streets-clean0.o5m");
    Core::Parser::ShowStatistics(fileStatistics);

    return 0;
}