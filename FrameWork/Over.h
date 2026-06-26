#ifndef __Over_H__
#define __Over_H__

class Over : public Chap
{
public:
	Over();
	~Over();

	Sprite m_Title;    // "GAMEOVER" 이미지
	int m_Alpha;       // 투명도 (0 ~ 255)

	D3DXIMAGE_INFO imagesinfo;

	virtual void Init();
	virtual void Update(double frame);
	virtual void Draw();

	virtual void OnMessage(MSG* msg);
};

#endif