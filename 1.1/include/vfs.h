#ifndef VFS_H
#define VFS_H

#include "kernel.h"
#include <stddef.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#define MAX_FILENAME 128
#define MAX_PATH 256
#define MAX_FILES 64
#define MAX_OPEN_FILES 16

// File types
#define VFS_TYPE_FILE 1
#define VFS_TYPE_DIRECTORY 2

// File permissions
#define VFS_PERM_READ 0x04
#define VFS_PERM_WRITE 0x02
#define VFS_PERM_EXEC 0x01

void kstrncpy(char* dest, const char* src, size_t n);
int kstrcmp(const char* s1, const char* s2);
char* kstrrchr(const char* s, int c);
int ksnprintf(char* str, size_t size, const char* format, ...);
void kstrncat(char* dest, const char* src, size_t n);
size_t kstrlen(const char* s);

// Memory allocation functions 
void* kmalloc(size_t size);
void kfree(void* ptr);

typedef struct vfs_node {
    char name[MAX_FILENAME];
    unsigned int type;
    unsigned int permissions;
    unsigned int size;
    unsigned int inode;
    struct vfs_node* parent;
    struct vfs_node* children;
    struct vfs_node* next_sibling;
    void* data;
} vfs_node_t;

typedef struct {
    vfs_node_t* root;
    vfs_node_t* current_dir;
    unsigned int num_files;
} vfs_t;

// VFS operations
void vfs_init(void);
vfs_node_t* vfs_create_node(const char* name, unsigned int type);
int vfs_create_file(const char* path);
int vfs_create_directory(const char* path);
vfs_node_t* vfs_find_node(const char* path);
int vfs_delete(const char* path, int recursive);
void vfs_list_directory(const char* path);
int vfs_change_directory(const char* path);

extern vfs_t vfs;

#endif