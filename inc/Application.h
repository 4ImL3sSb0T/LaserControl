// Generated with ImRAD 0.8
// visit https://github.com/tpecholt/imrad

#pragma once
#include "imrad.h"
#include <string>
#include "LaserControl.h"

// #include "ImRadFixed.h"
class Application
{
public:
    /// @begin interface
    void Open();
    void Close();
    void Draw();

    std::string machine_state = "NONE";
    int motor_x = 0;
    int motor_y = 0;
    float laser_power = 0;
    // std::string value1;
    int motor_x_step = 1;
    int motor_y_step = 1;
    // std::string value4;
    std::string selected_com = "NONE";
	std::string com_list = "NONE";
    /// @end interface


private:
    /// @begin impl
    bool isOpen = true;
    /// @end impl
};

extern Application application;
