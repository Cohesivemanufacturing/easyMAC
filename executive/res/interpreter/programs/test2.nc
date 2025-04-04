// free form 1 in relative coordinates
// Jorge correa
// 2017


N10 G90 G0 X0 Y0 Z0 // coordinate home
N20 G91 F50
N30 G01 X-15 
N40 G02 X10 Y10 I10 J0 
N50 G01 X15
N60 G02 X5 Y-5 I0 J-5
N70 G02 X-5 Y-5 I-5 J0
N80 G03 X-10 Y-10 I0 J-10
N90 G01 X-5
N100 G02 X-10 Y10 I0 J10
