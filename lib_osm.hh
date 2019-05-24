#ifndef LIB_OSM_HH
#define LIB_OSM_HH

#include <vector>
#include <string>

/*
OpenStreetMap data structures
Elements:
Elements are the basic components of OpenStreetMap's conceptual data model of the physical world.
They consist of nodes (defining points in space), ways (defining linear features and area boundaries), and
relations (which are sometimes used to explain how other elements work together).
All of the above can have one or more associated tags (which describe the meaning of a particular element).

Nodes:
A node is one of the core elements in the OpenStreetMap data model.
It consists of a single point in space defined by its latitude, longitude and node id.

Way:
A way is an ordered list of nodes that define a polyline.
Ways are used to represent linear features such as rivers and roads.
Ways can also represent the boundaries of areas (solid polygons) such as buildings or forests.
In this case, the way's first and last node will be the same. This is called a "closed way".

Relation:
A relation is a multi-purpose data structure that documents a relationship between two or more data elements
(nodes, ways, and/or other relations).

Tag
All types of data element (nodes, ways and relations), as well as changesets, can have tags.
Tags describe the meaning of the particular element to which they are attached.
A tag consists of two free format text fields; a 'key' and a 'value'.
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
//osm_node
/////////////////////////////////////////////////////////////////////////////////////////////////////

class osm_node
{
public:
  osm_node(unsigned int id_, double lat_, double lon_) :
    id(id_),
    lat(lat_),
    lon(lon_)
  {}
  unsigned int id;
  double lat;
  double lon;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//osm_node
//a list of nodes
/////////////////////////////////////////////////////////////////////////////////////////////////////

class osm_way
{
public:
  osm_way(unsigned int id_) :
    id(id_)
  {}
  unsigned int id;
  std::vector<osm_node> nd;
};

#endif