/*! \file */

#ifndef __NEONEXTION_NEXTIONUPLOADER
#define __NEONEXTION_NEXTIONUPLOADER

#include "Nextion.h"
#if defined(ESP8266) || defined(ESP32)
#include <FS.h>
#else
#include <SD.h>
#endif

/*!
 * \class NextionUploader
 * \brief Is used to upload a new TFT file to the Nextion display
 */


class NextionUploader
{
public:
  NextionUploader(Nextion &nex, const char* filename, uint8_t SD_chip_select = 10);
  NextionUploader(Nextion &nex, const String filename, uint8_t SD_chip_select = 10);

  bool upload();

protected:
  bool checkFile();
  bool initUpload();
  bool uploadFile();

  Nextion& m_nextion;

  const char* m_filename;

  uint8_t m_sdChipSelect;
};

#endif
