#!/usr/bin/python

import sys
import re
import math

# Offsets for maps : England and Wales

offsets = {
	1 : ( 1, 180, 49, 140 ),
	2 : ( -41, 140, 8, 100 ),
	3 : ( 8, 140, 62, 100 ),
	5 : ( -41.25, 100, 8, 64 ),
	6 : ( 8, 100, 62, 64 ),
	7 : ( 62, 100, 116, 64 ),
	8 : ( -28, 64, 26, 28 ),
	9 : ( 8, 64, 62, 28 ),
	10 : ( 62, 64, 116, 28 ),
	11 : ( -82, 16, -28, -20 ),
	12 : ( -28, 28, 26, -8 ),
	13 : ( 26, 28, 80, -8 ),
	14 : ( 80, 28, 134, -8 ),
	15 : ( -82, -20, -28, -58 ),
	115 : ( -90, -19, -82, -28 ),
	16 : ( -28, -8, 26, -44 ),
	17 : ( 26, -8, 80, -44 ),
	18 : ( 80, -8, 134, -44 ),
	19 : ( 134, -8, 188, -44 ),
        20 : ( -129, -73, -82, -110 ),
	21 : ( -82, -56, -28, -92 ),
	22 : ( -28, -44, 26, -80 ),
	23 : ( 26, -44, 80, -80 ),
	24 : ( 80, -44, 134, -80 ),
	25 : ( 134, -44, 188, -80 ),
	26 : ( -82, -92, -28, -128 ),
	27 : ( -28, -80, 26, -116 ),
	28 : ( 26, -80, 80, -116 ),
	29 : ( 80, -80, 125, -116 ),
	30 : ( 125, -80, 179, -116 ),
	31 : ( -82, -135.2, -28, -171.2 ),
	32 : ( -28, -116, 26, -142 ),
	33 : ( 26, -116, 80, -142 ),
	34 : ( 80, -104, 134, -140 ),
	35 : ( -136, -176, -82, -224 ),
        135 : ( -93, -164, -82, -176 ),
	36 : ( -82, -171.2, -28, -208.2 ),
	37 : ( -28, -142, 26, -182 ),
	137 : ( 8, -182, 15, -190 ),
	38 : ( 26, -142, 80, -183 ),
	39 : ( 80, -134, 134, -171 ),
	40 : ( 134, -116, 179, -164 )
}

exp = re.compile( '([A-N])([hqt]?)([0-9][0-9]?)([hqt]?),(\d+),(\d+)' )
f = open( sys.argv[ 1 ], 'r' )
lines = f.readlines()
sheet = int( lines[ 0 ] )
w, n, e, s = offsets[ sheet ]
for x in range( 1, len( lines ) ):
	m = exp.match(lines[ x ])
	if( m is not None ):
		row = m.group( 1 )
		nrow = ord( row ) - 65
		if( nrow > 8 ):
			nrow = nrow - 1	# Compensate for no 'I' square
		hrow = ( m.group( 2 ) == 'h' )
		qrow = ( m.group( 2 ) == 'q' )
		trow = ( m.group( 2 ) == 't' )
		col = int( m.group( 3 ) ) - 1
		hcol = ( m.group( 4 ) == 'h' )
		qcol = ( m.group( 4 ) == 'q' )
		tcol = ( m.group( 4 ) == 't' )
		x = int( m.group( 5 ) )
		y = int( m.group( 6 ) )
		fulln = n - ( nrow * 4 )
		if( hrow ):
			fulln = fulln - 2
		if( qrow ) :
			fulln = fulln - 1
		if( trow ) :
			fulln = fulln - 3
		fulle = w + ( col * 4 )
		if( hcol ):
			fulle = fulle + 2
		if( qcol ):
			fulle = fulle + 1
		if( tcol ):
			fulle = fulle + 3

		# Translate this into the real location, given square 0,0
		# is actually southeast of the true origin in Delamere Forest

		fudgede = float( fulle ) + (1511.13 / 5280 )
		fudgedn = float( fulln ) - (3613.84  / 5280 )
		print 'cas,%f,%f,%d,%d' % ( fudgede, fudgedn, x, y )

