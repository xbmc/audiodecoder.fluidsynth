/*
 *      Copyright (C) 2014 Arne Morten Kvarving
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "libXBMC_addon.h"
#include <fluidsynth.h>

extern "C" {
#include <stdio.h>
#include <stdint.h>

#include "kodi_audiodec_dll.h"

ADDON::CHelper_libXBMC_addon *XBMC           = NULL;
char soundfont[1024];

ADDON_STATUS ADDON_Create(void* hdl, void* props)
{
  if (!XBMC)
    XBMC = new ADDON::CHelper_libXBMC_addon;

  if (!XBMC->RegisterMe(hdl))
  {
    delete XBMC, XBMC=NULL;
    return ADDON_STATUS_PERMANENT_FAILURE;
  }

  return ADDON_STATUS_NEED_SETTINGS;
}

//-- Stop ---------------------------------------------------------------------
// This dll must cease all runtime activities
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
void ADDON_Stop()
{
}

//-- Destroy ------------------------------------------------------------------
// Do everything before unload of this add-on
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
void ADDON_Destroy()
{
  delete XBMC;
  XBMC = NULL;
}

//-- HasSettings --------------------------------------------------------------
// Returns true if this add-on use settings
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
bool ADDON_HasSettings()
{
  return true;
}

//-- GetStatus ---------------------------------------------------------------
// Returns the current Status of this visualisation
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
ADDON_STATUS ADDON_GetStatus()
{
  return ADDON_STATUS_OK;
}

//-- GetSettings --------------------------------------------------------------
// Return the settings for XBMC to display
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
unsigned int ADDON_GetSettings(ADDON_StructSetting ***sSet)
{
  return 0;
}

//-- FreeSettings --------------------------------------------------------------
// Free the settings struct passed from XBMC
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------

void ADDON_FreeSettings()
{
}

//-- SetSetting ---------------------------------------------------------------
// Set a specific Setting value (called from XBMC)
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
ADDON_STATUS ADDON_SetSetting(const char *strSetting, const void* value)
{
  if (strcmp(strSetting,"soundfont") == 0)
    strcpy(soundfont, (const char*)value);

  return ADDON_STATUS_OK;
}

//-- Announce -----------------------------------------------------------------
// Receive announcements from XBMC
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
void ADDON_Announce(const char *flag, const char *sender, const char *message, const void *data)
{
}

struct FluidContext
{
  fluid_settings_t* settings;
  fluid_synth_t* synth;
  fluid_player_t* player;
};

void* Init(const char* strFile, unsigned int filecache, int* channels,
           int* samplerate, int* bitspersample, int64_t* totaltime,
           int* bitrate, AEDataFormat* format, const AEChannel** channelinfo)
{
  FluidContext* result = new FluidContext;
  result->settings = new_fluid_settings();
  result->synth = new_fluid_synth(result->settings);
  fluid_synth_sfload(result->synth, soundfont, 1);
  result->player = new_fluid_player(result->synth);
  
  void* f = XBMC->OpenFile(strFile, 0);
  size_t size = XBMC->GetFileLength(f);
  char* temp = new char[size];
  XBMC->ReadFile(f, temp, size);
  XBMC->CloseFile(f);
  fluid_player_add_mem(result->player, temp, size);
  delete[] temp;
  fluid_player_play(result->player);
  static enum AEChannel map[3] = {
    AE_CH_FL, AE_CH_FR, AE_CH_NULL
  };
  *format = AE_FMT_FLOAT;
  *channelinfo = map;
  *channels = 2;

  *bitspersample = 32;
  *bitrate = 0.0;

  *samplerate = 44100;
  *totaltime = 0;

  return result;
}

int ReadPCM(void* context, uint8_t* pBuffer, int size, int *actualsize)
{
  FluidContext* ssf = (FluidContext*)context;
  if (fluid_player_get_status(ssf->player) == FLUID_PLAYER_DONE)
    return 1;
  fluid_synth_write_float(ssf->synth, size/8, pBuffer, 0, 2, pBuffer, 1, 2);

  *actualsize = size;
  return 0;
}

int64_t Seek(void* context, int64_t time)
{
  return -1;
}

bool DeInit(void* context)
{
  FluidContext* ssf = (FluidContext*)context;
  delete_fluid_player(ssf->player);
  delete_fluid_synth(ssf->synth);
  delete_fluid_settings(ssf->settings);
  delete ssf;

  return true;
}

bool ReadTag(const char* strFile, char* title, char* artist, int* length)
{
  return false;
}

int TrackCount(const char* strFile)
{
  return 1;
}
}
