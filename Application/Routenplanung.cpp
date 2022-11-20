#include "Logger.hpp"
#include "Datatypes.hpp"
#include "o5mFile.hpp"
#include "Graph.h"
#include "Utility.hpp"

int32_t TestGraph(const std::string& a_strInfile)
{
    std::string strLine, strTok;
    std::ifstream in;
    in.open(a_strInfile);

    std::vector<Core::node*> vecNodes;
    std::vector<Core::EdgeComponents> vecEdges;

    std::vector<std::string> vecLineContents;

    std::unordered_map<std::string, int32_t> mapStringCodes;

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
        int32_t iCodeA = std::hash<std::string>()(vecLineContents[i]);
        int32_t iCodeB = std::hash<std::string>()(vecLineContents[i + 1]);
        mapStringCodes[vecLineContents[i]] = iCodeA;
        mapStringCodes[vecLineContents[i + 1]] = iCodeB;

        Core::node* a = new Core::node(iCodeA);
        Core::node* b = new Core::node(iCodeB);

        vecNodes.push_back(a);
        vecNodes.push_back(b);

        vecEdges.emplace_back(std::make_tuple(std::atoi(vecLineContents[i + 2].c_str()), iCodeA, iCodeB));
    }

    Core::graph* g = new Core::graph(vecNodes, vecEdges);
    g->SetStringHashes(mapStringCodes);
    auto vec1 = g->DijkstraShortestPath("x", "y");
    auto vec2 = g->a_starShortestPath("x", "y");
    auto vec3 = g->BellmanFordShortestPath("x", "y");
    g->PrintNodes(vec1);
    LOG(INFO) << "-----------";
    g->PrintNodes(vec2);
    LOG(INFO) << "-----------";
    g->PrintNodes(vec3);

    return 0;
}

int32_t main()
{
    Core::Logger::Init();
    LOG(INFO) << "#############\t Routenplanungstool \t\t#############";

    Core::o5mFile duesseldorfStreets;
    duesseldorfStreets.ReadIn("../Res/regbez-duesseldorf-streets-clean0.o5m");
    //duesseldorfStreets.DisplayStatistics();
    //duesseldorfStreets.DisplayAllNodes();
    duesseldorfStreets.DisplayFirstThreeNodes();
    duesseldorfStreets.DisplayLastThreeNodes();
    duesseldorfStreets.DisplayAllWays();
    //duesseldorfStreets.DisplayFirstThreeNodes();
    //duesseldorfStreets.DisplayLastThreeNodes();

    //0x01 0x80 0xa5 0xc7 0x03 0x22 0x00 0x7e 0xda 0x8c 0x9f 0xf8 0x20 0xff 0xaf 0xb9 0xb5 0x1d 0xca 0xa6 0x91 0xb5 0x1d 0xf9 0x80 0xfe

    //0xda 0x8c 0x9f 0xf8 0x20
    //std::vector<uint8_t> values = {0xda, 0x8c, 0x9f, 0xf8, 0x20};
    //std::vector<uint8_t> values2 = {0xaf, 0xb9, 0xb5, 0x1d};
    //std::vector<uint8_t> values3 = {0x22};
    //auto val2 = Core::Utility::DeltaDecode_Int32(&values2[0], values2.size());
    //std::vector<uint8_t> values4 = {0xff, 0xaf, 0xb9, 0xb5, 0x1d};
    //auto val2 = Core::Utility::DeltaDecode_Int32(&values4[0], values4.size());
    //auto sum = val1 + val2;

    //std::vector<uint8_t> way = {0x00, 0xec, 0x9b, 0xe8, 0x03, 0x00, 0x07, 0xce, 0xb9, 0xfe, 0x13, 0xce, 0xeb, 0x01};
    //Core::Utility::ProcessWay(0, way, way.size());

    //0xac 0xd8 0x01 0x0d 0x00 0x26 0xd9 0xd5 0xc3 0x97 0x09 0x08 0x04 0x04 0xf4 0xd8 0xe7 0x84 0x36 0xf1 0xd8 0xe7 0x84 0x36 0xe2 0xd8 0xe7 0x84 0x36 0xdd 0xd8 0xe7 0x84 0x36 0xe4 0xcb 0xe8 0xb5 0x2a 0x9b 0xa2 0x9f 0xf0 0x24

    //0xdc 0x9e 0x1e 0x14 0x00 0x0f 0xad 0xf9 0xa0 0xc3 0x05 0xae 0xf9 0x8f 0xe4 0x09 0xd3 0xd2 0x9e 0xea 0x07

    //std::vector<uint8_t> values = {0xad, 0xf9, 0xa0, 0xc3, 0x05};
    //auto val1 = Core::Utility::DeltaDecode_Int32(&values[0], values.size());

    //std::vector<uint8_t> values2 = {0xae, 0xf9, 0x8f, 0xe4, 0x09};
    //auto val2 = Core::Utility::DeltaDecode_Int32(&values2[0], values2.size());

    //std::vector<uint8_t> values3 = {0xd3, 0xd2, 0x9e, 0xea, 0x07};
    //auto val3 = Core::Utility::DeltaDecode_Int32(&values3[0], values3.size());

    //TestGraph("../Res/graph.txt");

    return 0;
}