#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>

int main() {
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

	float test_value = 0;

	while (window.isOpen()) {
		sf::Event event{};
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed) window.close();
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		ImGui::Begin("Control Panel");
		ImGui::Text(reinterpret_cast<const char *>(u8"中文测试：你好，世界！"));
		ImGui::SliderFloat("Test Value", &test_value, 0.0f, 1.0f);
		ImGui::End();

		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}
	ImGui::SFML::Shutdown();
}
