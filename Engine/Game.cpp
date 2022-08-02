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
		if (wnd.kbd.KeyIsPressed('A'))
			gfx.MoveSpheresUp(2);
		else if (wnd.kbd.KeyIsPressed('S'))
			gfx.MoveSpheresUp(0);
		else if (wnd.kbd.KeyIsPressed('D'))
			gfx.MoveSpheresUp(1);
		else
			gfx.MoveSpheresUp();
	}
	if (wnd.kbd.KeyIsPressed(VK_DOWN))
	{
		if (wnd.kbd.KeyIsPressed('A'))
			gfx.MoveSpheresDown(2);
		else if (wnd.kbd.KeyIsPressed('S'))
			gfx.MoveSpheresDown(0);
		else if (wnd.kbd.KeyIsPressed('D'))
			gfx.MoveSpheresDown(1);
		else
			gfx.MoveSpheresDown();
	}
	if (wnd.kbd.KeyIsPressed(VK_LEFT))			
	{
		if (wnd.kbd.KeyIsPressed('A'))
			gfx.MoveSpheresLeft(2);
		else if (wnd.kbd.KeyIsPressed('S'))
			gfx.MoveSpheresLeft(0);
		else if (wnd.kbd.KeyIsPressed('D'))
			gfx.MoveSpheresLeft(1);
		else
			gfx.MoveSpheresLeft();
	}
	if (wnd.kbd.KeyIsPressed(VK_RIGHT))
	{
		if (wnd.kbd.KeyIsPressed('A'))
			gfx.MoveSpheresRight(2);
		else if (wnd.kbd.KeyIsPressed('S'))
			gfx.MoveSpheresRight(0);
		else if (wnd.kbd.KeyIsPressed('D'))
			gfx.MoveSpheresRight(1);
		else
			gfx.MoveSpheresRight();
	}
	if (wnd.kbd.KeyIsPressed(VK_CONTROL))
	{
		if (wnd.kbd.KeyIsPressed('A'))
			gfx.MoveSpheresCloser(2);
		else if (wnd.kbd.KeyIsPressed('S'))
			gfx.MoveSpheresCloser(0);
		else if (wnd.kbd.KeyIsPressed('D'))
			gfx.MoveSpheresCloser(1);
		else
			gfx.MoveSpheresCloser();
	}

	// 0 = RED, 1 = BLUE, 2  = GREEN
	if (wnd.kbd.KeyIsPressed(VK_SHIFT))
	{
		if (wnd.kbd.KeyIsPressed('A'))
			gfx.MoveSpheresBack(2);
		else if (wnd.kbd.KeyIsPressed('S'))
			gfx.MoveSpheresBack(0);
		else if (wnd.kbd.KeyIsPressed('D'))
			gfx.MoveSpheresBack(1);
		else
			gfx.MoveSpheresBack();
	}
	if (wnd.kbd.KeyIsPressed(VK_SPACE))
	{
		gfx.Reset();
	}
	//if (wnd.mouse.LeftIsPressed())
	//{
	//	gfx.MoveSphereWithMouse(wnd.mouse.GetPosX(), wnd.mouse.GetPosY());
	//}
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
