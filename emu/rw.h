#ifndef __RW_H_
#define __RW_H_

#include <abt.h>
#include <mpi.h>
#include "ClockSync.h"
#include "hdf5.h"
#include <boost/container_hash/hash.hpp>
#include "data_buffer.h"
#include "distributed_sort.h"
#include "data_server_client.h"

class read_write_process
{

private:
      ClockSynchronization<ClocksourceCPPStyle> *CM;
      int myrank;
      int numprocs;
      int numcores;
      boost::hash<uint64_t> hasher;
      uint64_t seed = 1;
      databuffer *dm;
      std::vector<struct event> myevents;
      std::vector<struct event> readevents;
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
           std::vector<tl::endpoint> serveraddrs = dsc->get_serveraddrs();
	   std::vector<std::string> ipaddrs = dsc->get_ipaddrs();
	   std::vector<std::string> shmaddrs = dsc->get_shm_addrs();
	   dm = new databuffer(numprocs,myrank,numcores,CM);
	   dm->server_client_addrs(t_server,t_client,t_server_shm,t_client_shm,serveraddrs,ipaddrs,shmaddrs);
	   ds = new dsort(numprocs,myrank);
	}
	~read_write_process()
	{
	   delete dm;
	   delete ds;
	   delete dsc;
	   H5close();

	}
	
	void get_events_from_map()
	{
	   myevents = dm->get_buffer();
	}
	std::vector<struct event> & get_events()
	{
		return myevents;
	}
	void sort_events()
	{
	    get_events_from_map();
	    ds->get_unsorted_data(myevents);
	    ds->sort_data(); 
	    ds->get_sorted_data(myevents); 
	}

	int num_events()
	{
		return myevents.size();
	}
	int dropped_events()
	{
	    return dm->num_dropped_events();
	}
	void create_events(int num_events);
        void pwrite(const char *);
	void pread(const char*);
};

#endif
