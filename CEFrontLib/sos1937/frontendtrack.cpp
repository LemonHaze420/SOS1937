/* The car object */

#include "ConsoleFrontLib.h"

extern struct GlobalStructure MainGameInfo;

FrontEndTrack::FrontEndTrack(int _Number)
{
	Number = _Number;

	TrackName = TrackNameModels[Number];
	TrackGraph = TrackStatsModels[Number];
	TrackObject = TrackModels[Number];
}


void FrontEndTrack::Rotate(float x, float y, float z)
{
	TrackObject->Rotate(x,y,z);
}

void FrontEndTrack::SetTrackPosition(int x, int y, float z)
{
	TrackObject->Draw(x,y,z);
	TrackObject->Hide();
}

void FrontEndTrack::SetNamePosition(int x, int y, float z)
{
	TrackName->Draw(x,y,z);
	TrackName->Hide();
}

void FrontEndTrack::SetGraphPosition(int x, int y, float z)
{
	TrackGraph->Draw(x,y,z);
	TrackGraph->Hide();
}

void FrontEndTrack::Draw()
{
	TrackObject->Draw();
	TrackName->Draw();
	TrackGraph->Draw();
}

void FrontEndTrack::Hide()
{
	TrackObject->Hide();
	TrackName->Hide();
	TrackGraph->Hide();
}

int FrontEndTrack::GetNumber()
{
	return Number;
}


FrontEndTrack::~FrontEndTrack()
{
	TrackObject->Hide();
	TrackName->Hide();
	TrackGraph->Hide();
}