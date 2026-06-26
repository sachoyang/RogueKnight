#pragma once
#include "Include.h"

#define BOSS_HP 10

// 보스가 가질 수 있는 행동 상태(State) 목록
enum BOSS_STATE {
    B_STATE_SLEEP,          // 배경에서 잠들어 있음
    B_STATE_AWAKE_ROAR,     // 깨어나서 포효함
    B_STATE_IDLE,           // 대기 (기사 관찰)
    B_STATE_WALK,           // 기사를 향해 뚜벅뚜벅 걷기
    B_STATE_MELEE,          // 근접 2연타 (가로 -> 세로)
    B_STATE_ROLL_DASH,      // 지상 구르기 돌진
    B_STATE_ROLL_BOUNCE,    // 탱탱볼 점프 돌진
    B_STATE_ROLL_BACK,       // 뒤로 구르기 (거리 벌리기)
    B_STATE_DIE,
};

class BossEnemy : public Enemy
{
public:
    int bossID; // 1번, 2번, 3번 보스 식별용 (누가 깨어날지 결정)
    int state;
    DWORD stateStartTime;

    int m_RollSoundChannel; // 채널 ID
    DWORD m_dwSoundTime; // 마지막 사운드 재생 시간 저장용
    // 추가 변수 (탱탱볼 바운스 카운트 등 추후 사용)
    int bounceCount;

    // 보스 전용 애니메이션 스프라이트
	Sprite sleepImg;
	Sprite walkImg[6]; // 걷기 애니메이션 (6프레임)
	Sprite roarImg[4]; // 포효 애니메이션 (4프레임)
    Sprite stopImg[3]; //대기 이미지

    // 공격 및 구르기 애니메이션 배열
    Sprite swingImg[11];   // 0~5(가로베기), 6~10(세로베기)
    Sprite rollImg[7];     // 0(준비), 1~4(구르기), 5~6(멈춤)
    Sprite dieImg[8];

    // 보스 전용 공격 판정 박스
    RECT attackBox;
    bool isAttacking;

    BossEnemy(int id);
    virtual ~BossEnemy();

    void Init(float x, float y) override;
    void Update() override;
    void Draw() override;
    void TakeDamage(int damage, int hitDir) override;
    bool m_bSoundPlayed_Attack1; // 가로 베기 소리 냈니?
    bool m_bSoundPlayed_Attack2; // 세로 베기 소리 냈니?
    // 상태를 안전하게 변경하는 헬퍼 함수
    void ChangeState(int newState);

    // 자고있을때 데미지 주지 않음.
    bool CanDealDamage() override;
    bool IsTargetable() override;

	void BossSetRect();
};