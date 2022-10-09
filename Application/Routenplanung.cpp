#include "Logger.hpp"
#include "Datatypes.hpp"
#include "Parser.hpp"
#include "Utility.hpp"
#include "Graph.h"

int TestGraph(std::string a_strInfile)
{
    std::string strLine, strTok;
    std::ifstream in;
    in.open(a_strInfile);

    std::vector<Core::node*> vecNodes;
    std::vector<Core::EdgeComponents> vecEdges;

    std::vector<std::string> vecLineContents;

    std::unordered_map<std::string, int> mapStringCodes;

    if (!in)
        return -1;

    while (std::getline(in, strLine))
    {
        std::istringstream ssLine(strLine);

        while (std::getline(ssLine, strTok, ','))
        {
            vecLineContents.push_back(strTok);
        }
    }

    for (uint32_t i = 0; i < vecLineContents.size(); i+=3)
    {
        int iCodeA = std::hash<std::string>()(vecLineContents[i]);
        int iCodeB = std::hash<std::string>()(vecLineContents[i + 1]);
        mapStringCodes[vecLineContents[i]] = iCodeA;
        mapStringCodes[vecLineContents[i + 1]] = iCodeB;

        Core::node* a = new Core::node(iCodeA);
        Core::node* b = new Core::node(iCodeB);

        vecNodes.push_back(a);
        vecNodes.push_back(b);

        vecEdges.push_back(std::make_tuple(std::atoi(vecLineContents[i + 2].c_str()), iCodeA, iCodeB));
    }

    Core::graph* g = new Core::graph(vecNodes, vecEdges);
    g->SetStringHashes(mapStringCodes);
    auto vec = g->DijkstraShortestPath("a", "b");
    g->PrintNodes(vec);

    return 0;
}

int main()
{
    Core::Logger::Init();
    LOG(INFO) << "##### Routenplanung #####";

    Core::o5mFile fileStatistics;
    Core::Parser::ReadIn_o5m(fileStatistics, "../Res/regbez-duesseldorf-streets-clean0.o5m");
    Core::Parser::ShowStatistics(fileStatistics);

    //TestGraph("../Res/graph.txt");

    // ------- A few tests for uint32_t delta encoding/decoding -------

    //auto res0 = Core::Utility::DeltaEncode_uInt32(5);      // => 0x05
    //auto res1 = Core::Utility::DeltaEncode_uInt32(127);    // => 0x7f
    //auto res2 = Core::Utility::DeltaEncode_uInt32(323);    // => 0xc3 0x02
    //auto res3 = Core::Utility::DeltaEncode_uInt32(1020);   // => 0xfc 0x07
    //auto res4 = Core::Utility::DeltaEncode_uInt32(16384);  // => 0x80 0x80 0x01
    //auto res5 = Core::Utility::DeltaEncode_uInt32(65535);  // => 0xff 0xff 0x03
    //auto res6 = Core::Utility::DeltaEncode_uInt32(125799); // => 0xe7 0xd6 0x07

    //auto res7 = Core::Utility::DeltaDecode_uInt32(&res0.at(0), res0.size());  // 5
    //auto res8 = Core::Utility::DeltaDecode_uInt32(&res1.at(0), res1.size());  // 127
    //auto res9 = Core::Utility::DeltaDecode_uInt32(&res2.at(0), res2.size());  // 323
    //auto res10 = Core::Utility::DeltaDecode_uInt32(&res3.at(0), res3.size()); // 1020
    //auto res11 = Core::Utility::DeltaDecode_uInt32(&res4.at(0), res4.size()); // 16384
    //auto res12 = Core::Utility::DeltaDecode_uInt32(&res5.at(0), res5.size()); // 65535
    //auto res13 = Core::Utility::DeltaDecode_uInt32(&res6.at(0), res6.size()); // 125799

    // ------- A few tests for int32_t delta encoding/decoding -------

    //We experience some kind of precision loss when using very big numbers
    //f.E: DeltaEncode_Int32(14312677) the last byte should be 0x0d but its 0x06
    //I think its due to all the bit shifting that is going on internally

    auto res01 = Core::Utility::DeltaEncode_Int32(-65);      // => 0x81 0x01
    auto res02 = Core::Utility::DeltaEncode_Int32(-3);       // => 0x05
    auto res03 = Core::Utility::DeltaEncode_Int32(-2);       // => 0x03
    auto res10 = Core::Utility::DeltaEncode_Int32(4);        // => 0x08
    auto res11 = Core::Utility::DeltaEncode_Int32(64);       // => 0x80 0x01
    auto res12 = Core::Utility::DeltaEncode_Int32(2952);     // => 0x90 0x2e
    auto res13 = Core::Utility::DeltaEncode_Int32(125799);   // => 0xce 0xad 0x0f
    auto res14 = Core::Utility::DeltaEncode_Int32(3999478);  // => 0xec 0x9b 0xe8 0x03
    auto res15 = Core::Utility::DeltaEncode_Int32(11560506); // => 0xf4 0x98 0x83 0x0b
    auto res16 = Core::Utility::DeltaEncode_Int32(14312677); // => 0xca 0x93 0xd3 0x0d

    return 0;
}