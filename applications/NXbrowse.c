/*-----------------------------------------------------------------------------
 NeXus - Neutron & X-ray Common Data Format
  
 NeXus Browser

 Copyright (C) 2000, Ray Osborn

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 Contact : R. Osborn <ROsborn@anl.gov>
           Materials Science Division
           Argonne National Laboratory
           Argonne, IL 60439-4845
           USA

 For further information, see <http://www.neutron.anl.gov/NeXus/>

 $Id$
!----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "napi.h"

#define StrEq(s1, s2) (strcmp((s1), (s2)) == 0)

int NXBdir (NXhandle fileId);
int NXBopen (NXhandle fileId, NXname groupName);
int NXBread (NXhandle fileId, NXname dataName, char *dimensions);
void PrintAttributes (NXhandle fileId);
void PrintDimensions (int rank, int *dimensions);
void PrintType (int dataType);
void PrintData (void* data, int dataType, int numElements);
int FindGroup (NXhandle fileId, char *groupName, char *groupClass);
int FindData (NXhandle fileId, char *dataName);

int main()
{
   NXhandle fileId;
   char fileName[80], inputText[255], path[80], *command, *dimensions, *stringPtr;
   NXname groupName, dataName;
   int status, groupLevel = 0, i;

   printf ("NXBrowse %s Copyright (C) 2000 R. Osborn, M. Koennecke, P. Klosowski\n", NEXUS_VERSION);
   printf ("Give name of NeXus file : ");
   fgets (fileName, sizeof(fileName), stdin);
   if ((stringPtr = strchr(fileName, '\n')) != NULL)
	  *stringPtr = '\0';

/* Open input file and output global attributes */
   if (NXopen (fileName, NXACC_READ, &fileId) != NX_OK) {
      printf ("NX_ERROR: Can't open %s\n", fileName);
      return NX_ERROR;
   }
   PrintAttributes (fileId);
/* Input commands until the EXIT command is given */
   strcpy (path, "NX");
   do {
      printf ("%s> ", path);
      fgets (inputText, sizeof(inputText), stdin);
      if ((stringPtr = strchr(inputText, '\n')) != NULL)
	     *stringPtr = '\0';
      command = strtok(inputText," ");
      /* Check if a command has been given */
      if (command == NULL) command = " ";
      /* Command is to print a directory of the current group */
      if (StrEq(command, "DIR") || StrEq(command, "dir")) {
         status = NXBdir (fileId);
      }    
      /* Command is to open the specified group */
      if (StrEq(command, "OPEN") || StrEq(command, "open")) {
         stringPtr = strtok(NULL, " "); 
         if (stringPtr != NULL) {
            strcpy (groupName, stringPtr);
            status = NXBopen (fileId, groupName);
            /* Add the group to the prompt string */
            if (status == NX_OK) {
               strcat (path, "/");
               strcat (path, groupName);
               groupLevel++;
            }
         }
         else {
            printf ("NX_ERROR: Specify a group\n");
         }
      }
      /* Command is to print the values of the data */
      if (StrEq(command, "READ") || StrEq(command, "read")) {
         stringPtr = strtok (NULL, " [");
         if (stringPtr != NULL) {
            strcpy (dataName, stringPtr);
            dimensions = strtok(NULL, "[]");
            status = NXBread (fileId, dataName, dimensions);
            /* Check for attributes unless a single element is specified */
            if (status == NX_OK && dimensions == NULL) PrintAttributes (fileId);
         }
         else {
            printf ("NX_ERROR: Specify a data item\n");
         }
      }
      /* Command is to close the current group */
      if (StrEq(command, "CLOSE") || StrEq(command, "close")) {
         if (NXclosegroup (fileId) == NX_OK) {
            /* Remove the group from the prompt string */
            stringPtr = strrchr (path, '/'); /* position of last group delimiter */
            if (stringPtr != NULL) 
               *stringPtr = '\0';            /* terminate the string there */
            groupLevel--;
         }
      }
      /* Command is to print help information */
      if (StrEq(command, "HELP") || StrEq(command, "help")) {
         printf ("NXbrowse commands : DIR\n");
         printf ("                    OPEN <groupName>\n");
         printf ("                    READ <dataName>\n");
         printf ("                    READ <dataName>[<dimension indices...>]\n");
         printf ("                    CLOSE\n");
         printf ("                    HELP\n");
         printf ("                    EXIT\n");
      }
      /* Command is to exit the program */
      if (StrEq(command, "EXIT") || StrEq(command, "exit") ||
          StrEq(command, "QUIT") || StrEq(command, "quit")) {
         for (i = groupLevel; i > 0; i--) NXclosegroup (fileId);
         NXclose (&fileId);
         return NX_OK;
      }
      status = NX_OK;
   } while (status == NX_OK);
   return NX_OK;
}

/* Outputs the contents of a NeXus group */
int NXBdir (NXhandle fileId)
{
   int status, dataType, dataRank, dataDimensions[NX_MAXRANK];
   NXname name, class;

   if (NXinitgroupdir (fileId) != NX_OK) return NX_ERROR;
   do {
      status = NXgetnextentry (fileId, name, class, &dataType);
      if (status == NX_ERROR) break;
      if (status == NX_OK) {
         if (!strncmp(class,"NX",2))
            printf ("  NX Group : %s (%s)\n", name, class);
         if (!strncmp(class,"SDS",3)) {
            printf ("  NX Data  : %s", name);
            if (NXopendata (fileId, name) != NX_OK) return NX_ERROR;
            if (NXgetinfo (fileId, &dataRank, dataDimensions, &dataType) != NX_OK) return NX_ERROR;
            PrintDimensions (dataRank, dataDimensions);
            printf (" ");
            PrintType (dataType);
            printf ("\n");
         }
      }
   } while (status != NX_EOD);
   return status;
}

/* Opens the requested group */
int NXBopen (NXhandle fileId, NXname groupName)
{
   NXname groupClass;

   if (groupName == NULL) {
      printf ("NX_ERROR: Specify a group name with the OPEN command\n");
      return NX_ERROR;
   }
   if (FindGroup (fileId, groupName, groupClass) != NX_OK) return NX_ERROR;
   if (NXopengroup (fileId, groupName, groupClass) != NX_OK) return NX_ERROR;
   return NX_OK;
}

/* Output requested data */
int NXBread (NXhandle fileId, NXname dataName, char *dimensions)
{
   int dataRank, dataDimensions[NX_MAXRANK], dataType, start[NX_MAXRANK], size[NX_MAXRANK], i, j;
   char dimString[80], *subString;
   void *dataBuffer;
  
   /* Check the specified data item exists */
   if (FindData (fileId, dataName) != NX_OK) return NX_ERROR;
   /* Open the data and obtain its type and rank details */
   if (NXopendata (fileId, dataName) != NX_OK) return NX_ERROR;
   if (NXgetinfo (fileId, &dataRank, dataDimensions, &dataType) != NX_OK) return NX_ERROR;
   /* Check if a single element has been specified */
   /* If so, read in the indices */
   if (dimensions != NULL) {
      strcpy (dimString, dimensions);      
      subString = strtok (dimString, ",");
      for (i = 0; subString != NULL && i < NX_MAXRANK; i++) {
         if (i >= dataRank) {
            printf ("NX_ERROR: Data rank = %d\n", dataRank);
            return NX_ERROR;
         }
         sscanf (subString, "%d", &j);
         if (j > dataDimensions[i] || j < 1) {
            printf ("NX_ERROR: Data dimension %d = %d\n", (i+1), dataDimensions[i]);
            return NX_ERROR;
         }
         start[i] = j - 1;
         size[i] = 1;
         subString = strtok (NULL, ",");
      }
      if (i != dataRank) {
         printf ("NX_ERROR: Data rank = %d\n", dataRank);
         return NX_ERROR;
      }
   }
   /* Otherwise, allocate enough space for the first 3 elements of each dimension */
   else {
      for (i = 0; i < dataRank; i++) {
         if (dataDimensions[i] > 3 && dataType != NX_CHAR) {
            start[i] = 0;
            size[i] = 3;
         } /* unless it's a character string */
         else {
            start[i] = 0;
            size[i] = dataDimensions[i];
         }
      }
   }
   if (NXmalloc((void**)&dataBuffer, dataRank, size, dataType) != NX_OK) return NX_ERROR;
   /* Read in the data with NXgetslab */
   if (NXgetslab (fileId, dataBuffer, start, size) != NX_OK) return NX_ERROR;
   /* Output data name, dimensions and type */
   printf ("  %s", dataName);
   if (dimensions == NULL)
      PrintDimensions (dataRank, dataDimensions);
   else 
      printf ("[%s]", dimensions);
   printf (" ");
   PrintType (dataType);
   printf (" = ");
   /* Output the data according to data type */
   if (dimensions == NULL) {  /* Print the first few values (max 3) */
      if (dataType == NX_CHAR) { /* If the data is a string, output the whole buffer */
         PrintData (dataBuffer, dataType, dataDimensions[0]);
      }
      else {
         if (dataRank == 1 && dataDimensions[0] == 1) {    /* It's a scalar */
            PrintData (dataBuffer, dataType, 1);
         }
         else {                                            /* It's an array */
            printf ("[ ");
            /* Determine total size of input buffer */
            for (i = 0, j = 0; i < dataRank; i++)
               j += dataDimensions[i];
            /* Output at least 3 values */
            if (j > 3) {
               PrintData (dataBuffer, dataType, 3);
               printf ("...");
            }
            /* unless the total size is smaller */
            else {
               PrintData (dataBuffer, dataType, j);
            }
            printf ("]");
         }
      }
   }
   else {                      /* Print the requested item */    
      PrintData (dataBuffer, dataType, 1);
   }
   printf ("\n");
   if (NXfree((void**)&dataBuffer) != NX_OK) return NX_ERROR;
   return NX_OK;
}

/* Checks for attributes and outputs their values */
void PrintAttributes (NXhandle fileId)
{
   int status, attrLen, attrType;
   NXname attrName;
   void *attrBuffer;

   do {
      status = NXgetnextattr (fileId, attrName, &attrLen, &attrType);
      if (status == NX_ERROR) return;
      if (status == NX_OK) {
         attrLen++; /* Add space for string termination */
         if (NXmalloc((void**)&attrBuffer, 1, &attrLen, attrType) != NX_OK) return;
         if (NXgetattr (fileId, attrName, attrBuffer, &attrLen, &attrType) != NX_OK) return;
         printf ("    %s = ", attrName);
         PrintData (attrBuffer, attrType, attrLen);
         printf ("\n");
         if (NXfree((void**)&attrBuffer) != NX_OK) return;
      }
   } while (status != NX_EOD);
   return;
}

/* Outputs the specified dimensions as a formatted string */
void PrintDimensions (int rank, int dimensions[])
{
   int i;

   if (rank > 1 || dimensions[0] != 1) {
      printf ("[");
      for (i=0; i<rank; i++) {
         if (i > 0) 
            printf (",");
         printf ("%d", dimensions[i]);
      }
      printf ("]");
   }
}

/* Converts the NeXus data type into a character string */
void PrintType (int dataType)
{
   switch (dataType) {
      case NX_CHAR:
        printf ("(NX_CHAR)");
        break;
      case NX_FLOAT32:
        printf ("(NX_FLOAT32)");
        break;
      case NX_FLOAT64:
        printf ("(NX_FLOAT64)");
        break;
      case NX_INT8:
        printf ("(NX_INT8)");
        break;
      case NX_UINT8:
        printf ("(NX_UINT8)");
        break;
      case NX_INT16:
        printf ("(NX_INT16)");
        break;
      case NX_UINT16:
        printf ("(NX_UINT16)");
        break;
      case NX_INT32:
        printf ("(NX_INT32)");
        break;
      case NX_UINT32:
        printf ("(NX_UINT32)");
        break;
      default:
        printf ("(UNKNOWN)");
        break;
   }
}

/* Outputs data items with the requested type */
void PrintData (void *data, int dataType, int numElements)
{
   int i;

   for (i=0; i<numElements; i++) {
      switch(dataType) {
         case NX_CHAR:
            printf ("%c", ((char *)data)[i]);
            break;
         case NX_INT8:
            printf ("%d ", ((char *)data)[i]);
            break;
         case NX_UINT8:
            printf ("%d ", ((unsigned char *)data)[i]);
            break;
         case NX_INT16:
            printf ("%d ", ((short *)data)[i]);
            break;
         case NX_UINT16:
            printf ("%d ", ((unsigned short *)data)[i]);
            break;
         case NX_INT32:
            printf ("%d ", ((long *)data)[i]);
            break;
         case NX_UINT32:
            printf ("%d ", ((unsigned long *)data)[i]);
            break;
         case NX_FLOAT32:
            printf ("%f ", ((float *)data)[i]);
            break;
         case NX_FLOAT64:
            printf ("%f ", ((double *)data)[i]);
            break;
         default:
            printf("PrintData: invalid type");
            break;
      }
   }
}

/* Searches group for requested group and return its class */
int FindGroup (NXhandle fileId, char *groupName, char *groupClass)
{
   int status, dataType;
   NXname name, class;

   NXinitgroupdir (fileId);
   do {
      status = NXgetnextentry (fileId, name, class, &dataType);
      if (status == NX_ERROR) return NX_ERROR;
      if (status == NX_OK) {
         if (StrEq (groupName, name)) {
            strcpy (groupClass, class);
            if (!strncmp(groupClass,"NX",2)) {
               return NX_OK;
            }
            else {
               printf ("NX_ERROR: %s is not a group\n", groupName);
               return NX_ERROR;
            }
         }
      }
   } while (status != NX_EOD);
   printf ("NX_ERROR: %s does not exist\n", groupName);
   return NX_EOD;
}

/* Searches group for the requested data item */
int FindData (NXhandle fileId, char *dataName)
{
   int status, dataType;
   NXname name, class;

   NXinitgroupdir (fileId);
   do {
      status = NXgetnextentry (fileId, name, class, &dataType);
      if (status == NX_ERROR) return NX_ERROR;
      if (status == NX_OK) {
         if (StrEq(dataName,name)) {
            if (!strncmp(class,"SDS",3)) { /* Data has class "SDS" */
               return NX_OK;
            }
            else {
               printf ("NX_ERROR: %s is not data\n", dataName);
               return NX_ERROR;
            }
         }
      }
   } while (status != NX_EOD);
   printf ("NX_ERROR: %s does not exist\n", dataName);
   return NX_EOD;
}