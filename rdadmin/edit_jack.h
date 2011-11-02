// edit_jack.h
//
// Edit a Rivendell Jack Configuration
//
//   (C) Copyright 2002-2011 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_jack.h,v 1.1 2011/10/31 11:38:33 cvs Exp $
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

#ifndef EDIT_JACK_H
#define EDIT_JACK_H

#include <qdialog.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qpixmap.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include <rdstation.h>

#define EDITJACK_DEFAULT_SERVERNAME QT_TR_NOOP("(default)")

class EditJack : public QDialog
{
 Q_OBJECT
 public:
  EditJack(RDStation *station,QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void okData();
  void cancelData();
  
 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *edit_start_jack_label;
  QCheckBox *edit_start_jack_box;
  QLabel *edit_jack_server_name_label;
  QLineEdit *edit_jack_server_name_edit;
  RDStation *edit_station;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
};


#endif  // EDIT_JACK_H
