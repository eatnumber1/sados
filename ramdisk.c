#define __KERNEL__20093__

#include "headers.h"
#include "disk.h"

#define DISK_SIZE 512
#define MAX_ADDR SECTOR_SIZE * DISK_SIZE - 1

// I wish I could dynamically allocate this.
static char disk[DISK_SIZE * SECTOR_SIZE];

static error_t check_sane( disk_t diskno, diskaddr_t addr, disk_size_t count ) {
	if( diskno != 0 ) return BAD_DISK;
	if( count < 0 || addr + count > MAX_ADDR ) return BAD_ADDRESS;
	return NO_ERROR;
}

error_t _disk_read( disk_t diskno, diskaddr_t addr, disk_size_t count, sector_t* dest ) {
	error_t err = check_sane(diskno, addr, count);
	if( err != NO_ERROR ) return err;
	for( int i = 0; i < count; i++ ) {
		_memcpy(dest[i], disk + (addr + i) * SECTOR_SIZE, SECTOR_SIZE);
	}
	return NO_ERROR;
}

error_t _disk_write( disk_t diskno, diskaddr_t addr, disk_size_t count, sector_t* src ) {
	error_t err = check_sane(diskno, addr, count);
	if( err != NO_ERROR ) return err;
	for( int i = 0; i < count; i++ ) {
		_memcpy(disk + (addr + i) * SECTOR_SIZE, src[i], SECTOR_SIZE);
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
