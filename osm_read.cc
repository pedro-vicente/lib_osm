#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
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


/////////////////////////////////////////////////////////////////////////////////////////////////////
//utility libhpxml functions
/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
//hpx_printf_attr
/////////////////////////////////////////////////////////////////////////////////////////////////////

void hpx_printf_attr(const hpx_attr_t *a)
{
  printf("%.*s=%c%.*s%c ", a->name.len, a->name.buf, a->delim, a->value.len, a->value.buf, a->delim);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//hpx_attr_value
//libhpxml uses "B strings" which are hold in the bstring_t structure. 
//The structure contains a pointer to the string and its length. 
/////////////////////////////////////////////////////////////////////////////////////////////////////

bstring_t hpx_attr_value(hpx_tag_t* tag, const char* name)
{
  for (int idx = 0; idx < tag->nattr; idx++)
  {
    hpx_attr_t *a = &tag->attr[idx];
    char* tmp = (char*)malloc(a->name.len);
    memcpy(tmp, a->name.buf, a->name.len);
    if (strncmp(tmp, name, a->name.len) == 0)
    {
      free(tmp);
      return a->value;
    }
    free(tmp);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//hpx_bstring_atof
//return double from B string
/////////////////////////////////////////////////////////////////////////////////////////////////////

double hpx_bstring_atof(bstring_t bs)
{
  char* tmp = (char*)malloc(bs.len);
  memcpy(tmp, bs.buf, bs.len);
  double d = atof((char*)tmp);
  free(tmp);
  return d;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//hpx_bstring_atof
//return double from B string
/////////////////////////////////////////////////////////////////////////////////////////////////////

int hpx_bstring_atoi(bstring_t bs)
{
  char* tmp = (char*)malloc(bs.len);
  memcpy(tmp, bs.buf, bs.len);
  int n = atoi((char*)tmp);
  free(tmp);
  return n;
}

std::map<unsigned int, osm_node> tmp_nodes;//temporary list of nodes before inserting in a <way>

/////////////////////////////////////////////////////////////////////////////////////////////////////
//read 'id', 'lat' and 'lon' attributes from a <node> tag 
//and insert as a new node in temporary list
/////////////////////////////////////////////////////////////////////////////////////////////////////

void store_node(hpx_tag_t* tag)
{
  bstring_t id_ = hpx_attr_value(tag, "id");
  bstring_t lat = hpx_attr_value(tag, "lat");
  bstring_t lon = hpx_attr_value(tag, "lon");
  unsigned int id = hpx_bstring_atoi(id_);
  osm_node node(id, hpx_bstring_atof(lat), hpx_bstring_atof(lon));
  printf("id=%d lat=%f lon=%f \n", id, hpx_bstring_atof(lat), hpx_bstring_atof(lon));
  tmp_nodes[id] = node;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//main
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
  hpx_ctrl_t* ctl;
  hpx_tag_t* tag;
  bstring_t b;
  long lno;
  int fd;

  osm_way *cway = NULL; //currently processed way
  osm_node *cnode = NULL; //currently processed node
  std::vector<osm_way> ways; //final list of ways
  std::vector<osm_node> nodes; //final list of nodes (standalone)

  //assumption: the XML data defines all the nodes before the ways
  //1) stream read <nodes> into a temporary list
  //2) every <way> is a list of <nd> items, each of which is a backreference to a <node>.
  //traverse all <nd> items and check if it exists in the temporary <node> list,
  //store in <way> if it does

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

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //loop as long as XML elements are available
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  while (hpx_get_elem(ctl, &b, NULL, &lno) > 0)
  {
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //parse XML element
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    if (hpx_process_elem(b, tag) < 0)
    {
      printf("[%ld] ERROR in element: %.*s\n", lno, b.len, b.buf);
      assert(0);
      exit(1);
    }
    printf("[%ld] type=%d, name=%.*s, nattr=%d\n", lno, tag->type, tag->tag.len, tag->tag.buf, tag->nattr);

    std::string tag_name(tag->tag.buf, tag->tag.len); //tag name
    bool in_node = false; //currently processing a node (open tag not closed)
    bool in_way = false; //currently processing a way (open tag not closed)

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //switch tag type 
    //HPX_ILL, HPX_OPEN, HPX_SINGLE, HPX_CLOSE, HPX_LITERAL, HPX_ATT, HPX_INSTR, HPX_COMMENT
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    switch (tag->type)
    {
      /////////////////////////////////////////////////////////////////////////////////////////////////////
      //HPX_OPEN
      /////////////////////////////////////////////////////////////////////////////////////////////////////

    case HPX_OPEN:
      if (tag_name.compare("node") == 0)
      {
        printf("%s open\n", tag_name.c_str());

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //store temporary <node> to add to way
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        store_node(tag);

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //instantiate <node> with id, lat, lon and enter node mode
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        unsigned int id = hpx_bstring_atoi(hpx_attr_value(tag, "id"));
        double lat = hpx_bstring_atof(hpx_attr_value(tag, "lat"));
        double lon = hpx_bstring_atof(hpx_attr_value(tag, "lon"));
        cnode = new osm_node(id, lat, lon);
        in_node = true;
      }
      else if (tag_name.compare("way") == 0)
      {
        printf("%s open\n", tag_name.c_str());

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //get <way> id and enter way mode
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        unsigned int id = hpx_bstring_atoi(hpx_attr_value(tag, "id"));
        cway = new osm_way(id);
        in_way = true;
      }
      break;

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      //HPX_CLOSE
      /////////////////////////////////////////////////////////////////////////////////////////////////////

    case HPX_CLOSE:
      if (tag_name.compare("node") == 0)
      {
        printf("%s close\n", tag_name.c_str());

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //close node mode and store noe in final <node> list, delete temporary storage
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        nodes.push_back(*cnode);
        delete cnode;
        in_node = false;
      }
      else if (tag_name.compare("way") == 0)
      {
        printf("%s close\n", tag_name.c_str());

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //close way mode and store way in final <way> list, delete temporary storage
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        ways.push_back(*cway);
        delete cway;
        tmp_nodes.clear();
        in_way = false;
      }
      break;

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      //HPX_SINGLE
      /////////////////////////////////////////////////////////////////////////////////////////////////////

    case HPX_SINGLE:
      if (tag_name.compare("node") == 0)
      {
        printf("%s single\n", tag_name.c_str());
        assert(in_node == false); //cannot have single XML node while there is an open XML node
        store_node(tag);
      }
      else if (tag_name.compare("nd") == 0)
      {
        unsigned int ref_id = hpx_bstring_atoi(hpx_attr_value(tag, "ref"));
        printf("%s single\n", tag_name.c_str());
        printf("ref=%d\n", ref_id);

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //look for this node in the temporarry list of nodes, insert in <way> if found
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        if (tmp_nodes.find(ref_id) != tmp_nodes.end())
        {
          osm_node node = tmp_nodes[ref_id];
          cway->nd.push_back(node);
        }
      }
      break;
    } //switch
  } //while

  printf("%zd ways %zd nodes\n", ways.size(), nodes.size());

  if (!ctl->eof)
  {
    exit(EXIT_FAILURE);
  }

  hpx_tm_free(tag);
  hpx_free(ctl);
  close(fd);
  exit(EXIT_SUCCESS);
}

