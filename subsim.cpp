#include "stdafx.h"
#include "SFMT/dSFMT/dSFMT.c"
#include "alg.cpp"

void init_random_seed()
{
    // Randomize the seed for generating random numbers
    dsfmt_gv_init_gen_rand(static_cast<uint32_t>(time(nullptr)));
}
int Logger::mode;
int main(int argc, char* argv[])
{

    TArgument Arg(argc, argv);
    Logger::mode = Arg._debug;
    Logger::getDatetime();
    if (Arg._probDist == PROB_DIST_ERROR)
    {
        Logger::LogInfo("The input probability distribution is not supported:", Arg._probDistStr);
        Logger::LogInfo("The supported probability distribution: weights, wc, uniform, skewed");
        return 0;
    }

    if (Arg._func == FUNC_ERROR)
    {
        Logger::LogInfo("The input func is not supported: ", Arg._funcStr);
        Logger::LogInfo("The supported func: format, im");
    }

    init_random_seed();

    const std::string infilename = Arg._dir + "/" + Arg._graphname;
    if (Arg._func == FORMAT)
    {
        // Format the graph
        GraphBase g;
        g.FormatGraph(infilename, Arg._probDist, Arg._wcVar, Arg._probEdge, Arg._skewType);
        return 0;
    }

    Timer mainTimer("main");
    // Load the reverse graph
    GraphBase g;
    Graph graph;
    g.LoadGraph(graph, infilename, Arg._probDist, Arg._wcVar, Arg._probEdge, Arg._skewType);
    // int probDist = GraphBase::LoadGraphProbDist(infilename);
    int probDist = Arg._probDist;

    // Initialize a result object to record the results
    TResult tRes;
    TAlg tAlg(graph, tRes);
    tAlg.set_vanilla_sample(Arg._vanilla);
    tAlg.set_prob_dist((ProbDist)probDist); // Set propagation model
    auto delta = Arg._delta;

    if (delta < 0) delta = 1.0 / graph.size();

    if (Arg._rrset)
    {
        double time = tAlg.rrsetOnly(Arg._rrsetNum);
        TIO::writeRRsetResult(Arg._graphname, Arg._probDist, Arg._wcVar, Arg._probEdge, Arg._skewType, time, Arg._resultFolder, Arg._vanilla, Arg._rrsetNum);
        Logger::LogInfo("result", time);
        Logger::LogInfo("Result written!!!");
        return 0;
    }

    int seedSize = Arg._seedsize;
    std::cout << "seedSize k=" << seedSize << std::endl;
    std::cout << "---The Begin of " << Arg._outFileName << "---\n";


    if (!Arg._hist)
    {
        tAlg.subsimOnly(seedSize, Arg._eps, delta);
    }
    else
    {
        std::cout << "HIST is invoked." << std::endl;
        if (seedSize < 10)
        {
            tAlg.subsimWithTrunc(seedSize, Arg._eps, delta);
        }
        else
        {
            tAlg.subsimWithHIST(seedSize, Arg._eps, delta);
        }
    }

    TIO::WriteResult(Arg._outFileName, tRes, Arg._resultFolder);
    TIO::WriteOrderSeeds(Arg._outFileName, tRes, Arg._resultFolder);
    std::cout << "---The End of " << Arg._outFileName << "---\n";
    tAlg.RefreshHypergraph();
    return 0;
}