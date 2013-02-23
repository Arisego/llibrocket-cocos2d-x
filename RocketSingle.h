#ifndef __ROCKET_SINGLE_H__
#define __ROCKET_SINGLE_H__

#include "RocketGlu.h"
#include "cocos2d.h"

USING_NS_CC;

class RocketSingle 
{
private:
	static RocketSingle* m_RocketSingle;
	CocoSystemInterface* csi;
	CocoRenderInterface* cri;

public:
	void init(){
		csi = new CocoSystemInterface();
		cri = new CocoRenderInterface();
		csi->init();

		Rocket::Core::SetSystemInterface(csi);
		Rocket::Core::SetRenderInterface(cri);
		Rocket::Core::Initialise();
	}
	
	~RocketSingle(){
		csi->Release();
		cri->Release();
	}

	static RocketSingle* sharedRocket(){
		if(!m_RocketSingle){
			m_RocketSingle = new RocketSingle();
			m_RocketSingle->init();
		}
		return m_RocketSingle;
	}
};

//RocketSingle* RocketSingle::m_RocketSingle = NULL;
#include "Inventory.h"

//Notice:It's not neccery to be a CCSprite here, just use cclayer as you wish.
class RocketLayer : public CCSprite
{
public:
	Inventory* inventory_1;

	Rocket::Core::Context* p_context;
	RocketLayer(const char* sn){
		name = sn;
	}

	void onEnter();
	void draw();
	void LoadFonts(const char* directory);
private:
	std::string name;


};

#endif