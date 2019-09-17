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

#include <kodi/addon-instance/AudioDecoder.h>
#include <kodi/General.h>
#include <kodi/Filesystem.h>
#include <kodi/gui/dialogs/OK.h>
#include <fluidsynth.h>

struct FluidContext
{
  fluid_settings_t* settings = nullptr;
  fluid_synth_t* synth = nullptr;
  fluid_player_t* player = nullptr;
};

class ATTRIBUTE_HIDDEN CFluidCodec : public kodi::addon::CInstanceAudioDecoder
{
public:
  CFluidCodec(KODI_HANDLE instance) :
    CInstanceAudioDecoder(instance)
  {
    m_soundfont = kodi::GetSettingString("soundfont");
  }

  ~CFluidCodec() override
  {
    if (ctx.player)
      delete_fluid_player(ctx.player);
    if (ctx.synth)
      delete_fluid_synth(ctx.synth);
    if (ctx.settings)
      delete_fluid_settings(ctx.settings);
  }

  bool Init(const std::string& filename, unsigned int filecache,
            int& channels, int& samplerate,
            int& bitspersample, int64_t& totaltime,
            int& bitrate, AEDataFormat& format,
            std::vector<AEChannel>& channellist) override
  {
    if (m_soundfont.empty() || m_soundfont == "OFF")
    {
      kodi::QueueNotification(QUEUE_ERROR, kodi::GetLocalizedString(30010), kodi::GetLocalizedString(30011));
      return false;
    }
    kodi::vfs::CFile file;
    if (!file.OpenFile(filename, 0))
      return false;

    ctx.settings = new_fluid_settings();
    ctx.synth = new_fluid_synth(ctx.settings);
    fluid_synth_sfload(ctx.synth, m_soundfont.c_str(), 1);
    ctx.player = new_fluid_player(ctx.synth);

    size_t size = file.GetLength();
    char* temp = new char[size];
    file.Read(temp, size);
    file.Close();
    fluid_player_add_mem(ctx.player, temp, size);
    delete[] temp;
    fluid_player_play(ctx.player);
    format = AE_FMT_FLOAT;
    channellist = { AE_CH_FL, AE_CH_FR };
    channels = 2;

    bitspersample = 32;
    bitrate = 0.0;

    samplerate = 44100;
    totaltime = 0;

    return true;
  }

  int ReadPCM(uint8_t* buffer, int size, int& actualsize) override
  {
    if (fluid_player_get_status(ctx.player) == FLUID_PLAYER_DONE)
      return 1;

    fluid_synth_write_float(ctx.synth, size/8, buffer, 0, 2, buffer, 1, 2);
    actualsize = size;
    return 0;
  }

  int64_t Seek(int64_t time) override
  {
    return -1;
  }

private:
  FluidContext ctx;
  std::string m_soundfont;
};


class ATTRIBUTE_HIDDEN CMyAddon : public kodi::addon::CAddonBase
{
public:
  CMyAddon() = default;
  ADDON_STATUS CreateInstance(int instanceType, std::string instanceID, KODI_HANDLE instance, KODI_HANDLE& addonInstance) override
  {
    addonInstance = new CFluidCodec(instance);
    return ADDON_STATUS_OK;
  }
  ~CMyAddon() = default;
};


ADDONCREATOR(CMyAddon);
