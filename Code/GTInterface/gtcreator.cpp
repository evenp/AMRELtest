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

#include <QtGui>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "gtcreator.h"
#include "gtperftest.h"

#define TXT_SUFF ".txt"
#define IM_SUFF ".png"
#define LASTAREA_NAME "../Data/areas/last.txt"
#define LASTROAD_NAME "../Data/roads/last.txt"
#define DETECT_PREF "../Data/detections/roads_"
#define ROADSET_PREF "../Data/roadsets/rgt_"
#define ROADS_PREF "../Data/roads/track_"
#define ROADS_SUFF ".txt"
#define AREA_PREF "../Data/areas/area_"
#define CAPT_PREF "../Data/outputs/capture_"

const std::string GTCreator::VERSION = "1.1.4";

const int GTCreator::BACK_BLACK = 0;
const int GTCreator::BACK_WHITE = 1;
const int GTCreator::BACK_IMAGE = 2;

const int GTCreator::DEFAULT_PEN_WIDTH = 1;
const int GTCreator::LARGE_ROAD_WIDTH = 16;
const int GTCreator::SELECT_TOL = 5;



GTCreator::GTCreator (QWidget *parent)
{
  Q_UNUSED (parent);

  // Sets initial user inputs parameters
  setFocus ();
  grabKeyboard ();
  picking = false;
  udef = false;
  nodrag = true;
  tile_disp = true;
  mask_disp = false;

  // Initializes the maps and the auxiliary views
  iratio = 1.0f;
  cellsize = 1.0f;

  // Sets initial user outputs parameters
  verbose = false;
  statsOn = false;
  background = BACK_IMAGE;
  blevel = 0;
  dispPub = false;

  // Sets display parameters
  selectionColor = Qt::red;

  maxWidth = 768;
  maxHeight = 512;
  xMaxShift = 0;
  yMaxShift = 0;
  xShift = 0;
  yShift = 0;
  zoom = 1;
  dezoom = 1;

  allPoints = false;
  trac = new ASTrack ();

  sector_name = std::string ("");
  area_mode = false;
  area = NULL;
}


GTCreator::~GTCreator ()
{
}


bool GTCreator::addBinaryFiles (const std::string &nmap,
                                        const std::string &pts)
{
  dtm_map.addNormalMapFile (nmap);
  return (ptset.addTile (pts));
}


bool GTCreator::addFiles (const std::string &dtm,
                                  const std::string &pts)
{
  bool success = dtm_map.addDtmFile (dtm);
  if (success)
  { 
    width = dtm_map.width ();
    height = dtm_map.height ();
    ptset.addTile (pts);
  }
  return success;
}


QSize GTCreator::createMap (bool binary)
{
  if (! ptset.create ()) return QSize (0, 0);
  if (binary)
  {
    if (! dtm_map.assembleMap (ptset.columnsOfTiles (), ptset.rowsOfTiles (),
                               ptset.xref (), ptset.yref ()))
      return QSize (0, 0);
  }
  else if (! dtm_map.create ()) return QSize (0, 0);
  width = dtm_map.width ();
  height = dtm_map.height ();
  cellsize = dtm_map.cellSize ();

  iratio = width / ptset.xmSpread ();

  loadedImage = QImage (width, height, QImage::Format_RGB32);
  for (int j = 0; j < height; j++)
    for (int i = 0; i < width; i++)
    {
      int val = dtm_map.get (i, j);
      loadedImage.setPixel (i, j, val + val * 256 + val * 256 * 256);
    }
  augmentedImage = loadedImage;

  update ();

  xMaxShift = (width > maxWidth ? maxWidth - width : 0);
  yMaxShift = (height > maxHeight ? maxHeight - height : 0);
  xShift = xMaxShift / 2;
  if (xShift > 0) xShift = 0;
  yShift = yMaxShift / 2;
  if (yShift > 0) yShift = 0;
  return QSize (width, height).boundedTo (QSize (maxWidth, maxHeight));
}


void GTCreator::setSectorName (std::string name)
{
  sector_name = name;
}


void GTCreator::setMaskDisplay (bool opt)
{
  mask_disp = opt;
}


bool GTCreator::loadRoadSet (const std::string &fname)
{
  old_roads.clear ();
  std::ifstream ifile (fname.c_str (), std::ios::in);
  if (! ifile) return false;
  char name[200];
  ifile >> name;
  while (! ifile.eof ())
  {
    std::cout << "Chargement de " << name << std::endl;
    std::string trname (ROADS_PREF);
    trname += std::string (name) + std::string (ROADS_SUFF);
    ASTrack tr;
    if (! tr.load (trname, ptset.xref (), ptset.yref (), 500))
      std::cout << "No " << trname << " file found" << std::endl;
    else old_roads.push_back (tr);
    ifile >> name;
  }
  ifile.close ();
  return true;
}


void GTCreator::rebuildImage ()
{
  for (int j = 0; j < height; j++)
    for (int i = 0; i < width; i++)
    {
      int val = dtm_map.get (i, j);
      loadedImage.setPixel (i, j, val + val * 256 + val * 256 * 256);
    }
  augmentedImage = loadedImage;
}


int **GTCreator::getBitmap (const QImage &image)
{
  int w = image.width ();
  int h = image.height ();
  
  int **tabImage = new int*[h];
  for (int i = 0; i < h; i++)
  {
    tabImage[i] = new int[w];
    for(int j = 0; j < w; j++)
    {
      QColor c = QColor (image.pixel (j, h - i - 1));
      tabImage[i][j] = c.value ();
    }
  }
  return tabImage;
}


void GTCreator::toggleBackground ()
{
  if (background++ == BACK_IMAGE) background = BACK_BLACK;
}


bool GTCreator::saveAugmentedImage (const QString &fileName,
                                            const char *fileFormat)
{
  QImage aImage = augmentedImage;
  return (aImage.save (fileName, fileFormat));
}


void GTCreator::clearImage ()
{
  augmentedImage.fill (qRgb (255, 255, 255));
  update ();
}


void GTCreator::paintEvent (QPaintEvent *)
{
  QPainter painter (this);
  if (dezoom > 1)
  {
    QImage zoomImage = augmentedImage.scaled (width / dezoom, height / dezoom);
    painter.drawImage (QPoint (xShift, yShift), zoomImage);
  }
  else if (zoom > 1)
  {
    QImage zoomImage = augmentedImage.scaled (width * zoom, height * zoom);
    painter.drawImage (QPoint (xShift, yShift), zoomImage);
  }
  else painter.drawImage (QPoint (xShift, yShift), augmentedImage);
}


void GTCreator::mousePressEvent (QMouseEvent *event)
{
  int ex = (dezoom * (event->pos().x () - xShift)) / zoom;
  int ey = height - 1 - (dezoom * (event->pos().y () - yShift)) / zoom;

  if (event->button () == Qt::RightButton)
  {
    ASTrack *selroad = select (ex, ey);
    if (selroad != NULL)
    {
      std::cout << "Selected: " << selroad->getName () << std::endl;
      trac = selroad;
    }
    displaySelectionResult ();
  }
  else
  {
    udef = true;
    if (area_mode)
    {
      int xc = ex;
      if (xc < 0) xc = 0;
      if (xc >= width) xc = width - 1;
      int yc = ey;
      if (yc < 0) yc = 0;
      if (yc >= height) yc = height - 1;
      area->addCorner (xc, yc);
    }
    else if (! trac->selectPoint (ex, ey)) trac->addPoint (ex, ey);
    displaySelectionResult ();
  }
}


void GTCreator::mouseReleaseEvent (QMouseEvent *event)
{
  if (udef)
  {
    int ex = (dezoom * (event->pos().x () - xShift)) / zoom;
    int ey = height - 1 - (dezoom * (event->pos().y () - yShift)) / zoom;
    trac->movePoint (ex, ey);
    displaySelectionResult ();
  }
}


void GTCreator::mouseMoveEvent (QMouseEvent *event)
{
  int ex = (dezoom * (event->pos().x () - xShift)) / zoom;
  int ey = height - 1 - (dezoom * (event->pos().y () - yShift)) / zoom;
  udef = false;
  trac->movePoint (ex, ey);
  displaySelectionResult ();
}


void GTCreator::keyPressEvent (QKeyEvent *event)
{
  if (isActiveWindow ()) switch (event->key ())
  {
    case Qt::Key_B :
      if (event->modifiers () & Qt::ControlModifier)
      {
        // Toggles background image
        toggleBackground ();
        if (p1.equals (p2)) displayBackground ();
        else displaySelectionResult ();
      }
      else
      {
        // Tunes the background image black level
        incBlackLevel ((event->modifiers () & Qt::ShiftModifier) ? -1 : 1);
        std::cout << "Background black level = " << getBlackLevel ()
                  << std::endl;
        displaySelectionResult ();
      }
      break;

    case Qt::Key_C :
      rebuildImage ();
      displaySelectionResult ();
      break;

    case Qt::Key_F :
      switchArea ();
      std::cout << "Area mode " << (area_mode ? "on" : "off") << std::endl;
      break;

    case Qt::Key_G :
      if (! trac->load (std::string (LASTROAD_NAME),
                       ptset.xref (), ptset.yref (), 500))
        std::cout << "No " << LASTROAD_NAME << " file found" << std::endl;
      displaySelectionResult ();
      break;

    case Qt::Key_H :
      if (event->modifiers () & Qt::ControlModifier)
      {
        dispPub = ! dispPub;
        displaySelectionResult ();
      }
      break;

    case Qt::Key_K :
      if (event->modifiers () & Qt::ControlModifier)
      {
        allPoints = ! allPoints;
        displaySelectionResult ();
      }
      break;

    case Qt::Key_L :
      if (event->modifiers () & Qt::ControlModifier)
      {
        // Switches shading type
        dtm_map.toggleShadingType ();
        std::cout << "Toggles shading type to "
                  << (dtm_map.shadingType () == TerrainMap::SHADE_HILL ?
                      "hill shading" : "slope shading") << std::endl;
        rebuildImage ();
        displaySelectionResult ();
      }
      else
      {
        // Tunes the lighting angle
        incLightAngle ((event->modifiers () & Qt::ShiftModifier) ? -1 : 1);
        std::cout << "Light angle = " << (getLightAngle () * 180.0f / M_PI)
                  << std::endl;
        rebuildImage ();
        displaySelectionResult ();
      }
      break;

/* DEV IN */
    case Qt::Key_M :
      if (event->modifiers () & Qt::ControlModifier)
      {
        trac->switchCutMode ();
        displaySelectionResult ();
      }
      break;
/* DEV OUT */

    case Qt::Key_N :
      // Selects next point
      trac->selectNext ((event->modifiers () & Qt::ShiftModifier) ? -1 : 1);
      displaySelectionResult ();
      break;

    case Qt::Key_P :
      // Captures main window
      {
        std::string capname (CAPT_PREF);
        capname += sector_name + std::string (IM_SUFF);
        std::cout << "Saves main window in " << capname << std::endl;
        augmentedImage.save (capname.c_str ());
      }
      break;

    case Qt::Key_S :
      trac->save (std::string (LASTROAD_NAME),
                  ptset.xref (), ptset.yref (), 500);
      std::cout << "Generated " << LASTROAD_NAME << std::endl;
      break;

    case Qt::Key_T :
      if (! loadRoadSet (std::string (ROADSET_PREF)
                         + sector_name + std::string (TXT_SUFF)))
        std::cout << "No " << ROADSET_PREF << sector_name << TXT_SUFF
                  << " file found" << std::endl;
      displaySelectionResult ();
      break;

    case Qt::Key_U :
      // Replays last extraction
      std::cerr << "p1 update: " << p1.x () << " " << p1.y () << std::endl;
      std::cerr << "p2 update: " << p2.x () << " " << p2.y () << std::endl;
      display ();
      break;

    case Qt::Key_W :
      // Increments estimated width
      trac->incWidth ((event->modifiers () & Qt::ShiftModifier) ? -1 : 1);
      std::cout << "Estimated width = " << trac->getWidth () << std::endl;
      break;

    case Qt::Key_X :
      if (event->modifiers () & Qt::ControlModifier)
      {
        trac->withdrawActivePoint ();
        displaySelectionResult ();
      }
      break;

    case Qt::Key_Y :
      if (event->modifiers () & Qt::ControlModifier)
      {
        trac->addMiddle ();
        displaySelectionResult ();
      }
      break;

    case Qt::Key_Z :
    case Qt::Key_Q :
      if (event->modifiers () & Qt::ControlModifier)
      {
        trac->unselect ();
        displaySelectionResult ();
      }
      break;

    case Qt::Key_Plus :
      if (dezoom > 1)
      {
        dezoom /= 2;
        xShift = xShift * 2 - maxWidth / 2;
        yShift = yShift * 2 - maxHeight / 2;
        std::cout << "Zoom: 1 / " << dezoom << std::endl;
        displaySelectionResult ();
      }
      else
      {
        zoom *= 2;
        xShift = xShift / 2 - maxWidth / 2;
        yShift = yShift / 2 - maxHeight / 2;
        std::cout << "Zoom: " << zoom << std::endl;
      }
      displaySelectionResult ();
      break;

    case Qt::Key_Minus :
      if (zoom > 1)
      {
        zoom /= 2;
        xShift = xShift / 2 + maxWidth / 4;
        if (xShift > 0) xShift = 0;
        if ((maxWidth - xShift) / zoom > width)
          xShift = maxWidth - width * zoom;
        yShift = yShift / 2 + maxHeight / 4;
        if (yShift > 0) yShift = 0;
        if ((maxHeight - yShift) / zoom > height)
          yShift = maxHeight - height * zoom;
        std::cout << "Zoom: " << zoom << std::endl;
      }
      else if (width / dezoom > maxWidth || height / dezoom > maxHeight)
      {
        dezoom *= 2;
        xShift = xShift / 2 + maxWidth / 4;
        if (xShift > 0) xShift = 0;
        if ((maxWidth - xShift) * dezoom > width)
          xShift = maxWidth - width / dezoom;
        yShift = yShift / 2 + maxHeight / 4;
        if (yShift > 0) yShift = 0;
        if ((maxHeight - yShift) * dezoom > height)
          yShift = maxHeight - height / dezoom;
        std::cout << "Zoom: 1 / " << dezoom << std::endl;
      }
      displaySelectionResult ();
      break;

    case Qt::Key_Left :
      xShift += 50;
      if (xShift > 0) xShift = 0;
      displaySelectionResult ();
      break;

    case Qt::Key_Right :
      xShift -= 50;
      if (((maxWidth - xShift) * dezoom) / zoom > width)
        xShift = maxWidth - (width * zoom) / dezoom;
      displaySelectionResult ();
      break;

    case Qt::Key_Up :
      yShift += 50;
      if (yShift > 0) yShift = 0;
      displaySelectionResult ();
      break;

    case Qt::Key_Down :
      yShift -= 50;
      if (((maxHeight - yShift) * dezoom) / zoom > height)
        yShift = maxHeight - (height * zoom) / dezoom;
      displaySelectionResult ();
      break;
  }
}


ASTrack *GTCreator::select (int x, int y)
{
  std::vector<ASTrack>::iterator trit = old_roads.begin ();
  while (trit != old_roads.end ())
  {
    if (trit->selectPoint (x, y)) return (&(*trit));
    trit ++;
  }
  return NULL;
}


void GTCreator::switchArea ()
{
  area_mode = ! area_mode;
  if (area_mode) area = new ASArea ();
  else
  {
    area->save (std::string (LASTAREA_NAME), ptset.xref (), ptset.yref (), 500);
    delete area;
    area = NULL;
  }
}


void GTCreator::drawPoints (QPainter &painter,
                                    std::vector<Pt2i> pts, QColor color)
{
  std::vector<Pt2i>::iterator iter = pts.begin ();
  while (iter != pts.end ())
  {
    Pt2i p (*iter++);
    painter.setPen (QPen (color, DEFAULT_PEN_WIDTH, Qt::SolidLine,
                          Qt::RoundCap, Qt::RoundJoin));
    if (p.x() < width && p.y() < height && p.x() >= 0 && p.y() >= 0)
      painter.drawPoint (QPoint (p.x(), height - 1 - p.y()));  // dec 1
  }
}


void GTCreator::drawPixels (QPainter &painter, std::vector<Pt2i> pix)
{
  std::vector<Pt2i>::iterator iter = pix.begin ();
  while (iter != pix.end ())
  {
    Pt2i p (*iter++);
    painter.setPen (QPen (QBrush (loadedImage.pixel (p.x (),
                                  loadedImage.height () - 1 - p.y ())),
                          DEFAULT_PEN_WIDTH, Qt::SolidLine,
                          Qt::RoundCap, Qt::RoundJoin));
    if (p.x() < width && p.y() < height && p.x() >= 0 && p.y() >= 0)
      painter.drawPoint (QPoint (p.x(), height - 1 - p.y()));  // dec 1
  }
}


void GTCreator::drawLine (QPainter &painter,
                                  const Pt2i from, const Pt2i to, QColor color)
{
  int n;
  Pt2i *pts = from.drawing (to, &n);
  painter.setPen (QPen (color, (DEFAULT_PEN_WIDTH * dezoom) / zoom,
                        Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  for (int i = 0; i < n; i++)
    painter.drawPoint (QPoint (pts[i].x (),
                               height - 1 - pts[i].y ()));  // dec 1
  delete [] pts;
}


void GTCreator::drawSelection (QPainter &painter,
                                       const Pt2i from, const Pt2i to)
{
  drawLine (painter, from, to, selectionColor);
}


void GTCreator::drawArea (QPainter &painter)
{
  if (area != NULL)
  {
    painter.setPen (QPen (Qt::red, 5));
    const std::vector<Pt2i> *corners = area->getCorners ();
    std::vector<Pt2i>::const_iterator it = corners->begin ();
    while (it != corners->end ())
    {
      Pt2i c1 (*it++);
      Pt2i c2 (*it++);
      painter.drawRect (c1.x (), height - 1 - c2.y (),
                        c2.x () - c1.x (), c2.y () - c1.y ());
    }
    if (area->started ())
      painter.drawPoint (area->startX (), area->startY ());
  }
}


void GTCreator::drawTiles (QPainter &painter)
{
  for (int i = 1000; i < width - 50; i += 1000)
    drawLine (painter, Pt2i (i, 0), Pt2i (i, height), Qt::green);
  for (int i = 1000; i < height - 50; i += 1000)
    drawLine (painter, Pt2i (0, i), Pt2i (width, i), Qt::green);
}


void GTCreator::incBlackLevel (int val)
{
  blevel += val * 5;
  if (blevel < 0) blevel = 0;
  if (blevel > 200) blevel = 200;
}


void GTCreator::incLightAngle (int val)
{
  dtm_map.incLightAngle (val);
}


float GTCreator::getLightAngle () const
{
  return (dtm_map.lightAngle ());
}


void GTCreator::lighten (QImage &im)
{
  if (blevel != 0 && background != BACK_BLACK && background != BACK_WHITE)
  {
    for (int i = 0; i < im.height (); i++)
      for(int j = 0; j < im.width (); j++)
      {
        int col = blevel + (QColor (im.pixel(j,i)).value ()
                            * (255 - blevel)) / 255;
        im.setPixel (j, i, col + col * 256 + col * 256 * 256);
      }
  }
}


void GTCreator::display ()
{
  if (udef)
  {
    if (p1.equals (p2))
    {
      displayBackground ();
      return;
    }
  }
}

 
void GTCreator::displayBackground ()
{
  loadedImage.save ("outputs/displayed.png");
  if (background == BACK_BLACK) augmentedImage.fill (qRgb (0, 0, 0));
  else if (background == BACK_WHITE) augmentedImage.fill (qRgb (255, 255, 255));
  else if (background == BACK_IMAGE) augmentedImage = loadedImage;
  QPainter painter (&augmentedImage);
  update (QRect (QPoint (0, 0), QPoint (width, height)));
}


void GTCreator::displaySelectionResult ()
{
  if (background == BACK_BLACK) augmentedImage.fill (qRgb (0, 0, 0));
  else if (background == BACK_WHITE) augmentedImage.fill (qRgb (255, 255, 255));
  else if (background == BACK_IMAGE) augmentedImage = loadedImage;
  lighten (augmentedImage);
  QPainter painter (&augmentedImage);
  drawArea (painter);
  if (tile_disp) drawTiles (painter);
  
  // draws saved roads
  painter.setPen (QPen (dispPub ? Qt::black : Qt::blue,
                        dispPub ? LARGE_ROAD_WIDTH : DEFAULT_PEN_WIDTH));
  std::vector<ASTrack>::iterator trit = old_roads.begin ();
  while (trit != old_roads.end ())
  {
    std::vector<Pt2i> pts = trit->points ();
    std::vector<Pt2i>::iterator it = pts.begin ();
    if (it != pts.end ())
    {
      Pt2i pt (*it++);
      while (it != pts.end ())
      {
        painter.drawLine (pt.x (), height - 1 - pt.y (),
                          it->x (), height - 1 - it->y ());
        pt.set (*it++);
      }
    }
    trit ++;
  }
  if (allPoints)
  {
    painter.setPen (QPen (dispPub ? Qt::black : Qt::white,
               dispPub ? 2 * LARGE_ROAD_WIDTH : 2 * DEFAULT_PEN_WIDTH));
    std::vector<ASTrack>::iterator trit = old_roads.begin ();
    while (trit != old_roads.end ())
    {
      std::vector<Pt2i> pts = trit->points ();
      std::vector<Pt2i>::iterator it = pts.begin ();
      while (it != pts.end ())
      {
        painter.drawPoint (it->x (), height - 1 - it->y ());
        it++;
      }
      trit ++;
    }
  }

  // draws lines
  painter.setPen (QPen (dispPub ? Qt::gray : Qt::green,
                        dispPub ? LARGE_ROAD_WIDTH : DEFAULT_PEN_WIDTH));
  std::vector<Pt2i> pts = trac->points ();
  std::vector<Pt2i>::iterator it = pts.begin ();
  if (it != pts.end ())
  {
    Pt2i pt (*it++);
    while (it != pts.end ())
    {
      painter.drawLine (pt.x (), height - 1 - pt.y (),
                        it->x (), height - 1 - it->y ());
      pt.set (*it++);
    }
  }

  if (trac->isCutMode ())
  {
    // draws cut points
    std::vector<Pt2i> cuts = trac->cutPoints ();
    it = cuts.begin ();
    painter.setPen (QPen (Qt::black, DEFAULT_PEN_WIDTH * 4));
    while (it != cuts.end ())
    {
      painter.drawPoint (QPoint (it->x (), height - 1 - it->y ()));
      it ++;
    }
    if (trac->isActive ())
    {
      Pt2i pt (trac->active ());
      painter.setPen (QPen (Qt::blue, DEFAULT_PEN_WIDTH * 4));
      painter.drawPoint (QPoint (pt.x (), height - 1 - pt.y ()));
    }
  }

  else
  {
    // draws line points
    it = pts.begin ();
    painter.setPen (QPen (dispPub ? Qt::white : Qt::black,
                          DEFAULT_PEN_WIDTH * 4));
    while (it != pts.end ())
    {
      painter.drawPoint (QPoint (it->x (), height - 1 - it->y ()));
      it ++;
    }
    if (trac->isActive ())
    {
      Pt2i pt (trac->active ());
      painter.setPen (QPen (Qt::blue, DEFAULT_PEN_WIDTH * 4));
      painter.drawPoint (QPoint (pt.x (), height - 1 - pt.y ()));
    }
  }
  update (QRect (QPoint (0, 0), QPoint (width, height)));
}


void GTCreator::compare ()
{
  GTPerfTest ptest;
  ptest.setSize (width, height, ptset.xref (), ptset.yref (), 500);
  ptest.loadSectorName (sector_name);
  ptest.loadDetectionMap (std::string (DETECT_PREF)
                          + sector_name + std::string (IM_SUFF));
  ptest.loadRoadSet (std::string (ROADSET_PREF)
                     + sector_name + std::string (TXT_SUFF));
  ptest.loadDiscardedAreas (std::string (AREA_PREF)
                            + sector_name + std::string (TXT_SUFF));
  if (mask_disp) ptest.getMask ();
  ptest.getRecall ();
  ptest.getPrecision ();
  ptest.getFMeasure ();
}
