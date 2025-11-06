#include "gui.hpp"
#include "cpu.hpp"
#include "apu.hpp"

namespace APU {


Nes_Apu apu;
Blip_Buffer buf;

const int OUT_SIZE = 4096;
blip_sample_t outBuf[OUT_SIZE];

void init()
{
    buf.sample_rate(96000);
    buf.clock_rate(1789773);

    apu.output(&buf);
    apu.dmc_reader(CPU::dmc_read);
}

void reset()
{
    apu.reset();
    buf.clear();
}

template <bool write> u8 access(int elapsed, u16 addr, u8 v, bool is_put_cycle)
{
    // is_put_cycle parameter is a stub for now - will be used in APU accuracy PR
    (void)is_put_cycle;  // Suppress unused parameter warning

    if (write)
        apu.write_register(elapsed, addr, v);
    else if (addr == apu.status_addr)
        v = apu.read_status(elapsed);

    return v;
}
template u8 access<0>(int, u16, u8, bool); template u8 access<1>(int, u16, u8, bool);

// Stub: APU IRQ checking - will be implemented in APU accuracy PR
bool check_irq(int elapsed)
{
    (void)elapsed;
    return false;  // No IRQ support yet
}

// Stub: Buffer management - will be implemented in APU accuracy PR
void end_buffer_frame(int elapsed)
{
    (void)elapsed;
    buf.end_frame(elapsed);
}

void run_frame(int elapsed)
{
    apu.end_frame(elapsed);
    end_buffer_frame(elapsed);

    if (buf.samples_avail() >= OUT_SIZE)
        GUI::new_samples(outBuf, buf.read_samples(outBuf, OUT_SIZE));
}


}
