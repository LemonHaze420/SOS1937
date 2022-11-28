//================================================================================================================================
// font.h
// ------
//================================================================================================================================

#ifndef _BS_font
#define _BS_font

class fontchar
{
public:
	float x, y, w, h;
	bool defined;				// does the character exist in this font?
};

typedef enum { JUST_LEFT,JUST_CENTRE,JUST_RIGHT } justType;


class font
{
private:
	DWORD handle;	// texture handle
	char name[64];
	fontchar  character[128];		// each character

public:

	font( char * filename );
	~font();

	void RenderString(int xd,int yd,char *s,justType justification = JUST_LEFT);
	int getSize(char *s);

	void printf(int xd,int yd,char *s,...);

	float kern,space,lineheight;

	float oneoverwidth, oneoverheight;		// reciprocals of bitmap dimensions
};

#endif	// _BS_font
