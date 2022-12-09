#pragma once
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif
class IoController
{
public:
    static void mkdir_absence(const char* outFolder)
    {
#if defined(_WIN32)
        CreateDirectoryA(outFolder, nullptr); // can be used on Windows
#else
        mkdir(outFolder, 0733); // can be used on non-Windows
#endif
    }

    static std::string build_distStr(ProbDist dist, std::string skewType = "err")
    {
        std::string distStr;

        if (dist == WEIGHTS)
        {
            distStr = "weights";
        }
        else if (dist == WC)
        {
            distStr = "wc";
        }
        else if (dist == UNIFORM)
        {
            distStr = "uniform";
        }
        else if (dist == SKEWED && skewType == "weibull")
        {
            distStr = "weibull";
        }
        else if (dist == SKEWED && skewType == "exp") distStr = "exp";
        else distStr = "unknown";
        return distStr;
    }

    /// Save a serialized file
    template <class T>
    static void SaveFile(const std::string filename, const T& output)
    {
        std::ofstream outfile(filename, std::ios::binary);

        if (!outfile.eof() && !outfile.fail())
        {
            StreamType res;
            serialize(output, res);
            outfile.write(reinterpret_cast<char*>(&res[0]), res.size());
            outfile.close();
            res.clear();
            std::cout << "Save file successfully: " << filename << '\n';
        }
        else
        {
            std::cout << "Save file failed: " + filename << '\n';
            exit(1);
        }
    }

    /// Load a serialized file
    template <class T>
    static void LoadFile(const std::string filename, T& input)
    {
        std::ifstream infile(filename, std::ios::binary);

        if (!infile.eof() && !infile.fail())
        {
            infile.seekg(0, std::ios_base::end);
            const std::streampos fileSize = infile.tellg();
            infile.seekg(0, std::ios_base::beg);
            std::vector<uint8_t> res(fileSize);
            infile.read(reinterpret_cast<char*>(&res[0]), fileSize);
            infile.close();
            input.clear();
            auto it = res.cbegin();
            input = deserialize<T>(it, res.cend());
            res.clear();
        }
        else
        {
            std::cout << "Cannot open file: " + filename << '\n';
            exit(1);
        }
    }

    /// Save graph structure to a file
    static void SaveGraphStruct(const std::string graphName, const Graph& vecGraph, const bool isReverse)
    {
        std::string postfix = ".vec.graph";

        if (isReverse) postfix = ".vec.rvs.graph";

        const std::string filename = graphName + postfix;
        SaveFile(filename, vecGraph);
    }

    /// Load graph structure from a file
    static void LoadGraphStruct(const std::string graphName, Graph& vecGraph, const bool isReverse)
    {
        std::string postfix = ".vec.graph";

        if (isReverse) postfix = ".vec.rvs.graph";

        const std::string filename = graphName + postfix;
        LoadFile(filename, vecGraph);
    }

    static void SaveGraphProbDist(const std::string graphName, int dist)
    {
        std::ofstream outFile(graphName + ".probdist");
        outFile << dist;
    }


    static int LoadGraphProbDist(const std::string graphName)
    {
        std::string filename = graphName + ".probdist";
        std::ifstream infile(filename);
        int probDist = WEIGHTS;

        if (!infile.is_open())
        {
            std::cout << "The file \"" + filename + "\" can NOT be opened\n";
            return probDist;
        }

        infile >> probDist;
        infile.close();
        std::cout << "probability distribution: " << probDist << std::endl;
        return probDist;
    }

    /// Get out-file name
    static std::string BuildOutFileName(const std::string graphName, const std::string algName, const int seedsize,
        const std::string probDist, const float probEdge)
    {
        if (probDist == "uniform")
        {
            return graphName + "_" + algName + "_k" + std::to_string(seedsize) + "_" + probDist + "_" + std::
                to_string(probEdge);
        }

        return graphName + "_" + algName + "_k" + std::to_string(seedsize) + "_" + probDist;
    }

    /// Print the results
    static void WriteResult(const std::string& outFileName, const TResult& resultObj, const std::string& outFolder)
    {
        const auto approx = resultObj.get_approximation();
        const auto runTime = resultObj.get_running_time();
        const auto influence = resultObj.get_influence();
        const auto influenceOriginal = resultObj.get_influence_original();
        const auto seedSize = resultObj.get_seed_size();
        const auto RRsetsSize = resultObj.get_RRsets_size();
        std::cout << "   --------------------" << std::endl;
        std::cout << "  |Approx.: " << approx << std::endl;
        std::cout << "  |Time (sec): " << runTime << std::endl;
        std::cout << "  |Influence: " << influence << std::endl;
        std::cout << "  |Self-estimated influence: " << influenceOriginal << std::endl;
        std::cout << "  |#Seeds: " << seedSize << std::endl;
        std::cout << "  |#RR sets: " << RRsetsSize << std::endl;
        std::cout << "   --------------------" << std::endl;
        mkdir_absence(outFolder.c_str());
        std::ofstream outFileNew(outFolder + "/" + outFileName);

        if (outFileNew.is_open())
        {
            outFileNew << "Approx.: " << approx << std::endl;
            outFileNew << "Time (sec): " << runTime << std::endl;
            outFileNew << "Influence: " << influence << std::endl;
            outFileNew << "Self-estimated influence: " << influenceOriginal << std::endl;
            outFileNew << "#Seeds: " << seedSize << std::endl;
            outFileNew << "#RR sets: " << RRsetsSize << std::endl;
            outFileNew.close();
        }
    }

    /// Print the seeds
    static void WriteOrderSeeds(const std::string& outFileName, const TResult& resultObj, const std::string& outFolder)
    {
        auto vecSeed = resultObj.get_seed_vec();
        mkdir_absence(outFolder.c_str());
        const auto outpath = outFolder + "/seed";
        mkdir_absence(outpath.c_str());
        std::ofstream outFile(outpath + "/seed_" + outFileName);

        for (auto i = 0; i < vecSeed.size(); i++)
        {
            outFile << vecSeed[i] << '\n';
        }

        outFile.close();
    }
    static void writeRRsetResult(const std::string filename, ProbDist probDist, const float sum, const float prob, const std::string skewType, double time, std::string path, bool vinalla, int rrsetNum) {
        std::string name = buildGraphName(filename, probDist, sum, prob, skewType);
        if (vinalla) name += "_vanilla";
        std::stringstream outputfile;
        outputfile << name << "_" << rrsetNum;
        std::ofstream output(path + "/" + outputfile.str());
        output << time << std::endl;
    }

    static std::string buildGraphName(const std::string filename, ProbDist probDist, const float sum, const float prob, const std::string skewType) {
        std::stringstream graphname;
        graphname << filename << "_" << IoController::build_distStr(probDist, skewType);
        if (probDist == UNIFORM) graphname << "_" << prob;
        else if (probDist == WC) graphname << "_" << sum;
        return graphname.str();
    }
};

using TIO = IoController;
using PIO = std::shared_ptr<IoController>;