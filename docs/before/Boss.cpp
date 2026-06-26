#include "Include.h"

BossEnemy::BossEnemy(int id)
{
    type = 3; // 보스 전용 타입
    bossID = id;
    hp = BOSS_HP;
    state = B_STATE_SLEEP; // 처음엔 무조건 잠들어 있음
}

BossEnemy::~BossEnemy() 
{
    // 보스가 사라질 때 혹시 구르는 소리가 나고 있다면 꺼줍니다.
    if (m_RollSoundChannel != -1) {
        SOUND->StopSound(m_RollSoundChannel);
    }
}

void BossEnemy::Init(float x, float y)
{
    // 1. 기본 위치 및 스펙
    pos.x = x;
    pos.y = y;
    hp = BOSS_HP;
    dir = 1; // 왼쪽을 바라봄

    // =======================================================
    // 우주로 텔레포트 방지! (모든 변수 강제 0 초기화)
    // =======================================================
    velocity.x = 0.0f;
    velocity.y = 0.0f;
    gravity = 0.0f;
    isDead = false;
    isHit = false;
    aniCount = 0;
    m_bSoundPlayed_Attack1 = false;
    m_bSoundPlayed_Attack2 = false;
    char FileName[256];

    // 0. 대기 이미지 로드
    for (int i = 0; i < 3; i++) {
        sprintf_s(FileName, "./resource/Img/boss/boss_stop%02d.png", i + 1);
        stopImg[i].Create(FileName, false, 0);
    }

    // 1. 수면 이미지 로드 (보스 ID에 따라 sleep01, 02, 03 배정)
    sprintf_s(FileName, "./resource/Img/boss/boss_sleep%02d.png", bossID);
    sleepImg.Create(FileName, false, 0);

    // 2. 걷기 이미지 로드 (walk01 ~ walk06)
    for (int i = 0; i < 6; i++) {
        sprintf_s(FileName, "./resource/Img/boss/boss_walk%02d.png", i + 1);
        walkImg[i].Create(FileName, false, 0);
    }

    // 3. 포효 이미지 로드 (roar01 ~ roar04)
    for (int i = 0; i < 4; i++) {
        sprintf_s(FileName, "./resource/Img/boss/boss_roar%02d.png", i + 1);
        roarImg[i].Create(FileName, false, 0);
    }
    
    // Swing (대못 공격 11장) 로드
    for (int i = 0; i < 11; i++) {
        sprintf_s(FileName, "./resource/Img/boss/swing%02d.png", i + 1);
        swingImg[i].Create(FileName, false, 0);
    }

    // Roll (구르기 7장) 로드
    for (int i = 0; i < 7; i++) {
        sprintf_s(FileName, "./resource/Img/boss/roll%02d.png", i + 1);
        rollImg[i].Create(FileName, false, 0);
    }

    for (int i = 0; i < 8; i++) {
        sprintf_s(FileName, "./resource/Img/boss/die%02d.png", i + 1); // 파일명 확인 (die01.png)
        dieImg[i].Create(FileName, false, 0);
    }
    m_RollSoundChannel = -1; // 채널 ID 초기화
    m_dwSoundTime = 0;
    // 히트박스 갱신
    BossSetRect();
}
void BossEnemy::TakeDamage(int damage, int hitDir)
{
    // 🌟 [수정] 이미 죽었거나, 시체 상태(DIE)면 데미지를 받지 않음
    if (isDead || state == B_STATE_DIE) return;
    if (state == B_STATE_SLEEP) return; // 자고 있을 땐 데미지를 입지 않음 (무적)

    hp -= damage;
    isHit = true;
    hitStartTime = GetTickCount();

    // 🌟 [핵심] 보스는 슈퍼아머! 일반 몹처럼 velocity.x나 gravity를 건드리지 않아서 넉백되지 않습니다.

    if (hp <= 0) {
        hp = 0;
        // isDead = true;
        // 사망 시 처리 (추후 2페이즈 호출 등)
        ChangeState(B_STATE_DIE);
    }
}

void BossEnemy::ChangeState(int newState)
{
    //// 구르기 상태에서 다른 상태로 바뀔 때 -> 소리를 끈다!
    //if (state == B_STATE_ROLL_DASH || state == B_STATE_ROLL_BOUNCE)
    //{
    //    // 다음 상태가 또 구르기라면 끄지 않고 유지할 수도 있음 (자연스러운 연결 위해)
    //    // 하지만 여기선 확실하게 끄고 다시 켜는 방식으로 구현 (끊김 방지하려면 조건 추가 가능)
    //    if (newState != B_STATE_ROLL_DASH && newState != B_STATE_ROLL_BOUNCE)
    //    {
    //        if (m_RollSoundChannel != -1) {
    //            SOUND->StopSound(m_RollSoundChannel);

    //            m_RollSoundChannel = -1; // 초기화
    //        }
    //    }
    //}

    // 1. 이전 상태 정리 (Clean Up)
    // 만약 이전에 구르기 상태였다면, 무조건 소리를 끈다.
    if (state == B_STATE_ROLL_DASH || state == B_STATE_ROLL_BOUNCE)
    {
        if (m_RollSoundChannel != -1)
        {
            SOUND->StopSound(m_RollSoundChannel);
            m_RollSoundChannel = -1; // 채널 초기화 필수!
        }
    }

    state = newState;
    stateStartTime = GetTickCount();
    aniCount = 0;
    aniTime = GetTickCount(); // 애니메이션 타이머 초기화
    m_bSoundPlayed_Attack1 = false;
    m_bSoundPlayed_Attack2 = false;

    if (newState == B_STATE_AWAKE_ROAR)
    {
        SOUND->PlayEffect(SND_EFF_BOSS_ROAR);
    }
    else if (newState == B_STATE_ROLL_DASH)
    {
        // 이미 소리가 나고 있지 않을 때만 재생 (중복 방지)
        if (m_RollSoundChannel == -1) {
            m_RollSoundChannel = SOUND->PlayEffect(SND_EFF_BOSS_ROLL);
        }
    }
	else if (newState == B_STATE_ROLL_BOUNCE)
    {
        if (m_RollSoundChannel == -1) {
            m_RollSoundChannel = SOUND->PlayEffect(SND_EFF_BOSS_ROLL_SKY);
        }
    }
    else if (newState == B_STATE_DIE)
    {
        SOUND->PlayEffect(SND_EFF_BOSS_DIE);
	}
}

bool BossEnemy::CanDealDamage()
{
    if (state == B_STATE_SLEEP || state == B_STATE_DIE || state == B_STATE_AWAKE_ROAR) {
        return false; // 자고 있으면 타격 판정 없음!
    }
    return true;
}
bool BossEnemy::IsTargetable()
{
    if (state == B_STATE_SLEEP || state == B_STATE_DIE) {
        return false; // 자고 있으면 타격 판정 아예 없음!
    }
    return true;
}
void BossEnemy::BossSetRect()
{
    SetRect(&m_rc, pos.x - 100, pos.y - 120, pos.x + 150, pos.y + 130);
}

void BossEnemy::Update()
{
    // 1. 거대한 몸집에 맞는 무거운 중력 적용
    pos.y += gravity;
    gravity += 0.8f;
    if (gravity > 15.0f) gravity = 15.0f;

    // 갱신
    BossSetRect();
    // 2. 바닥 충돌 (미끄러짐 및 지형 착지)
    pos.x += velocity.x;

    // 탱탱볼(바운스) 상태일 때는 공중에서 마찰력을 무시합니다!
    if (state != B_STATE_ROLL_BOUNCE) 
    {
        velocity.x *= 0.9f;
    }

    RECT temp;
    for (auto& w : coll.m_Walls) {
        if (IntersectRect(&temp, &m_rc, &w)) {
            // 발바닥 착지 (히트박스가 크기 때문에 120px 오프셋 필요)
            if (gravity >= 0 && (m_rc.bottom - 30.0f) <= w.top) {
                pos.y = w.top - 130.0f;
                gravity = 0;

                BossSetRect();
            }
        }
    }

    // 2. 애니메이션 프레임 제어
    DWORD elapsed = GetTickCount() - stateStartTime;
    float distToKnight = abs(knight.pos.x - pos.x);
    // 기사가 왼쪽에 있으면 1, 오른쪽에 있으면 -1 (보스가 바라보는 방향)
    int lookDir = (knight.pos.x < pos.x) ? 1 : -1;
    // 공격 박스 초기화 (공격 중일 때만 세팅됨)
    isAttacking = false;
    SetRect(&attackBox, 0, 0, 0, 0);

  //  if (state == B_STATE_AWAKE_ROAR) {
  //      // 🌟 2초(2000ms) 동안 4프레임 분배 (0.5초마다 변경)
		//SOUND->PlayEffect(SND_EFF_BOSS_ROAR); // 포효 시작할 때 효과음 재생
  //      if (elapsed < 500) aniCount = 0;       // 준비 동작 1
  //      else if (elapsed < 1000) aniCount = 1; // 준비 동작 2
  //      else if (elapsed < 1500) aniCount = 2; // 포효 1
  //      else aniCount = 3;                     // 포효 2
  //  }
  //  else if (state == B_STATE_WALK) {
  //      // 🌟 걷기 애니메이션 재생 (0.15초마다 프레임 넘김)
  //      if (GetTickCount() - aniTime > 150) {
  //          aniCount++;
  //          if (aniCount > 5) aniCount = 0; // 6프레임 반복
  //          aniTime = GetTickCount();
  //      }
  //  }

    // 히트박스 갱신
    BossSetRect();

    // 3.  상태 머신
    switch (state)
    {
    case B_STATE_SLEEP:
        break;

    case B_STATE_AWAKE_ROAR:
        if (elapsed < 500) aniCount = 0;
        else if (elapsed < 1000) aniCount = 1;
        else if (elapsed < 1500) aniCount = 2;
        else aniCount = 3;

        if (elapsed > 2000) {
            ChangeState(B_STATE_IDLE);
            knight.isCutscene = false;
        }

        if (elapsed == 500) { // 애니메이션이 포효를 시작하는 특정 프레임(시간)
            EFFECT->Play(EF_ROAR, pos.x, pos.y);
            CAM->Shake(20.0f, 1000); //보스가 포효하면 화면이 1초간 미친듯이 흔들림!
        }
        break;

    case B_STATE_IDLE:
        // 방향 전환
        dir = lookDir;

        // IDLE 커스텀 애니메이션 (3프레임 반복)
        if (GetTickCount() - aniTime > 300) { // 0.12초마다 그림 변경 (원하는 속도로 조절하세요)
            aniCount++;
            if (aniCount > 2) aniCount = 0; // 0, 1, 2, 3
            aniTime = GetTickCount();
        }

        if (elapsed > 1000) // 1초 대기 후 행동 결정
        {
            if (distToKnight <= 300.0f) {
                // 근접 시 확률: 60% 가로/세로베기, 20% 구르기 돌진, 20% 뒤로 구르기
                int randPattern = rand() % 100;

                if (randPattern < 60) ChangeState(B_STATE_MELEE);
                else if (randPattern < 80) ChangeState(B_STATE_ROLL_DASH); // 근접에서도 냅다 구르기!
                else ChangeState(B_STATE_ROLL_BACK);
            }
            else {
                // 🌟 멀다! -> 걷기(30%), 지상 구르기(40%), 탱탱볼 바운스(30%)
                int randPattern = rand() % 100;

                if (randPattern < 30) ChangeState(B_STATE_WALK);
                else if (randPattern < 70) ChangeState(B_STATE_ROLL_DASH);
                else {
                    ChangeState(B_STATE_ROLL_BOUNCE);
                    bounceCount = 0;
                    gravity = -25.0f;

                    // =======================================================
                    // [목표 타겟팅 계산] 기사 위치까지의 거리를 체공 시간(약 50프레임)으로 나눔
                    // =======================================================
                    float diffX = knight.pos.x - pos.x;
                    float targetV = diffX / 50.0f;

                    // 최소/최대 속도 제한 (최소 10 ~ 최대 25)
                    if (targetV >= 0 && targetV < 10.0f) targetV = 10.0f; // 제자리 점프 방지 (최소 우측)
                    if (targetV < 0 && targetV > -10.0f) targetV = -10.0f; // 제자리 점프 방지 (최소 좌측)
                    if (targetV > 25.0f) targetV = 25.0f;   // 빛의 속도로 날아가는 것 방지
                    if (targetV < -25.0f) targetV = -25.0f;

                    velocity.x = targetV; // 계산된 속도 적용!
                    dir = (velocity.x < 0) ? 1 : -1; // 날아가는 방향으로 이미지 반전
                }
            }
        }
        break;

    case B_STATE_WALK:
        dir = lookDir;
        // 느리게 걸어감
        velocity.x = (dir == 1) ? -1.5f : 1.5f;

        // 걷기 애니메이션 (0.15초)
        if (GetTickCount() - aniTime > 150) {
            aniCount = (aniCount + 1) % 6;
            aniTime = GetTickCount();
        }

        // 걷다가 가까워지면 즉시 공격!
        if (distToKnight <= 250.0f) ChangeState(B_STATE_MELEE);
        // 너무 오래 걸으면(3초) 다시 생각함
        else if (elapsed > 3000) ChangeState(B_STATE_IDLE);
        break;

    case B_STATE_MELEE:
        // 근접 2연타 (Swing01 ~ 11)
        // 0~5: 가로 베기 / 6~10: 세로 베기
        if (elapsed < 400) {
            // 준비 동작 (살짝 뒤로 물러남)
            aniCount = 0;
            velocity.x = (dir == 1) ? 2.0f : -2.0f; // 반대 방향으로 스멀스멀
        }
        else
        {
            
            if (GetTickCount() - aniTime > 100) { // 0.1초마다 프레임 넘김
                aniCount++;
                aniTime = GetTickCount();

                // 전진 이동 기믹 (특정 타격 프레임에서 앞으로 쿵! 이동)
                if (aniCount == 3 || aniCount == 8) {
                    velocity.x = (dir == 1) ? -15.0f : 15.0f;
                }
            }

            //// 타격 히트박스(attackBox) 활성화 구간 (이미지 크기에 맞게 거대하게!)
            //if ((aniCount >= 2 && aniCount <= 4) || (aniCount >= 7 && aniCount <= 9)) {
            //    isAttacking = true;
            //    if (dir == 1) { // 왼쪽 보고 칠 때
            //        SetRect(&attackBox, pos.x - 250, pos.y - 150, pos.x + 50, pos.y + 120);
            //    }
            //    else { // 오른쪽 보고 칠 때
            //        SetRect(&attackBox, pos.x - 50, pos.y - 150, pos.x + 250, pos.y + 120);
            //    }
            //}

            // 1. 가로 베기 구간 (옆으로 쫙 길게! 위아래는 얇게)
            if (aniCount >= 2 && aniCount <= 4) {
                if (!m_bSoundPlayed_Attack1) {
                    SOUND->PlayEffect(SND_EFF_BOSS_ATTACK1);
                    m_bSoundPlayed_Attack1 = true; // 소리 냈음 체크
                }
                isAttacking = true;
                if (dir == 1) { // 왼쪽 보고 칠 때 (X를 왼쪽으로 길게 -300)
                    SetRect(&attackBox, pos.x - 300, pos.y - 50, pos.x + 50, pos.y + 50);
                }
                else {        // 오른쪽 보고 칠 때 (X를 오른쪽으로 길게 +300)
                    SetRect(&attackBox, pos.x - 50, pos.y - 50, pos.x + 300, pos.y + 50);
                }
            }

            // 2. 세로 베기 구간 (위로 아주 높게! X 폭은 적당히)
            else if (aniCount >= 7 && aniCount <= 9) {
                // 🌟 [수정] 플래그 체크
                if (!m_bSoundPlayed_Attack2) {
                    SOUND->PlayEffect(SND_EFF_BOSS_ATTACK2);
                    m_bSoundPlayed_Attack2 = true;
                }
                isAttacking = true;
                if (dir == 1) { // 왼쪽 (Y를 위로 엄청 높게 -250)
                    SetRect(&attackBox, pos.x - 200, pos.y - 200, pos.x + 50, pos.y + 120);
                }
                else {        // 오른쪽
                    SetRect(&attackBox, pos.x - 50, pos.y - 200, pos.x + 200, pos.y + 120);
                }
            }

            if (aniCount > 10) ChangeState(B_STATE_IDLE); // 공격 끝나면 대기
        }
        break;

    case B_STATE_ROLL_DASH:
        //SOUND->PlayEffect(SND_EFF_BOSS_ROLL);

        // 지상 구르기 돌진 (Roll01 ~ Roll07)
        if (elapsed < 400) {
            // 준비 동작 (살짝 뒤로 물러남)
            aniCount = 0;
            velocity.x = (dir == 1) ? 2.0f : -2.0f; // 반대 방향으로 스멀스멀
        }
        else {

            // 본격적인 돌진 (무한 궤도 1~4 프레임 반복)
            velocity.x = (dir == 1) ? -15.0f : 15.0f; // 엄청난 속도로 돌진!

            if (GetTickCount() - aniTime > 80) {
                aniCount++;
                if (aniCount > 4 || aniCount < 1) aniCount = 1; // 1,2,3,4 무한 반복
                aniTime = GetTickCount();
            }

            // 벽에 부딪혔는가?
            bool hitWall = false;
            if (dir == 1 && pos.x <= 150.0f) hitWall = true; // 왼쪽 벽
            if (dir == -1 && pos.x >= SCREEN_WITH * 2 - 150.0f) hitWall = true; // 오른쪽 벽 (보스방 기준)

            // 기사에게 맞췄는가? (MapManager에서 보스가 기사 때릴 때 state를 강제로 IDLE로 바꾸게 처리할 수도 있지만, 우선 거리로 판별)
            //if (distToKnight < 100.0f) hitWall = true; // 기사와 쾅 부딪힘

            if (hitWall) {
                // 튕겨 나가면서 멈춤 애니메이션(5, 6) 재생 상태로 전환
                velocity.x = (dir == 1) ? 10.0f : -10.0f; // 반대로 튕김
                ChangeState(B_STATE_IDLE); // 일단 대기 상태로 빼서 멈춤 애니메이션은 나중에 다듬기
                return;
            }
        }
        // 🌟 [추가] 구르는 중인데 소리가 안 나면 다시 켠다! (끊김 방지)
        if (m_RollSoundChannel == -1 || !SOUND->IsPlaying(m_RollSoundChannel))
        {
            m_RollSoundChannel = SOUND->PlayEffect(SND_EFF_BOSS_ROLL);
        }
        break;

    case B_STATE_ROLL_BOUNCE:
        // =======================================================
        // 🌟 [핵심] 공중에 있는 동안 마찰력을 무시하고 좌우 속도를 강제 유지!
        // 이 숫자를 15.0f -> 20.0f 등으로 키우면 좌우로 엄청나게 길게 날아갑니다.
        // =======================================================
        //velocity.x = (dir == 1) ? -16.0f : 16.0f;

        // 1. 구르기 애니메이션 재생 (무한 루프)
        if (GetTickCount() - aniTime > 80) {
            aniCount++;
            if (aniCount > 4 || aniCount < 1) aniCount = 1;
            aniTime = GetTickCount();
        }

        // 2. 당구공처럼 벽에 튕기기!
        if (dir == 1 && pos.x <= 150.0f) {

            pos.x = 150.0f; // 벽에 파고들지 않게 꺼내줌
            velocity.x = -velocity.x; // 왼쪽 벽에 박으면 우측으로 튕김
            dir = -1;       // 방향을 꺾어주면 위의 velocity.x 강제 유지 코드 덕에 반대로 튕겨 날아감!
        }
        else if (dir == -1 && pos.x >= SCREEN_WITH * 2 - 150.0f) {
            pos.x = SCREEN_WITH * 2 - 150.0f;
            velocity.x = -velocity.x; // 오른쪽 벽에 박으면 좌측으로 튕김
            dir = 1;
        }

        // 3. 바닥에 닿았을 때의 처리 (착지)
        if (gravity == 0)
        {
            bounceCount++;

            if (bounceCount >= 3) {
                // 🌟 3번 통통 튀겼으면 멈추고 일어납니다.
                ChangeState(B_STATE_IDLE);
                velocity.x = 0;
                return;
            }
            else {
                // 🌟 바닥에 닿자마자 기사 위치를 스캔하여 궤도를 수정하며 다시 솟구침!
                gravity = -25.0f;

                float diffX = knight.pos.x - pos.x;
                float targetV = diffX / 50.0f;

                if (targetV >= 0 && targetV < 10.0f) targetV = 10.0f;
                if (targetV < 0 && targetV > -10.0f) targetV = -10.0f;
                if (targetV > 25.0f) targetV = 25.0f;
                if (targetV < -25.0f) targetV = -25.0f;

                velocity.x = targetV;
                dir = (velocity.x < 0) ? 1 : -1;
            }
        }
        if (m_RollSoundChannel == -1 || !SOUND->IsPlaying(m_RollSoundChannel))
        {
            m_RollSoundChannel = SOUND->PlayEffect(SND_EFF_BOSS_ROLL_SKY);
        }
        break;

    case B_STATE_ROLL_BACK: // 후퇴 (임시)
        ChangeState(B_STATE_IDLE);
        break;

    case B_STATE_DIE:
        velocity.x = 0; // 죽으면 이동 멈춤

        // 0.2초마다 프레임 넘김 (총 8프레임)
        if (GetTickCount() - aniTime > 200) {
            if (aniCount < 7) {
                aniCount++; // 7번(die08) 까지만 올라가고 멈춤!
            }
            aniTime = GetTickCount();
        }
        break;
    }
}

void BossEnemy::Draw()
{
    // 오렌지색 피격 깜빡임 연출
    D3DCOLOR color = 0xFFFFFFFF;

    // 1. 수면 상태: 배경에 묻혀있는 느낌을 주기 위해 아주 어두운 회색으로 렌더링합니다!
    if (state == B_STATE_SLEEP) 
    {
        color = D3DCOLOR_ARGB(255, 60, 60, 60);
    }
    else if (isHit && !isDead)
    {
        if ((GetTickCount() - hitStartTime) < 200) color = D3DCOLOR_ARGB(255, 255, 120, 0); // 주황색
        else isHit = false;
    }

    float renderX = pos.x - CAM->GetX();
    float renderY = pos.y - CAM->GetY();

    // 3배 거대화 렌더링!
    float scale = 1.0f;

    // 상태에 따라 출력할 이미지를 다르게 선택!
    Sprite* currentImg = &sleepImg; // 기본값

    // 상태에 맞춰 알맞은 이미지 배열을 가리킵니다.
    if (state == B_STATE_SLEEP) currentImg = &sleepImg;
    else if (state == B_STATE_AWAKE_ROAR) currentImg = &roarImg[aniCount];
    else if (state == B_STATE_WALK) currentImg = &walkImg[aniCount];
	else if (state == B_STATE_IDLE) currentImg = &stopImg[aniCount];
    else if (state == B_STATE_MELEE) currentImg = &swingImg[aniCount];
    else if (state == B_STATE_ROLL_DASH || state == B_STATE_ROLL_BOUNCE) 
    { 
        currentImg = &rollImg[aniCount]; 
        //if (GetTickCount() - m_dwSoundTime > 1500)
        //{
        //    SOUND->PlayEffect(SND_EFF_BOSS_ROLL); // Loop 아님, 1회 재생
        //    m_dwSoundTime = GetTickCount();
        //}
    }
    else if (state == B_STATE_DIE) currentImg = &dieImg[aniCount];

    currentImg->color = color;
    //currentImg->SetColor((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);
    currentImg->Render(renderX, renderY, 0, dir, scale, scale);

	//콜라이더 박스 그리기 (디버그용)
    if (Gmanager.m_GameStart && coll.isDebugDraw) {
        coll.BoxSow(m_rc, 0, 0, D3DCOLOR_ARGB(255, 255, 0, 0)); // 보스 몸통 (빨강)
        if (isAttacking) {
            coll.BoxSow(attackBox, 0, 0, D3DCOLOR_ARGB(255, 255, 255, 0)); // 보스 공격 범위 (노랑)
        }
    }
}