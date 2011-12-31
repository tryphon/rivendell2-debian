// exportpeaks.cpp
//
// Rivendell web service portal -- ExportPeaks service
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: exportpeaks.cpp,v 1.3 2011/12/23 23:07:00 cvs Exp $
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

#include <rdformpost.h>
#include <rdweb.h>
#include <rdcart.h>
#include <rdaudioconvert.h>
#include <rdsettings.h>
#include <rdconf.h>

#include <rdxport.h>

void Xport::ExportPeaks()
{
  //
  // Verify Post
  //
  int cartnum=0;
  if(!xport_post->getValue("CART_NUMBER",&cartnum)) {
    RDXMLResult("Missing CART_NUMBER",400);
  }
  int cutnum=0;
  if(!xport_post->getValue("CUT_NUMBER",&cutnum)) {
    RDXMLResult("Missing CUT_NUMBER",400);
  }

  //
  // Verify User Perms
  //
  if(!xport_user->cartAuthorized(cartnum)) {
    RDXMLResult("No such cart",404);
  }

  //
  // Open Audio File
  //
  RDWaveFile *wave=new RDWaveFile(RDCut::pathName(cartnum,cutnum));
  if(!wave->openWave()) {
    RDXMLResult("No such audio",404);
  }
  if(!wave->hasEnergy()) {
    RDXMLResult("No peak data available",400);
  }

  //
  // Send Data
  //
  printf("Content-type: application/octet-stream\n\n");
  fflush(NULL);
  unsigned short *peaks=new unsigned short[wave->energySize()];
  wave->readEnergy(peaks,wave->energySize());
  write(1,peaks,sizeof(unsigned short)*wave->energySize());
}
