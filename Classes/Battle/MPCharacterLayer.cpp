#include "MPCharacterLayer.h"
#include "CmdTool.h"

bool MPCharacterLayer:: init()
{
	int hero_lives = 3;

	// ---------- WebSocket ---------
	_wsiClient = new cocos2d::network::WebSocket();
	_wsiClient->init(*this, "ws://202.194.14.196:8001");
	// ---------- WebSocket ---------

	//触屏事件监听
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(MPCharacterLayer::onTouchBegan,this);
	//listener->onTouchMoved = CC_CALLBACK_2(GameLayer::onTouchMoved,this);
	listener->onTouchEnded = CC_CALLBACK_2(MPCharacterLayer::onTouchEnded,this);
	listener->setSwallowTouches(true);//不向下传递触摸
	dispatcher->addEventListenerWithSceneGraphPriority(listener,this);

	//定时判断角色状态
	this->schedule(schedule_selector(CharacterLayer::scheduleCallBack), 1.f);  

	return true;
}

bool MPCharacterLayer::onTouchBegan(Touch *pTouch, Event *pEvent){
	posBegan = pTouch->getLocation();
	return true;
}

void MPCharacterLayer::onTouchEnded(Touch *touch, Event *unused_event){
	posEnded = touch->getLocation();
	Vec2 force=2*(posEnded-posBegan);

	_wsiClient->send("fight!key="+enemyKey+"&px="+std::to_string(hero->getPosition().x)+"&py="+
		std::to_string(hero->getPosition().y)+"&fx="+std::to_string(force.x)+"&fy="+std::to_string(force.y));


}

void MPCharacterLayer::CheckResult(){
	float heroY = hero->getSprite()->getPositionY();
	if(heroY<0){
		CCLOG("you lose");
		Rebirth(hero);
	}
	float enemyY = enemy->getSprite()->getPositionY();
	if(enemy<0){
		CCLOG("you win");
	}
}

void MPCharacterLayer::Rebirth(Character* cha){
	Vec2 force = Vec2::ZERO;
	Vec2 vec = Vec2(300, 500);
	_wsiClient->send("fight!key="+enemyKey+"&px="+std::to_string(vec.x)+"&py="+
	std::to_string(vec.y)+"&fx="+std::to_string(force.x)+"&fy="+std::to_string(force.y));
}

MPCharacterLayer::~MPCharacterLayer(){
	delete hero;
	
}

void MPCharacterLayer::setHero(GameSetting::Character hero)
{
	//操作角色
	this->hero = new Character(hero);
	this->hero->setPosition(Vec2(300,500));
	this->addChild(this->hero->getSprite());
}

void MPCharacterLayer::setEnemy(std::vector<GameSetting::Character> enemy)
{
	this->enemy = new Character(enemy[0]);
	this->hero->setPosition(Vec2(500,500));
	this->addChild(this->enemy->getSprite());
}

// ------------------ websocket function -----------------------
// 开始socket连接
void MPCharacterLayer::onOpen(cocos2d::network::WebSocket* ws)
{
    CCLOG("connect");
}

// 接收到了消息
void MPCharacterLayer::onMessage(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::Data& data)
{
    
    std::string msgStr = data.bytes;
    CCLOG("收到的消息：%s",msgStr.c_str());
    std::string cmdStr = CmdTool::getCmd(msgStr);
    std::string cmdPara = CmdTool::getCmdPara(msgStr);
 
    // ---------- 收到服务器发送的打斗事件 ----------
    if (cmdStr == "fight") {
        string cmdKey = CmdTool::getCmdKey(msgStr);
        string px = CmdTool::getFightPx(msgStr);
        string py = CmdTool::getFightPy(msgStr);
        string fx = CmdTool::getFightFx(msgStr);
        string fy = CmdTool::getFightFy(msgStr);
        CCLOG("key = %s",CmdTool::getCmdKey(msgStr).c_str());
        CCLOG("px = %s",CmdTool::getFightPx(msgStr).c_str());
        CCLOG("py = %s",CmdTool::getFightPy(msgStr).c_str());
        CCLOG("fx = %s",CmdTool::getFightFx(msgStr).c_str());
        CCLOG("fy = %s",CmdTool::getFightFy(msgStr).c_str());
        if(cmdKey == enemyKey){
			enemy->applyImpulse(Vec2(atof(fx.c_str()),atof(fy.c_str())));
			enemy->setPosition(Vec2(atof(px.c_str()),atof(py.c_str())));
		} else {
			hero->applyImpulse(Vec2(atof(fx.c_str()),atof(fy.c_str())));
			hero->setPosition(Vec2(atof(px.c_str()),atof(py.c_str())));

		}
    }
 
}

// 连接关闭
void MPCharacterLayer::onClose(cocos2d::network::WebSocket* ws)
{
    if (ws == _wsiClient)
    {
        _wsiClient = NULL;
    }
    CC_SAFE_DELETE(ws);
    CCLOG("onClose");
}

// 遇到错误
void MPCharacterLayer::onError(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::ErrorCode& error)
{
    if (ws == _wsiClient)
    {
        char buf[100] = {0};
        sprintf(buf, "an error was fired, code: %d", error);
    }
    CCLOG("Error was fired, error code: %d", error);
}


void MPCharacterLayer::initBattleScene(int index){

	
    // -----------------
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    CCLOG("visibleSize = %f,%f",visibleSize.width,visibleSize.height);
    CCLOG("origin = %f,%f",origin.x,origin.y);
 
    // 如果是主场作战
    if(index == 0){
		hero->setPosition(Vec2(visibleSize.width / 4 , visibleSize.height / 2));
        enemy->setPosition(Vec2(visibleSize.width / 4 * 3 , visibleSize.height / 2));
    }
    
    // 如果是客场作战
    else{
        hero->setPosition(Vec2(visibleSize.width / 4 * 3 , visibleSize.height / 2));
        enemy->setPosition(Vec2(visibleSize.width / 4 , visibleSize.height / 2));
    }

    
}
