#include<cmath>
#include<vector>
#include"Vertex.h"
#include"GameManager.h"
int ymax = 0, ymin = 0;//�����y�����ֵ����Сֵ
class EDGE{

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
void InitScanLineNewEdgeTable(vector<EDGE>* NET,vector<Vertex> vs, int ymin, int ymax)
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
					e.ymax = ps.position.y-1;
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
void My_Sort(vector<EDGE> &list)
{
	EDGE d;
	for (int i = 0; i < list.size() - 1; i++)
	{
		for (int j = i + 1; j < list.size(); j++)//Ϲ��for (int j = i+1; i < list.Count; i++)
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

int line = 0;
void RemoveNonActiveEdgeFromAet(vector<EDGE> &AET, int y)
{
	line = y;
	for (auto it = AET.begin(); it !=AET.end(); it++)
	{
		if ((*it).ymax == line)
			AET.erase(it);
	}
}
/// <summary>
/// ���±߱���ÿ���xiֵ�����Ǹ���ɨ���ߵ���������dx�������������
/// ���Ҹ���xi��С�����ԭ��Ը��º��aet����������
/// </summary>
/// <param name="AET"></param>

void UpdateAndResortAet(vector<EDGE> &AET)
{
	//����xi
	for(auto e:AET)
	{
		e.xi += e.dx;;
	}
	My_Sort(AET);
}

/// <summary>
/// ɨ������䴦�����
/// ��ʼ��ÿ��ɨ���߽��д�����ÿ��ɨ���ߵĴ������ĸ�����
/// </summary>
/// <param name="NET"></param>
/// <param name="ymin"></param>
/// <param name="ymax"></param>
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
}
void FillAetScanLine(vector<EDGE> &AET, int y)
{
	if (AET.size()< 2) 
		return;
	for (int i = 0; i < AET.size(); i += 1)
	{
		int from = ((int)AET[i].xi + 1);
		int to = ((int)(AET[i + 1].xi + 1 / 2));
		while (from < to)
		{
			Vertex v;
			v.position = Vector3(from, y, 0);
			GameManager::Instance().draw_pixel(v, AColor(0, 255, 255, 255));
			from += 1;
		}
	}
}
void InsertNetListToAet(vector<EDGE> list, vector<EDGE> &AET)
{
	if (list.size()== 0) return;
	if (AET.size() == 0)
	{
		AET = list;
		return;
	}//�տ�ʼ����д��if����AET=list;return;һֱ�����´�һ��Ҫ�淶������
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

