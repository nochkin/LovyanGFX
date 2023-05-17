/*----------------------------------------------------------------------------/
  Lovyan GFX - Graphics library for embedded devices.

Original Source:
 https://github.com/lovyan03/LovyanGFX/

Licence:
 [FreeBSD](https://github.com/lovyan03/LovyanGFX/blob/master/license.txt)

Author:
 [lovyan03](https://twitter.com/lovyan03)

Contributors:
 [ciniml](https://github.com/ciniml)
 [mongonta0716](https://github.com/mongonta0716)
 [tobozo](https://github.com/tobozo)
/----------------------------------------------------------------------------*/
#include "Panel_LT768x.hpp"
#include "../Bus.hpp"
#include "../platforms/common.hpp"
#include "../misc/pixelcopy.hpp"

#include "../platforms/device.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  const uint8_t* Panel_LT768x::getInitCommands(uint8_t listno) const
  {
    uint16_t plldivm_sclk, plldivm_cclk, plldivm_mclk;
    uint16_t plldivn_sclk, plldivn_cclk, plldivn_mclk;
    uint16_t plldivk_sclk, plldivk_cclk, plldivk_mclk;
    uint16_t plldivk_sclkpow, plldivk_cclkpow, plldivk_mclkpow;
    uint16_t temp_sclk, temp_cclk, temp_mclk;
 
    //=====[05h] b3-b1
    #ifdef SCAN_DIVK1           // 000   
      plldivk_sclk = 0;  
      plldivk_sclkpow = 1;
    #endif
    #ifdef SCAN_DIVK2     // 001  
      plldivk_sclk = 1;
      plldivk_sclkpow = 2;  
    #endif
    #ifdef SCAN_DIVK4       // 010
      plldivk_sclk = 2;
      plldivk_sclkpow = 4;
    #endif
    #ifdef SCAN_DIVK8       // 011
      plldivk_sclk = 3;
      plldivk_sclkpow = 8;
    #endif 
    #ifdef SCAN_DIVK16      // 100
      plldivk_sclk = 4;
      plldivk_sclkpow = 16;
    #endif
    #ifdef SCAN_DIVK32      // 101
      plldivk_sclk = 5;
      plldivk_sclkpow = 32;
    #endif
    #ifdef SCAN_DIVK64      // 110
      plldivk_sclk = 6;
      plldivk_sclkpow = 64;
    #endif
    #ifdef SCAN_DIVK128     // 111
      plldivk_sclk = 7;
      plldivk_sclkpow = 128;
    #endif
    //=====[05h] b0
    #ifdef SCAN_DIVM1       // 0
      plldivm_sclk = 0;
    #endif
    #ifdef SCAN_DIVM2       // 1
      plldivm_sclk = 1; 
    #endif
    //=====[07h] b3-b1
    #ifdef DRAM_DIVK1       // 000 
      plldivk_mclk = 0; 
      plldivk_mclkpow = 1;
    #endif
    #ifdef DRAM_DIVK2     // 001
      plldivk_mclk = 1; 
      plldivk_mclkpow = 2;
    #endif
    #ifdef DRAM_DIVK4       // 010
      plldivk_mclk = 2; 
      plldivk_mclkpow = 4;
    #endif
    #ifdef DRAM_DIVK8       // 011
      plldivk_mclk = 3; 
      plldivk_mclkpow = 8;
    #endif
    //=====[07h] b0
    #ifdef DRAM_DIVM1       // 0
      plldivm_mclk = 0; 
    #endif
    #ifdef DRAM_DIVM2     // 1
      plldivm_mclk = 1; 
    #endif
    //=====[09h] b3-b1
    #ifdef CORE_DIVK1       // 000
      plldivk_cclk = 0; 
      plldivk_cclkpow = 1;
    #endif
    #ifdef CORE_DIVK2     // 001
      plldivk_cclk = 1; 
      plldivk_cclkpow = 2;
    #endif
    #ifdef CORE_DIVK4       // 010
      plldivk_cclk = 2; 
      plldivk_cclkpow = 4;
    #endif
    #ifdef CORE_DIVK8       // 011 
      plldivk_cclk = 3; 
      plldivk_cclkpow = 8;
    #endif 
    //=====[09h] b0       
    #ifdef CORE_DIVM1       // 0
      plldivm_cclk = 0; 
    #endif
    #ifdef CORE_DIVM2     // 1
      plldivm_cclk = 1; 
    #endif

    // PLL = FIN*(PLLDIVN+1) / (PLLDIVM+1) * (2^PLLDIVK)
    // PLLDIVN = [(PLL * (PLLDIVM+1) * (2^PLLDIVK)) / FIN ] - 1
    plldivn_sclk = ((SCAN_FREQ * (plldivm_sclk + 1) * plldivk_sclkpow) / OSC_FREQ) - 1;
    plldivn_mclk = ((DRAM_FREQ * (plldivm_mclk + 1) * plldivk_mclkpow) / OSC_FREQ) - 1;
    plldivn_cclk = ((CORE_FREQ * (plldivm_cclk + 1) * plldivk_cclkpow) / OSC_FREQ) - 1;

    temp_sclk = (plldivk_sclk << 1) | plldivm_sclk;
    temp_mclk = (plldivk_mclk << 1) | plldivm_mclk;
    temp_cclk = (plldivk_cclk << 1) | plldivm_cclk;

    static uint8_t list0[] PROGMEM =
    {
      // SW Reset
      //0x00      , 1+CMD_INIT_BIT_SET, 0x01,
      // PLL init
      0x05      , 1, (uint8_t)temp_sclk,
      0x07      , 1, (uint8_t)temp_mclk,
      0x09      , 1, (uint8_t)temp_cclk,
      0x06      , 1, (uint8_t)plldivn_sclk,
      0x08      , 1, (uint8_t)plldivn_mclk,
      0x0a      , 1, (uint8_t)plldivn_cclk,
      0x01      , 1 + CMD_INIT_DELAY, 0x00, 1,
      0x01      , 1 + CMD_INIT_DELAY, 0x80, 10,
#ifdef W9812G6KH
      0xe0      , 1, 0x29,
      0xe1      , 1, 0x03, // CAS:2=0x01 ACAS:3=0x03
      0xe2      , 1, ((64000000 / 8192) / (1000/DRAM_FREQ) & 0xff),
      0xe3      , 1, ((64000000 / 8192) / (1000/DRAM_FREQ)) >> 8,
      0xe4      , 1, 0x01,
#endif
      0xFF,0xFF, // end
    };
    static uint8_t list1[] PROGMEM = 
    {
      0x01, 1+CMD_INIT_BIT_RESET, 0x01,       // Host Bus 8-bit
      0x02, 1+CMD_INIT_BIT_RESET, 0b11000000, // RGB_16b_24bpp_mode1
      0x02, 1+CMD_INIT_BIT_SET  , 0b00000110, // MemWrite_Down_Top_Left_Right. Rotation?
      0x03, 1+CMD_INIT_BIT_RESET, 0b00000100, // Graphic mode
      0x03, 1+CMD_INIT_BIT_RESET, 0b00000011, // Select SDRAM
      0xcd, 1+CMD_INIT_BIT_SET,   0b00010000, // Font_90_degree
      0x12, 1+CMD_INIT_BIT_SET,   0b00001000, // VSCAN_B_to_T
      0x12, 1+CMD_INIT_BIT_SET,   0b00000111, // PDATA_Set_RGB
      0x12, 1+CMD_INIT_BIT_SET,   0b00001000, // Display Test Color Bar
      0x12, 1+CMD_INIT_BIT_RESET, 0b10000000, // PCLK_Rising
      0x13, 1+CMD_INIT_BIT_RESET, 0b00100000, // DE_High_Active
      0x13, 1+CMD_INIT_BIT_RESET, 0b10000000, // HSYNC_Low_Active
      0x13, 1+CMD_INIT_BIT_RESET, 0b01000000, // VSYNC_Low_Active
      // LCD_HorizontalWidth_VerticalHeight(1280, 400)
      0x14, 1                   , (1280/8)-1,
      0x15, 1                   , 1280%8,
      0x1a, 1                   , (uint8_t)400-1,
      0x1b, 1                   , (400-1)>>8,
      // LCD_Horizontal_Non_Display(160)
      0x16, 1                   , (160/8)-1,
      0x17, 1                   , 160%8,
      0x18, 1                   , 160,        // LCD_HSYNC_Start_Position(160)
      0x19, 1                   , (24/8)-1,   // LCD_HSYNC_Pulse_Width(24)
      // LCD_Vertical_Non_Display(10)
      0x1c, 1                   , (uint8_t)10-1,
      0x1d, 1                   , (10-1)>>8,
      0x1e, 1                   , 12-1,       // LCD_VSYNC_Start_Position(12)
      0x1f, 1                   , 2-1,        // LCD_VSYNC_Pulse_Width(2)
      0x10, 1+CMD_INIT_BIT_SET  , 0b00001000, // Select_Main_Window_24bpp
      // Main_Image_Start_Address(0)
      0x20, 1                   , 0,
      0x21, 1                   , 0>>8,
      0x22, 1                   , 0>>16,
      0x23, 1                   , 0>>24,
      // Main_Image_Width(400)
      0x24, 1                   , 400 & 0xff,
      0x25, 1                   , 400 >> 8,
      // Main_Window_Start_XY(0, 0)
      0x26, 1                   , 0,
      0x27, 1                   , 0>>8,
      0x28, 1                   , 0,
      0x29, 1                   , 0>>8,
      // Canvas_Image_Start_address(0)
      0x50, 1                   , 0,
      0x51, 1                   , 0>>8,
      0x52, 1                   , 0>>16,
      0x53, 1                   , 0>>24,
      // Canvas_image_width(400)
      0x54, 1                   , 400 & 0xff,
      0x55, 1                   , 400>8,
      // Active_Window_XY(0, 0)
      0x56, 1                   , 0,
      0x57, 1                   , 0>>8,
      0x58, 1                   , 0,
      0x59, 1                   , 0>>8,
      // Active_Window_WH(400, 1280)
      0x5a, 1                   , 400 & 0xff,
      0x5b, 1                   , 400 >> 8,
      0x5c, 1                   , 1280 & 0xff,
      0x5d, 1                   , 1280 >> 8,
      0x5e, 1+CMD_INIT_BIT_RESET, 0b00000100, // Memory_XY_Mode
      // Memory_24bpp_Mode
      // Select_Main_Window_24bpp

      0xFF,0xFF, // end
    };
    switch (listno)
    {
    case 0: return list0;
    case 1: return list1;
    default: return nullptr;
    }
  }

  bool Panel_LT768x::init(bool use_reset)
  {
    _flg_serialbus = _bus->busType() == bus_spi;

    if (!Panel_Device::init(use_reset))
    {
      return false;
    }

    if (_cfg.pin_busy >= 0)
    {
      pinMode(_cfg.pin_busy, pin_mode_t::input);
    }

    uint32_t freq_write = _bus->getClock();
    if (freq_write > 5000000)
    {
      /// 初期化処理は低めのクロックで実施する。;
      /// LT768xは起動直後は動作クロックが低く、送信クロックが速すぎると処理されないため。;
      /// 初期化時に動作クロックを上げるコマンドが実行されるので、;
      /// 初期化後にユーザーが設定した本来のクロックに戻す。;
      _bus->setClock(5000000);
    }

    ESP_LOGI("LT768x", "Init");

    uint8_t readData;

    _bus->beginTransaction();
    readData = _read_reg(0x00);
    ESP_LOGI("LT768x", "Read 0x00: %02X", readData);
    _write_reg(0x00, readData | 0x01);
    do {
      readData = _read_reg(0x00);
    } while (readData & 0x01);

    readData = _read_reg(0x05);
    ESP_LOGI("LT768x", "Read 0x05: %02X", readData);
    _bus->endTransaction();

    startWrite(true);

    for (uint8_t i = 0; auto cmds = getInitCommands(i); i++)
    {
      command_list(cmds);
      if (i == 0) {
        uint16_t iters = 10000;
        while (iters > 0) {
          readData = _bus->readData(0xff);
          ESP_LOGI("LT768x", "Bus: %02x", readData);
          if ((readData & 0x04) == 0) break;
          iters--;
        }
      }
    }

    {
      //HDWR  // Horizontal Display Width Setting
      //_write_reg( 0x14, (_cfg.offset_x + _cfg.panel_width +7) >> 3);

      //uint_fast16_t height = _cfg.offset_y + _cfg.panel_height - 1;
      //VDHR0  // Vertical Display Height Setting
      //_write_reg( 0x19, height);
      //_write_reg( 0x1a, height >> 8);
    }

    endWrite();

    /// 初期化後にクロックをユーザー設定値に戻す;
    _bus->setClock(freq_write);

    _latestcolor = 0;

    uint16_t data16[6] = { (uint16_t)_cfg.offset_x
                         , (uint16_t)_cfg.offset_y
                         , (uint16_t)(_cfg.offset_x + _cfg.panel_width)
                         , (uint16_t)(_cfg.offset_y + _cfg.panel_height)
                         };
    auto data = (uint8_t*)data16;
    for (size_t idx = 0; idx < 8; ++idx)
    {
      //_write_reg(0x30 + idx, data[idx]);
    }

    for (size_t reg = 0x51; reg <= 0x65; ++reg)
    {
      //_write_reg(reg, 0);
    }

    return true;
  }

  void Panel_LT768x::beginTransaction(void)
  {
    begin_transaction();
  }
  void Panel_LT768x::begin_transaction(void)
  {
    if (_in_transaction) return;
    _in_transaction = true;
    _flg_memorywrite = false;
    _bus->beginTransaction();

    if (!_flg_serialbus) { cs_control(false); }
  }

  void Panel_LT768x::endTransaction(void)
  {
    end_transaction();
  }
  void Panel_LT768x::end_transaction(void)
  {
    if (!_in_transaction) return;
    _in_transaction = false;

    _bus->endTransaction();
    cs_control(true);
  }

  color_depth_t Panel_LT768x::setColorDepth(color_depth_t depth)
  {
    depth = ((int)depth & color_depth_t::bit_mask) >= 16 ? rgb565_2Byte : rgb332_1Byte;
    _write_depth = depth;
    _read_depth = depth;

    update_madctl();

    return depth;
  }
  void Panel_LT768x::setRotation(uint_fast8_t r)
  {
    r &= 7;
    _rotation = r;
    _internal_rotation = ((r + _cfg.offset_rotation) & 3) | ((r & 4) ^ (_cfg.offset_rotation & 4));

    _width  = _cfg.panel_width;
    _height = _cfg.panel_height;
    if (_internal_rotation & 1) { std::swap(_width, _height); }

    _colstart = _cfg.offset_x;
    _rowstart = _cfg.offset_y;

    _xs = _xe = _ys = _ye = INT16_MAX;

    update_madctl();
  }

  void Panel_LT768x::update_madctl(void)
  {
    return;
    //SYSR   bit[4:3]=00 256 color  bit[2:1]=  00 8bit MPU interface    1x 64k color  1x 16bit
    _write_reg(0x10, _write_depth == rgb565_2Byte ? 0x0C : 0x00);

    uint_fast8_t data = 0;
    switch (_internal_rotation & 3)
    {
    default:
    case 0: case 4:              break; // left to right 0x00
    case 1: case 5: data = 0x02; break; // top to bottom 0x08
    case 2: case 6: data = 0x01; break; // right to left 0x04
    case 3: case 7: data = 0x03; break; // bottom to top 0x0C
    }
    _write_reg(0x40, data << 2); // write direction
    _write_reg(0x45, data); // read direction

    _latestcolor = ~0u;

    return;
  }

  uint8_t Panel_LT768x::readStatus(void)
  {
    // p.140
    return _bus->readData(8) & 0xff;
  }

  void Panel_LT768x::waitDisplay(void)
  {
    _wait_busy();
  }

  bool Panel_LT768x::displayBusy(void)
  {
    if (_bus->busy()) return true;
    if (_cfg.pin_busy >= 0 && !lgfx::gpio_in(_cfg.pin_busy)) return true;
    return false;
  }

  bool Panel_LT768x::_wait_busy(uint32_t timeout)
  {
    _bus->wait();
    cs_control(true);
    int_fast16_t pin = _cfg.pin_busy;
    if (pin >= 0 && !lgfx::gpio_in(pin))
    {
      auto time = millis();
      do
      {
        if (millis() - time > timeout)
        {
          return false;
        }
      } while (!lgfx::gpio_in(pin));
    }
    cs_control(false);
    return true;
  }

  void Panel_LT768x::_write_reg(uint8_t reg, uint8_t data)
  {
    _bus->flush();
    uint_fast8_t len = 8;
    _wait_busy();
    _bus->writeCommand(reg, len);
    _bus->writeData(data, len);
  }

  uint8_t Panel_LT768x::_read_reg(uint8_t reg) {
    _bus->flush();
    _wait_busy();
    _bus->writeCommand(reg, 8);
    _bus->beginRead();
    uint8_t data = _bus->readData(1);
    _bus->endRead();
    return data;
  }

  void Panel_LT768x::_start_memorywrite(void)
  {
    if (_flg_memorywrite) { return; }
    _flg_memorywrite = true;
    if (_flg_serialbus)
    {
      _wait_busy();
      _bus->writeCommand((0x02 << 8) + 0x80, 24);
    }
    else
    {
      _bus->flush();
      uint32_t reg = 0x02;
      uint_fast8_t len = 8;
      if (_cfg.dlen_16bit)
      {
        len <<= 1;
        reg <<= 8;
      }
      _wait_busy();
      _bus->writeCommand(reg, len);
    }
  }

  void Panel_LT768x::writeCommand(uint32_t cmd, uint_fast8_t length)
  {
    if (_flg_serialbus)
    {
      cmd = (cmd << 8) + 0x80;
      _wait_busy();
      _bus->writeCommand(cmd, 16);
    }
    else
    {
      Panel_Device::writeCommand(cmd, length);
    }
  }
  void Panel_LT768x::writeData(uint32_t data, uint_fast8_t length)
  {
    if (_flg_serialbus)
    {
      data <<= 8;
      _wait_busy();
      _bus->writeData(data, 16);
    }
    else
    {
      Panel_Device::writeData(data, length);
    }
  }

  void Panel_LT768x::setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye)
  {
    _xs = xs;
    _xe = xe;
    _ys = ys;
    _ye = ye;
    _xpos = _ypos = INT16_MAX;
  }

  void Panel_LT768x::_set_write_pos(uint_fast16_t x, uint_fast16_t y)
  {
    auto flg_x = (_xpos != x);
    _xpos = x;
    auto flg_y = (_ypos != y);
    _ypos = y;
    if (flg_x || flg_y)
    {
      uint_fast8_t r = _internal_rotation;
      if (r)
      {
        if ((1u << r) & 0b10010110) { y = _height - (y + 1); }
        if (r & 2)                  { x = _width  - (x + 1); }
        if (r & 1) { std::swap(x, y); std::swap(flg_x, flg_y); }
      }
      if (flg_x)
      {
        x += _colstart;
        _write_reg(0x46, x);
        _write_reg(0x47, x >> 8);
      }
      if (flg_y)
      {
        y += _rowstart;
        _write_reg(0x48, y);
        _write_reg(0x49, y >> 8);
      }
    }
  }

  void Panel_LT768x::drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor)
  {
    bool tr = _in_transaction;
    if (!tr) begin_transaction();

    writeFillRectPreclipped(x, y, 1, 1, rawcolor);

    if (!tr) end_transaction();
  }

  void Panel_LT768x::writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor)
  {
    return;
    _xs = x;
    _xe = x + w - 1;
    _ys = y;
    _ye = y + h - 1;
    if (h == 1 && w <= 8)
    {
      _set_write_pos(x, y);
      writeBlock(rawcolor, w);
    }
    else
    {
      if (_latestcolor != rawcolor)
      {
        _latestcolor = rawcolor;
        if (_write_depth == rgb565_2Byte)
        {
          rawcolor = getSwap16(rawcolor);
          _write_reg(0x63, rawcolor >>11);
          _write_reg(0x64, rawcolor >> 5);
          _write_reg(0x65, rawcolor     );
        }
        else
        {
          _write_reg(0x63, rawcolor >> 5);
          _write_reg(0x64, rawcolor >> 2);
          _write_reg(0x65, rawcolor     );
        }
      }

      uint_fast8_t r = _internal_rotation;
      if (r)
      {
        if ((1u << r) & 0b10010110) { y = _height - (y + h); }
        if (r & 2)                  { x = _width  - (x + w); }
        if (r & 1) { std::swap(x, y);  std::swap(w, h); }
      }

      _write_reg(0x51, 0x0C); // Solid Fill.

      x += _colstart;
      y += _rowstart;
      uint16_t data16[6] = { (uint16_t)x
                           , (uint16_t)y
                           , (uint16_t)w
                           , (uint16_t)h
                           };
      auto data = (uint8_t*)data16;
      for (size_t idx = 0; idx < 8; ++idx)
      {
        _write_reg(0x58 + idx, data[idx]);
      }
      _write_reg(0x50, 0x80);
    }
  }

  void Panel_LT768x::writeBlock(uint32_t rawcolor, uint32_t length)
  {
    uint32_t xpos = _xpos;
    uint32_t ypos = _ypos;
    do
    {
      bool flg_setpos = false;
      if (xpos > _xe) { flg_setpos = true; xpos = _xs; ++ypos; }
      if (ypos > _ye) { flg_setpos = true; ypos = _ys;         }
      if (flg_setpos)
      {
        _set_write_pos(xpos, ypos);
      }

      _start_memorywrite();

      auto w = std::min<uint32_t>(length, _xe + 1 - xpos);
      xpos += w;
      _xpos = xpos;
      _bus->writeDataRepeat(rawcolor, _write_bits, w);
      length -= w;
    } while (length);
  }

  void Panel_LT768x::writePixels(pixelcopy_t* param, uint32_t length, bool use_dma)
  {
    uint32_t xpos = _xpos;
    uint32_t ypos = _ypos;
    do
    {
      bool flg_setpos = false;
      if (xpos > _xe) { flg_setpos = true; xpos = _xs; ++ypos; }
      if (ypos > _ye) { flg_setpos = true; ypos = _ys;         }
      if (flg_setpos)
      {
        _set_write_pos(xpos, ypos);
      }

      _start_memorywrite();

      auto w = std::min<uint32_t>(length, _xe + 1 - xpos);
      xpos += w;
      _xpos = xpos;

      if (param->no_convert)
      {
        _bus->writeBytes(reinterpret_cast<const uint8_t*>(param->src_data), w * _write_bits >> 3, true, use_dma);
      }
      else
      {
        _bus->writePixels(param, w);
      }

      length -= w;
    } while (length);
  }

  void Panel_LT768x::write_bytes(const uint8_t* data, uint32_t length, bool use_dma)
  {
    uint32_t xpos = _xpos;
    uint32_t ypos = _ypos;
    do
    {
      bool flg_setpos = false;
      if (xpos > _xe) { flg_setpos = true; xpos = _xs; ++ypos; }
      if (ypos > _ye) { flg_setpos = true; ypos = _ys;         }
      if (flg_setpos)
      {
        _set_write_pos(xpos, ypos);
      }

      _start_memorywrite();

      auto w = std::min<uint32_t>(length >> (_write_bits >> 4), _xe + 1 - xpos);
      xpos += w;
      _xpos = xpos;
      w <<= (_write_bits >> 4);
      _bus->writeBytes(data, w, true, use_dma);
      data += w;
      length -= w;
    } while (length);
  }

  void Panel_LT768x::writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool use_dma)
  {
    auto bytes = param->dst_bits >> 3;
    auto src_x = param->src_x;

    if (param->transp == pixelcopy_t::NON_TRANSP)
    {
      setWindow(x, y, x + w - 1, y + h - 1);
      if (param->no_convert)
      {
        auto wb = w * bytes;
        uint32_t i = (src_x + param->src_y * param->src_bitwidth) * bytes;
        auto src = &((const uint8_t*)param->src_data)[i];
        auto add = param->src_bitwidth * bytes;
        do
        {
          write_bytes(src, wb, use_dma);
          src += add;
        } while (--h);
      }
      else
      {
        do
        {
          writePixels(param, w, use_dma);
          param->src_x = src_x;
          param->src_y++;
        } while (--h);
      }
    }
    else
    {
      h += y;
      uint32_t wb = w * bytes;
      do
      {
        uint32_t i = 0;
        while (w != (i = param->fp_skip(i, w, param)))
        {
          auto buf = _bus->getDMABuffer(wb);
          int32_t len = param->fp_copy(buf, 0, w - i, param);
          setWindow(x + i, y, x + i + len - 1, y);
          write_bytes(buf, len * bytes, true);
          if (w == (i += len)) break;
        }
        param->src_x = src_x;
        param->src_y++;
      } while (++y != h);
    }
  }

  void Panel_LT768x::copyRect(uint_fast16_t dst_x, uint_fast16_t dst_y, uint_fast16_t w, uint_fast16_t h, uint_fast16_t src_x, uint_fast16_t src_y)
  {
    uint_fast8_t r = _internal_rotation;
    if (r)
    {
      if ((1u << r) & 0b10010110) { src_y = _height - (src_y + h); dst_y = _height - (dst_y + h); }
      if (r & 2)                  { src_x = _width  - (src_x + w); dst_x = _width  - (dst_x + w); }
      if (r & 1) { std::swap(src_x, src_y); std::swap(dst_x, dst_y); std::swap(w, h); }
    }

    bool positive = (dst_y < src_y || (dst_y == src_y && dst_x < src_x));

    _write_reg( 0x51, positive ? 0xC2 : 0xC3 );
    if (!positive)
    {
      src_x += w - 1;
      dst_x += w - 1;
      src_y += h - 1;
      dst_y += h - 1;
    }
    src_x += _colstart;
    dst_x += _colstart;
    src_y += _rowstart;
    dst_y += _rowstart;

    uint16_t data16[6] = { (uint16_t)src_x
                         , (uint16_t)src_y
                         , (uint16_t)dst_x
                         , (uint16_t)dst_y
                         , (uint16_t)w
                         , (uint16_t)h
                         };
    auto data = (uint8_t*)data16;
    for (size_t idx = 0; idx < 12; ++idx)
    {
      _write_reg(0x54 + idx, data[idx]);
    }
    _write_reg(0x50, 0x80);
  }

  void Panel_LT768x::readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param)
  {
    uint_fast16_t bytes = param->dst_bits >> 3;
    auto len = w * h;
    if (!_cfg.readable)
    {
      memset(dst, 0, len * bytes);
      return;
    }

    auto dst8 = (uint8_t*)dst;

    int xadd = 0;
    int yadd = 1;
    uint_fast8_t r = _internal_rotation;
    if (r)
    {
      if ((1u << r) & 0b10010110) { y = _height - (y + 1); yadd = -1; }
      if (r & 2)                  { x = _width  - (x + 1); }
      if (r & 1) { std::swap(x, y); std::swap(xadd, yadd); } // ここでは wとhは入れ替えない;
    }

    x += _colstart;
    y += _rowstart;

    startWrite();
    do
    {
      _write_reg(0x4A, x);
      _write_reg(0x4B, x >> 8);
      _write_reg(0x4C, y);
      _write_reg(0x4D, y >> 8);

      if (_flg_serialbus)
      {
        _wait_busy();
        _bus->writeCommand(0x80 + (0x02 << 8) + (0x40 << 16), 24);
      }
      else
      {
        _bus->flush();
        uint32_t reg = 0x02;
        uint_fast8_t len_ = 8;
        if (_cfg.dlen_16bit)
        {
          len_ <<= 1;
          reg <<= 8;
        }
        _wait_busy();
        _bus->writeCommand(reg, len_);
      }

      _bus->beginRead(_cfg.dummy_read_pixel);
      if (param->no_convert)
      {
        _bus->readBytes(dst8, w * bytes);
      }
      else
      {
        _bus->readPixels(dst8, param, w);
      }
      dst8 += w * bytes;
      _bus->endRead();
      x += xadd;
      y += yadd;
    } while (--h);

    endWrite();

    if (_in_transaction) { cs_control(false); }
  }

//----------------------------------------------------------------------------
 }
}
