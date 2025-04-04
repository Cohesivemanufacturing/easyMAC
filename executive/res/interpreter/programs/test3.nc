// 45 deg cicular arcs in absolute coordinates
// Jorge correa
// 2017


N10 G90 G0 X0 Y0 Z0                         // coordinate home
N20 F50
N30 G02 X2.9289   Y7.0711  I10      J0      // first quadrant
N40 G02 X10       Y10      I7.0711  J-7.0711
N50 G02 X7.0711   Y2.9289  I-10     J0
N60 G02 X0        Y0       I-7.0711 J7.0711
N70 G03 X-7.0711  Y-2.9289 I0       J-10    // second quadrant
N80 G03 X-10      Y-10     I7.0711  J-7.0711
N90 G03 X-2.9289  Y-7.0711 I0       J10
N100 G03 X0       Y0       I-7.0711 J7.0711
N110 G03 X-2.9289 Y7.0711  I-10     J0      // Third quadrant
N120 G03 X-10     Y10      I-7.0711 J-7.0711
N130 G03 X-7.0711 Y2.9289  I10      J0
N140 G03 X0       Y0       I7.0711  J7.0711 
N150 G02 X7.0711  Y-2.9289 I0       J-10    // forth quadrant
N160 G02 X10      Y-10     I-7.0711 J-7.0711 
N170 G02 X2.9289  Y-7.0711 I0       J10  
N190 G02 X0       Y0       I7.0711  J7.0711
