#!/bin/bash
# create multiresolution windows icon
ICON_DST=../../src/qt/res/icons/DeOxyRibose.ico

convert ../../src/qt/res/icons/DeOxyRibose-16.png ../../src/qt/res/icons/DeOxyRibose-32.png ../../src/qt/res/icons/DeOxyRibose-48.png ${ICON_DST}
