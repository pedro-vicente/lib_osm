#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#endif
extern "C"
{
#include "bstring.h"
#include "libhpxml.h"
}
#include "lib_osm.hh"

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

int main(int argc, char* argv[])
{
  hpx_ctrl_t* ctl;
  hpx_tag_t* tag;
  bstring_t b;
  long lno;
  int fd;

  if ((fd = open("malibu_park.osm", O_RDONLY)) < 0)
  {
    exit(EXIT_FAILURE);
  }

  if ((ctl = hpx_init(fd, 100 * 1024 * 1024)) == NULL)
  {
  }
  if ((tag = hpx_tm_create(16)) == NULL)
  {
  }

  // loop as long as XML elements are available
  while (hpx_get_elem(ctl, &b, NULL, &lno) > 0)
  {
    // parse XML element
    if (!hpx_process_elem(b, tag))
    {
      printf("[%ld] type=%d, name=%.*s, nattr=%d\n", lno, tag->type, tag->tag.len, tag->tag.buf, tag->nattr);
    }
    else
    {
      printf("[%ld] ERROR in element: %.*s\n", lno, b.len, b.buf);
    }
  }

  if (!ctl->eof)
  {
    exit(EXIT_FAILURE);
  }

  hpx_tm_free(tag);
  hpx_free(ctl);
  close(fd);
  exit(EXIT_SUCCESS);
}

