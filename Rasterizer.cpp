#include "Rasterizer.h"

RenderState Rasterizer::renderState = RenderState::TEXTURE;
LightState Rasterizer::lightState = LightState::OFF;

void Rasterizer::sort_vertex(Vertex &v1,Vertex &v2,Vertex &v3)
{
	if (v1.position.y>v2.position.y || (v1.position.y==v2.position.y&&v1.position.x>v2.position.x) )
	{
		swap(v1,v2);
	}
	if (v2.position.y>v3.position.y || (v2.position.y==v3.position.y&&v2.position.x>v3.position.x) )
	{
		swap(v2,v3);
	}
	if (v1.position.y>v2.position.y || (v1.position.y==v2.position.y&&v1.position.x>v2.position.x) )
	{
		swap(v1,v2);
	}
}

Scanline Rasterizer::generate_scanline(Vertex vl,Vertex vr,int y)
{
	float divide = 1.0f/(vr.position.x - vl.position.x);
	Scanline scanline(vl.color,(vr.color-vl.color)*divide);
	scanline.x = static_cast<int>(vl.position.x + 0.5f);
	scanline.y = y;
	scanline.inv_w = vl.inv_w;
	scanline.dinv_w = (vr.inv_w-vl.inv_w)*divide;
	scanline.width = static_cast<int>(vr.position.x + 0.5f) - scanline.x;

	scanline.u = vl.u;
	scanline.v = vl.v;
	scanline.du = (vr.u-vl.u)*divide;
	scanline.dv = (vr.v-vl.v)*divide;//΢��
	scanline.light = vl.light;
	scanline.dl = (vr.light - vl.light)*divide;

	return scanline;
}

//��ֵ΢��DDA(Digital Differential Analyzer)�㷨
void Rasterizer::drawline_dda
(int x1,int y1,int x2,int y2,AColor color)
{
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	if (dx>=dy) //��dx=1Ϊ�������������ֶϵ�
	{
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
		}
		float k = static_cast<float>(y2-y1)/(x2-x1);
		float y = y1;
		for (int x=x1;x<=x2;++x)
		{
			DX::instance().drawPixel(x,y,color);
			y += k;
		}
	}
	else
	{
		if (y1>y2)
		{
			swap(x1,x2);
			swap(y1,y2);
		}
		float k = static_cast<float>(x2-x1)/(y2-y1);
		float x = x1;
		for (int y=y1;y<=y2;++y)
		{
			DX::instance().drawPixel(x,y,color);
			x += k;
		}
	}	
}
/* ����ֱ��(�е㷨)
 * ��ʽ����f(x,y)=(y1-y1)x+(x2-x1)y+x1y2-x2y1=0
 * B*f(x,y)>0Ϊ��,<0Ϊ��,ÿ�����е㣨x+1,y��0.5��������бȽ�
 * �˴�ȡB>0,����ֱ���ж�*/                                                                     
void Rasterizer::drawline_mid
(int x1,int y1,int x2,int y2,AColor color)
{
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	if (dx>=dy)//��dx=1Ϊ�������������ֶϵ�
	{
		if (x1>x2){
			swap(x1,x2);
			swap(y1,y2);
		}
		int A = y1-y2;
		int B = x2-x1;//б��[0,1]
		if(y2>=y1){
			int d = (A<<1) + B;//f(x+1,y+0.5)*2����������������
			int upIncrement = (A+B)<<1;//ȡ�ϵ�ʱd������
			int downTncrement = A<<1;//ȡ�µ�ʱd������
			for (int x=x1,y=y1;x<=x2;++x){
				DX::instance().drawPixel(x,y,color);
				if (d<0){//�е���ֱ���£�ȡ�ϵ�
					d += upIncrement;
					++y;
				}
				else
					d += downTncrement;
			}
		}//б��[-1,0)
		else{
			int d = (A<<1) - B;					
			int upIncrement = A<<1;			
			int downTncrement = (A-B)<<1;					
			for (int x=x1,y=y1;x<=x2;++x){
				DX::instance().drawPixel(x,y,color);
				if (d<0)							
					d += upIncrement;
				else{
					d += downTncrement;
					--y;
				}
			}
		}	
	}
	else{
		if (y1>y2){
			swap(x1,x2);
			swap(y1,y2);
		}
		int A = x1-x2;
		int B = y2-y1;
		if (x2>=x1){
			int d = (A<<1) + B;//f(x+0.5,y+1)*2����������������,�˴�Ay+Bx+C=0
			int upIncrement = (A+B)<<1;//ȡ�ϵ�ʱd������
			int downTncrement = A<<1;//ȡ�µ�ʱd������
			for (int x=x1,y=y1;y<=y2;++y){
				DX::instance().drawPixel(x,y,color);
				if (d<0){//�е���ֱ���£�ȡ�ϵ�
					d += upIncrement;
					++x;
				}
				else
					d += downTncrement;
			}
		}
		else{
			int d = (A<<1) - B;					
			int upIncrement = A<<1;			
			int downTncrement = (A-B)<<1;			
			for (int x=x1,y=y1;y<=y2;++y){
				DX::instance().drawPixel(x,y,color);
				if (d<0)					
					d += upIncrement;
				else{
					d += downTncrement;
					--x;
				}
			}
		}
	}	
}
//Bresenham�㷨 ��Ҫͨ��e=d��0.5�жϷ���
void Rasterizer::drawline_bresenham(int x1,int y1,int x2,int y2,AColor color)
{
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	if (dx>=dy)										//��dx=1Ϊ�������������ֶϵ�
	{
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
		}
		int flag = y2>=y1?1:-1;						//б��[-1,1]
		int k = flag*(dy<<1);
		int e = -dx*flag;
		for (int x=x1,y=y1;x<=x2;++x) 
		{
			DX::instance().drawPixel(x,y,color);
			e += k;
			if (flag*e>0)
			{
				y += flag;
				e -= 2*dx*flag;
			}
		}
	}
	else
	{
		if (y1>y2)
		{
			swap(x1,x2);
			swap(y1,y2);
		}
		int flag = x2>x1?1:-1;
		int k = flag*(dx<<1);
		int e = -dy*flag;
		for (int x=x1,y=y1;y<=y2;++y) 
		{
			DX::instance().drawPixel(x,y,color);
			e += k;
			if (flag*e>0)
			{
				x += flag;
				e -= 2*dy*flag;
			}
		}
	}
}

void Rasterizer::draw_scanline(Vertex vl,Vertex vr,int y)
{
	Scanline scanline = generate_scanline(vl,vr,y);
	for (int i=0;i<scanline.width;++i)
	{
		float w = 1.0f/scanline.inv_w;
		if (renderState==RenderState::TEXTURE)
		{
			AColor color = texture->get_color(scanline.u*w,scanline.v*w);
			if (lightState==LightState::ON)
			{
				//ȫ��texture
				color.r_ = max(0,min(color.r_*scanline.light.r,255));
				color.g_ = max(0,min(color.g_*scanline.light.g,255));
				color.b_ = max(0,min(color.b_*scanline.light.b,255));
			}
			
			directX.drawPixel(scanline.x+i,scanline.y,color);
			scanline.to_next_texture();
		}
		else  //�޲���
		{
			VColor color = scanline.color*w;	
			if (lightState==LightState::ON)
			{
				color.r = max(0,min(color.r*scanline.light.r,1));
				color.g = max(0,min(color.g*scanline.light.g,1));
				color.b = max(0,min(color.b*scanline.light.b,1));
			}
			directX.drawPixel(scanline.x+i,scanline.y,color.to_AColor());
			scanline.to_next_color();
		}
	}
}

void Rasterizer::draw_top_flat_triangle(Vertex v1,Vertex v2,Vertex v3)
{
	int startY = v1.position.y + 0.5;
	int endY = v3.position.y + 0.5;
	for (float y=startY;y<endY;++y) 
	{
		float factor = (static_cast<float>(y+0.5)-v1.position.y)/(v3.position.y-v1.position.y);
		Vertex vl = v1.interp(v3,factor);
		Vertex vr = v2.interp(v3,factor);
		draw_scanline(vl,vr,y);
		
	}
}

void Rasterizer::draw_buttom_flat_triangle(Vertex v1,Vertex v2,Vertex v3)
{
	int startY = v1.position.y + 0.5;
	int endY = v3.position.y + 0.5;
	for (int y=startY;y<endY;++y) 
	{
		float factor = (static_cast<float>(y+0.5)-v1.position.y)/(v3.position.y-v1.position.y);
		Vertex vl = v1.interp(v2,factor);
		Vertex vr = v1.interp(v3,factor);
		draw_scanline(vl,vr,y);
		
	}
}

void Rasterizer::draw_triangle(Vertex v1,Vertex v2,Vertex v3)
{
	if (renderState==RenderState::WIREFRAME)//�߿�ͼ������
	{
		draw_triangle_wireframe(v1,v2,v3);
		return;
	}
	sort_vertex(v1,v2,v3);
	if (v1.position.y==v2.position.y)
	{
		draw_top_flat_triangle(v1,v2,v3);//ƽ��������
	}
	else if (v2.position.y==v3.position.y)
	{
		draw_buttom_flat_triangle(v1,v2,v3);
	}
	else//��ƽ��
	{
		float factor = (v2.position.y-v1.position.y)/(v3.position.y-v1.position.y);
		Vertex v4 = v1.interp(v3,factor);//��ֵ
		if (v4.position.x<v2.position.x)
		{
			swap(v4,v2);
		}
		//�ֳ�����ƽ��������
		draw_buttom_flat_triangle(v1,v2,v4);
		draw_top_flat_triangle(v2,v4,v3);
	}
}

void Rasterizer::draw_triangle_wireframe(Vertex v1,Vertex v2,Vertex v3)
{
	drawline_bresenham(v1.position.x,v1.position.y,v2.position.x,v2.position.y,AColor(0,255,0,0));
	drawline_bresenham(v1.position.x,v1.position.y,v3.position.x,v3.position.y,AColor(0,255,0,0));
	drawline_bresenham(v3.position.x,v3.position.y,v2.position.x,v2.position.y,AColor(0,255,0,0));
}
void Rasterizer::set_texture(Texture *ptexture)
{
	if (texture!=nullptr)
	{
		delete texture;
	}
	texture = ptexture;
}