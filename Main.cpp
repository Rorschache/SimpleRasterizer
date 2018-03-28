#include "RenderManager.h"
#include <cmath>
#include<string>
#include<iostream>
#include<cstdio>
#include<vector>
#include<stdlib.h>
using std::cin;
using std::cout;
using std::endl;
using std::string;
#pragma  region ɨ����
int ymax = 0, ymin = 0;//�����y�����ֵ����Сֵ
int line = 0;
AColor policolor;
class EDGE {

public:
	double xi;//�ߵ��¶˵�x���꣬�ڻ����AET���У���ʾɨ������ߵĽ���x����
	double dx;//�Ǹ�������ֱ��б�ʵĵ�������x + dx, y + 1��
	int ymax;//�ߵ��϶����yֵ
	friend bool operator <(EDGE a, EDGE b)//�������й�ϵ
	{
		return (abs(a.xi - b.xi) < 1 ? a.dx < b.dx : a.xi < b.xi);
	}
	friend bool operator >(EDGE a, EDGE b)//�������й�ϵ
	{
		return (abs(a.xi - b.xi) < 1 ? a.dx > b.dx : a.xi > b.xi);
	}
	friend bool operator ==(EDGE a, EDGE b)//���ص��ں�
	{
		return (abs(a.xi - b.xi) < 1 && a.dx == b.dx && a.ymax == b.ymax);
	}
	friend bool operator !=(EDGE a, EDGE b)//���ز����ں�
	{
		return (abs(a.xi - b.xi) > 1 || a.dx != b.dx || a.ymax != b.ymax);
	}
};
vector<EDGE> allaet;
void My_Sort(vector<EDGE> &list)
{
	EDGE d;
	for (int i = 0; i < list.size() - 1; i++)
	{
		for (int j = i + 1; j < list.size(); j++)
		{
			if (list[j] < list[i])
			{
				d = list[j];
				list[j] = list[i];
				list[i] = d;
			}
		}
	}
}
void RemoveNonActiveEdgeFromAet(vector<EDGE> &AET, int y)
{
		line = y;
		for (auto it = AET.begin(); it != AET.end();)
		{
			if (line == (*it).ymax) 
				it = AET.erase(it);
			else 
				++it;
		}
}
/// <summary>
/// ���±߱���ÿ���xiֵ�����Ǹ���ɨ���ߵ���������dx�������������
/// ���Ҹ���xi��С�����ԭ��Ը��º��aet����������
void UpdateAndResortAet(vector<EDGE> &AET)
{
	//����xi
	
	for (auto& e : AET)//����stl��Ҫ&
	{
		e.xi += e.dx;
	}

	My_Sort(AET);
}
/// ɨ������䴦�����
/// ��ʼ��ÿ��ɨ���߽��д�����ÿ��ɨ���ߵĴ������ĸ�����
void FillAetScanLine(vector<EDGE> &AET, int y)
{
	if (AET.size() < 2)
		return;
	for (int i = 0; i < AET.size(); i+=2)
	{
		int from = ((int)AET[i].xi + 1);
		int to = ((int)(AET[i + 1].xi + 1));
		while (from < to)
		{
			Vertex v;
			v.position = Vector3(from, y, 0);
			RenderManager::Instance().draw_pixel(v, policolor);
			from += 1;
		}
	}
}
void InsertNetListToAet(vector<EDGE> list, vector<EDGE> &AET)
{
	if (list.size() == 0) 
		return;
	if (AET.size() == 0)
	{
		AET = list;
		return;
	}
	vector<EDGE> temp;
	int i = 0, j = 0;
	while (i < list.size() && j < AET.size())
	{
		if (list[i] == AET[j])
		{
			i++;
			temp.push_back(AET[j]);
			j++;
			continue;
		}
		if (list[i] < AET[j])
		{
			temp.push_back(list[i]);
			i++;
			continue;
		}
		if (list[i] > AET[j])
		{
			temp.push_back(AET[j]);
			j++;
			continue;
		}
	}
	while (i < list.size())
	{
		temp.push_back(list[i]);
		i++;
	}
	while (j < AET.size())
	{
		temp.push_back(AET[j]);
		j++;
	}
	AET = temp;
}
void GetPolygonMinMax(vector<Vertex> vs)
{
	ymax = -1;
	ymin = 1000;
	for (int i = 0; i < vs.size(); i++)
	{
		if (vs[i].position.y > ymax)
			ymax = vs[i].position.y;
		if (vs[i].position.y < ymin)
			ymin = vs[i].position.y;
	}
	return;
}
void InitScanLineNewEdgeTable(vector<EDGE>* NET, vector<Vertex> vs, int ymin, int ymax)
{
	vector<int> temp;
	EDGE e;
	for (int i = 0; i < vs.size(); i++)
	{
		Vertex ps = vs[i];
		Vertex pe = vs[(i + 1) % vs.size()];
		Vertex pss = vs[(i - 1 + vs.size()) % vs.size()];
		Vertex pee = vs[(i + 2) % vs.size()];
		if (pe.position.y != ps.position.y)//������ƽ����
		{
			e.dx = (double)(pe.position.x - ps.position.x) / (double)(pe.position.y - ps.position.y);
			if (pe.position.y > ps.position.y)
			{
				e.xi = ps.position.x;
				if (pee.position.y >= pe.position.y)
					e.ymax = pe.position.y - 1;
				else
					e.ymax = pe.position.y;
				NET[(int)ps.position.y - ymin].push_back(e);//�����Ӧ��NET��
				temp.push_back(ps.position.y - ymin);
			}
			else
			{
				e.xi = pe.position.x;
				if (pss.position.y >= ps.position.y)
					e.ymax = ps.position.y - 1;
				else
					e.ymax = ps.position.y;
				NET[(int)pe.position.y - ymin].push_back(e);//�����Ӧ��NET��
				temp.push_back(pe.position.y - ymin);
			}
		}
	}
	for (int i = 0; i < temp.size(); i++)
	{
		My_Sort(NET[temp[i]]);
	}
}
void ProcessScanLineFill(vector<EDGE>* NET, int ymin, int ymax)
{
	vector<EDGE> AET;//ɨ����
	for (int y = ymin; y < ymax; y += 1)
	{
		InsertNetListToAet(NET[y - ymin], AET);
		FillAetScanLine(AET, y);
		RemoveNonActiveEdgeFromAet(AET, y);//ɾ���ǻ��
		UpdateAndResortAet(AET);//���»�߱���ÿ���xiֵ��������xi��������
	}
	allaet = AET;
}
void ScanLinePolygonFill(vector<Vertex> vs)
{
	vector<EDGE>* NET = new vector<EDGE>[500];//�����±߱�
	GetPolygonMinMax(vs);//�������ymax��ymin(ok)
	InitScanLineNewEdgeTable(NET, vs, ymin, ymax);//��ʼ���±߱�
	ProcessScanLineFill(NET, ymin, ymax);
	delete[]NET;
}

#pragma endregion
void initwindow()
{
	const int WIDTH = 800;
	const int HEIGHT = 800;

	RenderManager& Mywindow = RenderManager::Instance();
	//Mywindow.game_start(WIDTH,HEIGHT);

	RenderWindow myWindow("Render", "Render", WIDTH, HEIGHT);
	//Camera,��ʼ����Ļ
	Camera::instance(WIDTH, HEIGHT);
	//DirectX
	DX::instance().initialDirectX(GetModuleHandle(nullptr), myWindow.get_hwnd(), WIDTH, HEIGHT);
	
	return;

}
void DrawBzier(vector<Vertex> vs);
void DrawCube();
int main()
{
	policolor = AColor(0, 255, 255, 255);
	bool ifget = false;
	while (1)
	{
		cout << "��ѡ��Ҫ����ͼ�Σ�" << endl;
		cout << "1, ��" << endl;
		cout << "2, Բ" << endl;
		cout << "3, ��Բ" << endl;
		cout << "4, ����μ����ü�" << endl;
		cout << "5, Bezier����" << endl;
		cout << "6, ��ȾCube" << endl;
		char choice;
		cin >> choice;

		//����
		if (choice == '1')
		{
			
			Vertex v1, v2;
			cout << "�������1�������꣬��ʽ��(x,y): ";
			cin >> v1;
			cout << "�������2�������꣬��ʽ��(x,y): ";
			cin >> v2;
			initwindow();

#pragma region MessageLoop
			//ʱ���ʼ��
			DWORD curTime = GetTickCount();
			DWORD preTime = GetTickCount();
			//2.��Ϣѭ��
			MSG msg = { 0 };
			while (msg.message != WM_QUIT)
			{
				//��ȡ��Ϣ
				if (PeekMessage(&msg, 0, NULL, NULL, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					curTime = GetTickCount();
					//cout<<1000.0f/(curTime-preTime)<<endl;
					if (curTime - preTime > 30)
					{
						preTime = curTime;
						//GameManager::Instance().game_update();
						DX &directx = DX::instance();
						directx.fillSurface();
						directx.lockSurface();

						
						RenderManager::Instance().draw_lines(v1, v2, AColor(0, 255, 255, 255));

						directx.unlockSurface();
						directx.flipSurface();
					}
				}
			}
#pragma endregion
		}
		//��Բ
		else if (choice == '2')
		{
			Vertex v; int r;
			cout << "������Բ�����꣬��ʽ(x,y): ";
			cin >> v;
			cout << "������뾶����: ";
			cin >> r;
			initwindow();
#pragma region MessageLoop
			//ʱ���ʼ��
			DWORD curTime = GetTickCount();
			DWORD preTime = GetTickCount();
			//2.��Ϣѭ��
			MSG msg = { 0 };
			while (msg.message != WM_QUIT)
			{
				//��ȡ��Ϣ
				if (PeekMessage(&msg, 0, NULL, NULL, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					curTime = GetTickCount();
					//cout<<1000.0f/(curTime-preTime)<<endl;
					if (curTime - preTime > 30)
					{
						preTime = curTime;
						//GameManager::Instance().game_update();
						DX &directx = DX::instance();
						directx.fillSurface();
						directx.lockSurface();

						RenderManager::Instance().draw_circle(v, r);

						directx.unlockSurface();
						directx.flipSurface();
					}
				}
			}
#pragma endregion
		}
		//����Բ
		else if (choice == '3')
		{
			Vertex v;
			int lr, sr;
			cout << "��������Բ���ĵ����꣬��ʽ(x,y): ";
			cin >> v;
			cout << "��������Բ����(x�᷽��): ";
			cin >> lr;
			cout << "��������Բ����(y�᷽��): ";
			cin >> sr;
			initwindow();
#pragma region MessageLoop
			//ʱ���ʼ��
			DWORD curTime = GetTickCount();
			DWORD preTime = GetTickCount();
			//2.��Ϣѭ��
			MSG msg = { 0 };
			while (msg.message != WM_QUIT)
			{
				//��ȡ��Ϣ
				if (PeekMessage(&msg, 0, NULL, NULL, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					curTime = GetTickCount();
					//cout<<1000.0f/(curTime-preTime)<<endl;
					if (curTime - preTime > 30)
					{
						preTime = curTime;
						//GameManager::Instance().game_update();
						DX &directx = DX::instance();
						directx.fillSurface();
						directx.lockSurface();

						RenderManager::Instance().draw_ellipse(v, lr, sr);

						directx.unlockSurface();
						directx.flipSurface();
					}
				}
			}
#pragma endregion
		}
		//�������
		else if (choice == '4')
		{
			int wxl, wxr, wyb, wyt;
			cout << "���������εı������Ա�����: ";
			int n;
			cin >> n;
			cout << "�밴����˳������ÿһ��������꣬��ʽ(x,y): " << endl;
			vector<Vertex> vs;
			vector<Vertex> vs2;
			for (int i = 0; i < n; i++)
			{
				Vertex temp;
				cout << "��" << i + 1 << "��������: ";
				cin >> temp;
				vs.push_back(temp);
			}

			cout << "�Ƿ���Ҫ�����ɫ����Ҫ��1������2: ";
			char choice2;
			char choice3;
			cin >> choice2;
			if (choice2 == '1')
			{
				cout << "��������ɫ��rgbֵ(0~255)����ʽ�� 255 255 255��"<<endl;
				int r, g, b;
				cin >> r >> g >> b;
				policolor.r_ = r;
				policolor.g_ = g;
				policolor.b_ = b;
				cout << "�Ƿ���Ҫ�ü�����Ҫ��1������2: ";
				cin >> choice3;
				if (choice3 == '1')
				{
					cout << "�ֱ����봰�ڵ������߽����꣺"<<endl;
					cout << "wxl=";
					cin >> wxl;
					cout << "wxr=";
					cin >> wxr;
					cout << "wyb=";
					cin >> wyb;
					cout << "wyt=";
					cin >> wyt;
					Vertex v1, v2, v3, v4;
					v1.position = Vector3(wxl, wyt, 0);
					v2.position = Vector3(wxr, wyt, 0);
					v3.position = Vector3(wxr, wyb, 0);
					v4.position = Vector3(wxl, wyb, 0);
					vs2.push_back(v1);
					vs2.push_back(v2);
					vs2.push_back(v3);
					vs2.push_back(v4);
				}

			}

			initwindow();
#pragma region MessageLoop
			//ʱ���ʼ��
			DWORD curTime = GetTickCount();
			DWORD preTime = GetTickCount();
			//2.��Ϣѭ��
			MSG msg = { 0 };
			while (msg.message != WM_QUIT)
			{
				//��ȡ��Ϣ
				if (PeekMessage(&msg, 0, NULL, NULL, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					curTime = GetTickCount();
					//cout<<1000.0f/(curTime-preTime)<<endl;
					if (curTime - preTime > 30)
					{
						preTime = curTime;
						//GameManager::Instance().game_update();
						DX &directx = DX::instance();
						directx.fillSurface();
						directx.lockSurface();

						RenderManager::Instance().draw_poligon(vs, AColor(0, 255, 255, 255));
						if (choice2 == '1')
						{
							ScanLinePolygonFill(vs);
							if (choice3 == '1')
							{
								RenderManager::Instance().draw_poligon(vs2, AColor(0, 255, 255, 255));
								for (int i = 0; i < 800; i++)
									for (int j = 0; j < 800; j++)
										if (i<wxl || i>wxr || j<wyt || j>wyb)
											DX::instance().drawPixel(i, j, AColor(0, 0, 0, 0));
							}
						}

						directx.unlockSurface();
						directx.flipSurface();
					}
				}
			}
#pragma endregion
		}
		else if (choice == '5')
		{
			
			cout << "������������: ";
			int n;
			cin >> n;
			cout << "�밴����˳������ÿһ��������꣬��ʽ(x,y): " << endl;
			vector<Vertex> vs;
			for (int i = 0; i < n; i++)
			{
				Vertex temp;
				cout << "��" << i + 1 << "��������: ";
				cin >> temp;
				vs.push_back(temp);
			}
			/*
			Vertex v1,v2,v3,v4;
			v1.position = Vector3(100, 120, 0);
			v2.position = Vector3(200, 100, 0);
			v3.position = Vector3(300, 200, 0);
			v4.position = Vector3(250, 150, 0);
			vector<Vertex> vs;
			vs.push_back(v1);
			vs.push_back(v2);
			vs.push_back(v3);
			vs.push_back(v4);*/

			initwindow();
#pragma region MessageLoop
			//ʱ���ʼ��
			DWORD curTime = GetTickCount();
			DWORD preTime = GetTickCount();
			//2.��Ϣѭ��
			MSG msg = { 0 };
			while (msg.message != WM_QUIT)
			{
				//��ȡ��Ϣ
				if (PeekMessage(&msg, 0, NULL, NULL, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					curTime = GetTickCount();
					//cout<<1000.0f/(curTime-preTime)<<endl;
					if (curTime - preTime > 30)
					{
						preTime = curTime;
						//GameManager::Instance().game_update();
						DX &directx = DX::instance();
						directx.fillSurface();
						directx.lockSurface();

						DrawBzier(vs);

						directx.unlockSurface();
						directx.flipSurface();
					}
				}
			}
#pragma endregion
		}
		else if (choice == '6')
		{
			RenderManager::Instance().Cube_start(800,800);
		}
		system("cls");
	}
	return 0;
}
//Bezier����
void DrawBzier(vector<Vertex> vs)
{
	int n = vs.size()-1;
	if (n <= 0)
		return;
	if ((vs[n].position.x < vs[0].position.x + 1) && (vs[n].position.x > vs[0].position.x - 1) 
		&& (vs[n].position.y < vs[0].position.y + 1) && (vs[n].position.y > vs[0].position.y - 1))
	{
		RenderManager::Instance().draw_pixel(vs[0], AColor(0, 255, 255, 255));
		return;
	}
	vector<Vertex> p1(n+1);
	int i, j;
	p1[0] = vs[0];
	for (i = 1; i <= n; i++)
	{
		for (j = 0; j <= n - i; j++)
		{
			vs[j].position.x = (vs[j].position.x + vs[j + 1].position.x) / 2;
			vs[j].position.y = (vs[j].position.y + vs[j + 1].position.y) / 2;
		}
		p1[i] = vs[0];
	}
	DrawBzier(vs);
	DrawBzier(p1);
}