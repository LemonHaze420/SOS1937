// $Header

// $Log

CycledBox::CycledBox(int _InitialColour, int _FinalColour, int _StartCycle, int _EndCycle)
{
	InitialColour = _InitialColour;
	FinalColour = _FinalColour;
	StartCycle = _StartCycle;
	EndCycle = _EndCycle;
}

int CycledBox::CurrentColour(float t)
{
	int Colour[4], TargetColour[4];
	float StartColour[4], FinishColour[4];
	for (int i=0 ; i<4 ; i++)
	{
		Colour[i] = (InitialColour >> (i*8)) & 0xFF;
		TargetColour[i] = (FinalColour >> (i*8)) & 0xFF;
		StartColour[i] = ((StartCycle >> (i*8)) & 0xFF) / 256.0f;
		FinishColour[i] = ((EndCycle >> (i*8)) & 0xFF) / 256.0f;
	}

	for (i=0 ; i<4 ; i++)
	{
		if (t > FinishColour[i])
			Colour[i] = TargetColour[i];
		else if (t < StartColour[i])
		;
		else
			Colour[i] += (int)(((float)(t-StartColour[i]) / (float)(FinishColour[i]-StartColour[i])) * (float)(TargetColour[i]-Colour[i]));
	}

	int NewColour;
	NewColour = Colour[0] | (Colour[1] << 8) | (Colour[2] << 16) | (Colour[3] << 24);

	return NewColour;
}