#include <Arduino.h>
#include <SPI.h>

#define CAM_ADDR       0
#define CAM_SERIAL     Serial1

const byte cameraAddr = (CAM_ADDR << 5);

/*********************************************************************/
void setup()
{
  CAM_SERIAL.begin(9600);

  initialize();
  setBaud(0x0f,0x01);
  CAM_SERIAL.begin(115200);
}
/*********************************************************************/
void loop(){}

void clearRxBuf()
{
  while (CAM_SERIAL.available())
  {
    CAM_SERIAL.read();
  }
}
/*********************************************************************/
void sendCmd(char cmd[], int cmd_len)
{
  for (char i = 0; i < cmd_len; i++) CAM_SERIAL.write(cmd[i]);
}
/*********************************************************************/
int readBytes(char *dest, int len, unsigned int timeout)
{
  int read_len = 0;
  unsigned long t = millis();
  while (read_len < len)
  {
    while (CAM_SERIAL.available()<1)
    {
      if ((millis() - t) > timeout)
      {
        return read_len;
      }
    }
    *(dest+read_len) = CAM_SERIAL.read();
    read_len++;
  }
  return read_len;
}
/*********************************************************************/
void initialize()
{
  char cmd[] = {0xaa,0x0d|cameraAddr,0x00,0x00,0x00,0x00};
  unsigned char resp[6];
  CAM_SERIAL.setTimeout(100);

  while (1)
  {
    sendCmd(cmd,6);
    if (readBytes((char *)resp, 6, 1000) != 6)
    {
      continue;
    }
    if (resp[0] == 0xaa && resp[1] == (0x0e | cameraAddr) && resp[2] == 0x0d && resp[4] == 0 && resp[5] == 0)
    {
      if (readBytes((char *)resp, 6, 500) != 6) continue;
      if (resp[0] == 0xaa && resp[1] == (0x0d | cameraAddr) && resp[2] == 0 && resp[3] == 0 && resp[4] == 0 && resp[5] == 0) break;
    }
  }
}
/*********************************************************************/
void setBaud(char BPS_A, char BPS_B) {
  char cmd[] = { 0xaa, 0x07, BPS_A, BPS_B, 0, 0};
  unsigned char resp[6];
  CAM_SERIAL.setTimeout(100);
  while (1) {
    clearRxBuf();
    sendCmd(cmd, 6);
    uint8_t x = CAM_SERIAL.readBytes((char *)resp, 6);
    if (x != 6) continue;
    if (resp[0] == 0xaa && resp[1] == 0x0e && resp[2] == 0x07 && resp[4] == 0 && resp[5] == 0) {
      break;
    }
  }
}
