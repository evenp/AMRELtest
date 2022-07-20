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
#include "gtperftest.h"

#define IMAGE_SUFFIX ".png"
#define MASK_PREF "../Data/outputs/mask_"
#define RECALL_PREF "../Data/outputs/recall_"
#define PREC_PREF "../Data/outputs/precision_"
#define ROAD_PREF "../Data/roads/track_"
#define ROAD_SUFF ".txt"


GTPerfTest::GTPerfTest ()
{
  det_map = NULL;
  recall = 0.0f;
  precision = 0.0f;
  fmeasure = 0.0f;
  xref = (int64_t) 0;
  yref = (int64_t) 0;
  mapsize = 1;
  tr_width = 28;
  area = NULL;
  sect_name = std::string ("");
}


GTPerfTest::~GTPerfTest ()
{
  std::vector<ASTrack *>::iterator it = gt_roads.begin ();
  while (it != gt_roads.end ()) delete *it++;
  gt_roads.clear ();
  if (area != NULL) delete area;
  if (det_map != NULL) delete det_map;
}


void GTPerfTest::setSize (int width, int height,
                          int64_t xref, int64_t yref, int mapsize)
{
  this->width = width;
  this->height = height;
  this->xref = xref;
  this->yref = yref;
  this->mapsize = mapsize;
  map_r = QImage (width, height, QImage::Format_RGB32);
  map_p = QImage (width, height, QImage::Format_RGB32);
}


void GTPerfTest::loadDetectionMap (std::string name)
{
  det_map = new QImage (QString (name.c_str ()));
}


void GTPerfTest::loadDiscardedAreas (std::string name)
{
  area = new ASArea ();
  if (! area->load (name, xref, yref, mapsize))
  {
    std::cout << "No " << name << " file found" << std::endl;
    delete area;
    area = NULL;
  }
  else std::cout << name << " loaded" << std::endl;
}


void GTPerfTest::loadSectorName (std::string name)
{
  sect_name = name;
}


bool GTPerfTest::loadRoadSet (std::string name)
{
  // Clears out structures
  gt_w.clear ();
  gt_l.clear ();
  std::vector<ASTrack *>::iterator it = gt_roads.begin ();
  while (it != gt_roads.end ()) delete *it++;
  gt_roads.clear ();

  // Loads road set
  std::ifstream input (name, std::ios::in);
  if (! input)
  {
    std::cout << "Cannot open " << name << std::endl;
    return false;
  }
  char roadname[200];
  input >> roadname;
  while (! input.eof ())
  {
    std::string tname (ROAD_PREF);
    tname += std::string (roadname) + std::string (ROAD_SUFF);
    ASTrack *tr = new ASTrack ();
    if (! tr->load (tname, xref, yref, mapsize))
    {
      delete tr;
      std::cout << "Cannot load " << tname << std::endl;
    }
    else gt_roads.push_back (tr);
    input >> roadname;
  }

  // Draws linear ground truth
  map_r.fill (Qt::white);
  QPainter painter (&map_r);
  painter.setPen (QPen (Qt::black, 1, Qt::SolidLine,
                        Qt::RoundCap, Qt::RoundJoin));
  std::vector<ASTrack *>::iterator itr = gt_roads.begin ();
  while (itr != gt_roads.end ())
  {
    std::vector<Pt2i> pts = (*itr)->points ();
    std::vector<Pt2i>::iterator it = pts.begin ();
    if (it != pts.end ())
    {
      Pt2i pt = *it++;
      while (it != pts.end ())
      {
        painter.drawLine (pt.x (), height - 1 - pt.y (),
                          it->x (), height - 1 - it->y ());
        pt.set (*it++);
      }
    }
    itr ++;
  }
  for (int j = 0; j < height; j ++)
    for (int i = 0; i < width; i++)
      if (QColor (map_r.pixel (i, height - 1 - j)).value () < 10)
        gt_l.push_back (Pt2i (i, j));

  // Draws thick ground truth
  map_r.fill (Qt::white);
  painter.setPen (QPen (Qt::black, tr_width, Qt::SolidLine,
                        Qt::RoundCap, Qt::RoundJoin));
  itr = gt_roads.begin ();
  while (itr != gt_roads.end ())
  {
    std::vector<Pt2i> pts = (*itr)->points ();
    std::vector<Pt2i>::iterator it = pts.begin ();
    if (it != pts.end ())
    {
      Pt2i pt = *it++;
      while (it != pts.end ())
      {
        painter.drawLine (pt.x (), height - 1 - pt.y (),
                          it->x (), height - 1 - it->y ());
        pt.set (*it++);
      }
    }
    itr ++;
  }
  for (int j = 0; j < height; j ++)
    for (int i = 0; i < width; i++)
      if (QColor (map_r.pixel (i, height - 1 - j)).value () < 10)
        gt_w.push_back (Pt2i (i, j));
  return true;
}


void GTPerfTest::getMask ()
{
  QImage area_mask (width, height, QImage::Format_RGB32);
  area_mask.fill (Qt::white);
  if (area != NULL)
  {
    QPainter painter (&area_mask);
    const std::vector<Pt2i> *corners = area->getCorners ();
    std::vector<Pt2i>::const_iterator it = corners->begin ();
    while (it != corners->end ())
    {
      Pt2i c1 (*it++);
      Pt2i c2 (*it++);
      painter.fillRect (c1.x (), height - 1 - c2.y (),
                        c2.x () - c1.x (), c2.y () - c1.y (), Qt::black);
    }
  }
  std::string rname (MASK_PREF);
  rname += sect_name + std::string (IMAGE_SUFFIX);
  area_mask.save (rname.c_str ());
}


void GTPerfTest::getRecall ()
{
  map_r.fill (0);
  if (area != NULL)
  {
    QPainter painter (&map_r);
    const std::vector<Pt2i> *corners = area->getCorners ();
    std::vector<Pt2i>::const_iterator it = corners->begin ();
    while (it != corners->end ())
    {
      Pt2i c1 (*it++);
      Pt2i c2 (*it++);
      painter.fillRect (c1.x (), height - 1 - c2.y (),
                        c2.x () - c1.x (), c2.y () - c1.y (), Qt::red);
    }
  }

  for (int j = 0; j < height; j++)
    for (int i = 0; i < width; i++)
      if (QColor(det_map->pixel(i,j)).lightness () > 100)
      {
        int red = QColor(map_r.pixel (i, j)).red ();
        if (red < 200) map_r.setPixel (i, j, 127 + 127 * 256 + 127 * 256 * 256);
      }
  std::vector<Pt2i>::iterator pit = gt_l.begin ();
  int nbok = 0, nbtot = 0;
  while (pit != gt_l.end ())
  {
    int red = QColor(map_r.pixel (pit->x (), height - 1 - pit->y ())).red ();
    if (red > 100 && red < 150)
    {
      map_r.setPixel (pit->x (), height - 1 - pit->y (), 255 * 256);
      nbok ++;
      nbtot ++;
    }
    else if (red < 200)
    {
      map_r.setPixel (pit->x (), height - 1 - pit->y (), 255);
      nbtot ++;
    }
    pit ++;
  }
  recall = (nbok * 100) / (float) nbtot;
  std::cout << "Recall : " << recall << std::endl;
  std::string rname (RECALL_PREF);
  rname += sect_name + std::string (IMAGE_SUFFIX);
  map_r.save (rname.c_str ());
}


void GTPerfTest::getPrecision ()
{
  map_p.fill (0);
  if (area != NULL)
  {
    QPainter painter (&map_p);
    const std::vector<Pt2i> *corners = area->getCorners ();
    std::vector<Pt2i>::const_iterator it = corners->begin ();
    while (it != corners->end ())
    {
      Pt2i c1 (*it++);
      Pt2i c2 (*it++);
      painter.fillRect (c1.x (), height - 1 - c2.y (),
                        c2.x () - c1.x (), c2.y () - c1.y (), Qt::red);
    }
  }

  std::vector<Pt2i>::iterator pit = gt_w.begin ();
  while (pit != gt_w.end ())
  {
    int red = QColor(map_p.pixel (pit->x (), height - 1 - pit->y ())).red ();
    if (red < 200) map_p.setPixel (pit->x (), height - 1 - pit->y (),
                                   127 + 127 * 256 + 127 * 256 * 256);
    pit ++;
  }
  int nbok = 0, nbtot = 0;
  for (int j = 0; j < height; j++)
    for (int i = 0; i < width; i++)
      if (QColor(det_map->pixel(i,j)).lightness () > 100)
      {
        int red = QColor(map_p.pixel (i, j)).red ();
        if (red > 100 && red < 150)
        {
          map_p.setPixel (i, j, 255 * 256 );
          nbok ++;
          nbtot ++;
        }
        else if (red < 200)
        {
          map_p.setPixel (i, j, 255);
          nbtot ++;
        }
      }
  precision = (nbok * 100) / (float) nbtot;
  std::cout << "Precision : " << precision << std::endl;
  std::string pname (PREC_PREF);
  pname += sect_name + std::string (IMAGE_SUFFIX);
  map_p.save (pname.c_str ());
}


void GTPerfTest::getFMeasure ()
{
  fmeasure = 2 * recall * precision / (recall + precision);
  std::cout << "F-measure : " << fmeasure << std::endl;
}
