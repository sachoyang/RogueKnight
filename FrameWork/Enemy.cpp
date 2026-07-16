#include "Include.h"

// ==========================================
// [부모] Enemy 공통 기능
// ==========================================
void Enemy::TakeDamage(int damage, int hitDir)
{
    if (isDead) return;

    hp -= damage;
    isHit = true;
    hitStartTime = GetTickCount();

    if (hp <= 0) 
    {
        hp = 0;
        isDead = true;
        aniCount = 0; // 시체 프레임 시작
        gravity = -5.0f; // 죽을 때도 살짝 위로 튀며 사망
        // 🌟 [핵심] 죽는 순간, 반복 재생되던 몬스터 소리를 끕니다!
        StopMonsterSound();
        SOUND->PlayEffect(SND_EFF_MONSTER_DIE);


    }
    else 
    {
        // 타격감 2번: 통통 튀어오르는 넉백!
        velocity.x = hitDir * 5.0f; // 맞은 방향으로 밀려남
        gravity = -7.0f;            // 위로 튀어오름
    }
}

void Enemy::StopMonsterSound() {
    if (m_iSoundChannel != -1) {
        SOUND->StopSound(m_iSoundChannel);
        m_iSoundChannel = -1;
    }
}

// ==========================================
// [부모] A* 추격 AI 공통 처리
//  - 플레이어와의 거리로 추격/순찰을 판정하고,
//    추격 중이면 일정 주기(400ms)마다 A* 경로를 다시 계산한다.
// ==========================================
void Enemy::UpdateAI()
{
    // 죽었거나 넉백 중이면 AI 정지
    if (isDead)
    {
        aiState = AI_PATROL;
        m_path.clear();
        m_pathIndex = 0;
        return;
    }

    float dx = knight.pos.x - pos.x;
    float dy = knight.pos.y - pos.y;
    float dist = sqrtf(dx * dx + dy * dy);

    // 탐지 범위 안 & 기사 생존 시 추격
    if (!knight.isDead && dist <= detectRange)
    {
        aiState = AI_CHASE;

        DWORD now = GetTickCount();
        // 매 프레임 A*는 부담 → 주기적으로만 다시 계산 (경로가 비었으면 즉시)
        if (m_path.empty() || (now - m_lastRepathTime) > 400)
        {
            std::vector<D3DXVECTOR2> newPath;
            if (navGrid.FindPath(pos.x, pos.y, knight.pos.x, knight.pos.y, newPath))
            {
                m_path = newPath;
                m_pathIndex = 0;
            }
            m_lastRepathTime = now;
        }
    }
    else
    {
        // 범위 밖 → 원래 반복 행동으로 복귀
        aiState = AI_PATROL;
        m_path.clear();
        m_pathIndex = 0;
    }
}

// 현재 향할 waypoint를 고른다(도달한 것은 건너뜀). 없으면 false.
static bool PickWaypoint(std::vector<D3DXVECTOR2>& path, int& idx,
                         const D3DXVECTOR2& pos, D3DXVECTOR2& outWp)
{
    while (idx < (int)path.size())
    {
        D3DXVECTOR2 t = path[idx];
        float ddx = t.x - pos.x;
        float ddy = t.y - pos.y;
        // 충분히 가까우면 다음 waypoint로
        if (fabsf(ddx) < 24.0f && fabsf(ddy) < 48.0f) { idx++; continue; }
        outWp = t;
        return true;
    }
    return false;
}

void Enemy::Draw()
{
    D3DCOLOR color = 0xFFFFFFFF;
    if (isHit && !isDead) {
        if ((GetTickCount() - hitStartTime) < 200) color = D3DCOLOR_ARGB(255, 255, 50, 50);
        else isHit = false;
    }

    float renderX = pos.x - CAM->GetX();
    float renderY = pos.y - CAM->GetY();

    if (isDead) 
    {
        // [핵심 수정] 살아있을 때와 죽었을 때의 이미지 높이 차이를 계산해서,
        // 시체의 발바닥(바닥)이 살아있을 때의 발바닥 위치와 정확히 일치하도록 아래로 내려줍니다!
        float liveHeight = img[0].imagesinfo.Height;
        float deadHeight = deadImg[aniCount].imagesinfo.Height;
        float yOffset = (liveHeight - deadHeight) / 2.0f; // 높이 차이의 절반만큼 내림

        deadImg[aniCount].SetColor((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);
        deadImg[aniCount].Render(renderX, renderY + yOffset, 0, dir, 1, 1);
    }
    else 
    {
        img[aniCount].SetColor((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);
        img[aniCount].Render(renderX, renderY, 0, dir, 1, 1);
    }

    // 디버그 박스
    if (Gmanager.m_GameStart == true) 
    {
         coll.BoxSow(m_rc, 0, 0, D3DCOLOR_ARGB(255, 255, 0, 255)); 
    }
}

// ==========================================
// [자식 1] 지상 몹 (Ground Enemy)
// ==========================================
void GroundEnemy::Init(float x, float y)
{
    pos.x = x; pos.y = y; hp = 3;
    char FileName[256];
    for (int i = 0; i < 3; i++) { sprintf_s(FileName, "./resource/Img/monster/ground%02d.png", i + 1); img[i].Create(FileName, false, 0); }
    for (int i = 0; i < 2; i++) { sprintf_s(FileName, "./resource/Img/monster/ground_dead%02d.png", i + 1); deadImg[i].Create(FileName, false, 0); }
    SetRect(&m_rc, pos.x - 30, pos.y - 40, pos.x + 30, pos.y + 40);
    
    m_iSoundChannel = -1;
}

void GroundEnemy::Update()
{
    if (GetTickCount() - aniTime > 150) {
        aniCount++;
        if (isDead) { if (aniCount > 1) aniCount = 1; }
        else { if (aniCount > 2) aniCount = 0; }
        aniTime = GetTickCount();
    }

    // ======================================================== =
    // 🔊 [스마트 사운드 로직] 죽으면 끄고, 살아있으면 켠다!
    // =========================================================
    if (isDead)
    {
        // 죽었는데 소리가 나고 있다면? -> 끈다!
        if (m_iSoundChannel != -1) {
            SOUND->StopSound(m_iSoundChannel);
            m_iSoundChannel = -1;
        }
    }
    else
    {
        // 살아있는데 소리가 끊겼다면? (피격 효과음에 밀렸거나, 맵 이동 후 복귀 등)
        if (m_iSoundChannel == -1 || !SOUND->IsPlaying(m_iSoundChannel))
        {
            m_iSoundChannel = SOUND->PlayEffect(SND_EFT_CRAWLER);
        }
    }

    // [A*] 플레이어 탐지 & 추격 경로 갱신
    UpdateAI();

    // 허공 정점(gravity=0)에서 얼지 않도록, '바닥에 닿아 속도가 0.0f가 된 순간'만 동결!
    bool isRestingCorpse = (isDead && velocity.x == 0.0f);

    if (!isRestingCorpse) {
        pos.y += gravity;
        gravity += 0.5f; if (gravity > 10.0f) gravity = 10.0f;

        if (isHit || isDead) {
            pos.x += velocity.x;
            velocity.x *= 0.9f;
        }
        else if (aiState == AI_CHASE) {
            // ===== A* 추격 이동 =====
            D3DXVECTOR2 wp;
            bool haveWp = PickWaypoint(m_path, m_pathIndex, pos, wp);
            // waypoint가 없으면 기사 쪽으로 직진
            float tx = haveWp ? wp.x : knight.pos.x;

            // 스프라이트는 항상 플레이어를 바라봄 (dir -1 : 오른쪽, dir 1 : 왼쪽)
            dir = (knight.pos.x >= pos.x) ? -1 : 1;

            // 수평 이동은 데드존 밖일 때만 → 목표가 거의 정면이면 정지(좌우 진동 제거)
            float dxToWp = tx - pos.x;
            const float DEAD = 8.0f;
            if      (dxToWp >  DEAD) pos.x += speed;
            else if (dxToWp < -DEAD) pos.x -= speed;

            // 다음 목표가 위쪽이고 바닥에 붙어 있으면 점프!
            if (grounded && haveWp && wp.y < pos.y - 30.0f) {
                gravity = -12.0f; // 위로 튀어오름
                grounded = false;
            }
        }
        else {
            pos.x += (dir == -1) ? speed : -speed;
        }
    }

    RECT nextRc = m_rc;
    nextRc.top += 10; nextRc.bottom -= 10;
    nextRc.left += (dir == -1) ? 5 : -5; nextRc.right += (dir == -1) ? 5 : -5;

    RECT cliffRc = m_rc;
    cliffRc.left += (dir == -1) ? 20 : -20; cliffRc.right += (dir == -1) ? 20 : -20;
    cliffRc.top = m_rc.bottom + 5; cliffRc.bottom = m_rc.bottom + 30;

    bool hitWall = false, hitFloor = false;
    RECT temp;

    grounded = false; // 이번 프레임 바닥 접촉 여부 (아래에서 갱신)

    for (auto& w : coll.m_Walls) {
        if (!isDead && IntersectRect(&temp, &nextRc, &w)) hitWall = true;
        if (!isDead && IntersectRect(&temp, &cliffRc, &w)) hitFloor = true;

        if (IntersectRect(&temp, &m_rc, &w)) {
            if (gravity >= 0 && (m_rc.bottom - 20) <= w.top) {
                pos.y = w.top - 40.0f;
                gravity = 0;
                grounded = true; // 바닥에 붙음 → 점프 가능
                if (isDead) velocity.x = 0.0f; // 바닥에 닿는 순간 속도 0 -> 다음 프레임부터 완벽 동결!
            }
        }
    }

    // 순찰 중일 때만 벽/낭떠러지에서 방향 전환.
    // (추격 중에는 A* 목표를 향해 dir을 직접 정하므로 뒤집지 않는다)
    if (!isHit && !isDead && aiState != AI_CHASE) {
        if (hitWall || (!hitFloor && gravity == 0)) dir *= -1;
    }

    SetRect(&m_rc, pos.x - 30, pos.y - 40, pos.x + 30, pos.y + 40);
}

// ==========================================
// [자식 2] 비행 몹 (Fly Enemy)
// ==========================================
void FlyEnemy::Init(float x, float y)
{
    pos.x = x; pos.y = y; startPos = pos; hp = 3;
    spawnTime = GetTickCount();
    char FileName[256];
    for (int i = 0; i < 4; i++) { sprintf_s(FileName, "./resource/Img/monster/fly%02d.png", i + 1); img[i].Create(FileName, false, 0); }
    for (int i = 0; i < 2; i++) { sprintf_s(FileName, "./resource/Img/monster/fly_dead%02d.png", i + 1); deadImg[i].Create(FileName, false, 0); }
	m_iSoundChannel = -1;
}

void FlyEnemy::Update()
{
    if (GetTickCount() - aniTime > 100) {
        aniCount++;
        if (isDead) { if (aniCount > 1) aniCount = 1; }
        else { if (aniCount > 3) aniCount = 0; }
        aniTime = GetTickCount();
    }
    // =========================================================
    // 🔊 [스마트 사운드 로직]
    // =========================================================
    if (isDead)
    {
        if (m_iSoundChannel != -1) {
            SOUND->StopSound(m_iSoundChannel);
            m_iSoundChannel = -1;
        }
    }
    else
    {
        // 날아다니는 소리 유지
        if (m_iSoundChannel == -1 || !SOUND->IsPlaying(m_iSoundChannel))
        {
            m_iSoundChannel = SOUND->PlayEffect(SND_EFT_FLYER);
        }
    }
    // [A*] 플레이어 탐지 & 추격 경로 갱신
    UpdateAI();

    bool isRestingCorpse = (isDead && velocity.x == 0.0f);

    if (isDead || isHit) {
        if (!isRestingCorpse) {
            pos.y += gravity; gravity += 0.5f;
            pos.x += velocity.x; velocity.x *= 0.9f;

            if (!isDead) {
                // 넉백 당하는 동안 계속 흘러가는 시간에 맞춰 중심점(startPos)을 역산합니다!
                // 이렇게 하면 무적(isHit)이 끝났을 때 텔레포트하지 않고 현재 위치에서 궤도를 자연스럽게 이어갑니다.
                DWORD t = GetTickCount() - spawnTime;
                float speed = 0.0015f;
                startPos.x = pos.x - 250.0f * sin(t * speed);
                startPos.y = pos.y - 80.0f * sin(t * speed * 2.0f);
            }
        }

        RECT temp;
        for (auto& w : coll.m_Walls) {
            if (IntersectRect(&temp, &m_rc, &w)) {
                if (gravity >= 0 && (m_rc.bottom - 20) <= w.top) {
                    pos.y = w.top - 30.0f;
                    gravity = 0;
                    velocity.x = 0.0f; // 바닥 닿음 인증!
                }
            }
        }
    }
    else if (aiState == AI_CHASE) {
        // ===== A* 추격 (비행: x/y 모두 경로를 직접 추종) =====
        D3DXVECTOR2 wp;
        bool haveWp = PickWaypoint(m_path, m_pathIndex, pos, wp);
        D3DXVECTOR2 target = haveWp ? wp : knight.pos;

        float ddx = target.x - pos.x;
        float ddy = target.y - pos.y;
        float len = sqrtf(ddx * ddx + ddy * ddy);
        const float flySpeed = 4.5f;
        if (len > 0.1f) {
            pos.x += (ddx / len) * flySpeed;
            pos.y += (ddy / len) * flySpeed;
        }
        if (ddx > 0) dir = -1; else dir = 1;

        // 넉백 복귀 시 8자 궤도가 튀지 않도록 중심점을 현재 위치 기준으로 재설정
        DWORD t = GetTickCount() - spawnTime;
        float s = 0.0015f;
        startPos.x = pos.x - 250.0f * sin(t * s);
        startPos.y = pos.y - 80.0f * sin(t * s * 2.0f);
    }
    else {
        DWORD t = GetTickCount() - spawnTime;
        float speed = 0.0015f;
        float nextX = startPos.x + 250.0f * sin(t * speed);
        float nextY = startPos.y + 80.0f * sin(t * speed * 2.0f);

        if (nextX > pos.x) dir = -1; else dir = 1;
        pos.x = nextX; pos.y = nextY;
    }

    SetRect(&m_rc, pos.x - 30, pos.y - 30, pos.x + 30, pos.y + 30);
}