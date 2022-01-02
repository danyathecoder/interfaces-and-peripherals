#include "Windows.h"
#include "batteryInfo.h"

int main() {
    BATTERY_INFORMATION b;
    while (1)
    {
        fflush(stdin);
        b = GetBatteryState();
        printBatteryInfo(b);
        Sleep(1000);
    }
}


