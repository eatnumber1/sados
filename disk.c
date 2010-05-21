#define __KERNEL__20093__

#include "headers.h"
#include "disk.h"

#include "klib.h"
#include "kalloc.h"
#include "assert.h"
#include "string.h"

sector_t *_buf_to_sector( void *src, int len ) {
	int num_sectors = len / SECTOR_SIZE;
	if( len % SECTOR_SIZE != 0 ) num_sectors++;

	void *src_ptr = src;
	sector_t *sector_vec = _kalloc(SECTOR_SIZE * num_sectors);
	_memclr(sector_vec, SECTOR_SIZE * num_sectors);
	for( int i = 0; i < num_sectors; i++ ) {
		_memcpy(sector_vec[i], src_ptr, len);
		len -= SECTOR_SIZE;
		src_ptr += SECTOR_SIZE;
	}
	return sector_vec;
}

void *_sector_to_buf( sector_t *sectors, int num ) {
	char *buf_vec = _kalloc(num * SECTOR_SIZE);
	char *buf_ptr = buf_vec;
	for( int i = 0; i < num; i++ ) {
		_memcpy(buf_ptr, sectors[i], SECTOR_SIZE);
		buf_ptr += SECTOR_SIZE;
	}
	return buf_vec;
}

sector_t *_int_to_sector( int num ) {
	char buf[sizeof(int)];
	_memcpy(buf, &num, sizeof(int));
	return _buf_to_sector(buf, sizeof(int));
}

int _sector_to_int( sector_t *sectors ) {
	char *buf = _sector_to_buf(sectors, sizeof(int));
	int num;
	_memcpy(&num, buf, sizeof(int));
	_kfree(buf);
	return num;
}

sector_t *_string_to_sector( char *str ) {
	return _buf_to_sector(str, strlen(str) + 1);
}

sector_t *_diskaddr_to_sector( diskaddr_t addr ) {
	return _int_to_sector(addr);
}

diskaddr_t _sector_to_diskaddr( sector_t *sectors ) {
	return _sector_to_int(sectors);
}

sector_t *_disk_size_to_sector( disk_size_t addr ) {
	return _int_to_sector(addr);
}

disk_size_t _sector_to_disk_size( sector_t *sectors ) {
	return _sector_to_int(sectors);
}

disk_size_t _sectors_in( disk_size_t size ) {
	return ((size / SECTOR_SIZE) + ((size % SECTOR_SIZE != 0) ? 1 : 0));
}
