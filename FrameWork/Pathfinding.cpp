#include "Include.h"

NavGrid navGrid;

NavGrid::NavGrid()
{
    m_rows = 0;
    m_cols = 0;
    m_mapW = 0;
    m_mapH = 0;
}

// -------------------------------------------------------
// 격자 생성 : 맵 크기를 CELL_SIZE로 나눠 셀을 만들고 벽을 마킹
// -------------------------------------------------------
void NavGrid::Build(int mapW, int mapH)
{
    m_mapW = mapW;
    m_mapH = mapH;

    m_cols = (mapW + CELL_SIZE - 1) / CELL_SIZE; // 올림
    m_rows = (mapH + CELL_SIZE - 1) / CELL_SIZE;

    if (m_cols < 1) m_cols = 1;
    if (m_rows < 1) m_rows = 1;

    m_nodes.assign((size_t)m_rows * m_cols, PathNode());

    for (int r = 0; r < m_rows; ++r)
    {
        for (int c = 0; c < m_cols; ++c)
        {
            PathNode& n = m_nodes[(size_t)r * m_cols + c];
            n.row = r;
            n.col = c;
            n.gCost = 0;
            n.hCost = 0;
            n.parent = nullptr;
            n.walkable = true;
            n.visual = PV_NONE;
            // 셀 중심 월드 좌표
            n.worldX = c * (float)CELL_SIZE + CELL_SIZE * 0.5f;
            n.worldY = r * (float)CELL_SIZE + CELL_SIZE * 0.5f;
        }
    }

    MarkWalls();
}

// coll.m_Walls와 겹치는 셀을 이동 불가(벽)로 마킹
void NavGrid::MarkWalls()
{
    for (auto& n : m_nodes)
    {
        RECT cell;
        cell.left   = (LONG)(n.col * CELL_SIZE);
        cell.top    = (LONG)(n.row * CELL_SIZE);
        cell.right  = cell.left + CELL_SIZE;
        cell.bottom = cell.top + CELL_SIZE;

        bool blocked = false;
        RECT tmp;
        for (auto& w : coll.m_Walls)
        {
            if (IntersectRect(&tmp, &cell, &w)) { blocked = true; break; }
        }

        n.walkable = !blocked;
        n.visual = blocked ? PV_WALL : PV_NONE;
    }
}

PathNode* NavGrid::At(int row, int col)
{
    if (row < 0 || row >= m_rows || col < 0 || col >= m_cols) return nullptr;
    return &m_nodes[(size_t)row * m_cols + col];
}

PathNode* NavGrid::NodeFromWorld(float x, float y)
{
    int col = (int)(x / CELL_SIZE);
    int row = (int)(y / CELL_SIZE);
    return At(row, col);
}

// 레퍼런스 휴리스틱: 대각 14, 직선 10
int NavGrid::GetDistance(PathNode* a, PathNode* b)
{
    int dx = abs(a->col - b->col);
    int dy = abs(a->row - b->row);
    return 14 * min(dx, dy) + 10 * abs(dx - dy);
}

// 8방향 이웃 (격자 범위 안에 있는 셀만)
void NavGrid::Neighbours(PathNode* n, std::vector<PathNode*>& out)
{
    out.clear();
    for (int dr = -1; dr <= 1; ++dr)
    {
        for (int dc = -1; dc <= 1; ++dc)
        {
            if (dr == 0 && dc == 0) continue;
            PathNode* nb = At(n->row + dr, n->col + dc);
            if (nb) out.push_back(nb);
        }
    }
}

void NavGrid::ResetCosts()
{
    for (auto& n : m_nodes)
    {
        n.gCost = 0;
        n.hCost = 0;
        n.parent = nullptr;
        // 시각화는 벽은 유지, 나머지는 기본으로
        n.visual = n.walkable ? PV_NONE : PV_WALL;
    }
}

// -------------------------------------------------------
// A* 본체 (레퍼런스 FindPath 로직 이식)
// -------------------------------------------------------
bool NavGrid::FindPath(float sx, float sy, float tx, float ty,
                       std::vector<D3DXVECTOR2>& out)
{
    out.clear();
    if (!Valid()) return false;

    ResetCosts();

    PathNode* startNode = NodeFromWorld(sx, sy);
    PathNode* targetNode = NodeFromWorld(tx, ty);
    if (!startNode || !targetNode) return false;

    // 시작/도착 셀이 벽과 겹쳐도(적이 바닥 위에 서 있는 등) 끝점은 통과 허용
    startNode->walkable = true;
    targetNode->walkable = true;

    std::vector<PathNode*> openList;
    std::vector<PathNode*> closedList;

    startNode->gCost = 0;
    startNode->hCost = GetDistance(startNode, targetNode);
    openList.push_back(startNode);

    std::vector<PathNode*> neighbours;

    while (!openList.empty())
    {
        // 오픈 리스트에서 최소 FCost(동률이면 HCost) 노드 선택
        int bestIdx = 0;
        for (int i = 1; i < (int)openList.size(); ++i)
        {
            PathNode* a = openList[i];
            PathNode* b = openList[bestIdx];
            if (a->FCost() < b->FCost() ||
                (a->FCost() == b->FCost() && a->hCost < b->hCost))
            {
                bestIdx = i;
            }
        }

        PathNode* curr = openList[bestIdx];

        // 도착!
        if (curr == targetNode)
        {
            // 부모를 따라 경로 역추적
            std::vector<D3DXVECTOR2> rev;
            PathNode* p = curr;
            while (p)
            {
                rev.push_back(D3DXVECTOR2(p->worldX, p->worldY));
                p->visual = PV_PATH;
                p = p->parent;
            }
            startNode->visual = PV_START;
            targetNode->visual = PV_TARGET;

            // 역순 → 시작에서 목표 순서로
            for (int i = (int)rev.size() - 1; i >= 0; --i)
                out.push_back(rev[i]);
            return true;
        }

        // curr를 오픈에서 빼고 닫힘으로
        openList.erase(openList.begin() + bestIdx);
        closedList.push_back(curr);
        if (curr->visual != PV_START) curr->visual = PV_CLOSED;

        Neighbours(curr, neighbours);
        for (auto* nb : neighbours)
        {
            if (!nb->walkable) continue;

            // 닫힘 리스트에 있으면 skip
            bool inClosed = false;
            for (auto* cn : closedList) { if (cn == nb) { inClosed = true; break; } }
            if (inClosed) continue;

            int newG = curr->gCost + GetDistance(curr, nb);

            bool inOpen = false;
            for (auto* on : openList) { if (on == nb) { inOpen = true; break; } }

            if (!inOpen || newG < nb->gCost)
            {
                nb->gCost = newG;
                nb->hCost = GetDistance(nb, targetNode);
                nb->parent = curr;

                if (!inOpen)
                {
                    openList.push_back(nb);
                    if (nb->visual != PV_TARGET) nb->visual = PV_OPEN;
                }
            }
        }
    }

    // 경로 없음
    return false;
}

// =======================================================
// 디버그 렌더
// =======================================================

// 화면(카메라)에 보이는 셀 인덱스 범위
void NavGrid::VisibleRange(int& r0, int& r1, int& c0, int& c1)
{
    float camX = (float)CAM->GetX();
    float camY = (float)CAM->GetY();

    c0 = (int)(camX / CELL_SIZE) - 1;
    r0 = (int)(camY / CELL_SIZE) - 1;
    c1 = (int)((camX + SCREEN_WITH) / CELL_SIZE) + 1;
    r1 = (int)((camY + SCREEN_HEIGHT) / CELL_SIZE) + 1;

    if (c0 < 0) c0 = 0;
    if (r0 < 0) r0 = 0;
    if (c1 > m_cols) c1 = m_cols;
    if (r1 > m_rows) r1 = m_rows;
}

// 반투명 채움 박스 (월드좌표 → 화면좌표)
void NavGrid::FillCell(float wl, float wt, float wr, float wb, D3DCOLOR color)
{
    float l = wl - (float)CAM->GetX();
    float t = wt - (float)CAM->GetY();
    float r = wr - (float)CAM->GetX();
    float b = wb - (float)CAM->GetY();

    struct Vertex { float x, y, z, rhw; D3DCOLOR color; };
    Vertex v[4] = {
        { l, t, 0.0f, 1.0f, color },
        { r, t, 0.0f, 1.0f, color },
        { l, b, 0.0f, 1.0f, color },
        { r, b, 0.0f, 1.0f, color },
    };

    IDirect3DDevice9* dev = dv_font.Device9;

    // 반투명 블렌딩 켜기
    DWORD oldAlpha, oldSrc, oldDst;
    dev->GetRenderState(D3DRS_ALPHABLENDENABLE, &oldAlpha);
    dev->GetRenderState(D3DRS_SRCBLEND, &oldSrc);
    dev->GetRenderState(D3DRS_DESTBLEND, &oldDst);

    dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    dev->SetTexture(0, NULL);
    dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(Vertex));

    // 상태 복원
    dev->SetRenderState(D3DRS_SRCBLEND, oldSrc);
    dev->SetRenderState(D3DRS_DESTBLEND, oldDst);
    dev->SetRenderState(D3DRS_ALPHABLENDENABLE, oldAlpha);
}

void NavGrid::DebugDrawBase()
{
    if (!Valid()) return;

    int r0, r1, c0, c1;
    VisibleRange(r0, r1, c0, c1);

    const BYTE A = 90; // 반투명 정도

    for (int r = r0; r < r1; ++r)
    {
        for (int c = c0; c < c1; ++c)
        {
            PathNode* n = At(r, c);
            if (!n) continue;

            D3DCOLOR col;
            switch (n->visual)
            {
            case PV_WALL:      col = D3DCOLOR_ARGB(A, 0, 200, 200);   break; // 청록
            case PV_OPEN:      col = D3DCOLOR_ARGB(A, 230, 230, 0);   break; // 노랑
            case PV_CLOSED:    col = D3DCOLOR_ARGB(A, 120, 120, 120); break; // 회색
            case PV_NEIGHBOUR: col = D3DCOLOR_ARGB(A, 60, 120, 255);  break; // 파랑
            case PV_PATH:      col = D3DCOLOR_ARGB(A, 230, 40, 40);   break; // 빨강
            case PV_START:     col = D3DCOLOR_ARGB(A, 40, 220, 40);   break; // 초록
            case PV_TARGET:    col = D3DCOLOR_ARGB(A, 200, 40, 220);  break; // 보라
            default:           col = D3DCOLOR_ARGB(40, 255, 255, 255); break; // 흰색(옅게)
            }

            float wl = (float)(c * CELL_SIZE) + 1;
            float wt = (float)(r * CELL_SIZE) + 1;
            float wr = (float)((c + 1) * CELL_SIZE) - 1;
            float wb = (float)((r + 1) * CELL_SIZE) - 1;
            FillCell(wl, wt, wr, wb, col);
        }
    }
}

void NavGrid::DebugDrawPath(const std::vector<D3DXVECTOR2>& path, float startX, float startY)
{
    if (!Valid()) return;

    const BYTE A = 160;
    for (size_t i = 0; i < path.size(); ++i)
    {
        PathNode* n = NodeFromWorld(path[i].x, path[i].y);
        if (!n) continue;
        D3DCOLOR col = D3DCOLOR_ARGB(A, 230, 40, 40); // 빨강 경로
        float wl = (float)(n->col * CELL_SIZE) + 6;
        float wt = (float)(n->row * CELL_SIZE) + 6;
        float wr = (float)((n->col + 1) * CELL_SIZE) - 6;
        float wb = (float)((n->row + 1) * CELL_SIZE) - 6;
        FillCell(wl, wt, wr, wb, col);
    }

    // 시작(적) 셀은 초록
    PathNode* s = NodeFromWorld(startX, startY);
    if (s)
    {
        float wl = (float)(s->col * CELL_SIZE) + 3;
        float wt = (float)(s->row * CELL_SIZE) + 3;
        float wr = (float)((s->col + 1) * CELL_SIZE) - 3;
        float wb = (float)((s->row + 1) * CELL_SIZE) - 3;
        FillCell(wl, wt, wr, wb, D3DCOLOR_ARGB((BYTE)200, 40, 220, 40));
    }
}

void NavGrid::DebugDrawTarget(float x, float y)
{
    if (!Valid()) return;
    PathNode* t = NodeFromWorld(x, y);
    if (!t) return;
    float wl = (float)(t->col * CELL_SIZE) + 3;
    float wt = (float)(t->row * CELL_SIZE) + 3;
    float wr = (float)((t->col + 1) * CELL_SIZE) - 3;
    float wb = (float)((t->row + 1) * CELL_SIZE) - 3;
    FillCell(wl, wt, wr, wb, D3DCOLOR_ARGB((BYTE)200, 200, 40, 220));
}
