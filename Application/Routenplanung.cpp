#include "Logger.hpp"
#include "Datatypes.hpp"
#include "o5mFile.hpp"
#include "Graph.h"

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
    auto vec1 = g->DijkstraShortestPath("R", "a");
    auto vec2 = g->a_starShortestPath("R", "a");
    g->PrintNodes(vec1);
    LOG(INFO) << "-----------";
    g->PrintNodes(vec2);

    return 0;
}

int32_t main()
{
    Core::Logger::Init();
    LOG(INFO) << "########### Routenplanung #############";

    Core::o5mFile duesseldorfStreets;
    duesseldorfStreets.ReadIn("../Res/regbez-duesseldorf-streets-clean0.o5m");
    duesseldorfStreets.DisplayStatistics();
    duesseldorfStreets.DisplayNodes();

    //TestGraph("../Res/graph.txt");

    return 0;
}