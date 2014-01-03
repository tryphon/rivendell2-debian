// rdcartdrag.cpp
//
// Stored value drag object for Rivendell carts.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcartdrag.cpp,v 1.1.2.6 2013/12/30 22:05:06 cvs Exp $
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

#include <string.h>

#include <qstringlist.h>

#include <rd.h>
#include <rdcart.h>
#include <rdcartdrag.h>

//
// Icons
//
#include "../icons/play.xpm"
#include "../icons/rml5.xpm"
#include "../icons/trashcan-16x16.xpm"

RDCartDrag::RDCartDrag(unsigned cartnum,const QPixmap *icon,QWidget *src)
  : QStoredDrag(RDMIMETYPE_CART,src)
{
  QByteArray data(15);
  data.duplicate(QString().sprintf("%06u:#000000",cartnum),15);
  setEncodedData(data);
  if(icon==NULL) {
    RDCart *cart=new RDCart(cartnum);
    switch(cart->type()) {
    case RDCart::Audio:
      setPixmap(QPixmap(play_xpm));
      break;

    case RDCart::Macro:
      setPixmap(QPixmap(rml5_xpm));
      break;

    case RDCart::All:
      break;
    }
    delete cart;
  }
  else {
    setPixmap(*icon);
  }
}


RDCartDrag::RDCartDrag(unsigned cartnum,const QColor &color,QWidget *src)
  : QStoredDrag(RDMIMETYPE_CART,src)
{
  QByteArray data(15);
  data.duplicate(QString().sprintf("%06u:",cartnum)+color.name(),15);
  setEncodedData(data);
  if(cartnum==0) {
    setPixmap(QPixmap(trashcan_xpm));
  }
  else {
    RDCart *cart=new RDCart(cartnum);
    switch(cart->type()) {
    case RDCart::Audio:
      setPixmap(QPixmap(play_xpm));
      break;
      
    case RDCart::Macro:
      setPixmap(QPixmap(rml5_xpm));
      break;
      
    case RDCart::All:
      break;
    }
    delete cart;
  }
}


bool RDCartDrag::canDecode(QMimeSource *e)
{
  return e->provides(RDMIMETYPE_CART);
}


bool RDCartDrag::decode(QMimeSource *e,unsigned *cartnum,QColor *color)
{
  QStringList f0;
  QString str(e->encodedData(RDMIMETYPE_CART));

  f0=f0.split(":",str);
  if(f0.size()!=2) {
    return false;
  }
  *cartnum=f0[0].toUInt();
  if(color!=NULL) {
    color->setNamedColor(f0[1]);
  }

  return true;
}


bool RDCartDrag::decode(QMimeSource *e,RDLogLine *ll,
			RDLogLine::TransType next_trans,int log_mach,
			bool timescale,RDLogLine::TransType trans)
{
  unsigned cartnum;

  RDCartDrag::decode(e,&cartnum);
  ll->loadCart(cartnum,next_trans,log_mach,timescale,trans);

  return true;
}
