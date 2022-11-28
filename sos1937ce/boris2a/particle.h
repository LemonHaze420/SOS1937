/*
	Particle system code
*/

void makeParticle(vector3 *position,vector3 *vel,vector3 *acc,
				  vector3 *posrand,vector3 *velrand,
				  unsigned long tickstolive,float size,DWORD col);

void runParticles();
void drawParticles();
void initialiseParticles();
void shutdownParticles();