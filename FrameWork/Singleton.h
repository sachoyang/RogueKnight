#pragma once
//============================================================================
//  Singleton.h  -  CRTP 기반 싱글톤 베이스
//----------------------------------------------------------------------------
//  목적: Camera / Sound / EffectManager / TimeManager 등 매니저들이 각자
//        제각각으로 구현하던 싱글톤(전역 포인터 + new, 또는 함수 내 static)을
//        하나의 패턴으로 통일한다.
//
//  특징:
//    - Meyers 싱글톤(함수 내 지역 static): C++11 이후 스레드-세이프하게
//      "최초 GetInstance() 호출 시점"에 한 번만 생성되고 프로그램 종료 시 자동 소멸.
//      → 전역 객체의 "static init order fiasco"와 new 누수를 동시에 해결.
//    - 복사/이동 금지로 단일 인스턴스를 강제.
//    - 기존 호출 규약(CAM->, SOUND-> 등 포인터 ->)을 유지하기 위해 포인터를 반환.
//
//  사용법:
//      class Camera : public Singleton<Camera> {
//          friend class Singleton<Camera>;   // 베이스가 생성자에 접근하도록 허용
//      private:
//          Camera();                          // 외부 생성 차단
//      };
//      // 호출:  CAM->Update();  (CAM == Camera::GetInstance())
//============================================================================
template <class T>
class Singleton
{
public:
    static T* GetInstance()
    {
        static T instance;   // 최초 호출 시 1회 생성, 종료 시 자동 소멸
        return &instance;
    }

    Singleton(const Singleton&)            = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&)                 = delete;
    Singleton& operator=(Singleton&&)      = delete;

protected:
    Singleton()  = default;
    ~Singleton() = default;
};
