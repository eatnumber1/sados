#define __KERNEL__20093__

#include "headers.h"
#include "disk.h"

#include "klib.h"
#include "assert.h"

#define MAX_SECTORS 512

static sector_t sector_vec[MAX_SECTORS];
static char buf_vec[MAX_SECTORS * SECTOR_SIZE];

sector_t *_buf_to_sector( void *src, int len ) {
	int num_sectors = len / SECTOR_SIZE;
	if( len % SECTOR_SIZE != 0 ) num_sectors++;
	assert(num_sectors <= MAX_SECTORS, "_buf_to_sector: Buffer too long");

	void *src_ptr = src;
	for( int i = 0; i < num_sectors; i++ ) {
		if( i == num_sectors - 1 ) {
			_memclr(sector_vec[i], SECTOR_SIZE);
			_memcpy(sector_vec[i], src_ptr, len);
		} else {
			_memcpy(sector_vec[i], src_ptr, SECTOR_SIZE);
			len -= SECTOR_SIZE;
		}
		src_ptr += SECTOR_SIZE;
	}
	return sector_vec;
}

void *_sector_to_buf( sector_t *sectors, int num ) {
	assert(num <= MAX_SECTORS, "_sector_to_buf: Too many sectors");

	char *buf_ptr = buf_vec;
	for( int i = 0; i < num; i++ ) {
		_memcpy(buf_ptr, sectors[i], SECTOR_SIZE);
		buf_ptr += SECTOR_SIZE;
	}
	return buf_vec;
}
