// edit_jack.cpp
//
// Edit a Rivendell Jack Configuration
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_jack.cpp,v 1.1 2011/10/31 11:38:33 cvs Exp $
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

#include <math.h>

#include <qdialog.h>
#include <qstring.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qcolordialog.h>
#include <qvalidator.h>
#include <qfiledialog.h>

#include <globals.h>
#include <rdcart_dialog.h>
#include <rddb.h>
#include <edit_jack.h>


EditJack::EditJack(RDStation *station,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString sql;

  edit_station=station;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  setCaption(tr("JACK Configuration for ")+edit_station->name());

  //
  // Create Fonts
  //
  QFont normal_font=QFont("Helvetica",12,QFont::Normal);
  normal_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // Start JACK Server
  //
  edit_start_jack_box=new QCheckBox(this);
  edit_start_jack_label=
    new QLabel(edit_start_jack_box,tr("Start JACK Server"),this);
  edit_start_jack_label->setFont(font);
  edit_start_jack_label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // JACK Server Name
  //
  edit_jack_server_name_edit=new QLineEdit(this);
  edit_jack_server_name_label=
    new QLabel(edit_jack_server_name_edit,tr("JACK Server Name:"),this);
  edit_jack_server_name_label->setFont(font);
  edit_jack_server_name_label->
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this,"edit_ok_button");
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(font);
  edit_ok_button->setText(tr("&OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this,"edit_cancel_button");
  edit_cancel_button->setFont(font);
  edit_cancel_button->setText(tr("&Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  edit_start_jack_box->setChecked(edit_station->startJack());
  edit_jack_server_name_edit->setText(edit_station->jackServerName());
  if(edit_jack_server_name_edit->text().isEmpty()) {
    edit_jack_server_name_edit->setText(EDITJACK_DEFAULT_SERVERNAME);
  }
}


QSize EditJack::sizeHint() const
{
  return QSize(450,125);
} 


QSizePolicy EditJack::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

void EditJack::okData()
{
  edit_station->setStartJack(edit_start_jack_box->isChecked());
  if(edit_jack_server_name_edit->text()==EDITJACK_DEFAULT_SERVERNAME) {
    edit_station->setJackServerName("");
  }
  else {
    edit_station->setJackServerName(edit_jack_server_name_edit->text());
  }

  done(0);
}


void EditJack::cancelData()
{
  done(-1);
}


void EditJack::resizeEvent(QResizeEvent *e)
{
  edit_start_jack_box->setGeometry(10,11,15,15);
  edit_start_jack_label->setGeometry(30,10,sizeHint().width()-70,20);

  edit_jack_server_name_label->setGeometry(10,32,130,20);
  edit_jack_server_name_edit->setGeometry(145,32,size().width()-155,20);

  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
