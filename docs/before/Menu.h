#ifndef __Menu_H__
#define __Menu_H__

class Menu : public Chap
{
public:

	Sprite menuimg;

	Sprite m_BtnStart;
	Sprite m_BtnExit;

	RECT m_rtStart;
	RECT m_rtExit;

	D3DXIMAGE_INFO imagesinfo[3];

	Menu();
	~Menu();

	virtual void Init();
	virtual void Update(double frame);
	virtual void Draw();

	virtual void OnMessage(MSG* msg);

};

#endif