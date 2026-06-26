#include "Include.h"


Over::Over()
{
}

Over::~Over()
{
}

void Over::Init()
{
	m_Title.Create("./resource/Img/UI/gameover_text.png", false, 0);
	D3DXGetImageInfoFromFile("./resource/Img/UI/gameover_text.png", &imagesinfo);

	m_Alpha = 0; // 투명하게 시작 (서서히 나타남)
}

// Chap, 재정의 함수 호출
void Over::Update(double frame)
{
	// 페이드 인 연출만 담당
	if (m_Alpha < 255) {
		m_Alpha += 2;
		if (m_Alpha > 255) m_Alpha = 255;
	}
}
//m_Title.SetColor(255, 255, 255, m_Alpha);

//m_Title.Render((SCREEN_WITH - imagesinfo.Width) / 2, 0, 0, 1, 1);
void Over::Draw()
{
	//if (g_Mng.chap[GAME]) {
	//	g_Mng.chap[GAME]->Draw();
	//}
	//if (m_Alpha > 0) {
	//	// 텍스트(GAMEOVER) 그리기
	//	m_Title.SetColor(255, 255, 255, m_Alpha); // Alpha 적용 (서서히 나타남)
	//	m_Title.Render((SCREEN_WITH - imagesinfo.Width) / 2, 0, 0, 1, 1);
	//	// 안내 문구
	//	if (m_Alpha >= 255) {
	//		if ((GetTickCount() / 500) % 2 == 0) {
	//			dv_font.DrawString("PRESS ANY BUTTON TO TITLE",
	//				SCREEN_WITH / 2 - 150, 600,
	//				D3DCOLOR_ARGB(255, 255, 255, 255));
	//		}
	//	}
	//}

	// 1. 게임 화면(배경) 그리기
	if (g_Mng.chap[GAME]) g_Mng.chap[GAME]->Draw();

	// 🌟 [핵심] MapManager가 마지막에 그렸던 것과 똑같은 상태(완전 어둠 + 글씨)를 덧그림
	// (이미 MapManager::Draw() 안에서 isDead 체크 후 검은 화면을 그리고 있으므로,
	//  g_Mng.chap[GAME]->Draw() 만 호출해도 화면은 어둡게 나옵니다!)

	// 다만, 확실하게 하기 위해 여기서도 안내 문구(PRESS ANY BUTTON)를 추가로 그립니다.
	if ((GetTickCount() / 500) % 2 == 0)
	{
		dv_font.DrawString("PRESS ANY BUTTON TO TITLE", SCREEN_WITH / 2 - 150, 600, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
}

void Over::OnMessage( MSG* msg )
{
	// 연출이 다 끝난 뒤에만 입력을 받음
	if (m_Alpha >= 255)
	{
		switch (msg->message)
		{
		case WM_KEYDOWN: // 키보드 아무거나 눌렀을 때
			g_Mng.n_Chap = MENU;
			break;

		case WM_LBUTTONDOWN: // 마우스 클릭했을 때
			g_Mng.n_Chap = MENU;
			break;
		}
	}
}