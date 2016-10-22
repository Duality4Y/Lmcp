#ifndef _LMCP_H_
#define _LMCP_H_

#include <stdint.h>
#include <stdio.h>

#define UNUSED(X) ((void)X)

const uint32_t TEXT_CHAR_WIDTH = 5;
const uint32_t TEXT_CHAR_HEIGHT = 7;

enum 
{
    MAGICK,
    VERSION = 4,
    CSUM_H,
    CSUM_L,
    COMMAND_H,
    COMMAND_L,
    LENGTH_H,
    LENGTH_L,
    DATA,
};

class Lmcp
{
public:
    Lmcp(uint32_t, uint32_t, uint8_t);
    /* data passed to this function for processing.*/
    bool process_packet(uint8_t *, uint16_t);
    /* override to implement writing framebuffer out to the matrix. */
    virtual void write_buffer();

private:

    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t bitdepth;
    uint8_t color[3];

    // header magic
    const uint8_t magic_bytes[4] = {'L', 'M', 'C', 'P'};
    // magic as a 32bit-word
    uint32_t magic = ((uint32_t *)magic_bytes)[0];

    /* clears the framebuffer. */
    void clear();
    /* set a pixel in framebuffer. */
    void set_pixel(uint32_t, uint32_t, uint8_t, uint8_t, uint8_t);
    /* draw a row of data in the framebuffer. */
    uint32_t draw_row(uint8_t *);
    /* draw a image rect in the framebuffer. */
    uint32_t draw_image(uint8_t *);
    /* draw text line based. */
    uint32_t draw_text(uint8_t *);
    /* draw text absolute. */
    uint32_t draw_text_abs(uint8_t *);
    /* set a drawing color, for functions that don't have or use RGB data.*/
    uint32_t set_color(uint8_t *);

    /* calculate checksum*/
    uint32_t csum(uint8_t *, uint32_t);
};

#endif