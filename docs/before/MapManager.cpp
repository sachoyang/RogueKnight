#include "Include.h"

MapManager mapMng;

int MapManager::GetRoomGridX(int roomID) {
	for (int y = 0; y < 6; y++)
		for (int x = 0; x < 6; x++)
			if (m_Grid[y][x] == roomID) return x;
	return 0;
}
int MapManager::GetRoomGridY(int roomID) {
	for (int y = 0; y < 6; y++)
		for (int x = 0; x < 6; x++)
			if (m_Grid[y][x] == roomID) return y;
	return 0;
}

MapManager::MapManager()
{
	m_Stage = 1;
	m_pCurrentMapChunk = nullptr;
}

MapManager::~MapManager()
{
}

void MapManager::Init()
{
	srand((unsigned int)time(NULL)); // 랜덤 시드 초기화
	m_GameOverTitle.Create("./resource/Img/UI/gameover_text.png", false, 0);
	m_BlackScreen.Create("./resource/Img/UI/black_bg.png", false, 0);
	D3DXGetImageInfoFromFile("./resource/Img/UI/gameover_text.png", &goimagesinfo);

	// 타일 이미지 로드
	// 1. 바닥
	m_FloorImages[0].Create("./resource/Img/map/floor01.png", false, 0);
	D3DXGetImageInfoFromFile("./resource/Img/map/floor01.png", &m_FloorImages[0].imagesinfo);
	m_FloorImages[1].Create("./resource/Img/map/floor02.png", false, 0);
	D3DXGetImageInfoFromFile("./resource/Img/map/floor02.png", &m_FloorImages[1].imagesinfo);
	m_FloorImages[2].Create("./resource/Img/map/floor03.png", false, 0);	
	D3DXGetImageInfoFromFile("./resource/Img/map/floor03.png", &m_FloorImages[2].imagesinfo);
	// 2. 플랫폼
	m_PlatformImage.Create("./resource/Img/map/platform.png", false, 0);	
	D3DXGetImageInfoFromFile("./resource/Img/map/platform.png", &m_PlatformImage.imagesinfo);

	// 3. 왼쪽 벽 (왼쪽:검은색 / 오른쪽:벽)
	m_WallImage.Create("./resource/Img/map/wall01.png", false, 0);
	D3DXGetImageInfoFromFile("./resource/Img/map/wall01.png", &m_WallImage.imagesinfo);
	// 4. 오른쪽 벽 (왼쪽 벽 이미지를 뒤집어 쓸 예정이면 따로 로드 안 해도 됨)
	// 5. 천장
	m_CeilingImage.Create("./resource/Img/map/ceiling01.png", false, 0);
	D3DXGetImageInfoFromFile("./resource/Img/map/ceiling.png", &m_CeilingImage.imagesinfo);

	m_CorpseRegistry.clear();
	
	m_GameClearTime = 0;

	// 프리펩 초기화 먼저 진행 후 맵 생성 (맵 생성 시 프리펩 정보가 필요)
	InitPrefabs();

	// [랜덤 맵 생성] 
	CreateRandomMap();

	// [초기 시작] 1번 맵으로 시작!
	ChangeMap(1);
	SOUND->PlayBGM(SND_BGM_STAGE1);
}

void MapManager::InitPrefabs()
{
	//RECT rc;
	int pH = 30; // 발판 두께

	// 초기화
	for (int i = 0; i < 17; i++) {
		m_Prefabs[i].typeID = 0;
		m_Prefabs[i].walls.clear();
	}
	WallInfo w;
	// ====================================================================
	// 🌟 [도어 소켓 룰] 🌟
	// 1. 좌/우 문: 항상 y = 700 위치에서 진입. (그래서 바닥 양옆에 300px짜리 발판이 무조건 있음)
	// 2. 하단 문: 항상 x = 300 ~ 980 사이의 680px 거대 구멍으로 떨어짐.
	// 3. 상단 문: 떨어질 때를 대비해 x = 490 ~ 790, y = 250 위치에 낙하 방지용 발판이 있음.
	// ====================================================================

	// [프리팹 1번] 1x1 기본 방
	{
		int id = 1;
		m_Prefabs[id].typeID = 1;
		m_Prefabs[id].gridW = 1; m_Prefabs[id].gridH = 1;
		m_Prefabs[id].width = SCREEN_WITH; m_Prefabs[id].height = SCREEN_HEIGHT;
		m_Prefabs[id].layerCount = 1;
		m_Prefabs[id].bgLayer[0].Create("./resource/Img/map1/Ch1_maps/map01.png", false, 0);

		// 🌟 도어 소켓 (안전 지대)
		//SetRect(&rc, 0, 770, 300, 770 + pH); m_Prefabs[id].walls.push_back(rc); // 좌측 문 앞
		//SetRect(&rc, 980, 770, 1280, 770 + pH); m_Prefabs[id].walls.push_back(rc); // 우측 문 앞
		SetRect(&w.rc, 490, 250, 790, 250 + pH);
		w.type = TILE_PLATFORM; 
		m_Prefabs[id].walls.push_back(w); //m_Prefabs[id].walls.push_back(rc); // 상단 문 낙하 방지

		// 내부 지형 (자유롭게 디자인)
		SetRect(&w.rc, 350, 500, 550, 500 + pH); 
		w.type = TILE_PLATFORM; 
		m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
		SetRect(&w.rc, 730, 500, 930, 500 + pH);
		w.type = TILE_PLATFORM; 
		m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
	}

	// [프리팹 2번] 1x2 수직 방 (엘리베이터)
	{
		int id = 2;
		m_Prefabs[id].typeID = 2;
		m_Prefabs[id].gridW = 1; m_Prefabs[id].gridH = 2;
		m_Prefabs[id].width = SCREEN_WITH; m_Prefabs[id].height = SCREEN_HEIGHT * 2;
		m_Prefabs[id].layerCount = 1;
		m_Prefabs[id].bgLayer[0].Create("./resource/Img/map1/Ch1_maps/map02.png", false, 0);

		// 도어 소켓 일괄 생성 (2개 칸 각각에 대해)
		for (int gy = 0; gy < 2; gy++) {
			float cy = gy * SCREEN_HEIGHT;
			switch (gy)
			{
			case 0:
				SetRect(&w.rc, 0, cy + 750, 300, cy + 750 + pH); 
				w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
				m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
				SetRect(&w.rc, 980, cy + 750, 1280, cy + 750 + pH);
				w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
				m_Prefabs[id].walls.push_back(w);// m_Prefabs[id].walls.push_back(rc);
				break;
			case 1:
				break;
			default:
				break;
			}
			//SetRect(&rc, 0, cy + 770, 300, cy + 770 + pH); m_Prefabs[id].walls.push_back(rc);
			//SetRect(&rc, 980, cy + 770, 1280, cy + 770 + pH); m_Prefabs[id].walls.push_back(rc);
			SetRect(&w.rc, 530, cy + 250, 750, cy + 250 + pH); w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
			m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
		}

		// 수직 연결 지형
		SetRect(&w.rc, 300, 1300, 500, 1300 + pH); 
		w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
		m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
		SetRect(&w.rc, 780, 1100, 980, 1100 + pH); w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
		m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
		SetRect(&w.rc, 300, 850, 500, 850 + pH); w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
		m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
		SetRect(&w.rc, 490, 550, 790, 550 + pH); w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
		m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
	}

	// [프리팹 3번] 2x1 수평 방 (긴 복도/웅덩이)
	{
		int id = 3;
		m_Prefabs[id].typeID = 3;
		m_Prefabs[id].gridW = 2; m_Prefabs[id].gridH = 1;
		m_Prefabs[id].width = SCREEN_WITH * 2; m_Prefabs[id].height = SCREEN_HEIGHT;
		m_Prefabs[id].layerCount = 1;
		m_Prefabs[id].bgLayer[0].Create("./resource/Img/map1/Ch1_maps/map03.png", false, 0);

		for (int gx = 0; gx < 2; gx++) {
			float cx = gx * SCREEN_WITH;
			/*SetRect(&rc, cx, 700, cx + 300, 700 + pH); m_Prefabs[id].walls.push_back(rc);
			SetRect(&rc, cx + 980, 700, cx + 1280, 700 + pH); m_Prefabs[id].walls.push_back(rc);*/
			SetRect(&w.rc, cx + 490, 250, cx + 790, 250 + pH); w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
			m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
		}

		// 웅덩이 위 흔들다리 지형
		SetRect(&w.rc, 800, 600, 1000, 600 + pH); w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
		m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
		SetRect(&w.rc, 1180, 500, 1380, 500 + pH); w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
		m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
		SetRect(&w.rc, 1560, 600, 1760, 600 + pH); w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
		m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
	}

	// [프리팹 4번] 2x2 거대 방 (중앙 허브)
	{
		int id = 4;
		m_Prefabs[id].typeID = 4;
		m_Prefabs[id].gridW = 2; m_Prefabs[id].gridH = 2;
		m_Prefabs[id].width = SCREEN_WITH * 2; m_Prefabs[id].height = SCREEN_HEIGHT * 2;
		m_Prefabs[id].layerCount = 1;
		m_Prefabs[id].bgLayer[0].Create("./resource/Img/map1/Ch1_maps/map04.png", false, 0);

		for (int gy = 0; gy < 2; gy++) {
			for (int gx = 0; gx < 2; gx++) {
				float cx = gx * SCREEN_WITH; float cy = gy * SCREEN_HEIGHT;
				switch (gy)
				{
				case 0:
					SetRect(&w.rc, cx, cy + 750, cx + 300, cy + 750 + pH); w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
					m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
					SetRect(&w.rc, cx + 980, cy + 750, cx + 1280, cy + 750 + pH); w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
					m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
					break;
				case 1:
					break;
				default:
					break;
				}
				//SetRect(&rc, cx, cy + 770, cx + 300, cy + 770 + pH); m_Prefabs[id].walls.push_back(rc);
				//SetRect(&rc, cx + 980, cy + 770, cx + 1280, cy + 770 + pH); m_Prefabs[id].walls.push_back(rc);
				SetRect(&w.rc, cx + 490, cy + 250, cx + 790, cy + 250 + pH); w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
				m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
			}
		}

		// 플랫폼
		SetRect(&w.rc, 1000, 1100, 1560, 1100 + pH); w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
		m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
		SetRect(&w.rc, 600, 1300, 800, 1300 + pH); w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
		m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
		SetRect(&w.rc, 1760, 1300, 1960, 1300 + pH); w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
		m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
		SetRect(&w.rc, 250, 500, 550, 500 + pH); w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
		m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
		SetRect(&w.rc, 1550, 500, 1850, 500 + pH); w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
		m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
	}
	// [프리팹 5번] 1x1 기본 방 변형
	{
		int id = 5;
		m_Prefabs[id].typeID = 5;
		m_Prefabs[id].gridW = 1; m_Prefabs[id].gridH = 1;
		m_Prefabs[id].width = SCREEN_WITH; m_Prefabs[id].height = SCREEN_HEIGHT;
		m_Prefabs[id].layerCount = 1;
		m_Prefabs[id].bgLayer[0].Create("./resource/Img/map1/Ch1_maps/map01.png", false, 0);

		// 🌟 도어 소켓 (안전 지대)
		//SetRect(&rc, 0, 770, 300, 770 + pH); m_Prefabs[id].walls.push_back(rc); // 좌측 문 앞
		//SetRect(&rc, 980, 770, 1280, 770 + pH); m_Prefabs[id].walls.push_back(rc); // 우측 문 앞
		SetRect(&w.rc, 490, 250, 790, 250 + pH); w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
		m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc); // 상단 문 낙하 방지

		// 내부 지형 (자유롭게 디자인)
		SetRect(&w.rc, 440, 500, 840, 500 + pH); w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
		m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
		//SetRect(&rc, 200, 400, 400, 400 + pH); m_Prefabs[id].walls.push_back(rc);
		//SetRect(&rc, 880, 400, 1080, 400 + pH); m_Prefabs[id].walls.push_back(rc);
	}
	// [프리팹 6번] 보스 대기실 (고정 1x1, 우측에 텔레포터)
	{
		int id = 6;
		m_Prefabs[id].typeID = 6;
		m_Prefabs[id].gridW = 1; m_Prefabs[id].gridH = 1;
		m_Prefabs[id].width = SCREEN_WITH; m_Prefabs[id].height = SCREEN_HEIGHT;
		m_Prefabs[id].layerCount = 1;
		m_Prefabs[id].bgLayer[0].Create("./resource/Img/map1/Ch1_maps/map_bossin.png", false, 0);

		// 기본 도어 소켓 (왼쪽 문만 뚫릴 예정이므로 왼쪽만 깔아둬도 무방함)
		// SetRect(&rc, 0, 700, 300, 700 + pH); m_Prefabs[id].walls.push_back(rc);

		// 보스방 텔레포트 제단 (중앙)
		SetRect(&w.rc, 490, 550, 790, 580); w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
		m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
		SetRect(&w.rc, 540, 520, 740, 550); w.type = TILE_PLATFORM; // 🌟 타입 지정 (알아서 늘어남)
		m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
	}

	// [프리팹 16번] 보스방 (고정 2x1) 
	{
		int id = ROOM_BOSS; // 16
		m_Prefabs[id].typeID = 16;
		m_Prefabs[id].gridW = 2; m_Prefabs[id].gridH = 1;
		m_Prefabs[id].width = SCREEN_WITH * 2; m_Prefabs[id].height = SCREEN_HEIGHT;
		m_Prefabs[id].layerCount = 1;
		m_Prefabs[id].bgLayer[0].Create("./resource/Img/map1/Ch1_maps/map_bossroom01.png", false, 0);

		//보스방은 텔레포트 전용이므로 소켓이 필요 없고 통짜 바닥만 깝니다.
		SetRect(&w.rc, 0, 700, 2560, 700 + pH); w.type = TILE_FLOOR; // 🌟 타입 지정 (알아서 늘어남)
		m_Prefabs[id].walls.push_back(w);//m_Prefabs[id].walls.push_back(rc);
	}
}

void MapManager::CreateRandomMap()
{
	bool bossPlaced = false;
	int mapGenAttempts = 0;

	while (!bossPlaced && mapGenAttempts < 50)
	{
		mapGenAttempts++;

		for (int i = 1; i < 40; i++) {
			for (int j = 0; j < 5; j++) m_MapList[i].nextMapID[j] = 0;
			m_MapList[i].prefabID = 0;
		}
		for (int y = 0; y < 6; y++) for (int x = 0; x < 6; x++) m_Grid[y][x] = 0;

		m_MapList[1].id = 1;
		m_MapList[1].prefabID = 1; // 시작방은 1x1 1번 프리팹 사용
		m_MapList[1].width = m_Prefabs[1].width;
		m_MapList[1].height = m_Prefabs[1].height;
		m_MapList[1].layerCount = m_Prefabs[1].layerCount;
		m_Grid[3][0] = 1;

		for (int i = 0; i < m_Prefabs[1].layerCount; i++) m_MapList[1].bgLayer[i] = m_Prefabs[1].bgLayer[i];

		struct OpenDoor { int rID, dir; };
		std::vector<OpenDoor> q;
		q.push_back({ 1, DIR_RIGHT });

		int currentMapCount = 1;
		int failCount = 0;

		while (!q.empty() && failCount < 1000 && currentMapCount < 32)
		{
			failCount++;
			int qIdx = rand() % q.size();
			OpenDoor d = q[qIdx];
			q.erase(q.begin() + qIdx);

			if (m_MapList[d.rID].nextMapID[d.dir] != 0) continue;

			int rx = -1, ry = -1;
			for (int y = 0; y < 6; y++) {
				for (int x = 0; x < 6; x++) {
					if (m_Grid[y][x] == d.rID) { rx = x; ry = y; break; }
				}
				if (rx != -1) break;
			}
			if (rx == -1) continue;

			// =======================================================
			// 보스 대기실(6)일 경우 1번 프리팹(1x1)의 규격을 빌려 쓰도록 방어!
			// =======================================================

			int gw_old = m_Prefabs[m_MapList[d.rID].prefabID].gridW;
			int gh_old = m_Prefabs[m_MapList[d.rID].prefabID].gridH;

			std::vector<int> validPrefabs;
			bool forceBoss = false;

			int testTargetX = rx;
			if (d.dir == DIR_RIGHT) testTargetX = rx + gw_old;
			else if (d.dir == DIR_LEFT) testTargetX = rx - 1;

			// X좌표가 5 이상 다다르면, 보스 대기실 확정!
			if (!bossPlaced && testTargetX == 5 && d.dir == DIR_RIGHT) {
				// 안전장치: 혹시라도 그 자리에 이미 방이 있다면 덮어쓰지 않음
				if (m_Grid[ry][5] == 0) {
					validPrefabs.push_back(6);
					forceBoss = true;
				}
			}
			else {
				// 무조건 남는 공간이 있으면 프리팹(1~5번) 투입!
				for (int newID = 1; newID <= 5; newID++) {
					int gw_new = m_Prefabs[newID].gridW;
					int gh_new = m_Prefabs[newID].gridH;

					int targetX = rx, targetY = ry;
					if (d.dir == DIR_UP) { targetY = ry - gh_new; targetX = rx; }
					if (d.dir == DIR_DOWN) { targetY = ry + gh_old; targetX = rx; }
					if (d.dir == DIR_LEFT) { targetX = rx - gw_new; targetY = ry; }
					if (d.dir == DIR_RIGHT) { targetX = rx + gw_old; targetY = ry; }

					if (targetX < 0 || targetX + gw_new > 6 || targetY < 0 || targetY + gh_new > 6) continue;

					bool spaceFree = true;
					for (int y = 0; y < gh_new; y++) {
						for (int x = 0; x < gw_new; x++) {
							if (m_Grid[targetY + y][targetX + x] != 0) spaceFree = false;
						}
					}
					if (spaceFree) validPrefabs.push_back(newID);
				}
			}

			if (validPrefabs.empty()) continue;

			int newPrefabID = validPrefabs[rand() % validPrefabs.size()];
			int gw_new = m_Prefabs[newPrefabID].gridW;
			int gh_new = m_Prefabs[newPrefabID].gridH;

			int targetX = rx, targetY = ry;
			if (d.dir == DIR_UP) { targetY = ry - gh_new; targetX = rx; }
			if (d.dir == DIR_DOWN) { targetY = ry + gh_old; targetX = rx; }
			if (d.dir == DIR_LEFT) { targetX = rx - gw_new; targetY = ry; }
			if (d.dir == DIR_RIGHT) { targetX = rx + gw_old; targetY = ry; }

			currentMapCount++;
			int newRoomID = currentMapCount;

			for (int y = 0; y < gh_new; y++) {
				for (int x = 0; x < gw_new; x++) {
					m_Grid[targetY + y][targetX + x] = newRoomID;
				}
			}

			m_MapList[newRoomID].id = newRoomID;
			m_MapList[newRoomID].prefabID = newPrefabID;
			m_MapList[newRoomID].width = m_Prefabs[newPrefabID].width;
			m_MapList[newRoomID].height = m_Prefabs[newPrefabID].height;
			m_MapList[newRoomID].layerCount = m_Prefabs[newPrefabID].layerCount;
			for (int i = 0; i < m_Prefabs[newPrefabID].layerCount; i++) {
				m_MapList[newRoomID].bgLayer[i] = m_Prefabs[newPrefabID].bgLayer[i];
			}

			// 무조건 사방에 뚫을 수 있는 문을 큐에 던져넣음
			int dirs[4] = { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };
			for (int checkD : dirs) {
				int adjX = targetX, adjY = targetY;
				if (checkD == DIR_UP) adjY = targetY - 1;
				if (checkD == DIR_DOWN) adjY = targetY + gh_new;
				if (checkD == DIR_LEFT) adjX = targetX - 1;
				if (checkD == DIR_RIGHT) adjX = targetX + gw_new;

				if (adjX >= 0 && adjX < 6 && adjY >= 0 && adjY < 6 && m_Grid[adjY][adjX] != 0) {
					int adjRoom = m_Grid[adjY][adjX];
					int adjOppDir = 0;
					if (checkD == DIR_UP) adjOppDir = DIR_DOWN;
					if (checkD == DIR_DOWN) adjOppDir = DIR_UP;
					if (checkD == DIR_LEFT) adjOppDir = DIR_RIGHT;
					if (checkD == DIR_RIGHT) adjOppDir = DIR_LEFT;

					m_MapList[newRoomID].nextMapID[checkD] = adjRoom;
					m_MapList[adjRoom].nextMapID[adjOppDir] = newRoomID;
				}
				else {
					q.push_back({ newRoomID, checkD });
				}
			}

			if (forceBoss) { bossPlaced = true; break; }
		}

		m_MapList[39].id = 39;
		m_MapList[39].prefabID = ROOM_BOSS;
		m_MapList[39].width = m_Prefabs[ROOM_BOSS].width;
		m_MapList[39].height = m_Prefabs[ROOM_BOSS].height;
		m_MapList[39].layerCount = m_Prefabs[ROOM_BOSS].layerCount;
		for (int i = 0; i < m_Prefabs[ROOM_BOSS].layerCount; i++) {
			m_MapList[39].bgLayer[i] = m_Prefabs[ROOM_BOSS].bgLayer[i];
		}

		for (int y = 0; y < 6; y++) for (int x = 0; x < 6; x++) { m_DoorRight[y][x] = false; m_DoorDown[y][x] = false; }
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				int roomA = m_Grid[y][x];
				if (roomA == 0) continue;
				if (x < 5 && m_Grid[y][x + 1] != 0 && roomA != m_Grid[y][x + 1]) {
					if (m_MapList[roomA].nextMapID[DIR_RIGHT] == m_Grid[y][x + 1]) m_DoorRight[y][x] = true;
				}
				if (y < 5 && m_Grid[y + 1][x] != 0 && roomA != m_Grid[y + 1][x]) {
					if (m_MapList[roomA].nextMapID[DIR_DOWN] == m_Grid[y + 1][x]) m_DoorDown[y][x] = true;
				}
			}
		}
	}
}

void MapManager::ChangeMap(int mapID)
{
	if (mapID!=1)
	{
		SOUND->PlayBGM(SND_BGM_FIGHT);
	}
	// 맵을 36개까지 쓰기로 했으니, 제한을 40으로
	if (mapID < 1 || mapID >= 40) return;
	// =======================================================
	// 맵을 떠나기 전, 현재 살아있는 몬스터들의 반복 소리(날개짓 등)를 모두 끕니다!
	// =======================================================
	for (auto e : m_Enemies)
	{
		// Enemy 클래스에 만들어둔 StopMonsterSound() 호출
		e->StopMonsterSound();
	}
	// =======================================================
	// 방을 나가기 전, 현재 "죽어있는 몬스터"들의 위치를 기록!
	// =======================================================
	if (m_pCurrentMapChunk != nullptr) // 첫 시작이 아닐 때만
	{
		for (auto e : m_Enemies)
		{
			// 이미 죽은 놈이라면?
			if (e->isDead) {
				CorpseInfo info;
				info.x = e->pos.x; // 바닥에 떨어진 최종 좌표
				info.y = e->pos.y;
				info.dir = e->dir;

				// 명부에 등록 (이미 있으면 위치 갱신)
				m_CorpseRegistry[e->m_ID] = info;
			}
		}
	}

	// 맵 바뀔 때 타임 엔진 초기화!
	TIMEMGR->ResetTime();

	m_pCurrentMapChunk = &m_MapList[mapID];

	CAM->SetMapSize(m_pCurrentMapChunk->width, m_pCurrentMapChunk->height);

	// [필수] 맵 바뀔 때 상태 리셋
	knight.isDashing = false;
	knight.gravity = 0;
	knight.isMove = false;
	coll.ClearWalls();

	// 동적 벽 리스트도 초기화!
	m_DynamicWalls.clear();

	int pID = m_pCurrentMapChunk->prefabID;

	WallInfo w; // 임시 변수 w

	// 혹시 에러로 프리팹이 배정 안 된 방이라면? (안전장치)
	if (pID == 0 || m_Prefabs[pID].walls.empty())
	{
		SetRect(&w.rc, 0, m_pCurrentMapChunk->height - 100, m_pCurrentMapChunk->width, m_pCurrentMapChunk->height + 50);
		coll.AddWall(w.rc);
		return;
	}

	// WallInfo에서 rc(RECT)만 쏙 빼서 물리 엔진에 전달
	if (pID != 0 && !m_Prefabs[pID].walls.empty())
	{
		for (auto& w : m_Prefabs[pID].walls)
		{
			coll.AddWall(w.rc);
		}
	}

	// =================================================================
	// 고급 도어 소켓 캡핑 엔진 (화면에 보이게 두께 30px 할당 & 구멍 뚫기)
	// =================================================================
	int currentRoomID = m_pCurrentMapChunk->id;
	int gw = m_Prefabs[pID].gridW;
	int gh = m_Prefabs[pID].gridH;
	int MW = SCREEN_WITH;
	int MH = SCREEN_HEIGHT;

	// 보스방(39번)은 그리드 바깥에 있으므로, 자동 캡핑을 무시하고 통짜 사방 벽을 세워줍니다!
	if (currentRoomID == 39)
	{
		SetRect(&w.rc, -50, -50, MW * 2 + 50, 30); w.type = TILE_CEILING;
		m_DynamicWalls.push_back(w); coll.AddWall(w.rc);

		SetRect(&w.rc, -50, MH - 30, MW * 2 + 50, MH + 30); w.type = TILE_FLOOR;
		m_DynamicWalls.push_back(w);coll.AddWall(w.rc);
		SetRect(&w.rc, -30, -50, 30, MH + 50); w.type = TILE_WALL_L;
		m_DynamicWalls.push_back(w);coll.AddWall(w.rc);
		SetRect(&w.rc, MW * 2 - 30, -50, MW * 2 + 30, MH + 50); w.type = TILE_WALL_R;
		m_DynamicWalls.push_back(w);coll.AddWall(w.rc);
	}
	else if (currentRoomID == 38)
	{
		SetRect(&w.rc, -50, -50, gw * MW + 50, 30); w.type = TILE_CEILING;
		m_DynamicWalls.push_back(w); coll.AddWall(w.rc); // 꽉 막힌 천장
		SetRect(&w.rc, -50, gh * MH - 30, gw * MW + 50, gh * MH + 30); w.type = TILE_FLOOR;
		m_DynamicWalls.push_back(w); coll.AddWall(w.rc); // 꽉 막힌 바닥
		SetRect(&w.rc, -30, -50, 30, gh * MH + 50);  w.type = TILE_WALL_L;
		m_DynamicWalls.push_back(w); coll.AddWall(w.rc); // 꽉 막힌 좌측 벽
		SetRect(&w.rc, gw * MW - 30, -50, gw * MW + 30, gh * MH + 50); w.type = TILE_WALL_R;
		m_DynamicWalls.push_back(w); coll.AddWall(w.rc); // 꽉 막힌 우측 벽
	}
	else 
	{
		int rootX = GetRoomGridX(currentRoomID);
		int rootY = GetRoomGridY(currentRoomID);

		for (int y = 0; y < gh; y++)
		{
			for (int x = 0; x < gw; x++)
			{
				int cx = rootX + x;
				int cy = rootY + y;
				float px = x * MW;
				float py = y * MH;

				// ⬆️ 윗면 막기 (천장)
				if (cy == 0 || (m_Grid[cy - 1][cx] != currentRoomID && !m_DoorDown[cy - 1][cx])) {
					SetRect(&w.rc, px - 50, py, px + MW + 50, py + 30);w.type = TILE_CEILING;
					m_DynamicWalls.push_back(w); coll.AddWall(w.rc); // 완전 막힘
				}
				else if (cy > 0 && m_Grid[cy - 1][cx] != currentRoomID && m_DoorDown[cy - 1][cx]) {
					SetRect(&w.rc, px - 50, py, px + 490, py + 30); w.type = TILE_CEILING;
					m_DynamicWalls.push_back(w);coll.AddWall(w.rc);     // 뚫림 (좌측 천장)
					SetRect(&w.rc, px + 790, py, px + MW + 50, py + 30); w.type = TILE_CEILING;
					m_DynamicWalls.push_back(w);coll.AddWall(w.rc);// 뚫림 (우측 천장)
				}

				// ⬇️ 아랫면 막기 (바닥)
				if (cy == 5 || (m_Grid[cy + 1][cx] != currentRoomID && !m_DoorDown[cy][cx])) {
					SetRect(&w.rc, px - 50, py + MH - 30, px + MW + 50, py + MH + 30); w.type = TILE_FLOOR;
					m_DynamicWalls.push_back(w);coll.AddWall(w.rc);
				}
				else if (cy < 5 && m_Grid[cy + 1][cx] != currentRoomID && m_DoorDown[cy][cx]) {
					SetRect(&w.rc, px - 50, py + MH - 30, px + 490, py + MH + 30);w.type = TILE_FLOOR;
					m_DynamicWalls.push_back(w); coll.AddWall(w.rc);
					SetRect(&w.rc, px + 790, py + MH - 30, px + MW + 50, py + MH + 30); w.type = TILE_FLOOR;
					m_DynamicWalls.push_back(w);coll.AddWall(w.rc);
				}

				// ⬅️ 좌측면 막기
				if (cx == 0 || (m_Grid[cy][cx - 1] != currentRoomID && !m_DoorRight[cy][cx - 1])) {
					SetRect(&w.rc, px - 30, py, px + 30, py + MH); w.type = TILE_WALL_L;
					m_DynamicWalls.push_back(w);coll.AddWall(w.rc);
				}
				else if (cx > 0 && m_Grid[cy][cx - 1] != currentRoomID && m_DoorRight[cy][cx - 1]) {
					SetRect(&w.rc, px - 30, py - 50, px + 30, py + 450); w.type = TILE_WALL_L;
					m_DynamicWalls.push_back(w);coll.AddWall(w.rc); // 뚫림 (문 위쪽 벽)
					SetRect(&w.rc, px - 30, py + 800, px + 30, py + MH + 50); w.type = TILE_WALL_L;
					m_DynamicWalls.push_back(w);coll.AddWall(w.rc); // 문 아래쪽 벽
				}

				// ➡️ 우측면 막기
				if (cx == 5 || (m_Grid[cy][cx + 1] != currentRoomID && !m_DoorRight[cy][cx])) {
					SetRect(&w.rc, px + MW - 30, py, px + MW + 30, py + MH); w.type = TILE_WALL_R;
					m_DynamicWalls.push_back(w); coll.AddWall(w.rc);//coll.AddWall(rc);
				}
				else if (cx < 5 && m_Grid[cy][cx + 1] != currentRoomID && m_DoorRight[cy][cx]) {
					SetRect(&w.rc, px + MW - 30, py - 50, px + MW + 30, py + 450); w.type = TILE_WALL_R;
					m_DynamicWalls.push_back(w); coll.AddWall(w.rc);//coll.AddWall(rc);
					SetRect(&w.rc, px + MW - 30, py + 800, px + MW + 30, py + MH + 50); w.type = TILE_WALL_R;
					m_DynamicWalls.push_back(w); coll.AddWall(w.rc);//coll.AddWall(rc);
				}
			}
		}
	}
	
	// 이전 맵 적들 메모리 정리
	for (auto e : m_Enemies) delete e;
	m_Enemies.clear();
	
	// =================================================================
	// 진짜 보스방(39번) 진입 시: 감시자의 기사 3형제 스폰 & 컷신 시작!
	// =================================================================
	if (currentRoomID == 39||(currentRoomID == 38)&&pID==ROOM_BOSS)
	{
		SOUND->PlayBGM(SND_BGM_BOSS);
		// 렌더링 순서(Z-Order)를 위해 배경 쪽에 있는 애들부터 먼저 스폰(Draw)합니다.
		knight.pos.x = 1000.0f;
		// 1. 왼쪽 보스 (2번, 잠듦)
		BossEnemy* b2 = new BossEnemy(2);
		b2->Init(800.0f, m_pCurrentMapChunk->height - 200.0f);
		b2->ChangeState(B_STATE_SLEEP);
		m_Enemies.push_back(b2);

		// 2. 오른쪽 보스 (3번, 잠듦)
		BossEnemy* b3 = new BossEnemy(3);
		b3->Init(2000.0f, m_pCurrentMapChunk->height - 200.0f);
		b3->ChangeState(B_STATE_SLEEP);
		m_Enemies.push_back(b3);

		// 3. 가운데 보스 (1번, 깨어남!) -> 가장 마지막에 그려서 화면 맨 앞에 오게 함
		BossEnemy* b1 = new BossEnemy(1);
		b1->Init(1400.0f, m_pCurrentMapChunk->height - 200.0f);
		b1->ChangeState(B_STATE_AWAKE_ROAR);
		m_Enemies.push_back(b1);

		// 기사 컷신 모드 돌입 (키보드 조작 불가)
		knight.isCutscene = true;
	}

	// 방 크기에 비례하여 다이나믹 몬스터 스폰!
	if (pID != ROOM_BOSS && currentRoomID != 1) // 보스방과 처음 시작방(1번방)은 몬스터 생성 X
	{
		int gw = m_Prefabs[pID].gridW;
		int gh = m_Prefabs[pID].gridH;

		// 고유 ID 생성을 위한 카운터 (방 번호 * 1000 + 순서)
		int spawnCounter = 0;
		int roomBaseID = currentRoomID * 1000;

		// 1. 지상 몹 (가로 칸 수당 2마리씩)
		//for (int i = 0; i < gw * 2; i++) {
		//	Enemy* g = new GroundEnemy();
		//	// 가로로 띄엄띄엄 배치, 높이는 제일 아래쪽 바닥 근처
		//	g->Init(400.0f + (i * 600.0f), (gh * SCREEN_HEIGHT) - 150.0f);
		//	m_Enemies.push_back(g);
		//}

		// 1. 지상 몹 루프
		for (int i = 0; i < gw * 2; i++) 
		{
			spawnCounter++;
			int myID = roomBaseID + spawnCounter; // 예: 3번방의 1번째 몹 = 3001

			Enemy* g = new GroundEnemy();

			// 이 ID가 사망자 명부에 있는가?
			if (m_CorpseRegistry.find(myID) != m_CorpseRegistry.end())
			{
				// 명부에 있다 = 이미 죽은 놈이다 -> 시체 위치로 소환!
				CorpseInfo info = m_CorpseRegistry[myID];
				g->Init(info.x, info.y); // 죽었던 그 자리 그대로!
				g->SetCorpse();          // 태어나자마자 죽은 상태로
				g->dir = info.dir;
			}
			else
			{
				// 명부에 없다 = 살아있다 -> 원래 리젠 위치에 소환
				g->Init(400.0f + (i * 600.0f), (gh * SCREEN_HEIGHT) - 150.0f);
			}

			g->m_ID = myID; // ID 부여
			m_Enemies.push_back(g);
		}

		// 2. 비행 몹 (세로 칸 수당 1마리씩)
		//for (int i = 0; i < gh; i++) {
		//	Enemy* f = new FlyEnemy();
		//	// 세로로 띄엄띄엄 공중에 배치
		//	f->Init(600.0f, 200.0f + (i * 800.0f));
		//	m_Enemies.push_back(f);
		//}

		// 2. 비행 몹 루프 (똑같이 적용)
		for (int i = 0; i < gh; i++) {
			spawnCounter++;
			int myID = roomBaseID + spawnCounter; // 예: 3003

			Enemy* f = new FlyEnemy();

			if (m_CorpseRegistry.find(myID) != m_CorpseRegistry.end())
			{
				CorpseInfo info = m_CorpseRegistry[myID];
				f->Init(info.x, info.y);
				f->SetCorpse();
				f->dir = info.dir;
			}
			else
			{
				f->Init(600.0f, 200.0f + (i * 800.0f));
			}

			f->m_ID = myID;
			m_Enemies.push_back(f);
		}
	}
}

void MapManager::Update(double frame)
{
	if (m_pCurrentMapChunk == nullptr) return;

	int MW = m_pCurrentMapChunk->width;
	int MH = m_pCurrentMapChunk->height;
	int rX = GetRoomGridX(m_pCurrentMapChunk->id);
	int rY = GetRoomGridY(m_pCurrentMapChunk->id);

	// 기사가 방의 어느 '칸(Cell)'에 있는지 정확히 계산
	float clampX = knight.pos.x; if (clampX < 0) clampX = 0; if (clampX >= MW) clampX = MW - 1;
	float clampY = knight.pos.y; if (clampY < 0) clampY = 0; if (clampY >= MH) clampY = MH - 1;

	int localX = (int)(clampX / SCREEN_WITH);
	int localY = (int)(clampY / SCREEN_HEIGHT);
	int gx = rX + localX; // 현재 있는 절대 그리드 X
	int gy = rY + localY; // 현재 있는 절대 그리드 Y

	// 해당 칸 안에서의 세부 픽셀 위치
	float offsetX = clampX - (localX * SCREEN_WITH);
	float offsetY = clampY - (localY * SCREEN_HEIGHT);

	// 1. 오른쪽으로 나갈 때
	if (knight.pos.x >= MW) {
		if (gx < 5 && m_DoorRight[gy][gx]) { // 오른쪽으로 뚫린 문이 있다면?
			int nextMap = m_Grid[gy][gx + 1];
			ChangeMap(nextMap);
			int new_rY = GetRoomGridY(nextMap);
			knight.pos.x = 50.0f;
			knight.pos.y = (gy - new_rY) * SCREEN_HEIGHT + offsetY; // 🌟 들어간 높이 그대로 튀어나옴!
			return;
		}
		else knight.pos.x = MW - 1; // 막혀있으면 튕겨냄
	}
	// 2. 왼쪽으로 나갈 때
	else if (knight.pos.x <= 0) {
		if (gx > 0 && m_DoorRight[gy][gx - 1]) {
			int nextMap = m_Grid[gy][gx - 1];
			ChangeMap(nextMap);
			int new_rY = GetRoomGridY(nextMap);
			knight.pos.x = m_pCurrentMapChunk->width - 50.0f;
			knight.pos.y = (gy - new_rY) * SCREEN_HEIGHT + offsetY;
			return;
		}
		else knight.pos.x = 1;
	}
	// 3. 위로 올라갈 때
	else if (knight.pos.y <= 0) {
		if (gy > 0 && m_DoorDown[gy - 1][gx]) {
			int nextMap = m_Grid[gy - 1][gx];
			ChangeMap(nextMap);
			int new_rX = GetRoomGridX(nextMap);
			knight.pos.x = (gx - new_rX) * SCREEN_WITH + offsetX; // 🌟 올라간 폭 그대로 튀어나옴!
			knight.pos.y = m_pCurrentMapChunk->height - 150.0f;
			knight.gravity = -12.0f;
			return;
		}
		else knight.pos.y = 1;
	}
	// 4. 아래로 떨어질 때
	else if (knight.pos.y >= MH) {
		if (gy < 5 && m_DoorDown[gy][gx]) {
			int nextMap = m_Grid[gy + 1][gx];
			ChangeMap(nextMap);
			int new_rX = GetRoomGridX(nextMap);
			knight.pos.x = (gx - new_rX) * SCREEN_WITH + offsetX;
			knight.pos.y = 50.0f;
			return;
		}
		else {
			knight.pos.y = MH - 100.0f; knight.grounded = true;
		}
	}

	// ==========================================================
	// 5. 보스 대기실 텔레포트 상호작용
	// ==========================================================
	if (m_pCurrentMapChunk->prefabID == 6) // 현재 방이 6번(보스 대기실)일 때만 작동!
	{
		// 🌟 키를 누르고 있는 시간을 측정할 타이머 변수 (static으로 선언해 프레임이 지나도 기억하게 함)
		static DWORD teleportHoldTime = 0;

		// 기사가 맵의 중앙(MW / 2) 부근 제단 위에 서 있는지 확인 (좌우 100픽셀 여유)
		if (knight.pos.x >= MW / 2.0f - 100.0f && knight.pos.x <= MW / 2.0f + 100.0f)
		{
			// 기사가 땅에 서 있고(grounded), 윗방향키(isLookup)를 눌렀다면 텔레포트 발동!
			if (knight.grounded && knight.isLookup)
			{
				if (teleportHoldTime == 0) {
					teleportHoldTime = GetTickCount();
				}
				// 3. 누른 상태로 1초(1000ms)가 지났다면? 텔레포트 발동!
				else if (GetTickCount() - teleportHoldTime >= 1000)
				{
					teleportHoldTime = 0; // 타이머 초기화

					ChangeMap(39); // 진짜 보스방(39번)으로 전환!

					int bossRoomID = 16;
					knight.pos.x = 1000.0f;
					knight.pos.y = m_Prefabs[bossRoomID].height - 250.0f;

					return; // 맵이 바뀌었으니 이번 프레임 Update 즉시 종료
				}
				//ChangeMap(39); // 미리 준비해둔 39번 맵(진짜 보스방)으로 전환!

				//int bossRoomID = 16;

				//// 진짜 보스방(16번)의 정중앙 공중에 스폰시켜서 멋지게 떨어지도록 연출
				//knight.pos.x = 1000.0f;
				//knight.pos.y = m_Prefabs[bossRoomID].height - 250.0f;

				//return; // 맵이 바뀌었으니 이번 프레임 Update 즉시 종료
			}
		}
		else
		{
			teleportHoldTime = 0; // 제단에서 벗어나면 타이머 초기화
		}
	}

	// 적 업데이트 및 전투(충돌) 처리
	for (auto it = m_Enemies.begin(); it != m_Enemies.end(); ) {
		Enemy* e = *it;
		e->Update();

		if (!e->isDead) {
			RECT temp;
			// 1. 기사가 적을 때림! (공격 히트박스 vs 적 몸체)
			if (knight.isAttacking && !knight.isAttackHit) {
				if (e->IsTargetable()&&IntersectRect(&temp, &knight.attackBox, &e->m_rc)) {
					knight.isAttackHit = true;
					e->TakeDamage(1, knight.dir == 1 ? -1 : 1); // 때린 방향으로 넉백

					// 기사가 때리면 0.04초 정지 (손맛)
					TIMEMGR->SetHitStop(40);
					CAM->Shake(8.0f, 100); //카메라 흔들림 효과 (강도 8, 지속시간 0.1초)

					// =======================================================
					// 충돌 교집합(temp)의 중심점에서 이펙트 생성!
					// =======================================================
					float hitX = (float)(temp.left + temp.right) / 2.0f;
					float hitY = (float)(temp.top + temp.bottom) / 2.0f;
					// 랜덤성 추가 (상처가 한 곳에만 나면 재미없으니 +- 10픽셀 정도 흩뿌리기)
					hitX += (rand() % 20) - 10;
					hitY += (rand() % 20) - 10;

					EFFECT->Play(EF_SPLASH, hitX, hitY, knight.dir);

					// 방향에 따른 이펙트 타입 및 좌표 계산 후 스폰
					float eX = knight.pos.x + (knight.dir == 1 ? -60 : 60);
					if (knight.attackType == 0) EFFECT->Play(EF_HIT, eX, knight.pos.y - 30, knight.dir == 1 ? 1 : -1);
					else if (knight.attackType == 1) EFFECT->Play(EF_HIT_UPDOWN, knight.pos.x, knight.pos.y - 80, knight.dir, -1.0f);
					else if (knight.attackType == 2) EFFECT->Play(EF_HIT_UPDOWN, knight.pos.x, knight.pos.y + 50, knight.dir, 1.0f);

					//float eX = e->pos.x;
					//float eY = e->pos.y - 20.0f; // 적 몸통 중간쯤
					//EFFECT->Play(EF_SPLASH, e->pos.x, e->pos.y - 20.0f, knight.dir);

					if (knight.attackType == 2) { // 하단 찍기 포고 점프!
						knight.gravity = -13.0f;
						// 강제로 공중 판정을 주고, 바닥에서 5픽셀 강제로 뜯어냅니다! (끼임 방지)
						knight.grounded = false;
						knight.pos.y -= 5.0f;
					}
				}
			}

			// 2. 적이 기사를 때림! (몸통 박치기 & 보스 무기 공격)
			if (!knight.isInvincible) {
				bool isHitByBoss = false;

				// A. 보스의 무기 공격(attackBox)에 맞았는가?
				if (e->type == 3) {
					BossEnemy* b = (BossEnemy*)e;
					if (b->isAttacking && IntersectRect(&temp, &knight.m_rc, &b->attackBox)) {
						isHitByBoss = true;
					}
				}

				// B. 일반적인 몸통 박치기에 맞았는가?
				if (e->CanDealDamage() && IntersectRect(&temp, &knight.m_rc, &e->m_rc)) {
					isHitByBoss = true;

					//// 돌진 중인 보스와 부딪히면 보스를 강제로 멈추게 함!
					//if (e->type == 3) {
					//	BossEnemy* b = (BossEnemy*)e;
					//	if (b->state == B_STATE_ROLL_DASH) {
					//		b->ChangeState(B_STATE_IDLE); // 보스 돌진 정지!
					//		b->velocity.x = (b->dir == 1) ? 10.0f : -10.0f; // 살짝 튕겨남
					//	}
					//}
				}

				// 맞았다면 데미지 적용!
				if (isHitByBoss) {
					int pushDir = (knight.pos.x < e->pos.x) ? -1 : 1;
					//knight.TakeDamage(1, pushDir);
					if (knight.TakeDamage(1, pushDir))
					{
						// 진짜로 맞았을 때만 시간이 멈추고 이펙트가 나옴 -> "드르르륵" 완전 해결!
						TIMEMGR->SetHitStop(150);
						CAM->Shake(15.0f, 200); //(카메라 쉐이크 있다면)

						EFFECT->Play(EF_STUN, knight.pos.x, knight.pos.y, -pushDir);
					}
					//// 기사가 맞으면 0.15초 뼈아픈 정지
					//TIMEMGR->SetHitStop(150);

					//CAM->Shake(15.0f, 200); // 세게 맞았으니 더 강하게(강도 15) 흔들림!
					//EFFECT->Play(EF_STUN, knight.pos.x, knight.pos.y, -pushDir); // 기사 피격 이펙트
				}
			}
		}

		// (옵션) 시체가 된 지 3초 후 삭제
		/*if (e->isDead && (GetTickCount() - e->hitStartTime > 3000)) {
			delete e;
			it = m_Enemies.erase(it);
		}
		else {
			++it;
		}*/
		++it;
	}

	// =======================================================
	// 다중 보스전 (페이즈) 관리 시스템
	// =======================================================
	if (m_pCurrentMapChunk->prefabID == 16 || m_pCurrentMapChunk->id == 39) // 보스방일 때만
	{
		BossEnemy* b1 = nullptr;
		BossEnemy* b2 = nullptr;
		BossEnemy* b3 = nullptr;

		// 방에 있는 보스 3형제 정보 수집
		for (auto e : m_Enemies) {
			if (e->type == 3) {
				BossEnemy* b = (BossEnemy*)e;
				if (b->bossID == 1) b1 = b;
				else if (b->bossID == 2) b2 = b;
				else if (b->bossID == 3) b3 = b;
			}
		}

		// 페이즈 2: 1번 보스의 체력이 반피(10) 이하이고, 2번 보스가 자고 있다면 기상!
		if (b1 && b1->hp <= BOSS_HP/2 && b2 && b2->state == B_STATE_SLEEP) {
			b2->ChangeState(B_STATE_AWAKE_ROAR);
		}

		// 페이즈 3: 1번과 2번 중 아무나 한 명이라도 죽었을 때, 3번 보스가 자고 있다면 기상!
		bool isAnyBossDead = (b1 && b1->state == B_STATE_DIE) || (b2 && b2->state == B_STATE_DIE);

		if (isAnyBossDead && b3 && b3->state == B_STATE_SLEEP) {
			b3->ChangeState(B_STATE_AWAKE_ROAR);
		}

		// 모든 보스가 죽는 순간! (3초간 30% 속도로 슬로우 연출)
		if( (b3 && b3->state == B_STATE_DIE)&& (b1 && b1->state == B_STATE_DIE)&& (b2 && b2->state == B_STATE_DIE)){
			// 처음 죽은 순간에만 타이머 기록!
			if (m_GameClearTime == 0) {
				m_GameClearTime = GetTickCount();
				TIMEMGR->SetTimeSlow(0.3f, 3000); // 3초간 슬로우
			}
			SOUND->PlayBGM(SND_BGM_CLEAR);
		}
	}

	// 보스 죽고 4초 뒤 엔딩 화면으로 이동!
	if (m_GameClearTime != 0 && (GetTickCount() - m_GameClearTime > 4000))
	{
		g_Mng.n_Chap = ENDING; // 엔딩 씬으로 전환
		m_GameClearTime = 0;   // 중복 실행 방지
	}

	// =======================================================
	// 게임오버 씬(OVER) 전환 연출
	// =======================================================
	if (knight.isDead)
	{
		SOUND->PlayBGM(SND_EFF_HERO_DIE);
	}
	if (knight.isDead && (GetTickCount() - knight.realDeadTime > 2000)) {
		// 2초의 슬로우 모션 연출이 끝나면 게임오버 씬으로 넘깁니다!
		g_Mng.n_Chap = OVER; // (혹시 Define.h에 게임오버 씬 변수 이름이 OVER가 아니라면 맞게 수정해 주세요!)
	}

}

void MapManager::Draw()
{
	if (m_pCurrentMapChunk == nullptr) return;

	// 현재 맵의 모든 레이어 그리기
	for (int i = 0; i < m_pCurrentMapChunk->layerCount; i++)
	{
		// 카메라 좌표만큼 빼주기
		m_pCurrentMapChunk->bgLayer[i].Render(0 - CAM->GetX(), 0 - CAM->GetY(), 0, 1, 1);
	}

	// [타일 렌더링 시스템]
	int pID = m_pCurrentMapChunk->prefabID;
	if (pID > 0)
	{
		for (auto& w : m_Prefabs[pID].walls)
		{
			if (w.type == TILE_NONE) continue;

			RECT rc = w.rc;
			float colliderW = (float)(rc.right - rc.left);
			float colliderH = (float)(rc.bottom - rc.top);

			// =========================================================
			// ☁️ 2. 플랫폼 (Platform) - 스케일링 + 중앙 정렬 + 중앙 피벗 보정
			// =========================================================
			if (w.type == TILE_PLATFORM)
			{
				Sprite* img = &m_PlatformImage;
				float imgW = (float)img->imagesinfo.Width;
				float imgH = (float)img->imagesinfo.Height;

				// 스케일 계산 (콜라이더 너비에 맞춤)
				float scaleX = colliderW / imgW;

				// 🌟 좌표 보정 (Render가 Center 기준이므로, 우리가 원하는 위치의 '중앙'을 줘야 함)
				// X: 콜라이더의 정중앙
				float centerX = (rc.left + rc.right) / 2.0f;
				// Y: 콜라이더 윗면(Top) + 이미지 높이의 절반
				float centerY = rc.top + (imgH / 2.0f);

				img->Render(centerX - CAM->GetX(), centerY - CAM->GetY(), 0, scaleX, 1, 1.0f);
			}
			// =========================================================
			// 🧱 1. 바닥 (Floor) - 상단 정렬 + 반복
			// =========================================================
			else if (w.type == TILE_FLOOR)
			{
				int imgW = m_FloorImages[0].imagesinfo.Width;
				int imgH = m_FloorImages[0].imagesinfo.Height;
				int idx = 0;

				for (int x = rc.left; x < rc.right; x += imgW)
				{
					// X: 현재 x + 이미지 절반 (중앙 맞춤)
					// Y: 콜라이더 Top + 이미지 절반
					float drawX = x + (imgW / 2.0f);
					float drawY = rc.top + (imgH / 2.0f);

					m_FloorImages[idx].Render(drawX - CAM->GetX(), drawY - CAM->GetY(), 0, 1, 1, 1);
					idx = (idx + 1) % 3;
				}
			}
		}
	}
	// 🌟 [2] 동적 벽 그리기 (추가된 부분)
	// 프리팹 벽 그리는 로직과 완전히 똑같은 코드를 m_DynamicWalls에 대해 한 번 더 돌립니다.
	// (함수로 분리하면 더 깔끔하겠지만, 일단 복사-붙여넣기로 빠르게 적용합니다)
	for (auto& w : m_DynamicWalls)
	{
		if (w.type == TILE_NONE) continue;

		RECT rc = w.rc;
		float colliderW = (float)(rc.right - rc.left);
		float colliderH = (float)(rc.bottom - rc.top);

		// =========================================================
			// 🧱 1. 바닥 (Floor) - 상단 정렬 + 반복
			// =========================================================
		if (w.type == TILE_FLOOR)
		{
			int imgW = m_FloorImages[0].imagesinfo.Width;
			int imgH = m_FloorImages[0].imagesinfo.Height;
			int idx = 0;

			for (int x = rc.left; x < rc.right; x += imgW)
			{
				// X: 현재 x + 이미지 절반 (중앙 맞춤)
				// Y: 콜라이더 Top + 이미지 절반
				float drawX = x + (imgW / 2.0f);
				float drawY = rc.top + (imgH / 2.0f);

				m_FloorImages[idx].Render(drawX - CAM->GetX(), drawY - CAM->GetY(), 0, 1, 1, 1);
				idx = (idx + 1) % 3;
			}
		}
		// =========================================================
		// 🧗 3. 왼쪽 벽 (Wall L) - 우측 정렬
		// =========================================================
		else if (w.type == TILE_WALL_L)
		{
			int imgW = m_WallImage.imagesinfo.Width;
			int imgH = m_WallImage.imagesinfo.Height;

			// X: 콜라이더 Right - 이미지 절반 (그래야 오른쪽 끝이 맞음)
			float drawX = rc.right - (imgW / 2.0f);

			// [스마트 Y 정렬]
			// 윗 벽(Ceiling 붙음)은 밑에서 위로 그려야 문을 안 가림!

			if (rc.top <= 0) // 천장에 붙은 벽
			{
				// Bottom Align: rc.bottom에서 위로 올라감
				for (int y = rc.bottom; y > rc.top; y -= imgH) {
					float drawY = y - (imgH / 2.0f); // Bottom - 반높이
					m_WallImage.Render(drawX - CAM->GetX(), drawY - CAM->GetY(), 0, 1, 1, 1);
				}
			}
			else // 아랫 벽 (바닥에 붙음) or 일반 벽
			{
				// Top Align: rc.top에서 아래로 내려감
				for (int y = rc.top; y < rc.bottom; y += imgH) {
					float drawY = y + (imgH / 2.0f); // Top + 반높이
					m_WallImage.Render(drawX - CAM->GetX(), drawY - CAM->GetY(), 0, 1, 1, 1);
				}
			}
		}
		// =========================================================
		// 🧗 4. 오른쪽 벽 (Wall R) - 좌측 정렬 & 반전
		// =========================================================
		else if (w.type == TILE_WALL_R)
		{
			int imgW = m_WallImage.imagesinfo.Width;
			int imgH = m_WallImage.imagesinfo.Height;

			// X: 콜라이더 Left + 이미지 절반 (그래야 왼쪽 끝이 맞음)
			// (이미지를 뒤집어도 Pivot이 중앙이면 좌표는 동일하게 중앙을 가리켜야 함)
			float drawX = rc.left + (imgW / 2.0f);
			
			// [스마트 Y 정렬]
			// 윗 벽(Ceiling 붙음)은 밑에서 위로 그려야 문을 안 가림!
			if (rc.top <= 0) // 천장에 붙은 벽
			{
				// Bottom Align: rc.bottom에서 위로 올라감
				for (int y = rc.bottom; y > rc.top; y -= imgH) {
					float drawY = y - (imgH / 2.0f); // Bottom - 반높이
					m_WallImage.Render(drawX - CAM->GetX(), drawY - CAM->GetY(), 0, -1, 1, 1);
				}
			}
			else // 아랫 벽 (바닥에 붙음) or 일반 벽
			{
				// Top Align: rc.top에서 아래로 내려감
				for (int y = rc.top; y < rc.bottom; y += imgH) {
					float drawY = y + (imgH / 2.0f); // Top + 반높이
					m_WallImage.Render(drawX - CAM->GetX(), drawY - CAM->GetY(), 0, -1, 1, 1);
				}
			}
		}
		// =========================================================
		// 🦇 5. 천장 (Ceiling) - 하단 정렬
		// =========================================================
		else if (w.type == TILE_CEILING)
		{
			int imgW = m_CeilingImage.imagesinfo.Width;
			int imgH = m_CeilingImage.imagesinfo.Height;

			for (int x = rc.left; x < rc.right; x += imgW) {

				// 스케일 계산 (콜라이더 너비에 맞춤)
				float scaleX = colliderW / imgW;

				// 좌표 보정 (Render가 Center 기준이므로, 우리가 원하는 위치의 '중앙'을 줘야 함)
				// X: 콜라이더의 정중앙
				float drawX = (rc.left + rc.right) / 2.0f;
				// Y: 콜라이더 윗면(Top) + 이미지 높이의 절반
				float drawY = rc.top + (imgH / 2.0f);

				m_CeilingImage.Render(drawX - CAM->GetX(), drawY - CAM->GetY(), 0, scaleX, 1, 1);
			}
		}
	}
	// =======================================================
	// 1. 배경 쪽에 깔려야 할 '시체' 및 '수면 상태' 먼저 렌더링
	// =======================================================
	for (auto e : m_Enemies) {
		if (e->type == 3) {
			BossEnemy* b = (BossEnemy*)e;
			// 보스가 죽었거나(시체) 자고 있다면 먼저 그린다!
			if (b->state == B_STATE_DIE || b->state == B_STATE_SLEEP) e->Draw();
		}
		else if (e->isDead) {
			e->Draw(); // 일반 몹 시체
		}
	}

	// =======================================================
	// 2. 화면 앞쪽에 그려져야 할 '살아있는 적' 나중에 렌더링 (덮어쓰기)
	// =======================================================
	for (auto e : m_Enemies) {
		if (e->type == 3) {
			BossEnemy* b = (BossEnemy*)e;
			// 보스가 살아있고 깨어있다면 나중에 그려서 화면 맨 앞으로 뺀다!
			if (b->state != B_STATE_DIE && b->state != B_STATE_SLEEP) e->Draw();
		}
		else if (!e->isDead) {
			e->Draw(); // 일반 살아있는 몹
		}
	}

	// =======================================================
	// 기사가 죽었다면, 서서히 어두워지며 게임오버 로고 출력!
	// =======================================================
	if (knight.isDead)
	{
		// 1. 죽은 지 얼마나 지났는지 계산 (0 ~ 2000ms)
		// (주의: knight.realDeadTime은 GetTickCount() 기준이어야 합니다!)
		DWORD elapsed = GetTickCount() - knight.realDeadTime;

		// 2. 투명도(Alpha) 계산 (0초일 때 0 -> 2초일 때 255)
		int galpha = (int)((elapsed / 2000.0f) * 255);
		int alpha = galpha;
		if (galpha > 128) alpha = 128;

		// 3. 화면 전체를 덮는 반투명 검은 박스 그리기
		// (이미지를 화면 크기만큼 확대해서 그립니다)
		m_BlackScreen.SetColor(255, 255, 255, alpha);
		m_BlackScreen.Render(0, 0, 0, SCREEN_WITH, SCREEN_HEIGHT); // 스케일을 키워서 꽉 채움

		// 4. 투명도가 절반(128)을 넘으면 게임오버 글씨 등장!
		if (galpha > 120)
		{
			// 글씨도 서서히 나타나게 하려면: (alpha - 120) * 2 등을 활용
			int textAlpha = (galpha - 120) * 2;
			if (textAlpha > 255) textAlpha = 255;

			m_GameOverTitle.SetColor(255, 255, 255, textAlpha);
			// 화면 중앙에 배치
			m_GameOverTitle.Render((SCREEN_WITH - goimagesinfo.Width) / 2, 0, 0, 1, 1);
		}
	}

	// =======================================================
	// 디버그용 텍스트 출력 모음
	// =======================================================
	if (coll.isDebugDraw)
	{
		char debugPrefab[256];
		sprintf_s(debugPrefab, "Current Prefab ID : %d", m_pCurrentMapChunk->prefabID);
		dv_font.DrawString(debugPrefab, 0, 0, D3DCOLOR_ARGB(255, 255, 255, 0));

		char debugPos[256];
		sprintf_s(debugPos, "Knight Pos: X(%.1f), Y(%.1f)", knight.pos.x, knight.pos.y);
		dv_font.DrawString(debugPos, 0, 100, D3DCOLOR_ARGB(255, 0, 255, 255));

		// =======================================================
		// 보스 3형제 실시간 HP & 상태 스캐너!
		// =======================================================
		int textY = 400; // 화면 위쪽부터 아래로 출력
		for (auto e : m_Enemies)
		{
			if (e->type == 3) // 몬스터가 보스(type 3)일 경우
			{
				BossEnemy* b = (BossEnemy*)e;
				char bossInfo[256];

				// ID, 현재 체력, 현재 상태(Sleep, Roar 등)를 출력
				sprintf_s(bossInfo, "[Boss %d] HP : %d / State : %s", b->bossID, b->hp, GetStateName(b->state));

				// 1번(가운데)은 빨간색, 2번/3번(배경)은 회색빛으로 글씨 색깔도 센스있게!
				D3DCOLOR textColor = (b->bossID == 1) ? D3DCOLOR_ARGB(255, 255, 100, 100) : D3DCOLOR_ARGB(255, 150, 150, 150);

				dv_font.DrawString(bossInfo, 10, textY, textColor);
				textY += 30; // 다음 줄로 내림
			}
		}
	}
}

const char* MapManager::GetStateName(int state)
{
	switch (state)
	{
	case B_STATE_SLEEP:       return "SLEEP";
	case B_STATE_AWAKE_ROAR:  return "AWAKE_ROAR";
	case B_STATE_IDLE:        return "IDLE";
	case B_STATE_WALK:        return "WALK";
	case B_STATE_MELEE:       return "MELEE";
	case B_STATE_ROLL_DASH:   return "ROLL_DASH";
	case B_STATE_ROLL_BOUNCE: return "ROLL_BOUNCE";
	case B_STATE_ROLL_BACK:   return "ROLL_BACK";
	case B_STATE_DIE:         return "DIE";
	default:                  return "UNKNOWN";
	}
}

void MapManager::LoadDebugPrefab(int pID)
{
	// 1. 1~15번을 순환하도록 범위 제한
	if (pID < 1) pID = 16;
	if (pID > 16) pID = 1;

	// 혹시 아직 도면을 안 짠 프리팹이면 건너뛰기
	if (m_Prefabs[pID].typeID == 0) return;

	m_DebugPrefabID = pID;

	// =========================================================
	// 1번 방이 아닌, 38번 방(디버그 전용)을 사용합니다!
	// =========================================================
	int debugRoomID = 38;

	m_MapList[debugRoomID].id = debugRoomID;
	m_MapList[debugRoomID].prefabID = pID;
	m_MapList[debugRoomID].width = m_Prefabs[pID].width;
	m_MapList[debugRoomID].height = m_Prefabs[pID].height;
	m_MapList[debugRoomID].layerCount = m_Prefabs[pID].layerCount;

	for (int i = 0; i < m_Prefabs[pID].layerCount; i++)
	{
		m_MapList[debugRoomID].bgLayer[i] = m_Prefabs[pID].bgLayer[i];
	}

	// 3. 38번 방으로 즉시 이동!
	ChangeMap(debugRoomID);

	// 4. 캐릭터를 맵의 가로 중앙 & 맨 위(공중)로 안전하게 스폰
	if (pID == 16) // 불러온 프리팹이 보스방일 경우
	{
		knight.pos.x = 1000.0f;
		knight.pos.y = m_Prefabs[pID].height - 250.0f;
	}
	else
	{
		knight.pos.x = m_Prefabs[pID].width / 2.0f;
		knight.pos.y = 50.0f;
	}
	knight.gravity = 0;
	knight.isDashing = false;
	knight.grounded = false;
}