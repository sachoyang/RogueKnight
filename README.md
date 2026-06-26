# ⚔️ Hollow Knight Style 2D Action Platformer

**DirectX 9와 C++로 자체 제작한 2D 횡스크롤 액션 게임 프레임워크** 로그라이크 스타일의 절차적 맵 생성 시스템과 FSM 기반의 정교한 보스 AI 구현

---

## 📖 프로젝트 개요 (Overview)

이 프로젝트는 상용 게임 엔진(Unity, Unreal)을 배제하고, **C++ WinAPI와 DirectX 9 SDK만을 사용하여 바닥부터 설계한 2D 게임 프레임워크**입니다.

유명 인디 게임 *'Hollow Knight'*의 모작 프로젝트로, **절차적 맵 생성(Procedural Generation)**을 통한 로그라이크 요소와 **유한 상태 머신(FSM)** 기반의 전략적인 보스 전투를 구현하는 데 중점을 두었습니다.

---

## 🛠️ 기술 스택 (Tech Stack)

* **Language:** C++ (STL, WinAPI)
* **Graphics:** DirectX 9 (d3d9, d3dx9, Sprite Rendering)
* **Audio:** FMOD Sound System (Custom Wrapper)
* **IDE:** Visual Studio 2022
* **Data:** Custom Prefab Structure, Hardcoded Game Data

---

## 🏗️ 시스템 아키텍처 (System Architecture)

### 1. 메인 루프 및 코어 (Core System)

* **Main Loop:** `WinMain`에서 `PeekMessage`를 사용한 비동기 메시지 처리 방식을 채택하여 부드러운 게임 루프를 구현했습니다.
* **Manager Pattern:** 핵심 시스템을 **싱글톤(Singleton)**으로 설계하여 전역 접근성을 확보하고 리소스를 효율적으로 관리합니다.
    * **MapManager:** 맵 데이터(`MapChunk`) 관리, 랜덤 맵 생성, 맵 이동 처리.
    * **TimeManager:** `QueryPerformanceCounter`를 이용한 정밀한 델타 타임 계산 및 히트 스톱(Hit Stop) 효과 구현.
    * **Sound (FmodSound):** FMOD 라이브러리를 래핑하여 채널 관리, 볼륨 조절, 상태 기반 사운드 재생(State-Based Play) 지원.
    * **UIManager:** 체력바(HP), 미니맵, 상호작용 UI 렌더링.
    * **EffectManager:** 파티클 및 이펙트 객체의 생성과 소멸 관리 (Object Pooling).

### 2. 씬 관리 시스템 (Scene Management)

추상 클래스 `Chap`을 상속받아 각 장면을 모듈화하고, `GameManager`(`g_Mng`)에서 상태 변수(`n_Chap`)를 통해 흐름을 제어합니다.

* **Scene Flow:** `Logo` ➔ `Menu` ➔ `Game` (인게임) ➔ `Over` (사망) / `Ending` (클리어)
* **Overlay Pause:** 인게임(`Game.cpp`) 내에서 `Pause` 객체를 통해 별도의 업데이트 루프를 돌려, 게임 로직은 멈추고 UI만 작동하는 일시정지/옵션 오버레이를 구현했습니다.

---

## 💡 핵심 알고리즘 및 구현 상세 (Key Algorithms)

### A. 🗺️ 절차적 맵 생성 (Procedural Map Generation)

매 플레이마다 구조가 바뀌는 로그라이크 던전 생성 알고리즘입니다 (`MapManager.cpp`).

1.  **그리드 초기화:** 6x6 크기의 2차원 배열(`m_Grid`)을 생성합니다.
2.  **프리팹(Prefab) 정의:** 다양한 크기(1x1, 1x2 수직, 2x1 수평, 2x2 대형)의 방 데이터를 사전에 정의합니다.
3.  **랜덤 워크 & 확장 (Random Walk & Expansion):**
    * 시작 방(1번)을 배치하고 큐(Queue)에 '확장 가능한 문(Door)' 정보를 넣습니다.
    * 큐에서 문을 하나 꺼내어, 해당 방향에 맞는 프리팹을 랜덤하게 선택하여 배치를 시도합니다.
4.  **충돌 검사:** 배치하려는 공간이 비어있는지, 그리드 범위를 벗어나지 않는지 검사합니다.
5.  **보스방 배치 및 마감:**
    * 맵이 일정 범위(X=5)에 도달하면 보스 대기실(ID 6)과 보스방(ID 39)을 배치합니다.
    * 생성이 끝난 후, 연결되지 않은 문은 `m_DynamicWalls`를 생성하여 자동으로 벽을 막아 맵 밖으로의 추락을 방지합니다.

### B. 🤖 다이내믹 보스 AI (FSM - Finite State Machine)

보스 **'Watcher'**는 단순 추적이 아닌, 상황에 따라 행동을 결정하는 상태 머신으로 구현되었습니다 (`Boss.cpp`).

* **State Pattern:** `switch(state)`문을 통해 행동 로직을 분리하여 관리합니다.
    * `SLEEP` / `AWAKE`: 플레이어 조우 전 대기 및 등장 연출.
    * `IDLE` (탐색): 플레이어와의 거리를 계산하여 다음 패턴 결정 (근접/원거리).
    * `WALK` / `MELEE`: 추적 후 근접 2연타 공격 (가로 베기 ➔ 세로 베기).
    * `ROLL_DASH` (지상 돌진): 바닥을 굴러 벽까지 돌진 후 튕겨 나옴.
    * `ROLL_BOUNCE` (공중 바운스): 물리 엔진의 마찰력을 무시하고, 플레이어의 미래 위치를 예측하여 포물선 궤도로 3회 튕기는 특수 패턴.
* **Multi-Boss Phase:** 보스 3마리가 존재하며, 동료의 체력이나 생존 여부에 따라 깨어나는 다중 페이즈 시스템을 적용했습니다.

### C. 🔊 상태 기반 사운드 시스템 (State-Based Audio)

FMOD의 단순 재생 방식의 한계(루프 끊김, 중복 재생)를 극복하기 위해 설계를 개선했습니다.

* **IsPlaying 체크:** 매 프레임 채널의 재생 상태를 검사하여, 사운드가 끊기면 즉시 복구하고 중복 재생을 방지합니다.
* **Sound Culling:** Update 로직에서 몬스터의 생존 여부(`isDead`)를 확인하여, 사망한 몬스터의 소리는 즉시 음소거(`StopSound`) 처리합니다.

---

## 🎮 조작 방법 (Controls)

| Key | Action | Description |
| :---: | :---: | :--- |
| **← / →** | 이동 | 좌우 이동 (관성 적용) |
| **Z** | 점프 | 짧게 누르면 소점프, 길게 누르면 대점프 |
| **X** | 공격 | `↑ + X` (상단 공격), `↓ + X` (하단 포고 점프) |
| **C** | 대시 | 전방으로 빠르게 이동 (공중 사용 가능, 쿨타임 존재) |
| **↑** | 상호작용 | 포탈 이동, NPC 대화 |
| **ESC** | 일시정지 | 옵션 메뉴 호출 (볼륨 조절 등) |
| **/** | Debug Mode | 디버그 모드, 콜라이더 확인 및 보스, 주인공 스테이트 확인 |


---

## 🔧 빌드 및 실행 가이드 (Build & Run)

이 프로젝트는 **Visual Studio** 환경에서 빌드하도록 설정되어 있습니다.

### 필수 요구 사항
* **Visual Studio 2019 이상**
* **DirectX SDK (June 2010)** 설치 필수 (시스템 경로 설정)

### 라이브러리 설정
1.  **프로젝트 속성** ➔ **VC++ 디렉터리** ➔ **포함 디렉터리 / 라이브러리 디렉터리**에 DirectX SDK 경로가 올바르게 설정되어 있는지 확인하세요.
2.  FMOD 관련 라이브러리(`fmodex_vc.lib` 등)는 프로젝트 내 `FMODENGIN` 폴더에 포함되어 있습니다.

### 빌드 및 배포
1.  구성 관리자를 **Release / x86**으로 설정하여 빌드하십시오.
2.  배포 시 생성된 `FrameWork.exe` 파일과 `resource` 폴더, 그리고 DLL 파일들(`fmodex.dll`, `d3dx9_xx.dll`)을 같은 경로에 두어야 합니다.

---

## 📝 개발자 노트 (Dev Notes)

* **물리 엔진 최적화:** 픽셀 단위 충돌 처리를 위해 AABB 방식을 사용하며, 착지 시 캐릭터 좌표를 바닥 높이에 강제로 맞추는 보정 로직을 통해 '떨림 현상'을 방지했습니다.
* **메모리 관리:** 맵 이동 시 `EffectManager`의 풀링된 객체와 `MapManager`의 몬스터 객체를 확실하게 정리(`delete` & `clear`)하여 메모리 누수를 방지했습니다.
* **확장성:** `InitPrefabs` 함수에 방 데이터만 추가하면, 알고리즘이 자동으로 새로운 맵 구조를 생성할 수 있도록 확장성을 고려하여 설계되었습니다.

* **States (상태)**:
    * `IDLE`: 대기 및 탐색
    * `TRACE`: 플레이어 추적
    * `ATTACK`: 공격 (근접/원거리)
    * `SKILL`: 특수 스킬 사용
    * `DIE`: 사망 처리
* **Transition (전이)**:
    * 거리, 체력, 쿨타임 등의 조건에 따라 유동적으로 상태가 변경됩니다.
    * *예시: 플레이어와의 거리가 멀어지면 `IDLE` ➡️ `TRACE`로 상태 전이*
