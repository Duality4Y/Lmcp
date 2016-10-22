// #include <stdio.h>
// #include <stdint.h>

// namespace Lmcp
// {
//     typedef struct
//     {
//         uint32_t magic;
//         uint32_t version;
//         uint32_t checksum;
//         uint32_t command;
//         uint32_t length;
//     } header_t;

//     uint8_t magic[4] = {'L', 'M', 'C', 'P'};
//     uint32_t header_start = ((uint32_t *)magic)[0];
// }

// int main(void)
// {

//     printf("magic is: 0x%08X\n", Lmcp::header_start);
//     uint8_t data[200] = {'L','M','C','P',
//                          0x01, 0x00, 0x00, 0x00,
//                          0x01, 0x02, 0x03, 0x04,
//                          0x32, 0x42, 0xFA, 0xBA,
//                          0x00, 0x00, 0x42, 0x00,};
    
//     uint32_t *idata = (uint32_t *)data;
//     for(int i = 0; i < 5; i++)
//     {
//         printf("word is: 0x%08X\n", idata[i]);
//     }
    
//     return 0;
// }
