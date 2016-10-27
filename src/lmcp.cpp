#include "lmcp.h"

Lmcp::Lmcp(uint32_t width, uint32_t height, uint8_t bitdepth)
{
    this->field_width = width;
    this->field_height = height;
    this->bitdepth = bitdepth;

    // magic as a 32bit-word
    memcpy(&this->magic, magic_bytes, sizeof(uint32_t));

    // initialize to white for default.
    for(int i = 0; i < 3; i++)
    {
        this->color[i] = 0xFF;
    }
}

Lmcp::header_t Lmcp::build_header(uint32_t magic,
                                  uint32_t version,
                                  uint32_t checksum,
                                  uint32_t length,
                                  uint32_t command)
{
    Lmcp::header_t h = {magic, version, checksum, length, command};
    return h;
}

Lmcp::header_t Lmcp::read_header(uint8_t *data)
{
    // this can totally go wrong, we are just coping like
    // from the end of data and we could go over the boundry.
    Lmcp::header_t h;
    memcpy(&h, data, sizeof(Lmcp::header_t));
    return h;
}

uint8_t *Lmcp::find_header(uint8_t *data, uint32_t length)
{
    if(data == NULL)
    {
        return data;
    }

    for(uint32_t i = 0; i < length; i++)
    {
        uint32_t read_seq;
        memcpy(&read_seq, (data + i), sizeof(uint32_t));
        if(read_seq == this->magic)
        {
            return (data + i);
        }
    }

    return NULL;
}

bool Lmcp::process(uint8_t *data, uint16_t length)
{
    UNUSED(data);
    UNUSED(length);
    // printf("\n");
    // static Lmcp::header_t *header = NULL;
    // uint32_t *idata = (uint32_t *)data;

    // for(uint32_t i = 0; i < length / sizeof(uint32_t); i++)
    // {
    //     if(idata[i] == this->magic)
    //     {
    //         printf("[%s:%d] found packet header at: [%lu]\n",
    //                __FILE__,
    //                __LINE__,
    //                i * sizeof(uint32_t));
    //         header = (Lmcp::header_t *)(data + (i * sizeof(uint32_t)));
    //         printf("\theader contains: \n"
    //                "\theader->magic   : 0x%08X\n"
    //                "\theader->version : 0x%08X\n"
    //                "\theader->checksum: 0x%08X\n"
    //                "\theader->length  : 0x%08X\n"
    //                "\theader->command : 0x%08X\n",
    //                header->magic,
    //                header->version,
    //                header->checksum,
    //                header->length,
    //                header->command);
    //     }
    // }
    // printf("\n");
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
