//================================================================================================================================
// FEdata.h
// ---------
//		communication of information from front end to game
//================================================================================================================================

#ifndef _FE_FEdata
#define _FE_FEdata


#define MAXIMUMCARS  32


//-------------------------------------------------------------------------------------------------------------------------------

#define TRACKCODE_BROOKLANDS  0
#define TRACKCODE_OVAL		  1
#define TRACKCODE_TRIPOLI     2
#define TRACKCODE_AVUS        3

char * tracknames[] = {

		"brooklands",
		"oval",
		"tripoli",
		"avus"
};

//-------------------------------------------------------------------------------------------------------------------------------

#define CARCODE_ALFARED		  0
#define CARCODE_BUGATTI       1

char * carnames[] = {

		"alfared",
		"bugatti"
};


//-------------------------------------------------------------------------------------------------------------------------------

class FEcar {

	public:
		int carCode;
	private:
	
};

class FEdata {

	public:
			FEdata( int tcode )
					{ trackCode = tcode;
					  nextFEcar = carontrack;
					  carCount = 0;};
			~FEdata(){};

			appendCarEntry( int ccode ) 
					{ nextFEcar->carCode = ccode;
					  nextFEcar++;
					  carCount++;};

	private:
			int trackCode;
			int carCount;							// FEcar entries used
			FEcar  carontrack[ MAXIMUMCARS ];

			FEcar * nextFEcar;


};


#endif	// _FE_FEdata

//================================================================================================================================
//END OF FILE
//================================================================================================================================
