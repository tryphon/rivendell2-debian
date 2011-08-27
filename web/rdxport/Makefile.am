## automake.am
##
## Automake.am for rivendell/web/rdxport
##
##   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.6 2011/02/17 20:58:00 cvs Exp $
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -DQTDIR=\"@QT_DIR@\" @QT_CXXFLAGS@
INCLUDES = -I$(top_srcdir)/lib
LIBS = @QT_LIBS@ -L$(top_srcdir)/lib
MOC = @QT_MOC@

libexec_PROGRAMS = rdxport.cgi

install-exec-hook:	
	chown root $(libexecdir)/rdxport.cgi
	chmod 4755 $(libexecdir)/rdxport.cgi

dist_rdxport_cgi_SOURCES = audioinfo.cpp\
                           carts.cpp\
                           copyaudio.cpp\
                           deleteaudio.cpp\
                           groups.cpp\
                           export.cpp\
                           exportpeaks.cpp\
                           import.cpp\
                           rdxport.cpp rdxport.h\
                           trimaudio.cpp

rdxport_cgi_LDADD = @LIB_RDLIBS@ -lsndfile @LIBVORBIS@

EXTRA_DIST = rdxport.pro

CLEANFILES = *~ moc_* *.qm *.obj *.idb *.pdb *ilk
MAINTAINERCLEANFILES = *~ Makefile.in configure aclocal.m4 *.tar.gz moc_*
