// $Header$

// $Log$


/* Registry emulation file for the ConsoleFrontLib

	Broadsword Interactive Ltd.

	Author:		Ian Gledhill
	Date:		29/11/1999
	Rev:		0.01 */

#include "ConsoleFrontLib.h"

struct RegistryNode *FindConsoleRegistryNode(char *KeyString)
{
	struct RegistryNode *CurrentNode = FirstNode;
	bool Found = false;

	while (!Found && CurrentNode)
	{
		if (!strcmp(KeyString, CurrentNode->Key))
		{
			Found = true;
		}
		else
			CurrentNode = CurrentNode->NextNode;
	}

	return CurrentNode;
}

void GetConsoleRegistryValue(char *RegistryKey, char *KeyString, char *KeyValue)
{
	// Use a default value.
	strcpy(KeyValue, "NULL");

	struct RegistryNode *FoundNode;
	FoundNode = FindConsoleRegistryNode(KeyString);
	if (FoundNode)
	{
		strcpy(KeyValue, FoundNode->Value);
	}

	return;
}

void SetConsoleRegistryValue(char *RegistryKey, char *KeyString, char *KeyValue)
{
	struct RegistryNode *FoundNode;
	FoundNode = FindConsoleRegistryNode(KeyString);

	if (!FirstNode)
	{
		FirstNode = new RegistryNode;
		strcpy(FirstNode->Key, KeyString);
		strcpy(FirstNode->Value, KeyValue);
		FirstNode->NextNode = NULL;
	}
	else
	{
		if (FoundNode)
		{
			strcpy(FoundNode->Value, KeyValue);
		}
		else
		{
			RegistryNode *ThisNode = new RegistryNode;
			ThisNode->NextNode = FirstNode;
			FirstNode = ThisNode;
			strcpy(FirstNode->Key, KeyString);
			strcpy(FirstNode->Value, KeyValue);
		}
	}
}
