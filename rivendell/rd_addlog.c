/* rd_addcart.c
 *
 * Header for the AddLog  Rivendell Access Library
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

#include <curl/curl.h>

#include "rd_addlog.h"

int RD_AddLog(const char hostname[],
	      const char username[],
	      const char passwd[],
	      const char logname[],
	      const char servicename[])
{
  char post[1500];
  char url[1500];
  CURL *curl=NULL;
  long response_code;
  char errbuf[CURL_ERROR_SIZE];
  CURLcode res;

  /*
   * Setup the CURL call
   */
  snprintf(url,1500,"http://%s/rd-bin/rdxport.cgi",hostname);
  snprintf(post,1500,"COMMAND=29&LOGIN_NAME=%s&PASSWORD=%s&LOG_NAME=%s&SERVICE_NAME=%s",
	   curl_easy_escape(curl,username,0),
	   curl_easy_escape(curl,passwd,0),
	   curl_easy_escape(curl,logname,0),
	   curl_easy_escape(curl,servicename,0));
  if((curl=curl_easy_init())==NULL) {
    curl_easy_cleanup(curl);
    return -1;
  }
  //  curl_easy_setopt(curl,CURLOPT_WRITEDATA,parser);
  //  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,__AddCartCallback);
  curl_easy_setopt(curl,CURLOPT_URL,url);
  curl_easy_setopt(curl,CURLOPT_POST,1);
  curl_easy_setopt(curl,CURLOPT_POSTFIELDS,post);
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1);
  curl_easy_setopt(curl,CURLOPT_ERRORBUFFER,errbuf);
  //  curl_easy_setopt(curl,CURLOPT_VERBOSE,1);
  res = curl_easy_perform(curl);
  if(res != CURLE_OK) {
    size_t len = strlen(errbuf);
    fprintf(stderr, "\nlibcurl error: (%d)", res);
    if (len)
        fprintf(stderr, "%s%s", errbuf,
            ((errbuf[len-1] != '\n') ? "\n" : ""));
    else
        fprintf(stderr, "%s\n", curl_easy_strerror(res));
    curl_easy_cleanup(curl);
    return -1;
  }
/* The response OK - so figure out if we got what we wanted.. */

  curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
  curl_easy_cleanup(curl);
  
  if (response_code > 199 && response_code < 300) {  //Success
    return 0;
  }
  return (int)response_code;
}
