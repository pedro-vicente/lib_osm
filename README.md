# lib_osm
c++ library to read the OpenStreetMap (OSM) format 

![alt text](https://user-images.githubusercontent.com/6119070/58339637-0766c000-7e18-11e9-83c3-0c82305c348e.png "logo")

https://wiki.openstreetmap.org/wiki/Main_Page


## XML parsing by libhpxml

libhpxml is a high performance XML stream parser library written in C with a simple API. 
It is intended to parse large XML files very speed and memory efficiently.
While parsing an XML file libhpxml returns pointers to the elements and attributes. 
C strings are usually '\0'-terminated but this is not applicable here because it would require that '\0' characters are inserted after each element, 
resulting in huge data movement. 
Thus, libhpxml uses "B strings" which are hold in the bstring_t structure. 
The structure contains a pointer to the string and its length.

```c
typedef struct bstring
{
  int len;
  char *buf;
} bstring_t;
```

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
