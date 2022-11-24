// $Header$

// $Log$
// Revision 1.2  2000-01-17 12:37:07+00  jjs
// Latest version has corrected Alpha Tested polygons.
//
// Revision 1.1  2000-01-13 17:27:05+00  jjs
// First version that supports the loading of objects.
//

#include "BS2all.h"

#include <stdio.h>


model * modelChain;

void startModelStore()
{
	modelChain = NULL;
}


extern char tempBuffer[];

model * getModelPtr( char * filename, float modelscale, bool uniqueCopy, char * SSname)
{
	bool matched = false;
	bool invisible = false;
	model * mymodel = modelChain;

	while( mymodel )
	{
		if( matched = ( strcmp(mymodel->filename, filename) == 0) )		// matched
		   break;
		mymodel = mymodel->next;
	}

	if (matched)
	{
		// model is already loaded in. If we require a unique copy of this model then
		// we shall have to completely copy the loaded version to a new model and add
		// an entry to the model store chain (ready fo	r deletion).

		if( uniqueCopy )
		{
			model * modelCopy = new model;
			modelCopy->copynext = mymodel->copynext;
			mymodel->copynext = modelCopy;
		
				// copy across the core data from the model

			modelCopy->meshData = new mesh[mymodel->meshct];
			modelCopy->meshct = mymodel->meshct;
			modelCopy->highest_visible_mesh = mymodel->highest_visible_mesh;
			modelCopy->filename="COPIED DATA";

			for(int i=0;i<mymodel->meshct;i++)
			{
				modelCopy->meshthresholds[i]=mymodel->meshthresholds[i];

				modelCopy->meshData[i].copyfrom(&mymodel->meshData[i]);
			}

			modelCopy->boundingBox = mymodel->boundingBox;  
			modelCopy->centroid = mymodel->centroid;
			modelCopy->boundingRadius = mymodel->boundingRadius;
		
			return( modelCopy );
		} 
		else
			return( mymodel );
	}
	else
	{
		// Always place new model at head of chain.
		
		model * oldmc = modelChain;
		modelChain = new model;
		modelChain->next = oldmc;
		modelChain->copynext = NULL;			// no copies initially
	
		modelChain->filename = new char[ strlen(filename) + 1];
		strcpy( modelChain->filename, filename );

		//
		// Load the mulder model from archive
		//

		// LOD determination code. We first attempt to load the MDF, or Model Description File.
		// This contains information about which actual models are used at which level of detail,
		// and what the distance thresholds are. Each line is in the format
		//		filename	distance
		// where the filename is the model for the LOD, and the distance is the distance *below* which this LOD is
		// shown.

		ulong mdfhandle = arcExtract_mdf(filename);

		int numlods=0;
		char lodfilenames[MAXLODS][64];	

		modelChain->highest_visible_mesh = -1;

		if(!mdfhandle)
		{
			// if the MDF isn't found, then load the model 'filename' as a single-lod model
			numlods = 1;
			strcpy(lodfilenames[0],filename);
			modelChain->meshthresholds[0] = 999999.0f;
		}
		else
		{
			char tempbuf[128],*s;

			while(arcGets(tempbuf,128,mdfhandle))
			{
				if(tempbuf[0]!='#')
				{
					s = strtok(tempbuf,"\t ");	// filename
					if(!s)break;

					if(*s == '*')
					{
						if(!numlods)invisible=true;
						modelChain->highest_visible_mesh = numlods-1;
						modelChain->meshthresholds[numlods]=999999.0f;
						numlods++;
						break;
					}
					strcpy(lodfilenames[numlods],s);
					s = strtok(NULL,"\t ");		// distance
					modelChain->meshthresholds[numlods++]=(float)atof(s);
				}
			}
			arcDispose(mdfhandle);
		}

		if(modelChain->highest_visible_mesh < 0)
			modelChain->highest_visible_mesh = numlods-1;

		modelChain->meshData = new mesh [numlods];
		modelChain->meshct = numlods;


		for(numlods=0;numlods<modelChain->meshct;numlods++)
		{
			if(numlods <= modelChain->highest_visible_mesh)
				loadMulderFile( lodfilenames[numlods], modelChain->meshData+numlods, modelscale, numlods?NULL:SSname,  false);
		}

		boundBox * boundingBox = &modelChain->boundingBox;
		vector3 * centroid = &modelChain->centroid;

		mesh * meshData = modelChain->meshData + 0;	// use zeroth lod to determine bboxes etc.

		boundingBox->lo.x = meshData->MinCoordX;
		boundingBox->hi.x = meshData->MaxCoordX;
		boundingBox->lo.y = meshData->MinCoordY;
		boundingBox->hi.y = meshData->MaxCoordY;
		boundingBox->lo.z = meshData->MinCoordZ;
		boundingBox->hi.z = meshData->MaxCoordZ;

		//
		// The centroid to take as the best position for the bounding sphere is at the centre of the bounding box.
		// The radius of this will be the Cartesian distance from the centre to a corner.
		//

		centroid->x = ( meshData->MinCoordX +  meshData->MaxCoordX ) * 0.5f;
		centroid->y = ( meshData->MinCoordY +  meshData->MaxCoordY ) * 0.5f;
		centroid->z = ( meshData->MinCoordZ +  meshData->MaxCoordZ ) * 0.5f;

		modelChain->boundingRadius = sqrtf(centroid->x * centroid->x  + 
					 centroid->y * centroid->y +  centroid->z * centroid->z);

		if(invisible)modelChain->highest_visible_mesh=-1;

		modelChain->minimum_lod = 0;

		return( modelChain );
	}
}


void removeModelStore()
{
	model * myModelNew;
	model * myModel = modelChain;
	while( myModel != 0 )
	{
		model * copymodel;

		copymodel = myModel->copynext;
		while(copymodel)
		{
			//filename field is UNUSED so we DONT delete it!!

			myModelNew = copymodel->copynext;
			delete [] copymodel->meshData;
			delete copymodel;
			copymodel = myModelNew;
		}
		
		delete myModel->filename;
		myModelNew = myModel->next;
		delete [] myModel->meshData;
		delete myModel;
		myModel = myModelNew;
	}
	modelChain = NULL;
}
