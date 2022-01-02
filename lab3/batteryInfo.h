//
// Created by danilapoddubny on 14.10.2021.
//

#ifndef LAB3_BATTERYINFO_H
#define LAB3_BATTERYINFO_H

#include <iostream>
#include "Windows.h"
#include "BatClass.h"
#pragma comment (lib, "Setupapi.lib")
#include <setupapi.h>
#include <devguid.h>
#include <locale.h>
#include <vector>
#include <atlstr.h>

void printBatteryInfo(BATTERY_INFORMATION b);
BATTERY_INFORMATION GetBatteryState();


#endif //LAB3_BATTERYINFO_H
