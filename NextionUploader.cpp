#include "NextionUploader.h"

#if defined(ESP8266) || defined(ESP32)
  #define FileSystem SPIFFS
#else
  #define FileSystem SD
#endif

NextionUploader::NextionUploader(Nextion& nextion, const char* filename, uint8_t SD_chip_select) : m_nextion(nextion)
{
  m_filename = filename;
  m_sdChipSelect = SD_chip_select;
}

NextionUploader::NextionUploader(Nextion& nextion, const String filename, uint8_t SD_chip_select) : NextionUploader(nextion, filename.c_str(), SD_chip_select)
{
}

bool NextionUploader::upload()
{
  if(!checkFile())
  {
    return false;
  }

  if(!initUpload())
  {
    return false;
  }

  if(!uploadFile())
  {
    return false;
  }
}

bool NextionUploader::checkFile()
{
#if defined(ESP8266) || defined(ESP32)
  if(!FileSystem.begin())
#else
  if(!FileSystem.begin(SD_chip_select))
#endif
    return false;

  if(!FileSystem.exists(m_filename))
    return false;
}

bool NextionUploader::initUpload()
{
  char string[16] = {0};
  m_nextion.sendCommand("");
  m_nextion.sendCommand("connect");
  m_nextion.receiveString(string, 16);
  if(strcmp("comok", string) != -1)
  {
    return false;
  }
  return true;
}

#define STR_LEN_BUF 64

bool NextionUploader::uploadFile()
{
  File file = FileSystem.open(m_filename, "r");
  if(!file.available())
    return false;

  int8_t c = 0;
  char string[STR_LEN_BUF + 1] = {0};
  uint32_t filesize = file.size();
  uint16_t send_timer = filesize / 4096 + 1;
  uint16_t last_send_num = filesize % 4096;

  while(send_timer)
  {
      if(send_timer == 1)
      {
          for(uint16_t j = 1; j <= 4096; j++)
          {
              if(j <= last_send_num)
              {
                  c = file.read();
                  m_nextion.sendRaw((char*)&c);
              }
              else
              {
                  break;
              }
          }
      }

      else
      {
          for(uint16_t i = 1; i <= 4096; i++)
          {
              c = file.read();
              m_nextion.sendRaw((char*)&c);
          }
      }

      m_nextion.receiveString(string, STR_LEN_BUF);

      if(strchr(string,0x05) != NULL)
      {
          string[0] = 0;
      }
      else
      {
          return 0;
      }
       --send_timer;
  }
  return true;
}
