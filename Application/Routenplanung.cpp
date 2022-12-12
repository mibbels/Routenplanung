#include "Logger.hpp"
#include "Datatypes.hpp"
#include "o5mFile.hpp"
#include "Graph.h"

void FindNodesRank2(Core::o5mFile &a_file)
{
    struct tmp{
        uint64_t predOsmID;
        uint64_t sucOsmID;
    };
    std::map<uint64_t, tmp> mapping;

    LOG(INFO) << "sort by start-node";

    a_file.SortEdgesStartAscending();
    const Core::edgeVec_t *edgeVec = a_file.GetEdgeVectorConst();

    auto it = edgeVec->begin();

    while (it != edgeVec->end())
    {
        if(it + 1 != edgeVec->end())
        {
            if(it->startNode != (it + 1)->startNode)
            {
                mapping.insert(std::make_pair(it->startNode, tmp{0, it->endNode}));
            }
        }
        it++;
    }

    LOG(INFO) << "sort by end-node";

    a_file.SortEdgesEndAscending();

    it = edgeVec->begin();

    while (it != edgeVec->end())
    {
        if(it + 1 != edgeVec->end())
        {
            if(it->endNode != (it + 1)->endNode)
            {
                if(mapping.find(it->endNode) != mapping.end())
                {
                    mapping[it->endNode].predOsmID = it->startNode;
                }
            }
        }
        it++;
    }

    // edgeVec.size() == 22.273.441
    auto s = mapping.size(); //16.888.215

    auto it_map = mapping.begin();
    while(it_map != mapping.end())
    {
        if (it_map->second.predOsmID == 0 || it_map->second.sucOsmID == 0)
        {
            it_map = mapping.erase(it_map);
        }
        else
        {
            it_map++;
        }
    }
    auto t = mapping.size(); // 16.810.089

    LOG(INFO) << s;
    LOG(INFO) << t;

}

int32_t main()
{

    // --- stxxl config -> taken from: https://stxxl.org/tags/1.4.1/install_config.html
#ifdef _WIN32
    stxxl::config * cfg = stxxl::config::get_instance();
    stxxl::disk_config disk_win("disk=C:\\Users\\maxib\\stxxl.tmp, 20 GiB, wincall delete");
    cfg->add_disk(disk_win);
#elif __linux__
    stxxl::config * cfg = stxxl::config::get_instance();
    stxxl::disk_config disk1("disk=/tmp/stxxl.tmp, 20 GiB, syscall unlink");
    disk1.direct = stxxl::disk_config::DIRECT_ON;
    cfg->add_disk(disk1);
#endif

    Core::Logger::Init();
    LOG(INFO) << "#############\t Routenplanung \t\t#############";

    auto hashval = Core::Utility::geohash(48.99885, 27.53426, 3);
    auto strval = Core::Utility::geohash_tostring(hashval, 3);
    auto decoded = Core::Utility::geohash_decode(hashval, 3);

    //--- General
    Core::o5mFile duesseldorfStreets;
    duesseldorfStreets.ReadIn("../Res/regbez-duesseldorf-streets-clean0.o5m");
    duesseldorfStreets.DisplayStatistics();

    //FindNodesRank2(duesseldorfStreets);

    //--- Nodes
    //duesseldorfStreets.DisplayAllNodes();
    duesseldorfStreets.DisplayFirstThreeNodes();
    duesseldorfStreets.DisplayLastThreeNodes();
    duesseldorfStreets.DisplayLastThreeStringTableEntries();

    //--- Ways
    //duesseldorfStreets.DisplayAllWays();
    //duesseldorfStreets.DisplayFirstThreeWays();
    //duesseldorfStreets.DisplayLastThreeWays();

    //--- Edges
    //duesseldorfStreets.DisplayAllEdges();
    //duesseldorfStreets.DisplayFirstThreeEdges();
    //duesseldorfStreets.DisplayLastThreeEdges();

    // --- Sort edges
    //duesseldorfStreets.SortEdgesStartAscending();
    //duesseldorfStreets.DisplayFirstThreeEdges();
    //duesseldorfStreets.DisplayLastThreeEdges();

    //--- Association example
    //auto index   = duesseldorfStreets.GetNodeIndex(160223); //First node
    //auto nodeVec = duesseldorfStreets.GetNodeVector();
    //auto nodeAt  = nodeVec->at(index);

    //auto index2   = duesseldorfStreets.GetNodeIndex(10065269349); //Last node
    //auto nodeVec2 = duesseldorfStreets.GetNodeVector();
    //auto nodeAt2  = nodeVec2->at(index2);

    //TestGraph("../Res/graph.txt");

    //Core::graph g;
    //g.create(duesseldorfStreets);
    //g.DijkstraShortestPath(143598317, 143598305, duesseldorfStreets);

    return 0;
}