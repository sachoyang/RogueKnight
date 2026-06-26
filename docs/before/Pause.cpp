#include "Include.h"
#include "Pause.h"

Pause::Pause() : m_fVolumeRatio(1.0f), m_bDragSlider(false)
{
}

Pause::~Pause()
{
}

void Pause::Init()
{
	// 리소스 로드 (경로 확인해주세요!)
	m_PauseBg.Create("./resource/Img/UI/Pause_Bg.png", false, 0);
	m_PauseBg2.Create("./resource/Img/UI/Pause_Bg2.png", false, 0);
	m_BtnContinue.Create("./resource/Img/UI/Btn_Continue.png", false, 0);
	m_BtnMenu.Create("./resource/Img/UI/Btn_Menu.png", false, 0);
	m_SliderBar.Create("./resource/Img/UI/Slider_Bar.png", false, 0);
	m_SliderKnob.Create("./resource/Img/UI/Slider_Knob.png", false, 0);

	m_fVolumeRatio = 1.0f; // 기본 볼륨 100% (필요시 사운드 매니저에서 가져와도 됨)
}

PAUSE_RESULT Pause::Update()
{
	// 마우스 좌표 가져오기
	POINT ptMouse = { 0, 0 };
	GetCursorPos(&ptMouse);
	ScreenToClient(dv_font.d3dpp.hDeviceWindow, &ptMouse);
	// =========================================================
	// 1. 슬라이더 (볼륨 조절) 로직
	// =========================================================
	// Render 좌표와 동일하게 설정
	int barX = 640-183;
	int barY = 300-100;
	int barWidth = 367; // 슬라이더 이미지 너비
	int barHeight = 200; // 슬라이더 이미지 높이

	// 슬라이더는 '누르고 있는 동안(Press)' 계속 동작해야 함
	if (KeyDown(VK_LBUTTON))
	{
		// 슬라이더 바 영역 체크
		RECT rtCheck = { barX, barY, barX + barWidth, barY + barHeight };

		// 이미 잡고 있거나(Drag), 새로 영역을 클릭했으면
		if (PtInRect(&rtCheck, ptMouse) || m_bDragSlider)
		{
			m_bDragSlider = true;

			// 비율 계산 (0.0 ~ 1.0)
			float ratio = (float)(ptMouse.x - barX) / (float)barWidth;
			if (ratio < 0.0f) ratio = 0.0f;
			if (ratio > 1.0f) ratio = 1.0f;

			m_fVolumeRatio = ratio;
			SOUND->SetMasterVolume(m_fVolumeRatio);
		}
	}
	else
	{
		m_bDragSlider = false; // 마우스를 떼면 드래그 해제
	}

	// =========================================================
	// 2. 버튼 클릭 로직
	// =========================================================
	if (KeyDown(VK_LBUTTON))
	{
		// Continue 버튼 (640, 400) - 크기 183x100
		RECT rtCont = { 640-91, 350, 640 - 91 + 183, 350 + 100 };
		if (PtInRect(&rtCont, ptMouse))
		{
			return PAUSE_RESUME; // "게임 재개"
		}

		// Menu 버튼 (640, 500) - 크기 183x100
		RECT rtMenu = { 640 - 91, 450, 640 - 91 + 183, 450 + 100 };
		if (PtInRect(&rtMenu, ptMouse))
		{
			return PAUSE_TO_MENU; // "메뉴로 이동"
		}
	}

	return PAUSE_KEEP; // 아무것도 안 눌렀으면 계속 일시정지
}

void Pause::Render()
{
	// 1. 반투명 배경 (화면 전체)
	m_PauseBg.Render(SCREEN_WITH/2, SCREEN_HEIGHT/2, 0, 2, 2, 1);
	m_PauseBg2.Render(SCREEN_WITH / 2, SCREEN_HEIGHT / 2, 0, 1, 1, 1);

	char volume[256];
	sprintf_s(volume, "Volume");
	dv_font.DrawString(volume, 330, 280, D3DCOLOR_ARGB(255, 255, 255, 255));

	// 2. 슬라이더
	int barX = 640; int barY = 300;
	m_SliderBar.Render(barX, barY, 0, 1.4, 1, 1);

	float barLeftX = 640.0f - (367.0f / 2.0f);
	float knobX = barLeftX + (367.0f * m_fVolumeRatio);
	m_SliderKnob.Render(knobX - 10, barY - 5, 0, 1, 1, 1);

	// 3. 버튼
	m_BtnContinue.Render(640, 400, 0, 1, 1, 1);
	m_BtnMenu.Render(640, 500, 0, 1, 1, 1);
}