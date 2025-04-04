// 90 deg cicular arcs in relative coordinates
// Jorge correa
// 2017


N10  G90 G0 X0 Y0 Z0  // coordinate home
N20  F50
N30  G02 X10  Y10  I10 J0 
N40  G02 X20  Y0   I0 J-10 
N50  G02 X10  Y-10 I-10 J0 
N60  G02 X0   Y0   I0 J10 
N70  G03 X-10 Y10  I-10 J0 
N80  G03 X-20 Y0   I0 J-10  
N90  G03 X-10 Y-10 I10 J0   
N100 G03 X0   Y0   I0 J10      
