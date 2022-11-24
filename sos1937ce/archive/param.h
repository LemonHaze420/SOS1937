// $Header$

// $Log$


class BroadParamFile
{
public:
	BroadParamFile();
	~BroadParamFile();

	bool Read(char *name);	// read the PF into memory - makes a copy of the data

	// search the file for the given key - if NULL, look for more occurrences
	// of the previous Find key. Returns the value, or NULL if the key
	// is not found.

	// If length is -1 (or no length is given), the whole key is compared.
	// If a number is specified, only the first n character are compared. If
	// zero is given, all keys match - you can iterate over the entire file.
	
	char *Find(char *key,int len=-1);

	// return the value/key of the last found item

	char *Value() { return (curoff>=0) ? vals[curoff] : NULL; }
	char *Key() { return (curoff>=0) ? keys[curoff] : NULL; }

private:
	char curkey[64];	// current search key
	char *data;	// array of null-sepped strings
	char **keys;	// offsets of key strings in 'data'
	char **vals;	// offsets of value strings in 'data'
	int offset;	// the current search point
	int num;	// number of items
	int curoff;	// last found item
	int lastlen;// last length parameter to Find for when key=NULL
};
