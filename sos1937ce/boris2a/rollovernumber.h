//================================================================================================================================
// rollOverNumber.h
// ----------------
//			- Implements a roll over number on screen. ONLY UNSIGNED INTEGERS since this is useful for things like lap counters
//			  , cricket scoreboards, old style digital clocks etc.
//
//			The number display consists of a set number of digits, initially all are set to blank!
//			(This initial state is changed ONLY by incrementing the number)
//
// rollover file describes the role over and contains 
// the texture filename
//		eg.
//			roll0
//		and	11 occurences of:
//			3df, u,v,w,h
//		(0..9 and BLANK)
//================================================================================================================================
#ifdef FEATURE_ROLLOVERNUMBER


#ifndef _BS_rollOverNumber
#define _BS_rollOverNumber


class rollOverDigit
{
	material  mat;			// flags etc. may be required
	float u,v,uw,vw;
};

class rollOverNumber
{

	public:

				// Creating it adds a reference to it to the engine's chain of 
				// rollOver counters.

		rollOverNumber( char * rollOverFileName, 
						float x, float y,
						float digitW, float digitH, float digitG,
						ulong count,  ulong startvalue, bool leadingZeroes);
		~rollOverNumber();



		float  digitWidth, digitHeight;		// constant for all digits
		float  digitGap;					// gap between digits

		rollOverDigit  digit[11];			//	0..9  (10 = BLANK)	

		ulong digitsInDisplay;				// number of digits wide

		bool lZeroes;						// true if numbers begin with zeroes, false if blanks
		
		char displaydigit[digitsInDisplay];	// value of each digit   0..9  or 10 for BLANK

	private:
			
		void increment();					// add one to counter.
		void setCounter( ulong n );					// sets counter to a specific value
};

#endif	// _BS_rollOverNumber

#endif //  FEATURE_ROLLOVERNUMBER

//================================================================================================================================
//END OF FILE
//================================================================================================================================
