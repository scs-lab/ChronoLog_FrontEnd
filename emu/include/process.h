#ifndef __PROCESS_H_
#define __PROCESS_H_

#include "mds.h"
#include "rw.h"
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "client.h"
#include "query_engine.h"

struct thread_arg_p
{
  int tid;
  std::vector<std::string> snames;
  std::vector<int> total_events;
  int nbatches;
};

class emu_process
{

private:
      int numprocs;
      int myrank;
      int numcores;
      data_server_client *dsc;
      read_write_process *rwp;
      ClockSynchronization<ClocksourceCPPStyle> *CM;
      std::string server_addr;
      //metadata_server *MS; 	
      //metadata_client *MC;
      query_engine *QE;
      std::vector<struct thread_arg_p> t_args;
      std::vector<std::thread> dw;
      std::vector<std::thread> qp;
public:

      emu_process(int np,int r,int n) : numprocs(np), myrank(r), numcores(n)
      {
	std::string unit = "microsecond";
	CM = new ClockSynchronization<ClocksourceCPPStyle> (myrank,numprocs,unit);

	int num_cores_rw = std::ceil(0.5*(double)numcores);
	int num_cores_ms = std::ceil(0.5*(double)numcores);

	if(myrank!=0) num_cores_rw = numcores;

	dsc = new data_server_client(numprocs,myrank,5555); 

	rwp = new read_write_process(r,np,CM,num_cores_rw,dsc);
	rwp->bind_functions();
	QE = new query_engine(numprocs,myrank,dsc,rwp);
	int nchars;
	std::vector<char> addr_string;

	t_args.resize(2);
	/*MS = nullptr;
	if(myrank==0)
	{
	   MS = new metadata_server(numprocs,myrank,server_addr,num_cores_ms);
	   MS->bind_functions();
	}	
	MPI_Barrier(MPI_COMM_WORLD);
        MC = new metadata_client(server_addr);*/
      }

      metadata_client *getclientobj()
      {
	return nullptr;//MC;
      }
      void synchronize()
      {
	CM->SynchronizeClocks();
	CM->ComputeErrorInterval();

      }
      std::string & get_serveraddr()
      {
	      return server_addr;
      }
      void prepare_service(std::string &name, event_metadata &em,int maxsize)
      {
	 int max_size_per_proc = maxsize/numprocs;
    	 int rem = maxsize% numprocs;
         if(myrank < rem) max_size_per_proc++;
         rwp->create_write_buffer(name,em,max_size_per_proc);
      }

      void data_streams(struct thread_arg_p *t)
      {
         rwp->spawn_write_streams(t->snames,t->total_events,t->nbatches);
      }

      void data_streams_s(std::vector<std::string> &snames, std::vector<int> &total_events, int &nbatches)
      {

	std::function<void(struct thread_arg_p *)> DataT(
        std::bind(&emu_process::data_streams,this, std::placeholders::_1));
	
	dw.resize(1);
	t_args[0].tid = 0;
	t_args[0].snames.assign(snames.begin(),snames.end());
	t_args[0].total_events.assign(total_events.begin(),total_events.end());
	t_args[0].nbatches = nbatches;

	std::thread t_d{DataT,&t_args[0]};
	dw[0] = std::move(t_d);

      }

      void spawn_post_processing();
      void process_queries(struct thread_arg_p *t)
      {

	   for(int n=0;n<5;n++)
	   {

	   usleep(20000*10);
	  
	   for(int i=0;i<10;i++)
	   {
	       uint64_t ts = CM->Timestamp();
	       //QE->send_query(t->snames[0]);
	       QE->query_point(t->snames[0],ts);
	   }

	   usleep(20000*10*128);
	   }

	   /*usleep(10*128*20000);
	   QE->send_query(t->snames[0]);
	   usleep(20000*128);
	   QE->send_query(t->snames[1]);
	   usleep(20000*128);
	   QE->send_query(t->snames[2]);
	   usleep(20000*128);
	   QE->send_query(t->snames[3]);*/
      }

      void generate_queries(std::vector<std::string> &snames)
      {
	 std::function<void(struct thread_arg_p *)> QT(
         std::bind(&emu_process::process_queries,this, std::placeholders::_1));

	 qp.resize(1);
	 t_args[1].snames.assign(snames.begin(),snames.end());
	 std::thread qt{QT,&t_args[1]};
	 qp[0] = std::move(qt);

      }

      void end_sessions()
      {
	for(int i=0;i<dw.size();i++) dw[i].join();
	for(int i=0;i<qp.size();i++) qp[i].join();
	rwp->end_sessions();
	QE->end_sessions();
      }
      ~emu_process()
      {
	/*if(MS != nullptr) delete MS;
	delete MC;*/
	delete QE;
	delete rwp;
	delete CM;
	delete dsc;
      }

};

#endif
