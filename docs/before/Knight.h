#pragma once
#include "Include.h"

class Knight
{
public:
	Sprite Knightimg[25];

	/*Sprite hitEffect[2];
	Sprite unhitEffect[2];*/

	float gravity;
	bool grounded;

public:
	Knight();
	~Knight();

	int hp;
	const int maxHp = 8;

	D3DXIMAGE_INFO imagesinfo;
	RECT m_rc;
	D3DXVECTOR2 pos;

	DWORD m_KnightAniTime;
	int m_KnightCount;
public:
	int dir;
	bool isMove;
	bool isLookup, isLookdown;

	// 사망 상태 및 시간 기록용 변수
	bool isDead;
	DWORD deadTime;
	DWORD realDeadTime;  // 🌟 [추가] 씬 전환용 현실 사망 시간 (RealTime)

	bool isDashing;         // 현재 대시 중인가?
	bool canAirDash;        // 공중 대시 가능한가? (점프 후 1회 제한)

	DWORD dashStartTime;    // 대시 시작 시간 (지속시간 체크용)
	DWORD dashCooldownTime; // 대시 쿨타임 체크용

	float dashSpeed;        // 대시 속도

	int attackType;			// 0 측면, 1 위, 2 아래
	bool isAttacking;       // 현재 공격 중인가?
	bool isAttackHit;

	DWORD attackStartTime;  // 공격 시작 시간 (애니메이션 및 판정 지속시간 체크용)
	DWORD lastAttackTime; // 공격 쿨타임 체크용
	RECT attackBox;         // 공격 판정 범위 (칼의 히트박스)

	bool isInvincible;       // 무적 상태
	DWORD invincibleTime;    // 무적 시작 시간

	bool isKnockback;        // 기사 넉백 상태
	D3DXVECTOR2 knockVelocity; // 기사 밀려나는 속도

	bool isCutscene;          // 컷씬 중인지 여부 (컷씬에서는 모든 행동이 제한됨)
	Sprite bossStartImg[2];
	int m_StartAniCount;
	DWORD m_StartAniTime;

public:
	void Init();
	void Update();
	void Draw();

	void KnightMove(); // 이동 관련 함수 - 위치 콜라이더 갱신용
	// 점프 관련 함수
	void JumpStart(); // Z키 누를 때 (점프 시작)
	void JumpCut();   // Z키 뗄 때 (속도 줄이기)
	// 대시 시작 함수
	void DashStart();
	// 공격 시작 함수 (X키 누를 때)
	void AttackStart();
	// 데미지 처리 함수
	// void TakeDamage(int damage, int hitDir);
	bool TakeDamage(int damage, int hitDir);
};
extern Knight knight;