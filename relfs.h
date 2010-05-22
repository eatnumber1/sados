#ifndef _RELFS_H
#define _RELFS_H

#include "disk.h"
#include "bool.h"

/*
 * This filesystem uses an in-memory pointer to point at the first free sector.
 * The space taken by an allocated inode is not reclaimed when deleted.
 *  - Defragmentation would be necessary to reclaim space.
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

/*
 * Represents an open relational filesystem
 */
typedef struct {
	// The drive + partition to write to
	disk_t disk;
	// The inode block
	disk_node_t inodes;
	// The data block
	disk_node_t data;
	// Allocated space for the free pointer
	disk_node_t free;

	// Cache variables
	// The cached free_ptr. Should be the same as what is on disk at free.
	diskaddr_t free_ptr;
	// The cached size of the disk.
	disk_size_t disk_size;
} relfs_t;

/*
 * Create a relfs filesystem.
 * Params:
 *  - disk - The drive to write to
 *  - inode_table_size - The size of the inode table in sectors
 *
 * Returns: A pointer to a relfs_t representing the open filesystem.
 *          This pointer must be freed using _relfs_close.
 *
 * Note: This function returns an open filesystem.
 *       No calls to _relfs_open should be made.
 */
relfs_t *_relfs_mkfs( disk_t disk, disk_size_t inode_table_size );

/*
 * Dump to the screen the contents of the drive.
 * Params:
 *  - The filesystem to dump.
 */
void _relfs_dump( relfs_t *fs );

/*
 * Open an existing relfs filesystem.
 * Params:
 *  - disk - The drive to open
 *
 * Returns: A pointer to a relfs_t representing the open filesystem.
 *          This pointer must be freed using _relfs_close.
 */
relfs_t *_relfs_open( disk_t disk );

/*
 * Close an open relfs_t filesystem.
 * Params:
 *  - fs - The fs to close
 */
void _relfs_close( relfs_t *fs );

/*
 * Allocate space from a relfs filesystem.
 * Param:
 *  - fs - The filesystem to allocate from
 *  - name - The name of the entry
 *  - size - The number of bytes to allocate
 *
 * Returns: A pointer to an open disk_node_t representing the allocated data.
 *          This pointer should be closed using _relfs_free.
 */
disk_node_t *_relfs_alloc( relfs_t *fs, char *name, disk_size_t size );

/*
 * Close a disk_node_t opened by _relfs_alloc or _relfs_retrieve.
 */
void _relfs_free( disk_node_t *node );

/*
 * Write to the disk at the location pointed to by node.
 * Param:
 *  - fs - The filesystem to write to
 *  - node - The disk node representing the location on the disk to write to
 *  - buf - The data to write
 *  - len - The length of the data in bytes
 */
void _relfs_write( relfs_t *fs, disk_node_t *node, char *buf, disk_size_t len );

/*
 * Read from the disk at the location pointed to by node.
 * Param:
 *  - fs - The filesystem to read from
 *  - node - The disk node representing the location on the disk to read from
 *  - buf - A buffer to write the read data into
 *  - len - The number of bytes to read
 */
void _relfs_read( relfs_t *fs, disk_node_t *node, char *buf, disk_size_t len );

/*
 * Delete an allocated relfs key-value node.
 * Param:
 *  - fs - The filesystem to delete from
 *  - name - The name of the entry to delete
 */
void _relfs_unlink( relfs_t *fs, char *name );

/*
 * Open an existing allocated relfs key-value node.
 * Param:
 *  - fs - The filesystem to open from
 *  - name - The name of the entry to open
 *
 * Return: A disk_node_t representing the data (value) portion of the key-value
 *         pair. This node must be closed using _relfs_free.
 */
disk_node_t *_relfs_retrieve( relfs_t *fs, char *name );

#endif
