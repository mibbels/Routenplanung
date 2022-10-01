#include "Logger.hpp"
#include "Datatypes.hpp"
#include "Parser.hpp"

int main()
{
    Core::Logger::Init();
    LOG(INFO) << "##### Routenplanung #####";

    Core::ui8vec o5m_RawDataVector;
    Core::Parser::ReadIn_o5m(o5m_RawDataVector, "../Res/regbez-duesseldorf-streets.o5m");
    Core::Parser::Display_ui8vec(o5m_RawDataVector, 7);
    Core::Parser::ShowStatistics(o5m_RawDataVector);

    return 0;
}