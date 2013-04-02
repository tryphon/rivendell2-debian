// edit_station.cpp
//
// Edit a Rivendell Workstation Configuration
//
//   (C) Copyright 2002-2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_station.cpp,v 1.57.4.6 2013/03/09 00:21:11 cvs Exp $
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

#include <qdialog.h>
#include <qstring.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>

#include <rddb.h>
#include <rdconf.h>
#include <rdcatch_connect.h>
#include <rdcart_dialog.h>
#include <rdtextvalidator.h>
#include <rdescape_string.h>

#include <edit_station.h>
#include <edit_rdlibrary.h>
#include <edit_rdairplay.h>
#include <edit_rdpanel.h>
#include <edit_rdlogedit.h>
#include <edit_cartslots.h>
#include <edit_decks.h>
#include <edit_audios.h>
#include <edit_ttys.h>
#include <list_matrices.h>
#include <list_hostvars.h>
#include <edit_backup.h>
#include <view_adapters.h>
#include <list_dropboxes.h>
#include <list_encoders.h>
#include <edit_jack.h>
#include <globals.h>

EditStation::EditStation(QString sname,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  RDSqlQuery *q;
  QString sql;
  int item=0;
  char temp[256];

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",10,QFont::Normal);
  small_font.setPixelSize(10);

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  station_station=new RDStation(sname);
  station_cae_station=NULL;
  caeStationActivatedData(station_station->caeStation());

  setCaption(tr("Host: ")+sname);

  GetPrivateProfileString(RD_CONF_FILE,"Identity","Password",temp,"",255);
  station_catch_connect=new RDCatchConnect(0,this,"station_catch_connect");
  station_catch_connect->connectHost(station_station->address().toString(),
				     RDCATCHD_TCP_PORT,temp);

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this,"validator");

  //
  // Station Name
  //
  station_name_edit=new QLineEdit(this,"station_name_edit");
  station_name_edit->setGeometry(115,11,80,19);
  station_name_edit->setReadOnly(true);
  QLabel *station_name_label=new QLabel(station_name_edit,
					   tr("Ho&st Name:"),this,
					   "station_name_label");
  station_name_label->setGeometry(10,11,100,19);
  station_name_label->setFont(font);
  station_name_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Station Description
  //
  station_description_edit=new QLineEdit(this,"station_description_edit");
  station_description_edit->setGeometry(115,32,sizeHint().width()-125,19);
  station_description_edit->setMaxLength(64);
  station_description_edit->setValidator(validator);
  QLabel *station_description_label=new QLabel(station_description_edit,
					   tr("&Description:"),this,
					   "station_description_label");
  station_description_label->setGeometry(10,32,100,19);
  station_description_label->setFont(font);
  station_description_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Station Default Name
  //
  station_default_name_edit=new QComboBox(this,"station_default_name_edit");
  station_default_name_edit->setGeometry(115,53,160,19);
  station_default_name_edit->setEditable(false);
  QLabel *station_default_name_label=new QLabel(station_default_name_edit,
					   tr("Default &User:"),this,
					   "station_default_name_label");
  station_default_name_label->setGeometry(10,53,100,19);
  station_default_name_label->setFont(font);
  station_default_name_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Broadcast security model
  //
  station_broadcast_sec_edit=new QComboBox(this,"station_broadcast_sec_edit");
  station_broadcast_sec_edit->setGeometry(115,74,140,19);
  // Index values should match RDStation class enum and database schema.
  station_broadcast_sec_edit->insertItem(tr("Host"),0);
  station_broadcast_sec_edit->insertItem(tr("User"),1);
  station_broadcast_sec_edit->setEditable(false);
  QLabel *station_broadcast_sec_label=new QLabel(station_broadcast_sec_edit,
					   tr("Security Model:"),this,
					   "station_broadcast_sec_label");
  station_broadcast_sec_label->setGeometry(10,74,100,19);
  station_broadcast_sec_label->setFont(font);
  station_broadcast_sec_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Station IP Address
  //
  station_address_edit=new QLineEdit(this,"station_address_edit");
  station_address_edit->setGeometry(115,95,120,19);
  station_address_edit->setMaxLength(15);
  station_address_edit->setValidator(validator);
  QLabel *station_address_label=new QLabel(station_address_edit,
					   tr("&IP Address:"),this,
					   "station_address_label");
  station_address_label->setGeometry(10,95,100,19);
  station_address_label->setFont(font);
  station_address_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Station Editor Command
  //
  station_editor_cmd_edit=new QLineEdit(this,"station_editor_cmd_edit");
  station_editor_cmd_edit->setGeometry(115,116,sizeHint().width()-130,19);
  station_editor_cmd_edit->setMaxLength(255);
  QLabel *station_editor_cmd_label=new QLabel(station_editor_cmd_edit,
					   tr("Editor &Command:"),this,
					   "station_editor_cmd_label");
  station_editor_cmd_label->setGeometry(10,116,100,19);
  station_editor_cmd_label->setFont(font);
  station_editor_cmd_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Station Time Offset
  //
  station_timeoffset_box=new QSpinBox(this,"station_timeoffset_box");
  station_timeoffset_box->setGeometry(115,137,80,19);
  station_timeoffset_box->setRange(-RD_MAX_TIME_OFFSET,RD_MAX_TIME_OFFSET);
  station_timeoffset_box->setSuffix(tr(" mS"));
  QLabel *station_timeoffset_label=new QLabel(station_timeoffset_box,
					     tr("&Time Offset:"),this,
					     "station_timeoffset_label");
  station_timeoffset_label->setGeometry(10,137,100,19);
  station_timeoffset_label->setFont(font);
  station_timeoffset_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Startup Cart
  //
  station_startup_cart_edit=new QLineEdit(this,"station_startup_cart_edit");
  station_startup_cart_edit->setGeometry(115,158,60,19);
  station_startup_cart_edit->setMaxLength(15);
  station_startup_cart_edit->setValidator(new QIntValidator(1,999999,this));
  QLabel *station_startup_cart_label=new QLabel(station_startup_cart_edit,
					   tr("&Startup Cart:"),this,
					   "station_startup_cart_label");
  station_startup_cart_label->setGeometry(10,158,100,19);
  station_startup_cart_label->setFont(font);
  station_startup_cart_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  QPushButton *select_button=
    new QPushButton(tr("Select"),this,"select_button");
  select_button->setFont(small_font);
  select_button->setGeometry(180,157,50,22);
  connect(select_button,SIGNAL(clicked()),this,SLOT(selectClicked()));

  //
  // Cue Output
  //
  station_cue_sel=new RDCardSelector(this);
  station_cue_sel->setGeometry(100,179,120,117);
  QLabel *station_cue_sel_label=new QLabel(station_cue_sel,
					   tr("Cue &Output:"),this);
  station_cue_sel_label->setGeometry(10,179,100,19);
  station_cue_sel_label->setFont(font);
  station_cue_sel_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Heartbeat Checkbox
  //
  station_heartbeat_box=new QCheckBox(this,"station_heartbeat_box");
  //  station_heartbeat_box->setGeometry(10,182,15,15);
  station_heartbeat_box->setGeometry(10,226,15,15);
  QLabel *label=new QLabel(station_heartbeat_box,
			   tr("Enable Heartbeat"),this,
			   "station_heartbeat_label");
  label->setGeometry(30,224,150,20);
  //  label->setGeometry(30,180,150,20);
  label->setFont(font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);
  connect(station_heartbeat_box,SIGNAL(toggled(bool)),
	  this,SLOT(heartbeatToggledData(bool)));

  //
  // Filter Checkbox
  //
  station_filter_box=new QCheckBox(this,"station_filter_box");
  station_filter_box->setGeometry(210,226,15,15);
  label=new QLabel(station_filter_box,
		   tr("Use Realtime Filtering"),this,
		   "station_filter_label");
  label->setGeometry(230,226,150,20);
  label->setFont(font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Heartbeat Cart
  //
  station_hbcart_edit=new QLineEdit(this,"station_hbcart_edit");
  station_hbcart_edit->setGeometry(65,248,60,19);
  station_hbcart_edit->setReadOnly(true);
  station_hbcart_label=new QLabel(station_hbcart_edit,
				  tr("Cart:"),this,
				  "station_hbcart_label");
  station_hbcart_label->setGeometry(10,248,50,19);
  station_hbcart_label->setFont(font);
  station_hbcart_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  station_hbcart_button=new QPushButton(this,"station_hbcart_button");
  station_hbcart_button->setGeometry(140,245,60,26);
  station_hbcart_button->setFont(font);
  station_hbcart_button->setText(tr("Select"));
  connect(station_hbcart_button,SIGNAL(clicked()),
	  this,SLOT(heartbeatClickedData()));

  //
  // Heartbeat Interval
  //
  station_hbinterval_spin=new QSpinBox(this,"station_hbinterval_spin");
  station_hbinterval_spin->setGeometry(275,248,45,19);
  station_hbinterval_spin->setRange(1,300);
  station_hbinterval_label=new QLabel(station_hbinterval_spin,
				  tr("Interval:"),this,
				  "station_hbinterval_label");
  station_hbinterval_label->setGeometry(220,248,50,19);
  station_hbinterval_label->setFont(font);
  station_hbinterval_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  station_hbinterval_unit=new QLabel(tr("secs"),
				     this,"station_hbinterval_unit");
  station_hbinterval_unit->setGeometry(325,248,100,19);
  station_hbinterval_unit->setFont(font);
  station_hbinterval_unit->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // System Maintenance Checkbox
  //
  station_maint_box=new QCheckBox(this,"station_maint_box");
  station_maint_box->setGeometry(10,275,15,15);
  label=new QLabel(station_maint_box,tr("Include in System Maintenance Pool"),
		   this,"station_maint_label");
  label->setGeometry(30,273,sizeHint().width()-40,20);
  label->setFont(font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // System Services Section
  //
  label=new QLabel(tr("System Services"),this);
  label->setGeometry(30,291,110,20);
  label->setFont(font);
  label->setAlignment(AlignCenter|ShowPrefix);

  //
  // HTTP Service Host
  //
  station_http_station_box=new QComboBox(this,"station_http_station_box");
  station_http_station_box->setGeometry(145,312,200,19);
  station_http_station_box->setEditable(false);
  QLabel *station_http_station_label=new QLabel(station_http_station_box,
					   tr("HTTP Xport:"),this,
					   "station_http_station_label");
  station_http_station_label->setGeometry(11,312,130,19);
  station_http_station_label->setFont(font);
  station_http_station_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // CAE Service Host
  //
  station_cae_station_box=new QComboBox(this,"station_cae_station_box");
  station_cae_station_box->setGeometry(145,333,200,19);
  station_cae_station_box->setEditable(false);
  connect(station_cae_station_box,SIGNAL(activated(const QString &)),
	  this,SLOT(caeStationActivatedData(const QString &)));
  QLabel *station_cae_station_label=new QLabel(station_cae_station_box,
					   tr("Core Audio Engine:"),this,
					   "station_cae_station_label");
  station_cae_station_label->setGeometry(11,333,130,19);
  station_cae_station_label->setFont(font);
  station_cae_station_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  //  RDLibrary Configuration Button
  //
  QPushButton *button=new QPushButton(this,"library_button");
  button->setGeometry(10,371,80,50);
  button->setFont(font);
  button->setText(tr("RD&Library"));
  connect(button,SIGNAL(clicked()),this,SLOT(editLibraryData()));

  //
  //  RDCatch Configuration Button
  //
  button=new QPushButton(this,"tty_button");
  button->setGeometry(100,371,80,50);
  button->setFont(font);
  button->setText(tr("RDCatch"));
  connect(button,SIGNAL(clicked()),this,SLOT(editDeckData()));

  //
  //  RDAirPlay Configuration Button
  //
  button=new QPushButton(this,"airplay_button");
  button->setGeometry(190,371,80,50);
  button->setFont(font);
  button->setText(tr("RDAirPlay"));
  connect(button,SIGNAL(clicked()),this,SLOT(editAirPlayData()));

  //
  //  RDPanel Configuration Button
  //
  button=new QPushButton(this,"rdpanel_button");
  button->setGeometry(280,371,80,50);
  button->setFont(font);
  button->setText(tr("RDPanel"));
  connect(button,SIGNAL(clicked()),this,SLOT(editPanelData()));

  //
  //  RDLogEdit Configuration Button
  //
  button=new QPushButton(this,"logedit_button");
  button->setGeometry(10,431,80,50);
  button->setFont(font);
  button->setText(tr("RDLogEdit"));
  connect(button,SIGNAL(clicked()),this,SLOT(editLogEditData()));

  //
  //  RDCartSlots Configuration Button
  //
  button=new QPushButton(this);
  button->setGeometry(100,431,80,50);
  button->setFont(font);
  button->setText(tr("RDCart\nSlots"));
  connect(button,SIGNAL(clicked()),this,SLOT(editCartSlotsData()));

  //
  // Dropboxes Configuration Button
  //
  button=new QPushButton(this,"dropboxes_button");
  button->setGeometry(190,431,80,50);
  button->setFont(font);
  button->setText(tr("Dropboxes"));
  connect(button,SIGNAL(clicked()),this,SLOT(editDropboxesData()));

  //
  //  Switcher Configuration Button
  //
  button=new QPushButton(this,"switcher_button");
  button->setGeometry(280,431,80,50);
  button->setFont(font);
  button->setText(tr("Switchers\nGPIO"));
  connect(button,SIGNAL(clicked()),this,SLOT(editSwitcherData()));

  //
  //  Host Variables Configuration Button
  //
  button=new QPushButton(this,"hostvars_button");
  button->setGeometry(10,491,80,50);
  button->setFont(font);
  button->setText(tr("Host\nVariables"));
  connect(button,SIGNAL(clicked()),this,SLOT(editHostvarsData()));

  //
  //  Audio Ports Configuration Button
  //
  button=new QPushButton(this,"audio_ports_button");
  button->setGeometry(100,491,80,50);
  button->setFont(font);
  button->setText(tr("Audio\nPorts"));
  connect(button,SIGNAL(clicked()),this,SLOT(editAudioData()));

  //
  //  TTY Configuration Button
  //
  button=new QPushButton(this,"tty_button");
  button->setGeometry(190,491,80,50);
  button->setFont(font);
  button->setText(tr("Serial\nPorts"));
  connect(button,SIGNAL(clicked()),this,SLOT(editTtyData()));

  //
  //  View Adapters (Audio Resources) Configuration Button
  //
  button=new QPushButton(this,"view_adapters_button");
  button->setGeometry(280,491,80,50);
  button->setFont(font);
  button->setText(tr("Audio\nResources"));
  connect(button,SIGNAL(clicked()),this,SLOT(viewAdaptersData()));

  //
  // Encoders Configuration Button
  //
  button=new QPushButton(this,"encoders_button");
  button->setGeometry(55,551,80,50);
  button->setFont(font);
  button->setText(tr("Custom\nEncoders"));
  connect(button,SIGNAL(clicked()),this,SLOT(editEncodersData()));

  //
  // JACK Settings Button
  //
  button=new QPushButton(this,"backup_button");
  button->setGeometry(145,551,80,50);
  button->setFont(font);
  button->setText(tr("JACK\nSettings"));
  connect(button,SIGNAL(clicked()),this,SLOT(jackSettingsData()));

  //
  // Backups Configuration Button
  //
  button=new QPushButton(this,"backup_button");
  button->setGeometry(235,551,80,50);
  button->setFont(font);
  button->setText(tr("Backups"));
  connect(button,SIGNAL(clicked()),this,SLOT(editBackupsData()));

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  station_name_edit->setText(station_station->name());
  station_description_edit->setText(station_station->description());
  station_address_edit->setText(station_station->address().toString());
  station_editor_cmd_edit->setText(station_station->editorPath());
  station_timeoffset_box->setValue(station_station->timeOffset());
  unsigned cartnum=station_station->startupCart();
  if(cartnum>0) {
    station_startup_cart_edit->setText(QString().sprintf("%06u",cartnum));
  }


  if(station_station->scanned()) {
    station_cue_sel->setMaxCards(station_station->cards());
    for(int i=0;i<station_cue_sel->maxCards();i++) {
      station_cue_sel->setMaxPorts(i,station_station->cardInputs(i));
    }
  }
  else {
    QMessageBox::information(this,tr("RDAdmin - No Audio Configuration Data"),
			     tr("Channel assignments will not be available for this host as audio resource data\nhas not yet been generated.  Please start the Rivendell daemons on the host\nconfigured to run the CAE service in order to populate the audio resources database."));
    station_cue_sel->setDisabled(true);
    station_cue_sel->setDisabled(true);
  }
  station_cue_sel->setCard(station_station->cueCard());
  station_cue_sel->setPort(station_station->cuePort());

  sql="select LOGIN_NAME from USERS";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    station_default_name_edit->insertItem(q->value(0).toString());
    if(q->value(0).toString()==station_station->defaultName()) {
      station_default_name_edit->setCurrentItem(item);
    }
    item++;
  }
  delete q;
  station_broadcast_sec_edit->
    setCurrentItem((RDStation::BroadcastSecurityMode)station_station->
                     broadcastSecurity());
  if((cartnum=station_station->heartbeatCart())>0) {
    station_heartbeat_box->setChecked(true);
    station_hbcart_edit->setText(QString().sprintf("%u",cartnum));
    station_hbinterval_spin->
      setValue(station_station->heartbeatInterval()/1000);
    heartbeatToggledData(true);
  }
  else {
    station_heartbeat_box->setChecked(false);
    heartbeatToggledData(false);
  }
  switch(station_station->filterMode()) {
    case RDStation::FilterSynchronous:
      station_filter_box->setChecked(true);
      break;

    case RDStation::FilterAsynchronous:
      station_filter_box->setChecked(false);
      break;
  }
  station_maint_box->setChecked(station_station->systemMaint());

  station_http_station_box->insertItem("localhost");
  station_cae_station_box->insertItem("localhost");
  sql=QString().sprintf("select NAME from STATIONS \
                         where NAME!=\"%s\" order by NAME",
			(const char *)RDEscapeString(sname));
  q=new RDSqlQuery(sql);
  while(q->next()) {
    station_http_station_box->insertItem(q->value(0).toString());
    if(q->value(0).toString()==station_station->httpStation()) {
      station_http_station_box->
	setCurrentItem(station_http_station_box->count()-1);
    }
    station_cae_station_box->insertItem(q->value(0).toString());
    if(q->value(0).toString()==station_station->caeStation()) {
      station_cae_station_box->
	setCurrentItem(station_cae_station_box->count()-1);
    }
  }
  delete q;
}


EditStation::~EditStation()
{
  delete station_station;
  delete station_cae_station;
}


void EditStation::selectClicked()
{
  int cartnum;

  if(admin_cart_dialog->exec(&cartnum,RDCart::Macro,NULL,0,
			     admin_user->name(),admin_user->password())==0) {
    station_startup_cart_edit->setText(QString().sprintf("%06d",cartnum));
  }
}


void EditStation::heartbeatToggledData(bool state)
{
   station_hbcart_label->setEnabled(state);
   station_hbcart_edit->setEnabled(state);
   station_hbcart_button->setEnabled(state);
   station_hbinterval_label->setEnabled(state);
   station_hbinterval_spin->setEnabled(state);
   station_hbinterval_unit->setEnabled(state);
}


void EditStation::heartbeatClickedData()
{
  int cartnum;

  if(admin_cart_dialog->exec(&cartnum,RDCart::Macro,NULL,0,
			     admin_user->name(),admin_user->password())==0) {
    station_hbcart_edit->setText(QString().sprintf("%06d",cartnum));
  }
}


void EditStation::caeStationActivatedData(const QString &station_name)
{
  if(station_cae_station!=NULL) {
    delete station_cae_station;
  }
  if(station_name=="localhost") {
    station_cae_station=new RDStation(station_station->name());
  }
  else {
    station_cae_station=new RDStation(station_name);
  }
}


QSize EditStation::sizeHint() const
{
  return QSize(375,681);
} 


QSizePolicy EditStation::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditStation::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(black);
  p->setBrush(black);

  //
  // System Services
  //
  p->moveTo(10,301);
  p->lineTo(sizeHint().width()-20,301);
  p->lineTo(sizeHint().width()-20,359);
  p->lineTo(10,359);
  p->lineTo(10,301);

  delete p;
}


void EditStation::okData()
{
  QHostAddress addr;
  unsigned cartnum=0;
  bool ok=false;
  QString sql;
  RDSqlQuery *q;

  if(!station_maint_box->isChecked()) {
    sql=QString().sprintf("select NAME from STATIONS where \
                         (NAME!=\"%s\")&&(SYSTEM_MAINT=\"Y\")",
			(const char *)RDEscapeString(station_station->name()));
    q=new RDSqlQuery(sql);
    if(!q->first()) {
      QMessageBox::warning(this,tr("System Maintenance"),
	  tr("At least one host must belong to the system maintenance pool!"));
      delete q;
      return;
    }
    delete q;
  }
  if(!addr.setAddress(station_address_edit->text())) {
    QMessageBox::warning(this,tr("Invalid Address"),
			 tr("The specified IP address is invalid!"));
    return;
  }
  if(station_heartbeat_box->isChecked()) {
    cartnum=station_hbcart_edit->text().toUInt();
    if((cartnum==0)||(cartnum>999999)) {
      QMessageBox::warning(this,tr("Invalid Cart"),
			   tr("The Heartbeat Cart number is invalid!"));
      return;
    }
    station_station->setHeartbeatCart(station_hbcart_edit->text().toUInt());
    station_station->
      setHeartbeatInterval(station_hbinterval_spin->value()*1000);
  }
  else {
    station_station->setHeartbeatCart(0);
    station_station->setHeartbeatInterval(0);
  }
  station_station->setCueCard(station_cue_sel->card());
  station_station->setCuePort(station_cue_sel->port());
  station_station->setDescription(station_description_edit->text());
  station_station->setDefaultName(station_default_name_edit->currentText());
  station_station->
    setBroadcastSecurity((RDStation::BroadcastSecurityMode)
                           station_broadcast_sec_edit->currentItem());
  station_station->setAddress(addr);
  station_station->
    setEditorPath(station_editor_cmd_edit->text());
  station_station->setTimeOffset(station_timeoffset_box->value());
  cartnum=station_startup_cart_edit->text().toUInt(&ok);
  if(ok&&(cartnum<=999999)) {
    station_station->setStartupCart(cartnum);
  }
  else {
    station_station->setStartupCart(0);
  }
  if(station_filter_box->isChecked()) {
    station_station->setFilterMode(RDStation::FilterSynchronous);
  }
  else {
    station_station->setFilterMode(RDStation::FilterAsynchronous);
  }
  station_station->setSystemMaint(station_maint_box->isChecked());
  station_station->setHttpStation(station_http_station_box->currentText());
  station_station->setCaeStation(station_cae_station_box->currentText());
  station_catch_connect->reloadHeartbeat();
  station_catch_connect->reloadOffset();

  //
  // Allow the event loop to run so the packets get delivered
  //
  QTimer *timer=new QTimer(this,"ok_timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(okTimerData()));
  timer->start(1,true);
}


void EditStation::okTimerData()
{
  done(0);
}


void EditStation::cancelData()
{
  done(-1);
}


void EditStation::editLibraryData()
{
  EditRDLibrary *edit_conf=
    new EditRDLibrary(station_station,station_cae_station);
  edit_conf->exec();
  delete edit_conf;
}


void EditStation::editDeckData()
{
  EditDecks *edit_conf=new EditDecks(station_station,station_cae_station,this);
  if(edit_conf->exec()==0) {
    station_catch_connect->reload();
  }
  delete edit_conf;
}


void EditStation::editAirPlayData()
{
  EditRDAirPlay *edit_conf=
    new EditRDAirPlay(station_station,station_cae_station);
  edit_conf->exec();
  delete edit_conf;
}


void EditStation::editPanelData()
{
  EditRDPanel *edit_conf=new EditRDPanel(station_station,this);
  edit_conf->exec();
  delete edit_conf;
}


void EditStation::editLogEditData()
{
  EditRDLogedit *edit_conf=
    new EditRDLogedit(station_station,station_cae_station);
  edit_conf->exec();
  delete edit_conf;
}


void EditStation::editCartSlotsData()
{
  EditCartSlots *edit_conf=
    new EditCartSlots(station_station,this);
  edit_conf->exec();
  delete edit_conf;
}


void EditStation::viewAdaptersData()
{
  ViewAdapters *view=new ViewAdapters(station_station,this,"view");
  view->exec();
  delete view;
}


void EditStation::editAudioData()
{
  EditAudioPorts *edit_conf=new EditAudioPorts(station_station->name(),0);
  edit_conf->exec();
  delete edit_conf;
}


void EditStation::editTtyData()
{
  EditTtys *edit_conf=new EditTtys(station_station->name(),0);
  edit_conf->exec();
  delete edit_conf;
}


void EditStation::editSwitcherData()
{
  ListMatrices *list_conf=new ListMatrices(station_station->name(),this);
  list_conf->exec();
  delete list_conf;
}


void EditStation::editHostvarsData()
{
  ListHostvars *list_conf=new ListHostvars(station_station->name(),this);
  list_conf->exec();
  delete list_conf;
}

void EditStation::editBackupsData()
{
  EditBackup *edit_backup=new EditBackup(station_station,this);
  edit_backup->exec();
  delete edit_backup;
}


void EditStation::editDropboxesData()
{
  ListDropboxes *list_conf=new ListDropboxes(station_station->name(),this);
  list_conf->exec();
  station_catch_connect->reloadDropboxes();
  delete list_conf;
}


void EditStation::editEncodersData()
{
  ListEncoders *list_conf=new ListEncoders(station_station->name(),this);
  list_conf->exec();
  delete list_conf;
}


void EditStation::jackSettingsData()
{
  EditJack *d=new EditJack(station_station,this);
  d->exec();
  delete d;
}


QString EditStation::DisplayPart(QString string)
{
  for(unsigned i=0;i<string.length();i++) {
    if(string.at(i)=='|') {
      return string.right(string.length()-i-1);
    }
  }
  return string;
}


QString EditStation::HostPart(QString string)
{
  for(unsigned i=0;i<string.length();i++) {
    if(string.at(i)=='|') {
      return string.left(i);
    }
  }
  return string;
}
