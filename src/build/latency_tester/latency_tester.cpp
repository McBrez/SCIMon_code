// 3rd party includes
#include "windows.h"
#include <easylogging++.h>
#include <ftd2xx.h>

// Project includes
// #include <impedance_spectrometer.hpp>
// #include <main_control.hpp>
// #include <message_impedance_spectrometer.hpp>

using namespace std;
// using namespace Devices;
// using namespace Control;

// The product id of the sciospec ISX3.
#define ISX3_PID 0x89D0

char connectToIsx3(FT_HANDLE *handle) {
  FT_STATUS ftStatus;
  FT_DEVICE_LIST_INFO_NODE *devInfo;
  DWORD numDevs;
  // create the device information list
  ftStatus = FT_CreateDeviceInfoList(&numDevs);
  // allocate storage for list based on numDevs
  devInfo = (FT_DEVICE_LIST_INFO_NODE *)malloc(
      sizeof(FT_DEVICE_LIST_INFO_NODE) *
      numDevs); // get the device information list
  ftStatus = FT_GetDeviceInfoList(devInfo, &numDevs);
  if (ftStatus != FT_OK) {
    printf("Error while reading device-count");
    return FALSE;
  }
  printf("Number of devices found: %d\n\n", (int)numDevs);
  int i;
  for (i = 0; i < numDevs; i++) {
    printf("#%i\t%#.4X\t%#.4X\n", i, ((int)devInfo[i].ID >> 16) & 0xFFFF,
           ((int)devInfo[i].ID) & 0xFFFF);
  }
  for (i = 0; i < numDevs; i++) {
    if ((devInfo[i].ID & 0xFFFF) == ISX3_PID) {
      ftStatus = FT_Open(i, handle);
      if (ftStatus != FT_OK) {
        printf("Error while connecting to device");
        return FALSE;
      }
      ftStatus = FT_SetBitMode(*handle, 0x00, 0x40);
      break;
    }
  }
  if (i == numDevs) {
    printf("No ISX3 found");
    return FALSE;
  }
  return TRUE;
}

void writeDataToDevice(FT_HANDLE handle, char *cmd, DWORD dataCount) {
  DWORD bytesWritten;
  char i;
  for (i = 0; i < dataCount; i++)
    printf("%.2X ", cmd[i]);
  printf("\n");
  FT_Write(handle, cmd, dataCount, &bytesWritten);
}

char readAck(FT_HANDLE handle) {
  DWORD availableBytes = 0;
  DWORD bytesToWrite = 0;
  DWORD currentStatus = 0;
  DWORD bytesRead = 0;
  char *readBuffer;
  while (availableBytes != 4)
    FT_GetStatus(handle, &availableBytes, &bytesToWrite, &currentStatus);
  readBuffer = (char *)malloc(sizeof(char) * availableBytes);
  FT_Read(handle, readBuffer, availableBytes, &bytesRead);
  printf("ACK-Frame: ");
  UINT8 i;
  for (i = 0; i < availableBytes; i++) {
    printf("%.2X ", readBuffer[i]);
  }
  printf("\n");
  if (readBuffer[2] == 0x83) {
    free(readBuffer);
    return TRUE;
  } else {
    free(readBuffer);
    return FALSE;
  }
}

INITIALIZE_EASYLOGGINGPP

int main() {
  LOG(INFO) << "Starting up.";

  LOG(INFO) << "Initializing impedance spectrometer.";
  FT_HANDLE handle;
  if (!connectToIsx3(&handle)) {
    LOG(INFO) << "Could not connect to ISX3. Aborting.";
    return -1;
  }

  LOG(INFO) << "Connection established";

  LOG(INFO) << "Shutting down.";
  return 0;
}