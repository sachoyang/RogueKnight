# DirectX SDK (June 2010) 세팅 메모

> 다른 PC(집/회사 등)에 이 프로젝트를 옮겨서 빌드할 때 필요한 준비물 메모.
> 이 프로젝트는 `d3dx9.h` / `d3dx9.lib` (DirectX SDK June 2010)에 의존한다.

## 왜 필요한가
- `FrameWork/Include.h` → `#include <d3dx9.h>`
- `FrameWork/FrameWork.vcxproj` 설정:
  - `IncludePath` = `$(DXSDK_DIR)Include`
  - `LibraryPath` = `$(DXSDK_DIR)Lib\x64`
- 즉 **SDK 파일**과 **`DXSDK_DIR` 환경변수** 둘 다 있어야 컴파일된다.
  없으면 `Include.h(35): error C1083: 'd3dx9.h' 파일 없음`으로 빌드 실패.

## 원래 작업 PC에서 챙길 것 (옮길 대상)
1. 명령프롬프트에서 경로 확인:
   ```
   echo %DXSDK_DIR%
   ```
   보통: `C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\`
2. 그 폴더 **통째로 복사**. 최소한 아래 하위 폴더는 반드시 포함:
   - `Include\`  (d3dx9.h 등)
   - `Lib\x64\`  (d3dx9.lib 등)  ← 이 프로젝트는 x64 빌드

## 새 PC에서 할 일 (방법 A — 폴더 복사, 권장)
> June 2010 설치관리자는 오류 S1023(VC++ 2010 재배포 충돌)로 자주 실패하므로
> 폴더 복사가 제일 깔끔하다.

1. 복사해온 폴더를 아무 경로에나 붙여넣기 (원래와 같은 경로면 더 편함).
2. `DXSDK_DIR` 환경변수를 그 **루트 경로**로 설정.
   - ⚠️ **끝에 백슬래시 `\` 필수** (vcxproj가 `$(DXSDK_DIR)Include`처럼 구분자 없이 이어 붙임)
   ```powershell
   setx DXSDK_DIR "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\"
   ```
3. Visual Studio 완전히 종료 후 다시 실행 (환경변수 반영).
4. 빌드: 구성 = **Debug / x64**.

## 방법 B — 설치관리자로 새로 설치
1. `DXSDK_Jun10.exe` 실행 (설치되면 `DXSDK_DIR` 자동 설정됨).
2. 오류 S1023 발생 시:
   제어판 → **"Microsoft Visual C++ 2010 x86/x64 Redistributable 10.0.40219"** 제거
   → SDK 재설치 → 재배포 패키지 다시 설치.

## 확인 방법
```powershell
echo $env:DXSDK_DIR
Test-Path "$env:DXSDK_DIR\Include\d3dx9.h"   # True 나와야 정상
Test-Path "$env:DXSDK_DIR\Lib\x64\d3dx9.lib" # True 나와야 정상
```

## 참고
- MySQL/DB 기능은 사용하지 않으므로 프로젝트에서 완전히 제거되었다
  (`Mysql.cpp/.h`, `libmysql.dll`, MySQL Include/Library 경로 모두 삭제).
