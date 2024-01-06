#pragma once

#define MENU_PAGE_OPTION 7

typedef struct S_MENU_ITEM
{
	std::string		Info;
	std::string		Text;
	bool			Disabled;
	std::string		Extra;
} P_MENU_ITEM, * LP_MENU_ITEM;

class CCSDM_Menu
{
public:
	void Clear();

	void Create(std::string Title, bool Exit, void* CallbackFunction);
	void AddItem(std::string Info, std::string Text, bool Disabled);
	void AddItem(std::string Info, std::string Text, bool Disabled, std::string Extra);

	void Show(int EntityIndex);
	void Hide(int EntityIndex);

	bool Handle(int EntityIndex, int Key);

	void Display(int EntityIndex, int Page);
	void ShowMenu(int EntityIndex, int Slots, int Time, std::string Text);

private:
	std::string m_Text;
	std::vector<P_MENU_ITEM> m_Data;
	int m_Page = 0;
	bool m_Exit = false;
	int m_PageOption = MENU_PAGE_OPTION;
	void* m_Func = nullptr;
};

extern std::array<CCSDM_Menu, MAX_CLIENTS + 1> gCSDM_Menu;
