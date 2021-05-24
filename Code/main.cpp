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

#include <QApplication>
#include <iostream>
#include <string>
#include <ostream>
#include "gtwindow.h"

#define NVM_DIR "../Data/nvm/"
#define NVM_SUFFIX ".nvm"
#define TIL_DIR "../Data/til/"
#define TIL_FILE "eco"
#define TIL_SUFFIX ".til"
#define TILE_SET_DIR "../Data/tilesets/"
#define DEFAULT_SET "grismouton.txt"
#define CELL_SIZE 0.5f


int main (int argc, char *argv[])
{
  int val = 0;
  bool tiledef = false;
  std::string tilename = std::string ("");
  bool comparing = false;
  std::string sector_name = std::string (DEFAULT_SET);
  QApplication app (argc, argv);

  GTWindow window (&val);   // val : necessary argument !
  for (int i = 1; i < argc; i++)
  {
    if (std::string(argv[i]).at(0) == '-')
    {
      if (std::string(argv[i]) == std::string ("--tile"))
      {
        if (++i == argc)
        {
          std::cout << "Tile missing" << std::endl;
          return 0;
        }
        tilename = std::string (argv[i]);
        tiledef = true;
      }
      else if (std::string(argv[i]) == std::string ("--comp"))
        comparing = true;
      else
      {
        int l = std::string (argv[i]).length ();
        for (int j = 1; j < l; j++)
        {
          char carac = std::string(argv[i]).at(j);
          if (carac == 't')
          {
            if (++i == argc)
            {
              std::cout << "Tile missing" << std::endl;
              return 0;
            }
            tilename = std::string (argv[i]);
            tiledef = true;
          }
          else
          {
            std::cout << "Unknown argument: " << argv[i] << std::endl;
            return 0;
          }
        }
      }
    }
    else sector_name = std::string (argv[i++]);
  }

  // Tile file loading
  if (tiledef)
  {
    std::string nvmfile (NVM_DIR);
    nvmfile += tilename + std::string (NVM_SUFFIX);
    std::string ptsfile (TIL_DIR);
    ptsfile += TIL_FILE + std::string ("/") + TIL_FILE + std::string ("_")
               + tilename + std::string (TIL_SUFFIX);
    if (! window.setBinaryLidarFiles (nvmfile, ptsfile))
    {
      std::cout << "Header of " << nvmfile << " inconsistent" << std::endl;
      return (0);
    }
  }
  else
  {
    char sval[12];
    std::vector<int> vals;
    std::string tsname (TILE_SET_DIR);
    tsname += sector_name + ".txt";
    std::ifstream input (tsname.c_str (), std::ios::in);
    bool reading = true;
    if (input)
    {
      while (reading)
      {
        input >> sval;
        if (input.eof ()) reading = false;
        else
        {
          std::string nvmfile (NVM_DIR);
          nvmfile += sval;
          std::string ptsfile (TIL_DIR);
          ptsfile += TIL_FILE + std::string ("/")
                     + TIL_FILE + std::string ("_") + sval;
          nvmfile += std::string (NVM_SUFFIX);
          ptsfile += std::string (TIL_SUFFIX);
          if (! window.setBinaryLidarFiles (nvmfile, ptsfile))
          {
            std::cout << "Header of " << nvmfile << " inconsistent"
                      << std::endl;
            return (0);
          }
        }
      }
      input.close ();
      window.setSectorName (sector_name);
    }
    else
    {
      std::cout << "No " << tsname << " file found" << std::endl;
      return 0;
    }
  }

  window.createMaps (true);

  if (comparing)
  {
    window.compare ();
    return (EXIT_SUCCESS);
  }
  window.runOptions (); 
  window.show ();
  return app.exec ();
}
