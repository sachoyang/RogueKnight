#include "Include.h"

static DWORD KeyTime = GetTickCount();
float menuScale = 0.3f;

Menu::Menu()
{
}

Menu::~Menu()
{
}

void Menu::Init()
{
	//menuimg.Create("./resource/Img/lobby/Lobby.png", false, D3DCOLOR_XRGB(0, 0, 0));

	// 1. 배경 및 버튼 이미지 로딩 (파일 경로는 본인 프로젝트에 맞게!)
	menuimg.Create("./resource/Img/lobby/title.png",false, D3DCOLOR_XRGB(0, 0, 0));
	D3DXGetImageInfoFromFile("./resource/Img/lobby/title.png", &imagesinfo[0]);

	m_BtnStart.Create("./resource/Img/lobby/Btn_Start.png", false, D3DCOLOR_XRGB(0, 0, 0));
	D3DXGetImageInfoFromFile("./resource/Img/lobby/title.png", &imagesinfo[1]);
	m_BtnExit.Create("./resource/Img/lobby/Btn_Exit.png", false, D3DCOLOR_XRGB(0, 0, 0));
	D3DXGetImageInfoFromFile("./resource/Img/lobby/title.png", &imagesinfo[2]);

	// 2. 버튼 위치 설정 (RECT: left, top, right, bottom)
	// 예: 시작 버튼은 (300, 300) 위치, 크기 200x80
	m_rtStart.left = (SCREEN_WITH - imagesinfo[1].Width * menuScale)/2;
	m_rtStart.top = 450;
	m_rtStart.right = m_rtStart.left + imagesinfo[1].Width* menuScale;  // 너비
	m_rtStart.bottom = 450 + imagesinfo[1].Height* menuScale;  // 높이

	// 예: 종료 버튼은 (300, 450) 위치, 크기 200x80
	m_rtExit.left = (SCREEN_WITH - imagesinfo[2].Width * menuScale) / 2;
	m_rtExit.top = 600;
	m_rtExit.right = m_rtStart.left + imagesinfo[2].Width* menuScale;
	m_rtExit.bottom = 600 + imagesinfo[2].Height* menuScale;
}

// Chap, 재정의 함수 호출
void Menu::Update(double frame)
{

	//key.Update();

	//if (xx.xxx > 100 * xxx.xxx)
	//{
	//	if (GetTickCount() - CoinTime > 100)
	//	{
	//		count++;
	//		CoinTime = GetTickCount();
	//	}
	//	if (count > 1) count = 0;
	//}
}

void Menu::Draw()
{
	//menuimg.Render(-50, 0, 0, 0.75, 0.75); //이미지출력
	//dv_font.DrawString("로비", 200, 210);   //글자출력

	// 배경 먼저 그리기
	menuimg.Render((SCREEN_WITH -imagesinfo[0].Width)/2, 0, 0, 1, 1);
	// 버튼 그리기 (RECT의 left, top 좌표 사용)
	//m_BtnStart.Draw(m_rtStart.left, m_rtStart.top);
	m_BtnStart.Render(m_rtStart.left, m_rtStart.top, 0, menuScale, menuScale);

	//m_BtnExit.Draw(m_rtExit.left, m_rtExit.top);
	m_BtnExit.Render(m_rtExit.left, m_rtExit.top, 0, menuScale, menuScale);

	SOUND->PlayBGM(SND_BGM_HK);
	//if (GetTickCount() - AlTime > 100)
	//{
	//	alpha++;
	//	AlTime = GetTickCount();
	//}

	//if (alpha == 0) menuimg[2].SetColor(255, 255, 255, 255);
	//if (alpha == 1) menuimg[2].SetColor(255, 255, 255, 0);

	//if (xxx.xxx == 0) menuimg[2].Draw(407, 336);
	//else if (count < 2) menuimg[count].Draw(412, 276);

}

void Menu::OnMessage(MSG* msg)
{
	//char aaa[256];
	//_itoa_s(msg->message, aaa,2);
	//dv_font.DrawString(aaa, 10 , 210 ) ;   
	//PostQuitMessage(0);

	switch (msg->message)
	{
		case WM_KEYDOWN:
		{
			switch (msg->wParam) 
			{
			case VK_F12:
				//MessageBox(NULL, "", "", 0);
				if (g_Mng.n_Chap == MENU) {

					g_Mng.n_Chap = GAME;
					//sound.m_Bk1->Play(0, DSBPLAY_LOOPING);

				}
				break;
			}
			break;
		}


		case WM_LBUTTONDOWN:
		{
			POINT pt;
			pt.x = LOWORD(msg->lParam);
			pt.y = HIWORD(msg->lParam);
			// 시작 버튼 클릭 여부 확인
			if (PtInRect(&m_rtStart, pt))
			{
				if (g_Mng.n_Chap == MENU)
				{
					coll.Init();
					CAM->Init();
					CAM->SetTarget(&knight.pos);
					mapMng.Init();
					knight.Init();
					SOUND->Init();
					Gmanager.Init();
					EFFECT->Init();
					uiMng.Init();

					g_Mng.n_Chap = GAME;
					//sound.m_Bk1->Play(0, DSBPLAY_LOOPING);
				}
			}
			// 종료 버튼 클릭 여부 확인
			else if (PtInRect(&m_rtExit, pt))
			{
				// 종료 버튼 클릭 시 처리
				PostQuitMessage(0);
			}
			break;
		}
		
	}
}