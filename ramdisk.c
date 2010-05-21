#define __KERNEL__20093__

#include "headers.h"
#include "disk.h"
#include "assert.h"

#define DISK_SIZE 48
#define MAX_ADDR SECTOR_SIZE * DISK_SIZE - 1

static char disk[DISK_SIZE * SECTOR_SIZE];

static void check_sane( disk_t diskno, diskaddr_t addr, disk_size_t count ) {
	assert(diskno.diskno == 0 && diskno.partno == 0, "Bad disk");
	assert(count >= 0 || addr + count <= MAX_ADDR, "Bad address");
}

disk_size_t _disk_size( disk_t diskno ) {
	return DISK_SIZE;
}

disk_size_b_t _disk_size_b( disk_t diskno ) {
	return _disk_size(diskno) * SECTOR_SIZE;
}

void _disk_read( disk_t diskno, diskaddr_t addr, disk_size_t count, sector_t* dest ) {
	check_sane(diskno, addr, count);
	for( int i = 0; i < count; i++ ) {
		_memcpy(dest[i], disk + (addr + i) * SECTOR_SIZE, SECTOR_SIZE);
	}
}

void _disk_write( disk_t diskno, diskaddr_t addr, disk_size_t count, sector_t* src ) {
	check_sane(diskno, addr, count);
	for( int i = 0; i < count; i++ ) {
		_memcpy(disk + (addr + i) * SECTOR_SIZE, src[i], SECTOR_SIZE);
	}
}
