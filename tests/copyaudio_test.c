/* copyaudio_test.c
 *
 * Test the Copy Audio API library.
 *
 * (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rivendell/copyaudio.h>

int main(int argc,char *argv[])
{

  char buf[BUFSIZ];
  char *p;
  long int src_cartnum=0;
  long int src_cutnum=0;
  long int dest_cartnum=0;
  long int dest_cutnum=0;
  char *host;
  char *user;
  char *passwd;

  /*      Get the Rivendell Host, User and Password if set in env */
  if (getenv("RIVHOST")!=NULL) {
    host = getenv("RIVHOST");
  }
  else {
    host="localhost";
  }

  if (getenv("RIVUSER")!=NULL) {
    user = getenv("RIVUSER");
  }
  else {
    user="USER";
  }

  if (getenv("RIVPASS")!=NULL) {
    passwd = getenv("RIVPASS");
  }
  else {
    passwd = "";
  } 

  printf("Please enter the Cart Number of the source audio that you want to copy from: ");
  if (fgets(buf,sizeof(buf),stdin) != NULL)
  {
    src_cartnum = strtol(buf, &p,10);

    if ( (buf[0] != '\n') &&
         ((*p != '\n') && (*p != '\0')))
    {
        fprintf(stderr," Illegal Characters detected! Exiting.\n");
	exit(0);
    }
  } 
  printf("Please enter the Cut Number of the source audio that you want to copy from: ");
  if (fgets(buf,sizeof(buf),stdin) != NULL)
  {
    src_cutnum = strtol(buf, &p,10);

    if ( (buf[0] != '\n') &&
         ((*p != '\n') && (*p != '\0')))
    {
        fprintf(stderr," Illegal Characters detected! Exiting.\n");
	exit(0);
    }
  } 
  //
  printf("Please enter the Cart Number of the destination audio that you want to copy to: ");
  if (fgets(buf,sizeof(buf),stdin) != NULL)
  {
    dest_cartnum = strtol(buf, &p,10);

    if ( (buf[0] != '\n') &&
         ((*p != '\n') && (*p != '\0')))
    {
        fprintf(stderr," Illegal Characters detected! Exiting.\n");
	exit(0);
    }
  } 
  printf("Please enter the Cut Number of the destination audio that you want to copy to: ");
  if (fgets(buf,sizeof(buf),stdin) != NULL)
  {
    dest_cutnum = strtol(buf, &p,10);

    if ( (buf[0] != '\n') &&
         ((*p != '\n') && (*p != '\0')))
    {
        fprintf(stderr," Illegal Characters detected! Exiting.\n");
	exit(0);
    }
  } 
  //
  // Call the function
  //
  int result=RD_CopyAudio( host,
		user,
		passwd,
		(unsigned)src_cartnum,
		(unsigned)src_cutnum,
		(unsigned)dest_cartnum,
		(unsigned)dest_cutnum);

  if(result<0) {
    fprintf(stderr,"Something went wrong!\n");
    exit(256);
  }

  if ((result< 200 || result > 299) && 
       (result != 0))
  {
    switch(result) {
      case 400:
        fprintf(stderr,"ERROR:  Cart/Cut Missing ! \n");
        break;
      case 404:
        fprintf(stderr,"          ERROR:  No Such Cart/Cut Exists! Or    \n");
        fprintf(stderr, "            UnAuthorized! \n");
        break;
      default:
        fprintf(stderr, "Unknown Error occurred ==> %d",result);
    }
    exit(256);
  }

  //
  // List the Results
  //
    printf(" Cart: %ld - Cut: %ld Audio was successfully copied to Cart: %ld - Cut: %ld!\n",
		src_cartnum, src_cutnum, dest_cartnum, dest_cutnum);
    printf("\n");

  exit(0);
}
