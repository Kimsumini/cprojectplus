#pragma once
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

// 1. main 함수에 있는 코드를 보고 사용법을 먼저 익히세요
// 2. 그리고 이 클래스의 소스를 생각해 보세요

template<typename T1, typename T2>
struct pair
{
	T1 first;
	T2 second;

	pair() : first(), second() {} // first, second 를 디폴트 값으로 초기화
	pair(const T1& first, const T2& second) : first(first), second(second) {}
};

class SMSManager
{
	using PHONENUMBER = std::string;

	std::map<std::string, std::vector<PHONENUMBER>> m;
public:
	static SMSManager& get_instance() {
		static SMSManager sms;
		return sms;
	}

	void add_sms_number(const std::string& key, const PHONENUMBER& number)
	{
		m[key].push_back(number);
	}
	void remove_sms_number(const std::string& key, const PHONENUMBER& number)
	{
		auto p1 = std::find_if(m[key].begin(), m[key].end(), [number](PHONENUMBER num) {return num == number; });
		m[key].erase(p1);
		std::cout << key << "동작상태의 콜백 서비스 번호 " << number << "를 삭제하였습니다." << std::endl;
	}

	void send_sms(const std::string& key, const std::string& msg)
	{
		//auto& v = m[key]; // v 는 vector 입니다.
		auto it = m.find(key);

		if (it != m.end()) {
			for (const PHONENUMBER& number : it->second)
			{
				std::cout << number << " : " << msg << std::endl;
			}
		}
		getchar();
	}
	void print_sms_number()
	{
		for (const auto& v : m)
		{
			std::cout << "동작상태 : " << v.first << std::endl;
			for (const std::string num : v.second) {
				std::cout << "sms 송신 번호 : " << num << std::endl;
			}
		}
		getchar();
	}
	void save_callback()
	{
		FILE* cf = NULL;
		std::string callbackstate;
		errno_t err = fopen_s(&cf, "calllog.txt", "w");
		if (err == 0) {
			std::cout << "파일이 성공적으로 열렸습니다" << std::endl;
			for (const auto& v : m) {
				callbackstate = v.first;
				for (const std::string num : v.second){
					fprintf(cf, "%s\n", callbackstate.c_str());
					fprintf(cf, "%s\n", num.c_str());
				} 
			}
			fclose(cf);
			std::cout << "데이터가 성공적으로 저장되었습니다" << std::endl;
			getchar();
		}
		else {
			std::cout << "파일열기에 실패하였습니다" << std::endl;
		}
		getchar();
	}
	void load_callback()
	{
		std::ifstream cfile("calllog.txt");
		std::string callbackstate;
		std::string callbacknum;
		int n = 0;
		while (cfile >> callbackstate >> callbacknum) {
			m[callbackstate].push_back(callbacknum);
			n++;
		}
		cfile.close();
		std::cout << "총" << n << "개의 콜백 정보가 로딩 되었습니다" << std::endl;
		getchar();
	}
};