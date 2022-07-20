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

#ifndef GT_WINDOW_H
#define GT_WINDOW_H

#include <QList>
#include <QMainWindow>
#include "gtcreator.h"


/** 
 * @class GTWindow gtwindow.h
 * \brief Road ground truth creation window.
 */
class GTWindow : public QMainWindow
{
    Q_OBJECT
    
public:

  /**
   * \brief Creates a road ground truth creation window.
   */
  GTWindow ();

  /**
   * \brief Creates a road ground truth creation window.
   */
  GTWindow (int *val);

  /**
   * Adds new NVM and TIL files.
   * Returns whether the TIL file is consistent.
   * @param nmap Name of the normal map file.
   * @param pts Name of the points file.
   */
  bool setBinaryLidarFiles (const std::string &nmap, const std::string &pts);

  /**
   * Adds new DTM and XYZ files.
   * Returns whether the DTM file header is consistent.
   * @param dtm Name of the DTM file.
   * @param pts Name of the points file.
   */
  bool setLidarFiles (const std::string &dtm, const std::string &pts);

  /**
   * Sets the processed points file.
   */
  void setPtsFile (const std::string &ptsName);

  /**
   * Requires DTM map and point cloud creations.
   * @param binary Indicates whether binary files are loaded.
   */
  void createMaps (bool binary = false);

  /**
   * Sets the tested sector name.
   * @param name New sector name.
   */
  void setSectorName (std::string name);

  /**
   * Sets the mask output option.
   * @param opt New option status.
   */
  void setMaskDisplay (bool opt);

  /**
   * Takes into account the option (after image load).
   */
  void runOptions ();

  /**
   * Performs a comparison of the roads with detected roads from AMREL.
   */
  void compare ();


protected:
  void closeEvent (QCloseEvent *event);


private slots:
  void open ();
  void save ();
  void updateActions ();


private:

  /** Blurred segment detection widget. */
  GTCreator *creationWidget;

  void createActions ();
  void createMenus ();
  bool saveFile (const QByteArray &fileFormat);
  QMenu *saveAsMenu;
  QMenu *fileMenu;

  QAction *openAct;
  QList<QAction *> saveAsActs;
  QAction *exitAct;

};
#endif
