#ifndef M4_GUI_HPP

#define M4_GUI_HPP



#include<vector>

using namespace CEGUI;


class CMainMenu 
{
};

class CHamsterGUI : public CObjectRendererProxy
{

public:
	CHamsterGUI() : CObjectRendererProxy()
	{
		m_bNewGame = false;
	}
public:
	void InitMainMenu(void* ptr);
	void HideMainMenu();
	bool handleNewGame(const CEGUI::EventArgs& e);
	
	bool IsNewGame()
	{
		bool tmp = m_bNewGame;
		m_bNewGame  = false;
		return tmp;
	}
private:
	bool m_bNewGame;



};

#endif