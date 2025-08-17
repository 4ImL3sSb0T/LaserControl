// Generated with ImRAD 0.8
// visit https://github.com/tpecholt/imrad

#include "Application.h"
#include <spdlog/spdlog.h>

#include "serial/serial.h"

Application application;


void Application::Open()
{
    isOpen = true;
}

void Application::Close()
{
    isOpen = false;
}

void Application::Draw()
{
    /// @style Dark
    /// @unit px
    /// @begin TopWindow
    // auto* ioUserData = (ImRad::IOUserData*)ImGui::GetIO().UserData;
    ImGui::SetNextWindowSize({ 1200, 800 }, ImGuiCond_FirstUseEver); //{ 1200, 800 }
    if (isOpen && ImGui::Begin("Application###Application", &isOpen, ImGuiWindowFlags_NoCollapse))
    {
        /// @separator

        // TODO: Add Draw calls of dependent popup windows here

        /// @begin Child
        ImGui::BeginChild("child1", { 680, 264 }, ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_NoSavedSettings);
        {
            /// @separator

            /// @begin Text
            ImRad::Spacing(4);
            ImGui::Indent(2 * ImGui::GetStyle().IndentSpacing / 2);
            ImGui::TextUnformatted("Machine State is OK");
            /// @end Text

            /// @begin Combo
            ImGui::SameLine(0, 28 * ImGui::GetStyle().ItemSpacing.x);
            ImGui::SetNextItemWidth(232);
            ImRad::Combo("##machine_state", &machine_state, "manul\000track\000draw\000", ImGuiComboFlags_None);
            /// @end Combo

            /// @begin Text
            ImRad::Spacing(4);
            ImGui::TextUnformatted("No COM Selected");
            /// @end Text

            /// @begin Button
            ImGui::SameLine(0, 15 * ImGui::GetStyle().ItemSpacing.x);
            if (ImGui::Button("OK", { 40, 0 })) {
	            if (selected_com != "NONE") {
	            	if (LaserControl::getInstance().OpenSerialPort(selected_com, 115200) > 0) spdlog::info("COM Opened");
	            	else spdlog::warn("CAN NOT OPEN {}", selected_com);
	            } else {
		            spdlog::warn("No COM Selected!");
	            }
            }	
            /// @end Button

            /// @begin Button
            ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
            if (ImGui::Button("Refresh", { 72, 0 })) {
            	com_list = LaserControl::EnumeratePortToString();
            }
            /// @end Button

            /// @begin Combo
            ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
            ImGui::SetNextItemWidth(224);
            ImRad::Combo("##com_list", &selected_com, com_list.c_str(), ImGuiComboFlags_None);
            /// @end Combo

            /// @begin Text
            ImRad::Spacing(2);
            ImGui::TextUnformatted("No Error");
            /// @end Text

            /// @begin Spacer
            ImRad::Dummy({ 640, 36 });
            /// @end Spacer

            /// @begin Button
            if (ImGui::Button("START", { 104, 56 })) {
                LaserControl::getInstance().CreateTask(10);
            }
            /// @end Button

            /// @begin Button
            ImGui::SameLine(0, 12 * ImGui::GetStyle().ItemSpacing.x);
            if(ImGui::Button("STOP", { 104, 56 })) {
                LaserControl::getInstance().StopTask();
            }
            /// @end Button

            /// @begin Button
            ImGui::SameLine(0, 28 * ImGui::GetStyle().ItemSpacing.x);
            ImGui::Button("Calibration", { 112, 56 });
            /// @end Button

            /// @separator
            ImGui::EndChild();
        }
        /// @end Child

        /// @begin Child
        ImGui::BeginChild("child2", { 680, -1 }, ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_NoSavedSettings);
        {
            /// @separator

            /// @begin Slider
            ImRad::Spacing(20);
            ImGui::SetNextItemWidth(376);
            ImGui::SliderInt("MotorX", &motor_x, 0, 16, nullptr);
            /// @end Slider

            /// @begin Slider
            ImRad::Spacing(5);
            ImGui::SetNextItemWidth(376);
            ImGui::SliderInt("MotorY", &motor_y, 0, 16, nullptr);
            /// @end Slider

            /// @begin Slider
            ImRad::Spacing(5);
            ImGui::SetNextItemWidth(376);
            ImGui::SliderFloat("Laser Power", &laser_power, 0, 1, nullptr);
            /// @end Slider

            /// @begin Text
            ImRad::Spacing(9);
            ImGui::TextUnformatted("MotoX Step");
            /// @end Text

            /// @begin Button
            ImRad::Spacing(1);
            ImGui::Button("<=", { 80, 0 });
            /// @end Button

            /// @begin Slider
            ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
            ImGui::SetNextItemWidth(200);
            ImGui::SliderInt("##motor_x_step", &motor_x_step, 1, 16, nullptr);
            /// @end Slider

            /// @begin Button
            ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
            ImGui::Button("=>", { 72, 0 });
            /// @end Button

            /// @begin Text
            ImRad::Spacing(4);
            ImGui::TextUnformatted("MotorY Step");
            /// @end Text

            /// @begin Button
            ImRad::Spacing(1);
            ImGui::Button("<=", { 80, 0 });
            /// @end Button

            /// @begin Slider
            ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
            ImGui::SetNextItemWidth(200);
            ImGui::SliderInt("##motor_y_step", &motor_y_step, 1, 16, nullptr);
            /// @end Slider

            /// @begin Button
            ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
            ImGui::Button("=>", { 72, 0 });
            /// @end Button

            /// @separator
            ImGui::EndChild();
        }
        /// @end Child

        /// @separator
        ImGui::End();
    }
    /// @end TopWindow
}
