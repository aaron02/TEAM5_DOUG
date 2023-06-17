/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "i2cTOF.h"

TCA9548 MP(0x70);
byte gbuf[16];

#define VL53L0X_REG_IDENTIFICATION_MODEL_ID         0xc0
#define VL53L0X_REG_IDENTIFICATION_REVISION_ID      0xc2
#define VL53L0X_REG_PRE_RANGE_CONFIG_VCSEL_PERIOD   0x50
#define VL53L0X_REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD 0x70
#define VL53L0X_REG_SYSRANGE_START                  0x00
#define VL53L0X_REG_RESULT_INTERRUPT_STATUS         0x13
#define VL53L0X_REG_RESULT_RANGE_STATUS             0x14
#define address                                     0x29  // I2C address

uint16_t bswap(byte b[]) {
    // Big Endian unsigned short to little endian unsigned short
    uint16_t val = ((b[0] << 8) & b[1]);
    return val;
}

uint16_t makeuint16(int lsb, int msb) {
    return ((msb & 0xFF) << 8) | (lsb & 0xFF);
}

void write_byte_data(byte data) {
    Wire.beginTransmission(address);
    Wire.write(data);
    Wire.endTransmission();
}

void write_byte_data_at(byte reg, byte data) {
    // write data word at address and register
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission();
}

void write_word_data_at(byte reg, uint16_t data) {
    // write data word at address and register
    byte b0 = (data & 0xFF);
    byte b1 = ((data >> 8) && 0xFF);

    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(b0);
    Wire.write(b1);
    Wire.endTransmission();
}

byte read_byte_data() {
    Wire.requestFrom(address, 1);
    while (Wire.available() < 1) delay(1);
    byte b = Wire.read();
    return b;
}

byte read_byte_data_at(byte reg) {
    // write_byte_data((byte)0x00);
    write_byte_data(reg);
    Wire.requestFrom(address, 1);
    while (Wire.available() < 1) delay(1);
    byte b = Wire.read();
    return b;
}

uint16_t read_word_data_at(byte reg) {
    write_byte_data(reg);
    Wire.requestFrom(address, 2);
    while (Wire.available() < 2) delay(1);
    gbuf[0] = Wire.read();
    gbuf[1] = Wire.read();
    return bswap(gbuf);
}

void read_block_data_at(byte reg, int sz) {
    int i = 0;
    write_byte_data(reg);
    Wire.requestFrom(address, sz);
    for (i = 0; i < sz; i++) {
        while (Wire.available() < 1) delay(1);
        gbuf[i] = Wire.read();
    }
}

i2cTOF::i2cTOF()
{
  if (MP.begin() == false)
  {
    sLogger.debug("Could not connect to TCA9548 multiplexer.");
  }

  sLogger.debug("Scan the channels of the multiplexer for searchAddress.");
  for (int chan = 0; chan < 8; chan++)
  {
    MP.selectChannel(chan);
    bool b = MP.isConnected(address);
    sLogger.debug("Channel %i found Sensor %i", chan, b ? 1 : 0);
  }
}

i2cTOF::~i2cTOF()
{

}

void i2cTOF::Update(uint64_t difftime)
{
    // Read Sensors
    if (updateTimer <= 0)
    {
        updateTimer = 10 * TimeVar::Millis;
        readSensorAtChannel(0);
        readSensorAtChannel(1);
        readSensorAtChannel(2);
        readSensorAtChannel(3);
    }
    else
        updateTimer -= difftime;

    // Debug Out
    if (debug)
    {
        if (timer <= 0)
        {
            timer = 1 * TimeVar::Seconds;
            sLogger.debug("Distance 1 %i, Distance 2 %i, Distance 3 %i, Distance 4 %i", getDistanceFromSensor(0), getDistanceFromSensor(1), getDistanceFromSensor(2), getDistanceFromSensor(3));
        }
        else
            timer -= difftime;
    }
}

void i2cTOF::readSensorAtChannel(int channel)
{
    MP.selectChannel(channel);

    if (MP.isConnected(address))
    {
        write_byte_data_at(VL53L0X_REG_SYSRANGE_START, 0x01);
        byte val = 0;

        int cnt  = 0;
        while (cnt < 50) 
        {
            delay(1);
            val = read_byte_data_at(VL53L0X_REG_RESULT_RANGE_STATUS);
            if (val & 0x01) break;
            cnt++;
        }

        if (val & 0x01)
        {
            read_block_data_at(0x14, 12);
            uint16_t acnt                  = makeuint16(gbuf[7], gbuf[6]);
            uint16_t scnt                  = makeuint16(gbuf[9], gbuf[8]);
            byte DeviceRangeStatusInternal = ((gbuf[0] & 0x78) >> 3);

            if (DeviceRangeStatusInternal == 11)
                distances[channel]             = makeuint16(gbuf[11], gbuf[10]);
            else
                distances[channel]             = 1000;

            //sLogger.debug("Distance %i at Channel %i", dist, channel);
            //sLogger.debug("Range Status %i at Channel %i", DeviceRangeStatusInternal, channel);
        }
    }
}