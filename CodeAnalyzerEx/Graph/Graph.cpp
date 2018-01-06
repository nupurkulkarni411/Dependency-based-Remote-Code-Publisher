///////////////////////////////////////////////////////////////
// Graph.cpp - Graph Library                                 //
// Ver 1.6                                                   //
// Language:    Visual C++ 2012                              //
// Platform:    Lenova ThinkPad E420s, Windows 7             //
// Application: Type-based Dependency Analysis,              //
//              CSE687 - Object Oriented Design              //
// Author:      Nupur Kulkarni, Syracuse University,         //
//              nvkulkar@syr.edu                             //
// Source:      Jim Fawcett, CST 4-187, Syracuse University  //
//              (315) 443-3948, jfawcett@twcny.rr.com        //
///////////////////////////////////////////////////////////////
/*

* Build Process:
* --------------
* Required Files: DepAnal.h, FileSystem.h, Parser.h, NoSqlDb.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 01 March 2017
* - first release of new design
*/


#include <iostream>
#include <fstream>
#include "Graph.h"
using namespace CodeAnalysis;

typedef Graph<std::string, std::string> graph;
typedef Vertex<std::string, std::string> vertex;
typedef Display<std::string, std::string> display;

void showVert(Vertex<std::string, std::string>& v)
{
  std::cout << "\n  " << v.id();
}

template<typename V, typename E>
void TshowVert(Vertex<V,E>& v)
{
  std::cout << "\n  " << v.id();
}

#ifdef TEST_GRAPH

int main()
{
  std::cout << "\n  Testing Graph Library";
  try{
    std::cout << "\n  Constructing Graph instance";
    graph g;
    vertex v1("v1");vertex v2("v2");vertex v3("v3");vertex v4("v4");vertex v5("v5", 50);
    g.addVertex(v2);g.addVertex(v1);g.addVertex(v3);g.addVertex(v4);
    g.addVertex(v5);g.addEdge("e1",v1,v2);g.addEdge("e2",v1,v3);g.addEdge("e3",v2,v3);
    g.addEdge("e4",v4,v3);g.addEdge("e5",v5,v2);
    display::show(g);
    std::cout << "\n  Making copy of instance";
    graph gcopy = g;
    display::show(gcopy);
    std::cout << "\n  Modifying copy's values";
    for(auto& v : gcopy)
      v.value() += "copy";
    display::show(gcopy);
    std::cout << "\n  Assigning original instance to copy";
    gcopy = g;
    display::show(gcopy);
    std::cout << "\n  Vertices with no Parents:";
    std::vector< vertex > verts = display::vertsWithNoParents(g);
    for(size_t i=0; i<verts.size(); ++i)
      std::cout << verts[i].value().c_str() << " ";
    std::cout << "\n";
    std::cout << "\n  Testing Depth First Search with function pointer";
    for(auto& vert : g){
      std::cout << "\n  starting at id " << vert.id();
      g.dfs(vert, TshowVert<std::string, std::string>);}
    std::cout << "\n  Testing Depth First Search with Functor";
    class showFunctor{
    public:
      void operator()(Vertex<std::string, std::string>& vert){
        std::cout << "\n  From functor: vertix id = " << vert.id();
        std::cout << ", number of edges = " << vert.size();}
    };
    g.dfs(g[0], showFunctor());
    std::cout << "\n  Testing Serialization to XML";
    std::string str = GraphToXmlString(g);
    std::ofstream out("testGraph.xml");
    std::cout << "\n  Testing Graph construction from XML";
    graph gtest;
    GraphFromXmlString(gtest, str);
    display::show(gtest);
  }
  catch(std::exception& ex){
    std::cout << "\n\n  " << ex.what() << "\n\n";}
  return 0;
}

#endif

