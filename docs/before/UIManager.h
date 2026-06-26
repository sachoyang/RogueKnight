#pragma once
#include "Include.h"

class UIManager
{
public:
    UIManager();
    ~UIManager();

    bool m_bLargeMap; // M키를 눌렀을 때 맵 확대 및 일시정지 여부

    // 체력 UI 이미지
    Sprite heartFull;
    Sprite heartEmpty;
    Sprite heartBreak[3]; // 깨지는 프레임 3장

    int lastHp;             // 체력이 깎였는지 확인하는 용도
    int breakingHeartIndex; // 현재 부서지고 있는 하트 번호 (-1이면 없음)
    DWORD breakStartTime;   // 부서지기 시작한 시간

    void Init();
    void Update();
    void Draw();

    void DrawMinimap();
};

extern UIManager uiMng;