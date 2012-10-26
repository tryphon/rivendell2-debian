// rdmonitor.h
//
// System Monitor Applet for Rivendell
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdmonitor.h,v 1.1.2.5 2012/10/22 23:09:39 cvs Exp $
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


#ifndef RDMONITOR_H
#define RDMONITOR_H

#include <vector>

#include <qwidget.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qfontmetrics.h>
#include <qpopupmenu.h>

#include <rdconfig.h>
#include <rdmonitor_config.h>

#include <status_tip.h>

class MainWidget : public QWidget
{
  Q_OBJECT
 public:
  MainWidget(QWidget *parent=0,const char *name=0);
  QSizePolicy sizePolicy() const;

 private slots:
  void validate();
  void selectPositionData(int pos);
  void quitMainWidget();

 protected:
  void mousePressEvent(QMouseEvent *e);
  void mouseDoubleClickEvent(QMouseEvent *e);
  void paintEvent(QPaintEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  void SetSummaryState(bool state);
  void SetPosition(RDMonitorConfig *mon,RDMonitorConfig::Position pos);
  QLabel *mon_name_label;
  QLabel *mon_green_label;
  QLabel *mon_red_label;
  QTimer *mon_validate_timer;
  QFontMetrics *mon_metrics;
  StatusTip *mon_tooltip;
  QPopupMenu *mon_menu;
};


#endif  // RDMONITOR_H
