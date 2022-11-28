// $Header$

// $Log$

void EmulationInsertObject(object *NewObject);
void EmulationRemoveObject(object *OldObject);

void EmulationGetWorldFromScreen(vector3 *Destination, float x, float y, float z);
void EmulationRender();
void EmulationRemoveAllObjects();