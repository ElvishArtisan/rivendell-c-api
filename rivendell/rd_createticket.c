/* rd_createticket.c
 *
 * Implementation of the Create Ticket Rivendell Access Library
 *
 * (C) Copyright 2017 Todd Baker  <bakert@rfa.org>             
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

#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <expat.h>

#include "rd_common.h"
#include "rd_createticket.h"

struct xml_data {
  char elem_name[256];
  char strbuf[1024];
  struct rd_ticketinfo *ticketinfo;
};


static void XMLCALL __CreateTicketElementStart(void *data, const char *el, 
					     const char **attr)
{
  struct xml_data *xml_data=(struct xml_data *)data;
  if(strcasecmp(el,"ticketInfo")==0) {    // Allocate a new ticketinfo entry
    xml_data->ticketinfo=realloc(xml_data->ticketinfo,
			   sizeof(struct rd_ticketinfo));
  }
  strlcpy(xml_data->elem_name,el,256);
  memset(xml_data->strbuf,0,1024);
}


static void XMLCALL __CreateTicketElementData(void *data,const XML_Char *s,
					    int len)
{
  struct xml_data *xml_data=(struct xml_data *)data;

  memcpy(xml_data->strbuf+strlen(xml_data->strbuf),s,len);
}


static void XMLCALL __CreateTicketElementEnd(void *data, const char *el)
{
  struct xml_data *xml_data=(struct xml_data *)data;
  struct rd_ticketinfo *ticketinfo=xml_data->ticketinfo;
  char hold_datetime[25];

  if(strcasecmp(el,"ticket")==0) {
    strlcpy(ticketinfo->ticket,xml_data->strbuf,41);
  }
  if(strcasecmp(el,"expires")==0) {
    strlcpy(hold_datetime,xml_data->strbuf,26);
    ticketinfo->tkt_expiration_datetime = RD_Cnv_DTString_to_tm(hold_datetime);
  }
}


size_t __CreateTicketCallback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
  XML_Parser p=(XML_Parser)userdata;

  XML_Parse(p,ptr,size*nmemb,0);
  
  return size*nmemb;
}


int RD_CreateTicket(struct rd_ticketinfo *ticketinfo[],
		  	const char hostname[],
                  	const char username[],
                  	const char passwd[],
                        const char user_agent[],
                  	unsigned *numrecs)
{
  char post[1500];
  char url[1500];
  CURL *curl=NULL;
  XML_Parser parser;
  struct xml_data xml_data;
  long response_code;
  char errbuf[CURL_ERROR_SIZE];
  CURLcode res;
  char PkgVersion[255]="Rivendell-C-API/";

   /* Set number of recs so if fail already set */
  *numrecs = 0;

  /*
   * Setup the CURL call
   */
  memset(&xml_data,0,sizeof(xml_data));
  parser=XML_ParserCreate(NULL);
  XML_SetUserData(parser,&xml_data);
  XML_SetElementHandler(parser,__CreateTicketElementStart,
			__CreateTicketElementEnd);
  XML_SetCharacterDataHandler(parser,__CreateTicketElementData);
  snprintf(url,1500,"http://%s/rd-bin/rdxport.cgi",hostname);
  snprintf(post,1500,"COMMAND=31&LOGIN_NAME=%s&PASSWORD=%s",
	curl_easy_escape(curl,username,0),
	curl_easy_escape(curl,passwd,0));
  if((curl=curl_easy_init())==NULL) {
    return -1;
  }

  // Check if User Agent Present otherwise set to default
  if (strlen(user_agent)> 0){
    curl_easy_setopt(curl, CURLOPT_USERAGENT,user_agent);
  }
  else
  {
    strcat(PkgVersion,VERSION);
    curl_easy_setopt(curl, CURLOPT_USERAGENT,PkgVersion);
  }

  curl_easy_setopt(curl,CURLOPT_WRITEDATA,parser);
  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,__CreateTicketCallback);
  curl_easy_setopt(curl,CURLOPT_URL,url);
  curl_easy_setopt(curl,CURLOPT_POST,1);
  curl_easy_setopt(curl,CURLOPT_POSTFIELDS,post);
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1);
  curl_easy_setopt(curl,CURLOPT_ERRORBUFFER,errbuf);
  //  curl_easy_setopt(curl,CURLOPT_VERBOSE,1);
  res = curl_easy_perform(curl);
  if(res != CURLE_OK) {
    #ifdef RIVC_DEBUG_OUT
        size_t len = strlen(errbuf);
        fprintf(stderr, "\nlibcurl error: (%d)", res);
        if (len)
            fprintf(stderr, "%s%s", errbuf,
                ((errbuf[len-1] != '\n') ? "\n" : ""));
        else
            fprintf(stderr, "%s\n", curl_easy_strerror(res));
    #endif
    curl_easy_cleanup(curl);
    return -1;
  }

/* The response OK - so figure out if we got what we wanted.. */

  curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
  curl_easy_cleanup(curl);
  
  if (response_code > 199 && response_code < 300) { 
    *ticketinfo=xml_data.ticketinfo;
    *numrecs = 1;
    return 0;
  }
  else {
    #ifdef RIVC_DEBUG_OUT
        fprintf(stderr,"rd_createticket Call Returned Error: %s\n",xml_data.strbuf);
    #endif
    return (int)response_code;
  }
}
