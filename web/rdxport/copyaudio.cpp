// copyaudio.cpp
//
// Rivendell web service portal -- CopyAudio service
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: copyaudio.cpp,v 1.3 2011/12/23 23:07:00 cvs Exp $
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

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include <rdformpost.h>
#include <rdweb.h>
#include <rdcart.h>
#include <rdaudioconvert.h>
#include <rdsettings.h>
#include <rdconf.h>

#include <rdxport.h>

void Xport::CopyAudio()
{
  int point;

  //
  // Verify Post
  //
  int source_cartnum=0;
  if(!xport_post->getValue("SOURCE_CART_NUMBER",&source_cartnum)) {
    RDXMLResult("Missing SOURCE_CART_NUMBER",400);
  }
  int source_cutnum=0;
  if(!xport_post->getValue("SOURCE_CUT_NUMBER",&source_cutnum)) {
    RDXMLResult("Missing SOURCE_CUT_NUMBER",400);
  }

  int destination_cartnum=0;
  if(!xport_post->getValue("DESTINATION_CART_NUMBER",&destination_cartnum)) {
    RDXMLResult("Missing DESTINATION_CART_NUMBER",400);
  }
  int destination_cutnum=0;
  if(!xport_post->getValue("DESTINATION_CUT_NUMBER",&destination_cutnum)) {
    RDXMLResult("Missing DESTINATION_CUT_NUMBER",400);
  }

  //
  // Verify User Perms
  //
  if(!xport_user->cartAuthorized(source_cartnum)) {
    RDXMLResult("No such cart",404);
  }
  if(!xport_user->cartAuthorized(destination_cartnum)) {
    RDXMLResult("No such cart",404);
  }

  //
  // Make the copy
  //
  unlink(RDCut::pathName(destination_cartnum,destination_cutnum));
  if(link(RDCut::pathName(source_cartnum,source_cutnum),
	  RDCut::pathName(destination_cartnum,destination_cutnum))!=0) {
    RDXMLResult(strerror(errno),400);
  }
  RDXMLResult("OK",200);
}
