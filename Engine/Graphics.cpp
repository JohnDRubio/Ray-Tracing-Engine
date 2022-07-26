/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.cpp																		  *
*	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
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
#include "MainWindow.h"
#include "Graphics.h"
#include "DXErr.h"
#include "ChiliException.h"
#include <assert.h>
#include <string>
#include <array>
#include <cmath>

using namespace std;

// Ignore the intellisense error "cannot open source file" for .shh files.
// They will be created during the build sequence before the preprocessor runs.
namespace FramebufferShaders
{
#include "FramebufferPS.shh"
#include "FramebufferVS.shh"
}

#pragma comment( lib,"d3d11.lib" )

#define CHILI_GFX_EXCEPTION( hr,note ) Graphics::Exception( hr,note,_CRT_WIDE(__FILE__),__LINE__ )

using Microsoft::WRL::ComPtr;

Graphics::Graphics( HWNDKey& key ): Sphere1(), Sphere2(), Sphere3(), Sphere4(), L1(), L2(), L3(), L4(), s1_up(key), s1_down(key), s2_up(key), s2_down(key), s3_up(key), s3_down(key), s4_up(key), s4_down(key), _L1(), _L2(), _L3(), _L4()
{

	s1_up.set(0, 1, 3, 255, 0, 0, 1, 500);
	s1_down.set(0, -1, 3, 255, 0, 0, 1, 500);
	s2_up.set(2, 2, 4, 0, 0, 255, 1, 500);
	s2_down.set(2, 0, 4, 0, 0, 255, 1, 500);
	s3_up.set(-2, 2, 4, 0, 255, 0, 1, 10);
	s3_down.set(-2, 0, 4, 0, 255, 0, 1, 10);
	s4_up.set(0, -5001, 0, 255, 255, 0, 5000, 1000);
	s4_down.set(0, -5001, 0, 255, 255, 0, 5000, 1000);

	_L1.set(Light::LTYPE::ambient, 0.2);
	_L2.set(Light::LTYPE::point, 0.6, 0, 0, 0, 2, 1, 0);
	_L3.set(Light::LTYPE::directional, 0.2, 1, 4, 4, 0, 0, 0);
	_L4.set(Light::LTYPE::directional, 0, -1, -4, 4, 0, 0, 0);

	this->SetSpheres(s1_down, s2_down, s3_down, s4_down);
	this->SetLights(_L1, _L2, _L3, _L4);

	assert( key.hWnd != nullptr );

	//////////////////////////////////////////////////////
	// create device and swap chain/get render target view
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = Graphics::ScreenWidth;
	sd.BufferDesc.Height = Graphics::ScreenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 60;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = key.hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	HRESULT				hr;
	UINT				createFlags = 0u;
#ifdef CHILI_USE_D3D_DEBUG_LAYER
#ifdef _DEBUG
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
#endif
	
	// create device and front/back buffers
	if( FAILED( hr = D3D11CreateDeviceAndSwapChain( 
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pDevice,
		nullptr,
		&pImmediateContext ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating device and swap chain" );
	}

	// get handle to backbuffer
	ComPtr<ID3D11Resource> pBackBuffer;
	if( FAILED( hr = pSwapChain->GetBuffer(
		0,
		__uuidof( ID3D11Texture2D ),
		(LPVOID*)&pBackBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Getting back buffer" );
	}

	// create a view on backbuffer that we can render to
	if( FAILED( hr = pDevice->CreateRenderTargetView( 
		pBackBuffer.Get(),
		nullptr,
		&pRenderTargetView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating render target view on backbuffer" );
	}


	// set backbuffer as the render target using created view
	pImmediateContext->OMSetRenderTargets( 1,pRenderTargetView.GetAddressOf(),nullptr );


	// set viewport dimensions
	D3D11_VIEWPORT vp;
	vp.Width = float( Graphics::ScreenWidth );
	vp.Height = float( Graphics::ScreenHeight );
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pImmediateContext->RSSetViewports( 1,&vp );


	///////////////////////////////////////
	// create texture for cpu render target
	D3D11_TEXTURE2D_DESC sysTexDesc;
	sysTexDesc.Width = Graphics::ScreenWidth;
	sysTexDesc.Height = Graphics::ScreenHeight;
	sysTexDesc.MipLevels = 1;
	sysTexDesc.ArraySize = 1;
	sysTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sysTexDesc.SampleDesc.Count = 1;
	sysTexDesc.SampleDesc.Quality = 0;
	sysTexDesc.Usage = D3D11_USAGE_DYNAMIC;
	sysTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	sysTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sysTexDesc.MiscFlags = 0;
	// create the texture
	if( FAILED( hr = pDevice->CreateTexture2D( &sysTexDesc,nullptr,&pSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating sysbuffer texture" );
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = sysTexDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// create the resource view on the texture
	if( FAILED( hr = pDevice->CreateShaderResourceView( pSysBufferTexture.Get(),
		&srvDesc,&pSysBufferTextureView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating view on sysBuffer texture" );
	}


	////////////////////////////////////////////////
	// create pixel shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreatePixelShader(
		FramebufferShaders::FramebufferPSBytecode,
		sizeof( FramebufferShaders::FramebufferPSBytecode ),
		nullptr,
		&pPixelShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating pixel shader" );
	}
	

	/////////////////////////////////////////////////
	// create vertex shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateVertexShader(
		FramebufferShaders::FramebufferVSBytecode,
		sizeof( FramebufferShaders::FramebufferVSBytecode ),
		nullptr,
		&pVertexShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating vertex shader" );
	}
	

	//////////////////////////////////////////////////////////////
	// create and fill vertex buffer with quad for rendering frame
	const FSQVertex vertices[] =
	{
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,1.0f,0.5f,1.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,-1.0f,0.5f,0.0f,1.0f },
	};
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( FSQVertex ) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;
	if( FAILED( hr = pDevice->CreateBuffer( &bd,&initData,&pVertexBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating vertex buffer" );
	}

	
	//////////////////////////////////////////
	// create input layout for fullscreen quad
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateInputLayout( ied,2,
		FramebufferShaders::FramebufferVSBytecode,
		sizeof( FramebufferShaders::FramebufferVSBytecode ),
		&pInputLayout ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating input layout" );
	}


	////////////////////////////////////////////////////
	// Create sampler state for fullscreen textured quad
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if( FAILED( hr = pDevice->CreateSamplerState( &sampDesc,&pSamplerState ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating sampler state" );
	}

	// allocate memory for sysbuffer (16-byte aligned for faster access)
	pSysBuffer = reinterpret_cast<Color*>( 
		_aligned_malloc( sizeof( Color ) * Graphics::ScreenWidth * Graphics::ScreenHeight,16u ) );
}

Graphics::~Graphics()
{
	// free sysbuffer memory (aligned free)
	if( pSysBuffer )
	{
		_aligned_free( pSysBuffer );
		pSysBuffer = nullptr;
	}
	// clear the state of the device context before destruction
	if( pImmediateContext ) pImmediateContext->ClearState();
}

void Graphics::EndFrame()
{
	HRESULT hr;

	// lock and map the adapter memory for copying over the sysbuffer
	if( FAILED( hr = pImmediateContext->Map( pSysBufferTexture.Get(),0u,
		D3D11_MAP_WRITE_DISCARD,0u,&mappedSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Mapping sysbuffer" );
	}
	// setup parameters for copy operation
	Color* pDst = reinterpret_cast<Color*>(mappedSysBufferTexture.pData );
	const size_t dstPitch = mappedSysBufferTexture.RowPitch / sizeof( Color );
	const size_t srcPitch = Graphics::ScreenWidth;
	const size_t rowBytes = srcPitch * sizeof( Color );
	// perform the copy line-by-line
	for( size_t y = 0u; y < Graphics::ScreenHeight; y++ )
	{
		memcpy( &pDst[ y * dstPitch ],&pSysBuffer[y * srcPitch],rowBytes );
	}
	// release the adapter memory
	pImmediateContext->Unmap( pSysBufferTexture.Get(),0u );

	// render offscreen scene texture to back buffer
	pImmediateContext->IASetInputLayout( pInputLayout.Get() );
	pImmediateContext->VSSetShader( pVertexShader.Get(),nullptr,0u );
	pImmediateContext->PSSetShader( pPixelShader.Get(),nullptr,0u );
	pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	const UINT stride = sizeof( FSQVertex );
	const UINT offset = 0u;
	pImmediateContext->IASetVertexBuffers( 0u,1u,pVertexBuffer.GetAddressOf(),&stride,&offset );
	pImmediateContext->PSSetShaderResources( 0u,1u,pSysBufferTextureView.GetAddressOf() );
	pImmediateContext->PSSetSamplers( 0u,1u,pSamplerState.GetAddressOf() );
	pImmediateContext->Draw( 6u,0u );

	// flip back/front buffers
	if( FAILED( hr = pSwapChain->Present( 1u,0u ) ) )
	{
		if( hr == DXGI_ERROR_DEVICE_REMOVED )
		{
			throw CHILI_GFX_EXCEPTION( pDevice->GetDeviceRemovedReason(),L"Presenting back buffer [device removed]" );
		}
		else
		{
			throw CHILI_GFX_EXCEPTION( hr,L"Presenting back buffer" );
		}
	}
}

void Graphics::BeginFrame()
{
	// clear the sysbuffer
	memset( pSysBuffer,0u,sizeof( Color ) * Graphics::ScreenHeight * Graphics::ScreenWidth );
}

void Graphics::PutPixel( int x,int y,Color c )
{
	assert( x >= 0 );
	assert( x < int( Graphics::ScreenWidth ) );
	assert( y >= 0 );
	assert( y < int( Graphics::ScreenHeight ) );
	pSysBuffer[Graphics::ScreenWidth * y + x] = c;
}

//void Graphics::PutPixel2(int x, int y, Color c)
//{
//	//assert(x >= 0);
//	//assert(x < int(Graphics::ScreenWidth));
//	//assert(y >= 0);
//	//assert(y < int(Graphics::ScreenHeight)); 
//
//	// translate origin to center
//	x = 400 + x;
//	y = 300 - y;
//
//	pSysBuffer[Graphics::ScreenWidth * y + x] = c;
//
//}

//void Graphics::PutPixel3(int x, int y, Color c)
//{
//	//assert(x >= 0);
//	//assert(x < int(Graphics::ScreenWidth));
//	//assert(y >= 0);
//	//assert(y < int(Graphics::ScreenHeight)); 
//
//	// translate origin to center
//	x = 400 + x;
//	y = 300 - y;
//
//	pSysBuffer[Graphics::ScreenWidth * y + x] = c;
//}

//////////////////////////////////////////////////
//           Graphics Exception
Graphics::Exception::Exception( HRESULT hr,const std::wstring& note,const wchar_t* file,unsigned int line )
	:
	ChiliException( file,line,note ),
	hr( hr )
{}

std::wstring Graphics::Exception::GetFullMessage() const
{
	const std::wstring empty = L"";
	const std::wstring errorName = GetErrorName();
	const std::wstring errorDesc = GetErrorDescription();
	const std::wstring& note = GetNote();
	const std::wstring location = GetLocation();
	return    (!errorName.empty() ? std::wstring( L"Error: " ) + errorName + L"\n"
		: empty)
		+ (!errorDesc.empty() ? std::wstring( L"Description: " ) + errorDesc + L"\n"
			: empty)
		+ (!note.empty() ? std::wstring( L"Note: " ) + note + L"\n"
			: empty)
		+ (!location.empty() ? std::wstring( L"Location: " ) + location
			: empty);
}

std::wstring Graphics::Exception::GetErrorName() const
{
	return DXGetErrorString( hr );
}

std::wstring Graphics::Exception::GetErrorDescription() const
{
	std::array<wchar_t,512> wideDescription;
	DXGetErrorDescription( hr,wideDescription.data(),wideDescription.size() );
	return wideDescription.data();
}

std::wstring Graphics::Exception::GetExceptionType() const
{
	return L"Chili Graphics Exception";
}

// Viewport parameters
// Looks slightly less distorted with height:width = 3:4
const int ViewportHeight = 1;
const int ViewportWidth = 1;

// Gives direction of ray 
void Graphics::CanvasToViewport(int x, int y)
{
	D[0] = (double)x * (double)ViewportWidth / (double)Graphics::ScreenWidth;
	D[1] = (double)y * (double)ViewportHeight / (double)Graphics::ScreenHeight;
	D[2] = 1;
}

// 3d dot product
int Graphics::dot(int a[], int b[])
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

// 3d dot product
double Graphics::dot(double a[], double b[])
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

// 3d dot product
double Graphics::dot(double a[], int b[])
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

// 3d vector addition
void Graphics::VecAdd(double a[3], double b[3], double c[3])
{
	c[0] = a[0] + b[0];
	c[1] = a[1] + b[1];
	c[2] = a[2] + b[2];

}

// 3d vector subtraction
void Graphics::VecSub(double a[3], double b[3], double c[3])
{
	c[0] = a[0] - b[0];
	c[1] = a[1] - b[1];
	c[2] = a[2] - b[2];
}

// scalar * vector
void Graphics::scaleVec(double s, double a[3], double b[3])
{
	b[0] = s * a[0];
	b[1] = s * a[1];
	b[2] = s * a[2];
}

// magnitude
double Graphics::vecLength(double a[3])
{
	return (sqrt(dot(a, a)));
}

// Bound pixel values
void Graphics::BoundPixelValues()
{
	for (int i = 0; i < 3; i++)
		if (COLORS[i] > 255)
			COLORS[i] = 255;
}

//Sphere SpheresInScene[4];

// Set position

//void Graphics::SetSphere(Sprite s1, Sprite s2, Sprite s3, Sprite s4)
//{
//	//Spheres in the scene
//	Sphere Sphere1(s1.x, s1.y, s1.z, s1.r, s1.g, s1.b, s1.radius, s1.specular);
//	Sphere Sphere2(s2.x, s2.y, s2.z, s2.r, s2.g, s2.b, s2.radius, s2.specular);
//	Sphere Sphere3(s3.x, s3.y, s3.z, s3.r, s3.g, s3.b, s3.radius, s3.specular);
//	Sphere Sphere4(s4.x, s4.y, s4.z, s4.r, s4.g, s4.b, s4.radius, s4.specular);
//
//	SpheresInScene[0] = Sphere1;
//	SpheresInScene[1] = Sphere2;
//	SpheresInScene[2] = Sphere3;
//	SpheresInScene[3] = Sphere4;
//
//	//SpheresInScene = { Sphere1, Sphere2, Sphere3, Sphere4 };
//}

//Light
//Light L1(Light::LTYPE::ambient, 0.2);					// <- DO SAME THING YOU DID FOR SPHERES (don't initialize each time)
//Light L2(Light::LTYPE::point, 0.6, 0, 0, 0, 2, 1, 0);
//Light L3(Light::LTYPE::directional, 0.2,1,4,4,0,0,0);
//Light L4(Light::LTYPE::directional, 0,-1,-4,4,0,0,0);
//Light LightsInScene[] = { L1,L2,L3, L4 };

// Returns color of sphere that ray intersects
void Graphics::IntersectRaySphere(int index)
{
	MISS = 1;
	CAMERA[0] = 0;
	CAMERA[1] = 0;
	CAMERA[2] = 0;

	double CO[3]{};
	VecSub(CAMERA, new double[3] { (double)SpheresInScene[index].center[0], (double)SpheresInScene[index].center[1], (double)SpheresInScene[index].center[2] }, CO);

	double r = SpheresInScene[index].radius;

	double a = dot(D, D);
	double b = 2*dot(CO, D);
	double c = dot(CO, CO) - r * r;
	double discriminant = b * b - 4 * a * c;

	if (discriminant < 0) {
		INT_PTS[0] = INT_PTS[1] = INT_MAX;
		MISS = 1;
		return;
	}

	else 
	{
		MISS = 0;
		double t1 = (( - 1 * b + sqrt(discriminant)) / (2 * a));
		double t2 = (( - 1 * b - sqrt(discriminant)) / (2 * a));
		INT_PTS[0] = t1;
		INT_PTS[1] = t2;
	}
}

void Graphics::TraceRay(double t_min, double t_max)
{
	double closest_t = DBL_MAX;
	Sphere* closest_sphere = NULL;
	double intensity = 0.0;

	//Compute pixel color
	for (int i = 0; i < sizeof(SpheresInScene) / sizeof(SpheresInScene[0]); i++)
	{
		IntersectRaySphere(i);

		if (!MISS)
		{
			if (INT_PTS[0] > t_min && INT_PTS[0] < t_max && INT_PTS[0] < closest_t) {
				closest_t = INT_PTS[0];
				closest_sphere = &SpheresInScene[i];
			}

			if (INT_PTS[1] > t_min && INT_PTS[1] < t_max && INT_PTS[1] <= closest_t) {
				closest_t = INT_PTS[1];
				closest_sphere = &SpheresInScene[i];
			}

			if (closest_t != DBL_MAX && closest_t >= 1)
			{
				// This condition accounts for case of more than one sphere being intersected
				if ((INT_PTS[0] > t_min && INT_PTS[0] < t_max && INT_PTS[0] <= closest_t) || (INT_PTS[1] > t_min && INT_PTS[1] < t_max && INT_PTS[1] <= closest_t))
				{					
					// Set P = O + t * D
					double tD[3]{};
					scaleVec(closest_t, D, tD);
					VecAdd(CAMERA, tD, P);

					if (INT_PTS[0] <= closest_t || INT_PTS[1] <= closest_t)
					{
						// shading
						intensity = ComputeLighting(P, new double[3]{-1 * D[0], -1 * D[1], -1 * D[2]}, i);
						
						// multiply pixel values by intensity
						scaleVec(intensity, new double[3]{ (double)closest_sphere->color[0], (double)closest_sphere->color[1], (double)closest_sphere->color[2] }, COLORS);

						// Bound pixel values
						BoundPixelValues();
					}
				}
			}
		}
		else 
		{
			if (closest_sphere == NULL) 
			{
				COLORS[0] = 255;
				COLORS[1] = 255;
				COLORS[2] = 255;
			}
		}
	}
}

//Compute diffuse light
double Graphics::ComputeLighting(double POS[3], double V[3], int index)
{
	// Initialize values
	double intensity = 0.0;
	double CENTER[3] = { SpheresInScene[index].center[0], SpheresInScene[index].center[1], SpheresInScene[index].center[2] };
	double L[3]{};
	double P_minus_C[3]{};
	double Normal[3]{};
	double length_P_minus_C;

	// Set P_minus_C
	VecSub(P, CENTER, P_minus_C);
	length_P_minus_C = vecLength(P_minus_C);

	// Set magnitude of Normal to 1
	scaleVec(1 / length_P_minus_C, P_minus_C, Normal);

	for (int i = 0; i < sizeof(LightsInScene) / sizeof(LightsInScene[0]); i++)
	{
		if (LightsInScene[i].type == Light::LTYPE::ambient)		// when refactoring, use enums for light type
		{
			intensity += LightsInScene[i].intensity;
		}
		else
		{
			if (LightsInScene[i].type == Light::LTYPE::point)
			{
				VecSub(new double[3]{ LightsInScene[i].POSITION[0], LightsInScene[i].POSITION[1], LightsInScene[i].POSITION[2] }, POS, L);
			}
			else
			{
				if (LightsInScene[i].type == Light::LTYPE::directional)
				{
					for (int j = 0; j < sizeof(LightsInScene[i].DIRECTION) / sizeof(LightsInScene[i].DIRECTION[0]); j ++)
					{
						L[j] = LightsInScene[i].DIRECTION[j];
					}
				}
			}

			double n_dot_L = dot(Normal, L);

			if (n_dot_L > 0)
				intensity += LightsInScene[i].intensity * (n_dot_L / (vecLength(Normal) * vecLength(L)));

			if (SpheresInScene[index].specular != -1)
			{
				double R[3]{};

				// R = 2 * N * <N,L> - L
				scaleVec(2 * n_dot_L, Normal, R);
				VecSub(R, L, R);

				double r_dot_v = dot(R, V);

				if (r_dot_v > 0)
					intensity += LightsInScene[i].intensity * pow(r_dot_v / (vecLength(R) * vecLength(V)), SpheresInScene[index].specular);
			}
		}
	}
	return intensity;
}