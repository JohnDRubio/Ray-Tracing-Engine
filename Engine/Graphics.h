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
#include "Light.h"
#include "Sphere.h"
#include "Init.h"
#include <vector>
#include <string>
using namespace std;

class Graphics
{
public:
	Vec3 D;
	Vec3 P;
	Vec3 CAMERA;
	Vec3 COLORS;
	double INT_PTS[2]{};
	bool MISS = 1;

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
	
	void PutPixel(int x, int y)
	{
		PutPixel(Graphics::ScreenWidth / 2 + x, Graphics::ScreenHeight / 2 - y, COLORS.V[0], COLORS.V[1], COLORS.V[2]);
	}

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
	static constexpr int ViewportHeight = 1;
	static constexpr int ViewportWidth = 1;
	float intensity;							// color intensity
	float closest_t;							// time of closest intersection
	void CanvasToViewport(int x, int y);
	void TraceRay(double, double);
	bool ClosestIntersection(Vec3&, Vec3&, double, double);
	void IntersectRaySphere(Vec3&, Vec3&, int index);
	void IntersectRaySphere(int);
	double ComputeLighting(int index);
	double dot(const Vec3&, const Vec3&);
	double vecLength(const Vec3&);
	void BoundPixelValues();
	void SetSpheres(init, init, init, init);
	void MoveSpheresUp();
	void MoveSpheresDown();
	void MoveSpheresLeft();
	void MoveSpheresRight();
	void MoveSpheresCloser();				   
	void MoveSpheresBack();
	void MoveSpheresUp(int index);
	void MoveSpheresDown(int index);
	void MoveSpheresLeft(int index);
	void MoveSpheresRight(int index);
	void MoveSpheresCloser(int index);
	void MoveSpheresBack(int index);
	void Reset();
	void SetLights(Light _L1, Light _L2, Light _L3, Light _L4);

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