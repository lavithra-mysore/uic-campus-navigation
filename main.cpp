/*main.cpp*/

//
// Lavithra Mysore
// U. of Illinois, Chicago
// CS 251: Spring 2020
// Project #07: open street maps, graphs, and Dijkstra's alg
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
#include <stack> 

#include "tinyxml2.h"
#include "dist.h"
#include "osm.h"
#include "graph.h"

using namespace std;
using namespace tinyxml2;

class prioritize
{
   public:
   bool operator()(const pair<long long,double>& p1, const pair<long long,double>& p2) const
   {
       // it sorts the pairs in vector
      if(p1.second>p2.second){// sort from min to max
         return true;
      }
      else if(p1.second == p2.second){
         // if same then prefrence to previous
         if(p1.first>p2.first){
            return true;
            }
            else{
               return false;
               }
         }
         
      else{
         return false;
      }
      
      }
   
};
void Dijkstra(Coordinates& StartCoords,map<long long, double>& ShortDist,map<long long,long long>& pred,graph<long long, double>& G){
    
      // Run Dijkstra’s algorithm from the start node
      const double INF = numeric_limits<double>::max();
      priority_queue< pair<long long,double>,vector<pair<long long,double>>,prioritize> uq;// priority_queue of Unvisited Queue
              
      set<long long> visitedSet;
      vector<long long> vertices=G.getVertices();
      
      for(auto it:vertices){// initalize all vertices to INF and previous to -1
          ShortDist[it]=INF;
          pred[it]=-1;
          uq.push(make_pair(it,INF));
      }
    
      ShortDist[StartCoords.ID]=0;
      pred[StartCoords.ID]=0;
      uq.push(make_pair(StartCoords.ID,0));//push currentV to priority_queue
      
       while(!uq.empty()){ // loop until priority_queue is empty
           
          long long currentV= uq.top().first;
          uq.pop();
           
          if(ShortDist[currentV]==INF){ //return if distances are inf
              break;
           }
          else if(visitedSet.count(currentV)==1){ //return if already visited
              continue;
          }
          else{    // else push back to visitedSet
              visitedSet.insert(currentV);
           }
           
          set<long long> N = G.neighbors(currentV);
          for(auto v:N){
              
              double edgeWeight=0;
              G.getWeight(currentV, v, edgeWeight);
              double alternativePathDistance = ShortDist[currentV] + edgeWeight;
              
              if(alternativePathDistance < ShortDist[v]){ //checks if there is a small distance
                  
                ShortDist[v] = alternativePathDistance; // if there is, we update the ShortdIST map and pred map
                pred[v]= currentV;
                uq.push(make_pair(v,alternativePathDistance)); // AND push in the queue
           
           }
          }
       
       }
    
}
void shortDistance(map<long long, double>& ShortDist,map<long long,long long>& pred,Coordinates& StartCoords,Coordinates& EndCoords){
    //Output the distance and path from start node to destination node. If no path exists,output “Sorry, destination unreachable”
    long long Pred=EndCoords.ID;
    cout<<"Navigating with Dijkstra...\n";
    
    if(pred[Pred]== -1){ // if prev is -1 it means its unreachable
        cout<<"Sorry, destination unreachable\n";
        return;
    }
        
    stack<long long> path;
    while(Pred != StartCoords.ID){ // looping through map, going to prev elements until startCoords.ID
        path.push(Pred);
        Pred=pred[Pred];    
    }
    
    cout<<"Distance to dest: "<<ShortDist[EndCoords.ID]<<" miles"<<endl;
    cout<<"Path: "<<StartCoords.ID;
    
    while(!path.empty()){ // looping and printing the content of stack
        cout<<"->"<<path.top();
        path.pop();
    }
    
}

bool BuildingFound(string startBuilding,vector<BuildingInfo>& Buildings,BuildingInfo& StartCoords){ // checks if input is correct                             
      for(auto Buldng : Buildings){// it first searches with abrev in Buildings
          if(Buldng.Abbrev==startBuilding){
              StartCoords=Buldng;
              return true;     
          }  
      }
        for(auto Buldng : Buildings){// if not found then in searches with Fullname in Buildings
            if(Buldng.Fullname.find(startBuilding)!=string::npos){
                StartCoords=Buldng;
                return true;
            }
        }
    return false;   
}

 void buildingInfo(string msg,BuildingInfo& StartCoords){// prints the info of building like ID and its coords
    
    cout<<msg<<"\n";
    cout<<" "<<StartCoords.Fullname<<"\n"
        <<" ("<<StartCoords.Coords.Lat<<", "<<StartCoords.Coords.Lon<<")\n";
}

Coordinates FindMin(map<long long, Coordinates>&  Nodes,BuildingInfo Build,vector<FootwayInfo>& Footways){
    Coordinates Coords; 
    int count=0;
    double min;
    
    for(auto& node:Footways){
        
        for(auto& foot:node.Nodes){
            double lat1= Build.Coords.Lat;
            double long1= Build.Coords.Lon; 
            double lat2= Nodes.at(foot).Lat;
            double long2= Nodes.at(foot).Lon;      

            double dist = distBetween2Points(lat1,long1,lat2,long2); //calculates the dist b/w node and building
            if((count==0)||(dist<min)){ // sets a inital value and if there is value less than that than it replaces it
                min=dist;
                Coords=Nodes.at(foot);
            }
            count++;
        }
    }
    return Coords;
}
void NodesInfo(Coordinates Coords,string msg){// prints nearest Node to building info
    cout<<msg<<endl;
    cout<<" "<<Coords.ID<<endl;
    cout<<" ("<<Coords.Lat<<", "<<Coords.Lon<<") \n";
}

int main()
{
  map<long long, Coordinates>  Nodes;     // maps a Node ID to it's coordinates (lat, lon)
  vector<FootwayInfo>          Footways;  // info about each footway, in no particular order
  vector<BuildingInfo>         Buildings; // info about each building, in no particular order
  XMLDocument                  xmldoc;
  
  cout << "** Navigating UIC open street map **" << endl;
  cout << endl;
  cout << std::setprecision(8);

  string def_filename = "map.osm";
  string filename;

  cout << "Enter map filename> ";
  getline(cin, filename);

  if (filename == "")
  {
    filename = def_filename;
  }

  //
  // Load XML-based map file 
  //
  if (!LoadOpenStreetMap(filename, xmldoc))
  {
    cout << "**Error: unable to load open street map." << endl;
    cout << endl;
    return 0;
  }
  
  //
  // Read the nodes, which are the various known positions on the map:
  //
  size_t nodeCount = ReadMapNodes(xmldoc, Nodes);

  //
  // Read the footways, which are the walking paths:
  //
  size_t footwayCount = ReadFootways(xmldoc, Footways);

  //
  // Read the university buildings:
  //
  size_t buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);

  //
  // Stats
  //
  assert(nodeCount == Nodes.size()); 
  assert(footwayCount == Footways.size());
  assert(buildingCount == Buildings.size());

  cout << endl;
  cout << "# of nodes: " << Nodes.size() << endl;
  cout << "# of footways: " << Footways.size() << endl;
  cout << "# of buildings: " << Buildings.size() << endl;


  //
  // TODO: build the graph, output stats:
  //
 // 5. Add nodes as vertices.Add each node to the graph.
  graph<long long, double> G;
    
  for(auto& vertex:Nodes){
      G.addVertex(vertex.first);
  }
  
    for(auto& path : Footways){
        for(size_t i=0;i<(path.Nodes).size()-1;i++){       
            double lat1=(Nodes.at(path.Nodes[i])).Lat;
            double long1=(Nodes.at(path.Nodes[i])).Lon;
            
            double lat2=(Nodes.at(path.Nodes[i+1])).Lat;
            double long2=(Nodes.at(path.Nodes[i+1])).Lon;   
            double dist = distBetween2Points(lat1,long1,lat2,long2);
            
            G.addEdge(path.Nodes[i],path.Nodes[i+1],dist);
            G.addEdge(path.Nodes[i+1],path.Nodes[i],dist);
        }
        
    }

  cout << "# of vertices: " << G.NumVertices() << endl;
  cout << "# of edges: " << G.NumEdges() << endl;
  cout << endl;

  //
  // Navigation from building to building
  //
  string startBuilding, destBuilding;
    
  BuildingInfo StartBuild, EndBuild;
  Coordinates StartCoords, EndCoords;
  
  cout << "Enter start (partial name or abbreviation), or #> ";
  getline(cin, startBuilding);
  

  while (startBuilding != "#")
  {
      cout << "Enter destination (partial name or abbreviation)> ";
      getline(cin, destBuilding);
      
      bool check=false;
      
      while(check==false){ // it takes in input until correct input is given
          
          if((BuildingFound(startBuilding,Buildings,StartBuild)==true)&&(BuildingFound(destBuilding,Buildings,EndBuild)==true)){
              check=true;
              break;
          }
          if(BuildingFound(startBuilding,Buildings,StartBuild)==false){
              cout<<"Start building not found\n";
          }
         else if(BuildingFound(destBuilding,Buildings,EndBuild)==false){
              cout<<"Destination building not found\n";
          }
          cout << "Enter start (partial name or abbreviation), or #> ";
          getline(cin, startBuilding);
          
          cout << "Enter destination (partial name or abbreviation)> ";
          getline(cin, destBuilding);
      }
      
    buildingInfo("Starting Point:",StartBuild);//prints building info
    buildingInfo("Destination Point:",EndBuild);
    
      
    StartCoords= FindMin(Nodes,StartBuild,Footways);  // lookup buildings, find nearest start and dest nodes,
    EndCoords= FindMin(Nodes,EndBuild,Footways);
    
    cout<<endl;
      
    NodesInfo(StartCoords,"Nearest start node:"); // prints nearest node info
    NodesInfo(EndCoords,"Nearest destination node:");
      
    //
    // run Dijkstra's alg, output distance and path to destination:
    //
    map<long long, double> ShortDist;
    map<long long,long long> pred;
      
    cout<<endl;
      
    Dijkstra(StartCoords,ShortDist,pred,G); // looks for shortest path
    shortDistance(ShortDist,pred,StartCoords,EndCoords); //prints out the shortest path
      
    //
    // another navigation?
    //
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
