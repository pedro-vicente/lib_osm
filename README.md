# lib_osm
c++ library to read the OpenStreetMap (OSM) format 

![alt text](https://user-images.githubusercontent.com/6119070/58339637-0766c000-7e18-11e9-83c3-0c82305c348e.png "logo")

https://wiki.openstreetmap.org/wiki/Main_Page


## XML parsing by libhpxml

libhpxml is a high performance XML stream parser library written in C with a simple API. 
It is intended to parse large XML files very speed and memory efficiently.

https://www.abenteuerland.at/libhpxml/

## OpenStreetMap data structures

### Elements

Elements are the basic components of OpenStreetMap's conceptual data model of the physical world. They consist of

    nodes (defining points in space),
    ways (defining linear features and area boundaries), and
    relations (which are sometimes used to explain how other elements work together).

All of the above can have one or more associated tags (which describe the meaning of a particular element). 

A node is one of the core elements in the OpenStreetMap data model. It consists of a single point in space defined by its latitude, longitude and node id. 

A way is an ordered list of between 2 and 2,000 nodes that define a polyline. Ways are used to represent linear features such as rivers and roads.

Ways can also represent the boundaries of areas (solid polygons) such as buildings or forests. 
In this case, the way's first and last node will be the same. This is called a "closed way".

A relation is a multi-purpose data structure that documents a relationship between two or more data elements (nodes, ways, and/or other relations).

### Tags

All types of data element (nodes, ways and relations), as well as changesets, can have tags. 
Tags describe the meaning of the particular element to which they are attached.
A tag consists of two free format text fields; a 'key' and a 'value'.

## Data structures

A node constains an ID and a location. A way is a list of nodes.

```c++
class osm_node
{
public:
  osm_node() {};
  unsigned int id;
  double lat;
  double lon;
};

class osm_way
{
public:
  osm_way(){}
  unsigned int id;
  std::vector<osm_node> nd;
};
```

## Storing nodes

Assumption: the XML data defines all the nodes before the ways.
  1) stream read nodes into a temporary list
  2) every way is a list of nd items, each of which is a backreference to a node. 
  Traverse all nd items and check if it exists in the temporary node list, store in way if it does


```xml
<node id='453966480' lat='34.07234' lon='-118.7343501' />
<node id='453966482' lat='34.0670965' lon='-118.7322253' />
<way id='38407529'>
    <nd ref='453966480' />
    <nd ref='453966482' />
</way>
```

## Conversion to geoJSON

<nodes> are converted to geoJSON "points", <ways> to "polygons"

```json
{
"type": "FeatureCollection",
"features": [
  {
  "type": "Feature",
  "geometry": {
  "type": "Polygon",
  "coordinates": [[  [-118.734 , 34.0723],  [-118.732 , 34.0722] ]]
  }
  }
]
}
```