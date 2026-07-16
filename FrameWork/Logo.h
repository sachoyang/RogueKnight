#ifndef __Logo_H__
#define __Logo_H__

class Logo : public Chap
{
public:
	Logo();
	~Logo();

	HWND hWnd;
	//CSound* Sound1;
	Sprite loadimg;

	float speed;
	float alpha;

	float m_fX;
	float m_fY;

	int count = 0;

	virtual void Init();
	virtual void Update(double frame);
	virtual void Draw();

	virtual void OnMessage(MSG* msg);

	
};

#endif