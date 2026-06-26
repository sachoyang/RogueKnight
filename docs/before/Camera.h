#pragma once

struct CameraPos
{
	double posX;
	double posY;

};

class Camera
{
private:
	//	유일한 객체
	// 먼저 데이타 영역에 올라감
	static Camera* m_pInstance;
	CameraPos camPos;

	float m_ShakeIntensity;   // 흔들림 강도
	DWORD m_ShakeEndTime;     // 흔들림 끝나는 시간
public:
	Camera(void);
	~Camera(void);

	//	객체 얻기
	static Camera*	GetInstance();

	// 맵 전체 크기 (이 밖으로 카메라가 못 나감)
	int mapWidth, mapHeight;

	// 카메라가 따라다닐 대상 (플레이어 좌표 포인터)
	D3DXVECTOR2* target;

	void Shake(float intensity, DWORD duration); // 흔들기 호출 함수!

	void SetXCam(double);
	void SetYCam(double);

	// 초기화 및 설정 함수
	void Init();
	void SetTarget(D3DXVECTOR2* _target);
	void SetMapSize(int w, int h);

	// 외부에서 카메라 위치를 가져갈 수 있게 해주는 함수
	double GetX() { return camPos.posX; }
	double GetY() { return camPos.posY; }

	void Update();
};
#define CAM Camera::GetInstance()