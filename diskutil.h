#ifndef _DISKUTIL_H
#define _DISKUTIL_H

#include "disk.h"

sector_t *_buf_to_sector( void *buf, int len );

void *_sector_to_buf( sector_t *sectors, int num );

#endif
