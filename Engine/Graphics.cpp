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
#include "Vec3.h"
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

// Gives direction of ray 
void Graphics::CanvasToViewport(int x, int y)
{
	D.V[0] = (double)x * (double)ViewportWidth / (double)Graphics::ScreenWidth;
	D.V[1] = (double)y * (double)ViewportHeight / (double)Graphics::ScreenHeight;
	D.V[2] = 1;
}

void Graphics::SetSpheres(init s1, init s2, init s3, init s4)
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

void Graphics::MoveSpheresUp()
{
	SpheresInScene[0].center.V[1] += 0.1;
	SpheresInScene[1].center.V[1] += 0.1;
	SpheresInScene[2].center.V[1] += 0.1;
}

void Graphics::MoveSpheresDown()
{
	SpheresInScene[0].center.V[1] -= 0.1;
	SpheresInScene[1].center.V[1] -= 0.1;
	SpheresInScene[2].center.V[1] -= 0.1;
}
void Graphics::MoveSpheresLeft()
{
	SpheresInScene[0].center.V[0] -= 0.1;
	SpheresInScene[1].center.V[0] -= 0.1;
	SpheresInScene[2].center.V[0] -= 0.1;
}
void Graphics::MoveSpheresRight()
{
	SpheresInScene[0].center.V[0] += 0.1;
	SpheresInScene[1].center.V[0] += 0.1;
	SpheresInScene[2].center.V[0] += 0.1;
}
void Graphics::MoveSpheresCloser()
{
	SpheresInScene[0].center.V[2] -= 0.1;
	SpheresInScene[1].center.V[2] -= 0.1;
	SpheresInScene[2].center.V[2] -= 0.1;
}
void Graphics::MoveSpheresBack()
{
	SpheresInScene[0].center.V[2] += 0.1;
	SpheresInScene[1].center.V[2] += 0.1;
	SpheresInScene[2].center.V[2] += 0.1;
}
void Graphics::MoveSpheresUp(int index)
{
	SpheresInScene[index].center.V[1] += 0.1;
}

void Graphics::MoveSpheresDown(int index)
{
	SpheresInScene[index].center.V[1] -= 0.1;
}
void Graphics::MoveSpheresLeft(int index)
{
	SpheresInScene[index].center.V[0] -= 0.1;
}
void Graphics::MoveSpheresRight(int index)
{
	SpheresInScene[index].center.V[0] += 0.1;
}
void Graphics::MoveSpheresCloser(int index)
{
	SpheresInScene[index].center.V[2] -= 0.1;
}
void Graphics::MoveSpheresBack(int index)
{
	SpheresInScene[index].center.V[2] += 0.1;
}
void Graphics::Reset()
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

void Graphics::SetLights(Light _L1, Light _L2, Light _L3, Light _L4)
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

// 3d dot product
double Graphics::Graphics::dot(const Vec3& lhs, const Vec3& rhs)
{
	return lhs.V[0] * rhs.V[0] + lhs.V[1] * rhs.V[1] + lhs.V[2] * rhs.V[2];
}

// Magnitude
double Graphics::vecLength(const Vec3& v)
{
	return (sqrt(dot(v, v)));
}

// Bound pixel values
void Graphics::BoundPixelValues()
{
	for (int i = 0; i < 3; i++)
		if (COLORS.V[i] > 255)
			COLORS.V[i] = 255;
}

// Returns color of sphere that ray intersects
void Graphics::IntersectRaySphere(int index)
{
	MISS = 1;
	Vec3 CO;
	CO = CAMERA - SpheresInScene[index].center;

	float r = SpheresInScene[index].radius;
	float a = dot(D, D);
	float b = 2*dot(CO, D);
	float c = dot(CO, CO) - r * r;
	float discriminant = b * b - 4 * a * c;

	if (discriminant < 0) {
		INT_PTS[0] = INT_PTS[1] = INT_MAX;
		MISS = 1;
		return;
	}

	else 
	{
		MISS = 0;
		INT_PTS[0] = (( - 1 * b + sqrt(discriminant)) / (2 * a));
		INT_PTS[1] = (( - 1 * b - sqrt(discriminant)) / (2 * a));
	}
}

void Graphics::IntersectRaySphere(Vec3& P, Vec3& L, int index)
{
	MISS = 1;
	Vec3 CP;
	CP = P - SpheresInScene[index].center;

	float r = SpheresInScene[index].radius;
	float a = dot(L, L);
	float b = 2*dot(CP, L);
	float c = dot(CP, CP) - r * r;
	float discriminant = b * b - 4 * a * c;

	if (discriminant < 0) {
		INT_PTS[0] = INT_PTS[1] = INT_MAX;
		MISS = 1;
		return;
	}

	else 
	{
		MISS = 0;
		INT_PTS[0] = (( - 1 * b + sqrt(discriminant)) / (2 * a));
		INT_PTS[1] = (( - 1 * b - sqrt(discriminant)) / (2 * a));
	}
}

void Graphics::TraceRay(double t_min, double t_max)
{
	closest_t = DBL_MAX;
	closest_sphere = NULL;
	intensity = 0.0;

	// Compute pixel color
	for (int i = 0; i < 4; i++)		// If more spheres are added, replace 4 with sizeof(SpheresInScene) / sizeof(SpheresInScene[0])
	{

		// Test if ray intersects Sphere[i]
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
					P = CAMERA + closest_t * D;

					if (INT_PTS[0] <= closest_t || INT_PTS[1] <= closest_t)
					{
						// Shading
						intensity = ComputeLighting(i);
						COLORS = intensity * closest_sphere->color;

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
				COLORS.V[0] = 255;
				COLORS.V[1] = 255;
				COLORS.V[2] = 255;
			}
		}
	}
}

bool Graphics::ClosestIntersection(Vec3& P, Vec3& L, double t_min, double t_max)
{
	double closest_t2 = DBL_MAX;
	Sphere* closest_sphere2 = NULL;

	for (int i = 0; i < 4; i++)		// If more spheres are added, replace 4 with sizeof(SpheresInScene) / sizeof(SpheresInScene[0])
	{

		// Test if ray intersects Sphere[i]
		IntersectRaySphere(P, L, i);

		if (!MISS)
		{
			if (INT_PTS[0] > t_min && INT_PTS[0] < t_max && INT_PTS[0] < closest_t2) {
				closest_t2 = INT_PTS[0];
				closest_sphere2 = &SpheresInScene[i];
			}

			if (INT_PTS[1] > t_min && INT_PTS[1] < t_max && INT_PTS[1] <= closest_t2) {
				closest_t2 = INT_PTS[1];
				closest_sphere2 = &SpheresInScene[i];
			}
			if (closest_sphere2 == NULL)
			{
				return false;
			}
			return true;
		}
	}
}

// Compute Lighting
double Graphics::ComputeLighting(int index)
{
	// Initialize values
	double length_P_minus_C, t_max;
	Vec3 L;
	Vec3 P_Minus_C;
	Vec3 Normal;
	intensity = 0;
	t_max = DBL_MAX;


	// Set P_minus_C
	P_Minus_C = P - SpheresInScene[index].center;
	length_P_minus_C = vecLength(P_Minus_C);

	// Set magnitude of Normal to 1
	Normal = (1 / length_P_minus_C) * P_Minus_C;

	for (int i = 0; i < 3; i++)		// If more lights are added to scene, replace 3 with sizeof(LightsInScene) / sizeof(LightsInScene[0])
	{
		if (LightsInScene[i].type == Light::LTYPE::ambient)		
		{
			intensity += LightsInScene[i].intensity;
		}
		else
		{
			if (LightsInScene[i].type == Light::LTYPE::point)
			{
				L = LightsInScene[i].POSITION - P;
				t_max = 1;
			}
			else
			{
				if (LightsInScene[i].type == Light::LTYPE::directional)
				{
					L = LightsInScene[i].DIRECTION;
					t_max = DBL_MAX;
				}
			}

			// Test whether there is an object between the point, P, and the light source
			if (ClosestIntersection(P, L, 0.001, t_max))
				continue;

			double n_dot_L = dot(Normal, L);

			// Diffuse light
			if (n_dot_L > 0)
				intensity += LightsInScene[i].intensity * (n_dot_L / (vecLength(Normal) * vecLength(L)));

			// Specular light
			if (SpheresInScene[index].specular != -1)
			{
				Vec3 R;
				Vec3 V = -1 * D;

				// R = 2 * <N,L> * N - L
				R = 2 * n_dot_L * Normal - L;

				double r_dot_v = dot(R, V);

				if (r_dot_v > 0)
					intensity += LightsInScene[i].intensity * pow(r_dot_v / (vecLength(R) * vecLength(V)), SpheresInScene[index].specular);
			}
		}
	}
	return intensity;
}