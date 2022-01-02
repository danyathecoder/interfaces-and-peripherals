#include <iostream>
#include <windows.h>
#pragma comment (lib, "Setupapi.lib")
#include <stdio.h>
#include <string>
#include <atlstr.h>
#include <Ntddscsi.h>
#include <winioctl.h>

#define BYTE_SIZE 8


using namespace std;

void printLogicalDriveInfo(LPCTSTR driveName);
const char* InterfaceTypeEnumToString(_STORAGE_BUS_TYPE type);
void printDriveInfo(HANDLE diskHandle, STORAGE_PROPERTY_QUERY storageProtertyQuery);

BOOL getAtaCompliance(HANDLE hDevice) {
    DWORD dwBytes;
    BOOL  bResult;

    CONST UINT bufferSize = 512;
    CONST BYTE identifyDataCommandId = 0xEC;

    UCHAR identifyDataBuffer[bufferSize
                             + sizeof(ATA_PASS_THROUGH_EX)] = { 0 };

    ATA_PASS_THROUGH_EX& PTE = *(ATA_PASS_THROUGH_EX*)identifyDataBuffer;
    PTE.Length = sizeof(PTE);
    PTE.TimeOutValue = 10;
    PTE.DataTransferLength = 512;
    PTE.DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX);

    IDEREGS* ideRegs = (IDEREGS*)PTE.CurrentTaskFile;
    ideRegs->bCommandReg = identifyDataCommandId;
    ideRegs->bSectorCountReg = 1;

    PTE.AtaFlags = ATA_FLAGS_DATA_IN | ATA_FLAGS_DRDY_REQUIRED;

    bResult = DeviceIoControl(hDevice, IOCTL_ATA_PASS_THROUGH, &PTE,
                              sizeof(identifyDataBuffer), &PTE,
                              sizeof(identifyDataBuffer), &dwBytes, 0);

    if (bResult == FALSE) {
        std::cout << "Oops, something went wrong, error code: "
                  << GetLastError() << std::endl;
        return bResult;
    }

    WORD* data = (WORD*)(identifyDataBuffer + sizeof(ATA_PASS_THROUGH_EX));

    int i, bitArray[2 * BYTE_SIZE];

    std::cout << "\nSupported standarts:";

    short ataSupportByte = data[80];
    i = 2 * BYTE_SIZE;
    while (i--) {
        bitArray[i] = ataSupportByte & 32768 ? 1 : 0;
        ataSupportByte = ataSupportByte << 1;
    }
    std::cout << "\n\tATA Support:\t";
    for (i = 8; i >= 4; i--) {
        if (bitArray[i] == 1) {
            std::cout << "ATA" << i;
            if (i != 4) {
                std::cout << ", ";
            }
        }
    }

    unsigned short pioSupportedBytes = data[63];
    i = 2 * BYTE_SIZE;
    while (i--) {
        bitArray[i] = pioSupportedBytes & 32768 ? 1 : 0;
        pioSupportedBytes = pioSupportedBytes << 1;
    }
    std::cout << "\n\tPIO Support:\t";
    for (i = 0; i < 2; i++) {
        if (bitArray[i] == 1) {
            std::cout << "PIO" << i + 3;
            if (i != 1) std::cout << ", ";
        }
    }

    unsigned short dmaSupportedBytes = data[63];
    i = 2 * BYTE_SIZE;
    while (i--) {
        bitArray[i] = dmaSupportedBytes & 32768 ? 1 : 0;
        dmaSupportedBytes = dmaSupportedBytes << 1;
    }
    std::cout << "\n\tDMA Support:\t";
    for (i = 0; i < 8; i++) {
        if (bitArray[i] == 1) {
            std::cout << "DMA" << i;
            if (i != 2) std::cout << ", ";
        }
    }

    std::cout << std::endl;

    return bResult;
}


int main() {
    STORAGE_PROPERTY_QUERY storageProtertyQuery;
    storageProtertyQuery.QueryType = PropertyStandardQuery;
    storageProtertyQuery.PropertyId = StorageDeviceProperty;

    HANDLE diskHandle = CreateFileA("//./PhysicalDrive0", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (diskHandle == INVALID_HANDLE_VALUE) {
        if (GetLastError() == 5) cout << "Access denied: run program as administrator\n";
        else if (GetLastError() == 2) cout << "Drive not foud\n";
        else cout << "Unknown error during opening drive\n";
        cout << "(Error: " << GetLastError() << ")" << endl;
        return -1;
    }

    printDriveInfo(diskHandle, storageProtertyQuery);

    // сведения о памяти для каждого логического диска (свободно / занято / всего)
    DWORD mydrives = 200;
    char lpBuffer[200];
    DWORD test = GetLogicalDriveStringsW(200, (LPWSTR)lpBuffer);
    if (test != 0) {
        for (int i = 0; i < 100; i++) {
            if (lpBuffer[i] == -52) break;
            if (lpBuffer[i] == 0 || lpBuffer[i] == '\\' || lpBuffer[i] == ':') continue;
            CString s;
            LPCTSTR lp;
            s.AppendChar(lpBuffer[i]);
            s.AppendChar(':');
            s.AppendChar('\\');
            lp = (LPCTSTR)s;
            printLogicalDriveInfo(lp);
        }
        cout << endl;
    }
    else {
        cout << "# failed to get logical drives\n";
    }

    getAtaCompliance(diskHandle);

    CloseHandle(diskHandle);
}

void printLogicalDriveInfo(LPCTSTR driveName) {
    ULARGE_INTEGER ufree = { 0 }, utotal = { 0 };
    bool f = GetDiskFreeSpaceEx(driveName, &ufree, &utotal, NULL);
    if (f) {
        cout << "# info for " << (char)*driveName << ":" << endl;
        cout << "\t\t\tfree: \t" << ufree.QuadPart << "\tbytes" << endl;
        cout << "\t\t\ttotal: \t" << utotal.QuadPart << "\tbytes" << endl;
        cout << "\t\t\tused: \t" << utotal.QuadPart - ufree.QuadPart << "\tbytes" << endl;
    }
    else {
        cout << "# failed to get infor for " << (char)*driveName << endl;
    }
}

const char* InterfaceTypeEnumToString(_STORAGE_BUS_TYPE type) {
    switch (type) {
        case BusTypeUnknown:			return "Unknown"; break;
        case BusTypeScsi:				return "Scsi"; break;
        case BusTypeAtapi:				return "Atapi"; break;
        case BusTypeAta:				return "Ata"; break;
        case BusType1394:				return "1394"; break;
        case BusTypeSsa:				return "Ssa"; break;
        case BusTypeFibre:				return "Fibre"; break;
        case BusTypeUsb:				return "Usb"; break;
        case BusTypeRAID:				return "RAID"; break;
        case BusTypeiScsi:				return "iScsi"; break;
        case BusTypeSas:				return "Sas"; break;
        case BusTypeSata:				return "Sata"; break;
        case BusTypeSd:					return "Sd"; break;
        case BusTypeMmc:				return "Mmc"; break;
        case BusTypeVirtual:			return "Virtual"; break;
        case BusTypeFileBackedVirtual:	return "FileBackedVirtual"; break;
        case BusTypeSpaces:				return "Spaces"; break;
        case BusTypeNvme:				return "Nvme"; break;
        case BusTypeSCM:				return "SCM"; break;
        case BusTypeUfs:				return "Ufs"; break;
        case BusTypeMax:				return "Max"; break;
        case BusTypeMaxReserved:		return "MaxReserved"; break;
        default:						return "Unknown";
    }
}

void printDriveInfo(HANDLE diskHandle, STORAGE_PROPERTY_QUERY storageProtertyQuery) {
    STORAGE_DEVICE_DESCRIPTOR* deviceDescriptor = (STORAGE_DEVICE_DESCRIPTOR*)calloc(1024, 1);
    deviceDescriptor->Size = 1024;

    if (!DeviceIoControl(diskHandle, IOCTL_STORAGE_QUERY_PROPERTY, &storageProtertyQuery, sizeof(storageProtertyQuery), deviceDescriptor, 1024, NULL, 0)) {
        cout << "Unknown error\n";
        CloseHandle(diskHandle);
        exit(-1);
    }

    cout << "# Model:\t\t" << (char*)(deviceDescriptor)+deviceDescriptor->ProductIdOffset << endl;
    cout << "# Firmware Revision:\t" << (char*)(deviceDescriptor)+deviceDescriptor->ProductRevisionOffset << endl;
    cout << "# Serial number:\t" << (char*)(deviceDescriptor)+deviceDescriptor->SerialNumberOffset << endl;
    cout << "# Manufacturer:\t" << (deviceDescriptor->VendorIdOffset != 0 ? (char*)(deviceDescriptor)+deviceDescriptor->VendorIdOffset : "\tNot specified") << endl;
    cout << "# Interface Type:\t" << InterfaceTypeEnumToString(deviceDescriptor->BusType) << endl;
}