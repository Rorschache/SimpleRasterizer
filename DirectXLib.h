#ifndef DIRECTXLIB_H_
#define DIRECTXLIB_H_

#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include <iostream>
#include "Enum.h"
#include "MyMath.h"
using namespace std;
class DX
{
public:
	//������ȡ
	static DX& instance()//static����
	{
		static DX* pDirectX = new DX();
		return *pDirectX;
	}
	//��ʼ��DirectX
	bool initialDirectX(HINSTANCE hInstance, HWND hWnd, int width, int height);
	void fillSurface();
	void lockSurface();
	void unlockSurface();
	//�������ص�������
	void drawPixel(int x, int y, AColor color);//,float depth=0.0f);
	//��ȡ����
	AColor DX::getPixel(int x, int y);
	//��������ת��ʾ
	void flipSurface();
	DWORD ARGB(int a,int r,int g,int b){return DWORD((a<<24)+(r<<16)+(g<<8)+b);}
	//����
	~DX();

private:
	DX():pD3DXDevice(nullptr),pD3DSurface(nullptr){}
	IDirect3DDevice9* pD3DXDevice;//�豸
	IDirect3DSurface9* pD3DSurface;//����
	D3DLOCKED_RECT LockRect;//ָ�����洢��
	float *z_buffer_;//vertexbuffer
	int width_;
	int height_;
	int buffer_size_;
};


#endif