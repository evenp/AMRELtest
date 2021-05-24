# AMRELtest: Data set for testing AMREL (Automatic Mountain Road Extraction from LiDAR data)

This data set contains :

* Gris-Mouton sector data in AMREL compatible formats NVM and TIL, derived
from the Fossard LiDAR data set by courtesy of AGER PCR project, dir. C.
Kraemer, in the scope of SolHoM-Fossard interdisciplinary project, dir.
A. Poszwa, Université de Lorraine.

* roadgt : a minimalist key-based interface to create a road ground truth
from LiDAR Digital Terrain Models and to compare it with road extractions
achieved using [AMREL software](https://github.com/evenp/AMREL).

Authors: Philippe Even and Phuc Ngo, LORIA/ADAGIo, Université de Lorraine.

Reference : Even, P. and Ngo, P., 2021.
[Automatic forest road extraction from LiDAR data of mountainous areas.](https://doi.org/10.1007/978-3-030-76657-3_6)
In International Joint Conference of Discrete Geometry Mathematical Morphology,
Uppsala, Sweden, May 24-17, 2021 (Springer LNCS 12708), pp. 93-106.
[hal-03144147](https://hal.archives-ouvertes.fr/hal-03144147).

## QUICK SETUP / COMPILATION GUIDE

* Requires Qt5 library.

* roadgt.pro provided for qmake.

## COMPARISON OF DETECTED ROADS WITH CREATED GROUND TRUTH

In the following, we assume that *'sector'* is the name of the studied sector,
defined by a set of tiles.

### Inputs

* A set of tiles specified in *Data/tilesets/'sector'.txt*

* Specified tiles digital terrain model in NVM format put into *Data/nvm/* directory

NVM format may be obtained from standard ASC files using AMREL.

* Specified tiles digital terrain model in TIL format put into one of *Data/til/* subdirectories

TIL formats may be obtained from standard XYZ files using AMREL.

* Detected roads as a PNG image to be put in *Data/detections/roads_'sector'.png* file

* A ground truth set of delineated roads specified in *Data/roadsets/rgt_'sector'.txt*

* Delineated roads refered by the ground truth set available in *Data/roads/* directory

### Command
```
roadgt --comp 'sector'
```
### Outputs

* Recall map : *Data/outputs/recall_'sector'.png*

* Precision map : *Data/outputs/precision_'sector'.png*

### GROUND TRUTH CREATION

### Inputs

* A set of tiles specified in *Data/tilesets/'sector'.txt*

### Command
```
roadgt 'sector'
```
or to process a single tile:
```
roadgt --tile 'tile'
```
### Main controls

* Click with mouse button to define a road point and select it.

* Click and drag with mouse button to select and move a road point.

* Type 'Control-X' to remove the selected point.

* Type key 'n' (resp. 'N') to select next (resp. previous) point.

* Type 'Control-Y' to add a point between selected point and next one.

* Type key 's' to save the defined road in *../Data/roads/last.txt* file.

* Type key 'g' to get a road from *../Data/roads/last.txt* file.

### Other controls

* Type arrow keys to shift the map.

* Type key '+' (resp '-') to zoom in (resp. out).

* Type key 'Control-H' to switch between thin or dilated road display.

* Type key 'Control-B' to switch the background of the window.

* Type key 'b' (resp. 'B') to set DTM map lighter (resp. darker).

* Type key 'Control-L' to switch DTM between slope or hill shading.

* Type key 'l' or 'L' to rotate the light direction of DTM hill-shading.

* Type key 't' to display the road set saved in *Data/roadsets/'sector'.txt*

* Type key 'k' to show or hide road points

* Type key 'p' to grab the window in *Data/outputs/capture_'sector'.png*
