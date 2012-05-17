## automake.am
##
## xdg/automake.am for Rivendell
##
##   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.11 2012/01/17 18:11:35 cvs Exp $
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License version 2 as
##   published by the Free Software Foundation.
##
##   This program is distributed in the hope that it will be useful,
##   but WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
##   You should have received a copy of the GNU General Public
##   License along with this program; if not, write to the Free Software
##   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##
## Use automake to process this into a Makefile.in

install-exec-am:	
	mkdir -p @prefix@/share/applications
	cp *.desktop @prefix@/share/applications/
	mkdir -p @prefix@/share/desktop-directories
	cp *.directory @prefix@/share/desktop-directories/
	mkdir -p /etc/xdg/menus/applications-merged
	cp *.menu /etc/xdg/menus/applications-merged/

uninstall:	
	rm -f @prefix@/share/applications/rivendell-*.desktop
	rm -f @prefix@/share/desktop-directories/rivendell-*.directory
	rm -f /etc/xdg/menus/applications-merged/rivendell-*.menu

EXTRA_DIST = rivendell-rdlibrary.desktop \
             rivendell-rdcatch.desktop \
             rivendell-rdairplay.desktop \
             rivendell-rdlogedit.desktop \
             rivendell-rdlogmanager.desktop \
             rivendell-rmlsend.desktop \
             rivendell-rdgpimon.desktop \
             rivendell-rdadmin.desktop \
             rivendell-rdlogin.desktop \
             rivendell-rdpanel.desktop \
             rivendell-rdhpiinfo.desktop \
             rivendell-rdcastmanager.desktop \
             rivendell-rdsoftkeys.desktop \
             rivendell-rdalsaconfig.desktop \
             rivendell-configuration.directory \
             rivendell-logtools.directory \
             rivendell-rivendell.directory \
             rivendell-utilities.directory \
             rivendell-rivendell.menu

CLEANFILES = *~
MAINTAINERCLEANFILES = *~ Makefile.in configure aclocal.m4
