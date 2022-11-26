#include "Logger.hpp"
#include "Datatypes.hpp"
#include "o5mFile.hpp"
#include "Graph.h"
#include "Utility.hpp"

/*
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
*/

int32_t main()
{

    // taken from: https://stxxl.org/tags/1.4.1/install_config.html

    stxxl::config * cfg = stxxl::config::get_instance();

    stxxl::disk_config disk_win("disk=C:\\Users\\maxib\\stxxl.tmp, 10 GiB, wincall delete");
    /*  use size = 0 for "autogrow"
     */

    //disk1.direct = stxxl::disk_config::DIRECT_ON; // force O_DIRECT

    cfg->add_disk(disk_win);


    Core::Logger::Init();
    LOG(INFO) << "#############\t Routenplanungstool \t\t#############";

    //--- General
    Core::o5mFile duesseldorfStreets;
    duesseldorfStreets.ReadIn("../Res/regbez-duesseldorf-streets-clean0.o5m");
    duesseldorfStreets.DisplayStatistics();

    //--- Nodes
    //duesseldorfStreets.DisplayAllNodes();
    //duesseldorfStreets.DisplayFirstThreeNodes();
    //duesseldorfStreets.DisplayLastThreeNodes();

    //--- Ways
    //duesseldorfStreets.DisplayAllWays();
    //duesseldorfStreets.DisplayFirstThreeWays();
    //duesseldorfStreets.DisplayLastThreeWays();

    //TestGraph("../Res/graph.txt");
    //Core::graph* g = new Core::graph();
    //g->create(duesseldorfStreets);

    return 0;
}