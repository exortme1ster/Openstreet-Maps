// graph.h <Starter Code>
// < Nikita Mashchenko >
//
// Basic graph class using adjacency matrix representation.  Currently
// limited to a graph with at most 100 vertices.
//
// University of Illinois at Chicago
// CS 251: Spring 2021
// Project #7 - Openstreet Maps
//

#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
#include <map>
#include<algorithm>

using namespace std;

template<typename VertexT, typename WeightT>
class graph {
 private:
    // set of verticies
    set<VertexT> Vertices;

    // map that works like VertexT - > VertexT (WeightT)
    map<VertexT, map <VertexT, WeightT>> adjL;
    // counter to keep track of size of edges
    int counter;


 public:
  //
  // constructor:
  // since we are using map and set we don't have to allocate anything
  // except of initializing counter to 0
    graph() {
        counter = 0;
    }

  //
  // NumVertices
  //
  // Returns the # of vertices currently in the graph.
  //
  int NumVertices() const {
    return static_cast<int>(this->Vertices.size());
  }

  //
  // NumEdges
  //
  // Returns the # of edges currently in the graph.
  //
  int NumEdges() const {
    return counter;
  }

  //
  // addVertex
  //
  // Adds the vertex v to the graph.  If vertex already
  // exists in the graph, then false is returned.
  //
  bool addVertex(VertexT v) {
    //
    // is the vertex already in the graph?  If so, we do not
    // insert again otherwise Vertices may fill with duplicates:
    //
    if (Vertices.count(v)) {
      return false;
    }

    // everytime we add vertex we add it's position to the map

    //
    // if we get here, vertex does not exist so insert.

    Vertices.insert(v);
    return true;
  }

  //
  // addEdge
  //
  // Adds the edge (from, to, weight) to the graph, and returns
  // true.  If the vertices do not exist false is returned.
  //
  // NOTE: if the edge already exists, the existing edge weight
  // is overwritten with the new edge weight.
  //
  bool addEdge(VertexT from, VertexT to, WeightT weight) {
    if (!Vertices.count(from)) {  // not found:
      return false;
    }


    if (!Vertices.count(to)) {  // not found:
      return false;
    }

    // take the size of adjL[from]
    size_t size = adjL[from].size();
    // insert
    this->adjL[from][to] = weight;
    // check if size is changed and if it is just increase counter by 1;
    if (size != adjL[from].size()) {
        counter++;
    }
    return true;
  }

  //
  // getWeight
  //
  // Returns the weight associated with a given edge.  If
  // the edge exists, the weight is returned via the reference
  // parameter and true is returned.  If the edge does not
  // exist, the weight parameter is unchanged and false is
  // returned.
  //
  bool getWeight(VertexT from, VertexT to, WeightT& weight) {
    if (!Vertices.count(from)) {  // not found:
      return false;
    }

    if (!Vertices.count(to)) {  // not found:
      return false;
    }

    // acess map and check if contains
    if (this->adjL[from].count(to) != 1) {
        return false;
    }
    //
    // Okay, the edge exists, return the weight via the
    // reference parameter:
    //
    weight = this->adjL[from][to];

    return true;
  }
    // _LookupNeighbor function that is used in neighbors
    // to check if v exist in map
    bool _LookupNeighbor(VertexT v) const {
      if (this->adjL.count(v)) {  // already in the graph:
        return true;
    }

    // if get here, not found:
    return false;
  }
  //
  // neighbors
  //
  // Returns a set containing the neighbors of v, i.e. all
  // vertices that can be reached from v along one edge.
  // Since a set is returned, the neighbors are returned in
  // sorted order; use foreach to iterate through the set.
  //
  set<VertexT> neighbors(VertexT v) const {
    set<VertexT>  S;

    int start = _LookupNeighbor(v);

    if (!start) {  // not found:
      return S;
    }
    // access map adjL by using .at(v) and insert VertexT
    for (auto &e : adjL.at(v)) {
        S.insert(e.first);
      }

    return S;
  }
  //
  // getVertices
  //
  // Returns a vector containing all the vertices currently in
  // the graph.
  //
  vector<VertexT> getVertices() const {
    vector<VertexT> VerticesV;
    // transfer all data from set to vector
    copy(Vertices.begin(),
            Vertices.end(),
            inserter(VerticesV, VerticesV.begin()) );
    return VerticesV;
  }

  //
  // dump
  //
  // Dumps the internal state of the graph for debugging purposes.
  //
  void dump(ostream& output) const {
    output << "***************************************************" << endl;
    output << "********************* GRAPH ***********************" << endl;

    output << "**Num vertices: " << this->NumVertices() << endl;
    output << "**Num edges: " << this->NumEdges() << endl;

     output << endl;
     output << "**Edges:" << endl;
          for (auto &e : adjL) {
        cout << "(" << e.first << ",";
      for (auto &f : e.second) {
          cout << f.first << "," << f.second << ")";
      }
          }
       output << endl;
    output << "**************************************************" << endl;
  }
};
