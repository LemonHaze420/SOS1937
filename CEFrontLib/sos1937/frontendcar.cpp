/* The car object */

#include "ConsoleFrontLib.h"

extern struct GlobalStructure MainGameInfo;

extern char *FindValue(char *, char *);
extern void ReadVector(char *, vector3 &);
extern void ReadText(char * &, char *);
extern void ConvertCarToDescription(char *CarName, char *DescriptionName);
extern char *CarNumbers[100];

extern int reallanguage;

FrontEndCar::FrontEndCar(int _Number)
{
	Number = _Number;

	char CurrentCarName[256], DescriptionName[256], TempName[256];
	sprintf(CurrentCarName, "%s_%d", CarNumbers[Number-1], Number);
	
	ConvertCarToDescription(CurrentCarName, DescriptionName);
	
	
	sprintf(TempName, "%sText", DescriptionName);
	CarName = new FrontEndObject(NULL, TempName, 1.0f, NULL, true); 
	
	sprintf(TempName, "%sStats", DescriptionName);	
	CarGraph = new FrontEndObject(NULL, TempName, 1.0f, NULL, true);

	// First, get the car positions.
	char Filename[256], ModelName[256];
	char *CurrentPointer;
	char *Buffer;

#if defined(UNDER_CE)
	sprintf(Filename, DISKNAME"\\Archives\\%s.tc", CurrentCarName);
#else
	sprintf(Filename, ".\\Archives\\%s.tc", CurrentCarName);
#endif
	arcPush(Filename);

	Buffer = archivestack.MallocAndExtract("Description", FMT_TXT_PARAM);

	// Find the necessary data.
	CurrentPointer = FindValue(Buffer, "BODY");
	ReadText(CurrentPointer, (char *)BodyName);
	CurrentPointer = FindValue(Buffer, "FRONT_AXLE");
	ReadText(CurrentPointer, (char *)FrontAxleName);
	CurrentPointer = FindValue(Buffer, "REAR_AXLE");
	ReadText(CurrentPointer, (char *)RearAxleName);

	// Load the wheel positions.
	CurrentPointer = FindValue(Buffer, "FRONT_WHEEL");
	ReadText(CurrentPointer, (char *)FrontWheelName);
	ReadVector(CurrentPointer, WheelPosition[0]);

	// Copy the wheel 0 position to wheel 1.
	WheelPosition[1] = WheelPosition[0]; WheelPosition[1].x = -WheelPosition[1].x;

	CurrentPointer = FindValue(Buffer, "REAR_WHEEL");
	ReadText(CurrentPointer, (char *)RearWheelName);
	ReadVector(CurrentPointer, WheelPosition[2]);

	// Copy the wheel 2 position to wheel 3.
	WheelPosition[3] = WheelPosition[2]; WheelPosition[3].x = -WheelPosition[3].x;

	dprintf("Body name loaded: %s", BodyName);
	GetLevel1Model(BodyName, ModelName);
	CarObject = new FrontEndObject(NULL, ModelName, 1.0f, NULL, true);
	GetLevel1Model(FrontAxleName, ModelName);
	FrontAxle = new FrontEndObject(NULL, ModelName, 1.0f, CarObject, true);
	GetLevel1Model(RearAxleName, ModelName);
	RearAxle = new FrontEndObject(NULL, ModelName, 1.0f, CarObject, true);	
	GetLevel1Model(FrontWheelName, ModelName);
	Wheel[0] = new FrontEndObject(NULL, ModelName, 1.0f, CarObject);
	Wheel[1] = new FrontEndObject(NULL, ModelName, 1.0f, CarObject);
	GetLevel1Model(RearWheelName, ModelName);
	Wheel[2] = new FrontEndObject(NULL, ModelName, 1.0f, CarObject);
	Wheel[3] = new FrontEndObject(NULL, ModelName, 1.0f, CarObject);
	arcPop();
	free32(Buffer);

	for (int i=0 ; i<4 ; i++)
		Wheel[i]->MoveInModelSpace(&WheelPosition[i]);
}

void FrontEndCar::GetLevel1Model(char *Name, char *Model)
{
	char *Buffer;
	Buffer = archivestack.MallocAndExtract(Name, FMT_TXT_MODDESC);
	int Length = strchr(Buffer, ' \t') - Buffer;

	strncpy(Model, Buffer, Length);
	Model[Length] = 0;

	free32(Buffer);
}

void FrontEndCar::Rotate(float x, float y, float z)
{
	CarObject->Rotate(x,y,z);
}

void FrontEndCar::SetCarPosition(int x, int y, float z)
{
	CarObject->Draw(x,y,z);
	CarObject->Hide();
}

void FrontEndCar::SetNamePosition(int x, int y, float z)
{
	CarName->Draw(x,y,z);
	CarName->Hide();
}

void FrontEndCar::SetGraphPosition(int x, int y, float z)
{
	if (reallanguage == LG_GERMAN || reallanguage == LG_SPANISH)
		CarGraph->Draw(x-30,y,z);
	else
		CarGraph->Draw(x,y,z);
	CarGraph->Hide();
}

void FrontEndCar::Draw(bool ModelOnly)
{
	CarObject->Draw();
	if (!ModelOnly)
	{	
		CarName->Draw();
		CarGraph->Draw();
	}
}

void FrontEndCar::Hide()
{
	CarObject->Hide();
	CarName->Hide();
	CarGraph->Hide();
}

int FrontEndCar::GetNumber()
{
	return Number;
}

FrontEndCar::~FrontEndCar()
{
	CarObject->DeleteModel();
	FrontAxle->DeleteModel();
	RearAxle->DeleteModel();
	Wheel[0]->DeleteModel();
	
	delete FrontAxle;
	delete RearAxle;

	for (int i=0 ; i<4 ; i++) delete Wheel[i];
	delete CarObject;

	CarName->DeleteModel();
	delete CarName;
	CarGraph->DeleteModel();
	delete CarGraph;
}