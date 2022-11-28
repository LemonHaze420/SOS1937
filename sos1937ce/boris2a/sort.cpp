//================================================================================================================================
// sort.cpp
// --------
//
//		sort in unsigned ascending order an array of 'sortlistentry'
//		uses selective radix2 bin-sort ... a VERY FAST assembler suited approach.
//		( a lot better than bloody 'quicksort' on a lot of entries of the kind we are using here ... 
//		  ... don't always believe the textbooks...)
//
//================================================================================================================================

#include "BS2all.h"


	//
	//  A Big static array created for use in all sorts. This must be big enough to
	//  hold all triangles to be rendered on a frame.
	//


sortlistentry tempSortBuffer[ MAXIMUM_SORT_ENTRIES ];

sortlistentry * tbs;
ulong mask;
ulong eCount;
ulong sMask;

void sort (  
   			 ulong		  entryCount, 
			 ulong		  selectiveMask,
			 sortlistentry * inList
	       )
{
	mask = 1;
	tbs = &tempSortBuffer[0];
	eCount = entryCount;
	sMask = selectiveMask;

	
	while( mask )
	{
		if( mask & sMask )
		{

		_asm{
				pushad
				mov esi, inList
				mov edi, tbs

				mov ecx, eCount
		        lea esi, [esi + ecx*8]
				neg ecx
				
				mov ebp, mask

	nextZeroBit:       
				test dword ptr [esi + ecx*8], ebp
				jnz  firstNz
				inc ecx
				js  nextZeroBit
				jmp alldone			; will only occur if entire column is zero
	firstNz:
			   push  edi

				mov ebx, ecx
				mov eax, [esi + ecx*8]
				mov edx, [esi + ecx*8 + 4]
				mov [edi], eax
				mov [edi+4],edx
				add edi, 8
				inc ecx
				jns LoopOver
    NextItem:
				mov eax, [esi + ecx*8]
				mov edx, [esi + ecx*8 + 4]
				test eax, ebp
				jnz  bitSet
				mov  [esi + ebx*8 ], eax
				mov  [esi + ebx*8 +4], edx
				inc  ebx
				inc  ecx
				js   NextItem
				jmp  LoopOver       
    bitSet:
				mov  [edi], eax
				mov  [edi+4],edx
				add  edi, 8
				inc  ecx
				js   NextItem
    LoopOver:					; append the '1' buffer to the '0' buffer
				lea eax, [ebx*8]               

			   pop edi

				sub edi, eax
    NextCopyItem:                
				mov eax, [edi + ebx*8]
				mov edx, [edi + ebx*8 +4] 
				mov [esi + ebx*8], eax
				mov [esi + ebx*8 + 4], edx
				inc ebx
				js  NextCopyItem
	alldone:
				popad
			}
		}
		mask += mask;
	}
}


//================================================================================================================================
// END OF FILE
//================================================================================================================================
