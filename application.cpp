// application.cpp <Starter Code>
// <Nikita Mashchenko>
//
// University of Illinois at Chicago
// CS 251: Spring 2021
// Project #7 - Openstreet Maps
//
// References:
// TinyXML: https://github.com/leethomason/tinyxml2
// OpenStreetMap: https://www.openstreetmap.org
// OpenStreetMap docs:
//   https://wiki.openstreetmap.org/wiki/Main_Page
//   https://wiki.openstreetmap.org/wiki/Map_Features
//   https://wiki.openstreetmap.org/wiki/Node
//   https://wiki.openstreetmap.org/wiki/Way
//   https://wiki.openstreetmap.org/wiki/Relation
//

#include <iostream>
#include <iomanip>  /*setprecision*/
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <limits>
#include <algorithm>
#include <queue>
#include <stack>

#include "tinyxml2.h"
#include "dist.h"
#include "osm.h"
#include "graph.h"

using namespace std;
using namespace tinyxml2;

double INF = numeric_limits<double>::max();

// class prioritize that is used to calculate priority_queue
class prioritize {
 public:
bool operator() (const pair<long long, double>& p1,
 const pair<long long, double>& p2) const {
  if (p1.second == p2.second) {
    return p1.first > p2.first;
  }
    return p1.second > p2.second;
}
};

// Dijkstra algorithm
// map pre is used to store predecessors
vector <long long> Dijkstra(graph<long long, double>  &G,
long long startV, map<long long, double>  &distances,
map<long long, long long> &pre) {
    vector<long long> visited;

    set<long long> visitedSet;

    vector<long long> adjV;

    priority_queue<pair<long long, double>, vector<pair<long long, double>>,
    prioritize> unvisitedQueue;

    for (long long vertex : G.getVertices()) {
    distances[vertex] = INF;

    pre[vertex] = -1;

    unvisitedQueue.push(make_pair(vertex, INF));
    }
    unvisitedQueue.push(make_pair(startV, 0));

    distances[startV] = 0;

     while (!unvisitedQueue.empty()) {
          long long currentV = unvisitedQueue.top().first;

          unvisitedQueue.pop();

          if (distances[currentV] == INF) {
           break;

        } else if (visitedSet.count(currentV) > 0) {  // search visisted
           continue;
          } else {
           visited.push_back(currentV);

           visitedSet.insert(currentV);
          }

            for (long long adjV : G.neighbors(currentV)) {
           double edgeWeight = 0;

           G.getWeight(currentV, adjV, edgeWeight);

           double alternativePathDistance = distances[currentV] + edgeWeight;

           if (alternativePathDistance < distances[adjV]) {
            distances[adjV] = alternativePathDistance;

            pre[adjV] = currentV;

            unvisitedQueue.push(make_pair(adjV, alternativePathDistance));
           }
            }
}
  return visited;
}

int main() {
  // maps a Node ID to it's coordinates (lat, lon)
  map<long long, Coordinates>  Nodes;
  // info about each footway, in no particular order
  vector<FootwayInfo>          Footways;
  // info about each building, in no particular order
  vector<BuildingInfo>         Buildings;
  XMLDocument                  xmldoc;
  cout << "** Navigating UIC open street map **" << endl;
  cout << endl;
  cout << std::setprecision(8);

  string def_filename = "map.osm";
  string filename;

  cout << "Enter map filename> ";
  getline(cin, filename);

  if (filename == "") {
    filename = def_filename;
  }
  //
  // Load XML-based map file
  //
  if (!LoadOpenStreetMap(filename, xmldoc)) {
    cout << "**Error: unable to load open street map." << endl;
    cout << endl;
    return 0;
  }

  //
  // Read the nodes, which are the various known positions on the map:
  //
  int nodeCount = ReadMapNodes(xmldoc, Nodes);

  //
  // Read the footways, which are the walking paths:
  //
  int footwayCount = ReadFootways(xmldoc, Footways);

  //
  // Read the university buildings:
  //
  int buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);

  //
  // Stats
  //
  assert(nodeCount == (int)Nodes.size());
  assert(footwayCount == (int)Footways.size());
  assert(buildingCount == (int)Buildings.size());

  cout << endl;
  cout << "# of nodes: " << Nodes.size() << endl;
  cout << "# of footways: " << Footways.size() << endl;
  cout << "# of buildings: " << Buildings.size() << endl;


    graph<long long, double>  G;

    for (auto &c : Nodes) {
        G.addVertex(c.first);
    }

    for (size_t i = 0; i < Footways.size(); i++) {
         for (size_t j =0; j < (Footways[i].Nodes.size())-1; j++) {
            auto it = Nodes.find(Footways[i].Nodes[j]);
            auto it1 = Nodes.find(Footways[i].Nodes[j+1]);
            double weight = distBetween2Points(it->second.Lat,
            it->second.Lon, it1->second.Lat, it1->second.Lon);
            G.addEdge(Footways[i].Nodes[j], Footways[i].Nodes[j+1], weight);
            G.addEdge(Footways[i].Nodes[j+1], Footways[i].Nodes[j], weight);
         }
    }
    cout << "# of vertices: " << G.NumVertices() << endl;
    cout << "# of edges: " << G.NumEdges() << endl;
    cout << endl;

  //
  // Navigation from building to building
  //
  string startBuilding, destBuilding;

  cout << "Enter start (partial name or abbreviation), or #> ";
  getline(cin, startBuilding);

  while (startBuilding != "#") {
    cout << "Enter destination (partial name or abbreviation)> ";
    getline(cin, destBuilding);
    bool check = false;
    bool check1 = false;
    int startPos, endPos;

      for (size_t i1 = 0; i1 < Buildings.size(); i1++) {
            if (Buildings[i1].Abbrev == startBuilding) {
                check = true;
                startPos = i1;
                break;
            } else if ((Buildings[i1].Fullname.find(startBuilding))
            != string::npos) {
                check = true;
                startPos = i1;
                break;
            }
        }
              for (size_t i2 = 0; i2 < Buildings.size(); i2++) {
                if (Buildings[i2].Abbrev == destBuilding) {
                check1 = true;
                endPos = i2;
                break;
            } else if ((Buildings[i2].Fullname.find(destBuilding))
            != string::npos) {
                check1 = true;
                endPos = i2;
                break;
            }
        }
                if (check == true && check1 == true) {
                cout << "Starting point:" << endl;
                cout << " " << Buildings[startPos].Fullname << endl;
                cout << " " << "(" << Buildings[startPos].Coords.Lat
                << ", " << Buildings[startPos].Coords.Lon << ")" << endl;
                cout << "Destination point:" << endl;
                cout << " " << Buildings[endPos].Fullname << endl;
                cout << " " << "(" << Buildings[endPos].Coords.Lat <<
                ", " << Buildings[endPos].Coords.Lon << ")" << endl;
            }


    auto checkmin = Nodes.find(Footways[0].Nodes[0]);
    double min = distBetween2Points(Buildings[0].Coords.Lat,
    Buildings[0].Coords.Lon,
    checkmin->second.Lat, checkmin->second.Lon);
    double min2 = distBetween2Points(Buildings[0].Coords.Lat,
    Buildings[0].Coords.Lon,
    checkmin->second.Lat, checkmin->second.Lon);
    int index1;
    int index2;

    int index1_1;
    int index2_2;

    for (size_t k = 0; k < Footways.size(); k++) {
         for (size_t n = 0; n < Footways[k].Nodes.size(); n++) {
            auto it = Nodes.find(Footways[k].Nodes[n]);
            double start = distBetween2Points(Buildings[startPos].Coords.Lat,
            Buildings[startPos].Coords.Lon, it->second.Lat, it->second.Lon);
            if (start < min) {
                 min = start;
                index1_1 = k;
                index2_2 = n;
             }
            double finish = distBetween2Points(Buildings[endPos].Coords.Lat,
            Buildings[endPos].Coords.Lon, it->second.Lat, it->second.Lon);

            if (finish < min2) {
                min2 = finish;
                index1 = k;
                index2 = n;
            }
         }
    }

    cout << endl;
    auto result1 = Nodes.find(Footways[index1_1].Nodes[index2_2]);
    auto result = Nodes.find(Footways[index1].Nodes[index2]);

    if (check == false) {
        cout << "Start building not found" << endl;
    } else if (check1 == false) {
        cout << "Destination building not found" << endl;
    } else {
            cout << "Nearest start node:" << endl;
            cout << " " << result1->first << endl;
            cout << " " << "(" << result1->second.Lat << ", "
            << result1->second.Lon << ")" << endl;
            cout << "Nearest destination node:" << endl;
            cout << " " << result->first << endl;
            cout << " " << "(" << result->second.Lat << ", "
            << result->second.Lon << ")" << endl;

    map<long long, double> distances;
    map<long long, long long> pre;
    vector<long long> visited = Dijkstra(G, result1->first, distances, pre);

    double finalDist = 0.0;
    stack<long long> path;
    cout << endl;
    cout << "Navigating with Dijkstra..." << endl;

    if (distances[result->first] == INF) {
        cout << "Sorry, destination unreachable" << endl;
    } else if (result1 == result) {
        finalDist = distances[result->first] + finalDist;
        cout << "Distance to dest: " << finalDist << " miles" << endl;
        cout << "Path: " << result->first << endl;
    } else {
    path.push(result->first);
    finalDist = distances[result->first] + finalDist;

    long long search = pre[result->first];

    path.push(search);
  while (search != -1) {
        search = pre[search];
        if (search != -1) {
            path.push(search);
        }
      }
      cout << "Distance to dest: " << finalDist << " miles" << endl;
      cout << "Path: ";
          while (!path.empty()) {
        cout << path.top();
        path.pop();
        if (!path.empty()) {
        cout << "->";
        }
    }
    }
    }
    cout << endl;
    cout << "Enter start (partial name or abbreviation), or #> ";
    getline(cin, startBuilding);
  }
  //
  // done:
  //
  cout << "** Done **" << endl;
  return 0;
}
