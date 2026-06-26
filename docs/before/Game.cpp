#include "Include.h"

Game::Game() : m_pPause(nullptr), m_bPaused(false)
{
}

Game::~Game()
{
	if (m_pPause) delete m_pPause;
}

void Game::Init()
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

	m_pPause = new Pause();
	m_pPause->Init();
	m_bPaused = false;

	m_TutorialImg.Create("./resource/Img/UI/Tutorial_Key.png", false, 0);
	// 데이타 베이스///////////////////
	// sql.Init();
}

void Game::Draw()
{
	mapMng.Draw();
	knight.Draw();
	EFFECT->Draw();
	coll.Draw();
	Gmanager.Draw();
	uiMng.Draw();

	// =========================================================
	// 🌟 튜토리얼 텍스트 (1번 맵일 때만!)
	// =========================================================
	if (mapMng.m_pCurrentMapChunk && mapMng.m_pCurrentMapChunk->id == 1)
	{
		// 배경 적절한 위치에 출력
		m_TutorialImg.Render(300 - CAM->GetX(), 300 - CAM->GetY(), 0, 1, 1, 1);
	}

	// =========================================================
	// ⏸️ 일시정지 화면 그리기 (Pause 객체에게 위임)
	// =========================================================
	if (m_bPaused && m_pPause)
	{
		m_pPause->Render();
	}
	// 데이타 베이스///////////////////
	// sql.Draw();
}

// Chap, 재정의 함수 호출
void Game::Update(double frame)
{
	//static int a = 0;
	//if(a == 0) 
	//{
	//	GameTime = GetTickCount();
	//	a = 1;
	//}
	static DWORD a = 0;

	if (GetTickCount64() - a > frame)
	{
		//Camera::GetInstance()->Update();
		if (uiMng.m_bLargeMap) {
			uiMng.Update();
			return;
		}		

		key.Update();

		if (m_bPaused)
		{
			if (m_pPause)
			{
				PAUSE_RESULT result = m_pPause->Update();

				if (result == PAUSE_RESUME)
				{
					m_bPaused = false; // 게임 재개
				}
				else if (result == PAUSE_TO_MENU)
				{
					m_bPaused = false;
					g_Mng.n_Chap = MENU; // 메뉴로 이동
				}
				// PAUSE_KEEP이면 아무것도 안 함 (계속 멈춤)
			}

			a = GetTickCount64();
			return;
		}

		uiMng.Update(); // UI업데이트

		TIMEMGR->UpdateTime();

		SOUND->Update();
		// =======================================================
		// [타임 컨트롤 발동] 엔진이 멈추라고 하면 아래 인게임 로직은 건너뜀
		// =======================================================
		if (!TIMEMGR->IsTimeStopped())
		{
			// 시간의 영향을 받는 진짜 인게임 오브젝트들
			knight.Update();
			coll.Update();
			mapMng.Update(130);
			Gmanager.Update();

			EFFECT->Update();

			CAM->Update(); // 카메라 업데이트
		}
		//// 입 맛에 맞게
		//mapMng.Update(130);
		//Gmanager.Update();

		// 데이타 베이스///////////////////
		// 입 맛에 맞게 (여기선 안쓰임..프레임 값이 필요 할때만.. 그냥 방법만...)
		//sql.Update(frame+3000);

		a = GetTickCount64();
	}

}


void Game::OnMessage( MSG* msg )
{
	// 게임 씬에서 특수 키 처리 (ESC로 일시정지 등)
	switch (msg->message)
	{
	case WM_KEYDOWN:
		switch (msg->wParam)
		{
		case VK_ESCAPE:
			// ESC 누르면 일시정지 토글!
			m_bPaused = !m_bPaused;
			break;

		case '1':
			mapMng.ChangeMap(1);
			break;
		case '2':
			mapMng.ChangeMap(2);
			break;

		// =======================================================
		// 디버그 모드 단축키 (Page Up / Page Down)
		// =======================================================
		case VK_PRIOR: // Page Up 키 (이전 프리팹)
			mapMng.LoadDebugPrefab(mapMng.m_DebugPrefabID - 1);
			break;

		case VK_NEXT:  // Page Down 키 (다음 프리팹)
			mapMng.LoadDebugPrefab(mapMng.m_DebugPrefabID + 1);
			break;
		// =======================================================
		// 미니맵 (M)
		// =======================================================
			// M 키를 누르면 맵 확대 & 일시정지 토글
		case 'M':
			uiMng.m_bLargeMap = !uiMng.m_bLargeMap;
			break;

		case VK_OEM_2: // '/' 키
			coll.isDebugDraw = !coll.isDebugDraw;
			break;
		}
		break;
	}
}