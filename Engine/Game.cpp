/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
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
#include "Game.h"
#include "Graphics.h"
#include <Windows.h>


Game::Game(MainWindow& wnd)
	:
	wnd(wnd),
	gfx(wnd)
{
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	if (wnd.kbd.KeyIsPressed(VK_UP))
	{
		gfx.MoveSpheresUp();
	}
	if (wnd.kbd.KeyIsPressed(VK_DOWN))
	{
		gfx.MoveSpheresDown();
	}
	if (wnd.kbd.KeyIsPressed(VK_LEFT))			
	{
		gfx.MoveSpheresLeft();
	}
	if (wnd.kbd.KeyIsPressed(VK_RIGHT))
	{
		gfx.MoveSpheresRight();
	}
	if (wnd.kbd.KeyIsPressed(VK_SPACE))
	{
		gfx.Reset();
	}
}

void Game::ComposeFrame()
{
	// This window is 800 x 600 pixels
	// Center has been translated to the point (400, 300)
	// Main method from computer graphics from scratch ch 1

		for (int x = 1 - Graphics::ScreenWidth / 2; x < Graphics::ScreenWidth / 2; x++)
		{
			for (int y = 1 - Graphics::ScreenHeight / 2; y < Graphics::ScreenHeight / 2; y++)
			{
				gfx.CanvasToViewport(x, y);
				gfx.TraceRay(1, DBL_MAX);
				gfx.PutPixel(Graphics::ScreenWidth / 2 + x, Graphics::ScreenHeight / 2 - y, gfx.COLORS.V[0], gfx.COLORS.V[1], gfx.COLORS.V[2]);
			}
		}
}
