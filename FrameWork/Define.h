#pragma once

// 화면 크기 (constexpr: 타입/스코프 보장, 디버거에서 보임)
// 주: 기존 매크로명 SCREEN_WITH(오타) 는 호출부 호환을 위해 그대로 유지한다.
constexpr int SCREEN_WITH   = 1280;   // 화면 가로 픽셀크기
constexpr int SCREEN_HEIGHT = 800;    // 화면 세로 픽셀크기

constexpr int FRAME = 1000;

// 방향
constexpr int LEFT  = 0;
constexpr int RIGHT = 1;
constexpr int UP    = 2;
constexpr int DOWN  = 3;

enum gamechap
{
	LOGO = 0,
	MENU = 1,
	GAME = 2,
	OVER = 3,
	ENDING, // 엔딩 화면
	PAUSE,
	TOTALCHAP
};

enum alphabet
{
	A_, B_, C_, D_, E_, F_, G_, H_, I_, J_, K_, L_
};
enum gamework
{
	INIT = 0,
	WORK = 1,
	DRAW = 2
};
