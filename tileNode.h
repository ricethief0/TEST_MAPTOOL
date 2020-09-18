#pragma once
#include "gameNode.h"

#define MAXUNDOSIZE 5
#define BLACK RGB(0,0,0)
#define RED   RGB(255,0,0)
#define GREEN RGB(0,255,0)
#define BLUE  RGB(0,0,255)
enum STATE
{
	NONE,
	DRAW,	  // ����
	DRAW_SET, // ��Ʈ
	ERASE,
	MOVE
};
struct tagMap
{
	RECT rc;

	POINT frame;

	bool active;
	bool isCol;
};
struct tagSample
{
	RECT rc;

	POINT frame;
};
struct tagMouse
{
	RECT rc;
	POINT start, end;

	bool active; //draw,erase mode
	POINT frame;
	STATE saveBrush;

	//set_draw -> �ݸ����� ù �迭 ��ǥ, ������ �迭 ��ǥ ����
	int minIndex, maxIndex;
};