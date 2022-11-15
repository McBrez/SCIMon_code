// Project includes
#include <device_isx3.hpp>

namespace Devices {

DeviceIsx3::DeviceIsx3() : Device(), handle(new FT_HANDLE()) {}

bool DeviceIsx3::configure(DeviceConfiguration *deviceConfiguration) {
  return true;
}

bool DeviceIsx3::open() { return true; }

bool DeviceIsx3::close() { return true; }

bool DeviceIsx3::write(shared_ptr<InitDeviceMessage> initMsg) {
  FT_STATUS ftStatus;
  FT_DEVICE_LIST_INFO_NODE *devInfo;
  DWORD numDevs;

  // create the device information list
  ftStatus = FT_CreateDeviceInfoList(&numDevs);

  // allocate storage for list based on numDevs
  devInfo = (FT_DEVICE_LIST_INFO_NODE *)malloc(
      sizeof(FT_DEVICE_LIST_INFO_NODE) * numDevs);

  // get the device information list
  ftStatus = FT_GetDeviceInfoList(devInfo, &numDevs);
  if (ftStatus != FT_OK) {
    printf("Error while reading device-count");
    return false;
  }
  printf("Number of devices found: %d\n\n", (int)numDevs);
  int i;
  for (i = 0; i < numDevs; i++) {
    printf("#%i\t%#.4X\t%#.4X\n", i, ((int)devInfo[i].ID >> 16) & 0xFFFF,
           ((int)devInfo[i].ID) & 0xFFFF);
  }
  for (i = 0; i < numDevs; i++) {
    if ((devInfo[i].ID & 0xFFFF) == ISX3_PID) {
      ftStatus = FT_Open(i, this->handle.get());
      if (ftStatus != FT_OK) {
        printf("Error while connecting to device");
        return false;
      }
      ftStatus = FT_SetBitMode(*handle, 0x00, 0x40);
      break;
    }
  }
  if (i == numDevs) {
    printf("No ISX3 found");
    return false;
  }
  return true;
}

bool DeviceIsx3::write(shared_ptr<WriteDeviceMessage> writeMsg) {
  int dataCount = 0;
  char *cmd;

  DWORD bytesWritten;
  char i;
  for (i = 0; i < dataCount; i++)
    printf("%.2X ", cmd[i]);
  printf("\n");
  FT_Write(*this->handle, cmd, dataCount, &bytesWritten);

  return true;
}

shared_ptr<ReadDeviceMessage> DeviceIsx3::read() {
  return shared_ptr<ReadDeviceMessage>();
}

char DeviceIsx3::readAck() {
  DWORD availableBytes = 0;
  DWORD bytesToWrite = 0;
  DWORD currentStatus = 0;
  DWORD bytesRead = 0;
  char *readBuffer;
  while (availableBytes != 4)
    FT_GetStatus(*this->handle, &availableBytes, &bytesToWrite, &currentStatus);
  readBuffer = (char *)malloc(sizeof(char) * availableBytes);
  FT_Read(*this->handle, readBuffer, availableBytes, &bytesRead);
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

} // namespace Devices