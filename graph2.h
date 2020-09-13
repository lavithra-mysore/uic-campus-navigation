/*graph.h*/

//
// Lavithra Mysore
//
// Basic graph class using adjacency matrix representation.  Currently
// limited to a graph with at most 100 vertices.
//
// U. of Illinois, Chicago
// CS 251: Spring 2020
//

#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <string>

using namespace std;


template<typename VertexT, typename WeightT>
class graph
{
private:
  struct EdgeData
  {
    bool     EdgeExists;
    WeightT  Weight;

    EdgeData()
    {
      EdgeExists = false;  // initially no edge, and no weight
    }
  };

  //
  // We are using adjacency matrix implementation, where rows
  // are the starting vertex and cols are the ending vertex.
  // We keep track of the vertices in the Vertices vector,
  // where the vertex's position in the vector --- 0, 1, 2, 
  // 3, 4, 5, ... --- denotes the row in the adjacency matrix
  // where their edges are found.  Example: if vertex "ORD" is
  // in position 1 of the Vertices vector, then row 1 of
  // AdjList are the edges that start at "ORD" and lead to
  // other vertices.
  //
  
  map<VertexT,map<VertexT, EdgeData>> AdjList;
  vector<VertexT>  Vertices;

  //
  // _LookupVertex
  //
  // Finds the vertex in the Vertices vector and returns it returns
  // 1 position if found, otherwise returns -1.
  //
  int _LookupVertex(VertexT v) const
  {
    if(AdjList.count(v)==1){  // already in the graph:
        return 1;
     }

    // if get here, not found:
    return -1;
  }


public:
  //
  // constructor:
  //
  // Constructs an empty graph where there is the no max # of vertices 
 
  //
  graph()
  {
  }

  //
  // NumVertices
  //
  // Returns the # of vertices currently in the graph.
  //
  int NumVertices() const
  {
    return AdjList.size();
  }

  //
  // NumEdges
  //
  // Returns the # of edges currently in the graph.
  //
  int NumEdges() const
  {
    int count = 0;

    //
    // loop through the adjacency matrix and count how many 
    // edges currently exist:
    //
    for (const auto& kv : AdjList)
    {
      count=count + (kv.second).size();
    }

    return count;
  }

  //
  // addVertex
  //
  // Adds the vertex v to the graph if there's room, and if so
  // returns true.  If the graph is full, or the vertex already
  // exists in the graph, then false is returned.
  //
  bool addVertex(VertexT v)
  {
    //
    // is the vertex already in the graph?  If so, we do not 
    // insert again otherwise Vertices may fill with duplicates:
    //
    if (_LookupVertex(v) >= 0)
      return false;

    //
    // if we get here, vertex does not exist so insert. 
    //
    AdjList[v]={};
    this->Vertices.push_back(v);

    return true;
  }

  //
  // addEdge
  //
  // Adds the edge (from, to, weight) to the graph, and returns
  // true.  If the vertices do not exist or for some reason the
  // graph is full, false is returned.
  //
  // NOTE: if the edge already exists, the existing edge weight
  // is overwritten with the new edge weight.
  //
  bool addEdge(VertexT from, VertexT to, WeightT weight)
  {
    //
    // we need to search the Vertices and find the position 
    // of each vertex; this will denote the row and col to
    // access in the adjacency matrix:
    //
    int row = _LookupVertex(from);

    if (row < 0)  // not found:
      return false;

    int col = _LookupVertex(to);

    if (col < 0)  // not found:
      return false;

    //
    // the vertices exist and  so insert / update the edge:
    // 
    
   (AdjList.at(from))[to] = {};
   ((AdjList.at(from)).at(to)).EdgeExists = true;
   ((AdjList.at(from)).at(to)).Weight = weight;

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
  bool getWeight(VertexT from, VertexT to, WeightT& weight) const
  {
    //
    // we need to search the Vertices and find the position 
    // of each vertex; this will denote the row and col to
    // access in the adjacency matrix:
    //
    int row = _LookupVertex(from);

    if (row < 0)  // not found:
      return false;

    int col = _LookupVertex(to);

    if (col < 0)  // not found:
      return false;

    //
    // the vertices exist, but does the edge exist?
    //
    if ((AdjList.at(from)).count(to)==0){  // no:
      return false;
      }

    //
    // Okay, the edge exists, return the weight via the 
    // reference parameter:
    //
    weight = ((AdjList.at(from)).at(to)).Weight;

    return true;
  }

  //
  // neighbors
  //
  // Returns a set containing the neighbors of v, i.e. all
  // vertices that can be reached from v along one edge.
  // Since a set is returned, the neighbors are returned in
  // sorted order; use foreach to iterate through the set.
  //
  set<VertexT> neighbors(VertexT v) const
  {
    set<VertexT>  S;

    //
    // we need to search the Vertices and find the position 
    // of v, that will be the row we need in the adjacency 
    // matrix:
    //
    int row = _LookupVertex(v);

    if (row < 0)  // not found:
      return S;

    //
    // we loop throug the map of v and add it on vertices
    //
    // NOTE: how many columns are there?  The # of vertices.
    //
    for (const auto& kv : AdjList.at(v))
    {
        VertexT dest = kv.first;  // dest vertex is here:

        S.insert(dest);
    }

    return S;
  }

  //
  // getVertices
  //
  // Returns a vector containing all the vertices currently in
  // the graph.
  //
  vector<VertexT> getVertices() const
  {
    return Vertices;  // returns a copy:
  }

  //
  // dump
  // 
  // Dumps the internal state of the graph for debugging purposes.
  //
  // Example:
  //    graph<string,int>  G;
  //    ...
  //    G.dump(cout);  // dump to console
  //
  void dump(ostream& output) const
  {
    output << "***************************************************" << endl;
    output << "********************* GRAPH ***********************" << endl;

    output << "**Num vertices: " << this->NumVertices() << endl;
    output << "**Num edges: " << this->NumEdges() << endl;

    output << endl;
    output << "**Vertices:" << endl;
    for (int i = 0; i < this->NumVertices(); ++i)
    {
      output << " " << i << ". " << this->Vertices[i] << endl;
    }

    output << endl;
    output << "**Edges:" << endl;
    for (int row = 0; row < this->NumVertices(); ++row)
    {
      output << Vertices[row] << ": ";

      for (int col = 0; col < this->NumVertices(); ++col)
      {
        if (AdjList.at(Vertices[row]).count(Vertices[col]) == 0)
        {
          continue;
        }
        else
        {
          output << "("<<Vertices[row]<<","<<Vertices[col]<<","
            << ((AdjList.at(Vertices[row])).at(Vertices[col])).Weight
            << ") ";
        }
      }

      output << endl;
    }

    output << "**************************************************" << endl;
  }

};
