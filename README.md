# lib_osm
c++ library to read the OpenStreetMap (OSM) format 

https://wiki.openstreetmap.org/wiki/Main_Page


## XML parsing by libhpxml
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
