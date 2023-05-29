/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Lib/Defnies.h"

class Updateable;

extern const unsigned short firmware_length;
extern prog_uchar firmware_data[];

namespace adns 
{
    // Registers
#define REG_Product_ID                           0x00
#define REG_Revision_ID                          0x01
#define REG_Motion                               0x02
#define REG_Delta_X_L                            0x03
#define REG_Delta_X_H                            0x04
#define REG_Delta_Y_L                            0x05
#define REG_Delta_Y_H                            0x06
#define REG_SQUAL                                0x07
#define REG_Pixel_Sum                            0x08
#define REG_Maximum_Pixel                        0x09
#define REG_Minimum_Pixel                        0x0a
#define REG_Shutter_Lower                        0x0b
#define REG_Shutter_Upper                        0x0c
#define REG_Frame_Period_Lower                   0x0d
#define REG_Frame_Period_Upper                   0x0e
#define REG_Configuration_I                      0x0f
#define REG_Configuration_II                     0x10
#define REG_Frame_Capture                        0x12
#define REG_SROM_Enable                          0x13
#define REG_Run_Downshift                        0x14
#define REG_Rest1_Rate                           0x15
#define REG_Rest1_Downshift                      0x16
#define REG_Rest2_Rate                           0x17
#define REG_Rest2_Downshift                      0x18
#define REG_Rest3_Rate                           0x19
#define REG_Frame_Period_Max_Bound_Lower         0x1a
#define REG_Frame_Period_Max_Bound_Upper         0x1b
#define REG_Frame_Period_Min_Bound_Lower         0x1c
#define REG_Frame_Period_Min_Bound_Upper         0x1d
#define REG_Shutter_Max_Bound_Lower              0x1e
#define REG_Shutter_Max_Bound_Upper              0x1f
#define REG_LASER_CTRL0                          0x20
#define REG_Observation                          0x24
#define REG_Data_Out_Lower                       0x25
#define REG_Data_Out_Upper                       0x26
#define REG_SROM_ID                              0x2a
#define REG_Lift_Detection_Thr                   0x2e
#define REG_Configuration_V                      0x2f
#define REG_Configuration_IV                     0x39
#define REG_Power_Up_Reset                       0x3a
#define REG_Shutdown                             0x3b
#define REG_Inverse_Product_ID                   0x3f
#define REG_Motion_Burst                         0x50
#define REG_SROM_Load_Burst                      0x62
#define REG_Pixel_Burst                          0x64

#define ENABLE_MOTION_BURST                      0
}

enum MotionBurst 
    {
        Motion = 0,
        Observation,
        Delta_X_L,
        Delta_X_H,
        Delta_Y_L,
        Delta_Y_H,
        SQUAL, 
        Pixel_Sum,
        Maximum_Pixel,
        Minimum_Pixel, 
        Shutter_Upper,
        Shutter_Lower, 
        Frame_Period_Upper,
        Frame_Period_Lower,
        EndData
    };

class ADNSInterface : public Updateable
{
public:
    ADNSInterface();
    ~ADNSInterface();

    void Initialize();
    void Update(uint64_t difftime) override;

    virtual void get_xy(int16_t x, int16_t y) = 0;
    virtual void get_xy_dist(int16_t x_sum, int16_t y_sum) = 0;
    virtual void get_squal(uint16_t s) = 0;
    virtual void get_fault() = 0;
    virtual void clear() = 0;
    
    void reset_xy_dist();
    void printMotionData();

private:
    void upload_firmware();
    void perform_startup();
    void display_registers();

    static void com_begin();
    static void com_end();
    static byte read_reg(byte reg_addr);
    static void write_reg(byte reg_addr, byte data);
    static void read_motion_burst_data();
    static void copy_data();
    static void update_motion_data();
    static void update_motion_burst_data();
    static int8_t convert_twos_compliment(byte b);
    static int16_t convert_twos_compliment(byte l, byte h);
    static int16_t convert_twos_compliment(uint16_t u);
    static uint16_t join_byte(byte l, byte h);

    int32_t timer = 10 * TimeVar::Millis;
};

class ADNS_CTRL : public ADNSInterface
{
public:
    int getX() { return _x; }
    int getY() { return _y; }
    int getXDistance() { return _x_dist; }
    int getYDistance() { return _y_dist; }

    void ResetXYDistance() { _x_dist = 0; _y_dist = 0; reset_xy_dist(); }

private:
    void get_xy(int16_t x, int16_t y);
    void get_xy_dist(int16_t x_sum, int16_t y_sum);
    void get_squal(uint16_t s);
    void get_fault();
    void clear();

private:
    int16_t _x = 0;
    int16_t _y = 0;
    int16_t _x_dist = 0;
    int16_t _y_dist = 0;
    uint16_t _squal = 0;
};