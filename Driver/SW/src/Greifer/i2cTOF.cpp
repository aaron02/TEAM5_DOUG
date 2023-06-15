/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "i2cTOF.h"

i2cTOF::i2cTOF()
{
    Sensoren_I2C_Auslesen();
}

i2cTOF::~i2cTOF()
{

}

void Greifer::Update(uint64_t difftime)
{

}

void i2cTOF::Sensoren_I2C_Auslesen()
{
    /*
    sLogger.debug("searchAddress");

    for (int chan = 0; chan < 80; chan++)
    {
        MP.selectChannel(chan);
        bool bi2c = MP.isConnected(searchAddress);
        sLogger.debug("CHAN: %u", chan);
        sLogger.debug("bi2c");
    }
    sLogger.debug("searchAddress");
    */
    /*
        for (int chan = 0; chan < 80; chan++)
    {
        MP.selectChannel(chan);
        bool b = MP.isConnected(searchAddress);
        sLogger.debug("CHAN: %u", chan);
        Serial.print("c: ");
        Serial.print(chan);
        Serial.print("\t");
        Serial.print( b ? "found!" : "x");
    }
    sLogger.debug("searchAddress");
    */
}

void i2cBusReader(int howMany)
{
    int address;
    int error;
    sLogger.debug("scanning Address [HEX]");
    for (address = 1; address < 127; address++) {
        Wire.beginTransmission(
            address);  // Data transmission to the specified device address

        error = Wire.endTransmission();
            
        if (error == 0) {
            sLogger.debug("address %u", address);
        }
    }
}