#include "Logger.hpp"
#include "Datatypes.hpp"
#include "o5mFile.hpp"
#include "Graph.h"

int32_t main()
{
    // --- stxxl config -> taken from: https://stxxl.org/tags/1.4.1/install_config.html
    #ifdef _WIN32
        stxxl::config * cfg = stxxl::config::get_instance();
        stxxl::disk_config disk_win("disk=C:\\Users\\maxib\\stxxl.tmp, 2 GiB, wincall delete");
        cfg->add_disk(disk_win);
    #elif __linux__
        stxxl::config * cfg = stxxl::config::get_instance();
        stxxl::disk_config disk1("disk=/tmp/stxxl.tmp, 2 GiB, syscall unlink");
        disk1.direct = stxxl::disk_config::DIRECT_ON;
        cfg->add_disk(disk1);
    #endif

    Core::Logger::Init();
    LOG(INFO) << "#############\t Routenplanung \t\t#############";

    //--- General
    Core::o5mFile duesseldorfStreets;
    duesseldorfStreets.ReadIn("../Res/regbez-duesseldorf-hard-filtered.o5m");
    duesseldorfStreets.DisplayStatistics();

    //duesseldorfStreets.SortEdgesStartAscending();
    //duesseldorfStreets.FindNodesOfRankTwo();      //Deleted for now => is in source control if needed again

    //--- Displaying
    //duesseldorfStreets.DisplayAllNodes();
    duesseldorfStreets.DisplayFirstThreeNodes();
    duesseldorfStreets.DisplayLastThreeNodes();
    duesseldorfStreets.DisplayLastThreeStringTableEntries();
    duesseldorfStreets.DisplayAllNodeEdges(10065266642);
    duesseldorfStreets.DisplayFirstThreeWays();
    duesseldorfStreets.DisplayLastThreeWays();

    //--- Geohash
    //auto hashval = Core::Utility::geohash(48.99885, 27.53426, 9);
    //auto strval  = Core::Utility::geohash_tostring(hashval, 9);
    //auto decoded = Core::Utility::geohash_decode(hashval, 9);

    //--- Graph
    Core::graph g;
    g.create(duesseldorfStreets);
    // 19, Weinheimer Straße nach 10, Weilburger Weg
    // 253026066 nach 1026008204

    //Kronprinzenstraße 48 nach Jahnstraße 62
    // 8839776766 nach 3578141726

    auto sp = g.DijkstraShortestPath(8839776766, 3578141726, duesseldorfStreets);

    for (uint64_t node: sp)
        LOG(INFO) << node;

    return 0;
}