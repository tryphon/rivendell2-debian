// rdmonitor.cpp
//
// System Monitor for Rivendell
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdmonitor.cpp,v 1.1.2.9 2012/10/23 14:47:48 cvs Exp $
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

#include <stdlib.h>
#include <unistd.h>

#include <qapplication.h>
#include <qpainter.h>
#include <qsqldatabase.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qpainter.h>
#include <qdir.h>
#include <qsignalmapper.h>
#include <qsqldatabase.h>

#include <dbversion.h>
#include <rd.h>
#include <rdcheck_daemons.h>
#include <rdcmd_switch.h>
#include <rdaudioinfo.h>
#include <rdstation.h>
#include <rdcut.h>
#include <rdstatus.h>
#include <rdmonitor_config.h>

#include <rdmonitor.h>

//
// Icons
//
#include "../icons/rivendell-22x22.xpm"
#include "../icons/greenball.xpm"
#include "../icons/redball.xpm"


MainWidget::MainWidget(QWidget *parent,const char *name)
  :QWidget(parent,name,Qt::WStyle_Customize|Qt::WStyle_NoBorder|Qt::WStyle_StaysOnTop|WX11BypassWM)
{
  QString str;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdmonitor","\n");
  delete cmd;

  //
  // Hide at startup
  //
  setGeometry(0,0,0,0);

  //
  // Generate Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  mon_metrics=new QFontMetrics(font);

  //
  // Create And Set Icon
  //
  setIcon(QPixmap(rivendell_xpm));

  //
  // Right Button Menu
  //
  mon_menu=new QPopupMenu(this);
  mon_menu->setCheckable(true);
  connect(mon_menu,SIGNAL(activated(int)),this,SLOT(selectPositionData(int)));
  for(int i=0;i<RDMonitorConfig::LastPosition;i++) {
    mon_menu->
     insertItem(RDMonitorConfig::positionText((RDMonitorConfig::Position)i),i);
  }

  //
  // Name Label
  //
  mon_name_label=new QLabel(this);
  mon_name_label->setFont(font);

  //
  // Status Icons
  //
  mon_green_label=new QLabel(this);
  mon_green_label->setPixmap(QPixmap(greenball_xpm));
  mon_green_label->hide();

  mon_red_label=new QLabel(this);
  mon_red_label->setPixmap(QPixmap(redball_xpm));
  mon_red_label->hide();

  //
  // Validation Timer
  //
  mon_validate_timer=new QTimer(this);
  connect(mon_validate_timer,SIGNAL(timeout()),this,SLOT(validate()));
  mon_validate_timer->start(5000);

  mon_tooltip=new StatusTip(this);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::validate()
{
  bool db_ok=false;
  bool snd_ok=false;
  int schema=0;
  std::vector<QString> bad_cuts;

  //
  // Get Configurations
  //
  RDConfig *config=new RDConfig();
  config->load();
  RDMonitorConfig *mon=new RDMonitorConfig();
  mon->load();
  mon_name_label->setText(config->label());
  SetPosition(mon,mon->position());

  //
  // Check Audio Store
  //
  snd_ok=RDAudioStoreValid(config);

  //
  // Check Database
  //
  db_ok=RDDbValid(config,&schema);

  //
  // Record Results
  //
  mon_tooltip->setStatus(QRect(0,0,size().width(),size().height()),db_ok,schema,snd_ok);
  SetSummaryState(db_ok&&(schema==RD_VERSION_DATABASE)&&snd_ok);

  delete mon;
  delete config;
}


void MainWidget::selectPositionData(int pos)
{
  RDMonitorConfig::Position position=(RDMonitorConfig::Position)pos;
  RDMonitorConfig *mon=new RDMonitorConfig();
  mon->load();
  if(position!=mon->position()) {
    SetPosition(mon,position);
    delete mon;
  }
}


void MainWidget::quitMainWidget()
{
  exit(0);
}


void MainWidget::mousePressEvent(QMouseEvent *e)
{
  if(e->button()!=QMouseEvent::RightButton) {
    e->ignore();
    return;
  }
  e->accept();
  mon_menu->setGeometry(geometry().x()+e->pos().x()+2,
			geometry().y()+e->pos().y(),
			mon_menu->sizeHint().width(),
			mon_menu->sizeHint().height());
  mon_menu->exec();
}


void MainWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
  if(e->button()!=QMouseEvent::LeftButton) {
    e->ignore();
    return;
  }
  e->accept();
  QDir dir(RD_DEFAULT_RDSELECT_DIR);
  dir.setFilter(QDir::Files|QDir::Readable);
  dir.setNameFilter("*.conf");
  if(dir.entryList().size()>1) {
    system("rdselect");
    validate();
  }
}


void MainWidget::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(QPen(Qt::black,2));
  p->drawRect(1,1,size().width(),size().height());

  delete p;
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  mon_name_label->setGeometry(10,5,size().width()-30,size().height()-10);
  mon_green_label->setGeometry(size().width()-20,5,15,20);
  mon_red_label->setGeometry(size().width()-20,5,15,20);
}


void MainWidget::SetSummaryState(bool state)
{
  if(state) {
    mon_red_label->hide();
    mon_green_label->show();
  }
  else {
    mon_green_label->hide();
    mon_red_label->show();
  }
}


void MainWidget::SetPosition(RDMonitorConfig *mon,
			     RDMonitorConfig::Position pos)
{
  int width=mon_metrics->width(mon_name_label->text())+40;
  QDesktopWidget *dw=qApp->desktop();
  switch(pos) {
  case RDMonitorConfig::UpperLeft:
    setGeometry(0,0,width,RDMONITOR_HEIGHT);
    break;

  case RDMonitorConfig::UpperCenter:
    setGeometry((dw->size().width()-width)/2,0,width,RDMONITOR_HEIGHT);
    break;

  case RDMonitorConfig::UpperRight:
    setGeometry(dw->size().width()-width,0,width,RDMONITOR_HEIGHT);
    break;

  case RDMonitorConfig::LowerLeft:
    setGeometry(0,dw->size().height()-RDMONITOR_HEIGHT,width,RDMONITOR_HEIGHT);
    break;

  case RDMonitorConfig::LowerCenter:
    setGeometry((dw->size().width()-width)/2,
		dw->size().height()-RDMONITOR_HEIGHT,width,RDMONITOR_HEIGHT);
    break;

  case RDMonitorConfig::LowerRight:
    setGeometry(dw->size().width()-width,
		dw->size().height()-RDMONITOR_HEIGHT,width,RDMONITOR_HEIGHT);
    break;

  case RDMonitorConfig::LastPosition:
    break;
  }
  for(unsigned i=0;i<mon_menu->count();i++) {
    mon_menu->setItemChecked(i,false);
  }
  mon_menu->setItemChecked(pos,true);
  mon->setPosition(pos);
  mon->save();
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  
  //
  // Load Translations
  //
  QTranslator qt(0);
  qt.load(QString(QTDIR)+QString("/translations/qt_")+QTextCodec::locale(),
	  ".");
  a.installTranslator(&qt);

  QTranslator rd(0);
  rd.load(QString(PREFIX)+QString("/share/rivendell/librd_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&rd);

  QTranslator rdhpi(0);
  rdhpi.load(QString(PREFIX)+QString("/share/rivendell/librdhpi_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&rdhpi);

  QTranslator tr(0);
  tr.load(QString(PREFIX)+QString("/share/rivendell/rdmonitor_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&tr);

  //
  // Start Event Loop
  //
  MainWidget *w=new MainWidget(NULL,"main");
  a.setMainWidget(w);
  w->show();
  return a.exec();
}
