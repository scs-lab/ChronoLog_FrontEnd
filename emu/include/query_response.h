#ifndef __QUERY_RESP_H_
#define __QUERY_RESP_H_

#include <string>
#include "event.h"



struct query_resp
{
   int id;
   int response_id;
   uint64_t minkey;
   uint64_t maxkey;
   std::vector<struct event> *response_vector;
   std::string output_file;
   bool complete;
};

#endif