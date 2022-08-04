/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.h																			  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "ChiliException.h"
#include "Colors.h"
#include "Vec3.h"
#include <vector>
#include <string>
using namespace std;

class Light {
public:
	enum LTYPE { ambient, point, directional };
	LTYPE type;
	double intensity;
	Vec3 DIRECTION;
	Vec3 POSITION;

	Light(LTYPE t = ambient, double i = 1, double d1 = 0, double d2 = 0, double d3 = 0, double p1 = 0, double p2 = 0, double p3 = 0)
	{
		type = t;
		intensity = i;
		DIRECTION.V[0] = d1;
		DIRECTION.V[1] = d2;
		DIRECTION.V[2] = d3;
		POSITION.V[0] = p1;
		POSITION.V[1] = p2;
		POSITION.V[2] = p3;
	}

	Light (const Light& L)
	{
		type = L.type;
		intensity = L.intensity;

		for (int i = 0; i < 3; i++)
		{
			DIRECTION.V[i] = L.DIRECTION.V[i];
			POSITION.V[i] = L.POSITION.V[i];
		}
	}

	Light& operator=(const Light& lhs)
	{
		if (this != &lhs)
		{
			type = lhs.type;
			intensity = lhs.intensity;

			for (int i = 0; i < 3; i++)
			{
				DIRECTION.V[i] = lhs.DIRECTION.V[i];
				POSITION.V[i] = lhs.POSITION.V[i];
			}
		}
		return *this;
	}

	void set(LTYPE t = ambient, double i = 1, double d1 = 0, double d2 = 0, double d3 = 0, double p1 = 0, double p2 = 0, double p3 = 0)
	{
		type = t;
		intensity = i;
		DIRECTION.V[0] = d1;
		DIRECTION.V[1] = d2;
		DIRECTION.V[2] = d3;
		POSITION.V[0] = p1;
		POSITION.V[1] = p2;
		POSITION.V[2] = p3;
	}
};

class Sphere {
public:
	Vec3 center;
	Vec3 color;
	double radius = 1;
	int specular = -1;

	/*void CenterToOrigin()
	{
		center[0] = center[1] = center[2] = 0;
	}*/
	void noColor()
	{
		color.V[0] = color.V[1] = color.V[2] = 0;
	}

	Sphere(int x = 0, int y = 0, int z = 0, int r = 0, int g = 0, int b = 0, double rad = 1, int s = -1)
	{
		center.V[0] = x;
		center.V[1] = y;
		center.V[2] = z;
		color.V[0] = r;
		color.V[1] = g;
		color.V[2] = b;
		radius = rad;
		specular = s;
	}

	Sphere(const Sphere& sphere)
	{
		radius = sphere.radius;
		specular = sphere.specular;
		for (int i = 0; i < 3; i++)
		{
			center.V[i] = sphere.center.V[i];
			color.V[i] = sphere.color.V[i];
		}
	}
};

struct init
{
public:
	int x;
	int y;
	int z; 
	int r;
	int g;
	int b;
	double radius = 1;
	int specular = -1;

	init(class HWNDKey& key, int _x = 0, int _y = 0, int _z = 0, int _r = 0, int _g = 0, int _b = 0, double rad = 1, int s = -1)
	{
		x = _x;
		y = _y;
		z = _z;
		r = _r;
		g = _g;	
		b = _b;
		radius = rad;
		specular = s;
	}

	void set(int _x = 0, int _y = 0, int _z = 0, int _r = 0, int _g = 0, int _b = 0, double rad = 1, int s = -1)
	{
		x = _x;
		y = _y;
		z = _z;
		r = _r;
		g = _g;
		b = _b;
		radius = rad;
		specular = s;
	}
};

class Graphics
{
public:
	Vec3 D;
	Vec3 P;
	Vec3 CAMERA;
	Vec3 COLORS;
	double INT_PTS[2]{};
	bool MISS = 1;
	

	friend Sphere;
	friend Light;

	class Exception : public ChiliException
	{
	public:
		Exception( HRESULT hr,const std::wstring& note,const wchar_t* file,unsigned int line );
		std::wstring GetErrorName() const;
		std::wstring GetErrorDescription() const;
		virtual std::wstring GetFullMessage() const override;
		virtual std::wstring GetExceptionType() const override;
	private:
		HRESULT hr;
	};
private:
	// vertex format for the framebuffer fullscreen textured quad
	struct FSQVertex
	{
		float x,y,z;		// position
		float u,v;			// texcoords
	};
public:
	Graphics( class HWNDKey& key );
	Graphics( const Graphics& ) = delete;
	Graphics& operator=( const Graphics& ) = delete;
	void EndFrame();
	void BeginFrame();
	void PutPixel( int x,int y,int r,int g,int b )
	{
		PutPixel( x,y,{ unsigned char( r ),unsigned char( g ),unsigned char( b ) } );
	}
	void PutPixel( int x,int y,Color c );
	
	void PutPixelRT(int x, int y)
	{
		PutPixel(Graphics::ScreenWidth / 2 + x, Graphics::ScreenHeight / 2 - y, COLORS.V[0], COLORS.V[1], COLORS.V[2]);
	}

	//void PutPixel2(int x, int y, int r, int g, int b)
	//{
	//	PutPixel2(x, y, { unsigned char(r),unsigned char(g),unsigned char(b) });
	//}
	//void PutPixel2(int x, int y, Color c);

	//void PutPixel3(int x, int y, int r, int g, int b)
	//{
	//	PutPixel3(x, y, { 0,0,0 });
	//}
	//void PutPixel3(int x, int y, Color c);
	~Graphics();

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain>				pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device>				pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			pImmediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				pSysBufferTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pSysBufferTextureView;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			pSamplerState;
	D3D11_MAPPED_SUBRESOURCE							mappedSysBufferTexture;
	Color*                                              pSysBuffer = nullptr;

public:
	static constexpr int ScreenWidth = 800;
	static constexpr int ScreenHeight = 600;
	/*bool inhibitUp = false;
	bool inhibitDown = false;
	bool inhibitLeft = false;
	bool inhibitRight = false;*/
	float intensity, closest_t;

	void CanvasToViewport(int x, int y);
	void TraceRay(double, double);
	void IntersectRaySphere(int);
	double ComputeLighting(int index);
	//int dot(int[], int[]);
	//double dot(double[], double[]);
	//double dot(double[], int[]);
	double dot(const Vec3&, const Vec3&);
	//void VecAdd(double[3], double[3], double c[3]);
	//void VecSub(double[3], double[3], double c[3]);
	//void scaleVec(double, double[3], double[3]);
	//double vecLength(double[3]);
	double vecLength(const Vec3&);
	void BoundPixelValues();
	friend Vec3 operator+(const Vec3& lhs, const Vec3& rhs);
	friend Vec3 operator-(const Vec3& lhs, const Vec3& rhs);
	friend Vec3 operator*(double s, const Vec3& rhs);
	

	void SetSpheres(init s1, init s2, init s3, init s4)
	{
		Sphere1.center.V[0] = s1.x;
		Sphere1.center.V[1] = s1.y;
		Sphere1.center.V[2] = s1.z;
		Sphere1.color.V[0] = s1.r;
		Sphere1.color.V[1] = s1.g;
		Sphere1.color.V[2] = s1.b;
		Sphere1.radius = s1.radius;
		Sphere1.specular = s1.specular;

		Sphere2.center.V[0] = s2.x;
		Sphere2.center.V[1] = s2.y;
		Sphere2.center.V[2] = s2.z;
		Sphere2.color.V[0] = s2.r;
		Sphere2.color.V[1] = s2.g;
		Sphere2.color.V[2] = s2.b;
		Sphere2.radius = s2.radius;
		Sphere2.specular = s2.specular;

		Sphere3.center.V[0] = s3.x;
		Sphere3.center.V[1] = s3.y;
		Sphere3.center.V[2] = s3.z;
		Sphere3.color.V[0] = s3.r;
		Sphere3.color.V[1] = s3.g;
		Sphere3.color.V[2] = s3.b;
		Sphere3.radius = s3.radius;
		Sphere3.specular = s3.specular;

		Sphere4.center.V[0] = s4.x;
		Sphere4.center.V[1] = s4.y;
		Sphere4.center.V[2] = s4.z;
		Sphere4.color.V[0] = s4.r;
		Sphere4.color.V[1] = s4.g;
		Sphere4.color.V[2] = s4.b;
		Sphere4.radius = s4.radius;
		Sphere4.specular = s4.specular;

		SpheresInScene[0] = Sphere1;
		SpheresInScene[1] = Sphere2;
		SpheresInScene[2] = Sphere3;
		SpheresInScene[3] = Sphere4;
	}

	void MoveSpheresUp()
	{
		SpheresInScene[0].center.V[1] += 0.1;
		SpheresInScene[1].center.V[1] += 0.1;
		SpheresInScene[2].center.V[1] += 0.1;
	}	
	
	void MoveSpheresDown()
	{
		SpheresInScene[0].center.V[1] -= 0.1;
		SpheresInScene[1].center.V[1] -= 0.1;
		SpheresInScene[2].center.V[1] -= 0.1;
	}
	void MoveSpheresLeft()
	{
		SpheresInScene[0].center.V[0] -= 0.1;
		SpheresInScene[1].center.V[0] -= 0.1;
		SpheresInScene[2].center.V[0] -= 0.1;
	}
	void MoveSpheresRight()
	{
		SpheresInScene[0].center.V[0] += 0.1;
		SpheresInScene[1].center.V[0] += 0.1;
		SpheresInScene[2].center.V[0] += 0.1;
	}
	void MoveSpheresCloser()
	{
		SpheresInScene[0].center.V[2] -= 0.1;
		SpheresInScene[1].center.V[2] -= 0.1;
		SpheresInScene[2].center.V[2] -= 0.1;
	}							   
	void MoveSpheresBack()		   
	{							   
		SpheresInScene[0].center.V[2] += 0.1;
		SpheresInScene[1].center.V[2] += 0.1;
		SpheresInScene[2].center.V[2] += 0.1;
	}
	void MoveSpheresUp(int index)
	{
		SpheresInScene[index].center.V[1] += 0.1;
		/*SpheresInScene[1].center.V[1] += 0.1;
		SpheresInScene[2].center.V[1] += 0.1;*/
	}	
	
	void MoveSpheresDown(int index)
	{
		SpheresInScene[index].center.V[1] -= 0.1;
		/*SpheresInScene[1].center.V[1] -= 0.1;
		SpheresInScene[2].center.V[1] -= 0.1;*/
	}
	void MoveSpheresLeft(int index)
	{
		SpheresInScene[index].center.V[0] -= 0.1;
		/*SpheresInScene[1].center.V[0] -= 0.1;
		SpheresInScene[2].center.V[0] -= 0.1;*/
	}
	void MoveSpheresRight(int index)
	{
		SpheresInScene[index].center.V[0] += 0.1;
		/*SpheresInScene[1].center.V[0] += 0.1;
		SpheresInScene[2].center.V[0] += 0.1;*/
	}
	void MoveSpheresCloser(int index)
	{
		SpheresInScene[index].center.V[2] -= 0.1;
		/*SpheresInScene[1].center.V[2] += 0.1;
		SpheresInScene[2].center.V[2] += 0.1;*/
	}
	void MoveSpheresBack(int index)
	{
		SpheresInScene[index].center.V[2] += 0.1;
		/*SpheresInScene[1].center.V[2] -= 0.1;
		SpheresInScene[2].center.V[2] -= 0.1;*/
	}
	void Reset()
	{
		SpheresInScene[0].center.V[0] = 0;
		SpheresInScene[1].center.V[0] = 2;
		SpheresInScene[2].center.V[0] = -2;
		SpheresInScene[0].center.V[1] = -1;
		SpheresInScene[1].center.V[1] = 0;
		SpheresInScene[2].center.V[1] = 0;
		SpheresInScene[0].center.V[2] = 3;
		SpheresInScene[1].center.V[2] = 4;
		SpheresInScene[2].center.V[2] = 4;
	}

	void SetLights(Light _L1, Light _L2, Light _L3, Light _L4)
	{
			L1 = _L1;
			L2 = _L2;
			L3 = _L3;
			L4 = _L4;

		LightsInScene[0] = L1;
		LightsInScene[1] = L2;
		LightsInScene[2] = L3;
		LightsInScene[3] = L4;
	}

	// 2D distance
	double distance(double x1, double y1, double x2, double y2)
	{
		return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	}

	// translate from chili coordinates to traditional
	double translateX(double x)
	{
		return x - ScreenWidth / 2;
	}
	
	// translate from chili coordinates to traditional
	double translateY(double y)
	{
		return ScreenHeight / 2 - y;
	}

	// Returns index of closest sphere
	int GetClosest(int x, int y)
	{
		int closest = 0;
		int i = 0;
		double temp = 0;
		double dist = DBL_MAX;

		for (; i < sizeof(SpheresInScene) / sizeof(SpheresInScene[0]); i++)
		{
			temp = distance(SpheresInScene[i].center.V[0], translateX(x), SpheresInScene[i].center.V[1], translateY(y));
			if (temp < dist)
			{
				dist = temp;
				closest = i;
			}
		}
		return closest;
	}

	// Set closest sphere's (x,y) position to mouse position
	void MoveSphereWithMouse(int mouseX, int mouseY)
	{
		int index = GetClosest(mouseX, mouseY);
		SpheresInScene[0].center.V[0] = translateX(mouseX);
		SpheresInScene[0].center.V[1] = translateY(mouseY);
	}

private:
	Sphere Sphere1, Sphere2, Sphere3, Sphere4;
	Sphere SpheresInScene[4];
	Sphere* closest_sphere;
	Light L1, L2, L3, L4; 				
	Light LightsInScene[4];	
	init s1_up;
	init s1_down;
	init s2_up;
	init s2_down;
	init s3_up;
	init s3_down;
	init s4_up;
	init s4_down;
	Light _L1, _L2, _L3, _L4; 
};


