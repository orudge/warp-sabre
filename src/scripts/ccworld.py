#!/usr/bin/python

import sys
import math
import Image
import cconv

sheet = sys.argv[ 1 ]
w, n, e, s = cconv.offsets[ int( sheet ) ]
geoFrom = cconv.CS2Geo( ( w + 0.5 ), ( n + 0.25 ), cconv.CDEL )
geoFromResult = cconv.OSGB362WGS84(geoFrom.latitude, geoFrom.longitude)
geoTo = cconv.CS2Geo( ( e + 0.5 ), ( s + 0.25 ), cconv.CDEL )
geoToResult = cconv.OSGB362WGS84( geoTo.latitude, geoTo.longitude)
img = Image.open( sys.argv[ 2 ] )
sizex, sizey = img.size
fromLon = math.degrees( geoFromResult.longitude )
fromLat = math.degrees( geoFromResult.latitude )
toLon = math.degrees( geoToResult.longitude )
toLat = math.degrees( geoToResult.latitude )
print ( toLon - fromLon ) / sizex
print '0.0'
print '0.0'
print ( toLat - fromLat ) / sizey
print fromLon
print fromLat 
