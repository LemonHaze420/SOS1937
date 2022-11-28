//================================================================================================================================
// modelStore.h
// --------------
//
//		Holding of all the models in the system. Each model filename stored for the appropriate index,
//================================================================================================================================

#ifndef _BS_modelStore
#define _BS_modelStore




void startModelStore();
model * getModelPtr( char * filename, float modelscale,  bool uniqueCopy, char * SSname  );
void removeModelStore();


#endif	// _BS_textureStore

//================================================================================================================================
//END OF FILE
//================================================================================================================================
