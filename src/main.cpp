#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>
#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

// #include "../backup/ImRadFixed.h"
#include "Application.h"
#include "serial/serial.h"
#include "LaserControl.h"


int main() {
	namespace fs = std::filesystem;
	std::string config_path = "../config";
	std::vector<std::string> config_list;

	for (const auto& entry : fs::directory_iterator(config_path)) {
		static int file_index = 0;
		if (entry.is_regular_file()) { // 判断是否为普通文件
			config_list.push_back(entry.path().string());
			std::cout << "Index: "<< file_index << " | " << entry.path().string() << std::endl;
			file_index++;
		}
	}
	std::cout << "Total config files: " << config_list.size() << std::endl;
	std::cout << "Please select a config file: " << std::endl;
	int user_choice = 0;
	std::cin >> user_choice;
	if (user_choice < config_list.size() && user_choice >= 0) {
		std::cout << "Selected config file: " << config_list[user_choice] << std::endl;
		std::ifstream config_file(config_list[user_choice]);
		nlohmann::json config;
		config = nlohmann::json::parse(config_file);
		std::cout << config.dump(2) << std::endl;
	} else {
		spdlog::error("Not Available");
	}
	
	
	sf::RenderWindow window(sf::VideoMode(1200, 800), "Control Panel");
	window.setFramerateLimit(60);

	if (!ImGui::SFML::Init(window)) {
		std::cout << "Failed to initialize ImGui!" << std::endl;
		return -1;
	}
	sf::Clock deltaClock;

	ImGuiIO& io = ImGui::GetIO();
	ImFont* myFont = io.Fonts->AddFontFromFileTTF(
		"C:/Windows/Fonts/msyh.ttc",
		20.0f,
		nullptr,
		io.Fonts->GetGlyphRangesChineseFull()
	);
	io.FontDefault = myFont; // 设置为默认字体
	
	
	if (!ImGui::SFML::UpdateFontTexture()) {
		std::cout << "Failed to update font texture!" << std::endl;
		return -1;
	}

	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(1.5f);
	// style.WindowRounding = 0.25f;
	
	application.Open();

	while (window.isOpen()) {
		sf::Event event{};
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed) window.close();
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		// Render UI
		application.Draw();

		window.clear();
		ImGui::SFML::Render(window);
		window.display();

		// std::cout << LaserControl::EnumeratePortToString() << std::endl;
		// spdlog::info("Hello");
	}
	
	application.Close();
	ImGui::SFML::Shutdown();
	return 0;


}