// rdmonitor_config.h
//
// A container class for a Rivendell RDMonitor Configuration
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdmonitor_config.h,v 1.1.2.1 2012/10/22 17:49:33 cvs Exp $
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

#ifndef RDMONITOR_CONFIG_H
#define RDMONITOR_CONFIG_H

#include <qstring.h>

class RDMonitorConfig
{
 public:
  enum Position {UpperLeft=0,UpperCenter=1,UpperRight=2,
		 LowerLeft=3,LowerCenter=4,LowerRight=5,
		 LastPosition=6};
  RDMonitorConfig();
  Position position() const;
  void setPosition(RDMonitorConfig::Position pos);
  bool load();
  bool save();
  void clear();
  static QString positionText(Position pos);

 private:
  Position mon_position;
  QString mon_filename;
};


#endif  // RDMONITOR_CONFIG_H
