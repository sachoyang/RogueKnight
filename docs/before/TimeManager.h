#pragma once
#include "Include.h"

class TimeManager
{
private:
    TimeManager();  // 싱글톤을 위해 private 처리
    ~TimeManager();

    DWORD m_HitStopTime;
    DWORD m_HitStopDuration;

    float m_GlobalTimeScale;
    float m_TimeAccumulator;

    DWORD m_TimeSlowStartTime;
    DWORD m_TimeSlowDuration;

    // 인게임 절대 시간 변수
    DWORD m_GameTime;
    DWORD m_PrevRealTime;

public:
    // 싱글톤 인스턴스 반환
    static TimeManager* GetInstance() {
        static TimeManager instance;
        return &instance;
    }

    void Init();
    void ResetTime(); // 맵 이동 시 시간 강제 정상화용

    // 시간 갱신 및 가져오기 함수
    void UpdateTime();
    DWORD GetGameTime() { return m_GameTime; }

    // 외부에서 호출할 2개의 핵심 무기!
    void SetHitStop(DWORD duration);                      // 역경직
    void SetTimeSlow(float scale, DWORD duration);        // 타임 슬로우

    // 게임 루프가 매 프레임 물어볼 함수
    bool IsTimeStopped();
};

// 어디서든 TIMEMGR-> 함수() 형태로 쓸 수 있게 매크로 정의
#define TIMEMGR TimeManager::GetInstance()