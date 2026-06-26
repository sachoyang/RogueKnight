#include <memory>

class CMng
{
public:
	CMng();
	~CMng();

	// 씬 소유는 unique_ptr 로 → 프로그램 종료 시 자동 해제 (수동 delete 불필요).
	// (Chap 가상 소멸자와 함께 동작하여 누수/UB 방지)
	std::unique_ptr<Chap> chap[TOTALCHAP];
	int n_Chap;

	HWND hWnd;
	HINSTANCE hInstance;

};

extern CMng g_Mng;