// import.cpp
//
// Rivendell web service portal -- Import service
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: import.cpp,v 1.8 2011/08/30 15:45:36 cvs Exp $
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
#include <syslog.h>

#include <rdformpost.h>
#include <rdweb.h>
#include <rdcart.h>
#include <rdaudioconvert.h>
#include <rdsettings.h>
#include <rdconf.h>
#include <rdlibrary_conf.h>

#include <rdxport.h>

void Xport::Import()
{
  unsigned length_deviation=0;
  unsigned msecs;

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
  int channels=0;
  if(!xport_post->getValue("CHANNELS",&channels)) {
    RDCgiError("Missing CHANNELS",400);
  }
  int normalization_level=0;
  if(!xport_post->getValue("NORMALIZATION_LEVEL",&normalization_level)) {
    RDCgiError("Missing NORMALIZATION_LEVEL",400);
  }
  int autotrim_level=0;
  if(!xport_post->getValue("AUTOTRIM_LEVEL",&autotrim_level)) {
    RDCgiError("Missing AUTOTRIM_LEVEL",400);
  }
  int use_metadata=0;
  if(!xport_post->getValue("USE_METADATA",&use_metadata)) {
    RDCgiError("Missing USE_METADATA",400);
  }
  QString filename;
  if(!xport_post->getValue("FILENAME",&filename)) {
    RDCgiError("Missing FILENAME",400);
  }
  if(!RDCart::exists(cartnum)) {
    RDCgiError("No such cart",404);
  }
  if(!RDCut::exists(cartnum,cutnum)) {
    RDCgiError("No such cut",404);
  }
  if(!xport_post->isFile("FILENAME")) {
    RDCgiError("Missing file data",400);
  }

  //
  // Verify User Perms
  //
  if(!xport_user->cartAuthorized(cartnum)) {
    RDCgiError("No such cart",404);
  }
  if(!xport_user->editAudio()) {
    RDCgiError("Unauthorized",401);
  }

  //
  // Load Configuration
  //
  RDCart *cart=new RDCart(cartnum);
  RDCut *cut=new RDCut(cartnum,cutnum);
  RDLibraryConf *conf=new RDLibraryConf(xport_config->stationName(),0);
  RDSettings *settings=new RDSettings();
  switch(conf->defaultFormat()) {
  case 0:
    settings->setFormat(RDSettings::Pcm16);
    break;

  case 1:
    settings->setFormat(RDSettings::MpegL2Wav);
    break;
  }
  settings->setChannels(channels);
  settings->setSampleRate(xport_system->sampleRate());
  settings->setBitRate(channels*conf->defaultBitrate());
  settings->setNormalizationLevel(normalization_level);
  RDWaveFile *wave=new RDWaveFile(filename);
  if(!wave->openWave()) {
    delete wave;
    printf("Content-type: text/html\n");
    printf("Status: %d\n",415);
    printf("\n");
    printf("Format Not Supported\n");
    exit(0);
  }
  msecs=wave->getExtTimeLength();
  delete wave;
  RDAudioConvert *conv=new RDAudioConvert(xport_config->stationName());
  conv->setSourceFile(filename);
  conv->setDestinationFile(RDCut::pathName(cartnum,cutnum));
  conv->setDestinationSettings(settings);
  RDAudioConvert::ErrorCode conv_err=conv->convert();
  switch(conv_err) {
  case RDAudioConvert::ErrorOk:
    if(use_metadata>0) {
      cart->setMetadata(conv->sourceWaveData());
      cut->setMetadata(conv->sourceWaveData());
    }
    cut->checkInRecording(xport_config->stationName(),settings,msecs);
    if(autotrim_level!=0) {
      cut->autoTrim(RDCut::AudioBoth,100*autotrim_level);
    }
    cart->updateLength();
    cart->resetRotation();
    cart->calculateAverageLength(&length_deviation);
    cart->setLengthDeviation(length_deviation);
    printf("Content-type: text/html\n");
    printf("Status: %d\n",200);
    printf("\n");
    printf("OK\n");
    break;

  case RDAudioConvert::ErrorFormatNotSupported:
    printf("Content-type: text/html\n");
    printf("Status: %d\n",415);
    printf("\n");
    printf("Format Not Supported\n");
    break;

  case RDAudioConvert::ErrorInvalidSettings:
    printf("Content-type: text/html\n");
    printf("Status: %d\n",415);
    printf("\n");
    printf("Invalid Export Settings\n");
    break;

  case RDAudioConvert::ErrorNoSource:
    printf("Content-type: text/html\n");
    printf("Status: %d\n",500);
    printf("\n");
    printf("No Source Error\n");
    break;

  case RDAudioConvert::ErrorNoDestination:
    printf("Content-type: text/html\n");
    printf("Status: %d\n",500);
    printf("\n");
    printf("No Destination Error\n");
    break;

  case RDAudioConvert::ErrorInvalidSource:
    printf("Content-type: text/html\n");
    printf("Status: %d\n",500);
    printf("\n");
    printf("Invalid Source Error\n");
    break;

  case RDAudioConvert::ErrorInternal:
    printf("Content-type: text/html\n");
    printf("Status: %d\n",500);
    printf("\n");
    printf("Internal Server Error\n");
    break;

  case RDAudioConvert::ErrorFormatError:
    printf("Content-type: text/html\n");
    printf("Status: %d\n",400);
    printf("\n");
    printf("Malformatted Source File Error\n");
    break;
  }
  delete conv;
  delete settings;
  delete conf;
  delete cut;
  delete cart;
}
