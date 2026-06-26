#define SCREEN_WITH 1280	// 화면 가로 픽셀크기
#define SCREEN_HEIGHT 800	// 화면 세로 픽셀크기

#define FRAME 1000
#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3

// 데이타 베이스
#define DB_HOST "127.0.0.1"
#define DB_USER "root"
#define DB_PASS "rlacodnjs0801!"
#define DB_NAME "test"
#define CHOP(x) x[strlen(x) - 1] = ' ' // 쓸려다가 안씀...

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