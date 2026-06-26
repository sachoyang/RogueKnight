#pragma once
#include "Include.h"
#include <vector>
#include <map>

// =======================================================
// 프리팹 방 방향 비트 플래그
// =======================================================
#define DOOR_UP    1   // 0001
#define DOOR_DOWN  2   // 0010
#define DOOR_LEFT  4   // 0100
#define DOOR_RIGHT 8   // 1000
// 보스방 고유 ID
#define ROOM_BOSS  16

// 1. 타일 타입 정의 (기획하신 리소스 목록)
enum TileType {
	TILE_NONE = 0,
	TILE_FLOOR,     // 바닥 (270*80)
	TILE_PLATFORM,  // 플랫폼 (283*71, 스케일링)
	TILE_WALL_L,    // 왼쪽 벽 (240*339, 우측 정렬)
	TILE_WALL_R,    // 오른쪽 벽 (좌측 정렬)
	TILE_CEILING    // 천장 (316*47)
};

// 2. 벽 정보 구조체 (충돌박스 + 이미지타입)
struct WallInfo {
	RECT rc;
	int type; // TileType
};

// =======================================================
// 프리팹(도면) 구조체
// =======================================================
struct RoomPrefab
{
	int typeID;           // 1~15 사이의 값 (문이 뚫린 방향의 합)
	int width;            // 이 방의 가로 크기
	int height;           // 이 방의 세로 크기
	
	int gridW;				// 이 방이 차지하는 그리드 칸 수 (가로)
	int gridH;				// 이 방이 차지하는 그리드 칸 수 (세로)

	Sprite bgLayer[2];    // 배경 이미지 (일단 2개 쓴다고 가정)
	int layerCount;
	// 이 방에 무조건 고정으로 들어갈 벽과 발판들
	//std::list<RECT> walls;
	std::vector<WallInfo> walls;
	// 특정방향으로 들어왔을 때 플레이어가 스폰될 위치 (문 위치에서 약간 안쪽으로)
	// 인덱스: 1(UP), 2(DOWN), 3(LEFT), 4(RIGHT)
	float spawnX[5];
	float spawnY[5];
};

enum DIR_TYPE { DIR_UP = 1, DIR_DOWN, DIR_LEFT, DIR_RIGHT, DIR_NUM };

struct MapChunk
{
	int id;
	Sprite bgLayer[5];
	int layerCount;

	int nextMapID[DIR_NUM]; // 다음 맵 ID (오른쪽, 왼쪽, 위, 아래)
	// 문 위치나 몬스터 위치 등등

	// 맵 크기 정보
	int width;
	int height;

	int prefabID;
};

//시체 정보를 담을 구조체
struct CorpseInfo {
	float x, y; // 시체가 널브러진 최종 좌표
	int dir;    // 시체가 바라보는 방향
};

class Enemy;


class MapManager
{

public :
	MapManager();
	~MapManager();

	MapChunk* m_pCurrentMapChunk;

	// 4. 타일 이미지 저장소
	//Sprite m_TileImages[10];
	Sprite m_FloorImages[3];    // 바닥 3종
	Sprite m_PlatformImage;     // 플랫폼
	Sprite m_WallImage;         // 벽 (왼쪽 기준, 오른쪽 벽은 반전해서 사용)
	Sprite m_CeilingImage;      // 천장

	// ChangeMap에서 생성된 '동적 벽(캡핑)'들을 저장할 리스트
	std::vector<WallInfo> m_DynamicWalls;

	Sprite m_GameOverTitle; // GAME OVER 글씨
	Sprite m_BlackScreen;   // 검은 배경 (1x1 픽셀 이미지 추천)
	D3DXIMAGE_INFO goimagesinfo;
	// std::map<int, MapChunk*> m_MapList;
	MapChunk m_MapList[40];


	std::list<Enemy*> m_Enemies; //현재 맵의 적 리스트
	// 사망자 명부 (Key: 몬스터ID, Value: 위치 정보)
	std::map<int, CorpseInfo> m_CorpseRegistry;

	// 게임 클리어(보스 사망) 시간 기록용
	DWORD m_GameClearTime;

	// UI에서 미니맵을 그릴 때 읽어갈 6x6 배열
	int m_Grid[6][6];

	// 

	// =======================================================
	// 도어 소켓 시스템 변수 및 헬퍼 함수
	// =======================================================
	bool m_DoorRight[6][6]; // [y][x]와 [y][x+1] 사이의 문 여부
	bool m_DoorDown[6][6];  // [y][x]와 [y+1][x] 사이의 문 여부

	int GetRoomGridX(int roomID); // 이 방의 기준점(좌측 끝) X칸 구하기
	int GetRoomGridY(int roomID); // 이 방의 기준점(상단 끝) Y칸 구하기


	Sprite m_MapImg1_1[6];
	Sprite m_MapImg1_2[45];

	DWORD m_MapImg1_1_ani1;

	int m_MapImg1_1_ani1Count;

	int m_Stage;

	double posX, posY;

	RoomPrefab m_Prefabs[17]; // 1~16번 프리팹 (0번은 빈칸)

	void Init();
	void Update(double frame);
	void Draw();

	void InitPrefabs();
	void CreateRandomMap();
	void ChangeMap(int mapID);

	//디버그용
	const char* GetStateName(int state);
	int m_DebugPrefabID; // 현재 보고 있는 프리팹 번호
	void LoadDebugPrefab(int pID); // 특정 프리팹을 강제로 로드하는 함수
};

extern MapManager mapMng;