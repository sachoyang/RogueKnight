#pragma once
#include "Include.h"
#include <vector>

// =======================================================
// A* 길찾기 (레퍼런스 Unity 구현을 C++/DX9 월드에 이식)
//  - 레퍼런스는 이미 격자(Node[,])가 있었지만, 이 게임은 연속 물리 월드다.
//    따라서 월드를 CELL_SIZE 간격의 격자로 '래스터화'하고,
//    각 셀이 coll.m_Walls(벽 RECT)와 겹치면 이동 불가(벽)로 마킹한다.
// =======================================================

// A* 디버그 시각화를 위한 노드 상태 (레퍼런스 색상 규칙과 동일)
enum PathVisual
{
    PV_NONE = 0,   // 흰색  : 기본(이동 가능)
    PV_WALL,       // 청록  : 벽(장애물)
    PV_OPEN,       // 노랑  : 열림 리스트
    PV_CLOSED,     // 회색  : 닫힘 리스트
    PV_NEIGHBOUR,  // 파랑  : 현재 시점의 이웃
    PV_PATH,       // 빨강  : 확정된 경로
    PV_START,      // 초록  : 시작(적)
    PV_TARGET      // 보라  : 타겟(기사)
};

// 격자 한 칸
struct PathNode
{
    int row, col;
    int gCost;          // 시작→현재까지 소비 비용
    int hCost;          // 현재→목표까지 추정 비용
    bool walkable;      // 이동 가능 여부 (벽이면 false)
    PathNode* parent;   // 경로 추적용 부모
    float worldX, worldY; // 셀 중심의 월드 좌표
    PathVisual visual;    // 디버그 시각화 상태

    int FCost() const { return gCost + hCost; }
};

class NavGrid
{
public:
    static const int CELL_SIZE = 40; // 셀 크기(px)

    std::vector<PathNode> m_nodes; // m_rows * m_cols (행 우선)
    int m_rows;
    int m_cols;
    int m_mapW;
    int m_mapH;

    NavGrid();

    // 현재 맵 크기로 격자를 만들고 coll.m_Walls로 벽을 마킹한다.
    void Build(int mapW, int mapH);
    // 벽(walkable) 정보만 다시 계산 (Build 내부에서 호출)
    void MarkWalls();

    bool Valid() const { return m_rows > 0 && m_cols > 0; }

    PathNode* At(int row, int col);
    PathNode* NodeFromWorld(float x, float y);

    // 두 노드의 거리(레퍼런스 휴리스틱: 대각 14, 직선 10)
    int GetDistance(PathNode* a, PathNode* b);
    // 8방향 이웃
    void Neighbours(PathNode* n, std::vector<PathNode*>& out);

    // 탐색 전 비용/부모/시각화 초기화 (벽 표시는 유지)
    void ResetCosts();

    // A* 실행. 성공 시 out에 월드 waypoint를 채우고 true 반환.
    bool FindPath(float sx, float sy, float tx, float ty,
                  std::vector<D3DXVECTOR2>& out);

    // =============== 디버그 렌더 ===============
    // 화면(카메라)에 보이는 셀만 상태별 색으로 채워 그린다.
    void DebugDrawBase();
    // 특정 경로(waypoint들)를 빨강, 시작 셀을 초록으로 오버레이.
    // pathColor: 적마다 다른 색을 주면 여러 경로가 겹쳐도 구분된다 (기본=빨강)
    void DebugDrawPath(const std::vector<D3DXVECTOR2>& path, float startX, float startY,
                       D3DCOLOR pathColor = D3DCOLOR_ARGB(160, 230, 40, 40));
    // 타겟(기사) 셀을 보라색으로.
    void DebugDrawTarget(float x, float y);

private:
    // 반투명 채움 박스 (월드좌표 RECT 기준)
    void FillCell(float wl, float wt, float wr, float wb, D3DCOLOR color);
    // 화면에 보이는 셀 범위 계산
    void VisibleRange(int& r0, int& r1, int& c0, int& c1);
};

extern NavGrid navGrid;
