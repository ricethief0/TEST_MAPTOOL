#include "stdafx.h"
#include "mapToolScene.h"

HRESULT mapToolScene::init()
{
	IMAGEMANAGER->addFrameImage("tile", "Images/tile.bmp", 144, 144, 3, 3);
	IMAGEMANAGER->addImage("tileSet", "Images/tile.bmp", 144, 144, true, RGB(255, 0, 255));

	//제작할곳
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 17; j++)
		{
			int order = 17 * i + j;

			tile[order].rc = RectMake(200 + ((j % 17) * 48), 130 + ((i % 9) * 48), 48, 48);
			tile[order].active = false;
			tile[order].isCol = false;

			tile[order].frame = { 0,0 };
		}
	}
	
	//가져올곳 <단일>
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int order = 3 * i + j;

			sTile[order].frame = { j,i };

			sTile[order].rc = RectMake(1100 + (j * 55), 380+ (i * 55), 48, 48);
		}
	}

	//가져올곳 <세트>
	sTile_set.rc = RectMake(1107, 180, 144, 144);

	//mouse
	memset(&mouse, 0, sizeof(mouse));

	//button down
	isLeftDown = isLeftkey = false;
	return S_OK;
}

void mapToolScene::release()
{
}

void mapToolScene::update()
{
	//tile set
	dragTile();

	//tile get < SINGLE >
	if (isLeftDown)
	{
		for (int i = 0; i < 9; i++)
		{
			if (PtInRect(&sTile[i].rc, _ptMouse))
			{
				mouse.frame = sTile[i].frame;
				//mouse.frame.x = sTile[i].frame.x;
				//mouse.frame.y = sTile[i].frame.y;
				state = DRAW;
				mouse.active = true;
			}
		}
	}
	//tile get < SET >
	if (isLeftDown)
	{
		if (PtInRect(&sTile_set.rc, _ptMouse))
		{
			state = DRAW_SET;
			mouse.active = true;
		}
	}

	//MOUSE.active = draw나 draw_set 상태일 때(true), erase 상태일 때(false)
	/*
	state로 draw,draw_set,erase 구분
	*/

	
	//MODE

	//erace mode
	if (INPUT->GetKeyDown('E'))
	{
		if (state != ERASE)mouse.saveBrush = state;
		mouse.active = false;
	}

	//brush mode
	if (INPUT->GetKeyDown('B'))
	{
		if (mouse.saveBrush != NONE)state = mouse.saveBrush;
		mouse.active = true;
	}

	if (!mouse.active) state = ERASE;

	if (INPUT->GetKeyDown(VK_SPACE))init();

	if (INPUT->GetKeyDown('S')) save();
	if (INPUT->GetKeyDown('L')) load();

	if (INPUT->GetKeyDown('Z')) UNDO();

	//button
	if (INPUT->GetKeyDown(VK_LBUTTON)) isLeftDown = true;
	else isLeftDown = false;
	if (INPUT->GetKey(VK_LBUTTON))isLeftkey = true;
	else isLeftkey = false;
}

void mapToolScene::render()
{
	//render
	for (int i = 0; i < 153; i++)
	{
		if (!tile[i].active)
		{
			if (!tile[i].isCol)
			{
				Rectangle(getMemDC(), tile[i].rc);
			}
			else
			{
				HPEN hPen, oPen;
				hPen = CreatePen(PS_SOLID, 2, GREEN);
				oPen = (HPEN)SelectObject(getMemDC(), hPen);
				Rectangle(getMemDC(), tile[i].rc);
				
				SelectObject(getMemDC(), oPen);
				DeleteObject(hPen);
			}
		}
		else
		{
			if (!tile[i].isCol) Rectangle(getMemDC(), tile[i].rc);

			if (tile[i].active && i < 136 && tile[i + 17].active)
			{
				if (checkPt(tile[i + 17].frame, { 0,1 }) && checkPt(tile[i].frame, { 1,0 }))
				{
					tile[i].frame = { 0,0 };
				}
				else if (checkPt(tile[i + 17].frame, { 2,1 }) && checkPt(tile[i].frame, { 1,0 }))
				{
					tile[i].frame = { 2,0 };
				}
				else IMAGEMANAGER->frameRender("tile", getMemDC(), tile[i].rc.left, tile[i].rc.top, tile[i].frame.x, tile[i].frame.y);
			}
			else if (tile[i].active && i > 16 && tile[i - 17].active)
			{
				if (checkPt(tile[i - 17].frame, { 0,1 }) && checkPt(tile[i].frame, { 1,2 }))
				{
					tile[i].frame = { 0,2 };
				}
				else if (checkPt(tile[i - 17].frame, { 2,1 }) && checkPt(tile[i].frame, { 1,2 }))
				{
					tile[i].frame = { 2,2 };
				}
				else  IMAGEMANAGER->frameRender("tile", getMemDC(), tile[i].rc.left, tile[i].rc.top, tile[i].frame.x, tile[i].frame.y);
			}
			else IMAGEMANAGER->frameRender("tile", getMemDC(), tile[i].rc.left, tile[i].rc.top, tile[i].frame.x, tile[i].frame.y);

			if (tile[i].isCol)
			{
				HBRUSH hBrush, oBrush;
				HPEN hPen, oPen;

				//hBrush = CreateSolidBrush(RGB(255, 255, 255));
				hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
				oBrush = (HBRUSH)SelectObject(getMemDC(), hBrush);
				SetBkMode(getMemDC(), TRANSPARENT);

				hPen = CreatePen(PS_SOLID, 4, RED);
				oPen = (HPEN)SelectObject(getMemDC(), hPen);
				Rectangle(getMemDC(), tile[i].rc);

				SelectObject(getMemDC(), oPen);
				SelectObject(getMemDC(), oBrush);

				DeleteObject(hPen);
				DeleteObject(hBrush);
			}
		}
	}

	//mouse img
	if (mouse.active)
	{
		if(state == DRAW_SET) IMAGEMANAGER->render("tileSet", getMemDC(), _ptMouse.x, _ptMouse.y);
		else IMAGEMANAGER->frameRender("tile", getMemDC(), _ptMouse.x - 24, _ptMouse.y - 24, mouse.frame.x, mouse.frame.y);
	}

	//fremerender
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int order = 3 * i + j;

			//Rectangle(getMemDC(), sTile[order].rc);
			IMAGEMANAGER->frameRender("tile", getMemDC(), sTile[order].rc.left, sTile[order].rc.top,
				sTile[order].frame.x, sTile[order].frame.y);
		}
	}

	IMAGEMANAGER->render("tileSet", getMemDC(), sTile_set.rc.left, sTile_set.rc.top);


	if (isLeftkey)
	{
		switch (state)
		{
		case DRAW:
			FrameRect(getMemDC(), mouse.rc, BLACK);
			break;
		case DRAW_SET:
			FrameRect(getMemDC(), mouse.rc, BLACK);
			break;
		}
	}	

	textRender(); //모드 확인용
}

void mapToolScene::save()
{
	HANDLE file;
	DWORD write;

	file = CreateFile("mapData/save.map", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(file, tile, sizeof(tagMap) * 17 * 9, &write, NULL);
	CloseHandle(file);
}

void mapToolScene::load()
{
	HANDLE file;
	DWORD read;

	file = CreateFile("mapData/save.map", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	ReadFile(file, tile, sizeof(tagMap) * 17 * 9, &read, NULL);
	CloseHandle(file);
}

void mapToolScene::dragTile()
{
	if (isLeftDown)
	{
		mouse.start = _ptMouse;
	}
	if (isLeftkey)
	{
		mouse.end = _ptMouse;
	}

	if (mouse.start.y < mouse.end.y)
	{
		if (mouse.start.x < mouse.end.x) mouse.rc = RectMake(mouse.start.x, mouse.start.y, mouse.end.x - mouse.start.x, mouse.end.y - mouse.start.y);
		else mouse.rc = RectMake(mouse.end.x, mouse.start.y, mouse.start.x - mouse.end.x, mouse.end.y - mouse.start.y);
	}
	else
	{
		if (mouse.start.x > mouse.end.x) mouse.rc = RectMake(mouse.end.x, mouse.end.y, mouse.start.x - mouse.end.x, mouse.start.y - mouse.end.y);
		else mouse.rc = RectMake(mouse.start.x, mouse.end.y, mouse.end.x - mouse.start.x, mouse.start.y - mouse.end.y);
	}

	//VK_LBUTTON을 안눌렀을 때
	if(!isLeftkey) mouse.start = mouse.end = { 0,0 };

	//COL
	RECT temp;
	for (int i = 0; i < 153; i++)
	{
		if (IntersectRect(&temp, &mouse.rc, &tile[i].rc))
		{
			sIndex.push_back(i);
			sIndex.begin();
			
			tile[i].isCol = true;
		}
		else tile[i].isCol = false;
	}
	if (INPUT->GetKeyUp(VK_LBUTTON))
	{
		for (int i = 0; i < 153; i++)
		{
			if (IntersectRect(&temp, &mouse.rc, &tile[i].rc))
			{
				switch (state)
				{
				case DRAW:
					tile[i].active = true;
					tile[i].frame = mouse.frame;
					break;
				case DRAW_SET:
					tile[i].active = true;

					getMinMaxIndex();

					if(i == mouse.minIndex)tile[mouse.minIndex].frame = { 0,0 };
					else if (i == mouse.maxIndex)tile[mouse.maxIndex].frame = { 2,2 };
					else if (mouse.maxIndex - 17 > 0)
					{
						for (int i = 1; i < 20; i++)
						{
							if (mouse.maxIndex - (17 * (i+1)) > mouse.minIndex)
							{
								tile[mouse.maxIndex - (17 * i)].frame = { 2,1 };
								continue;
							}
							tile[mouse.maxIndex - (17 * i)].frame = { 2,0 };

							int temp = mouse.maxIndex - (17 * i);
							while (temp >= mouse.minIndex)
							{
								temp--;
								tile[temp].frame = { 1,0 };
							}
							break;
						}
					}
					else tile[i].frame = { 1,1 };

					break;
				case ERASE:
					tile[i].active = false;
					break;
				}
			}
		}
	}
	resetIndex();
}

void mapToolScene::saveIndex(int index)
{
	if (backup.size() < MAXUNDOSIZE)
	{
		backup.push_back(index);
	}
	else
	{
		backup.pop_front();
		backup.push_back(index);
	}
}

void mapToolScene::UNDO()
{
	if (!backup.empty())
	{
		tile[backup.back()].active = false;
		backup.pop_back();
	}
}

bool mapToolScene::checkPt(POINT pt, POINT _pt)
{
	if (pt.x == _pt.x && pt.y == _pt.y)return true;

	return false;
}

void mapToolScene::getMinMaxIndex()
{
	if (!sIndex.empty())
	{
		mouse.minIndex = sIndex[0];
		
		mouse.maxIndex = sIndex.back();

		cout << sIndex[0] << '\n';
	}
}

void mapToolScene::resetIndex()
{
	sIndex.clear();
	mouse.maxIndex = mouse.minIndex = 0;
	cout << sIndex.size() << '\n';
}

void mapToolScene::textRender()
{
	string str;

	switch (state)
	{
	case DRAW:
		str = "DRAW MODE";
		break;
	case DRAW_SET:
		str = "SET DRAW MODE";
		break;
	case ERASE:
		str = "ERASE MODE";
		break;
	}

	TextOut(getMemDC(), 10, 10, str.c_str(), strlen(str.c_str()));
}