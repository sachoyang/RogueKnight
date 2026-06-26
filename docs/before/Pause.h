#pragma once
#include "Include.h"

// Pause::Update의 결과값 (Game 씬에게 알려줄 신호)
enum PAUSE_RESULT {
	PAUSE_KEEP,     // 계속 일시정지 상태 유지
	PAUSE_RESUME,   // "Continue" 눌렀음 -> 게임 재개해줘
	PAUSE_TO_MENU   // "Menu" 눌렀음 -> 메뉴 씬으로 보내줘
};

class Pause
{
public:
	Pause();
	~Pause();

	void Init();
	PAUSE_RESULT Update(); // 반환값으로 상태 전달
	void Render();

private:
	// UI 리소스
	Sprite  m_PauseBg;      // 반투명 배경
	Sprite  m_PauseBg2;      // 반투명 배경
	Sprite  m_BtnContinue;  // 계속하기 버튼
	Sprite  m_BtnMenu;      // 메뉴 버튼

	// 볼륨 슬라이더
	Sprite  m_SliderBar;
	Sprite  m_SliderKnob;

	float   m_fVolumeRatio; // 0.0 ~ 1.0
	bool    m_bDragSlider;  // 슬라이더 잡고 있는지 여부
};