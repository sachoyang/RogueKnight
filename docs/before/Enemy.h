#pragma once
#include "Include.h"

// 부모 클래스 (모든 적들의 공통 기능)
class Enemy
{
public:
	int m_ID;          // 몬스터 고유 ID (필요하면 사용)
    bool isCorpseFixed;

    int m_iSoundChannel; // 현재 재생 중인 몬스터 소리 채널 (없으면 -1)
    void StopMonsterSound(); // 소리 끄기 함수

    int type;           // 1: 지상, 2: 비행
    D3DXVECTOR2 pos;    // 현재 위치
    RECT m_rc;          // 피격 박스 (몸체)
    int hp;             // 체력
    int dir;            // 바라보는 방향 (1: 왼쪽, -1: 오른쪽)
    bool isDead;        // 죽었는가?

    // 피격 및 넉백 관련
    bool isHit;
    DWORD hitStartTime;
    D3DXVECTOR2 velocity; // 넉백 속도
    float gravity;

    // 애니메이션
    Sprite img[5];      // 이동 애니메이션 (넉넉히 5개)
    Sprite deadImg[2];  // 시체 애니메이션
    int aniCount;
    DWORD aniTime;

    Enemy() : hp(3), isDead(false), isHit(false), dir(1), gravity(0), type(1), aniCount(0), aniTime(0), m_iSoundChannel(-1) {}    virtual ~Enemy() {}

    virtual void Init(float x, float y) = 0; // 자식들이 각자 구현할 초기화
    virtual void Update() = 0;               // 각자의 행동 패턴

    // 공통 렌더링 및 데미지 함수
    virtual void Draw();
    virtual void TakeDamage(int damage, int hitDir);

    virtual bool CanDealDamage() { return true; }
    virtual bool IsTargetable() { return true; }

    // 시체 상태로 강제 전환하는 함수 (맵 로드 시 사용)
    virtual void SetCorpse() {
        isDead = true;
        hp = 0;

        // 1. [ChangeState 대체] 애니메이션 직접 지정
        // deadImg는 2장(0, 1)이므로 마지막인 1번으로 고정
        aniCount = 1;
        StopMonsterSound();
        // 2. [증발 버그 해결] 물리 변수 강제 초기화!
        // 이 값이 쓰레기 값이면 isRestingCorpse가 false가 되어 저 멀리 날아갑니다.
        velocity = D3DXVECTOR2(0.0f, 0.0f);
        gravity = 0.0f;
    }
};

// 지상 몹 (1번)
class GroundEnemy : public Enemy
{
public:
    float speed;
    GroundEnemy() { type = 1; speed = 2.0f; }
    void Init(float x, float y) override;
    void Update() override;
};

// 비행 몹 (2번)
class FlyEnemy : public Enemy
{
public:
    D3DXVECTOR2 startPos; // 기준점 (8자 비행의 중심)
    DWORD spawnTime;
    FlyEnemy() { type = 2; }
    void Init(float x, float y) override;
    void Update() override;
};