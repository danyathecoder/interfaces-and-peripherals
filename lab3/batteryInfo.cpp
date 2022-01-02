//
// Created by danilapoddubny on 14.10.2021.
//

#include "batteryInfo.h"

void printBatteryInfo(BATTERY_INFORMATION b)
{
    system("cls");
    std::cout << "Press Q to quit. Press S to enter 'Sleep Mode'. Press H to enter 'Hibernation Mode'. Press another to continue\n\n";

    SYSTEM_POWER_STATUS status; //get power status
    GetSystemPowerStatus(&status);

    if (status.ACLineStatus) {
        std::cout << "# Working from: \tAC" << std::endl;
        std::cout << "# Charging: \t\t";
    }
    else {
        std::cout << "# Working from: \tbattery" << std::endl;
        std::cout << "# Remaining: \t\t";
    }

    std::cout << (int) status.BatteryLifePercent << "%" << std::endl;


    if (!status.ACLineStatus) {

        if (status.SystemStatusFlag) {
            std::cout << "# Battery saver is: \ton (Save energy where possible)" << std::endl;
        }
        else {
            std::cout << "# Battery saver is: \toff" << std::endl;
        }


        unsigned long hours = status.BatteryLifeTime / 3600;
        unsigned long minutes = (status.BatteryLifeTime - hours * 3600) / 60;
        std::cout << "# Battery life: \t";
        if (status.BatteryLifeTime == 4294967295) {
            std::cout << "computing\n";
        }
        else {
            std::cout << hours << "h " << minutes << "m\n";
        }

    }

    std::cout << "# Battery type: \t";
    std::cout << b.Chemistry[0] << b.Chemistry[1] << b.Chemistry[2] << b.Chemistry[3] << std::endl;

    char key;
    std::cout << "Press key: \n";
    key = getchar();

    if (key == 'S')
    {
        //User pressed 'S'
        system("powercfg -hibernate off");
        system("rundll32.exe powrprof.dll, SetSuspendState Sleep");
    }
    if (key == 'H')
    {
        //User pressed 'H'
        system("powercfg -hibernate on");
        system("rundll32.exe powrprof.dll, SetSuspendState Sleep");
    }
    if (key == 'Q')
    {
        //User pressed 'Q'
        exit(0);
    }


}

BATTERY_INFORMATION GetBatteryState()
{
    BATTERY_INFORMATION bb;

    HDEVINFO hdev =
            SetupDiGetClassDevs(&GUID_DEVCLASS_BATTERY,
                                0,
                                0,
                                DIGCF_PRESENT | DIGCF_DEVICEINTERFACE); //enumeration of battery device
    if (INVALID_HANDLE_VALUE != hdev)
    {
        SP_DEVICE_INTERFACE_DATA did = { 0 };
        did.cbSize = sizeof(did);

        if (SetupDiEnumDeviceInterfaces(hdev,
                                        0,
                                        &GUID_DEVCLASS_BATTERY,
                                        0,
                                        &did))
        {
            DWORD cbRequired = 0;

            SetupDiGetDeviceInterfaceDetail(hdev,
                                            &did,
                                            0,
                                            0,
                                            &cbRequired,
                                            0);
            if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
            {
                PSP_DEVICE_INTERFACE_DETAIL_DATA pdidd =
                        (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR,
                                                                     cbRequired);
                if (pdidd)
                {
                    pdidd->cbSize = sizeof(*pdidd);
                    if (SetupDiGetDeviceInterfaceDetail(hdev,
                                                        &did,
                                                        pdidd,
                                                        cbRequired,
                                                        &cbRequired,
                                                        0))
                    {
                        HANDLE hBattery =
                                CreateFile(pdidd->DevicePath,
                                           GENERIC_READ | GENERIC_WRITE,
                                           FILE_SHARE_READ | FILE_SHARE_WRITE,
                                           NULL,
                                           OPEN_EXISTING,
                                           FILE_ATTRIBUTE_NORMAL,
                                           NULL);
                        if (INVALID_HANDLE_VALUE != hBattery)
                        {
                            BATTERY_QUERY_INFORMATION bqi = { 0 };

                            DWORD dwWait = 0;
                            DWORD dwOut;

                            if (DeviceIoControl(hBattery,
                                                IOCTL_BATTERY_QUERY_TAG,
                                                &dwWait,
                                                sizeof(dwWait),
                                                &bqi.BatteryTag,
                                                sizeof(bqi.BatteryTag),
                                                &dwOut,
                                                NULL)
                                && bqi.BatteryTag)
                            {
                                BATTERY_INFORMATION bi = { 0 };
                                bqi.InformationLevel = BatteryInformation;

                                if (DeviceIoControl(hBattery,
                                                    IOCTL_BATTERY_QUERY_INFORMATION,
                                                    &bqi,
                                                    sizeof(bqi),
                                                    &bi,
                                                    sizeof(bi),
                                                    &dwOut,
                                                    NULL))
                                {
                                    bb = bi;
                                }
                            }
                        }
                    }
                }
            }
        }

    }

    return bb;
}