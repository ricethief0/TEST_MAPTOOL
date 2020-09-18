#pragma once
#include "gameNode.h"
#include "tileNode.h"

class mapToolScene : public gameNode
{
private:
	tagMap tile[153]; //�⺻ �� Ÿ��
	tagSample sTile[9]; //���� �� Ÿ�� <����>

	tagSample sTile_set;//���� �� Ÿ�� <��Ʈ>

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