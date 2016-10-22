#include "lmcp.h"

Lmcp::Lmcp(uint32_t width, uint32_t height, uint8_t bitdepth)
{
    this->framebuffer_width = width;
    this->framebuffer_height = height;
    this->bitdepth = bitdepth;
    for(int i = 0; i < 3; i++)
    {
        this->color[i] = 0xFF;
    }
}

bool Lmcp::process_packet(uint8_t *data, uint16_t length)
{
    uint32_t *idata = (uint32_t *)data;
    for(uint32_t i = 0; i < length / sizeof(uint32_t); i++)
    {
        if(idata[i] == this->magic)
        {
            printf("[%s:%d]found packet header at: [%lu]\n",
                   __FILE__,
                   __LINE__,
                   i * sizeof(uint32_t));
        }
    }
    return false;
}

void Lmcp::write_buffer(){}
void Lmcp::clear(){}

void Lmcp::set_pixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
    UNUSED(x);
    UNUSED(y);
    UNUSED(r);
    UNUSED(g);
    UNUSED(b);
}

uint32_t Lmcp::draw_row(uint8_t *data)
{
    UNUSED(data);
    return 0;
}

uint32_t Lmcp::draw_image(uint8_t *data)
{
    UNUSED(data);
    return 0;
}

uint32_t Lmcp::draw_text(uint8_t *data)
{
    UNUSED(data);
    return 0;
}

uint32_t Lmcp::draw_text_abs(uint8_t *data)
{
    UNUSED(data);
    return 0;
}

uint32_t Lmcp::set_color(uint8_t *data)
{
    UNUSED(data);
    return 0;
}
