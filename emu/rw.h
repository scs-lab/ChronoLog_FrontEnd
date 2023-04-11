#ifndef __RW_H_
#define __RW_H_

#include <abt.h>
#include <mpi.h>
#include "ClockSync.h"
#include "hdf5.h"
#include <boost/container_hash/hash.hpp>
#include "write_buffer.h"
#include "distributed_sort.h"
#include "data_server_client.h"
#include "event_metadata.h"

class read_write_process
{

private:
      ClockSynchronization<ClocksourceCPPStyle> *CM;
      int myrank;
      int numprocs;
      int numcores;
      boost::hash<uint64_t> hasher;
      uint64_t seed = 1;
      databuffers *dm;
      std::set<std::string> file_names;
      std::unordered_map<std::string,std::pair<int,event_metadata>> write_names;
      std::unordered_map<std::string,std::pair<int,event_metadata>> read_names;
      std::unordered_map<std::string,std::pair<uint64_t,uint64_t>> write_interval;
      std::unordered_map<std::string,std::pair<uint64_t,uint64_t>> read_interval;
      std::unordered_map<std::string,std::pair<uint64_t,uint64_t>> file_minmax;
      std::vector<std::vector<struct event>*> myevents;
      std::vector<std::vector<struct event>> readevents;
      dsort *ds;
      data_server_client *dsc;
public:
	read_write_process(int r,int np,ClockSynchronization<ClocksourceCPPStyle> *C,int n) : myrank(r), numprocs(np), numcores(n)
	{
           H5open();
           std::string unit = "microsecond";
	   CM = C;
	   dsc = new data_server_client(numprocs,myrank);
	   tl::engine *t_server = dsc->get_thallium_server();
	   tl::engine *t_server_shm = dsc->get_thallium_shm_server();
	   tl::engine *t_client = dsc->get_thallium_client();
	   tl::engine *t_client_shm = dsc->get_thallium_shm_client();
	   std::vector<tl::endpoint> server_addrs = dsc->get_serveraddrs();
	   std::vector<std::string> ipaddrs = dsc->get_ipaddrs();
	   std::vector<std::string> shmaddrs = dsc->get_shm_addrs();
	   dm = new databuffers(numprocs,myrank,numcores,CM);
	   dm->server_client_addrs(t_server,t_client,t_server_shm,t_client_shm,ipaddrs,shmaddrs,server_addrs);
	   ds = new dsort(numprocs,myrank);
	}
	~read_write_process()
	{
	   delete dm;
	   delete ds;
	   delete dsc;
	   H5close();

	}

	void create_write_buffer(std::string &s,event_metadata &em)
	{
          
	    if(write_names.find(s)==write_names.end())
	    {
	      std::vector<struct event> *ev = nullptr;
	      myevents.push_back(ev);
	      std::pair<int,event_metadata> p1(myevents.size()-1,em);
	      std::pair<std::string,std::pair<int,event_metadata>> p2(s,p1);
	      write_names.insert(p2);
	      dm->create_write_buffer();
	      ds->create_sort_buffer();
	    }
	}	
	void create_read_buffer(std::string &s,event_metadata &em)
	{
	    auto r = read_names.find(s);;
	    if(r==read_names.end())
	    {
	        std::vector<struct event> ev;	    
		readevents.push_back(ev);
		std::pair<int,event_metadata> p1(readevents.size()-1,em);
		std::pair<std::string,std::pair<int,event_metadata>> p2(s,p1);
		read_names.insert(p2);
	    }	

	}
	void get_events_from_map(std::string &s)
	{
           auto r = write_names.find(s);
	   int index = (r->second).first;
	   myevents[index] = dm->get_write_buffer(index);
	}
	
	void sort_events(std::string &s)
	{
	    auto r = write_names.find(s);
	    int index = (r->second).first;
	    get_events_from_map(s);
	    ds->get_unsorted_data(myevents[index],index);
	    uint64_t min_v,max_v;
	    ds->sort_data(index,min_v,max_v);
	    auto r1 = write_interval.find(s);
	    if(r1 == write_interval.end())
	    {
		std::pair<uint64_t,uint64_t> p(min_v,max_v);
		std::pair<std::string,std::pair<uint64_t,uint64_t>> q(s,p);
		write_interval.insert(q);
	    }
	    else 
	    {
		(r1->second).first = min_v;
		(r1->second).second = max_v;
	    }
	}
	event_metadata & get_metadata(std::string &s)
	{
		auto r = write_names.find(s);
		return (r->second).second;
	}
	bool get_range_in_file(std::string &s, uint64_t &min_v,uint64_t &max_v)
	{
	   min_v = UINT64_MAX; max_v = 0;
	   bool err = false;

	   auto r = std::find(file_names.begin(),file_names.end(),s);
	   if(r != file_names.end())
	   {





	   }

	}

        bool get_range_in_read_buffers(std::string &s,uint64_t &min_v,uint64_t &max_v)
	{
	    min_v = UINT64_MAX; max_v = 0;
	    bool err = false;
	    auto r = read_interval.find(s);
	    if(r != read_interval.end())
	    {
		min_v = (min_v < (r->second).first) ? min_v : (r->second).first;
		max_v = (max_v > (r->second).second) ? max_v : (r->second).second;
		err = true;
	    }
	    return err;
	}

	bool get_range_in_write_buffers(std::string &s,uint64_t &min_v,uint64_t &max_v)
	{
	    min_v = UINT64_MAX; max_v = 0;
	    bool err = false;
	    auto r = write_interval.find(s);
	    if(r != write_interval.end())
	    {
		min_v = (min_v < (r->second).first) ? min_v : (r->second).first;
		max_v = (max_v > (r->second).second) ? max_v : (r->second).second;
		err = true;
	    }
	    return err;
	}

	int num_write_events(std::string &s)
	{
		auto r = write_names.find(s);
		int index = (r->second).first;
		return myevents[index]->size();
	}
	int dropped_events()
	{
	    return dm->num_dropped_events();
	}
        bool get_events_in_range_from_read_buffers(std::string &s,std::pair<uint64_t,uint64_t> &range,std::vector<struct event> &oup)
	{
	     uint64_t min = range.first; uint64_t max = range.second;
	     bool err = false;
	     auto r = read_interval.find(s);
	     if(r != read_interval.end())
	     {
		uint64_t min_s = (r->second).first;
		uint64_t max_s = (r->second).second;
		if(!((max < min_s) && (min > max_s)))
		{
		   min_s = std::max(min_s,min);
		   max_s = std::min(max_s,max);
		   
		   auto r1 = read_names.find(s);
		   int index = (r1->second).first;
		
		   for(int i=0;i<readevents[index].size();i++)
		   {
			uint64_t ts = readevents[index][i].ts;
	     	        if(ts >= min_s && ts <= max_s) oup.push_back(readevents[index][i]);		
		   }	  
		   err = true; 
		}
	     }
	     return err;
	}

	bool get_events_in_range_from_write_buffers(std::string &s,std::pair<uint64_t,uint64_t> &range,std::vector<struct event> &oup)
	{
	     bool err = false;
	     uint64_t min = range.first; uint64_t max = range.second;
	     auto r = write_interval.find(s);
	     if(r != write_interval.end())
	     {
		uint64_t min_s = (r->second).first;
		uint64_t max_s = (r->second).second;
		if(!((min > max_s) && (max < min_s)))
		{	
		   min_s = std::max(min,min_s);
		   max_s = std::min(max,max_s);
	           auto r1 = write_names.find(s);
		   int index = (r1->second).first;
		   for(int i=0;i<myevents[index]->size();i++)
		   {
		     uint64_t ts = (*myevents[index])[i].ts;
		     if(ts >= min && ts <= max) oup.push_back((*myevents[index])[i]);
		   }
	           err = true;	   
	        }
	     }
	     return err;
	}
	void create_events(int num_events,std::string &s);
	void clear_events(std::string &s);
	void get_range(std::string &s);
	void pwrite(const char *,std::string &s);
        void pwrite_new(const char *,std::string &s);
	void pwrite_extend(const char*,std::string &s);
	void preaddata(const char*,std::string &s);
	void preadfileattr(const char*);
};

#endif
