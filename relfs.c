#define __KERNEL__20093__

#include "headers.h"
#include "disk.h"
#include "relfs.h"
#include "kalloc.h"
#include "assert.h"

static relfs_t *init_relfs_t( disk_t disk, disk_size_t inode_table_size ) {
	relfs_t *fs = _kalloc(sizeof(relfs_t));
	_memcpy(&fs->disk, &disk, sizeof(disk));
	disk_node_t *inodes = &fs->inodes;
	inodes->start = 1;
	inodes->length = inode_table_size;
	assert(inodes->length > 0, "Inode table too small");
	disk_node_t *data = &fs->data;
	data->start = inode_table_size + 1;
	data->length = _disk_size(disk) - inode_table_size - 1;
	assert(data->length > 0, "Data table too small");
	return fs;
}

static void _print_sector( sector_t sector ) {
	c_printf("[ ");
	for( disk_size_t x = 0; x < SECTOR_SIZE; x++ ) {
		c_printf("%x ", sector[x]);
	}
	c_printf("] ");
}

void _relfs_dump( relfs_t *fs ) {
	// Dump the inode table size
	sector_t sec[_sector_sizeof(disk_size_t)];
	_disk_read(fs->disk, 0, _sector_sizeof(disk_size_t), sec);
	disk_size_t inode_table_size = _sector_to_disk_size(sec);
	c_printf("inode_table_size = %d: [ ", inode_table_size);
	for( disk_size_t i = 0; i < _sector_sizeof(disk_size_t); i++ ) {
		_print_sector(sec[i]);
	}
	c_printf("]\n");

	// Dump the inode table
	sector_t inode_sec;
	c_printf("inode table { start = %d, length = %d }: [ ", fs->inodes.start, fs->inodes.length);
	for( int i = 0; i < fs->inodes.length; i++ ) {
		_disk_read(fs->disk, fs->inodes.start + i, 1, &inode_sec);
		_print_sector(inode_sec);
	}
	c_printf("]\n");
	
	// Dump the data table
	sector_t data_sec;
	c_printf("data table { start = %d, length = %d }: [ ", fs->data.start, fs->data.length);
	for( int i = 0; i < fs->data.length; i++ ) {
		_disk_read(fs->disk, fs->data.start + i, 1, &data_sec);
		_print_sector(data_sec);
	}
	c_printf("]\n");
}

/*
 * Create a relfs filesystem.
 *
 * - Create disk_node_t for the inode table and the data.
 * - Write the inode table size to the first sector.
 * - Zero the inode table
 */
relfs_t *_relfs_mkfs( disk_t disk, disk_size_t inode_table_size ) {
	// Create disk_node_t for the inode table and the data.
	relfs_t *fs = init_relfs_t(disk, inode_table_size);

	// Write the inode table size to the first sector.
	sector_t *sec = _disk_size_to_sector(inode_table_size);
	_disk_write(disk, 0, _sector_sizeof(disk_size_t), sec);
	_kfree(sec);
	
	// Zero the inode table
	sector_t s;
	_memclr(&s, sizeof(sector_t));
	for( disk_size_t i = fs->data.start; i < fs->data.length; i++ ) {
		_disk_write(disk, i, 1, &s);
	}

	return fs;
}

relfs_t *_relfs_open( disk_t disk ) {
	// Not yet implemented
	return NULL;
}

void _relfs_close( relfs_t *fs ) {
	_kfree(fs);
}
