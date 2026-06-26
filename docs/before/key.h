#pragma once
#include "Include.h"

class Key
{

public:
	Key(void);
	~Key(void);
	
	DWORD KeyTime;
	// 동시 키 선택을 위함
	DWORD KeyTime1;
	DWORD KeyTime2;
	DWORD KeyTime3;
	

	void Update();
};

extern Key key;