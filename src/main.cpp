#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

// #include "../backup/ImRadFixed.h"
#include "Application.h"
#include "serial/serial.h"
#include "LaserControl.h"

int main() {
	auto* serial = new serial::Serial();
	
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