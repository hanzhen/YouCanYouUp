#include "Battle/Character.h"
#include "Battle/BodyInfo.h"
#include <vector>
#include "ManagerInfo.h"
#include <list>

#ifndef AI_DEFINE
#define AI_DEFINE

class StateNode{

private:

protected:
	ManagerInfo * info;
	Character * enemy;

		//判断距离enemy近的box，通过这个判断方向
	Vec2 getCloseVec(Vec2 now);

	int leftup(Vec2 * v);


public:

	StateNode(ManagerInfo * info, Character * enemy) :info(info), enemy(enemy){}

	//定义进入该状态时采取的操作
	virtual void enter()=0;

	//定义在该状态时的动作
	virtual void action()=0;

	//定义离开该状态时采取的操作
	virtual void exit()=0;

	virtual StateNode* changeState()=0;
};

//主防守节点
class DefendStateNode: public StateNode{

public:

	DefendStateNode(ManagerInfo * info, Character * enemy):StateNode(info,enemy){}

	void enter();
	void action();
	void exit();
	StateNode* changeState();
};

//主进攻节点
class AttackStateNode: public StateNode{
public:
	AttackStateNode(ManagerInfo * info, Character * enemy):StateNode(info,enemy){}
	void enter();
	void action();
	void exit();
	StateNode* changeState();
};

//采用策略节点
class StrategyStateNode: public StateNode{
public:
	StrategyStateNode(ManagerInfo * info, Character * enemy):StateNode(info,enemy){}
	void enter();
	void action();
	void exit();
	StateNode* changeState();
};


class AI{
protected:
	ManagerInfo * info;
	std::vector<StateNode*> states;
	bool ready;	//判断信息是否获取


	const static int NUM=10;
	const static int posNum=10;  //记录位置的数量
	std::list<Vec2> eList[NUM];   //存储enemy的前面的位置
	std::list<Vec2> hList[NUM];    //存储hero前面的位置
	int eListSize[NUM];      //存储eList的长度
	int hListSize[NUM];   //存储hList的长度

public:

	const static int delta = 20;

	AI(ManagerInfo * info) : info(info), ready(false)
	{
		for(int i=0;i<NUM;i++)
		{
			eListSize[i]=0;
			hListSize[i]=0;
		}
	}

	void init();

	//改变所有enemy的state
	void changeState();

	//每个enemy需要执行动作
	void actions();

	void addState(Character* enemy){
		states.push_back(new DefendStateNode(info,enemy));
	}

	void setReady(bool ready){
		this->ready=ready;
	}
};

#endif