#ifndef _LMCP_H_
#define _LMCP_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define UNUSED(X) ((void)X)

class Lmcp
{
public:
    typedef struct __attribute__((packed, aligned(sizeof(uint32_t))))
    {
        uint32_t magic;
        uint32_t version;
        uint32_t checksum;
        uint32_t length;
        uint32_t command;
    } header_t;

    Lmcp(uint32_t, uint32_t, uint8_t);
    /* build a header from the arguments. */
    header_t build_header(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
    /* reads in a header corresponding to what is found in the data stream */
    header_t read_header(uint8_t *);
    /* returns a pointer to position in data stream. */
    uint8_t *find_header(uint8_t *, uint32_t);
    /* pass data to this function for processing.*/
    bool process(uint8_t *, uint16_t);
    /* override to implement writing framebuffer out to the matrix. */
    virtual void write_buffer();

private:

    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t bitdepth;
    uint8_t color[3];

    // header magic
    const uint8_t magic_bytes[4] = {'L', 'M', 'C', 'P'};
    // holds the magic number.
    uint32_t magic;
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