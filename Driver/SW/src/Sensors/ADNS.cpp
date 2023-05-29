/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "ADNS.h"
#include "ADNS_Firmeware.h"

const int _ncs = 10; // The SS pin (3)
byte _boot_complete = 0;

volatile byte _data[MotionBurst::EndData];
volatile uint16_t _ux = 0;
volatile uint16_t _uy = 0;
volatile uint16_t _ux_dist = 0;
volatile uint16_t _uy_dist = 0;

volatile int16_t xT = 0;
volatile int16_t yT = 0;

volatile byte _mot = 0;
volatile byte _fault = 0;
volatile byte _squal = 0;
volatile byte _moved = 0;

ADNSInterface::ADNSInterface()
{
    Initialize();
}

ADNSInterface::~ADNSInterface()
{

}

void ADNSInterface::Initialize()
{
    pinMode (_ncs, OUTPUT);

    SPI.begin();

    perform_startup();  
    display_registers();
    delay(100);
    _boot_complete=9;
}

void ADNSInterface::Update(uint64_t difftime)
{
    /*
    if (read_reg(REG_Motion))
    {
#if ENABLE_MOTION_BURST
        update_motion_burst_data();
#else
        update_motion_data();
#endif
    }
    */

    // Read Sensor Data Timed
    if (timer <= 0)
    {
        timer = 10 * TimeVar::Millis;
        update_motion_data();
    }
    else
        timer -= difftime;

    if (! _moved)
        return;

    if(_mot) 
    {
        clear();
        get_squal(_squal);
        get_xy(convert_twos_compliment(_ux), convert_twos_compliment(_uy));
        get_xy_dist(convert_twos_compliment(_ux_dist), convert_twos_compliment(_uy_dist));
        delay(3);
    }

    if (_fault) 
        get_fault();

    _moved = 0;
}

void ADNSInterface::reset_xy_dist() 
{
    _ux = _uy = _ux_dist = _uy_dist = 0;
}

void ADNSInterface::printMotionData() 
{
    Serial.print(_data[Delta_X_L], BIN);
    Serial.print("(");
    Serial.print(convert_twos_compliment(_data[Delta_X_L]));
    Serial.print(") ");
    Serial.print(_data[Delta_X_H], BIN);
    Serial.print("(");
    Serial.print(convert_twos_compliment(_data[Delta_X_H]));
    Serial.print(") ");
    Serial.print(_data[Delta_Y_L], BIN);
    Serial.print("(");
    Serial.print(convert_twos_compliment(_data[Delta_Y_L]));
    Serial.print(") ");
    Serial.print(_data[Delta_Y_H], BIN);
    Serial.print("(");
    Serial.print(convert_twos_compliment(_data[Delta_Y_H]));
    Serial.print(") ");
    Serial.print(_data[SQUAL], BIN);
    Serial.println("");
}

void ADNSInterface::upload_firmware() 
{
    // send the firmware to the chip, cf p.18 of the datasheet
    Serial.println("Uploading firmware...");

    // set the configuration_IV register in 3k firmware mode
    write_reg(REG_Configuration_IV, 0x02); // bit 1 = 1 for 3k mode, other bits are reserved 

    // write 0x1d in SROM_enable reg for initializing
    write_reg(REG_SROM_Enable, 0x1d); 

    // wait for more than one frame period
    delay(10); // assume that the frame rate is as low as 100fps... even if it should never be that low

    // write 0x18 to SROM_enable to start SROM download
    write_reg(REG_SROM_Enable, 0x18); 

    // write the SROM file (=firmware data) 
    com_begin();
    SPI.transfer(REG_SROM_Load_Burst | 0x80); // write burst destination adress
    delayMicroseconds(15);

    // send all bytes of the firmware
    unsigned char c;
    for (unsigned int i = 0; i < firmware_length; i++)
    {
        c = (unsigned char)pgm_read_byte(firmware_data + i);
        SPI.transfer(c);
        delayMicroseconds(15);
    }

    com_end();
}

void ADNSInterface::perform_startup() 
{
    com_end(); // ensure that the serial port is reset
    com_begin(); // ensure that the serial port is reset
    com_end(); // ensure that the serial port is reset
    write_reg(REG_Power_Up_Reset, 0x5a); // force reset
    delay(50); // wait for it to reboot
    // read registers 0x02 to 0x06 (and discard the data)
    read_reg(REG_Motion);
    read_reg(REG_Delta_X_L);
    read_reg(REG_Delta_X_H);
    read_reg(REG_Delta_Y_L);
    read_reg(REG_Delta_Y_H);
    // upload the firmware
    upload_firmware();
    delay(10);
    //enable laser(bit 0 = 0b), in normal mode (bits 3,2,1 = 000b)
    // reading the actual value of the register is important because the real
    // default value is different from what is said in the datasheet, and if you
    // change the reserved bytes (like by writing 0x00...) it would not work.
    byte laser_ctrl0 = read_reg(REG_LASER_CTRL0);
    write_reg(REG_LASER_CTRL0, laser_ctrl0 & 0xf0 );

    delay(1);

    // set the configuration_I register
    // 0x01 = 50, minimum
    // 0x44 = 3400, default
    // 0x8e = 7100
    // 0xA4 = 8200, maximum -> counts per inch ( 1 inch -> 2.54cm )
    write_reg(REG_Configuration_I, 0xA4);
    write_reg(REG_Configuration_V, 0xA4);

    // Disable rest mode, 0x08 = fixed frame rate, disable AGC
    //write_reg(REG_Configuration_II, 0x08 + 0x10);

    uint16_t shutterMaxBound = 0x4e20;  // default value = 0x4e20, 0x100 allows 11748 fps tracking but requires better surface quality
    write_reg(REG_Shutter_Max_Bound_Upper, shutterMaxBound);
    write_reg(REG_Frame_Period_Min_Bound_Upper, 0x0fa0); // 0x0fa0 is the minimal allowed value
    // Set upper frame bound (default 0x5dc0 = 0x4e20 + 0x0fa0)
    // This register must be written last. This write also activates Shutter_Max_Bound and Frame_Period_Min_Bound sett
    write_reg(REG_Frame_Period_Max_Bound_Upper, 0x0fa0 + shutterMaxBound);
    // Must seriously wait after setting this register
    delay(2);

    sLogger.debug("Optical Chip Initialized");
}

void ADNSInterface::display_registers()
{
    int oreg[7] = { 
        REG_Product_ID, REG_Inverse_Product_ID, REG_SROM_ID, REG_Motion, REG_LASER_CTRL0                                                                                                                                                                                                                                                                                                                                                                                             };
    const char* oregname[] = {
        "Product_ID","Inverse_Product_ID","SROM_Version","Motion", "LASER_CTRL0"                                                                                                                                                                                                                                                                                                                                                                                            };
    byte regres;

    com_begin();

    int rctr = 0;

    for(rctr = 0; rctr < 5; rctr++)
    {
        SPI.transfer(oreg[rctr]);
        delay(1);
        Serial.println("---");
        Serial.print(oregname[rctr]);
        Serial.print(" (0x");
        Serial.print(oreg[rctr],HEX);
        Serial.println(")");
        regres = SPI.transfer(0);
        Serial.print(regres,BIN);  
        Serial.print(" (0x");
        Serial.print(regres,HEX);  
        Serial.println(")");
        delay(1);
    }

    com_end();
}

void ADNSInterface::com_begin() 
{
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
    digitalWrite(_ncs, LOW);
}

void ADNSInterface::com_end() 
{
    SPI.endTransaction();
    digitalWrite(_ncs, HIGH);
}

byte ADNSInterface::read_reg(byte reg_addr) 
{
    com_begin();

    // send adress of the register, with MSBit = 0 to indicate it's a read
    SPI.transfer(reg_addr & 0x7f);
    delayMicroseconds(100); // tSRAD
    // read data
    byte data = SPI.transfer(0);

    delayMicroseconds(1); // tSCLK-_ncs for read operation is 120ns
    com_end();
    delayMicroseconds(19); //  tSRW/tSRR (=20us) minus tSCLK-_ncs

    return data;
}

void ADNSInterface::write_reg(byte reg_addr, byte data) 
{
    com_begin();

    // send adress of the register, with MSBit = 1 to indicate it's a write
    SPI.transfer(reg_addr | 0x80);
    // sent data
    SPI.transfer(data);

    delayMicroseconds(20); // tSCLK-_ncs for write operation
    com_end();
    delayMicroseconds(100); // tSWW/tSWR (=120us) minus tSCLK-_ncs. Could be shortened, but is looks like a safe lower bound 
}

void ADNSInterface::read_motion_burst_data() 
{
    com_begin();

    // send adress of the register, with MSBit = 1 to indicate it's a write
    SPI.transfer(REG_Motion_Burst & 0x7f);
    //delayMicroseconds(100); // tSRAD
    // read data
    for (int i = 0; i < Pixel_Sum; ++i) {
        _data[i] = SPI.transfer(0);
    }

    com_end();
    delayMicroseconds(1); //  tBEXIT
}

uint16_t ADNSInterface::join_byte(byte l, byte h)
{
    uint16_t b = l;
    b |= (h << 8);
    return b;
}

int8_t ADNSInterface::convert_twos_compliment(byte b)
{
    int8_t val = b;
    //Convert from 2's complement
    if (b & 0x80) 
        val = -1 * ((b ^ 0xff) + 1);

    return val;
}

int16_t ADNSInterface::convert_twos_compliment(uint16_t b)
{
    int16_t val = b;
    //Convert from 2's complement
    if (b & 0x8000) 
        val = -1 * ((b ^ 0xffff) + 1);

    return val;
}

int16_t ADNSInterface::convert_twos_compliment(byte l, byte h)
{
    uint16_t b = join_byte(l, h);
    return convert_twos_compliment(b);
}

void ADNSInterface::copy_data()
{
    _squal = _data[SQUAL];
    
    _ux = join_byte(_data[Delta_X_L], _data[Delta_X_H]);
    _uy = join_byte(_data[Delta_Y_L], _data[Delta_Y_H]);

    xT = join_byte(_data[Delta_X_L], _data[Delta_X_H]);
    yT = join_byte(_data[Delta_Y_L], _data[Delta_Y_H]);

    //sLogger.debug("Moved X:%i and Y:%i at COPY DATA", xT, yT);

    _ux_dist += _ux;
    _uy_dist += _uy;
}

void ADNSInterface::update_motion_data() 
{
    if (_boot_complete != 9)
        return;

    com_begin();

    _data[Motion] = read_reg(REG_Motion);
    _data[SQUAL] = read_reg(REG_SQUAL);

    _mot = _data[Motion] & 0x80;
    _fault = _data[Motion] & 0x40;

    if (!_fault && _mot) 
    {
        _data[Delta_X_L] = read_reg(REG_Delta_X_L);
        _data[Delta_X_H] = read_reg(REG_Delta_X_H);
        _data[Delta_Y_L] = read_reg(REG_Delta_Y_L);
        _data[Delta_Y_H] = read_reg(REG_Delta_Y_H);

        copy_data();

        _moved = 1;
    }

    com_end();
}

void ADNSInterface::update_motion_burst_data() 
{
    if (_boot_complete != 9)
        return;

    com_begin();

    read_motion_burst_data();

    _mot = _data[Motion] & 0x80;
    _fault = _data[Motion] & 0x40;

    if (!_fault && _mot) 
    {
        copy_data();

        _moved = 1;
    }

    com_end();
}

void ADNS_CTRL::get_xy(int16_t x, int16_t y)
{
    _x = x;
    _y = y;

    //sLogger.debug("X:%i and Y:%i at GetXY()", _x, _y);
}

void ADNS_CTRL::get_xy_dist(int16_t x_dist, int16_t y_dist)
{
    _x_dist = x_dist;
    _y_dist = y_dist;

    //sLogger.debug("Moved X:%i and Y:%i at GetXYDistance()", _x_dist, _y_dist);
}

void ADNS_CTRL::get_squal(uint16_t s)
{
    _squal = s;
}

void ADNS_CTRL::get_fault()
{
    sLogger.failure("XY_LASER is shorted to GND");
}

void ADNS_CTRL::clear()
{

}
