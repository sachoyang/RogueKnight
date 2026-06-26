#ifndef __Chap_H__
#define __Chap_H__

// 모든 씬(Scene)의 공통 인터페이스.
// 씬은 Chap* (CMng::chap[]) 로 다형적으로 다뤄지므로 소멸자는 반드시 virtual 이어야 한다.
// (그렇지 않으면 기반 포인터로 delete 시 파생 소멸자가 호출되지 않아 누수/UB 발생)
class Chap
{
public :
	Chap() = default;
	virtual ~Chap() = default;   // ★ 가상 소멸자

	virtual void Init() = 0;
	virtual void Update(double frame) = 0;
	virtual void Draw() = 0;
	virtual void OnMessage(MSG* msg) = 0;
};

#endif