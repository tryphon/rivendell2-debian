// rdpaths.h
//
// Install-Dependent Path Values for Rivendell
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdpaths.h.in,v 1.6 2011/03/26 01:37:33 cvs Exp $
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

#ifndef RDPATHS_H
#define RDPATHS_H

/*
 * Rivendell Loadable Modules Directory
 */
#define RD_MODULES_DIR "/usr/local/lib64/rivendell"

/*
 * Default Skins
 */
#define RD_DEFAULT_RDAIRPLAY_SKIN "/usr/local/share/pixmaps/rivendell/rdairplay_skin.png"
#define RD_DEFAULT_RDPANEL_SKIN "/usr/local/share/pixmaps/rivendell/rdpanel_skin.png"

/*
 * CD Paranoia Includes
 */
extern "C" {
#include <cdda/cdda_interface.h>
}

/*
 * Installation Prefix
 */
#define RD_PREFIX "/usr/local"


#endif  // RDPATHS_H
