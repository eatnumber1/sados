#ifndef _DISK_H
#define _DISK_H

#define SECTOR_SIZE 4

typedef int diskaddr_t;             // A type for storing disk addresses
typedef int disk_size_t;            // Defines the size of the data to be read/written (in sectors)
typedef int disk_size_b_t;          // Defines the size of data to be read/written (in bytes)
typedef char sector_t[SECTOR_SIZE]; // Defines a _single_ sector
typedef int diskno_t;               // Defines the disk number being controlled
typedef int partno_t;               // Defines the disk partition number being controlled
typedef struct {
	diskno_t diskno;
	partno_t partno;
} disk_t;                           // Defines the disk being controlled

// Read count sectors starting from address addr into dest from disk diskno.
void _disk_read( disk_t diskno, diskaddr_t addr, disk_size_t count, sector_t *dest );

// Write count sectors starting from address addr from src onto disk diskno.
void _disk_write( disk_t diskno, diskaddr_t addr, disk_size_t count, sector_t *src );

// Get the size of the disk in sectors.
disk_size_t _disk_size( disk_t diskno );

sector_t *_buf_to_sector( void *buf, int len );
void *_sector_to_buf( sector_t *sectors, int num );

sector_t *_int_to_sector( int num );
int _sector_to_int( sector_t *sectors );

sector_t *_diskaddr_to_sector( diskaddr_t addr );
diskaddr_t _sector_to_diskaddr( sector_t *sectors );

sector_t *_disk_size_to_sector( disk_size_t addr );
disk_size_t _sector_to_disk_size( sector_t *sectors );

disk_size_t _sectors_in( disk_size_t size );

#define _sector_sizeof(type) _sectors_in(sizeof(type))

#endif
