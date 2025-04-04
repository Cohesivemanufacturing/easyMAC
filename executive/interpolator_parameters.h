#pragma once

typedef struct InterpolatorParameters
{
	int CB;  // circular bit
	int IB;  // interchange bit
	int PB;  // plane bit
	long x;
	long y;
	long z;
	long xtarget;
	long ytarget;
	long ztarget;
	long px;
	long py;
	long pz;
	long pf;
	long s;
	long steplim;
};

typedef struct Interpolatordata
{
	int x;  
	int y;  
	int z; 
	int countin;
	int countout;
};

typedef struct Bufferdata
{
	int error;
	int countin;
};