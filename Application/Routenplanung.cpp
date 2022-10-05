#include "Logger.hpp"
#include "Datatypes.hpp"
#include "Parser.hpp"
#include "Utility.hpp"

int main()
{
    Core::Logger::Init();
    LOG(INFO) << "##### Routenplanung #####";

    Core::o5mFile fileStatistics;
    Core::Parser::ReadIn_o5m(fileStatistics, "../Res/regbez-duesseldorf-streets-clean0.o5m");
    Core::Parser::ShowStatistics(fileStatistics);

    //A few tests for delta encoding/decoding
    auto res0 = Core::Utility::DeltaEncode_uInt32(5);
    auto res1 = Core::Utility::DeltaEncode_uInt32(127);
    auto res2 = Core::Utility::DeltaEncode_uInt32(323);
    auto res3 = Core::Utility::DeltaEncode_uInt32(1020);
    auto res4 = Core::Utility::DeltaEncode_uInt32(16384);

    //auto res5 = Core::Utility::DeltaDecode_sInt32(&res0.at(0), res0.size());
    //auto res6 = Core::Utility::DeltaDecode_sInt32(&res1.at(0), res1.size());
    //auto res7 = Core::Utility::DeltaDecode_sInt32(&res2.at(0), res2.size());
    //auto res8 = Core::Utility::DeltaDecode_sInt32(&res3.at(0), res3.size());
    //auto res9 = Core::Utility::DeltaDecode_sInt32(&res4.at(0), res4.size());

    return 0;
}