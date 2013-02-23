

/*



class CGame
{
	enum STATE
	{
		MAIN_MENU, 
		IN_GAME_MENU, 
		PLAY_FIELD,
		SHOP, 
		MAP
	};
	STATE m_GameState;
	CMainMenu m_Menu;
	CGameMenu m_GameMenu;
	CPlayField m_PlayField;
	CShop m_Shop;
	CMap m_Map;
};

class CPlayField
{
	CScore m_Score;
	CCarrotCounter m_CarrotCounter;
	CHamsterCounter m_HamsterCounter;
	CTip m_Tip;
	CWaterTank m_WaterTank;
	CWeapon m_Weapon;
	
	std::vector<CHamster> m_Hamsters;
	std::vector<CCarrot> m_Carrots;
	std::vector<CSpring> m_Springs;
};

class CHamster
{
	float x, y;
	float life;
	float dir;
	float speed;
	enum STATE
	{
		WALKING,
		EATING,
		SCARED,
		RUNNING
	};
	STATE m_HamsterState;

	void ProcessState();
};

class CSpring
{
	float x, y;
	float dir;
	float controlDir;
	float distance;
	float controlDistance;
	float timeToDischarge;
	float radius;
};

class CCarrot
{
	float x,y;
};*/
