#include "Include.h"

Knight knight;


Knight::Knight()
{
 //   m_KnightAniTime = 0;
 //   m_KnightCount = 0;
 //   isMove = 0;
 //   isLookup = 0;
 //   isLookdown = 0;
 //   pos.x = 200;
 //   pos.y = 100;
 //   gravity = 5.75f;
 //   grounded = false;
 //   dir = -1;

	////대시 관련 초기화
 //   isDashing = false;
 //   canAirDash = true;
 //   dashStartTime = 0;
 //   dashCooldownTime = 0;
 //   dashSpeed = 25.0f; // 대시 속도 (걷기보다 훨씬 빠르게)
}

Knight::~Knight()
{
}

void Knight::Init()
{
    char FileName[256];

    for (int i = 0; i < 3; i++)
    {
        sprintf_s(FileName, "./resource/Img/knight1/walk%02d.png", i + 1);
        Knightimg[i].Create(FileName, false, D3DCOLOR_XRGB(0, 0, 0));
    }
    sprintf_s(FileName, "./resource/Img/knight1/stay01.png");
    Knightimg[3].Create(FileName, false, D3DCOLOR_XRGB(0, 0, 0));
    sprintf_s(FileName, "./resource/Img/knight1/Lookup01.png");
    Knightimg[4].Create(FileName, false, D3DCOLOR_XRGB(0, 0, 0));
    D3DXGetImageInfoFromFile(FileName, &imagesinfo);

    sprintf_s(FileName, "./resource/Img/knight1/Lookdown01.png");
    Knightimg[5].Create(FileName, false, D3DCOLOR_XRGB(0, 0, 0));

    // 6: 점프 준비, 7: 상승, 8: 꼭대기, 9: 하강, 10: 착지
    sprintf_s(FileName, "./resource/Img/knight1/jump01.png");
    Knightimg[6].Create(FileName, false, 0);

    sprintf_s(FileName, "./resource/Img/knight1/jump02.png");
    Knightimg[7].Create(FileName, false, 0);

    sprintf_s(FileName, "./resource/Img/knight1/jump03.png");
    Knightimg[8].Create(FileName, false, 0);

    sprintf_s(FileName, "./resource/Img/knight1/fall01.png");
    Knightimg[9].Create(FileName, false, 0);

    sprintf_s(FileName, "./resource/Img/knight1/fall02.png");
    Knightimg[10].Create(FileName, false, 0);

	// 대시 애니메이션 4프레임 (11~14)
    sprintf_s(FileName, "./resource/Img/knight1/dash01.png"); // 준비
    Knightimg[11].Create(FileName, false, 0);

    sprintf_s(FileName, "./resource/Img/knight1/dash02.png"); // 대시 중
    Knightimg[12].Create(FileName, false, 0);

    sprintf_s(FileName, "./resource/Img/knight1/dash03.png"); // 공중 느낌
    Knightimg[13].Create(FileName, false, 0);

    sprintf_s(FileName, "./resource/Img/knight1/dash04.png"); // 정지
    Knightimg[14].Create(FileName, false, 0);

	// 공격 애니메이션 3프레임 (15~17) + 상단 공격 3프레임 (18~20) + 하단 공격 3프레임 (21~23)
    sprintf_s(FileName, "./resource/Img/knight1/attack01.png"); // 공격 준비
    Knightimg[15].Create(FileName, false, 0);

    sprintf_s(FileName, "./resource/Img/knight1/attack02.png"); // 칼 휘두름 (히트박스 발생)
    Knightimg[16].Create(FileName, false, 0);

    sprintf_s(FileName, "./resource/Img/knight1/attack03.png"); // 공격 마무리
    Knightimg[17].Create(FileName, false, 0);

    sprintf_s(FileName, "./resource/Img/knight1/attackup01.png"); Knightimg[18].Create(FileName, false, 0);
    sprintf_s(FileName, "./resource/Img/knight1/attackup02.png"); Knightimg[19].Create(FileName, false, 0);
    sprintf_s(FileName, "./resource/Img/knight1/attackup03.png"); Knightimg[20].Create(FileName, false, 0);

    sprintf_s(FileName, "./resource/Img/knight1/attackdown01.png"); Knightimg[21].Create(FileName, false, 0);
    sprintf_s(FileName, "./resource/Img/knight1/attackdown02.png"); Knightimg[22].Create(FileName, false, 0);
    sprintf_s(FileName, "./resource/Img/knight1/attackdown03.png"); Knightimg[23].Create(FileName, false, 0);

    sprintf_s(FileName, "./resource/Img/knight1/bossstart01.png");
    bossStartImg[0].Create(FileName, false, 0);

    sprintf_s(FileName, "./resource/Img/knight1/bossstart02.png");
    bossStartImg[1].Create(FileName, false, 0);
   
    KnightMove();

    m_KnightAniTime = 0;
    m_KnightCount = 0;
    isMove = 0;
    isLookup = 0;
    isLookdown = 0;
    pos.x = 200;
    pos.y = 100;
    gravity = 5.75f;
    grounded = false;
    dir = -1;

    //대시 관련 초기화
    isDashing = false;
    canAirDash = true;
    dashStartTime = 0;
    dashCooldownTime = 0;
    dashSpeed = 25.0f; // 대시 속도 (걷기보다 훨씬 빠르게)

    hp = 8;
    isDead = false;
    deadTime = 0;
    isInvincible = false;
    isKnockback = false;
    isAttackHit = false;
    isAttacking = false;
    isCutscene = false;
    m_StartAniCount = 0;
    m_StartAniTime = 0;
    attackStartTime = 0;
    lastAttackTime = 0; // 쿨타임 초기화
    SetRect(&attackBox, 0, 0, 0, 0);
}


// Knight.cpp

void Knight::Update()
{
    // 🌟 [추가] 기사가 죽었을 때의 강제 상태 고정!
    if (isDead) 
    {
        isMove = false;
        isDashing = false;
        isAttacking = false;
        isLookup = false;
        isLookdown = false;

        // 아직 기사 죽는 전용 애니메이션이 없으므로, 
        // 5번(숙이기) 이미지를 써서 바닥에 쓰러진 것처럼 연출합니다.
        m_KnightCount = 5;

        // 이 상태에서도 중력과 바닥 충돌은 먹혀야 하므로 return 하지 않고 밑으로 흘려보냅니다.
    }
    else if (isCutscene) 
    {
        isMove = false;
        isDashing = false;
        isAttacking = false;
        // (이 상태에서 중력 연산과 충돌 처리만 정상적으로 진행되도록 둡니다)
    }
    else
    {

    }
    if (TIMEMGR->GetGameTime() - m_KnightAniTime > 10)
    {
        //컷신 중일 때 도전(도발) 애니메이션 재생
        if (isCutscene)
        {
            if (TIMEMGR->GetGameTime() - m_StartAniTime > 200) { // 0.2초 간격
                m_StartAniCount++;
                if (m_StartAniCount > 1) m_StartAniCount = 0;
                m_StartAniTime = TIMEMGR->GetGameTime();
            }
        }
        // 무적 시간 1.5초(1500ms) 해제 로직
        if (isInvincible && (TIMEMGR->GetGameTime() - invincibleTime > 1500)) {
            isInvincible = false;
        }
        // ========================================================
        // 0단계: 공격 상태 및 히트박스(AttackBox) 업데이트
        // ========================================================
        if (isAttacking)
        {
            DWORD attackTime = TIMEMGR->GetGameTime() - attackStartTime;

            // 공격 지속 시간 (예: 300ms = 0.3초 동안 공격)
            if (attackTime > 300)
            {
                isAttacking = false;
                SetRect(&attackBox, 0, 0, 0, 0); // 공격이 끝나면 히트박스 소멸
            }
            else
            {
                // [히트박스 위치 계산]
                if (attackType == 0) // 측면 공격
                {
                    if (dir == -1) { // 오른쪽
                        attackBox.left = pos.x + 10; attackBox.right = pos.x + 170;
                    }
                    else {         // 왼쪽 (dir == 1)
                        attackBox.left = pos.x - 170; attackBox.right = pos.x - 10;
                    }
                    attackBox.top = pos.y - 70; attackBox.bottom = pos.y + 30;
                }
                else if (attackType == 1) // 상단 공격
                {
                    attackBox.left = pos.x - 60; attackBox.right = pos.x + 60;
                    attackBox.top = pos.y - 170; attackBox.bottom = pos.y - 10; // 머리 위로 길게
                }
                else if (attackType == 2) // 하단 공격
                {
                    attackBox.left = pos.x - 60; attackBox.right = pos.x + 60;
                    attackBox.top = pos.y + 10; attackBox.bottom = pos.y + 170; // 발 밑으로 길게
                }

                // [포고 점프] 하단 찍기 중에 무언가(적/오브젝트)를 맞췄다면?
                if (attackType == 2 && isAttackHit)
                {
                    gravity = -13.0f; // 공중에서 위로 강하게 튕겨오름! (체공 시간 벌기)
                    //isAttackHit = false; // 연달아 무한 점프하지 않도록 판정 1회 소모
                }
            }
        }
        else
        {
            SetRect(&attackBox, 0, 0, 0, 0);
        }

        // ========================================================
        // 1단계: 수평(X) 이동 및 충돌 처리 (걷기 + 대시 + 넉백)
        // ========================================================
        float moveX = 0.0f;
		float knockMoveX = 0.0f; //넉백 이동량

        // [대시 중]
        if (isDashing)
        {
            if (TIMEMGR->GetGameTime() - dashStartTime > 200) // 대시 지속시간 끝
            {
                isDashing = false;
                gravity = 0;
            }
            else
            {
                moveX = -(dir * dashSpeed); // 대시 이동량 계산
            }
        }

        // [넉백 중 이동량 계산]
        if (isKnockback) {
            knockMoveX = knockVelocity.x;
            knockVelocity.x *= 0.85f;
            if (abs(knockVelocity.x) < 0.5f) { knockVelocity.x = 0; isKnockback = false; }
        }
        // [걷기 상태] (Key.cpp에서 이미 pos.x를 건드려서 왔으므로 변화량을 추적해야 함)
        // 하지만 지금 구조상 Key.cpp가 pos.x를 직접 바꾸므로, 
        // 여기서 "이동 후 위치"가 벽이라면 "이동 전"으로 되돌리는 방식을 씁니다.

        // 1-1. 일단 대시 이동 적용 (걷기는 Key.cpp에서 이미 적용됨)
        if (isDashing) pos.x += moveX;
		if (isKnockback) pos.x += knockMoveX; // 넉백 이동 적용
        // 1-2. 현재 위치로 박스 갱신
        KnightMove();

        // 1-3. [수평 충돌 검사] (바닥에 걸리는 것 방지 위해 위아래를 줄임!)
        RECT wallCheckRect = m_rc;
        wallCheckRect.top += 10;    // 머리 쪽 여유 (천장에 머리 박고 걸을 때 걸림 방지)
        wallCheckRect.bottom -= 10; // 발 쪽 여유 (바닥에 1px 박혀있을 때 걸림 방지)

        bool isHitWall = false;
        RECT tempRect;

        for (auto& wall : coll.m_Walls)
        {
            if (IntersectRect(&tempRect, &wallCheckRect, &wall))
            {
                isHitWall = true;
                break;
            }
        }

        // 1-4. 벽에 부딪혔다면? -> 튕겨내기!
        if (isHitWall)
        {
            if (isDashing)
            {
                // 대시 중 벽 충돌 -> 대시 종료 및 원상복구
                isDashing = false;
                pos.x -= moveX;
            }
            if (isKnockback) { // 넉백 중에 벽을 만났다면?
                pos.x -= knockMoveX; // 파고든 만큼 다시 빼냅니다!
                knockVelocity.x = 0; // 벽에 박았으니 밀려나는 힘 소멸
            }
            if(isMove)
            {
                // 걷기 중 벽 충돌 -> Key.cpp가 옮긴 걸 취소해야 함
                // Key.cpp에서 4.0f만큼 움직였다고 가정하고 반대로 밈
                // (정확히 하려면 prevX를 저장해야 하지만, 간단히 반대 방향으로 밈)
                
                if (dir == 1) pos.x += 4.0f; // 왼쪽 보고 있었으면 오른쪽으로 밈
                else pos.x -= 4.0f;          // 오른쪽 보고 있었으면 왼쪽으로 밈
            }

            // 위치 복구 후 박스 재갱신
            KnightMove();
        }


        // ========================================================
        // 2단계: 수직(Y) 이동 및 충돌 처리 (중력 + 점프)
        // ========================================================

        // 2-1. 중력 적용 (대시 중엔 무시)
        if (!isDashing)
        {
            if (!grounded) pos.y += gravity;
        }

        // 2-2. Y축 이동 후 박스 갱신
        KnightMove();

        // 2-3. [수직 충돌 검사]
        bool isLanded = false;

        if (!isDashing) // 대시 중엔 바닥 착지 안 함 (공중 부양)
        {
            for (auto& wall : coll.m_Walls)
            {
                if (IntersectRect(&tempRect, &m_rc, &wall))
                {
                    // 바닥 착지 판정 (내려오고 있고 + 발이 벽 윗면 근처)
                    if (gravity >= 0 && (m_rc.bottom - 30) < wall.top)
                    {
                        grounded = true;
                        canAirDash = true; // 공중 대시 리필
                        isLanded = true;
                        gravity = 0;

                        // [떨림 방지 +1.0f 적용]
                        pos.y = (float)wall.top - (imagesinfo.Height - 70) + 1.0f;

                        // 위치 보정 후 최종 박스 갱신
                        KnightMove();

                        break;
                    }
                    // 머리 찧기 판정 (올라가고 있고 + 머리 쪽이 부딪힘)
                    else if (gravity < 0 && (m_rc.top + 30) > wall.bottom)
                    {
                        gravity = 0; // 상승 힘 제거 (머리 쿵)

                        // 머리 위치 보정: 벽의 바닥(wall.bottom) 바로 아래로 내림
                        // m_rc.top = pos.y - 40 이므로, pos.y = wall.bottom + 40
                        pos.y = (float)wall.bottom + 40.0f;

                        // 박스 갱신
                        KnightMove();

                    }
                }
            }
        }

        if (!isLanded) grounded = false;

        // 2-4. 중력 가속
        if (!grounded && !isDashing)
        {
            gravity += 0.6f;
            if (gravity > 15.0f) gravity = 15.0f;
        }


        // ========================================================
        // 3단계: 애니메이션 처리
        // ========================================================
        if (TIMEMGR->GetGameTime() - m_KnightAniTime > 50)
        {
            // 공격 애니메이션이 최우선!
            if (isDead)
            {
                m_KnightCount = 5;
            }
            else if (isAttacking)
            {
                DWORD attackTime = TIMEMGR->GetGameTime() - attackStartTime;
                int frameOffset = 0;
                if (attackTime < 100)      frameOffset = 0;
                else if (attackTime < 200) frameOffset = 1;
                else                       frameOffset = 2;

                // 공격 타입에 따라 15, 18, 21번대 애니메이션을 각각 재생
                if (attackType == 0)      m_KnightCount = 15 + frameOffset;
                else if (attackType == 1) m_KnightCount = 18 + frameOffset;
                else if (attackType == 2) m_KnightCount = 21 + frameOffset;
            }
            else if (isDashing)
            {
                DWORD dashTime = TIMEMGR->GetGameTime() - dashStartTime;

                if (dashTime < 50)       m_KnightCount = 11;
                else if (dashTime < 100) m_KnightCount = 12;
                else if (dashTime < 150) m_KnightCount = 13;
                else                     m_KnightCount = 14;
            }
            else if (grounded)
            {
                if (isMove) { m_KnightCount++; if (m_KnightCount > 2) m_KnightCount = 0; }
                else if (isLookup) m_KnightCount = 4;
                else if (isLookdown) m_KnightCount = 5;
                else m_KnightCount = 3;
            }
            else
            {
                if (gravity < -12.0f) m_KnightCount = 6;
                else if (gravity < -4.0f) m_KnightCount = 7;
                else if (gravity < 4.0f) m_KnightCount = 8;
                else m_KnightCount = 9;
            }
            m_KnightAniTime = TIMEMGR->GetGameTime();
        }
    }
}

void Knight::Draw()
{
    if (Gmanager.m_GameStart == true)
    {
        // 🌟 [추가] 컷신 중이면 무조건 도전(bossstart) 애니메이션만 그림!
        if (isCutscene)
        {
            dir = -1;
            float drawingOffsetY = 20.0f; // 필요시 발바닥 높이 조절
            bossStartImg[m_StartAniCount].Render(pos.x - CAM->GetX(), pos.y - CAM->GetY() + drawingOffsetY, 0, dir, 1, 1);
        }
        // 🌟 무적 시간일 때 0.1초 단위로 깜빡이기 (짝수 틱에만 렌더링 무시)
        else if (isInvincible && ((TIMEMGR->GetGameTime() / 100) % 2 == 0)) {
            // 이 프레임은 기사를 그리지 않고 넘김 (깜빡깜빡)
        }
        else 
        {
            float drawingOffsetY = 0.0f;
            if (m_KnightCount == 5)
            {
                // 이 숫자를 조절해서 발을 땅에 맞춤 
                // 이미지가 납작한 만큼 더해줘야 함. (예: 10 ~ 30 사이)
                drawingOffsetY = 20.0f;
            }
            Knightimg[m_KnightCount].Render(pos.x - CAM->GetX(), pos.y - CAM->GetY() + drawingOffsetY, 0, dir, 1, 1);


            // =======================================================
            // 2. hit / unhit 상태에 따른 공격 이펙트 그리기
            // =======================================================

            //if (isAttacking)
            //{
            //    float renderX = pos.x - CAM->GetX();
            //    float renderY = pos.y - CAM->GetY();

            //    if (attackType == 0) // [측면 공격]
            //    {
            //        float effectOffsetX = 60.0f;
            //        float effectOffsetY = -30.0f;

            //        if (dir == -1) // 오른쪽 (반전 필요)
            //        {
            //            if (isAttackHit) hitEffect[0].Render(renderX + effectOffsetX, renderY + effectOffsetY, 0, -1, 1, 1);
            //            else             unhitEffect[0].Render(renderX + effectOffsetX, renderY + effectOffsetY, 0, -1, 1, 1);
            //        }
            //        else // 왼쪽 (dir == 1, 원본)
            //        {
            //            if (isAttackHit) hitEffect[0].Render(renderX - effectOffsetX, renderY + effectOffsetY, 0, 1, 1, 1);
            //            else             unhitEffect[0].Render(renderX - effectOffsetX, renderY + effectOffsetY, 0, 1, 1, 1);
            //        }
            //    }
            //    else // [상단 / 하단 공격] -> hitEffect[1] 사용
            //    {
            //        float effectOffsetX = 0.0f; // 몸의 중심에서 나감
            //        float effectOffsetY = (attackType == 1) ? -80.0f : 50.0f; // 위 공격은 마이너스, 아래는 플러스 좌표

            //        // 기본이 하단 이펙트이므로, 상단 공격(1)일 때는 Y축 크기를 -1로 주어 위아래 반전시킴!
            //        float scaleY = (attackType == 1) ? -1.0f : 1.0f;

            //        if (isAttackHit) hitEffect[1].Render(renderX + effectOffsetX, renderY + effectOffsetY, 0, dir, scaleY, 1);
            //        else             unhitEffect[1].Render(renderX + effectOffsetX, renderY + effectOffsetY, 0, dir, scaleY, 1);
            //    }
            //}

            //if (isAttacking)
            //{
            //    float effectOffsetX = -30.0f;
            //    float effectOffsetY = -10.0f;
            //    float renderX = pos.x - CAM->GetX();
            //    float renderY = pos.y - CAM->GetY() + effectOffsetY;
            //    if (dir == 1) // 왼쪽을 보고 쏠 때
            //    {
            //        if (isAttackHit) hitEffect[0].Render(renderX + effectOffsetX, renderY, 0, 1, 1, 1);
            //        else             unhitEffect[0].Render(renderX + effectOffsetX, renderY, 0, 1, 1, 1);
            //    }
            //    else // 오른쪽을 보고 쏠 때 (반전)
            //    {
            //        if (isAttackHit) hitEffect[0].Render(renderX - effectOffsetX, renderY, 0, -1, 1, 1);
            //        else             unhitEffect[0].Render(renderX - effectOffsetX, renderY, 0, -1, 1, 1);
            //    }
            //}

            if (grounded)
            {
                coll.BoxSow(m_rc, 0, -5, 0xffff0000);
            }
            else
            {
                coll.BoxSow(m_rc, 0, -5);
            }

            if (isAttacking)
            {
                coll.BoxSow(attackBox, 0, 0, D3DCOLOR_ARGB(255, 255, 255, 0)); // 노란색
            }
        }
    }
}

void Knight::KnightMove()
{
    m_rc.left = pos.x - 30;
    //m_rc.top = pos.y - 40;
    m_rc.right = pos.x + imagesinfo.Width - 40;
    m_rc.bottom = pos.y + imagesinfo.Height - 70;

    // =======================================================
    // 🌟 [추가] 하단 키(Lookdown)를 눌러서 머리를 숙였을 때 히트박스 축소!
    // =======================================================
    if (isLookdown && grounded)
    {
        // 보스의 가로베기(노란색 박스) 밑단은 바닥에서 약 70픽셀 떠 있습니다.
        // 기사의 머리(top)를 바닥(bottom)에서 50픽셀 위까지만 오도록 확 낮춰줍니다!
        m_rc.top = m_rc.bottom - 65;
    }
    else
    {
        // 평상시 원래 기사의 콜라이더 높이
        m_rc.top = pos.y - 40;
    }
}

// 점프 시작 함수 (Key.cpp에서 호출)
void Knight::JumpStart()
{
    if (grounded) // 땅에 있을 때만 점프 가능
    {
        // 최대 점프력 설정 (값이 클수록 높이 뜀)
        gravity = -20.0f;
        grounded = false;
        SOUND->PlayEffect(SND_EFF_JUMP);

    }
}

// 가변 점프 (키 뗐을 때 속도 자르기)
void Knight::JumpCut()
{
    // 공중에서 상승 중일 때(음수)만 속도를 깎음
    if (!grounded && gravity < -5.0f)
    {
        // 현재 상승 속도를 절반으로 줄여버림 -> 뚝 떨어지는 느낌 구현
        gravity *= 0.5f;
    }
}

// 대시 시작
void Knight::DashStart()
{
    DWORD curTime = TIMEMGR->GetGameTime();

    // 1. 쿨타임 체크 (0.5초 = 500ms)
    if (curTime - dashCooldownTime < 500) return;

    // 2. 공중 사용 제한 체크
    if (!grounded && !canAirDash) return;

    // 3. 대시
    isDashing = true;
    dashStartTime = curTime;
    dashCooldownTime = curTime;
    gravity = 0; // 중력 제거 (일직선 이동)
    SOUND->PlayEffect(SND_EFF_DASH);

    // =================================================================
    // [핵심 로직] 벽을 뚫지 않는 "최종 도착 지점" 계산
    // =================================================================

    float maxDashDist = 200.0f; // 최대 대시 거리
    float finalX = pos.x;       // 최종 이펙트가 생길 위치 (일단 내 위치로 시작)

    // 방향에 따른 목표 지점 설정 (dir: 1=왼쪽, -1=오른쪽)
    // 주의: Knight::Update의 moveX 로직(moveX = -(dir * dashSpeed))에 따르면
    // dir이 1이면 왼쪽(-), -1이면 오른쪽(+)으로 이동합니다.

    if (dir == 1) // [왼쪽]으로 대시
    {
        float targetX = pos.x - maxDashDist; // 원래 가고 싶은 곳
        finalX = targetX; // 일단 장애물 없다고 가정

        // 모든 벽을 검사해서 "내 왼쪽"에 있고 "가장 가까운" 벽을 찾음
        for (auto& wall : coll.m_Walls)
        {
            // Y축(높이)이 겹치는지 확인 (내 몸통 높이에 있는 벽인가?)
            if (m_rc.bottom > wall.top && m_rc.top < wall.bottom)
            {
                // 벽이 내 왼쪽에 있고(wall.right < m_rc.left)
                // 원래 목표보다 더 가깝다면(wall.right > targetX)
                if (wall.right <= m_rc.left && wall.right > targetX)
                {
                    finalX = (float)wall.right + 60.0f; // 벽 바로 오른쪽(앞)으로 위치 수정
                    // (targetX를 갱신해서 더 멀리 있는 벽은 무시하게 만듦)
                    targetX = finalX;
                }
            }
        }
    }
    else // [오른쪽]으로 대시 (dir == -1)
    {
        float targetX = pos.x + maxDashDist;
        finalX = targetX;

        for (auto& wall : coll.m_Walls)
        {
            // Y축 겹침 확인
            if (m_rc.bottom > wall.top && m_rc.top < wall.bottom)
            {
                // 벽이 내 오른쪽에 있고(wall.left > m_rc.right)
                // 원래 목표보다 가깝다면(wall.left < targetX)
                if (wall.left >= m_rc.right && wall.left < targetX)
                {
                    finalX = (float)wall.left - 60.0f; // 벽 바로 왼쪽(앞)
                    targetX = finalX;
                }
            }
        }
    }

    // 🌟 계산된 'finalX' 위치에 이펙트 생성!
    // (살짝 위치 보정을 위해 dir에 따라 offset 추가)
    EFFECT->Play(EF_DASH_DUST, finalX + (dir == 1 ? 40 : -40), pos.y, dir);

    // 공중에서 썼다면 기회 소진
    if (!grounded) canAirDash = false;
}

void Knight::AttackStart()
{
    if (isAttacking || isDashing) return;
    if (TIMEMGR->GetGameTime() - lastAttackTime < 500) return;

    isAttacking = true;
    isAttackHit = false;
    attackStartTime = TIMEMGR->GetGameTime(); // (이 부분은 이따 3단계에서 바꿀 겁니다!)
    lastAttackTime = TIMEMGR->GetGameTime();

    // 🌟 1. 키 입력 조합으로 공격 타입 결정 (여기서 타입이 완벽히 굳어집니다)
    if (!grounded && isLookdown) attackType = 2; // 하단
    else if (isLookup) attackType = 1;           // 상단
    else attackType = 0;                         // 측면
    SOUND->PlayEffect(SND_EFF_ATTACK);

    // =======================================================
    // 2. [버그 수정] 이펙트는 무조건 attackType을 기준으로 여기서 스폰!
    // (이외의 위치에 허공 스윙(Unhit) 이펙트를 띄우는 코드가 있다면 모두 지워주세요!)
    // =======================================================
    float eX = pos.x + (dir == 1 ? -60 : 60);
    if (attackType == 0)      EFFECT->Play(EF_UNHIT, eX, pos.y - 30, dir);
    else if (attackType == 1) EFFECT->Play(EF_UNHIT_UPDOWN, pos.x, pos.y - 80, dir, -1.0f);
    else if (attackType == 2) EFFECT->Play(EF_UNHIT_UPDOWN, pos.x, pos.y + 50, dir, 1.0f);
}

// 반환 타입 bool로 변경
bool Knight::TakeDamage(int damage, int hitDir)
{
    // 🌟 1. [수정] 'isDashing' 조건을 제거했습니다! (이제 대시 중에 맞으면 아픕니다)
    // 무적 상태거나 이미 죽었다면 데미지 무시 -> false 반환
    if (isInvincible || isDead) return false;

    hp -= damage;
    // 사망 처리
    if (hp <= 0) {
        hp = 0;
        isDead = true;

        m_KnightCount = 5;

        deadTime = TIMEMGR->GetGameTime();
        realDeadTime = GetTickCount();
        SOUND->PlayEffect(SND_EFF_HERO_DIE);

        isAttacking = false;
        isAttackHit = false;
        isDashing = false; // 죽을 때도 대시 캔슬
        SetRect(&attackBox, 0, 0, 0, 0);

        TIMEMGR->SetTimeSlow(0.2f, 2000);

        return true; // 죽었을 때도 '맞은 건 맞으니까' true 반환
    }
    SOUND->PlayEffect(SND_EFF_HIT);

    // 일반 피격 처리
    isInvincible = true;
    invincibleTime = TIMEMGR->GetGameTime();

    // 맞았으니 공격 취소
    isAttacking = false;
    isAttackHit = false;

    isDashing = false;

    SetRect(&attackBox, 0, 0, 0, 0);

    // 기사 넉백
    isKnockback = true;
    knockVelocity.x = hitDir * 8.0f;
    gravity = -8.0f;
    grounded = false;

    return true; // 🌟 데미지가 제대로 들어갔으니 true 반환!
}