/**
  * TestJapi does some testing of the NeXus for Java API.  It can also
  * serve as an example for the usage of the NeXus API for Java.
  *
  * Mark Koennecke, October 2000
  *
  * updated for NAPI-2 with HDF-5 support
  * 
  * Mark Koennecke, August 2001
  */
import neutron.nexus.*;
import java.util.Hashtable;
import java.util.Enumeration;

public class TestJapi {
    static public void main(String args[])
    {
        String fileName = "JapiTest.nxs";
        String fileName5 = "JapiTest.h5";
        String group = "entry1";
        String nxclass = "NXentry"; 
        int iData1[][] = new int[3][10];
        int iData2[][] = new int[3][10];
        float fData1[][] = new float[3][10];
        int islab[] = new int[10];
        int iDim[] = new int[2], i, j;
        int iStart[] = new int[2];
        int iEnd[] = new int[2];
        NexusFile nf = null;
        NXlink gid, did;
        String attname, vname, vclass;
        AttributeEntry atten;
        boolean HDF5 = false;

        // check if we should do a HDF-5 test
	if(args.length >= 1){
	    if(args[0].indexOf("HDF5") >= 0){
		HDF5 = true;
            }
        }

        // create some data
        for(i = 0; i < 3; i++)
	{
          for(j = 0;  j < 10; j++)
	  {
             iData1[i][j] = i*10 + j;
             fData1[i][j] = (float)(i*10.1 + j*.2);
          }
        }
        for(i = 0; i < 10; i++)
	{
           islab[i] = 10000 + i;
        }

        try{
	 //create a NexusFile
	    if(HDF5){ 
                nf = new NexusFile(fileName5,NexusFile.NXACC_CREATE5);
            } else {
                nf = new NexusFile(fileName,NexusFile.NXACC_CREATE);
            }
	   
         // error handling check
	 try{
             nf.opengroup(group,nxclass);
         }catch(NexusException nex) {
           System.out.println("Exception handling mechanism works");
         }
         // create and open a group
         nf.makegroup(group,nxclass);
         nf.opengroup(group,nxclass);

         // get a link ID for this group
         gid = nf.getgroupID();

         // create and open a dataset
         iDim[0] = 3;
         iDim[1] = 10;
         nf.makedata("iData1",NexusFile.NX_INT32,2,iDim);
         nf.opendata("iData1");

         // get a link ID to this data set
         did = nf.getdataID();

         // write data to it 
         nf.putdata(iData1);

         // add attributes, the first one is also an example how to write
         // strings (by converting to byte arrays)
         String units = "MegaFarts";
         nf.putattr("Units",units.getBytes(),NexusFile.NX_CHAR);
         iStart[0] = 1;
	 nf.putattr("signal",iStart,NexusFile.NX_INT32);

         // closedata
         nf.closedata();

         // write a compressed data set
         nf.compmakedata("iData1_compressed",NexusFile.NX_INT32,2,iDim,
                        NexusFile.NX_COMP_LZW,iDim);
         nf.opendata("iData1_compressed");
         nf.putdata(iData1);
         nf.closedata();

         // write a float data set
         nf.makedata("fData1",NexusFile.NX_FLOAT32,2,iDim);
         nf.opendata("fData1");
         nf.putdata(fData1);
         nf.closedata();

         // write a dataset in slabs */
         nf.makedata("slabbed",NexusFile.NX_INT32,2,iDim);
         nf.opendata("slabbed");
         iStart[1] = 0;
         iEnd[1] = 10;
         iEnd[0] = 1;
         for(i = 0; i < 3; i++)
	 {
           iStart[0] = i;
           nf.putslab(islab,iStart, iEnd);
         }
         nf.closedata();

         // closegroup
         nf.closegroup();

         // test linking code 
         nf.makegroup("entry2","NXentry");
         nf.opengroup("entry2","NXentry");
         nf.makegroup("data","NXdata");
         nf.opengroup("data","NXdata");
         nf.makelink(did);
	 // nf.debugstop();  
         nf.makelink(gid);
         nf.closegroup();

         // close a file explicitly (recommended!)
         nf.finalize();
         System.out.println(" *** Writing Tests passed with flying banners"); 

	 //**************** reading tests *******************************
        iData2[2][5] = 66666;
        fData1[2][5] = (float)66666.66;

         // reopen the file
	 if(HDF5){ 
            nf = new NexusFile(fileName5,NexusFile.NXACC_READ);
         } else {
            nf = new NexusFile(fileName,NexusFile.NXACC_READ);
         }

         // test attribute enquiry routine at global attributes
         Hashtable h = nf.attrdir();
         Enumeration e = h.keys();
         while(e.hasMoreElements())
	 {
           attname = (String)e.nextElement();
           atten = (AttributeEntry)h.get(attname);
           System.out.println("Found global attribute: " + attname +
             " type: "+ atten.type + " ,length: " + atten.length); 
         }

         // test reading vGroup directory 
         //nf.debugstop();
         nf.opengroup(group,nxclass);
         h = nf.groupdir();
         e = h.keys();
         System.out.println("Found in vGroup entry:");
         while(e.hasMoreElements())
	 {
            vname = (String)e.nextElement();
            vclass = (String)h.get(vname);
            System.out.println("     Item: " + vname + " class: " + vclass);
         }

         // test reading SDS info and attributes
         nf.opendata("iData1");
         nf.getinfo(iDim,iStart);
         System.out.println("Found iData1 with: rank = " + iStart[0] +
           " type = " + iStart[1] + " dims = " +
           iDim[0] + ", " + iDim[1]);
         h = nf.attrdir();
         e = h.keys();
         while(e.hasMoreElements())
	 {
           attname = (String)e.nextElement();
           atten = (AttributeEntry)h.get(attname);
           System.out.println("Found SDS attribute: " + attname +
             " type: "+ atten.type + " ,length: " + atten.length); 
         }

         // success for inquiry routines
         nf.closedata();
         nf.closegroup();
         System.out.println(" **** Inquiry routines passed test");

         // test the data reading routines
         nf.opengroup(group,nxclass);
         nf.opendata("iData1");
         nf.getdata(iData2);
         for(i = 0; i < 3; i++)
	 {
            for(j = 0; j < 10; j++)
	    {
              if(iData1[i][j] != iData2[i][j])
		  System.out.println(" Data Reading Error at : " + i 
                                      + ", " + j);  
            }
         }
         // test attribute reading. This is also an example for reading
         // Strings from a NeXus file. 
         byte bString[] = new byte[60];
         iDim[0] = 60;
         iDim[1] = NexusFile.NX_CHAR;
         nf.getattr("Units",bString,iDim);
         System.out.println("Read attribute Units to: " + new String(bString));
         // check reading a slab
         iStart[0] = 0;
         iStart[1] = 0;
         iEnd[0] = 1;
         iEnd[1] = 10;
         nf.getslab(iStart,iEnd,islab);
         for(i = 0; i < 10; i++)
	 {
           if(islab[i] != iData1[0][i])
		  System.out.println(" Slab Reading Error at : " + i + 
                   " expected: " + iData1[0][i] + ", got: " + islab[i]); 
         }                 
         nf.closedata();          

         // check compressed data
         nf.opendata("iData1_compressed");
         nf.getdata(iData2);
         for(i = 0; i < 3; i++)
	 {
            for(j = 0; j < 10; j++)
	    {
              if(iData1[i][j] != iData2[i][j])
		  System.out.println(" Data Reading Error at : " + i 
                                      + ", " + j);  
            }
         }
         nf.closedata();

         // now, for completeness: check float data as well
         nf.opendata("fData1");
         nf.getdata(fData1);
         nf.closedata();
         for(i = 0; i < 3; i++)
	 {
           for(j = 0;  j < 10; j++)
	   {
              if(Math.abs(fData1[i][j] -  (float)(i*10.1 + j*.2)) >
                .05)
	      {
		  System.out.println(" Float Reading Error at : " + i 
                                      + ", " + j);  
              }
           }
         }
         
         // reading success
         System.out.println(" *** Data Reading routines appear to work");

         // the success message
         System.out.println(" ***** CONGATULATION *****");
         System.out.println(" When you see this, the likelihood of " +
         " the Java NeXus API working for you");
         System.out.println(" has increased significantly");
        }catch(NexusException ne) {
           System.out.println("NexusException: " + ne.getMessage() + 
                              " Occurred");
           if(nf != null)
	   {
	       try{
                  nf.finalize();
               }catch(Throwable tt) {}
           }
           ne.printStackTrace();
        }catch(Throwable t) {
           System.out.println("Error closing NeXus file");
           System.out.println(t.getMessage());
           t.printStackTrace();
        }
    }
}  



