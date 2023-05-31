#include "external_sort.h"
#include <string>

bool compare_fields(struct event &e1, struct event &e2)
{

   int v1,v2;

   v1 = *(int*)(e1.data);
   v2 = *(int*)(e2.data);

   if(v1 < v2) return true;
   else return false; 
}

std::string hdf5_sort::sort_on_secondary_key(std::string &s1_string,std::string &attr_name,int offset,uint64_t minkey,uint64_t maxkey)
{
   std::string filename1 = "file";
   filename1 += s1_string+".h5";

   std::string filename2 = "file";
   filename2 += s1_string+"secsort";
   filename2 += ".h5";

   std::string s2_string;

    hid_t       fid;
    hid_t       acc_tpl;
    hid_t       xfer_plist;
    hid_t       file_dataspace;
    hid_t       mem_dataspace;
    hid_t       dataset1, dataset2, dataset5, dataset6, dataset7;

    xfer_plist = H5Pcreate(H5P_DATASET_XFER);
    hid_t fapl = H5Pcreate(H5P_FILE_ACCESS);
    H5Pset_fapl_mpio(fapl,merge_comm, MPI_INFO_NULL);
    H5Pset_dxpl_mpio(xfer_plist, H5FD_MPIO_COLLECTIVE);

    hsize_t chunkdims[1];
    chunkdims[0] = 8192;
    hsize_t maxdims[1];
    maxdims[0] = (hsize_t)H5S_UNLIMITED;

    hid_t dataset_pl = H5Pcreate(H5P_DATASET_CREATE);

    int ret = H5Pset_chunk(dataset_pl,1,chunkdims);


    fid = H5Fopen(filename1.c_str(), H5F_ACC_RDONLY, fapl);

    hid_t fid2 = H5Fcreate(filename2.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT,fapl);
  
    H5Fclose(fid2);

    fid2 = H5Fopen(filename2.c_str(),H5F_ACC_RDWR,fapl);

    hsize_t attr_space[1];
    attr_space[0] = MAXBLOCKS*4+4;
    const char *attrname[1];

    attrname[0] = "Datasizes";

    std::string data_string = "Data1";
    dataset1 = H5Dopen2(fid,data_string.c_str(), H5P_DEFAULT);

    hid_t attr_id = H5Aopen(dataset1,attrname[0],H5P_DEFAULT);
    std::vector<uint64_t> attrs;
    attrs.resize(attr_space[0]);

    ret = H5Aread(attr_id,H5T_NATIVE_UINT64,attrs.data());

    hsize_t adims[1];
    adims[0] = VALUESIZE;
    hid_t s1 = H5Tarray_create(H5T_NATIVE_CHAR,1,adims);
    hid_t s2 = H5Tcreate(H5T_COMPOUND,sizeof(struct event));
    H5Tinsert(s2,"key",HOFFSET(struct event,ts),H5T_NATIVE_UINT64);
    H5Tinsert(s2,"value",HOFFSET(struct event,data),s1);

    int total_k = attrs[0];
    int k_size = attrs[1];
    int data_size = attrs[2];
    int numblocks = attrs[3];

    std::vector<int> blockids;
    int pos = 4;

    blockids.push_back(0);

    for(int i=0;i<numblocks;i++)
    {
	uint64_t minv = attrs[pos+i*4+0];
	uint64_t maxv = attrs[pos+i*4+1];

	/*if(minkey >= minv && minkey <= maxv || 
	   maxkey >= minv && maxkey <= maxv ||
	   minkey <= minv && maxkey >= maxv)*/
		blockids.push_back(i);
    }
    

    std::vector<struct event> *inp = new std::vector<struct event> ();
    std::vector<struct event> *oup = new std::vector<struct event> ();

    hsize_t offset_w = 0;

    for(int i=0;i<blockids.size();i++)
    {

    int blockid = blockids[i];
    hsize_t offset_f = 0;
    hsize_t numrecords = attrs[pos+blockid*4+3];
    int records_per_proc = numrecords/numprocs;
    int rem = numrecords%numprocs;

    for(int j=0;j<blockid;j++)
	    offset_f += attrs[pos+j*4+3];

    for(int j=0;j<myrank;j++)
    {	
       int size_p = 0;
       if(j < rem) size_p = records_per_proc+1;
       else size_p = records_per_proc;
       offset_f += size_p;
    }

    hsize_t blocksize = records_per_proc;
    if(myrank < rem) blocksize++;

    inp->resize(blocksize);

    file_dataspace = H5Dget_space(dataset1);
    ret = H5Sselect_hyperslab(file_dataspace, H5S_SELECT_SET,&offset_f,NULL,&blocksize,NULL);
    mem_dataspace = H5Screate_simple(1,&blocksize, NULL);
    ret = H5Dread(dataset1,s2, mem_dataspace, file_dataspace, xfer_plist,inp->data());

    int tag = 20000;
    int minv,maxv;
    minv = INT_MAX;maxv=0;
    int offset_f2=0;
    sort_block_secondary_key(inp,tag,0,minv,maxv,offset_f2);

    hsize_t offsetf2 = offset_f2;
    if(i==0)
    {
	offsetf2 += offset_w;	
	hid_t file_dataspace2 = H5Screate_simple(1,&numrecords,maxdims);
        hsize_t block_count = inp->size();
        hid_t mem_dataspace2 = H5Screate_simple(1,&block_count, NULL);
	ret = H5Sselect_hyperslab(file_dataspace2,H5S_SELECT_SET,&offsetf2,NULL,&block_count,NULL);
        hid_t dataset2 = H5Dcreate(fid2,data_string.c_str(),s2,file_dataspace2, H5P_DEFAULT,dataset_pl,H5P_DEFAULT);
	//ret = H5Dwrite(dataset2,s2, mem_dataspace2,file_dataspace2,xfer_plist,inp->data());

	H5Pclose(dataset_pl);
	H5Sclose(file_dataspace2);
	H5Sclose(mem_dataspace2);
	H5Dclose(dataset2);
    }
    else
    {
	offsetf2 += offset_w;
	hid_t  dataset2 = H5Dopen(fid2,data_string.c_str(), H5P_DEFAULT);

	 hsize_t dims[1];
         dims[0] = (hsize_t)(offset_w+numrecords);
         H5Dset_extent(dataset2, dims);
         hid_t file_dataspace2 = H5Dget_space(dataset2);

	 hsize_t maxsize = H5S_UNLIMITED;
	 hsize_t blocksize = inp->size();
         hid_t mem_dataspace2 = H5Screate_simple(1,&blocksize,&maxsize);
	 hsize_t one = 1;
	 ret = H5Sselect_hyperslab(file_dataspace2,H5S_SELECT_SET,&offsetf2,NULL,&one,&blocksize);
    	 ret = H5Dwrite(dataset2,s2, mem_dataspace2, file_dataspace2,xfer_plist,inp->data());


	H5Sclose(file_dataspace2);
	H5Sclose(mem_dataspace2);
	H5Dclose(dataset2);

    }

    offset_w += numrecords;
    H5Sclose(mem_dataspace);
    }

    delete inp;
    delete oup;
    H5Pclose(xfer_plist);
    H5Sclose(file_dataspace);
    H5Aclose(attr_id);
    H5Dclose(dataset1);
    H5Tclose(s2);
    H5Tclose(s1);

    H5Fclose(fid);
    H5Fclose(fid2);

   return s2_string;
}

std::string hdf5_sort::merge_datasets(std::string &s1,std::string &s2)
{
    std::string s3 = "merged_file";





    return s3;
}

std::string hdf5_sort::merge_stream_with_dataset(std::string &s,std::vector<struct event>* events)
{
   std::string s2 = "merged_file";



   return s2;

}
       
std::string hdf5_sort::merge_multiple_dataset(std::vector<std::string>& snames)
{
    std::string s2 = "merged_file";






   return s2;
}


void hdf5_sort::sort_block_secondary_key(std::vector<struct event> *events,int tag,int offset,int &min_value,int &max_value,int &offset_f2)
{

  MPI_Datatype value_field;
  MPI_Type_contiguous(VALUESIZE,MPI_CHAR,&value_field);
  MPI_Type_commit(&value_field);

  struct event e;
  MPI_Aint tdispl1[2];

  MPI_Get_address(&e,&tdispl1[0]);
  MPI_Get_address(&e.data,&tdispl1[1]);

  MPI_Aint base = tdispl1[0];
  MPI_Aint valuef = MPI_Aint_diff(tdispl1[1],base);

  MPI_Datatype key_value;
  int blocklens[2];
  MPI_Aint tdispl[2];
  int types[2];
  blocklens[0] = 1;
  blocklens[1] = 1;
  tdispl[0] = 0;
  tdispl[1] = valuef;
  types[0] = MPI_UINT64_T;
  types[1] = value_field;

  MPI_Type_create_struct(2,blocklens,tdispl,types,&key_value);
  MPI_Type_commit(&key_value);

  MPI_Request *reqs = (MPI_Request*)std::malloc(2*numprocs*sizeof(MPI_Request));

  int total_events = 0;

  int local_events = events->size();

   std::vector<int> mysplitters;

   if(local_events >= 2)
   {
     int r1 = random()%local_events;

     int r2 = r1;

     do
     {
        r2 = random()%local_events;
     }while(r2==r1);

     int v1,v2;
     std::string s1,s2;
     s1.assign((*events)[r1].data,(*events)[r1].data+sizeof(int));
     s2.assign((*events)[r2].data,(*events)[r2].data+sizeof(int));

     std::memcpy(&v1,(*events)[r1].data+offset,sizeof(int));
     std::memcpy(&v2,(*events)[r2].data+offset,sizeof(int));

     mysplitters.push_back(v1);
     mysplitters.push_back(v2);
   }
   
   std::vector<int> splitter_counts(numprocs);
   std::fill(splitter_counts.begin(),splitter_counts.end(),0);
   std::vector<int> splitter_counts_l(numprocs);

   splitter_counts_l[myrank] = mysplitters.size();

   int nreq = 0;
   for(int i=0;i<numprocs;i++)
   {
        MPI_Isend(&splitter_counts_l[myrank],1,MPI_INT,i,tag,merge_comm,&reqs[nreq]);
        nreq++;
   }

   for(int i=0;i<numprocs;i++)
   {
        MPI_Irecv(&splitter_counts[i],1,MPI_INT,i,tag,merge_comm,&reqs[nreq]);
        nreq++;
   }

   MPI_Waitall(nreq,reqs,MPI_STATUS_IGNORE);

    int num_splitters = 0;
   for(int i=0;i<numprocs;i++) num_splitters += splitter_counts[i];

   
   if(num_splitters > 0)
   {
     std::vector<int> splitters;
     splitters.resize(num_splitters);

     std::vector<int> displ(numprocs);
     std::fill(displ.begin(),displ.end(),0);

     for(int i=1;i<numprocs;i++)
           displ[i] = displ[i-1]+splitter_counts[i-1];


     nreq = 0;
    for(int i=0;i<numprocs;i++)
    {
        if(splitter_counts[myrank]>0)
        {
          MPI_Isend(mysplitters.data(),splitter_counts[myrank],MPI_INT,i,tag,merge_comm,&reqs[nreq]);
          nreq++;
        }
   }

   for(int i=0;i<numprocs;i++)
   {
        if(splitter_counts[i] > 0)
        {
          MPI_Irecv(&splitters[displ[i]],splitter_counts[i],MPI_INT,i,tag,merge_comm,&reqs[nreq]);
          nreq++;
        }
   }

   MPI_Waitall(nreq,reqs,MPI_STATUS_IGNORE);

   std::sort(splitters.begin(),splitters.end());

   int splitters_per_proc =  num_splitters/numprocs;
   int rem = num_splitters%numprocs;
   int offset = rem*(splitters_per_proc+1);

   mysplitters.clear();

   std::vector<int> procs;

   for(int i=0;i<splitters.size();i++)
   {
        int proc=-1;
        if(i < offset)
        {
           proc = i/(splitters_per_proc+1);
        }
        else proc = rem+((i-offset)/splitters_per_proc);
        procs.push_back(proc);
   }

   std::vector<int> send_counts(numprocs);
   std::vector<int> recv_counts(numprocs);
   std::vector<int> recv_displ(numprocs);
   std::vector<int> send_displ(numprocs);
   std::fill(send_counts.begin(),send_counts.end(),0);
   std::fill(send_displ.begin(),send_displ.end(),0);
   std::fill(recv_counts.begin(),recv_counts.end(),0);
   std::fill(recv_displ.begin(),recv_displ.end(),0);

   std::vector<int> event_dest;

   std::vector<int> event_count(numprocs);
   std::fill(event_count.begin(),event_count.end(),0);

   for(int i=0;i<events->size();i++)
   {
        int dest = -1;
	int ts = *(int*)((*events)[i].data+offset);
        for(int j=0;j<splitters.size();j++)
        {
            if(ts <= splitters[j])
            {
                 dest = procs[j]; break;
            }
        }
        if(dest == -1) dest = procs[splitters.size()-1];
        send_counts[dest]++;
        event_dest.push_back(dest);
   }

    for(int i=1;i<numprocs;i++)
        send_displ[i] = send_displ[i-1]+send_counts[i-1];

   std::vector<struct event> send_buffer;
   std::vector<struct event> recv_buffer;

   std::fill(recv_displ.begin(),recv_displ.end(),0);

   nreq = 0;
   for(int i=0;i<numprocs;i++)
   {
        MPI_Isend(&send_counts[i],1,MPI_INT,i,tag,merge_comm,&reqs[nreq]);
        nreq++;
        MPI_Irecv(&recv_counts[i],1,MPI_INT,i,tag,merge_comm,&reqs[nreq]);
        nreq++;
   }

   MPI_Waitall(nreq,reqs,MPI_STATUS_IGNORE);

   int total_recv_size = 0;
   for(int i=0;i<numprocs;i++)
           total_recv_size += recv_counts[i];

   send_buffer.resize(events->size());
   recv_buffer.resize(total_recv_size);

   int datasize = VALUESIZE;
   for(int i=0;i<events->size();i++)
   {
        int dest = event_dest[i];
        send_buffer[send_displ[dest]] = (*events)[i];
        send_displ[dest]++;
   }

   std::fill(send_displ.begin(),send_displ.end(),0);

   for(int i=1;i<numprocs;i++)
           send_displ[i] = send_displ[i-1]+send_counts[i-1];

    for(int i=1;i<numprocs;i++)
           send_displ[i] = send_displ[i-1]+send_counts[i-1];

   for(int i=1;i<numprocs;i++)
           recv_displ[i] = recv_displ[i-1]+recv_counts[i-1];

   nreq = 0;
   for(int i=0;i<numprocs;i++)
   {
        if(send_counts[i]>0)
        {
          MPI_Isend(&send_buffer[send_displ[i]],send_counts[i],key_value,i,tag,merge_comm,&reqs[nreq]);
          nreq++;
        }

   }

   for(int i=0;i<numprocs;i++)
   {
        if(recv_counts[i]>0)
        {
          MPI_Irecv(&recv_buffer[recv_displ[i]],recv_counts[i],key_value,i,tag,merge_comm,&reqs[nreq]);
          nreq++;
        }
   }

   MPI_Waitall(nreq,reqs,MPI_STATUS_IGNORE);

   events->clear();

   for(int i=0;i<numprocs;i++)
   {
           for(int j=0;j<recv_counts[i];j++)
           {
                struct event e = recv_buffer[recv_displ[i]+j];
                events->push_back(e);
           }
   }
   std::sort(events->begin(),events->end(),compare_fields);

   offset_f2 = 0;

   int lsize = events->size();
   std::vector<int> dsizes(numprocs);
   std::fill(dsizes.begin(),dsizes.end(),0);

   nreq = 0;

   for(int i=0;i<numprocs;i++)
   {
	MPI_Isend(&lsize,1,MPI_INT,i,tag,merge_comm,&reqs[nreq]);
	nreq++;
	MPI_Irecv(&dsizes[i],1,MPI_INT,i,tag,merge_comm,&reqs[nreq]);
	nreq++;
   }

   MPI_Waitall(nreq,reqs,MPI_STATUS_IGNORE);

   for(int i=0;i<myrank;i++)
	   offset_f2 += dsizes[i];


   }
   MPI_Type_free(&key_value);
   MPI_Type_free(&value_field);
   std::free(reqs);
}
