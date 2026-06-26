//#include <windows.h>
//#include <mmsystem.h>
#include "Include.h" 
const int TICKS_PER_SECOND = 60;
const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
const int MAX_FRAMESKIP = 5;
int loops;
float interpolation;
////////////////////////////////////
LRESULT CALLBACK WndProc( HWND g_hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASSEX wc ;
	HWND g_hWnd ;
	wc.hInstance = GetModuleHandle( NULL ) ;
	wc.cbSize = sizeof( wc ) ;
	wc.style = CS_CLASSDC ;
	wc.cbClsExtra = NULL ;
	wc.cbWndExtra = NULL ;
	wc.hbrBackground = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
	//wc.hCursor = LoadCursor( wc.hInstance, IDC_ARROW ) ;
	wc.hCursor = (HCURSOR)LoadImage(
		NULL,                           // 파일에서 로드할 때는 NULL
		TEXT("./resource/Img/UI/Cursor.cur"), // 커서 파일 경로 (.cur)
		IMAGE_CURSOR,                   // 이미지 타입
		0, 0,                           // 기본 크기 사용 (0, 0)
		LR_LOADFROMFILE | LR_DEFAULTSIZE // 파일 로드 옵션
	);
	wc.hIcon = LoadIcon( wc.hInstance, IDI_APPLICATION ) ;
	wc.hIconSm = LoadIcon( wc.hInstance, IDI_APPLICATION ) ;
	wc.lpfnWndProc = WndProc ;
	wc.lpszClassName ="Game" ;
	wc.lpszMenuName = NULL ;

	RegisterClassEx( &wc ) ;

	/* ---------------------------------------------------
	/////////////////// 정가운데 출력 ////////////////////

				프로그램을 모니터의 정가운데
				출력하게 해 주는 코드입니다.

	//////////////////////////////////////////////////////
	----------------------요-기-서-부-터-----------------*/
	RECT rt = {0, 0, SCREEN_WITH, SCREEN_HEIGHT};
	int W = rt.right - rt.left;
	int H = rt.bottom - rt.top;
	int X = (GetSystemMetrics(SM_CXSCREEN)/2) - SCREEN_WITH/2;
	int Y = ( (GetSystemMetrics(SM_CYSCREEN)/2) - SCREEN_HEIGHT/2 );
	/*---------------------요-기-까-지-------------------*/

	/*
		int WINAPI GetSystemMetrics(
			int nIndex
		);

		<인자값>
		SM_CXSCREEN : 화면의 넓이를 구할 수 있다.
		SM_CYSCREEN : 화면의 높이를 구할 수 있다.

		현재 화면의 해상도를 구할 수 있다. 이 외에도 인자값에따라 다양한 값을 구할 수 있다.
		자세한 사항은 MSDN 의 GetSystemMetrics 를 참고하길 바람
		단, 다중 모니터에서 값을 구할 경우 메인 모니터에 대해서만 값을 구하기때문에 이 함수를 사용하면 안된다.
		다중 모니터에서 전체 해상도를 구하길 원할경우 GetMonitorInfo 함수를 이용하자
	*/

	// WS_POPUP 빼면 테투리가 생긴다~
	g_hWnd = CreateWindowEx( NULL, wc.lpszClassName, 
		"Game", 
		WS_OVERLAPPED | 
		WS_CAPTION | 
		WS_SYSMENU | 
		WS_THICKFRAME | 
		WS_MINIMIZEBOX
		//WS_OVERLAPPEDWINDOW
		//WS_EX_TOPMOST|WS_POPUP, 
		,X, Y, W, H,
		NULL, NULL, wc.hInstance, NULL ) ;


	dv_font.Create(g_hWnd) ;
	//sound.g_pSoundManager = new CSoundManager();
	//sound.g_pSoundManager->Initialize(g_hWnd, DSSCL_PRIORITY);
	//sound.g_pSoundManager->SetPrimaryBufferFormat(2,22050,16);
	
	//g_SoundManager.Initialize(g_hWnd, DSSCL_PRIORITY);
	//g_SoundManager.SetPrimaryBufferFormat(2,22050,16);

	ShowWindow( g_hWnd, SW_SHOW ) ;
	UpdateWindow( g_hWnd ) ;
	// 마우스 안보이게~
	//ShowCursor(FALSE);

	///////////////////////////////////////////////////////////////////
	ZeroMemory(&msg, sizeof(MSG));
	//TRACE("REV ====== %s ========= \n\n", buffer);
	/////////// 챕터(씬) 초기화 /////////////////
	// unique_ptr 소유 → 종료 시 자동 해제
	g_Mng.chap[LOGO]   = std::make_unique<Logo>();
	g_Mng.chap[MENU]   = std::make_unique<Menu>();
	g_Mng.chap[GAME]   = std::make_unique<Game>();
	g_Mng.chap[OVER]   = std::make_unique<Over>();
	g_Mng.chap[ENDING] = std::make_unique<Ending>();
	/////////////////////////////////////////

	for (int i = 0; i < TOTALCHAP; i++)
	{
		if (g_Mng.chap[i])
		{
			g_Mng.chap[i]->Init();
		}
	}
	// Replace the main message loop with this
	while (msg.message != WM_QUIT)
	{
		// process all pending Windows messages
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			// 포워딩: 현재 장면에 메시지 전달 (null 검사)
			if (g_Mng.chap[g_Mng.n_Chap])
				g_Mng.chap[g_Mng.n_Chap]->OnMessage(&msg);
		}

		// fixed timestep update
		static ULONGLONG next_game_tick = GetTickCount64();
		loops = 0;
		while (GetTickCount64() > next_game_tick && loops < MAX_FRAMESKIP)
		{
			interpolation = float(GetTickCount64() + SKIP_TICKS - next_game_tick) / float(SKIP_TICKS);
			if (Gmanager.m_Pause == false)
				g_Mng.chap[g_Mng.n_Chap]->Update(interpolation);

			next_game_tick += SKIP_TICKS;
			loops++;
		}

		// render...
		dv_font.Device9->BeginScene();
		dv_font.Device9->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 0, 0);
		g_Mng.chap[g_Mng.n_Chap]->Draw();
		dv_font.Device9->EndScene();
		dv_font.Device9->Present(NULL, NULL, NULL, NULL);
	}

	return msg.wParam;
}

// 키 한번만 먹을거는 여기가 안전...아니면 귀찮다.(key.cpp 에서 연속키 안먹게 하기 위해 로직이 더러워짐)
LRESULT CALLBACK WndProc( HWND g_hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_ERASEBKGND:
		return 1;
	case WM_COMMAND:
		if (g_Mng.chap[g_Mng.n_Chap])
		{
			// 메시지 포장해서 OnMessage로 토스
			MSG msg;
			msg.hwnd = g_hWnd;
			msg.message = uMsg;
			msg.wParam = wParam;
			msg.lParam = lParam;

			g_Mng.chap[g_Mng.n_Chap]->OnMessage(&msg);
		}
		break;
	case WM_DESTROY:
         PostQuitMessage( 0 );
         return 0;
	case WM_CHAR:
		// 'p' 키로 일시정지 토글. 한 글자 명령은 직접 비교가 안전·명확하다.
		// (기존: 고정 버퍼 + strcat 누적 → 버퍼 오버플로 위험 제거)
		if (wParam == 'p' || wParam == 'P')
		{
			if (GetTickCount64() - key.KeyTime > 200)
			{
				Gmanager.m_Pause = !Gmanager.m_Pause;
				key.KeyTime = GetTickCount64();
			}
		}
		break;
	}
	return DefWindowProc( g_hWnd, uMsg, wParam, lParam ) ;
}