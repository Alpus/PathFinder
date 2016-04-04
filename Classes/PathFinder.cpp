#include "../Headers/Graph.h"
#include "../Headers/PathFinder.h"

#include <algorithm>
#include <set>

using std::set;
using std::reverse;

const double Graph::PathFinder::getPathCost(const Graph::Node& goal) {
    return *costs[goal];
}

Graph::PathFinder::Costs::Costs(const Graph &graph) {
    costs.assign(graph.getSize(), numeric_limits<double>::infinity());
}

Graph::PathFinder::PathFinder(Graph& graph): graph(&graph), costs(Costs(graph)) {}

double* const Graph::PathFinder::Costs::operator[](const Graph::Node& node) {
    return &costs[node.getId()];
}

const vector<const Graph::Node*> Graph::HeuristicSearch::getFullPath(Node &goal) {
    if (getPathCost(goal) == numeric_limits<double>::infinity()) {
        return vector<const Graph::Node*>();
    }
    vector<const Graph::Node*> pathOrder;
    pathOrder.push_back(&goal);

    const Graph::Node* curNode = &goal;

    const vector<Graph::Edge>* edges;
    double destCost;
    const Graph::Node* destNode;
    while (curNode != begin) {
        edges = curNode->getEdges();
        for (auto edgeIter = edges->begin(); edgeIter != edges->end(); ++edgeIter) {
            if (edgeIter->getType() == Graph::EdgeType::Backward ||
                edgeIter->getType() == Graph::EdgeType::BiDirect) {

                destNode = edgeIter->getDest();
                destCost = *costs[*destNode];
                if (*costs[*curNode] - edgeIter->getCost() == destCost) {
                    pathOrder.push_back(destNode);
                    curNode = destNode;
                    break;
                }
            }
        }
    }

    reverse(pathOrder.begin(), pathOrder.end());
    return pathOrder;
}

const bool operator<(tuple<double, const Graph::Node*> first,
                     tuple<double, const Graph::Node*> second) {
    return get<0>(first) < get<0>(second);
}

const double Graph::HeuristicSearch::getMinCost(const Graph::Edge &edge) {
    return *costs[*edge.getFrom()] + edge.getCost();
}

void Graph::HeuristicSearch::findPath(const Graph::Node& begin, const Graph::Node& goal) {
    uint64_t errCounter = 0;

    this->begin = &begin;
    set<tuple<double, const Graph::Node*>> open;

    costs = Costs(*graph);
    *costs[begin] = 0;
    open.insert(make_tuple(double(0), &begin));

    const Graph::Node* curNode;
    double curCost;
    const vector<Graph::Edge>* edges;
    while (!open.empty()) {
        ++errCounter;
        if (errCounter > 2 * graph->getSize()) {
            throw Graph::Exeptions::EndlessSearch();
        }
        curNode = get<1>(*open.begin());
        open.erase(open.begin());

        if (curNode == &goal) {
            break;
        }

        edges = curNode->getEdges();
        tuple<double, const Graph::Node*> oldNode, newNode;
        const Graph::Node* curDest;
        for (auto edgeIter = edges->begin(); edgeIter != edges->end(); ++edgeIter) {
            if (edgeIter->getType() == Graph::EdgeType::Forward ||
                edgeIter->getType() == Graph::EdgeType::BiDirect) {

                curDest = edgeIter->getDest();
                oldNode = make_tuple(*costs[*curDest], curDest);
                newNode = make_tuple(getMinCost(*edgeIter) + getHeuristic(*curNode), curDest);
                if (newNode < oldNode) {
                    open.erase(oldNode);
                    *costs[*curDest] = get<0>(newNode);
                    open.insert(newNode);
                }
            }
        }
    }
}

const double Graph::DijkstraSearch::getHeuristic(const Graph::Node &node) {
    return 0;
}

