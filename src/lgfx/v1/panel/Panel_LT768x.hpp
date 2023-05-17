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

#include "Panel_Device.hpp"

/*==== [HW_(3)]  SDRAM  =====*/
//#define IS42SM16160D
//#define IS42S16320B
//#define IS42S16400F 
//#define M12L32162A 
//#define M12L2561616A 
//#define W9825G6JH
//#define W9812G6JH
#define W9812G6KH
//#define MT48LC4M16A
//#define K4S641632N
//#define K4S281632K

/*==== [SW_(1)]  PLL  =====*/
#define OSC_FREQ     10   // crystal clcok
#define DRAM_FREQ    100  // SDRAM clock frequency, unti: MHz       
#define CORE_FREQ    100  // Core (system) clock frequency, unit: MHz 
#define SCAN_FREQ     50 // Panel Scan clock frequency, unit: MHz  

//=====[05h] b3-b1=====//
//#define SCAN_DIVK1      // 000    
#define SCAN_DIVK2     // 001   
//#define SCAN_DIVK4      // 010
//#define SCAN_DIVK8      // 011 
//#define SCAN_DIVK16     // 100
//#define SCAN_DIVK32     // 101
//#define SCAN_DIVK64     // 110
//#define SCAN_DIVK128    // 111
//=====[05h] b0
#define SCAN_DIVM1        // 0
//#define SCAN_DIVM2      // 1 

//=====[07h] b3-b1
//#define DRAM_DIVK1      // 000 
#define DRAM_DIVK2        // 001
//#define DRAM_DIVK4      // 010
//#define DRAM_DIVK8      // 011
//=====[07h] b0
//#define DRAM_DIVM1      // 0
//#define DRAM_DIVM2   // 1

//=====[09h] b3-b1
//#define CORE_DIVK1      // 000
#define CORE_DIVK2     // 001
//#define CORE_DIVK4      // 010
//#define CORE_DIVK8      // 011  
//=====[09h] b0           
#define CORE_DIVM1        // 0
//#define CORE_DIVM2   // 1

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct Panel_LT768x : public Panel_Device
  {
    Panel_LT768x(void)
    {
      _cfg.memory_width  = _cfg.panel_width  = 800;
      _cfg.memory_height = _cfg.panel_height = 480;
      _cfg.dummy_read_pixel = 16;
      _cfg.dummy_read_bits  =  0;
    }

    bool init(bool use_reset) override;
    void beginTransaction(void) override;
    void endTransaction(void) override;

    color_depth_t setColorDepth(color_depth_t depth) override;
    void setRotation(uint_fast8_t r) override;

    void writeCommand(uint32_t, uint_fast8_t) override;
    void writeData(uint32_t, uint_fast8_t) override;

    void waitDisplay(void) override;
    bool displayBusy(void) override;

    void writePixels(pixelcopy_t* param, uint32_t len, bool use_dma) override;
    void writeBlock(uint32_t rawcolor, uint32_t len) override;

    void setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye) override;
    void drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor) override;
    void writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor) override;
    void writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool use_dma) override;

    void readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param) override;
    void copyRect(uint_fast16_t dst_x, uint_fast16_t dst_y, uint_fast16_t w, uint_fast16_t h, uint_fast16_t src_x, uint_fast16_t src_y) override;


    void setInvert(bool invert) override {}; // Not yet implemented.
    void setSleep(bool flg) override {}; // Not yet implemented.
    void setPowerSave(bool flg) override {}; // Not yet implemented.
    uint32_t readCommand(uint_fast16_t cmd, uint_fast8_t index, uint_fast8_t len) override { return 0; }; // Not yet implemented.
    uint32_t readData(uint_fast8_t index, uint_fast8_t len)  override { return 0; }; // Not yet implemented.

  protected:

    uint32_t _latestcolor = 0;
    uint16_t _colstart = 0;
    uint16_t _rowstart = 0;
    uint16_t _xpos = UINT16_MAX;
    uint16_t _ypos = UINT16_MAX;
    bool _in_transaction = false;
    bool _flg_serialbus = false;
    bool _flg_memorywrite = false;
    uint8_t _reg_0x51[16] = {0};

    const uint8_t* getInitCommands(uint8_t listno) const override;
    void begin_transaction(void);
    void end_transaction(void);
    void write_bytes(const uint8_t* data, uint32_t len, bool use_dma);
    void update_madctl(void);

    uint8_t readStatus(void);

    void _set_write_pos(uint_fast16_t x, uint_fast16_t y);
    bool _wait_busy( uint32_t timeout = 1000);
    void _write_reg(uint8_t reg, uint8_t data);
    uint8_t _read_reg(uint8_t reg);
    void _start_memorywrite(void);
  };

//----------------------------------------------------------------------------
 }
}
