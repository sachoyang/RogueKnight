#include "Include.h"


Collider coll;


Collider::Collider()
{

}

Collider::~Collider()
{
}

void Collider::Init()
{
	/*m_rc.left = 0;
	m_rc.top = 600;
	m_rc.right = 1250;
	m_rc.bottom = 720;*/
	m_Walls.clear();
	isDebugDraw = false;
}

void Collider::Update()
{
}

// 바닥
void Collider::Draw()
{
	if ((Gmanager.m_GameStart == true) && isDebugDraw)
	{
		/*dv_font.DrawString("┌ ", m_rc.left, m_rc.top, D3DCOLOR_ARGB(255, 0, 255, 0));
		dv_font.DrawString("┐ ", m_rc.right, m_rc.top, D3DCOLOR_ARGB(255, 0, 255, 0));
		dv_font.DrawString("└", m_rc.left, m_rc.bottom, D3DCOLOR_ARGB(255, 0, 255, 0));
		dv_font.DrawString(" ┘", m_rc.right, m_rc.bottom, D3DCOLOR_ARGB(255, 0, 255, 0));*/
		// 리스트에 있는 모든 벽 그리기
		for (auto& wall : m_Walls)
		{
			// 벽을 초록색 박스로 그림
			BoxSow(wall, 0, 0, D3DCOLOR_ARGB(255, 0, 255, 0));
		}
	}
}

// 디버그 콜라이더 (x, y는 오프셋)
//void Collider::BoxSow(RECT m_rc, long x, long y, D3DCOLOR color)
//{
//	if (Gmanager.m_GameStart == true /* && 디버그 일때 처리 */)
//	{
//		/*dv_font.DrawString("┌ ", m_rc.left-x, m_rc.top  - y, color);
//		dv_font.DrawString("┐ ", m_rc.right+x, m_rc.top - y, color);
//		dv_font.DrawString("└", m_rc.left-x, m_rc.bottom + y, color);
//		dv_font.DrawString(" ┘", m_rc.right+x, m_rc.bottom + y, color);*/
//		// Draw slightly inside the rect so it's visible
//
//		// 화면에 그릴 좌표 계산
//		int viewLeft = m_rc.left - (int)CAM->GetX();
//		int viewTop = m_rc.top - (int)CAM->GetY();
//		int viewRight = m_rc.right - (int)CAM->GetX();
//		int viewBottom = m_rc.bottom - (int)CAM->GetY();
//
//		dv_font.DrawString("┌", viewLeft - x, viewTop - y, color);
//		dv_font.DrawString("┐", viewRight + x, viewTop - y, color);
//		dv_font.DrawString("└", viewLeft - x, viewBottom + y, color);
//		dv_font.DrawString(" ┘", viewRight + x, viewBottom + y, color);
//
//		//// Top-Left
//		//dv_font.DrawString("┌", m_rc.left + 5, m_rc.top + 5, color);
//		//// Top-Right (Draw slightly left of the right edge)
//		//dv_font.DrawString("┐", m_rc.right - 15, m_rc.top + 5, color);
//		//// Bottom-Left (Draw slightly above the bottom edge)
//		//dv_font.DrawString("└", m_rc.left + 5, m_rc.bottom - 15, color);
//		//// Bottom-Right
//		//dv_font.DrawString("┘", m_rc.right - 15, m_rc.bottom - 15, color);
//	}
//
//}

// 디버그 콜라이더(정확한 1픽셀 선으로 그리기)
void Collider::BoxSow(RECT m_rc, long x, long y, D3DCOLOR color)
{
	// 스위치가 꺼져있으면 아예 안 그림
	if (!isDebugDraw || !Gmanager.m_GameStart) return;

	// 화면에 그릴 카메라 좌표 계산
	float viewLeft = (float)(m_rc.left - CAM->GetX() - x);
	float viewTop = (float)(m_rc.top - CAM->GetY() - y);
	float viewRight = (float)(m_rc.right - CAM->GetX() + x);
	float viewBottom = (float)(m_rc.bottom - CAM->GetY() + y);

	// 🌟 4개의 꼭짓점을 정의 (사각형 테두리)
	struct Vertex {
		float x, y, z, rhw;
		D3DCOLOR color;
	};

	Vertex vertices[] = {
		{ viewLeft,  viewTop,    0.0f, 1.0f, color }, // 좌상단
		{ viewRight, viewTop,    0.0f, 1.0f, color }, // 우상단
		{ viewRight, viewBottom, 0.0f, 1.0f, color }, // 우하단
		{ viewLeft,  viewBottom, 0.0f, 1.0f, color }, // 좌하단
		{ viewLeft,  viewTop,    0.0f, 1.0f, color }  // 다시 좌상단으로 돌아와서 선 닫기
	};

	// DirectX 기능으로 하얀색 텍스처를 빼고, 순수하게 선만 그립니다!
	dv_font.Device9->SetTexture(0, NULL);
	dv_font.Device9->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	dv_font.Device9->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, vertices, sizeof(Vertex));
}