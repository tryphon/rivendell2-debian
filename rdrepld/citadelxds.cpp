// citadelxds.cpp
//
// Replicator implementation for the Citadel XDS Portal
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: citadelxds.cpp,v 1.4 2011/05/26 20:38:25 cvs Exp $
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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <qfileinfo.h>
#include <qdatetime.h>

#include <rddb.h>
#include <rdconf.h>
#include <rdaudioconvert.h>
#include <rdupload.h>
#include <rdcut.h>
#include <rdcart.h>
#include <rdescape_string.h>
#include <rdstringlist.h>

#include <citadelxds.h>

#define RD_MAX_CART_NUMBER 999999

CitadelXds::CitadelXds(ReplConfig *config)
  : ReplFactory(config)
{
  QString sql;
  RDSqlQuery *q;

  sql="select LAST_ISCI_XREFERENCE from VERSION";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    xds_isci_datetime=q->value(0).toDateTime();
  }
  delete q;
}


void CitadelXds::startProcess()
{
  CheckIsciXreference();
  CheckCarts();
}


bool CitadelXds::processCart(const unsigned cartnum)
{
  QString sql;
  RDSqlQuery *q;
  bool ret=false;

  sql=QString().sprintf("select FILENAME from ISCI_XREFERENCE \
                         where (CART_NUMBER=%u)&&(LATEST_DATE>=now())&&\
                         ((TYPE=\"R\")||(TYPE=\"B\"))",cartnum);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=PostCut(RDCut::cutName(cartnum,1),q->value(0).toString());
  }
  delete q;
  return ret;
}


void CitadelXds::CheckIsciXreference()
{
  QString sql;
  RDSqlQuery *q;

  QFileInfo *fi=new QFileInfo(rdsystem->isciXreferencePath());
  if(fi->exists()) {
    if(fi->lastModified()>xds_isci_datetime) {
      if(LoadIsciXreference(rdsystem->isciXreferencePath())) {
	sql="update VERSION set LAST_ISCI_XREFERENCE=now()";
	q=new RDSqlQuery(sql);
	delete q;
	xds_isci_datetime=QDateTime(QDate::currentDate(),QTime::currentTime());
	rdconfig->log("rdrepld",RDConfig::LogNotice,
		 QString().sprintf("loaded ISCI cross reference file \"%s\"",
			    (const char *)rdsystem->isciXreferencePath()));
      }
      else {
	rdconfig->log("rdrepld",RDConfig::LogErr,
	  QString().sprintf("unable to load ISCI cross reference file \"%s\"",
			    (const char *)rdsystem->isciXreferencePath()));
      }
    }
  }
  else {
    rdconfig->log("rdrepld",RDConfig::LogErr,
	  QString().sprintf("unable to load ISCI cross reference file \"%s\"",
			    (const char *)rdsystem->isciXreferencePath()));
  }
  delete fi;
}


bool CitadelXds::LoadIsciXreference(const QString &filename)
{
  FILE *f=NULL;
  char line[1024];
  QString sql;
  RDSqlQuery *q;
  RDStringList fields;
  unsigned cartnum;
  QStringList datelist;
  QDate date;
  bool ok=false;
  unsigned linenum=3;

  if((f=fopen(filename,"r"))==NULL) {
    return false;
  }

  //
  // Purge Old Data
  //
  sql="delete from ISCI_XREFERENCE";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Skip Header
  //
  fgets(line,1024,f);
  fgets(line,1024,f);

  //
  // Load Records
  //
  while(fgets(line,1024,f)!=NULL) {
    fields=fields.split(",",line,"\"");
    if(fields.size()==9) {
      for(unsigned i=0;i<fields.size();i++) {
	fields[i]=fields[i].replace("\"","").stripWhiteSpace();
      }
      cartnum=fields[3].right(fields[3].length()-1).toUInt(&ok);
      if(ok&&(cartnum<=RD_MAX_CART_NUMBER)) {
	datelist=datelist.split("/",fields[6]);
	if(datelist.size()==3) {
	  date.setYMD(datelist[2].toInt()+2000,datelist[0].toInt(),
		      datelist[1].toInt());
	  if(ValidateFilename(fields[8])) {
	    if(date.isValid()) {
	      sql=QString("insert into ISCI_XREFERENCE set ")+
	      "CART_NUMBER="+QString().sprintf("%u",cartnum)+","+
	      "ISCI=\""+RDEscapeString(fields[4])+"\","+
	      "FILENAME=\""+RDEscapeString(fields[8])+"\","+
	      "LATEST_DATE=\""+date.toString("yyyy/MM/dd")+"\","+
	      "TYPE=\""+RDEscapeString(fields[0])+"\","+
	      "ADVERTISER_NAME=\""+RDEscapeString(fields[1])+"\","+
	      "PRODUCT_NAME=\""+RDEscapeString(fields[2])+"\","+
	      "CREATIVE_TITLE=\""+RDEscapeString(fields[5])+"\","+
	      "REGION_NAME=\""+RDEscapeString(fields[7])+"\"";
	      q=new RDSqlQuery(sql);
	      delete q;
	    }
	    else {
	      rdconfig->log("rdrepld",RDConfig::LogWarning,QString().
			    sprintf("invalid date in line %d of \"%s\"",
				    linenum,(const char *)filename));
	    }
	  }
	  else {
	    rdconfig->log("rdrepld",RDConfig::LogWarning,QString().
	      sprintf("invalid FILENAME field \"%s\" in line %d of \"%s\"",
		      (const char *)fields[8],linenum,(const char *)filename));
	  }
	}
	else {
	  rdconfig->log("rdrepld",RDConfig::LogWarning,QString().
			sprintf("invalid date in line %d of \"%s\"",
				linenum,(const char *)filename));
	}
      }
      else {
	rdconfig->log("rdrepld",RDConfig::LogDebug,QString().
		  sprintf("missing/invalid cart number in line %d of \"%s\"",
			      linenum,(const char *)filename));
      }
    }
    else {
      rdconfig->log("rdrepld",RDConfig::LogWarning,QString().
		    sprintf("line %d malformed in \"%s\"",
			    linenum,(const char *)filename));
    }
    linenum++;
  }

  //
  // Clean Up
  //
  fclose(f);
  return true;
}


bool CitadelXds::ValidateFilename(const QString &filename)
{
  bool ret=true;

  //
  // List of illegal characters taken from 'Illegal Characters4.doc'
  // from Citadel
  //
  ret=ret&&(filename.find(" ")<0);
  ret=ret&&(filename.find("\"")<0);
  ret=ret&&(filename.find("%")<0);
  ret=ret&&(filename.find("*")<0);
  ret=ret&&(filename.find("+")<0);
  //ret=ret&&(filename.find(",")<0);
  ret=ret&&(filename.find("/")<0);
  ret=ret&&(filename.find(":")<0);
  ret=ret&&(filename.find(";")<0);
  ret=ret&&(filename.find("<")<0);
  ret=ret&&(filename.find("=")<0);
  ret=ret&&(filename.find(">")<0);
  ret=ret&&(filename.find("?")<0);
  ret=ret&&(filename.find("@")<0);
  ret=ret&&(filename.find("[")<0);
  ret=ret&&(filename.find("\\")<0);
  ret=ret&&(filename.find("]")<0);
  ret=ret&&(filename.find("^")<0);
  ret=ret&&(filename.find("{")<0);
  ret=ret&&(filename.find("|")<0);
  ret=ret&&(filename.find("}")<0);

  return ret;
}


void CitadelXds::CheckCarts()
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  RDSqlQuery *q2;
  QString now=QDateTime(QDate::currentDate(),QTime::currentTime()).addDays(-6).
    toString("yyyy-MM-dd hh:mm:ss");

  //
  // Generate Update List
  //
  sql="select CART_NUMBER,FILENAME from ISCI_XREFERENCE \
       where (LATEST_DATE>=now())&&((TYPE=\"R\")||(TYPE=\"B\"))";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString().sprintf("select ID from REPL_CART_STATE \
                           where (REPLICATOR_NAME=\"%s\")&&\
                           (CART_NUMBER=%u)&&\
                           (ITEM_DATETIME>\"%s\")",
			  (const char *)RDEscapeString(config()->name()),
			  q->value(0).toUInt(),
			  (const char *)now);
    q1=new RDSqlQuery(sql);
    if(!q1->first()) {
      if(PostCut(RDCut::cutName(q->value(0).toUInt(),1),
		 q->value(1).toString())) {
	sql=QString().sprintf("select ID from REPL_CART_STATE where \
                               (REPLICATOR_NAME=\"%s\")&&\
                               (CART_NUMBER=%u)",
			      (const char *)RDEscapeString(config()->name()),
			      q->value(0).toUInt());
	q2=new RDSqlQuery(sql);
	if(q2->first()) {
	  sql=QString().sprintf("update REPL_CART_STATE set\
                                 ITEM_DATETIME=now() where \
                                 (REPLICATOR_NAME=\"%s\")&&\
                                 (CART_NUMBER=%u)",
				(const char *)RDEscapeString(config()->name()),
				q->value(0).toUInt());
	}
	else {
	  sql=QString().sprintf("insert into REPL_CART_STATE set \
                                 ITEM_DATETIME=now(),\
                                 REPLICATOR_NAME=\"%s\",\
                                 CART_NUMBER=%u",
				(const char *)RDEscapeString(config()->name()),
				q->value(0).toUInt());
	}
	delete q2;
	q2=new RDSqlQuery(sql);
	delete q2;
      }
    }
    delete q1;
  }
  delete q;
}


bool CitadelXds::PostCut(const QString &cutname,const QString &filename)
{
  //
  // Export File
  //
  RDAudioConvert::ErrorCode conv_err;
  RDUpload::ErrorCode upload_err;
  float speed_ratio=1.0;
  RDCut *cut=new RDCut(cutname);
  if(!cut->exists()) {
    delete cut;
    return false;
  }
  if(cut->length()==0) {
    delete cut;
    return true;
  }
  RDCart *cart=new RDCart(cut->cartNumber());
  if(cart->enforceLength()) {
    speed_ratio=(float)cut->length()/(float)cart->forcedLength();
  }
  RDSettings *settings=new RDSettings();
  QString tempfile=RDTempDir()+"/"+filename;
  RDAudioConvert *conv=new RDAudioConvert(rdconfig->stationName());
  conv->setSourceFile(RDCut::pathName(cutname));
  conv->setDestinationFile(tempfile);
  conv->setRange(cut->startPoint(),cut->endPoint());
  conv->setSpeedRatio(speed_ratio);
  settings->setFormat(config()->format());
  settings->setChannels(config()->channels());
  settings->setSampleRate(config()->sampleRate());
  settings->setBitRate(config()->bitRate());
  settings->setQuality(config()->quality());
  settings->setNormalizationLevel(config()->normalizeLevel());
  conv->setDestinationSettings(settings);
  delete cart;
  delete cut;
  switch(conv_err=conv->convert()) {
  case RDAudioConvert::ErrorOk:
    break;

  default:
    rdconfig->log("rdrepld",RDConfig::LogErr,
      QString().sprintf("CitadelXds: audio conversion failed: %s, cutname: %s",
			(const char *)RDAudioConvert::errorText(conv_err),
			(const char *)cutname));
    delete conv;
    delete settings;
    return false;
  }
  delete conv;
  delete settings;

  //
  // Upload File
  //
  RDUpload *upload=new RDUpload(rdconfig->stationName());
  upload->setSourceFile(tempfile);
  upload->setDestinationUrl(config()->url()+"/"+filename);
  switch(upload_err=upload->runUpload(config()->urlUsername(),
				      config()->urlPassword(),
				      rdconfig->logXloadDebugData())) {
  case RDUpload::ErrorOk:
    break;

  default:
    rdconfig->log("rdrepld",RDConfig::LogErr,
		  QString().sprintf("CitadelXds: audio upload failed: %s",
			       (const char *)RDUpload::errorText(upload_err)));
    unlink(tempfile);
    delete upload;
    return false;
  }
  unlink(tempfile);
  delete upload;
  rdconfig->log("rdrepld",RDConfig::LogNotice,
		QString().sprintf("CitadelXds: uploaded cut %s to %s/%s",
				  (const char *)cutname,
				  (const char *)config()->url(),
				  (const char *)filename));

  return true;
}
