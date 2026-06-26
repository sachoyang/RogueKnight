#include <wrl/client.h>   // Microsoft::WRL::ComPtr (자동 Release)

class Sprite
{
public:
	Sprite(void);
	~Sprite(void);

	// RAII: ComPtr 가 소멸/재생성 시 자동으로 Release() 호출 → GPU 텍스처 누수 방지.
	// (기존 raw IDirect3DTexture9* 는 소멸자에서 Release 하지 않아 누수 발생)
	Microsoft::WRL::ComPtr<IDirect3DTexture9> Texture;
	D3DXIMAGE_INFO imagesinfo;
	D3DCOLOR color;

	bool Create(const char* filename, bool bUseTransparency, D3DCOLOR TransparencyColor);

	void Draw( float dx , float dy , float sx , float sy , float sw , float sh, float centerX = 0, float centerY = 0 ); // #92

	void Draw(float x, float y); // #01

	//void Render( float x , float y , float radian, float sx, float sy);

	void Render(float x, float y, float radian, float sx, float sy, int pivotMode = 0);

	void RenderDraw( float x , float y , float sx , float sy , float sw , float sh, float radian, float sl, float st) ;

	void SetColor( int r , int g , int b , int a );
};

extern Sprite g_Load;