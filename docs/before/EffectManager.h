#pragma once
#include "Include.h"
#include <vector>

// 🌟 사용할 이펙트 종류들을 정의합니다. 나중에 기획이 추가되면 여기에 이름만 추가하면 됩니다!
enum EffectType {
    EF_HIT,             // 측면 타격 (단일 이미지)
    EF_UNHIT,           // 측면 빗맞음 (단일 이미지)
    EF_HIT_UPDOWN,      // 상하 타격
    EF_UNHIT_UPDOWN,    // 상하 빗맞음
    EF_STUN,       // 피격 스턴
    EF_SPLASH,       // 타격 스플래시
    EF_DASH_DUST,       // 대시 잔상/먼지
    EF_ROAR             // 보스 포효 충격파 (단일 이미지 오래 유지)
};

// 개별 이펙트가 화면에 띄워질 때 가질 정보들
struct EffectObj {
    int type;
    float x, y;
    int dir;
    float scaleY; // 상하 공격 반전용

    DWORD spawnTime;
    DWORD lastFrameTime;
    int currentFrame;
    int maxFrame;

    bool isAnimation;
    DWORD duration; // 단일 이미지일 경우 화면에 남는 시간 (ms)
    bool isDead;
};

class EffectManager {
private:
    EffectManager();
    ~EffectManager();

    std::list<EffectObj> m_Effects;

    // 이펙트별 이미지 배열 (애니메이션이면 여러 장, 단일이면 1장)
    std::vector<Sprite> m_SprHit, m_SprUnhit, m_SprHitUD, m_SprUnhitUD;
    std::vector<Sprite> m_SprDashDust, m_SprRoar;
    std::vector<Sprite> m_SprStun;
    std::vector<Sprite> m_SprSplash;

public:
    static EffectManager* GetInstance() {
        static EffectManager instance;
        return &instance;
    }

    void Init();
    void Update();
    void Draw();

    // 단 한 줄로 이펙트를 소환하는 마법의 함수!
    void Play(int type, float x, float y, int dir = 1, float scaleY = 1.0f);
};

#define EFFECT EffectManager::GetInstance()