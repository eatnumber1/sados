#ifndef _RELFS_H
#define _RELFS_H

#include "disk.h"
#include "bool.h"

#define FREE_LIST_ADAPTER_ID 0
#define RELFS_ADAPTER_ID 1

/*
 * Disk Layout
 * +=============================+
 * | iTable Size | Inodes | Data |
 * +=============================+
 *
 * Inodes
 * +====================+
 * | Node | Node | Node |
 * +====================+
 *
 * Node
 * +=====================================+
 * | Key disk_node_t | Value disk_node_t |
 * +=====================================+
 */

/*
 * Represents a single logical "block" of data.
 */
typedef struct {
	diskaddr_t start;
	disk_size_t length;
} disk_node_t;

/*
 * Represents a relfs inode
 */
typedef struct {
	disk_node_t key;
	disk_node_t value;
	bool allocated;
} relinode_t;

typedef struct {
	disk_t disk;
	disk_node_t inodes;
	disk_node_t data;
	disk_node_t free;

	// Cache variables
	diskaddr_t free_ptr;
	disk_size_t disk_size;
} relfs_t;

/*
 * Create a relfs filesystem.
 *
 * - Create disk_node_t for the inode table and the data.
 * - Write the inode table size to the first sector.
 * - Zero the inode table
 */
relfs_t *_relfs_mkfs( disk_t disk, disk_size_t inode_table_size );

void _relfs_dump( relfs_t *fs );

relfs_t *_relfs_open( disk_t disk );
void _relfs_close( relfs_t *fs );

/*
 * Allocate space from a relfs filesystem.
 */
disk_node_t *_relfs_alloc( relfs_t *fs, char *name, disk_size_t size );
void _relfs_free( disk_node_t *node );

void _relfs_write( relfs_t *fs, disk_node_t *node, char *buf, disk_size_t len );
void _relfs_read( relfs_t *fs, disk_node_t *node, char *buf, disk_size_t len );

void _relfs_unlink( relfs_t *fs, char *name );
disk_node_t *_relfs_retrieve( relfs_t *fs, char *name );

#endif
