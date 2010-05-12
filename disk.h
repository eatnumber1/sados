#ifndef _DISK_H
#define _DISK_H

#define SECTOR_SIZE 4

typedef int diskaddr_t;             // A type for storing disk addresses
typedef int disk_size_t;            // Defines the size of the data to be read/written (in sectors)
typedef int disk_size_b_t;          // Defines the size of data to be read/written (in bytes)
typedef char sector_t[SECTOR_SIZE]; // Defines a _single_ sector
typedef int error_t;                // A type for storing error codes
typedef int diskno_t;               // Defines the disk number being controlled
typedef int partno_t;               // Defines the disk partition number being controlled
typedef struct {
	diskno_t diskno;
	partno_t partno;
} disk_t;                           // Defines the disk being controlled

typedef enum {
	NO_ERROR,   // Returned on success
	BAD_DISK,   // Invalid drive index
	BAD_ADDRESS // Invalid address
} _disk_error;

// Read count sectors starting from address addr into dest from disk diskno.
error_t _disk_read( disk_t diskno, diskaddr_t addr, disk_size_t count, sector_t *dest );

// Write count sectors starting from address addr from src onto disk diskno.
error_t _disk_write( disk_t diskno, diskaddr_t addr, disk_size_t count, sector_t *src );

// Get the size of the disk in sectors.
disk_size_t _disk_size( disk_t diskno );

// Get the size of the disk in bytes.
disk_size_b_t _disk_size_b( disk_t diskno );

// Convert an LBA error into a human-readable string
char *_disk_strerror( error_t code );

sector_t *_buf_to_sector( void *buf, int len );
void *_sector_to_buf( sector_t *sectors, int num );

sector_t *_int_to_sector( int num );
int _sector_to_int( sector_t *sectors );

sector_t *_diskaddr_to_sector( diskaddr_t addr );
diskaddr_t _sector_to_diskaddr( sector_t *sectors );

sector_t *_disk_size_to_sector( disk_size_t addr );
disk_size_t _sector_to_disk_size( sector_t *sectors );

#define _sector_sizeof(type) \
	((sizeof(type) / SECTOR_SIZE) + \
	((sizeof(type) % SECTOR_SIZE != 0) ? 1 : 0))

#endif
