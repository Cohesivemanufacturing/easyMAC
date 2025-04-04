// free form 1 in absolute coordinates
// Jorge correa
// 2017


N10 G90 G0 X0 Y0 Z0 // coordinate home
N20 F200
N30 G01 X-15
N40 G02 X-5 Y10 I10 J0
N50 G01 X10
N60 G02 X15 Y5 I0 J-5
N70 X10 Y0 I-5 J0
N80 G03 X0 Y-10 I0 J-10
N90 G01 X-5
N100 G02 X-15 Y0 I0 J10 
