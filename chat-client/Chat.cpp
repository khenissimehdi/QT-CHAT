#include <QMessageBox>
#include "Chat.h"

#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// Chat ////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Processeurs.
const std::map<QString, Chat::Processor> Chat::PROCESSORS {
  {"#error", &Chat::process_error},
    {"#alias",&Chat::process_alias},
    {"#connected",&Chat::process_connected},
    {"#disconnected",&Chat::process_disconnected},
    {"#renamed",&Chat::process_renamed},
    {"#list",&Chat::process_list},
    {"#private",&Chat::process_private}


};

// Constructeur.
Chat::Chat (const QString & host, quint16 port, QObject * parent) :
  QObject (parent),
  socket ()
{
  // Signal "connected" émis lorsque la connexion est effectuée.
  // TODO

    connect(&socket,&QTcpSocket::connected,[this]
    {
        emit connected(socket.peerName(),socket.peerPort());

    });


  // Signale "disconnected" émis lors d'une déconnexion du socket.
  // TODO
    connect(&socket,&QTcpSocket::disconnected,this,&Chat::disconnected);
  // Lecture.
  connect (&socket, &QIODevice::readyRead, [this] () {
    // Tant que l'on peut lire une ligne...
    while (socket.canReadLine ())
    {
      // Lecture d'une ligne et suppression du "newline".
      QByteArray m = socket.readLine ();
      QByteArray n = m.left(m.length()-1);

      // Flot de lecture.
      QTextStream stream (&m);
      // Lecture d'une commande potentielle.
      QString command;
      stream >> command;

      // Recherche de la commande serveur dans le tableau associatif.
      // - si elle existe, traitement du reste du message par le processeur ;
      // - sinon, émission du signal "message" contenant la ligne entière.
      // TODO
      /*
       * we are just creating a table that contains functions
       */
//      typedef ,void(Chat::*Processor)(QTextStream&);
//        Chat::PROCESSORS = std::map(QString,Processor)
          auto it = PROCESSORS.find(command);
          if (it != PROCESSORS.end()){
              (this->*it->second)(stream);

          }
          else
          {
                emit message(m);
          }

      }





  });

  // cCONNEXION !
  socket.connectToHost (host, port, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
}

Chat::~Chat ()
{
  // Déconnexion des signaux.
  socket.disconnect ();
}

// Commande "#alias"
// TODO
void Chat::process_alias(QTextStream & alias)
{
  QString al;
  alias >> al >> ws;
  emit user_alias(al);
}
// Commande "#connected"
// TODO
void Chat::process_connected(QTextStream & alias)
{
  QString al;
  alias >> al >> ws;
  emit user_connected(al);
}

// Commande "#disconnected"
// TODO
void Chat::process_disconnected(QTextStream & alias)
{
  QString al;
  alias >> al >> ws;
  emit user_disconnected(al);
}

// Commande "#renamed"
// TODO
void Chat::process_renamed(QTextStream & renamed)
{
  QString oldName;
  renamed >> oldName >> ws;
  QString newName;
  renamed >> dec >> newName >> ws;
  emit user_renamed(oldName,newName);
  this->write("/list");
}

// Commande "#list"
// TODO
void Chat::process_list(QTextStream & li)
{
  QStringList liste;
  while (li.status() == QTextStream::Ok) {
      QString s ;
      li >> s >> ws ;
      liste.append(s);

  }
  this->write("/list");
  emit user_list(liste);
}

// Commande "#private"
// TODO
void Chat::process_private(QTextStream & alias)
{
  QString al = alias.readAll();
  emit user_private(al);
}

// Commande "#error"
void Chat::process_error (QTextStream & is)
{
  QString id;
  is >> id >> ws;
  emit error (id);
}

// Envoi d'un message à travers le socket.
void Chat::write (const QString & message)
{
  socket.write (message.toUtf8 () + '\n');
}

////////////////////////////////////////////////////////////////////////////////
// ChatWindow //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

ChatWindow::ChatWindow (const QString & host, quint16 port, QWidget * parent) :
  QMainWindow (parent),
  chat (host, port, this),
  text (this),
  input (this)
{
  text.setReadOnly (true);
  setCentralWidget (&text);

  // Insertion du la zone de saisie.
  // QDockWidget insérable en haut ou en bas, inséré en bas.
  // TODO
  QDockWidget *dock = new QDockWidget(this);
  dock->setWidget(&input);
  //dock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
  //dock->resize(100, dock->height());
  addDockWidget(Qt::BottomDockWidgetArea,dock);
  QDockWidget *droite = new QDockWidget(this);
  QListWidget *list = new QListWidget(this);
  droite->setWidget(list);
  addDockWidget(Qt::RightDockWidgetArea,droite);

  // Désactivation de la zone de saisie.
  input.setEnabled (false);

  // Envoi de messages lorsque la touche "entrée" est pressée.
  // - transmission du texte au moteur de messagerie instantanée ;
  // - effacement de la zone de saisie.
  // TODO
  //we don't have parm like in connect(message) cuz return pressed don't have parms
  connect(&input,&QLineEdit::returnPressed,[this]
  {
      chat.write(input.text());
      input.clear();
  });


  // Connexion.
  // - affichage d'un message confirmant la connexion ;
  // - saisie de l'alias ;
  // - envoi de l'alias ;
  // - activation de la zone de saisie.
  // TODO
  connect(&chat,&Chat::connected,[this] (const QString & host, quint16 port)
  {
      text.append("<h1>connected</h1>");
      QString alias = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                               tr("User name:"));

      chat.write(alias);
      input.setEnabled (true);



          });


  // Déconnexion.
  // - désactivation de la zone de saisie.
  // - affichage d'un message pour signaler la déconnexion.
  // TODO

  connect(&chat,&Chat::disconnected,[this]
  {
      input.setEnabled (false);

      text.append("<h1>disconnected</h1>");




          });
  // Messages.
  connect (&chat, &Chat::message, [this] (const QString & message) {
    text.append (message);
  });

  // Liste des utilisateurs.
  // Connexion d'un utilisateur.
  // Déconnexion d'un utilisateur.
  // Nouvel alias d'un utilisateur.
  // Message privé.
  // TODO

  // Gestion des erreurs.
  connect (&chat, &Chat::error, [this] (const QString & id) {
    QMessageBox::critical (this, tr("Error"), id);
  });
 // Gestion des connexion.
  connect (&chat, &Chat::user_connected, [this] (const QString & id) {
    text.append(tr("conex:")+id);
  });
  // Gestion des connexion.
   connect (&chat, &Chat::user_disconnected, [this] (const QString & id) {
     text.append(tr("dex:")+id);
   });
   // Gestion des alias.
   connect (&chat, &Chat::user_renamed, [this] (const QString & oldName,const QString & newName) {
     text.append(tr("old name was :")+oldName+tr(" new name is :")+newName);
   });
   // Gestion des listes.
   connect (&chat, &Chat::user_list, [this,list] (const QStringList & li) {

     while(list->count()!= 0)
     {
        list->takeItem(0);
     }
      list->addItems(li);
     //text.append(tr("users are :")+li);
   });
   // Gestion de Message privé.
   connect (&chat, &Chat::user_private, [this] (const QString & id) {
     text.append(tr("This a private message :")+id);
   });
  // CONNEXION !
  text.append (tr("<b>Connecting...</b>"));
}

