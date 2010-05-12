#ifndef _RELFS_H
#define _RELFS_H

#include "disk.h"

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

typedef int relfs_view_id;

typedef struct {
	relfs_view_id id;
	disk_node_t disk;
} relfs_view_t;

/*
 * Represents a relfs inode
 */
typedef struct {
	disk_node_t key;
	relfs_view_t value;
	char allocated;
} relinode_t;

typedef struct {
	disk_t disk;
	disk_node_t inodes;
	disk_node_t data;
} relfs_t;

/*
 * Create a relfs filesystem.
 *
 * - Create disk_node_t for the inode table and the data.
 * - Write the inode table size to the first sector.
 * - Zero the inode table
 */
relfs_t *_relfs_mkfs( disk_t disk, disk_size_t inode_table_size );

relfs_t *_relfs_open( disk_t disk );

void _relfs_close( relfs_t *fs );

void _relfs_dump( relfs_t *fs );

#endif
