#pragma once
#include <iostream>
#include <stdio.h>
#include <vector>
#include <list>
#include <map>
#include <memory.h>
#include <fstream>
#include <string>
#include <conio.h>
#include "sms.h"

class Appliance
{
protected:
	//부모클래스에서 초기화
	double Power = 0;
	int powerFlag = 0;
	time_t startHour = 0;
	time_t endHour = 0;
	//자식 클래스에서 초기화
	int type = -1;
	std::string name = "";
	int powerConsumption = 0;

public:
	Appliance(int type, const std::string& name, int powerconsumption, int powerflag, double power)
		:type(type), name(name), powerConsumption(powerconsumption), powerFlag(powerflag), Power(power) {}
	Appliance(int type, const std::string& name, int powerconsumption)
		: type(type), name(name), powerConsumption(powerconsumption) {}
	virtual ~Appliance() {};
	void set_startHour() { time(&startHour); }
	void set_endHour() { time(&endHour); }
	time_t get_startHour() { return startHour; }
	time_t get_endHour() { return endHour; }
	std::string get_name() { return name; }
	int get_type() { return type; }
	int get_powerflag() { return powerFlag; }
	double get_power() { return Power; }
	int get_powerconsumption() { return powerConsumption; }
	void set_powerflag(int flag) { powerFlag = flag; }
	void set_power(double power) { Power = power; }
	void set_type(int type) { this->type = type; }
	void set_name(const std::string& name) { this->name = name; }
	void set_powerconsumption(int powerconsumption) { powerConsumption = powerconsumption; }
	virtual void control() = 0;
	virtual void display() = 0;
	void save(FILE* f) {
		fprintf(f, "%d\n", get_type()); //타입 저장
		fprintf(f, "%s\n", get_name().c_str()); //이름 저장
		fprintf(f, "%d\n", get_powerconsumption()); //전력소비량저장
		fprintf(f, "%d\n", get_powerflag()); //전원 상태 저장
		fprintf(f, "%02lf\n", get_power()); //전력소모량 저장
		//시작, 종료시간은 조금 더 생각해보자

		save_detail(f);
	}

	virtual void save_detail(FILE* f) = 0;

	void set_param(std::ifstream& file) {
		std::string name;
		int powerconsumption;
		int powerflag;
		double power;
		file >> name >> powerconsumption >> powerflag >> power;
		set_name(name);
		set_powerconsumption(powerconsumption);
		set_powerflag(powerflag);
		set_power(power);
		set_detail(file);
	}
	virtual void set_detail(std::ifstream& file) = 0;
};

class Airconditioner : public Appliance {
	int state = -1;
	int temp = -1;
public:
	Airconditioner(int type, const std::string& name, int powerconsumption) : Appliance(type, name, powerconsumption) {}
	Airconditioner(int type, const std::string& name, int powerconsumption, int powerflag, double power, int state, int temp)
		: Appliance(type, name, powerconsumption, powerflag, power), state(state), temp(temp) {}
	int get_state() { return state; }
	void set_state(int state) { this->state = state; }
	void set_temp(int temp) { this->temp = temp; }
	int get_temp() { return temp; }
	void control() override {
		SMSManager& sms = SMSManager::get_instance();
		std::cout << "에어컨의 전원을 켜시겠습니까?(1:true, 0:false)" << std::endl;
		std::cin >> powerFlag;
		if (powerFlag == 1) {
			std::cout << "에어컨의 전원이 켜졌습니다" << std::endl;
			set_startHour();
			state = 1;
			sms.send_sms("aircon_on", "에어컨이 켜졌습니다");
		}
		else if (powerFlag == 0) {
			std::cout << "에어컨의 전원이 꺼졌습니다" << std::endl;
			set_endHour();
			Power += powerConsumption * (endHour - startHour) / 3600;
			sms.send_sms("aircon_off", "에어컨이 꺼졌습니다");
			getchar();
		}
		while (powerFlag == 1) {
			std::cout << "에어컨의 상태를 선택하시오(1:SENDAIR, 2:COOLER, 3:HEATER)" << std::endl;
			std::cin >> state;
			if (state == 1) {
				std::cout << "송풍 상태입니다 설정온도는 25도 입니다" << std::endl;
				break;
			}
			else if (state == 2) {
				std::cout << "냉방 상태입니다 설정온도를 입력해주세요" << std::endl;
				std::cin >> temp;
				break;
			}
			else if (state == 3) {
				std::cout << "난방 상태입니다 설정온도를 입력해주세요" << std::endl;
				std::cin >> temp;
				break;
			}
			else {
				std::cout << "잘못된 상태입력입니다" << std::endl;
				break;
			}
		}
	}
	void display() override {
		std::cout << "에어컨의 상태를 출력하겠습니다" << std::endl;
		std::cout << "제품명 :" << name << std::endl;
		std::cout << "제품타입 : " << type << std::endl;
		std::cout << "전력소비량 : " << powerConsumption << std::endl;
		std::cout << "전원 상태(1:켜짐, 0: 꺼짐): " << powerFlag << std::endl;
		std::cout << "설정 상태(1:SENDAIR, 2:COOLER, 3:HEATER) : " << state << std::endl;
		std::cout << "설정 온도 : " << temp << std::endl;
		std::cout << "누적 전력소모량 :" << Power << std::endl;
	}
	void save_detail(FILE* f) override {
		fprintf(f, "%d\n", get_state()); //상태 저장
		fprintf(f, "%d\n", get_temp()); //온도 저장
	}
	void set_detail(std::ifstream& file) override {
		int state;
		int temp;
		file >> state >> temp;
		set_state(state);
		set_temp(temp);
	}
};
class Ricecooker : public Appliance {
	int state = -1;
public:
	Ricecooker(int type, const std::string& name, int power) : Appliance(type, name, power) {}
	Ricecooker(int type, const std::string& name, int powerconsumption, int powerflag, double power, int state, int temp)
		: Appliance(type, name, powerconsumption, powerflag, power), state(state) {}
	int get_state() { return state; }
	void set_state(int state) { state = state; }
	void control() override {
		SMSManager& sms = SMSManager::get_instance();
		std::cout << "밥솥의 전원을 켜시겠습니까?(1:true, 0:false)" << std::endl;
		std::cin >> powerFlag;
		if (powerFlag == 1) {
			std::cout << "밥솥의 전원이 켜졌습니다" << std::endl;
			set_startHour();
			state = 1;
			sms.send_sms("ricecooker_on", "밥솥이 켜졌습니다");
		}
		else if (powerFlag == 0) {
			std::cout << "밭솥의 전원이 꺼졌습니다" << std::endl;
			set_endHour();
			Power += powerConsumption * (endHour - startHour) / 3600;
			sms.send_sms("ricecooker_off", "밥솥이 꺼졌습니다");
			getchar();
		}
		while (powerFlag == 1) {
			std::cout << "밥솥의 상태를 선택하시오(1:NO_OPERATION, 2:WARM, 3:COOK, 4:HEAT)" << std::endl;
			std::cin >> state;
			if (state == 1) {
				std::cout << "무동작 상태입니다" << std::endl;
				break;
			}
			else if (state == 2) {
				std::cout << "보온 상태입니다" << std::endl;
				break;
			}
			else if (state == 3) {
				std::cout << "밥짓기 상태입니다" << std::endl;
				break;
			}
			else if (state == 4) {
				std::cout << "데우기 상태입니다" << std::endl;
				break;
			}
			else {
				std::cout << "잘못된 상태입력입니다" << std::endl;
				break;
			}
		}
	}
	void display() override {
		std::cout << "밭솥의 상태를 출력하겠습니다" << std::endl;
		std::cout << "제품명 :" << name << std::endl;
		std::cout << "제품타입 : " << type << std::endl;
		std::cout << "전력소비량 : " << powerConsumption << std::endl;
		std::cout << "전원 상태(1:켜짐, 0: 꺼짐): " << powerFlag << std::endl;
		std::cout << "설정 상태(1:NO_OPERATION, 2:WARM, 3:COOK, 4:HEAT) : " << state << std::endl;
		std::cout << "누적 전력소모량 :" << Power << std::endl;
		std::cout << std::endl;
	}
	void save_detail(FILE* f) override {
		fprintf(f, "%d\n", get_state()); //타입 저장
	}
	void set_detail(std::ifstream& file) override {
		int state;
		file >> state;
		set_state(state);
	}
};

class Washer : public Appliance {
	int state = -1;
public:
	Washer(int type, const std::string& name, int power) : Appliance(type, name, power) {}
	Washer(int type, const std::string& name, int powerconsumption, int powerflag, double power, int state, int temp)
		: Appliance(type, name, powerconsumption, powerflag, power), state(state) {}
	int get_state() { return state; }
	void set_state(int state) { state = state; }
	void control() override {
		std::cout << "세탁기의 전원을 켜시겠습니까?(1:true, 0:false)" << std::endl;
		std::cin >> powerFlag;
		if (powerFlag == 1) {
			std::cout << "세탁기의 전원이 켜졌습니다" << std::endl;
			set_startHour();
			state = 1;
		}
		else if (powerFlag == 0) {
			std::cout << "세탁기의 전원이 꺼졌습니다" << std::endl;
			set_endHour();
			Power += powerConsumption * (endHour - startHour) / 3600;
			getchar();
		}
		while (powerFlag == 1) {
			std::cout << "세탁기의 상태를 선택하시오(1:NO_OPERATION, 2:GENERAL, 3:BOIL, 4:DRY)" << std::endl;
			std::cin >> state;
			if (state == 1) {
				std::cout << "무동작 상태입니다" << std::endl;
				break;
			}
			else if (state == 2) {
				std::cout << "일반 빨래 상태입니다" << std::endl;
				break;
			}
			else if (state == 3) {
				std::cout << "삶은 빨래 상태입니다" << std::endl;
				break;
			}
			else if (state == 4) {
				std::cout << "건조 상태입니다" << std::endl;
				break;
			}
			else {
				std::cout << "잘못된 상태입력입니다" << std::endl;
				break;
			}
		}
	}
	void display() override {
		std::cout << "세탁기의 상태를 출력하겠습니다" << std::endl;
		std::cout << "제품명 :" << name << std::endl;
		std::cout << "제품타입 : " << type << std::endl;
		std::cout << "전력소비량 : " << powerConsumption << std::endl;
		std::cout << "전원 상태(1:켜짐, 0: 꺼짐): " << powerFlag << std::endl;
		std::cout << "설정 상태(1:NO_OPERATION, 2:GENERAL, 3:BOIL, 4:DRY) : " << state << std::endl;
		std::cout << "누적 전력소모량 :" << Power << std::endl;
		std::cout << std::endl;
	}
	void save_detail(FILE* f) override {
		fprintf(f, "%d\n", get_state()); //타입 저장
	}
	void set_detail(std::ifstream& file) override {
		int state;
		file >> state;
	}
};

class Light : public Appliance {
public:
	Light(int type, const std::string& name, int power) : Appliance(type, name, power) {}
	Light(int type, const std::string& name, int powerconsumption, int powerflag, double power, int state, int temp)
		: Appliance(type, name, powerconsumption, powerflag, power) {}
	void control() override {
		std::cout << "전등의 전원을 켜시겠습니까?(1:true, 0:false)" << std::endl;
		std::cin >> powerFlag;
		if (powerFlag == 1) {
			std::cout << "전등의 전원이 켜졌습니다" << std::endl;
			set_startHour();
			getchar();
		}
		else if (powerFlag == 0) {
			std::cout << "젼등의 전원이 꺼졌습니다" << std::endl;
			set_endHour();
			Power += powerConsumption * (endHour - startHour) / 3600;
			getchar();
		}
	}

	void display() override {
		std::cout << "전등의 상태를 출력하겠습니다" << std::endl;
		std::cout << "제품명 :" << name << std::endl;
		std::cout << "제품타입 : " << type << std::endl;
		std::cout << "전력소비량 : " << powerConsumption << std::endl;
		std::cout << "전원 상태(1:켜짐, 0: 꺼짐): " << powerFlag << std::endl;
		std::cout << "누적 전력소모량 :" << Power << std::endl;
		std::cout << std::endl;
	}
	void save_detail(FILE* f) override {
	}
	void set_detail(std::ifstream& file) override {
	}
};