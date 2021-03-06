#include <iostream>
#include <sstream>
#include <iomanip>

#include <u8g2.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "ui.h"
#include "screens/nas_stats.h"
#include "screens/air_conditioner.h"

static const uint8_t display_dev_id = 0x3c;

int g_i2c_fd = 0;

//#define DEBUG

uint8_t u8g2_rpi_hw_i2c_byte(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr)
{
#ifdef DEBUG
    std::cout << __FUNCTION__ << ": arg_int=" << (int)arg_int << ", msg=";
#endif

    static int s_i2c_mode = 0;
    static bool s_first_byte_after_start = false;

    switch (msg)
    {
        case U8X8_MSG_BYTE_INIT:
#ifdef DEBUG
            std::cout << "U8X8_MSG_BYTE_INIT";
#endif
            break;

        case U8X8_MSG_BYTE_START_TRANSFER:
#ifdef DEBUG
            std::cout << "U8X8_MSG_BYTE_START_TRANSFER";
#endif
            s_first_byte_after_start = true;
            s_i2c_mode = 0;
            break;

        case U8X8_MSG_BYTE_END_TRANSFER:
#ifdef DEBUG
            std::cout << "U8X8_MSG_BYTE_END_TRANSFER";
#endif
            break;

        case U8X8_MSG_BYTE_SEND:
        {
#ifdef DEBUG
            std::cout << "U8X8_MSG_BYTE_SEND:";
            std::cout << std::hex;
#endif

            if (s_first_byte_after_start)
            {
                s_first_byte_after_start = false;
                s_i2c_mode = *reinterpret_cast<uint8_t*>(arg_ptr);
#ifdef DEBUG
                std::cout << " setting mode to " << s_i2c_mode;
#endif
                break;
            }

            auto p = reinterpret_cast<uint8_t*>(arg_ptr);
            for (int i = 0; i < arg_int; ++i)
            {
                auto data = *p++;
#ifdef DEBUG
                std::cout << " " << (int)data;
#endif
                wiringPiI2CWriteReg8(g_i2c_fd, s_i2c_mode, data);
            }
#ifdef DEBUG
            std::cout << std::dec;
#endif

            break;
        }

        default:
#ifdef DEBUG
            std::cout << "UNKNOWN(" << (int)msg << ")";
#endif
            break;
    }

#ifdef DEBUG
    std::cout << std::endl;
#endif

    return 0;
}

uint8_t u8g2_rpi_gpio_delay(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr)
{
#ifdef DEBUG
    std::cout << __FUNCTION__ << ": arg_int=" << (int)arg_int << ", msg=";
#endif

    switch (msg)
    {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
#ifdef DEBUG
            std::cout << "U8X8_MSG_GPIO_AND_DELAY_INIT";
#endif
            break;

        case U8X8_MSG_DELAY_MILLI:
#ifdef DEBUG
            std::cout << "U8X8_MSG_DELAY_MILLI";
#endif
            delay(arg_int);
            break;

        case U8X8_MSG_GPIO_RESET:
#ifdef DEBUG
            std::cout << "U8X8_MSG_GPIO_RESET";
#endif
            break;

        default:
#ifdef DEBUG
            std::cout << "UNKNOWN(" << (int)msg << ")";
#endif
            break;
    }

#ifdef DEBUG
    std::cout << std::endl;
#endif

    return 0;
}

u8g2_t setup_display()
{
    std::cout << "setting up the display" << std::endl;

    u8g2_t u8g2;
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8g2_rpi_hw_i2c_byte, u8g2_rpi_gpio_delay);

    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
    u8g2_SetContrast(&u8g2, 0);

    return u8g2;
}

bool setup_wiringpi()
{
    std::cout << "setting up WiringPi" << std::endl;

    wiringPiSetup();

    if ((g_i2c_fd = wiringPiI2CSetup(display_dev_id)) == 0)
    {
        std::cerr << "failed to setup WiringPi I2C" << std::endl;
        return false;
    }

    std::cout << "i2c fd: " << g_i2c_fd << std::endl;

    return true;
}

int calc_next_coord(int coord, int max, bool& inc)
{
    int new_coord;

    if (inc)
    {
        new_coord = coord + 1;
        if (new_coord >= max)
        {
            new_coord = max;
            inc = false;
        }
    }
    else
    {
        new_coord = coord - 1;
        if (new_coord <= 0)
        {
            new_coord = 0;
            inc = true;
        }
    }

    return new_coord;
}

int main()
{
    std::cout << "ESPCC playground is starting" << std::endl;

    if (!setup_wiringpi())
    {
        std::cerr << "failed to setup WiringPi, exiting" << std::endl;
        return 1;
    }

    auto u8g2 = setup_display();

    std::cout << "drawing test picture" << std::endl;

    u8g2_ClearBuffer(&u8g2);
    u8g2_SetDrawColor(&u8g2, 1);
    u8g2_SendBuffer(&u8g2);

    ui::oled_ui oled_ui{ &u8g2 };

    ui::screens::nas_stats nas_stats_screen;
    if (!oled_ui.add_screen(&nas_stats_screen))
    {
        std::cerr << "failed to add NAS stats screen" << std::endl;
        return 1;
    }

    ui::screens::air_conditioner ac_bedroom{ "BEDROOM" };
    if (!oled_ui.add_screen(&ac_bedroom))
    {
        std::cerr << "failed to add ac_bedroom screen" << std::endl;
        return 1;
    }

    ui::screens::air_conditioner ac_kitchen{ "KITCHEN" };
    if (!oled_ui.add_screen(&ac_kitchen))
    {
        std::cerr << "failed to add ac_kitchen screen" << std::endl;
        return 1;
    }

    uint8_t screen_index = 0;
    uint8_t counter = 0;

    ac_bedroom.set_temp = 24;
    ac_bedroom.fan_speed = 0;
    ac_kitchen.set_temp = 25;
    ac_kitchen.fan_speed = 5;

    while (true)
    {
        nas_stats_screen.cpu_usage = std::rand() % 101;
        nas_stats_screen.mem_usage = std::rand() % 101;
        nas_stats_screen.upload_speed = std::rand() % 11000;
        nas_stats_screen.download_speed = std::rand() % 11000;

        oled_ui.task();
        delay(1000);

        if (++counter == 3)
        {
            counter = 0;

            if (++screen_index == oled_ui.screen_count())
            {
                screen_index = 0;
            }

            oled_ui.switch_screen(screen_index);
        }
    }

    std::cout << "finished" << std::endl;

    return 0;
}