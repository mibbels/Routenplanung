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
    auto res0 = Core::Utility::DeltaEncode_uInt32(5);      // => 0x05
    auto res1 = Core::Utility::DeltaEncode_uInt32(127);    // => 0x7f
    auto res2 = Core::Utility::DeltaEncode_uInt32(323);    // => 0xc3 0x02
    auto res3 = Core::Utility::DeltaEncode_uInt32(1020);   // => 0xfc 0x07
    auto res4 = Core::Utility::DeltaEncode_uInt32(16384);  // => 0x80 0x80 0x01
    auto res5 = Core::Utility::DeltaEncode_uInt32(65535);  // => 0xff 0xff 0x03
    auto res6 = Core::Utility::DeltaEncode_uInt32(125799); // => 0xe7 0xd6 0x07

    auto res7 = Core::Utility::DeltaDecode_uInt32(&res0.at(0), res0.size());  // 5
    auto res8 = Core::Utility::DeltaDecode_uInt32(&res1.at(0), res1.size());  // 127
    auto res9 = Core::Utility::DeltaDecode_uInt32(&res2.at(0), res2.size());  // 323
    auto res10 = Core::Utility::DeltaDecode_uInt32(&res3.at(0), res3.size()); // 1020
    auto res11 = Core::Utility::DeltaDecode_uInt32(&res4.at(0), res4.size()); // 16384
    auto res12 = Core::Utility::DeltaDecode_uInt32(&res5.at(0), res5.size()); // 65535
    auto res13 = Core::Utility::DeltaDecode_uInt32(&res6.at(0), res6.size()); // 125799

    return 0;
}