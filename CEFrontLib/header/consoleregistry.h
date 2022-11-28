// $Header$

// $Log$

/* Registry emulation file for the ConsoleFrontLib

	Acts as an emulation layer between the testmain registry handling, and
	  the console 

	Broadsword Interactive Ltd.

	Author:		Ian Gledhill
	Date:		29/11/1999
	Rev:		0.01 */

#ifndef __CONSOLEREGISTRY_H__
#define __CONSOLEREGISTRY_H__

struct RegistryNode
{
	char Key[32], Value[64];
	struct RegistryNode *NextNode;
};

static struct RegistryNode *FirstNode=NULL;

void GetConsoleRegistryValue(char *RegistryKey, char *KeyString, char *KeyValue);
void SetConsoleRegistryValue(char *RegistryKey, char *KeyString, char *KeyValue);

#endif