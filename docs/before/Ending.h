#pragma once
#include "Include.h"

#define ID_BTN_THANKS 2001
class Ending : public Chap
{
public:
	Ending();
	~Ending();

	Sprite m_Bg;          // 클리어 화면 이미지
	RECT   m_rtThanks;    // Special Thanks 버튼 영역
	HWND   m_hBtn;   // 버튼 핸들
	bool   m_bShow;  // 버튼이 현재 보여지고 있나?

	virtual void Init();
	virtual void Update(double frame);
	virtual void Draw();
	virtual void OnMessage(MSG* msg);
};