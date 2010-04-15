#ifndef _DISK_H
#define _DISK_H

#define SECTOR_SIZE 512

typedef int diskaddr_t;             // A type for storing disk addresses
typedef int disk_size_t;            // Defines the size of the data to be read/written
typedef char sector_t[SECTOR_SIZE]; // Defines a _single_ sector
typedef int error_t;                // A type for storing error codes
typedef int disk_t;                 // Defines the disk number being controlled

typedef enum {
	NO_ERROR,   // Returned on success
	BAD_DISK,   // Invalid drive index
	BAD_ADDRESS // Invalid address
} _disk_error;

// Read count sectors starting from address addr into dest from disk diskno.
error_t _disk_read( disk_t diskno, diskaddr_t addr, disk_size_t count, sector_t *dest );

// Write count sectors starting from address addr from src onto disk diskno.
error_t _disk_write( disk_t diskno, diskaddr_t addr, disk_size_t count, sector_t *src );

// Convert an LBA error into a human-readable string
char *_disk_strerror( error_t code );

#endif
