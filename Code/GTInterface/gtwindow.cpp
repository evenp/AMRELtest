/*  Copyright 2021 Philippe Even and Phuc Ngo,
      authors of paper:
      Even, P., and Ngo, P., 2021,
      Automatic forest road extraction fromLiDAR data of mountainous areas.
      In the First International Joint Conference of Discrete Geometry
      and Mathematical Morphology (Springer LNCS 12708), pp. 93-106.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "gtwindow.h"
#include <QtGui>
#include <QFileDialog>
#include <QMenuBar>
#include <iostream>


GTWindow::GTWindow (int *val)
{
  Q_UNUSED (val);
  creationWidget = new GTCreator;
  setCentralWidget (creationWidget);
  // setFocus();  
  // createActions ();
  // createMenus ();
  setWindowTitle (tr ("ASD"));
  resize (400, 400);
}


GTWindow::GTWindow ()
{
  creationWidget = new GTCreator;
  setCentralWidget (creationWidget);
  // setFocus ();
  // createActions ();
  // createMenus ();
  setWindowTitle (tr ("ASD"));
  resize (400, 400);
}


bool GTWindow::setBinaryLidarFiles (const std::string &nmap,
                                    const std::string &pts)
{
  return (creationWidget->addBinaryFiles (nmap, pts));
}


bool GTWindow::setLidarFiles (const std::string &dtm, const std::string &pts)
{
  return (creationWidget->addFiles (dtm, pts));
}


void GTWindow::createMaps (bool binary)
{
  resize (creationWidget->createMap (binary));
}


void GTWindow::setSectorName (std::string name)
{
  creationWidget->setSectorName (name);
}


void GTWindow::runOptions ()
{
}


void GTWindow::compare ()
{
  creationWidget->compare ();
}


void GTWindow::closeEvent (QCloseEvent *event)
{
  event->accept ();
}


void GTWindow::open ()
{
  QSize windowSize;
  QString fileName = QFileDialog::getOpenFileName (this,
                                     tr ("Open File"), QDir::currentPath ());
  if (! fileName.isEmpty ())
  {
//    windowSize = creationWidget->openDtm ("Data/MNT_50cm_9705_67750.asc",
//                                           gradType);
    updateActions ();
  }
}


void GTWindow::save ()
{
  QAction *action = qobject_cast < QAction *> (sender ());
  QByteArray fileFormat = action->data().toByteArray ();
  saveFile (fileFormat);
}


void GTWindow::updateActions ()
{
}


void GTWindow::createActions ()
{
  openAct = new QAction (tr ("&Open..."), this);
  openAct->setShortcut (tr ("Ctrl+O"));
  connect (openAct, SIGNAL (triggered ()), this, SLOT (open()));

  foreach (QByteArray format, QImageWriter::supportedImageFormats ())
  {
    QString text = tr("%1...").arg (QString(format).toUpper ());
    QAction *action = new QAction (text, this);
    action->setData (format);
    connect (action, SIGNAL (triggered ()), this, SLOT (save ()));
    saveAsActs.append (action);
  }
  exitAct = new QAction (tr ("E&xit"), this);
  exitAct->setShortcut (tr ("Ctrl+Q"));
  connect (exitAct, SIGNAL (triggered ()), this, SLOT (close ()));
}


void GTWindow::createMenus ()
{
  saveAsMenu = new QMenu (tr ("&Save As"), this);
  foreach (QAction *action, saveAsActs) saveAsMenu->addAction (action);
  fileMenu = new QMenu (tr ("&File"), this);
  fileMenu->addAction (openAct);
  fileMenu->addMenu (saveAsMenu);
  fileMenu->addSeparator ();
  fileMenu->addAction (exitAct);
  menuBar()->addMenu (fileMenu);
}


bool GTWindow::saveFile (const QByteArray &fileFormat)
{
  QString initialPath = QDir::currentPath () + "/untitled." + fileFormat;
  QString fileName = QFileDialog::getSaveFileName (this, tr ("Save As"),
                       initialPath, tr ("%1 Files (*.%2);;All Files (*)")
                           .arg(QString(fileFormat.toUpper()))
                           .arg(QString(fileFormat)));
  if (fileName.isEmpty ()) return false;
  else return creationWidget->saveAugmentedImage (fileName, fileFormat);
}
