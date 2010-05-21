#define __KERNEL__20093__

#include "headers.h"
#include "disk.h"
#include "relfs.h"
#include "kalloc.h"
#include "assert.h"
#include "hash.h"
#include "string.h"
#include "bool.h"
#include "sio.h"

static relfs_t *init_relfs_t( disk_t disk, disk_size_t inode_table_size ) {
	relfs_t *fs = _kalloc(sizeof(relfs_t));
	_memclr(fs, sizeof(relfs_t));
	_memcpy(&fs->disk, &disk, sizeof(disk));
	fs->disk_size = _disk_size(disk);
	disk_node_t *inodes = &fs->inodes;
	inodes->start = 1;
	inodes->length = inode_table_size;
	assert(inodes->length > 0, "Inode table too small");
	disk_node_t *data = &fs->data;
	data->start = inode_table_size + 1;
	data->length = fs->disk_size - inode_table_size - 1;
	assert(data->length > 0, "Data table too small");
	return fs;
}

static void _print_sector( sector_t sector ) {
	_sio_puts("[ ");
	for( disk_size_t x = 0; x < SECTOR_SIZE; x++ ) {
		_sio_puti(sector[x]); _sio_puts(" ");
	}
	_sio_puts("] ");
}

static void _reduce_free( relfs_t *fs, disk_size_t size ) {
	// Reduce the free_ptr
	fs->free_ptr += size;
	assert(fs->free_ptr <= fs->disk_size, "Out of free space");

	// Reduce the on-disk free size
	sector_t *sec = _buf_to_sector(&(fs->free_ptr), sizeof(fs->free_ptr));
	_disk_write(fs->disk, fs->free.start, fs->free.length, sec);
	_kfree(sec);
}

void _relfs_dump( relfs_t *fs ) {
	// Print the free ptr
	_sio_puts("free_ptr = "); _sio_puti(fs->free_ptr); _sio_puts("\n");

	// Dump the inode table size
	sector_t sec[_sector_sizeof(disk_size_t)];
	_disk_read(fs->disk, 0, _sector_sizeof(disk_size_t), sec);
	disk_size_t inode_table_size = _sector_to_disk_size(sec);
	_sio_puts("inode_table_size = "); _sio_puti(inode_table_size); _sio_puts(": [ ");
	for( disk_size_t i = 0; i < _sector_sizeof(disk_size_t); i++ )
		_print_sector(sec[i]);
	_sio_puts("]\n");

	// Dump the inode table
	sector_t inode_sec;
	_sio_puts("inode table { start = "); _sio_puti(fs->inodes.start);
	_sio_puts(", length = "); _sio_puti(fs->inodes.length); _sio_puts(" }: [ ");
	for( int i = 0; i < fs->inodes.length; i++ ) {
		_disk_read(fs->disk, fs->inodes.start + i, 1, &inode_sec);
		_print_sector(inode_sec);
	}
	_sio_puts("]\n");
	
	// Dump the data table
	sector_t data_sec;
	_sio_puts("data table { start = "); _sio_puti(fs->data.start);
	_sio_puts(", length = "); _sio_puti(fs->data.length); _sio_puts(" }: [ ");
	for( int i = 0; i < fs->data.length; i++ ) {
		_disk_read(fs->disk, fs->data.start + i, 1, &data_sec);
		_print_sector(data_sec);
	}
	_sio_puts("]\n");
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
	fs->free_ptr = fs->data.start;

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

	// Initialize the free disk_node_t manually. Can't _relfs_alloc yet.
	fs->free.start = fs->data.start;
	fs->free.length = _sector_sizeof(disk_size_t);
	fs->data.length -= fs->free.length;
	fs->data.start = fs->free.start + fs->free.length;
	_reduce_free(fs, fs->free.length);

	return fs;
}

relfs_t *_relfs_open( disk_t disk ) {
	sector_t sec[_sector_sizeof(disk_size_t)];
	_disk_read(disk, 0, _sector_sizeof(disk_size_t), sec);
	disk_size_t inode_table_size = _sector_to_disk_size(sec);

	// Create disk_node_t for the inode table and the data.
	return init_relfs_t(disk, inode_table_size);
}

void _relfs_close( relfs_t *fs ) {
	_kfree(fs);
}

static diskaddr_t _inode_index_to_addr( relfs_t *fs, unsigned int index ) {
	disk_size_t inode_size = _sector_sizeof(relinode_t);
	disk_size_t max_inode_idx = fs->inodes.length / inode_size;
	return fs->inodes.start + ((index % max_inode_idx) * inode_size);
}

static void write_inode( relfs_t *fs, relinode_t *inode, char *key ) {
	sector_t *sec = _buf_to_sector(inode, sizeof(relinode_t));
	_disk_write(fs->disk,
		_inode_index_to_addr(fs,
			_hash(key, strlen(key))),
		_sector_sizeof(relinode_t),
		sec);
	_kfree(sec);
}

static relinode_t *read_inode( relfs_t *fs, char *key ) {
	sector_t sec[_sector_sizeof(relinode_t)];
	_disk_read(fs->disk,
		_inode_index_to_addr(fs,
			_hash(key, strlen(key))),
		_sector_sizeof(relinode_t),
		sec);
	return (relinode_t *) _sector_to_buf(sec, _sector_sizeof(relinode_t));
}

/*
 * Allocate space from a relfs filesystem.
 */
disk_node_t *_relfs_alloc( relfs_t *fs, char *name, disk_size_t size ) {
	disk_size_t key_size = strlen(name) + 1,
		key_size_sec = _sectors_in(key_size),
		size_sec = _sectors_in(size);

	// Allocate the space
	relinode_t inode;
	inode.allocated = true;
	inode.key.start = fs->free_ptr;
	inode.key.length = key_size_sec;
	_reduce_free(fs, key_size_sec);
	inode.value.start = fs->free_ptr;
	inode.value.length = size_sec;
	_reduce_free(fs, size_sec);

	// Write the key
	_relfs_write(fs, &inode.key, name, key_size);

	// Write the inode
	write_inode(fs, &inode, name);

	// Generate a disk_node_t and return it.
	disk_node_t *data = _kalloc(sizeof(disk_node_t));
	_memcpy(data, &(inode.value), sizeof(disk_node_t));
	return data;
}

void _relfs_free( disk_node_t *node ) {
	_kfree(node);
}

void _relfs_write( relfs_t *fs, disk_node_t *node, char *buf, disk_size_t len ) {
	disk_size_t sector_len = _sectors_in(len);
	assert(node->length <= sector_len, "Buffer length too big");
	sector_t *sec = _buf_to_sector(buf, len);
	_disk_write(fs->disk, node->start, sector_len, sec);
	_kfree(sec);
}

void _relfs_read( relfs_t *fs, disk_node_t *node, char *buf, disk_size_t len ) {
	disk_size_t sector_len = _sectors_in(len);
	assert(node->length <= sector_len, "Buffer length too big");
	sector_t sec[sector_len];
	_disk_read(fs->disk, node->start, sector_len, sec);
	char *buf2 = _sector_to_buf(sec, sector_len);
	_memcpy(buf, buf2, len);
	_kfree(buf2);
}

void _relfs_unlink( relfs_t *fs, char *name ) {
	relinode_t *inode = read_inode(fs, name);
	inode->allocated = false;
	write_inode(fs, inode, name);
	_kfree(inode);
}

disk_node_t *_relfs_retrieve( relfs_t *fs, char *name ) {
	relinode_t *inode = read_inode(fs, name);
	if( inode->allocated == false ) {
		_kfree(inode);
		return NULL;
	}
	disk_node_t *data = _kalloc(sizeof(disk_node_t));
	_memcpy(data, &(inode->value), sizeof(disk_node_t));
	_kfree(inode);
	return data;
}
