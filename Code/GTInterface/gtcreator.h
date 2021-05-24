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

#ifndef GT_CREATOR_H
#define GT_CREATOR_H

#include <QGraphicsView>
#include <QColor>
#include <QImage>
#include <QWidget>
#include <QVector>
#include <fstream>
#include "pt3f.h"
#include "terrainmap.h"
#include "astrack.h"
#include "asarea.h"
#include "ipttileset.h"


/** 
 * @class GTCreator gtcreator.h
 * \brief Road ground truth creation widget.
 */
class GTCreator : public QWidget 
{
  Q_OBJECT


public:

  /**
   * \brief Creates a road ground truth creator.
   */
  GTCreator (QWidget *parent = 0);

  /**
   * \brief Deletes the ground truth creation widget.
   */
  ~GTCreator ();

  /**
   * \brief Adds new normal map and point cloud files.
   * Return whether the tile file is consistent.
   * @param nmap Name of the normal map file.
   * @param pts Name of the cloud file.
   */
  bool addBinaryFiles (const std::string &nmap, const std::string &pts);

  /**
   * \brief Adds new digital terrain model and point cloud files.
   * Return whether the DTM file header is consistent.
   * @param dtm Name of the DTM file.
   * @param pts Name of the cloud file.
   */
  bool addFiles (const std::string &dtm, const std::string &pts);

  /**
   * \brief Creates and distributes DTM images.
   * Returns the size of the DTM image.
   * @param binary Indicates whether binary files are loaded.
   */
  QSize createMap (bool binary);

  /**
   * Sets the tested sector name.
   */
  void setSectorName (std::string name);

  /**
   * \brief Applies default options (read in defaults files).
   */
  void setDefaults ();

  /**
   * \brief Builds and returns the image bitmap.
   */
  int **getBitmap (const QImage &image);
  
  /**
   * \brief Updates the Qt widget display.
  void paint (QPainter *painter,
              const QStyleOptionGraphicsItem *option, QWidget *widget);
   */

  /**
   * \brief Toggles the background image.
   */
  void toggleBackground ();

  /**
   * \brief Saves the augmented image with extraction results.
   */
  bool saveAugmentedImage (const QString &fileName, const char *fileFormat);

  /**
   * Performs a comparison of the roads with detected roads from AMREL.
   */
  void compare ();


public slots:
  /**
   * \brief Clears the widget drawing.
   */
  void clearImage ();


protected:
  /**
   * \brief Updates the widget drawing.
   */
  void paintEvent (QPaintEvent *event);

  /**
   * \brief Processes mouse press events.
   */
  void mousePressEvent (QMouseEvent *event);

  /**
   * \brief Processes mouse release events.
   */
  void mouseReleaseEvent (QMouseEvent *event);

  /**
   * \brief Processes move release events.
   */
  void mouseMoveEvent (QMouseEvent *event);

  /**
   * \brief Processes key press events.
   */
  void keyPressEvent (QKeyEvent *event);


private:
 
  /** Background status : uniform black. */
  static const int BACK_BLACK;
  /** Background status : uniform white. */
  static const int BACK_WHITE;
  /** Background status : intensity image displayed. */
  static const int BACK_IMAGE;
  /** Background status : gradient image displayed. */
  static const int BACK_GRAD;
  /** Background status : X-gradient image displayed. */
  static const int BACK_GRADX;
  /** Background status : Y-gradient image displayed. */
  static const int BACK_GRADY;

  /** Default value for pen width. */
  static const int DEFAULT_PEN_WIDTH;
  /** Tolerence for segment picking (in count of naive lines) */
  static const int SELECT_TOL;

  /** Point cloud / Dtm image ratio. */
  static const int SUBDIV;


  /** Initial scan start point. */
  Pt2i p1;
  /** Initial scan end point. */
  Pt2i p2;
  /** Saved scan start point. */
  Pt2i oldp1;
  /** Saved scan end point. */
  Pt2i oldp2;
  /** Flag indicating if the mouse is not dragging. */
  bool nodrag;
  /** Flag indicating if picking mode is set. */
  bool picking;
  /** Flag indicating if the detection is user defined. */
  bool udef;
  /** Saved user definition flag. */
  bool oldudef;

  int background;
  /** Black level used to lighten background images. */
  int blevel;
  /** Publication color modality. */
  bool dispPub;
  /** Accepted plateaux display modality. */
  bool acceptation;
  /** Flag indicating whether detection stats should be output. */
  bool statsOn;
  /** Flag indicating whether detection result should be output. */
  bool verbose;
  /** Selection stroke color. */
  QColor selectionColor;
  /** Tile borders display modality. */
  bool tiledisp;

  /** Presently loaded image. */
  QImage loadedImage;
  /** Present image augmented with processed data. */
  QImage augmentedImage;
  /** Points cloud. */
  IPtTileSet ptset;
  /** Width of the present image. */
  int width;
  /** Height of the present image. */
  int height;
  /** Image to meter ratio : inverse of cell size. */
  float iratio;
  /** DTM cell size. */
  float cellsize;

  /** Maximal window width. */
  int maxWidth;
  /** Maximal window height. */
  int maxHeight;
  /** Maximal value of x-scroll shift. */
  int xMaxShift;
  /** Maximal value of y-scroll shift. */
  int yMaxShift;
  /** X-scroll shift. */
  int xShift;
  /** Y-scroll shift. */
  int yShift;
  /** Window zoom. */
  int zoom;
  /** Window dezoom. */
  int dezoom;

  /** DTM normal map. */
  TerrainMap dtm_map;
  /** Delineated road. */
  ASTrack *trac;
  /** Saved roads. */
  std::vector<ASTrack> old_roads;
  /** Old roads point display modality. */
  bool allPoints;

  /** Loaded sector name. */
  std::string sector_name;
  /** Area selection modality status. */
  bool area_mode;
  /** Selected area. */
  ASArea *area;


  /**
   * \brief Loads already saved roads.
   */
  bool loadRoadSet (const std::string &fname);

  /**
   * \brief Rebuilds the background image after lighting modification.
   */
  void rebuildImage ();

  /**
   * \brief Selects the road that owns the selected point.
   * @param x Selected point X-coordinate.
   * @param y Selected point Y-coordinate.
   */
  ASTrack *select (int x, int y);

  /**
   * \brief Switches area selection modality on or off.
   */
  void switchArea ();

  /**
   * \brief Draws a list of points with the given color.
   * @param painter Drawing device.
   * @param pts List of points to be drawn.
   * @param color Drawing color.
   */
  void drawPoints (QPainter &painter,
                   std::vector<Pt2i> pts, QColor color);

  /**
   * \brief Draws a list of image pixels.
   * @param painter Drawing device.
   * @param pix List of pixels to be drawn.
   */
  void drawPixels (QPainter &painter, std::vector<Pt2i> pix);

  /**
   * \brief Draws the line joining two points.
   * @param painter Drawing device.
   * @param from Line start position.
   * @param to Line reach position.
   * @param color Drawing color.
   */
  void drawLine (QPainter &painter,
                 const Pt2i from, const Pt2i to, QColor color);

  /**
   * \brief Draws a user selection.
   * @param painter Drawing device.
   * @param from Selection line start position.
   * @param to Selection line reach position.
   */
  void drawSelection (QPainter &painter, const Pt2i from, const Pt2i to);

  /**
   * \brief Draws rectangular areas.
   * @param painter Drawing device.
   */
  void drawArea (QPainter &painter);

  /**
   * \brief Draws tile borders.
   * @param painter Drawing device.
   */
  void drawTiles (QPainter &painter);

  /**
   * \brief Returns the background black level.
   */
  inline int getBlackLevel () const { return (blevel); }

  /**
   * \brief Increments the background black level.
   * @param val Increment value;
   */
  void incBlackLevel (int val);

  /**
   * \brief Returns the lightening angle.
   */
  float getLightAngle () const;

  /**
   * \brief Increments the lightening angle.
   * @param val Increment direction;
   */
  void incLightAngle (int val);

  /**
   * \brief Lighten the image according to the black level set.
   * @param im Image to lighten.
   */
  void lighten (QImage &im);

  /**
   * \brief Writes the stats of the last detection in a file.
   */
  void writeStats ();

  /**
   * \brief Writes the result of the last detection in a file.
   */
  void writeDetectionResult ();

  /**
   * \brief Displays the saved blurred segments.
   */
  void displaySavedSegments ();

  /**
   * \brief Registers the last extracted blurred segment.
   * Returns the count of registered segments more.
   */
  int saveExtractedSegment ();

  /**
   * \brief Clears off the saved blurred segments.
   */
  void clearSavedSegments ();

  /**
   * \brief Displays the detected carriage road.
   */
  void display ();

  /**
   * \brief Displays the window background (no detection).
   */
  void displayBackground ();

  /**
   * \brief Displays the road selection.
   */
  void displaySelectionResult ();

  /**
   * \brief Displays the stored strokes and associated carriage roads.
   */
  void displayStoredStrokesDetection ();

};
#endif
