/**
* @file            statusdevice.cpp
* @brief           CPP-Datei für die Headerdatei "statusdevice.h"
* @details
*/

/* Includes */
#include "statusdevice.h"
#include "sunnyconnector.h"

/* Variable für die Anzahl der StatusObjekte initialisieren */
unsigned int cStatusDevice::iDeviceCounter = 0;
unsigned int cStatusDevice::iDeviceBufferCounter = 0;

/* Standard-Konstruktor */
cStatusDevice::cStatusDevice(void)
{
    this->bPowerOnFlag = false;
    this->bFeedInFlag = false;
    this->iDeviceSN = 0;
}

/* Eigener Konstruktor */
cStatusDevice::cStatusDevice(int iDeviceSN)
{
    this->bPowerOnFlag = false;
    this->bFeedInFlag = false;
    this->iDeviceSN = iDeviceSN;
}

/* Überladung des "="-Operators */
cStatusDevice& cStatusDevice::operator= (const cStatusDevice &Referenz)
 {
     this->bPowerOnFlag = Referenz.bPowerOnFlag;
     this->bFeedInFlag = Referenz.bFeedInFlag;
     this->iDeviceSN = Referenz.iDeviceSN;
     return *this;
 }

/* Destruktor */
cStatusDevice::~cStatusDevice(void)
{
}



