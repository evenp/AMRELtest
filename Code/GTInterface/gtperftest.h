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

#ifndef GT_PERF_TEST_H
#define GT_PERF_TEST_H

#include <QImage>
#include "astrack.h"
#include "asarea.h"


/** 
 * @class GTPerfTest gtperftest.h
 * \brief Road detector performance test.
 * Compares detected roads with a created ground truth (recall and precision).
 */
class GTPerfTest
{
public:

  /**
   * \brief Creates a performance test.
   */
  GTPerfTest ();

  /**
   * \brief Deletes the performance test.
   */
  ~GTPerfTest ();

  /**
   * \brief Sets the size and dimensions of the structures.
   * @param width Map width.
   * @param height Map width.
   * @param xref Tile origin X-coordinate.
   * @param yref Tile origin Y-coordinate.
   * @param mapsize covered terrain size per tile (in meters).
   */
  void setSize (int width, int height,
                int64_t xref, int64_t yref, int mapsize);

  /**
   * \brief Loads a map of detected structures.
   * @param name of the map file.
   */
  void loadDetectionMap (std::string name);

  /**
   * \brief Loads discarded areas for this test.
   * @param name Area file name.
   */
  void loadDiscardedAreas (std::string name);

  /**
   * \brief Loads a road set to test.
   * @param name Road set file name.
   */
  bool loadRoadSet (std::string name);

  /**
   * \brief Computes and edits recall measure.
   */
  void getRecall ();

  /**
   * \brief Computes and edits precision measure.
   */
  void getPrecision ();

  /**
   * \brief Computes F-measure.
   */
  void getFMeasure ();


private:
 
  /** Ground truth roads. */
  std::vector<ASTrack *> gt_roads;
  /** Wide ground truth map. */
  std::vector<Pt2i> gt_w;
  /** Fine ground truth map. */
  std::vector<Pt2i> gt_l;
  /** Detected plateaux. */
  QImage *det_map;

  /** Output recall. */
  float recall;
  /** Output precision. */
  float precision;
  /** Output F-measure. */
  float fmeasure;

  /** Output recall map. */
  QImage map_r;
  /** Output precision map. */
  QImage map_p;

  /** Tile set width. */
  int width;
  /** Tile set height. */
  int height;
  /** Tile origin X-coordinate. */
  int64_t xref;
  /** Tile origin Y-coordinate. */
  int64_t yref;
  /** Covered terrain size per tile (in meters). */
  int mapsize;

  /** Studied sector name. */
  std::string sect_name;
  /** Delineated road max width. */
  int tr_width;
  /** Discarded areas. */
  ASArea *area;

};
#endif
