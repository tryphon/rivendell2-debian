// rdaudioimport.cpp
//
// Import an Audio File using the RdXport Web Service
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdaudioimport.cpp,v 1.9 2011/04/01 21:40:58 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#include <curl/curl.h>

#include <qapplication.h>

#include <rd.h>
#include <rdxport_interface.h>
#include <rdformpost.h>
#include <rdaudioimport.h>

//
// CURL Progress Callback
//
int ImportProgressCallback(void *clientp,double dltotal,double dlnow,
			  double ultotal,double ulnow)
{
  RDAudioImport *conv=(RDAudioImport *)clientp;
  qApp->processEvents();
  if(conv->aborting()) {
    return 1;
  }
  return 0;
}


RDAudioImport::RDAudioImport(RDStation *station,
			     QObject *parent,const char *name)
  : QObject(parent,name)
{
  conv_station=station;
  conv_cart_number=0;
  conv_cut_number=0;
  conv_settings=NULL;
  conv_use_metadata=false;
  conv_aborting=false;
}


void RDAudioImport::setCartNumber(unsigned cartnum)
{
  conv_cart_number=cartnum;
}


void RDAudioImport::setCutNumber(unsigned cutnum)
{
  conv_cut_number=cutnum;
}


void RDAudioImport::setSourceFile(const QString &filename)
{
  conv_src_filename=filename;
}


void RDAudioImport::setUseMetadata(bool state)
{
  conv_use_metadata=state;
}


void RDAudioImport::setDestinationSettings(RDSettings *settings)
{
  conv_settings=settings;
}


RDAudioImport::ErrorCode RDAudioImport::runImport(const QString &username,
						  const QString &password)
{
  long response_code;
  CURL *curl=NULL;
  struct curl_httppost *first=NULL;
  struct curl_httppost *last=NULL;
  char url[1024];
  char str[1024];

  //
  // Generate POST Data
  //
  // We have to use multipart here because we have a file to send.
  //
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"COMMAND",
	       CURLFORM_COPYCONTENTS,
	       (const char *)QString().sprintf("%u",RDXPORT_COMMAND_IMPORT),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"LOGIN_NAME",
	       CURLFORM_COPYCONTENTS,(const char *)username,CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"PASSWORD",
	       CURLFORM_COPYCONTENTS,(const char *)password,CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"CART_NUMBER",
	       CURLFORM_COPYCONTENTS,
	       (const char *)QString().sprintf("%u",conv_cart_number),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"CUT_NUMBER",
	       CURLFORM_COPYCONTENTS,
	       (const char *)QString().sprintf("%u",conv_cut_number),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"CHANNELS",
	       CURLFORM_COPYCONTENTS,
	       (const char *)QString().sprintf("%u",conv_settings->channels()),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"NORMALIZATION_LEVEL",
	       CURLFORM_COPYCONTENTS,
	       (const char *)QString().sprintf("%d",conv_settings->
					       normalizationLevel()),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"AUTOTRIM_LEVEL",
	       CURLFORM_COPYCONTENTS,
	       (const char *)QString().sprintf("%d",conv_settings->
					       autotrimLevel()),CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"USE_METADATA",
	       CURLFORM_COPYCONTENTS,
	       (const char *)QString().sprintf("%u",conv_use_metadata),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"FILENAME",
	       CURLFORM_FILE,(const char *)(conv_src_filename.utf8()),
	       CURLFORM_END);

  //
  // Set up the transfer
  //
  if((curl=curl_easy_init())==NULL) {
    return RDAudioImport::ErrorInternal;
  }
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,stdout);
  curl_easy_setopt(curl,CURLOPT_HTTPPOST,first);
  curl_easy_setopt(curl,CURLOPT_TIMEOUT,RD_CURL_TIMEOUT);
  curl_easy_setopt(curl,CURLOPT_PROGRESSFUNCTION,ImportProgressCallback);
  curl_easy_setopt(curl,CURLOPT_PROGRESSDATA,this);
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,0);
  //
  // Write out URL as a C string before passing to curl_easy_setopt(), 
  // otherwise some versions of LibCurl will throw a 'bad/illegal format' 
  // error.
  //
  strncpy(url,conv_station->webServiceUrl(),1024);
  curl_easy_setopt(curl,CURLOPT_URL,url);

  //
  // Send it
  //
  switch(curl_easy_perform(curl)) {
  case CURLE_OK:
    break;

  case CURLE_ABORTED_BY_CALLBACK:
    curl_easy_cleanup(curl);
    return RDAudioImport::ErrorAborted;

  case CURLE_UNSUPPORTED_PROTOCOL:
  case CURLE_FAILED_INIT:
  case CURLE_COULDNT_RESOLVE_PROXY:
  case CURLE_PARTIAL_FILE:
  case CURLE_HTTP_RETURNED_ERROR:
  case CURLE_WRITE_ERROR:
  case CURLE_OUT_OF_MEMORY:
  case CURLE_OPERATION_TIMEDOUT:
  case CURLE_HTTP_POST_ERROR:
    curl_easy_cleanup(curl);
    return RDAudioImport::ErrorInternal;

  case CURLE_URL_MALFORMAT:
  case CURLE_COULDNT_RESOLVE_HOST:
  case CURLE_COULDNT_CONNECT:
#ifdef CURLE_REMOTE_ACCESS_DENIED
  case CURLE_REMOTE_ACCESS_DENIED:
#endif  // CURLE_REMOTE_ACCESS_DENIED
    curl_easy_cleanup(curl);
    return RDAudioImport::ErrorUrlInvalid;
  }

  //
  // Process the results
  //
  curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
  curl_easy_cleanup(curl);
  curl_formfree(first);
  switch(response_code) {
  case 200:
    break;

  case 403:
    return RDAudioImport::ErrorInvalidUser;

  case 415:
    return RDAudioImport::ErrorInvalidSettings;

  case 500:
    return RDAudioImport::ErrorInvalidSettings;

  default:
    return RDAudioImport::ErrorService;
  }
  return RDAudioImport::ErrorOk;
}


bool RDAudioImport::aborting() const
{
  return conv_aborting;
}


QString RDAudioImport::errorText(RDAudioImport::ErrorCode err)
{
  QString ret=QString().sprintf("Uknown Error [%u]",err);

  switch(err) {
  case RDAudioImport::ErrorOk:
    ret=tr("OK");
    break;

  case RDAudioImport::ErrorInvalidSettings:
    ret=tr("Invalid/unsupported audio parameters");
    break;

  case RDAudioImport::ErrorNoSource:
    ret=tr("No such cart/cut");
    break;

  case RDAudioImport::ErrorNoDestination:
    ret=tr("Unable to create destination file");
    break;

  case RDAudioImport::ErrorInternal:
    ret=tr("Internal Error");
    break;

  case RDAudioImport::ErrorUrlInvalid:
    ret=tr("Invalid URL");
    break;

  case RDAudioImport::ErrorService:
    ret=tr("RDXport service returned an error");
    break;

  case RDAudioImport::ErrorInvalidUser:
    ret=tr("Invalid user or password");
    break;

  case RDAudioImport::ErrorAborted:
    ret=tr("Aborted");
    break;
  }
  return ret;
}


void RDAudioImport::abort()
{
  conv_aborting=true;
}
