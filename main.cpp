#pragma once
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <conio.h>
#include <stdio.h>
#include <memory.h>
#include <fstream>
#include "menu.h"
#include "appliance.h"
#include "sms.h"

class HomeAutomationSystem {
	int applianceCnt = 0;
	int callbackCnt = 0;
	//std::vector<Appliance*> v;
	std::map<int, std::vector<std::shared_ptr<Appliance>>> m;

public:
	HomeAutomationSystem() { init_menu(); }
	virtual ~HomeAutomationSystem() {};
	void set_applianceCnt(int count) { applianceCnt = count; }
	void set_callbackCnt(int count) { callbackCnt = count; }
	void init_menu()
	{
		std::shared_ptr<PopupMenu> root = std::make_shared<PopupMenu>("ROOT");
		std::shared_ptr<PopupMenu> pm1 = std::make_shared<PopupMenu>("홈시스템 등록");
		std::shared_ptr<PopupMenu> pm2 = std::make_shared<PopupMenu>("홈시스템 정보 삭제");
		std::shared_ptr<PopupMenu> pm3 = std::make_shared<PopupMenu>("홈시스템 제어");
		std::shared_ptr<PopupMenu> pm4 = std::make_shared<PopupMenu>("가전시스템 확인");
		std::shared_ptr<PopupMenu> pm5 = std::make_shared<PopupMenu>("등록정보저장 및 로딩");
		root->add(pm1);
		root->add(pm2);
		root->add(pm3);
		root->add(pm4);
		root->add(pm5);
		pm1->add(std::make_shared<MenuItem>("가전등록", 11, std::bind(&HomeAutomationSystem::add_product, this)));
		pm1->add(std::make_shared<MenuItem>("콜백서비스정보등록", 12, std::bind(&HomeAutomationSystem::call_back, this)));
		pm2->add(std::make_shared<MenuItem>("가전 삭제", 13, std::bind(&HomeAutomationSystem::remove_product, this)));
		pm2->add(std::make_shared<MenuItem>("콜백서비스정보삭제", 14, std::bind(&HomeAutomationSystem::remove_callback, this)));
		pm3->add(std::make_shared<MenuItem>("개별가전제어", 15, std::bind(&HomeAutomationSystem::control_state, this)));
		pm3->add(std::make_shared<MenuItem>("전체가전종료", 16, std::bind(&HomeAutomationSystem::home_control, this)));
		pm4->add(std::make_shared<MenuItem>("등록가전확인", 17, std::bind(&HomeAutomationSystem::check_product, this)));
		pm4->add(std::make_shared<MenuItem>("세부상태확인", 18, std::bind(&HomeAutomationSystem::display_state, this)));
		pm4->add(std::make_shared<MenuItem>("전력소비량확인", 19, std::bind(&HomeAutomationSystem::power_state, this)));
		pm4->add(std::make_shared<MenuItem>("콜백정보확인", 24, std::bind(&HomeAutomationSystem::check_callback, this)));
		pm5->add(std::make_shared<MenuItem>("콜백번호저장", 20, std::bind(&HomeAutomationSystem::save_callback, this)));
		pm5->add(std::make_shared<MenuItem>("콜백번호로딩", 21, std::bind(&HomeAutomationSystem::load_callback, this)));
		pm5->add(std::make_shared<MenuItem>("홈시스템정보저장", 22, std::bind(&HomeAutomationSystem::save_product, this)));
		pm5->add(std::make_shared<MenuItem>("홈시스템정보로딩", 23, std::bind(&HomeAutomationSystem::load_product, this)));
		root->command();
	}
	void add_product()
	{
		int type;
		std::string name;
		int power;
		while (applianceCnt < 20) {
			std::cout << "[제품등록] 추가할 기기의 type을 입력하세요 (1:에어컨, 2:밥솥, 3: 세탁기, 4: 전등, 0: 종료)" << std::endl;
			std::cin >> type;
			if (type == 0) break;
			std::cout << "[제품등록] 제품의 이름을 입력하시오" << std::endl;
			std::cin >> name;
			std::cout << "[제품등록] 제품의 전력소비량을 입력하시오" << std::endl;
			std::cin >> power;
			if (type == 1) m[type].push_back(std::make_shared<Airconditioner>(type, name, power));
			else if (type == 2) m[type].push_back(std::make_shared<Ricecooker>(type, name, power));
			else if (type == 3) m[type].push_back(std::make_shared<Washer>(type, name, power));
			else if (type == 4) m[type].push_back(std::make_shared<Light>(type, name, power));
			applianceCnt++;
			std::cout << "등록된 가전제품의 개수 :" << applianceCnt << std::endl;
		}
		getchar();
	}
	void remove_product() {
		int type;
		std::string name;
		std::cout << "삭제하려는 제품의 type을 입력하시오" << std::endl;
		std::cin >> type;
		std::cout << "삭제하려는 제품의 이름을 입력하시오" << std::endl;
		std::cin >> name;
		auto p1 = std::find_if(m[type].begin(), m[type].end(), [name](std::shared_ptr<Appliance> p) {return p->get_name() == name; });
		m[type].erase(p1);
		getchar();
	}
	void check_product() {
		for (const auto& p : m) {
			std::cout << "type:" << p.first << std::endl;;
			for (std::shared_ptr<Appliance> ptr : p.second) {
				std::cout << "name:" << ptr->get_name() << std::endl;
			}
		}
		std::cout << "등록된 총 가전제품의 수 : " << applianceCnt << std::endl;
		getchar();
		getchar();
	}
	void display_state() {
		for (const auto& p : m) {
			std::cout << "type:" << p.first << std::endl;;
			for (std::shared_ptr<Appliance> ptr : p.second) {
				ptr->display();
			}
		}
		getchar();
		getchar();
	}
	void control_state() {
		int type;
		std::string name;
		std::cout << "제어할 제품의 type을 입력하시오(1:에어컨, 2:밥솥, 3: 세탁기, 4: 전등)" << std::endl;
		std::cin >> type;
		auto t = m.find(type);
		if (t == m.end()) return;
		std::cout << "제어할 제품의 제품명을 입력하시오" << std::endl;
		std::cin >> name;
		for (std::shared_ptr<Appliance> ptr : m[type]) {
			if (ptr->get_name().compare(name) == 0) {
				ptr->control();
				break;
			}
		}
		return;
	}
	void home_control() {
		int type;
		int num = 0;
		double exit_power;
		std::cout << "종료할 제품의 type을 입력하시오(1:에어컨, 2: 전등, 3: 전체 가전)" << std::endl;
		std::cin >> type;
		if (type == 1) {
			for (std::shared_ptr<Appliance> ptr : m[1]) {
				std::cout << "name:" << ptr->get_name() << ", 전원 상태(1:켜짐, 0:꺼짐)" << ptr->get_powerflag() << std::endl;
				if (ptr->get_powerflag() == 1) {
					ptr->set_powerflag(0);
					ptr->set_endHour();
					exit_power = ptr->get_powerconsumption() * (ptr->get_endHour() - ptr->get_startHour()) / 3600;
					ptr->set_power(ptr->get_power() + exit_power);
					num++;
				}
			}
		getchar();
		}
		if (type == 2) {
			for (std::shared_ptr<Appliance> ptr : m[4]) {
				std::cout << "name:" << ptr->get_name() << ", 전원 상태(1:켜짐, 0:꺼짐)" << ptr->get_powerflag() << std::endl;
				if (ptr->get_powerflag() == 1) {
					ptr->set_powerflag(0);
					ptr->set_endHour();
					exit_power = ptr->get_powerconsumption() * (ptr->get_endHour() - ptr->get_startHour()) / 3600;
					ptr->set_power(ptr->get_power() + exit_power);
					num++;
				}
			}
		getchar();
		}
		if (type == 3) {
			int answer;
			std::cout << "종료전에 전체 가전시스템의 정보를 저장하시겠습니까?(1: 예, 0: 아니요)" << std::endl;
			std::cin >> answer;
			if (answer == 1) { save_product(); }
			for (const auto& p : m) {
				for (std::shared_ptr<Appliance> ptr : p.second) {
					std::cout <<"type : " << ptr->get_type()<< ", name : " << ptr->get_name() << ", 전원 상태(1:켜짐, 0:꺼짐) : " << ptr->get_powerflag() << std::endl;
					if (ptr->get_powerflag() == 1) {
						ptr->set_powerflag(0);
						ptr->set_endHour();
						exit_power = ptr->get_powerconsumption() * (ptr->get_endHour() - ptr->get_startHour()) / 3600;
						ptr->set_power(ptr->get_power() + exit_power);
						num++;
					}
				}
			}
		getchar();
		}
		std::cout << "종료한 가전의 수 : " << num << std::endl;
		getchar();
	}

	void power_state() {
		double total_power = 0;
		double exit_power = 0;
		for (const auto& p : m) {
			for (std::shared_ptr<Appliance> ptr : p.second) {
				if (ptr->get_powerflag() == 0 && ptr->get_power() > 0) {
					total_power += ptr->get_power();
				}
				else if (ptr->get_powerflag() == 1) {
					ptr->set_endHour();
					exit_power = ptr->get_powerconsumption() * (ptr->get_endHour() - ptr->get_startHour()) / 3600;
					ptr->set_startHour();
					ptr->set_power(ptr->get_power() + exit_power);
					total_power += ptr->get_power();
				}
			}
		}
		std::cout << "전체 가전의 누적전력소비량 : " << total_power << std::endl;
		getchar();
		getchar();
	}
	void call_back() {
		int count = 0;
		int type;
		SMSManager& sms = SMSManager::get_instance();
		while (count < 20) {
			std::cout << "콜백 정보를 추가로 등록하시겠습니까? (1:예, 0: 아니오)" << std::endl;
			std::cin >> type;
			if (type == 0) break;
			std::string alrm;
			std::string num;
			std::cout << "알림을 설정하려는 동작상태를 입력하시오(규칙 : 가전종류_동작상태_세부설정)" << std::endl;
			std::cin >> alrm;
			std::cout << "알림을 전달할 콜백 번호를 입력하세요(-없이)" << std::endl;
			std::cin >> num;
			sms.add_sms_number(alrm, num);
			count++;
		}
		set_callbackCnt(count);
		std::cout << "등록된 전체 콜백 설정은 " << callbackCnt << "개 입니다" << std::endl;
		sms.print_sms_number();
		getchar();
	}
	void save_callback() {
		SMSManager& sms = SMSManager::get_instance();
		sms.save_callback();
		getchar();
	}
	void load_callback() {
		SMSManager& sms = SMSManager::get_instance();
		sms.load_callback();
		getchar();
	}
	void check_callback() {
		SMSManager& sms = SMSManager::get_instance();
		sms.print_sms_number();
		getchar();
	}

	void remove_callback() {
		std::string alrm;
		std::string num;
		SMSManager& sms = SMSManager::get_instance();
		std::cout << "등록된 전체 콜백 설정은 " << callbackCnt << "개 입니다" << std::endl;
		sms.print_sms_number();
		std::cout << "삭제하려는 동작상태를 입력하시오(규칙 : 가전종류_동작상태_세부설정)" << std::endl;
		std::cin >> alrm;
		std::cout << "삭제하려는 콜백 번호를 입력하세요(-없이)" << std::endl;
		std::cin >> num;
		sms.remove_sms_number(alrm, num);
	}
	void save_product() {
		FILE* f = NULL;
		errno_t err = fopen_s(&f, "log.txt", "w");
		if (err == 0) {
			std::cout << "파일이 성공적으로 열렸습니다" << std::endl;
			fprintf(f, "%d\n", applianceCnt); //총 등록된 제품의 개수 저장
			for (const auto& p : m) {
				//fprintf(f, "%d\n", p.first); //등록된 제품의 타입 저장
				for (std::shared_ptr<Appliance> ptr : p.second) {
					ptr->save(f);
				}
			}
			fclose(f);
			std::cout << "데이터가 성공적으로 저장되었습니다" << std::endl;
			getchar();
		}
		else {
			std::cout << "파일열기에 실패하였습니다" << std::endl;
		}
		getchar();
	}

	void load_product() {
		std::shared_ptr<Appliance> ptr;
		std::ifstream file("log.txt");
		int totalCnt;
		int type;
		int n = 0;
		file >> totalCnt;
		set_applianceCnt(totalCnt);
		for (int i = 0; i < totalCnt; i++) {
			file >> type;
			std::cout << type << std::endl;
			if (type == 1)
			{
				std::shared_ptr<Airconditioner> p1 = std::make_shared<Airconditioner>(1, "", 0, 0, 0.1, 0, 0); //default 생성자
				p1->set_param(file);
				p1->set_type(type);
				m[type].push_back(p1);
			}
			else if (type == 2) {
				std::shared_ptr<Ricecooker> p2 = std::make_shared<Ricecooker>(2, "", 0, 0, 0.1, 0, 0);
				p2->set_param(file);
				p2->set_type(type);
				m[type].push_back(p2);
			}
			else if (type == 3) {
				std::shared_ptr<Washer> p3 = std::make_shared<Washer>(3, "", 0, 0, 0.1, 0, 0);
				p3->set_param(file);
				p3->set_type(type);
				m[type].push_back(p3);
			}
			else if (type == 4) {
				std::shared_ptr<Light> p4 = std::make_shared<Light>(4, "", 0, 0, 0.1, 0, 0);
				p4->set_param(file);
				p4->set_type(type);
				m[type].push_back(p4);
			}
		}
		file.close();
		std::cout << "총" << totalCnt << "개의 제품정보가 로딩 되었습니다" << std::endl;
		getchar();
		getchar();
	}
};

int main()
{
	HomeAutomationSystem has;
}