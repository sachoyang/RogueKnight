#include "Include.h"

TimeManager::TimeManager() {
    Init();
}

TimeManager::~TimeManager() {}

void TimeManager::Init() 
{
    m_GameTime = 0;
    m_PrevRealTime = GetTickCount();
    ResetTime();
}

void TimeManager::ResetTime() {
    m_HitStopDuration = 0;
    m_GlobalTimeScale = 1.0f;
    m_TimeAccumulator = 0.0f;
    m_TimeSlowDuration = 0;
}

void TimeManager::SetHitStop(DWORD duration) {
    // 이미 더 긴 역경직이 진행 중이라면 짧은 역경직으로 덮어씌우지 않음!
    if (m_HitStopDuration > 0) {
        DWORD elapsed = GetTickCount() - m_HitStopTime;
        if (elapsed < m_HitStopDuration) {
            DWORD remain = m_HitStopDuration - elapsed;
            if (duration <= remain) return; // 기존 남은 시간이 더 길면 무시
        }
    }

    m_HitStopTime = GetTickCount();
    m_HitStopDuration = duration;
}

void TimeManager::SetTimeSlow(float scale, DWORD duration) {
    // 이미 동일하거나 더 강한 슬로우가 걸려있다면 타이머 리셋 무시!
    // (이 조건문 덕분에 매 프레임 호출되어도 타이머가 정상적으로 흘러갑니다)
    if (m_GlobalTimeScale <= scale && m_GlobalTimeScale != 1.0f) {
        return;
    }

    m_GlobalTimeScale = scale;
    m_TimeSlowStartTime = GetTickCount();
    m_TimeSlowDuration = duration;
}

bool TimeManager::IsTimeStopped() {
    if (m_HitStopDuration > 0) return true;

    m_TimeAccumulator += m_GlobalTimeScale;
    if (m_TimeAccumulator < 1.0f) return true;
    m_TimeAccumulator -= 1.0f;

    return false;
}
void TimeManager::UpdateTime() {
    DWORD curTime = GetTickCount();
    DWORD delta = curTime - m_PrevRealTime;
    m_PrevRealTime = curTime;

    // 역경직 중이면 게임 시간 정지!
    if (m_HitStopDuration > 0) {
        if (curTime - m_HitStopTime < m_HitStopDuration) return; // 시간 안 흘러감!
        else m_HitStopDuration = 0;
    }

    // 슬로우 타이머 체크
    if (m_GlobalTimeScale != 1.0f) {
        if (curTime - m_TimeSlowStartTime > m_TimeSlowDuration) m_GlobalTimeScale = 1.0f;
    }

    // 슬로우 배율(0.3배 등)이 곱해진 만큼만 게임 시간이 조심스럽게 흘러감!
    m_GameTime += (DWORD)(delta * m_GlobalTimeScale);
}