#define __KERNEL__20093__

#include "headers.h"

#include "disk.h"

#define BLOCKSIZ 512
#define DISK_SIZE 512
#define MAX_ADDR BLOCKSIZ * DISK_SIZE - 1

// I wish I could dynamically allocate this.
static char disk[DISK_SIZE * BLOCKSIZ];

typedef enum {
	NO_ERROR,
	BAD_DISK,
	BAD_ADDRESS
} _disk_error;

inline blocksiz_t _blocksiz( disk_t diskno ) {
	return BLOCKSIZ;
}

static error_t check_sane( disk_t diskno, diskaddr_t addr, disksiz_t count ) {
	if( diskno != 0 ) return BAD_DISK;
	if( count < 0 || addr + count > MAX_ADDR ) return BAD_ADDRESS;
	return NO_ERROR;
}

error_t _disk_read( disk_t diskno, diskaddr_t addr, disksiz_t count, block_t* dest ) {
	error_t err = check_sane(diskno, addr, count);
	if( err != NO_ERROR ) return err;
	for( int i = 0; i < count; i++ ) {
		_memcpy(dest[i], disk + (addr + i) * BLOCKSIZ, BLOCKSIZ);
	}
	return NO_ERROR;
}

error_t _disk_write( disk_t diskno, diskaddr_t addr, disksiz_t count, block_t* src ) {
	error_t err = check_sane(diskno, addr, count);
	if( err != NO_ERROR ) return err;
	for( int i = 0; i < count; i++ ) {
		_memcpy(disk + (addr + i) * BLOCKSIZ, src[i], BLOCKSIZ);
	}
	return NO_ERROR;
}

char* _disk_strerror( error_t code ) {
	switch( code ) {
		case NO_ERROR:
			return "No error";
		case BAD_DISK:
			return "Invalid drive index";
		case BAD_ADDRESS:
			return "Bad address";
		default:
			return "Unknown error code";
	}
}
