#ifndef _DISK_H
#define _DISK_H

typedef int diskaddr_t; // A type for storing disk addresses
typedef int disksiz_t;  // Defines the size of the data to be read/written
typedef char* block_t;  // Defines a _single_ block
typedef int blocksiz_t; // Defines the number of bytes in a single block
typedef int error_t;    // A type for storing error codes
typedef int disk_t;     // Defines the disk number being controlled

// Get the number of bytes in a block
blocksiz_t _blocksiz( disk_t diskno );

// Read count blocks starting from address addr into dest from disk diskno.
error_t _disk_read( disk_t diskno, diskaddr_t addr, disksiz_t count, block_t* dest );

// Write count blocks starting from address addr from src onto disk diskno.
error_t _disk_write( disk_t diskno, diskaddr_t addr, disksiz_t count, block_t* src );

// Convert an LBA error into a human-readable string
char* _disk_strerror( error_t code );

#endif
