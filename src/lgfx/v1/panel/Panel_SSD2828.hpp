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
#pragma once

#include "Panel_LCD.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  class Panel_SSD2828 : public Panel_LCD
  {
  public:
    Panel_SSD2828(void) : Panel_LCD()
    {
      _cfg.memory_width  = _cfg.panel_width  = 1;
      _cfg.memory_height = _cfg.panel_height = 1;
    }

    void setInvert(bool invert) override;
    void setSleep(bool flg) override;
    void setPowerSave(bool flg) override;

    void setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye) override;
    void drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor) override;
    void writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor) override;

  protected:

    static constexpr uint8_t CMD_VICR1 = 0xB1;
    static constexpr uint8_t CMD_VICR2 = 0xB2;
    static constexpr uint8_t CMD_VICR3 = 0xB3;
    static constexpr uint8_t CMD_VICR4 = 0xB4;
    static constexpr uint8_t CMD_VICR5 = 0xB5;
    static constexpr uint8_t CMD_VICR6 = 0xB6;
    static constexpr uint8_t CMD_CFGR  = 0xB7;
    static constexpr uint8_t CMD_VCR   = 0xB8;
    static constexpr uint8_t CMD_PCR   = 0xB9;
    static constexpr uint8_t CMD_PLCR  = 0xBA;
    static constexpr uint8_t CMD_CCR   = 0xBB;
    static constexpr uint8_t CMD_DAR1  = 0xC9;
    static constexpr uint8_t CMD_DAR2  = 0xCA;
    static constexpr uint8_t CMD_DAR3  = 0xCB;
    static constexpr uint8_t CMD_DAR4  = 0xCC;
    static constexpr uint8_t CMD_HTTR2 = 0xD0;
    static constexpr uint8_t CMD_LRR   = 0xD4; // SPI Read Register, 0x00FA
    static constexpr uint8_t CMD_TR    = 0xD6;
    static constexpr uint8_t CMD_ACR4  = 0xDB;
    static constexpr uint8_t CMD_LCFR  = 0xDE;

    const uint8_t* getInitCommands(uint8_t listno) const override
    {
      static constexpr uint8_t list0[] =
      {
          CMD_CFGR , 2, 0x50, 0x00,  // 50=TX_CLK, 70=PCLK
          CMD_VCR  , 2, 0x00, 0x00,
          CMD_PCR  , 2, 0x00, 0x00,  // 1=PLL disable
          CMD_PLCR , 2, 0x14, 0x42,  // PLL=(TX_CLK/MS)*NS
                                     // 14,D7-0=NS(0x01 : NS=1)
                                     // 42,D15-14=PLL00=62.5-125 01=126=250 10=251-500 11=501-1000  DB12-8=MS(01:MS=1)
          CMD_CCR  , 2, 0x03, 0x00,  // LP Clock Divider LP clock = 400MHz / LPD / 8=480 / 8/ 8 = 7MHz
                                     // D5-0=LPD=0x01 "C Divide by 2
          CMD_PCR  , 2, 0x01, 0x00,  // 1=PLL disable
          CMD_LCFR , 2, 0x00, 0x00,  // MIPI lane configuration
                                     // 11=4lane 10=3lane 01=2lane 00=1lane
          CMD_DAR1 , 2, 0x02, 0x23,  // p1: HS-Data-zero, p2: HD_Data-prepare
          CMD_CFGR , 2, 0x50, 0x00,
          CMD_VCR  , 2, 0x00, 0x00,
          CMD_PCR  , 2, 0x00, 0x00,
          CMD_PLCR , 2, 0x2d, 0x82,
          CMD_CCR  , 2, 0x07, 0x00,  // LP Clock Divider
          CMD_PCR  , 2, 0x01, 0x00,
          CMD_DAR1 , 2+CMD_INIT_DELAY, 0x02, 0x23, 100,
          CMD_DAR2 , 2, 0x01, 0x23, // CLK Prepare, CLK Zero
          CMD_DAR3 , 2, 0x10, 0x05, // 0x0510, Clk Post, Clk Per
          CMD_DAR4 , 2, 0x05, 0x10, // HS Trail, Clk Trail
          CMD_HTTR2, 2, 0x00, 0x00,
          CMD_VICR1, 2, 24,  2,     // LCD_HSPW=24 LCD_VSPW=2
          CMD_VICR2, 2, 160, 10,    // LCD_HBPD=160 LCD_VBPD=10
          CMD_VICR3, 2, 160, 12,    // LCD_HFPD=160 LCD_VFPD=12
          CMD_VICR4, 2, 0x90, 0x01, // Horizontal active period 400, 013f=319 02d0=72
          CMD_VICR5, 2, 0x00, 0x05, // Vertical active period 1280
          CMD_VICR6, 2, 0x0b, 0xc0, // RGB CLK 16BPP=00 18BPP=01
                                    // D7=0 D6=0 D5=0, D1-0=11 "C 24bpp //07
                                    // D15=VS D14=HS D13=CLK D12-9=NC D8-0=Video with blanking packet. 00-F0
          CMD_LCFR , 2, 0x03, 0x00, // MIPI lane configuration
          CMD_TR   , 2, 0x01, 0x00, // 05=BGR 04=RGB
                                    // D0=0=RGB 1:BGR D1=1=Most significant byte sent first
          CMD_ACR4 , 2, 0x58, 0x00,
          CMD_CFGR , 2, 0x4b, 0x02,
          0x2c     , 0,

          0xFF,0xFF, // end
      };
      switch (listno)
      {
      case 0: return list0;
      default: return nullptr;
      }
    }

  };

//----------------------------------------------------------------------------
 }
}
