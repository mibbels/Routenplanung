#include "Logger.hpp"
#include "Datatypes.hpp"
#include "Parser.hpp"

int main()
{
    Core::Logger::Init();
    LOG(INFO) << "##### Routenplanung #####";

    Core::ui8vec o5m_RawDataVector;
    Core::Parser::ReadIn_o5m(o5m_RawDataVector, "../Res/regbez-duesseldorf-streets.o5m");
    Core::Parser::Display(o5m_RawDataVector, 7);

    return 0;
}