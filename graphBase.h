#pragma once

bool greater_first(const std::pair<uint32_t, double> x, const std::pair<uint32_t, double> y)
{
  if (x.second > y.second) return true;
  else if (x.second < y.second) return false;
  else if (x.first < y.first) return true;
  else return false;
}

class GraphBase
{
public:
  CountType numV, numE;
  NeighborList nodeList;
  NeighborWeightList weightList;
  CountType* neighborNumList;
  std::string filename;
  ProbDist probDist;
  double sum;
  double prob;
  std::string skewType;
  std::string buildGraphName() {
    std::stringstream graphname;
    graphname << filename << "_" << build_distStr();
    if (probDist == UNIFORM) graphname << "_" << prob;
    else if (probDist == WC) graphname << "_" << sum;
    return graphname.str();
  }

  std::string build_distStr()
  {
    std::string distStr;

    if (probDist == WEIGHTS)
    {
      distStr = "weights";
    }
    else if (probDist == WC)
    {
      distStr = "wc";
    }
    else if (probDist == UNIFORM)
    {
      distStr = "uniform";
    }
    else if (probDist == SKEWED && skewType == "weibull")
    {
      distStr = "weibull";
    }
    else if (probDist == SKEWED && skewType == "exp") distStr = "exp";
    else distStr = "unknown";
    return distStr;
  }

  /// Format the input for future computing, which is much faster for loading. Vector serialization is used.
  void FormatGraph(const std::string _filename, ProbDist _probDist, const double _sum, const double _prob, const std::string _skewType)
  {
    Logger::LogInfo("Cannot format.");
  }

  /// Load graph via vector deserialization.
  void LoadGraph(Graph& graph, const std::string _filename, ProbDist _probDist, const double _sum, const double _prob, const std::string _skewType)
  {
    filename = _filename;
    probDist = _probDist;
    sum = _sum;
    prob = _prob;
    skewType = _skewType;
    Logger::LogInfo("Graph", filename);
    Logger::LogInfo("probDist", build_distStr());
    Logger::LogInfo("wcPara", sum);
    Logger::LogInfo("uniformPara", prob);
    Logger::LogInfo("skewType", skewType);
    std::string graphName = buildGraphName();
    std::ifstream graphAttrIn(filename + ".attribute");
    CountType numV, numE;
    graphAttrIn >> numV >> numE;
    Logger::LogInfo("numV", numV);
    Logger::LogInfo("numE", numE);
    graphAttrIn.close();
    nodeList = new NodeType[numE];
    neighborNumList = new CountType[numV + 1];
    std::ifstream foutEL(filename + ".outEdges", std::ios::in | std::ios::binary);
    std::ifstream foutPL(filename + ".outPtr", std::ios::in | std::ios::binary);
    if (probDist == SKEWED) {
      weightList = new WeightType[numE];
      std::ifstream foutWEL(graphName + ".outWEdges", std::ios::in | std::ios::binary);
      foutWEL.read((char*)&weightList[0], sizeof(weightList[0]) * numE);
      foutWEL.close();
    }
    foutEL.read((char*)&nodeList[0], sizeof(nodeList[0]) * numE);
    foutPL.read((char*)&neighborNumList[0], sizeof(neighborNumList[0]) * (numV + 1));
    foutEL.close();
    foutPL.close();
    Logger::LogInfo("Load success!!!");
    graph.resize(numV);
    for (CountType i = 0; i < numV; i++)
    {
      for (CountType j = neighborNumList[i];j < neighborNumList[i + 1];j++) {
        if (probDist == SKEWED)
          graph[i].push_back(Edge(nodeList[j], weightList[j]));
        else if (probDist == UNIFORM)
          graph[i].push_back(Edge(nodeList[j], _prob));
        else if (probDist == WC)
          graph[i].push_back(Edge(nodeList[j], _sum / (neighborNumList[i + 1] - neighborNumList[i])));
      }
    }
    if (Logger::mode == _DEBUG) {
      for (CountType i = 0;i < 10;i++)
        for (CountType j = 0;j < graph[i].size();j++)
          std::cout << i << " " << graph[i][j].first << " " << graph[i][j].second << std::endl;
    }
    return;
  }
};
