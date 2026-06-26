#include "Include.h"

EffectManager::EffectManager() {}
EffectManager::~EffectManager() {}

void EffectManager::Init() {
    char FileName[256];
    Sprite temp;

    // 1. 기존 타격 이펙트 로드 (단일 이미지)
    sprintf_s(FileName, "./resource/Img/effect/hit.png");
    temp.Create(FileName, false, 0); m_SprHit.push_back(temp);

    sprintf_s(FileName, "./resource/Img/effect/unhit.png");
    temp.Create(FileName, false, 0); m_SprUnhit.push_back(temp);

    sprintf_s(FileName, "./resource/Img/effect/hitdown.png");
    temp.Create(FileName, false, 0); m_SprHitUD.push_back(temp);

    sprintf_s(FileName, "./resource/Img/effect/unhitdown.png");
    temp.Create(FileName, false, 0); m_SprUnhitUD.push_back(temp);

    // 2. 새로 추가할 이펙트들! (나중에 이미지가 생기면 여기에 위처럼 push_back 하시면 됩니다)
    // 예: 먼지 애니메이션 3장이라면 temp.Create() 3번 하고 m_SprJumpDust.push_back(temp) 3번 수행!
    // [보스 포효 충격파] - 1장짜리라고 가정
    // (경로는 실제 있는 곳으로 맞춰주세요!)
    sprintf_s(FileName, "./resource/Img/effect/roar_effect01.png");
    temp.Create(FileName, false, 0); m_SprRoar.push_back(temp);


    // =======================================================
    // 2. 애니메이션 이펙트 (먼지, 잔상 등 여러 장짜리)
    // =======================================================

    // [기사가 맞음]
    for (int i = 1; i <= 7; i++) {
        sprintf_s(FileName, "./resource/Img/effect/stun_impact_effect%02d.png", i);
        temp.Create(FileName, false, 0);
        m_SprStun.push_back(temp);
    }
    // [몬스터가 맞음]
    for (int i = 1; i <= 6; i++) {
        sprintf_s(FileName, "./resource/Img/effect/whitesplash_%02d.png", i);
        temp.Create(FileName, false, 0);
        m_SprSplash.push_back(temp);
    }

    // [대시 먼지(잔상)] - 예: dash_dust01.png ~ dash_dust03.png (3장)
    for (int i = 1; i <= 5; i++) {
        sprintf_s(FileName, "./resource/Img/effect/dash_effect_%02d.png", i-1);
        temp.Create(FileName, false, 0);
        m_SprDashDust.push_back(temp);
    }
}

void EffectManager::Play(int type, float x, float y, int dir, float scaleY) {
    EffectObj obj;
    obj.type = type;   obj.x = x;           obj.y = y;
    obj.dir = dir;     obj.scaleY = scaleY;
    obj.spawnTime = TIMEMGR->GetGameTime(); obj.lastFrameTime = TIMEMGR->GetGameTime();
    obj.currentFrame = 0; obj.isDead = false;

    // 🌟 이펙트 종류에 따른 애니메이션/유지시간 세팅
    if (type == EF_HIT || type == EF_UNHIT || type == EF_HIT_UPDOWN || type == EF_UNHIT_UPDOWN) {
        obj.isAnimation = false; obj.duration = 200; // 0.1초 띄우고 삭제
        obj.maxFrame = 1;
    }
    else if (type == EF_ROAR) {
        obj.isAnimation = false; obj.duration = 1000; // 포효는 1초 유지
        obj.maxFrame = 1;
    }
    else if (type == EF_DASH_DUST) {
        obj.isAnimation = true;
        obj.maxFrame = 5;
        obj.duration = 40; // 대시는 속도감이 생명이니 아주 빠르게 (0.04초 간격)
    }
    else if (type == EF_STUN) {
        obj.isAnimation = true;
        obj.maxFrame = 7;
        obj.duration = 50; // 0.05초 간격 재생
    }
    else if (type == EF_SPLASH) {
        obj.isAnimation = true;
        obj.maxFrame = 6;
        obj.duration = 40; // 타격감 극대화를 위해 빠르게 재생
    }
    m_Effects.push_back(obj);
}

void EffectManager::Update() {
    if (TIMEMGR->IsTimeStopped()) return; // 🌟 시간 정지 시 이펙트도 같이 멈춤!

    for (auto it = m_Effects.begin(); it != m_Effects.end(); ) {
        DWORD curTime = TIMEMGR->GetGameTime();

        if (it->isAnimation) {
            if (curTime - it->lastFrameTime > it->duration) {
                it->currentFrame++;
                it->lastFrameTime = curTime;
                if (it->currentFrame >= it->maxFrame) it->isDead = true; // 애니메이션 끝나면 삭제
            }
        }
        else {
            if (curTime - it->spawnTime > it->duration) it->isDead = true; // 시간 다 되면 삭제
        }

        if (it->isDead) it = m_Effects.erase(it);
        else ++it;
    }
}

void EffectManager::Draw() {
    for (auto& obj : m_Effects) {
        float drawX = obj.x - CAM->GetX();
        float drawY = obj.y - CAM->GetY();
        std::vector<Sprite>* sprList = nullptr;

        switch (obj.type) {
        case EF_HIT: sprList = &m_SprHit; break;
        case EF_UNHIT: sprList = &m_SprUnhit; break;
        case EF_HIT_UPDOWN: sprList = &m_SprHitUD; break;
        case EF_UNHIT_UPDOWN: sprList = &m_SprUnhitUD; break;
        case EF_STUN: sprList = &m_SprStun; break;
        case EF_SPLASH: sprList = &m_SprSplash; break;
        case EF_DASH_DUST: sprList = &m_SprDashDust; break;
        case EF_ROAR: sprList = &m_SprRoar; break;
        }

        // 이미지가 로드되어 있을 때만 그림 (에러 방지 안전장치)
        if (sprList && !sprList->empty() && obj.currentFrame < sprList->size()) {
            (*sprList)[obj.currentFrame].Render(drawX, drawY, 0, obj.dir, obj.scaleY, 1);
        }
    }
}