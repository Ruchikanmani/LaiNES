#include <cstdio>
#include "apu.hpp"
#include "cpu.hpp"
#include "mappers/mapper0.hpp"
#include "mappers/mapper1.hpp"
#include "mappers/mapper2.hpp"
#include "mappers/mapper3.hpp"
#include "mappers/mapper4.hpp"
#include "ppu.hpp"
#include "cartridge.hpp"

namespace Cartridge {


Mapper* mapper = nullptr;  // Mapper chip.

/* PRG-ROM access */
template <bool wr> u8 access(u16 addr, u8 v)
{
    if (!wr) return mapper->read(addr);
    else     return mapper->write(addr, v);
}
template u8 access<0>(u16, u8); template u8 access<1>(u16, u8);

/* CHR-ROM/RAM access */
template <bool wr> u8 chr_access(u16 addr, u8 v)
{
    if (!mapper) return 0;  // Safety: return 0 if no cartridge loaded
    if (!wr) return mapper->chr_read(addr);
    else     return mapper->chr_write(addr, v);
}
template u8 chr_access<0>(u16, u8); template u8 chr_access<1>(u16, u8);

void signal_scanline(int scanline)
{
    (void)scanline;  // Scanline parameter will be used in mapper PRs
    mapper->signal_scanline();
}

/* Load the ROM from a file. */
void load(const char* fileName)
{
    FILE* f = fopen(fileName, "rb");

    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);

    u8* rom = new u8[size];
    fread(rom, size, 1, f);
    fclose(f);

    int mapperNum = (rom[7] & 0xF0) | (rom[6] >> 4);
    if (loaded())
    {
        delete mapper;
        mapper = nullptr;
    }
    switch (mapperNum)
    {
        case 0:  mapper = new Mapper0(rom); break;
        case 1:  mapper = new Mapper1(rom); break;
        case 2:  mapper = new Mapper2(rom); break;
        case 3:  mapper = new Mapper3(rom); break;
        case 4:  mapper = new Mapper4(rom); break;
        default:
            fprintf(stderr, "%s: mapper %d not supported\n", fileName, mapperNum);
            return;
    }

    PPU::reset();  // Reset PPU first so it's ready when CPU::power() ticks
    APU::reset();
    CPU::power();  // power() calls INT<RESET>() which ticks the PPU
}

bool loaded()
{
    return mapper != nullptr;
}

// Stub: Mapper IRQ checking - will be implemented in mapper PRs
bool check_mapper_irq(int elapsed)
{
    (void)elapsed;
    return false;  // No mapper IRQ support yet
}

// Stub: Expansion address handling - will be implemented in mapper PRs
bool handles_expansion_addr(u16 addr)
{
    (void)addr;
    return false;  // No expansion address support yet
}

// Stub: Mapper audio - will be implemented in mapper PRs
void run_mapper_audio(int elapsed)
{
    (void)elapsed;
    // No mapper audio yet
}

// Stub: Mapper audio frame end - will be implemented in mapper PRs
void end_mapper_audio_frame(int elapsed)
{
    (void)elapsed;
    // No mapper audio yet
}

// Stub: PPU write hook - will be implemented in mapper PRs
void ppu_write_hook(u16 addr, u8 v)
{
    (void)addr;
    (void)v;
    // No PPU write observation yet
}


}
