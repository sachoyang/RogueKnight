#pragma once

/* ---------------------------------------------------
///////////////// 프로그램 파일 //////////////////////

		라이브러리에 필요한 파일입니다.
		지우면 안돼요~~^^;

//////////////////////////////////////////////////////
-----------------------------------------------------*/
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dsound.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")
// 데이타 베이스///////////////////
//#include <my_global.h>
#include <winsock2.h>
//#include <mysql.h>
//#pragma comment(lib, "libmySQL.lib")

//////////////////////////////////

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include <d3d9.h>
#include <d3dx9.h>
#include <dsound.h>
#include "DXUTsound.h"

#include <list>

#include "Define.h"
#include "DeviceFont.h"
#include "Sprite.h"
#include "Chap.h"

#include "FmodSound.h"

/* ---------------------------------------------------
///////////////// 사용자 추가 파일 ///////////////////

		새로 클래스 만들 때 마다
		헤더파일을 추가해 주세요~

//////////////////////////////////////////////////////
-----------------------------------------------------*/
// =========================================
// 1단계: 코어 (가장 기본 도구들)
// =========================================
#include "Camera.h"
#include "Key.h"
#include "Sound.h"
#include "Collider.h"

// =========================================
// 2단계: 게임 엔티티 (실제로 화면에 등장하는 애들)
// =========================================
#include "Enemy.h"  
#include "Boss.h"
#include "Knight.h" 

// =========================================
// 3단계: 매니저 시스템 (엔티티들을 관리하는 애들)
// =========================================
#include "MapManager.h" 
#include "UIManager.h"  
#include "TimeManager.h"
#include "EffectManager.h"
// =========================================
// 4단계: 최상위 게임 루프
// =========================================
#include "GameManager.h" 
#include "CMng.h"

// =========================================
// 5단계: 씬(Scene) 및 메인 
// =========================================
#include "Logo.h"
#include "Menu.h"
#include "Over.h"
#include "Game.h"
#include "Ending.h"

// 데이타 베이스///////////////////
//#include "Mysql.h"

/* ---------------------------------------------------
///////////////// 사용자 추가 파일 ///////////////////

		키 입력 처리 함수입니다.

//////////////////////////////////////////////////////
-----------------------------------------------------*/

inline int KeyDown	( int vk ) 
{ 
	return ( (GetAsyncKeyState(vk) & 0x8000) ? 1:0 ); 
}

inline int KeyUp	( int vk ) 
{ 
	return ( (GetAsyncKeyState(vk) & 0x8000) ? 0:1 ); 
}
