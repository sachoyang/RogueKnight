#pragma once
#include "Include.h"

class Collider
{


public :
	Collider();
	~Collider();

	std::list<RECT> m_Walls;

	bool isDebugDraw;	//콜라이더 박스 스위치

	void Init();
	void Update();
	void Draw();

	void BoxSow(RECT m_rc, long x, long y, D3DCOLOR color = D3DCOLOR_ARGB(255, 0, 255, 0));
	// [추가] 맵 바뀔 때마다 벽 리셋하고 다시 세팅하는 함수
	void ClearWalls() { m_Walls.clear(); }
	void AddWall(RECT rc) { m_Walls.push_back(rc); }
};

extern Collider coll;