// Sound.h: interface for the Sound class.
//
//////////////////////////////////////////////////////////////////////

/*
	$Header: C:\\RCS\\C\\boris\\sound\\sound.h,v 1.2 1999/01/25 17:27:12 jcf Exp jcf $
	$Log: sound.h,v $
	Revision 1.2  1999/01/25 17:27:12  jcf
	general check-in after minor mods

	Revision 1.1  1998/12/02 12:05:26  jcf
	Initial revision

*/

#if !defined(AFX_SOUND_H__01EA9506_35B1_11D2_9D7E_0080C86B40F3__INCLUDED_)
#define AFX_SOUND_H__01EA9506_35B1_11D2_9D7E_0080C86B40F3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdarg.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include <math.h>

#define	INITSTRUCT(s)	{memset(&(s),0,sizeof((s)));(s).dwSize=sizeof((s));}
#define	NUMCHANS	32	// maximum number of sounds


// a playing channel

struct Channel
{
	LPDIRECTSOUNDBUFFER buf;	// the buffer being played
	LPDIRECTSOUND3DBUFFER buf3d;	// 3d interface to above, or NULL if not 3d
	class Sound *sound;				// the Sound it came from
	bool isdup;					// true if the buffer is a duplicate of that in the Sound
	bool muted;
	bool locked;
	unsigned long flags;
	int pri;
};



class SoundSystem  
{

	//	put all Sound classes here as friends
	friend class Sound;

private:
	struct SoundList * soundlist;
	void KillChan(int i);
	LPDIRECTSOUND3DLISTENER listener;
	int GetFreeChannel(int priority);
	bool GetCaps();
	bool DSError(char *place,HRESULT errVal);
	LPDIRECTSOUND	lpds;
	DSCAPS dscaps;
	Channel chans[NUMCHANS];
	int forcechannel;

	char error[128];
	void Error(char *s) { strcpy(error,s); }
	
public:
	Sound * GetSoundOnChannel(int i);
	void DeleteAllSounds();
	Sound * FindOrLoadSound(char *fn,bool pitchvary,bool load3d,int pri);

	// change - now two versions of SetListener

	void SetListener(float x,float y,float z,float xv,float yv,float zv,float heading);
	void SetListener(float x,float y,float z,
		float xv,float yv,float zv,
		float upx,float upy,float upz,
		float forwx,float forwy,float forwz);

	void SetVelocity(int s,float xv,float yv,float zv);
	void SetPosition(int s,float x,float y,float z);

	void ForceNextChannel(int s) { forcechannel=s; }

	int Play3D(Sound * s, bool loop,float x, float y, float z,
		float xv, float yv, float zv,float vol,float pitch);
	int Play(Sound * s,bool loop,float vol,float pan,float pitch);

	void SetVolume(int i,float v);
	void SetPan(int i,float v);
	void SetFrequency(int i,float v);
	void Stop(int i);

	void Mute(int i);
	void Unmute(int i);

	void Lock(int i) { chans[i].locked=true; }
	void Unlock(int i) {chans[i].locked=false; }

	void StopAllChannels();
	void Shutdown();
	bool Initialise(void *hwnd);
	bool Initialise(long hwnd);
	char *GetError() { return error;}

	SoundSystem();
	virtual ~SoundSystem();

};

extern SoundSystem soundsys;





class Sound  
{
	friend class SoundSystem;
	int priority;
	bool Play3D(Channel *c,
			bool loop,float x,float y,float z,float xv,float yv,float zv,float vol,float pitch);
	bool loaded;
	bool is3d;

	float origpitch;

	LPDIRECTSOUNDBUFFER buf;

	LPDIRECTSOUNDBUFFER Duplicate();
	bool Play(Channel *c,bool loop,float vol,float pan,float pitch);

public:
	int mutedcount;
	char name[128];
	void Unload();
	bool Load(char *fn,bool pitchvary,bool load3d,int pri);
	bool IsPlaying();
	Sound();
	virtual ~Sound();

};

// list of sounds for the librarian

struct SoundList
{
	Sound s;
	struct SoundList *next;
};





#endif // !defined(AFX_SOUND_H__01EA9506_35B1_11D2_9D7E_0080C86B40F3__INCLUDED_)
