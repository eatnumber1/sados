#include "headers.h"
#include "ide.h"
#include "klib.h"

#define BEGIN_SEARCH    0xE0000
#define END_SEARCH      0xFFFFF


/*
struct pciheader {
    uint_16 vendor_id;  // 0
    uint_16 device_id;  // 2
    uint_16 command;    // 4
    uint_16 status;     // 6
    uint_8 blank1;      // 8
    uint_24 class_code; // 9

    uint_32 blank2;
    //something bar;
}
*/

typedef struct bios {
    char ascii_32_[4];
    void *entry_point;
    char revision;
    char length;
    char checksum;
    char reserved[5];
} bios_t;

typedef union {
    void *v;
    struct char_array {
        char c1;
        char c2;
        char c3;
        char c4;
    } chararray;
} int_chararray;

#define __KERNEL_CS (12 * 8)

static struct {
    unsigned long address;
    unsigned short segment;
} bios32_indirect = { 0, __KERNEL_CS };

void findbios() {

    /* Look for the BIOS32 Service Directory */
    char *i;
    bios_t *b = 0;
    for (i = (char *)BEGIN_SEARCH; b == 0 && i < (char *)END_SEARCH; i += 16) {
        if (i[0] == '_' && i[1] == '3' && i[2] == '2' && i[3] == '_') {
            b = (bios_t *)i;
            char sum = b->ascii_32_[0] + b->ascii_32_[1] + b->ascii_32_[2] +
                       b->ascii_32_[3] + b->revision + b->length + b->checksum +
                       b->reserved[0] + b->reserved[1] + b->reserved[2] +
                       b->reserved[3] + b->reserved[4];
            int_chararray entry = (int_chararray) b->entry_point;
            sum += entry.chararray.c1 + entry.chararray.c2 +
                   entry.chararray.c3 + entry.chararray.c4;
            if ((int)sum != 0) {
                b = 0;
            } else {
                c_printf ("Yayy! %d: %c%c%c%c\n", &i, i[0], i[1], i[2], i[3]);
            }
        }
    }

    if (b == 0) {
        //BIOS32 Service Directory not found
    }

    unsigned char return_code;
    unsigned long address;
    unsigned long length;
    unsigned long entry;    
    unsigned long flags;

//    __asm__("pushal; call *(%%edi); cld; popal"
    //__asm__("push %%cs; call *(%%edx); cld"
    //        : "=a" (return_code),
    //          "=b" (address),
    //          "=c" (length),
    //          "=d" (entry)
    //        : "0"  (0x49435024), //(b->entry_point),
    //          "1"  (0),
	//      "3"  (b->entry_point), 
         //     "D"  (&bios32_indirect));

    _kpanic("got to before callbios");

    _callbios(b->entry_point);

    c_printf ("Returned from callbios\n");

    _kpanic("returned");


    switch (return_code) {
        case 0:
            //address + entry
            break;
        case 0x80:
            _kpanic("bios32_entry not present");
        default:
            _kpanic("bios32_entry");
    }

    //pcb_t *curr = _current;
    //context_t *cont = curr->context;

    //int returncode = (int)(*((char *)cont->eax) + 3);
    //void (*biosbase)(void) = (void(*)())cont->ebx;
    //int bioslen = cont->ecx;
    //int offset = cont->edx;

    c_printf ("return code: %d\n", return_code);
    c_printf ("bios base: %d\n", &address);
    c_printf ("bios length: %d\n", length);
    c_printf ("offset: %d\n", entry);

    c_printf("Done findbios\n");
    _kpanic("findbios");
}

/*
error_t _disk_read( disk_t diskno, diskaddr_t addr, disk_size_t count, sector_t *dest ) {
    return -1;
}

error_t _disk_write( disk_t diskno, diskaddr_t addr, disk_size_t count, sector_t *src ) {
    return -1;
}

char *_disk_strerror( error_t code ) {
    return "not yet implemented";
}
*/
