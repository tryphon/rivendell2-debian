// deleteaudio.cpp
//
// Rivendell web service portal -- DeleteAudio service
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: deleteaudio.cpp,v 1.3 2010/08/26 11:11:48 cvs Exp $
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <rdformpost.h>
#include <rdweb.h>
#include <rdcart.h>
#include <rdconf.h>

#include <rdxport.h>

void Xport::DeleteAudio()
{
  //
  // Verify Post
  //
  int cartnum=0;
  if(!xport_post->getValue("CART_NUMBER",&cartnum)) {
    RDCgiError("Missing CART_NUMBER",400);
  }
  int cutnum=0;
  if(!xport_post->getValue("CUT_NUMBER",&cutnum)) {
    RDCgiError("Missing CUT_NUMBER",400);
  }

  //
  // Process Request
  //
  if(!xport_user->deleteCarts()) {
    RDCgiError("User not authorized",401);
  }
  RDCut *cut=new RDCut(cartnum,cutnum);
  if(!cut->exists()) {
    delete cut;
    RDCgiError("No such cut",404);
  }
  unlink(RDCut::pathName(cartnum,cutnum));
  unlink(RDCut::pathName(cartnum,cutnum)+".energy");
  syslog(LOG_NOTICE,"unlink(%s): %s",(const char *)RDCut::pathName(cartnum,cutnum),strerror(errno));
  delete cut;
  printf("Content-type: text/html\n");
  printf("Status: 200\n\n");
  printf("OK\n");
  Exit(0);
}
