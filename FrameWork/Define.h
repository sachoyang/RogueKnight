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

// ---------------------------------------------------------------------------
// [보안] DB 자격증명은 소스에 하드코딩하지 않는다.
//   - 평문 비밀번호를 커밋하면 git 히스토리에 영구히 남는다.
//   - 실제 사용 시 config.ini / 환경변수에서 읽어오도록 분리할 것.
//   (DB 기능은 현재 빌드에서 제외되어 있음: Mysql.cpp / Mysql.h)
// ---------------------------------------------------------------------------

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
