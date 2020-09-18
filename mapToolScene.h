#pragma once
#include "gameNode.h"
#include "tileNode.h"

class mapToolScene : public gameNode
{
private:
	tagMap tile[153]; //기본 맵 타일
	tagSample sTile[9]; //선택 맵 타일 <단일>

	tagSample sTile_set;//선택 맵 타일 <세트>

	deque<int> backup;

	tagMouse mouse;
	STATE state;

	vector<int> sIndex;

	//button down
	bool isLeftDown;
	bool isLeftkey;
public:
	HRESULT init();
	void release();
	void update();
	void render();

	void save();
	void load();
	void dragTile();

	void saveIndex(int index); // save
	void UNDO();//ctrl + z

	bool checkPt(POINT pt, POINT _pt);

	void getMinMaxIndex();
	void resetIndex();
	//text view
	void textRender();
};