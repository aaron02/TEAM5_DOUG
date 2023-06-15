/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Lib/Defnies.h"

TCA9548 MP(0x70);

uint8_t searchAddress = 0x29;   //  dummy, adjust to your needs.


class i2cTOF : public Updateable
{
public:
  i2cTOF();
  ~i2cTOF();
  void Update(uint64_t difftime) override;
  void Sensoren_I2C_Auslesen();
  void i2cBusReader(int howMany);

private:

protected:

};