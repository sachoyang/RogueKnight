#include "Include.h"

Key key;

Key::Key(void)
{

	
}

Key::~Key(void)
{
}

// 동시 키를 위하여 KeyTime, KeyTime1,KeyTime2,KeyTime3 따로 처리

void Key::Update()
{
	if (knight.isCutscene) // 컷신 중에는 키 입력 무시
	{
		return;
	}
	knight.isMove = false;
	knight.isLookup = false;
	knight.isLookdown = false;
	// 시스템에 따라...GetTickCount/GetTickCount64
	if(KeyDown('1'))
	{
		if(GetTickCount64() - KeyTime > 200)
		{
			KeyTime = GetTickCount64();
		}
	}

	if(KeyDown('2'))
	{
		if(GetTickCount64() - KeyTime > 200)
		{
			KeyTime = GetTickCount64();
		}
		
	}

	if(KeyDown('3'))
	{
		if(GetTickCount64() - KeyTime > 200)
		{
			KeyTime = GetTickCount64();
		}
		
	}
	
	if(KeyDown('4'))
	{
		if(GetTickCount64() - KeyTime > 200)
		{
			KeyTime = GetTickCount64();
		}
		
	}

	if(KeyDown('5'))
	{
		if(GetTickCount64() - KeyTime > 200)
		{
			KeyTime = GetTickCount64();
		}
	
	}
	
	if(KeyDown('6'))
	{
		if(GetTickCount64() - KeyTime > 200)
		{
			KeyTime = GetTickCount64();
		}
	}



	if(KeyDown('7'))
	{
		if(GetTickCount64() - KeyTime > 200)
		{
			KeyTime = GetTickCount64();
		}
		
	}



	
	

	if(KeyDown('8'))
	{
		if(GetTickCount64() - KeyTime > 200)
		{

		}
		
	}

	if(KeyDown('9'))
	{
		if(GetTickCount64() - KeyTime > 200)
		{

			KeyTime = GetTickCount64();
		}
		
	}


	if(KeyDown('0'))
	{
		if(GetTickCount64() - KeyTime > 200)
		{

			KeyTime = GetTickCount64();
		}
		
	}


	if(KeyDown(VK_F1))
	{
		if(GetTickCount64() - KeyTime1 > 10)
		{	
			Camera::GetInstance()->SetXCam(1);
			KeyTime1 = GetTickCount64();
		}
	
	}

	
	if(KeyDown(VK_F2))
	{
		if(GetTickCount64() - KeyTime1 > 10)
		{	
			Camera::GetInstance()->SetXCam(-1);
			KeyTime1 = GetTickCount64();
		}
	
	}

	
	if(KeyDown(VK_F3))
	{
		if(GetTickCount64() - KeyTime > 200)
		{	
			g_Mng.n_Chap = ENDING;

			KeyTime = GetTickCount64();
		}
	
	}

	
	if(KeyDown(VK_F4))
	{
		if(GetTickCount64() - KeyTime > 200)
		{	
			KeyTime = GetTickCount64();
		}
	
	}

	
	if(KeyDown(VK_F5))
	{
		if(GetTickCount64() - KeyTime > 200)
		{
			KeyTime = GetTickCount64();
		}
	
	}

	
	if(KeyDown(VK_F6))
	{
		if(GetTickCount64() - KeyTime > 200)
		{	
			KeyTime = GetTickCount64();
		}
	
	}

	
	if(KeyDown(VK_F7))
	{
		if(GetTickCount64() - KeyTime > 200)
		{	
		
			KeyTime = GetTickCount64();
		}
	
	}

	
	if(KeyDown(VK_F8))
	{
		if(GetTickCount64() - KeyTime > 200)
		{		
			KeyTime = GetTickCount64();
		}
	
	}

	
	if(KeyDown(VK_F9))
	{
		if(GetTickCount64() - KeyTime > 200)
		{	
			KeyTime = GetTickCount64();
		}
	
	}

	
	if(KeyDown(VK_F10))
	{
		if(GetTickCount64() - KeyTime > 200)
		{
			KeyTime = GetTickCount64();
		}
	
	}

	
	if(KeyDown(VK_F11))
	{
		if(GetTickCount64() - KeyTime > 200)
		{
			KeyTime = GetTickCount64();
		}
	
	}

	
	if(KeyDown(VK_F12))
	{
		if(GetTickCount64() - KeyTime > 200)
		{	
			KeyTime = GetTickCount64();
		}
	
	}
	


	if(KeyDown(VK_SPACE) ) 
	{
		
		if(GetTickCount64() - KeyTime2 > 200)
		{
			KeyTime2 = GetTickCount64();
		}
		
	}

	if (!knight.isDead)
	{
		if (knight.isDashing == false)
		{
			if (KeyDown(VK_LEFT))
			{
				if (GetTickCount64() - KeyTime1 > 10)
				{
					knight.isMove = TRUE;
					knight.pos.x -= 4.0f;
					knight.dir = 1;

					KeyTime1 = GetTickCount64();
				}
			}

			if (KeyDown(VK_RIGHT))
			{
				if (GetTickCount64() - KeyTime1 > 10)
				{
					knight.isMove = TRUE;
					knight.pos.x += 4.0f;
					knight.dir = -1;

					KeyTime1 = GetTickCount64();
				}
			}
		}



		if (KeyDown(VK_UP))
		{
			if (GetTickCount64() - KeyTime3 > 10)
			{
				knight.isLookup = TRUE;
				KeyTime3 = GetTickCount64();
			}

		}

		if (KeyDown(VK_DOWN))
		{

			if (GetTickCount64() - KeyTime3 > 10)
			{
				knight.isLookdown = TRUE;
				KeyTime3 = GetTickCount64();
			}

		}
	}

	

	// 1. 점프 시작 (누르고 있을 때)
	if (KeyDown('Z'))
	{
		// 땅에 있을 때만 점프 시작 (함수 내부에서 grounded 체크함)
		knight.JumpStart();
	}
	else
	{
		// 2. 점프 컷 (키를 뗐을 때)
		// Z키가 눌리지 않은 상태라면 무조건 CutJump 호출
		// (함수 내부에서 '상승 중일 때만' 작동하도록 되어 있어서 안전함)
		knight.JumpCut();
	}

	//if (GetAsyncKeyState('X') & 0x8000)
	//{
	//	knight.AttackStart();
	//}
	if (KeyDown('X'))
	{
		knight.AttackStart();
	}

	if(KeyDown('C') )
	{
		knight.DashStart();
	}




	
	if(KeyDown('N'))
	{
		
		if(GetTickCount64() - KeyTime > 200)
		{			
			KeyTime = GetTickCount64();
		}
	}

	
	if(KeyDown('A'))
	{
		if(GetTickCount64() - KeyTime > 200)
		{
			KeyTime = GetTickCount64();
		}
	}




	if(KeyDown('W') )
	{
		if(GetTickCount64() - KeyTime > 200)
		{
			
			KeyTime = GetTickCount64();
		}

	}


	if(KeyDown('E')  )
	{
		if(GetTickCount64() - KeyTime > 200)
		{	
			KeyTime = GetTickCount64();
		}
		
	}


	if(KeyDown('R') )
	{
		if(GetTickCount64() - KeyTime > 200)
		{

			KeyTime = GetTickCount64();
		}
	}


	if(KeyDown('S') )
	{
		if(GetTickCount64() - KeyTime > 200)
		{
	
			KeyTime = GetTickCount64();
		}
	}


	if(KeyDown('D') )
	{
		if(GetTickCount64() - KeyTime > 200)
		{
			
			KeyTime = GetTickCount64();
		}
	}


	if(KeyDown(VK_RETURN) ) 
	{
		
		if(GetTickCount64() - KeyTime1 > 200)
		{

			KeyTime1 = GetTickCount64();
		}
		
	}
}