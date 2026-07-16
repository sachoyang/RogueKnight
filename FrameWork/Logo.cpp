#define _CRT_NONSTDC_NO_DEPRECATE

#include "Include.h"

Logo::Logo()
{
	count = 0; //g_Mng.n_Chap = GAME;
}

Logo::~Logo()
{
}

void Logo::Init()
{
	speed = 800;
	alpha = 0;

	//loadimg.Create("./resource/Img/Load/tree.png", false, D3DCOLOR_XRGB(0, 0, 0));
	loadimg.Create("./resource/Img/Load/logo1.png", false, D3DCOLOR_XRGB(0, 0, 0));
	//g_SoundManager.Create(&Sound1, (LPWSTR)L"sound/hatnim.wav", 0, GUID_NULL);
}

// Chap, 재정의 함수 호출
void Logo::Update(double frame)
{
	alpha += speed * frame / 1000;
	if( alpha >= 255 )
	{
		alpha = 255;
		speed = -speed;
		//Draw();
	}
	if(alpha <= 0) {
		 g_Mng.n_Chap = MENU;
	}
	//Sound1.Play(0, DSBPLAY_LOOPING);
	
}

void Logo::Draw()
{
	loadimg.SetColor(255,255,255, alpha);  // 색상 변경
	loadimg.Draw( 0 , 0 ) ;  //이미지출력
	//dv_font.DrawString( "로고" , 200 , 210 ) ;   //글자출력
}

void Logo::OnMessage( MSG* msg )
{
	switch(msg->message)
	{
	case WM_LBUTTONDOWN :
		if (g_Mng.n_Chap == LOGO)
		{
			g_Mng.n_Chap = MENU;
			break;
		}
		
	}

}