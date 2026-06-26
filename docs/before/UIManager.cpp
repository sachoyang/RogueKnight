#include "Include.h"

// ==========================================
// DirectX 그래픽 그리기 도우미 함수
// ==========================================
struct ColorVertex {
    float x, y, z, rhw;
    D3DCOLOR color;
};

// 속을 색상으로 꽉 채운 사각형 그리기
void DrawSolidRect(float x, float y, float w, float h, D3DCOLOR color) {
    ColorVertex vertices[] = {
        { x,     y,     0.0f, 1.0f, color },
        { x + w, y,     0.0f, 1.0f, color },
        { x,     y + h, 0.0f, 1.0f, color },
        { x + w, y + h, 0.0f, 1.0f, color }
    };
    dv_font.Device9->SetTexture(0, NULL);
    dv_font.Device9->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    dv_font.Device9->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(ColorVertex));
}

// 테두리 선만 있는 사각형 그리기
void DrawLineRect(float x, float y, float w, float h, D3DCOLOR color) {
    ColorVertex vertices[] = {
        { x,     y,     0.0f, 1.0f, color },
        { x + w, y,     0.0f, 1.0f, color },
        { x + w, y + h, 0.0f, 1.0f, color },
        { x,     y + h, 0.0f, 1.0f, color },
        { x,     y,     0.0f, 1.0f, color }
    };
    dv_font.Device9->SetTexture(0, NULL);
    dv_font.Device9->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    dv_font.Device9->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, vertices, sizeof(ColorVertex));
}

UIManager uiMng;

UIManager::UIManager()
{
    m_bLargeMap = false; // 기본은 우측 상단 미니맵 모드
    lastHp = 8;
    breakingHeartIndex = -1;
    breakStartTime = 0;
}

UIManager::~UIManager()
{
}

void UIManager::Init()
{
    char FileName[256];
    sprintf_s(FileName, "./resource/Img/ui/heart_full.png");
    heartFull.Create(FileName, false, 0);

    sprintf_s(FileName, "./resource/Img/ui/heart_empty.png");
    heartEmpty.Create(FileName, false, 0);
	m_bLargeMap = false; // 게임 시작 시에는 항상 미니맵 모드로 시작

    sprintf_s(FileName, "./resource/Img/UI/heart_break01.png"); heartBreak[0].Create(FileName, false, 0);
    sprintf_s(FileName, "./resource/Img/UI/heart_break02.png"); heartBreak[1].Create(FileName, false, 0);
    sprintf_s(FileName, "./resource/Img/UI/heart_break03.png"); heartBreak[2].Create(FileName, false, 0);

	lastHp = knight.maxHp; // 초기 체력 상태 저장
}

void UIManager::Update()
{
    // 나중에 체력 애니메이션 등이 생기면 여기서 처리
    if (knight.hp < lastHp) 
    {
        breakingHeartIndex = knight.hp; // 방금 깎인 바로 그 위치의 하트!
        breakStartTime = GetTickCount();
    }
    // (체력이 회복될 수도 있으니 동기화)
    lastHp = knight.hp;
}

void UIManager::Draw()
{
    if (!Gmanager.m_GameStart) return;

    // 1. 미니맵 그리기
    DrawMinimap();

    // 2. 향후 체력바, 스킬 아이콘 등은 여기에 추가
    // =========================================================
    // 🌟 2. 기사 체력(HP) 그리기
    // =========================================================
    float startX = 30.0f; // 화면 왼쪽 여백
    float startY = 30.0f; // 화면 위쪽 여백
    float spacingX = 50.0f; // 하트 간격 (이미지 크기에 맞게 조절하세요!)

    for (int i = 0; i < knight.maxHp; i++)
    {
        float drawX = startX + (i * spacingX);

        if (i < knight.hp) 
        {
            // 온전한 하트
            heartFull.Render(drawX, startY, 0, 1, 1);
        }
        else if (i == breakingHeartIndex) 
        {
            // 깨지는 중인 하트! (100ms마다 프레임 변경)
            DWORD t = GetTickCount() - breakStartTime;
            if (t < 100)      heartBreak[0].Render(drawX, startY, 0, 1, 1);
            else if (t < 200) heartBreak[1].Render(drawX, startY, 0, 1, 1);
            else if (t < 300) heartBreak[2].Render(drawX, startY, 0, 1, 1);
            else 
            {
                heartEmpty.Render(drawX, startY, 0, 1, 1); // 다 깨지면 빈 하트로
            }
        }
        else 
        {
            // 이미 다 깨져서 비어버린 하트
            heartEmpty.Render(drawX, startY, 0, 1, 1);
        }
    }
}

void UIManager::DrawMinimap()
{
    // 맵 데이터가 없으면 패스
    if (mapMng.m_pCurrentMapChunk == nullptr) return;

    int currentRoomID = mapMng.m_pCurrentMapChunk->id;

    // M키 토글 여부에 따라 지도의 크기와 위치 결정
    float cellSize = m_bLargeMap ? 60.0f : 20.0f; // 큰 맵은 60px, 우상단 미니맵은 20px
    float mapWidth = 6 * cellSize;
    float mapHeight = 6 * cellSize;

    // 미니맵은 우측 상단, 전체 맵은 화면 정중앙
    // (기획자님 코드에 SCREEN_WITH 로 철자가 되어있어 그대로 사용합니다!)
    float startX = m_bLargeMap ? (SCREEN_WITH - mapWidth) / 2.0f : SCREEN_WITH - mapWidth - 30.0f;
    float startY = m_bLargeMap ? (SCREEN_HEIGHT - mapHeight) / 2.0f : 30.0f;

    // 지도의 배경 (검은색 반투명 유리판 느낌 깔아주기)
    DrawSolidRect(startX - 10, startY - 10, mapWidth + 20, mapHeight + 20, D3DCOLOR_ARGB(150, 0, 0, 0));

    for (int y = 0; y < 6; y++) {
        for (int x = 0; x < 6; x++) {
            int roomID = mapMng.m_Grid[y][x];

            if (roomID > 0) { // 방이 존재하는 곳만 사각형 그리기
                float cellX = startX + (x * cellSize);
                float cellY = startY + (y * cellSize);
                float padding = m_bLargeMap ? 4.0f : 2.0f; // 칸과 칸 사이 여백

                // 1. 방의 속성에 따라 기본 색상 세팅
                D3DCOLOR fillColor = D3DCOLOR_ARGB(200, 100, 100, 100); // 안 가본 방 (어두운 회색)
                D3DCOLOR lineColor = D3DCOLOR_ARGB(255, 200, 200, 200); // 테두리 (밝은 회색)

                // 🔴 보스방 색상 세팅
                // 보스 대기실(5번 프리팹) 색상 세팅
                if (mapMng.m_MapList[roomID].prefabID == 6) {
                    fillColor = D3DCOLOR_ARGB(255, 150, 0, 0); // 진한 빨간색
                    lineColor = D3DCOLOR_ARGB(255, 255, 50, 50);
                }

                // 🔵 현재 내 위치 방 색상 세팅
                if (roomID == currentRoomID) {
                    fillColor = D3DCOLOR_ARGB(255, 0, 150, 255); // 파란색

                    // 현재 방은 반짝이는 형광 테두리 효과 (숨 쉬는 애니메이션)
                    int alpha = 150 + (int)(sin(GetTickCount() * 0.01f) * 100);
                    lineColor = D3DCOLOR_ARGB(alpha, 0, 255, 255);
                }

                // 2. 사각형 내부 색칠하기
                DrawSolidRect(cellX + padding, cellY + padding, cellSize - padding * 2, cellSize - padding * 2, fillColor);

                // 3. 엣지 있는 사각형 테두리 그리기
                //DrawLineRect(cellX + padding, cellY + padding, cellSize - padding * 2, cellSize - padding * 2, lineColor);

                // =========================================================
                // 4. 연결된 길(통로) 뚫어주기!
                // =========================================================
                // ① 오른쪽 방이랑 연결되었는가?
                if (x < 5) {
                    int rightRoom = mapMng.m_Grid[y][x + 1];
                    if (rightRoom == roomID) {
                        // 같은 거대 방 내부면 벽을 터서 하나처럼 보이게 함
                        DrawSolidRect(cellX + cellSize - padding, cellY + padding, padding * 2, cellSize - padding * 2, fillColor);
                    }
                    else if (mapMng.m_DoorRight[y][x]) {
                        // 다른 방으로 향하는 문(다리)
                        DrawSolidRect(cellX + cellSize - padding, cellY + cellSize / 2.0f - 2.0f, padding * 2, 4.0f, lineColor);
                    }
                }

                // ② 아래쪽 방이랑 연결되었는가?
                if (y < 5) {
                    int downRoom = mapMng.m_Grid[y + 1][x];
                    if (downRoom == roomID) {
                        DrawSolidRect(cellX + padding, cellY + cellSize - padding, cellSize - padding * 2, padding * 2, fillColor);
                    }
                    else if (mapMng.m_DoorDown[y][x]) {
                        DrawSolidRect(cellX + cellSize / 2.0f - 2.0f, cellY + cellSize - padding, 4.0f, padding * 2, lineColor);
                    }
                }
            }
        }
    }

    // 전체 맵 모드일 때 상단에 안내 텍스트
    if (m_bLargeMap) {
        dv_font.DrawString("MAP [M]", startX, startY - 40, D3DCOLOR_ARGB(255, 255, 255, 255));
    }
}