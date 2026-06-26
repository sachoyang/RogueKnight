#include "Include.h"


Camera cam;

//	유일 객체 (데이타 영역에 올라간 변수에 초기값 을 nullptr 로 셋팅)
//  이 로직은 프로그램 시작전에(main 호출) 실행된다.
Camera * Camera::m_pInstance = nullptr;

Camera* Camera::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		// 힙 메모리에 객체 할당 and 데이타 영역의 참조 변수에 주소 할당
		m_pInstance = new Camera();
	}

	return m_pInstance;
}


Camera::Camera()
{
	camPos.posX = 0;
	camPos.posY = 0;
	mapWidth = 0;
	mapHeight = 0;
	target = nullptr;
}

Camera::~Camera()
{

}

void Camera::Init()
{
	camPos.posX = 0;
	camPos.posY = 0;
	// 기본값은 화면 크기와 같게 설정 (따로 설정 안 하면 고정됨)
	mapWidth = SCREEN_WITH;
	mapHeight = SCREEN_HEIGHT;
	target = nullptr;
}

void Camera::SetTarget(D3DXVECTOR2* _target)
{
	target = _target;
}

void Camera::SetMapSize(int w, int h)
{
	mapWidth = w;
	mapHeight = h;
}

void Camera::Update() // 원래는 모든 클래스를 다형성으로 배열로 참조하여 값을 전부 변경(리스트?)
{
	// 일단 맵 처리...
	/*mapMng.posX = camPos.posX;
	mapMng.posY = camPos.posY;*/

	// 타겟(플레이어)이 없으면 아무것도 안 함
	if (target == nullptr) return;

	// =============================================================
	// 1. 중앙 정렬 계산
	// 목표: 캐릭터가 화면 정중앙에 오게 카메라 좌표를 잡음
	// 공식: 캐릭터위치 - (화면크기 / 2)
	// =============================================================
	camPos.posX = target->x - (SCREEN_WITH / 2.0f);
	camPos.posY = target->y - (SCREEN_HEIGHT / 2.0f);

	// =============================================================
	// 2. 맵 밖으로 못 나가게 가두기 (Clamping)
	// =============================================================

	// 왼쪽 & 위쪽 제한 (0보다 작아질 수 없음)
	if (camPos.posX < 0) camPos.posX = 0;
	if (camPos.posY < 0) camPos.posY = 0;

	// 오른쪽 & 아래쪽 제한 (맵 끝 - 화면 크기 보다 커질 수 없음)
	if (camPos.posX > mapWidth - SCREEN_WITH)
		camPos.posX = (float)(mapWidth - SCREEN_WITH);

	if (camPos.posY > mapHeight - SCREEN_HEIGHT)
		camPos.posY = (float)(mapHeight - SCREEN_HEIGHT);

	if (GetTickCount() < m_ShakeEndTime) {
		float offsetX = ((rand() % 200) - 100) / 100.0f * m_ShakeIntensity;
		float offsetY = ((rand() % 200) - 100) / 100.0f * m_ShakeIntensity;
		camPos.posX += offsetX;
		camPos.posY += offsetY;
	}
}

void Camera::Shake(float intensity, DWORD duration)
{
	m_ShakeIntensity = intensity;
	m_ShakeEndTime = GetTickCount() + duration;
}

void Camera::SetXCam(double x)
{
	camPos.posX += x;
}

void Camera::SetYCam(double y)
{
	camPos.posY += y;
}

// Singleton 디자인 패턴으로 extern 선언 안해도 됨 (공부용)
