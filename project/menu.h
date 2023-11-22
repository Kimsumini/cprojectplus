#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <memory>
using namespace std::placeholders;

class BaseMenu
{
	std::string title;
public:
	BaseMenu(const std::string& s) : title(s) {}
	virtual ~BaseMenu() {};
	std::string get_title() { return title; }

	virtual void command() = 0;
};

class PopupMenu : public BaseMenu
{

	//std::vector<BaseMenu*> v;
	std::vector<std::shared_ptr<BaseMenu>>v;
public:
	PopupMenu(const std::string& s) : BaseMenu(s) {}

	//void add(BaseMenu* p) { v.push_back(p); }
	void add(std::shared_ptr<BaseMenu> p) { v.push_back(p); }

	void command()
	{
		while (1)
		{
			system("cls");

			int sz = v.size();
			for (int i = 0; i < sz; i++)
			{
				std::cout << i + 1 << ". " << v[i]->get_title() << std::endl;
			}
			std::cout << sz + 1 << ". 종료\n";

			int cmd;
			std::cout << "메뉴를 선택하세요 >> ";
			std::cin >> cmd;

			if (cmd == sz + 1)
				break;

			if (cmd < 1 || cmd > sz + 1)
				continue;

			v[cmd - 1]->command();
		}
	}
};

class MenuItem : public BaseMenu
{
	int id;
	using HANDLER = std::function<void()>;

	HANDLER handler;
public:
	MenuItem(const std::string& s, int n, HANDLER h = nullptr)
		: BaseMenu(s), id(n), handler(h) {}

	void command()
	{
		if (handler != nullptr)
			handler();
	}
};