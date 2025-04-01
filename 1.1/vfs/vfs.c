#include "../include/vfs.h"

vfs_t vfs;

typedef struct {
    int filesdeleted;
    int dirsdeleted;
    int errors;
    char lasterrorpath[MAX_PATH];
} vfs_delete_result_t;

void get_full_path(vfs_node_t* node, char* path, size_t size) {
    if (node == NULL || size == 0) {
        print("get_full_path: Invalid node or size\n");
        return;
    }

    char temp_stack[MAX_PATH][MAX_FILENAME];
    int depth = 0;

    vfs_node_t* current = node;
    while (current != NULL && depth < MAX_PATH) {
        if (current->name == NULL) {
            print("get_full_path: Node name is NULL\n");
            return;
        }
        kstrncpy(temp_stack[depth], current->name, MAX_FILENAME);
        depth++;
        current = current->parent;

        vfs_node_t* check = node;
        int steps = 0;
        while (check != NULL && steps < depth) {
            if (check == current) {
                print("get_full_path: Cycle detected in parent pointers\n");
                return;
            }
            check = check->parent;
            steps++;
        }
    }
    path[0] = '\0';
    kstrncat(path, "/", size);
    for (int i = depth - 2; i >= 0; i--) { 
        if (path[kstrlen(path) - 1] != '/') {
            kstrncat(path, "/", size);
        }
        kstrncat(path, temp_stack[i], size);
    }
}

void vfs_init(void) {
    print("Initializing VFS...\n");
    vfs.root = vfs_create_node("/", VFS_TYPE_DIRECTORY);
    if (vfs.root == NULL) {
        print("Failed to create root directory!\n");
        return;
    }
    print("Root directory created\n");
    
    vfs.current_dir = vfs.root;
    vfs.num_files = 0;
    
    print("VFS initialized successfully\n");
}

vfs_node_t* vfs_create_node(const char* name, unsigned int type) {
    vfs_node_t* node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
    if (!node) return NULL;

    kstrncpy(node->name, name, MAX_FILENAME - 1);
    node->type = type;
    node->permissions = VFS_PERM_READ | VFS_PERM_WRITE;
    node->size = 0;
    node->inode = vfs.num_files++;
    node->parent = NULL;
    node->children = NULL;
    node->next_sibling = NULL;
    node->data = NULL;

    return node;
}

static int vfs_can_delete_node(vfs_node_t* node) {
    if(!node) {
        print("vfs_can_delete_node: NULL node\n");
        return 0;
    }
    if(node == vfs.root) {
        print("vfs_can_delete_node: Cannot delete root directory\n");
        return 0;
    }
    if(node == vfs.current_dir) {
        print("vfs_can_delete_node: Cannot delete current directory\n");
        return 0;
    }
    return 1;
}

static void vfs_free_node(vfs_node_t* node) {
    if (!node) {
        print("vfs_free_node: null node\n");
        return;
    }
    
    print("vfs_free_node: freeing node '");
    print(node->name);
    print("', inode: ");
    print_int(node->inode);
    print("\n");

    if (node->data) {
        print("vfs_free_node: freeing node data\n");
        kfree(node->data);
    }
    kfree(node);
}

static void vfs_delete_recursive(vfs_node_t* node, vfs_delete_result_t* result) {
    if(!node) return;

    if(node->type == VFS_TYPE_DIRECTORY) {
        vfs_node_t* child = node->children;
        while(child) {
            vfs_node_t* next = child->next_sibling;
            vfs_delete_recursive(child, result);
            child = next;
        }
    }
    if(node->type == VFS_TYPE_FILE) {
        result->filesdeleted++;
    } else {
        result->dirsdeleted++;
    }
    vfs_free_node(node);
}

static char* get_next_path_component(const char** path) {
    static char component[MAX_FILENAME];
    int i = 0;
    
    while (**path == '/') (*path)++;
    
    while (**path && **path != '/' && i < MAX_FILENAME - 1) {
        component[i++] = *(*path)++;
    }
    
    component[i] = '\0';
    return component;
}

vfs_node_t* vfs_find_node(const char* path) {
    if (!path || !*path) return NULL;

    vfs_node_t* current = (*path == '/') ? vfs.root : vfs.current_dir;
    const char* path_ptr = path;

    while (current && *path_ptr) {
        char* component = get_next_path_component(&path_ptr);
        if (!*component) break;

        if (kstrcmp(component, ".") == 0) continue;
        if (kstrcmp(component, "..") == 0) {
            current = current->parent ? current->parent : current;
            continue;
        }

        vfs_node_t* found = NULL;
        vfs_node_t* child = current->children;
        while (child) {
            if (kstrcmp(child->name, component) == 0) {
                found = child;
                break;
            }
            child = child->next_sibling;
        }

        if (!found) return NULL;
        current = found;
    }

    return current;
}

int vfs_create_file(const char* path) {
    char parent_path[MAX_PATH];
    char* last_slash = kstrrchr(path, '/');
    const char* filename;

    if (last_slash) {
        kstrncpy(parent_path, path, last_slash - path);
        parent_path[last_slash - path] = '\0';
        filename = last_slash + 1;
    } else {
        parent_path[0] = '.';
        parent_path[1] = '\0';
        filename = path;
    }

    vfs_node_t* parent = vfs_find_node(parent_path);
    if (!parent || parent->type != VFS_TYPE_DIRECTORY) {
        print("vfs_create_file: Invalid parent directory for path: ");
        print(path);
        print("\n");
        return -1;
    }

    vfs_node_t* new_node = vfs_create_node(filename, VFS_TYPE_FILE);
    if (!new_node) return -1;

    new_node->parent = parent;
    new_node->next_sibling = parent->children;
    parent->children = new_node;

    print("vfs_create_file: Created file '");
    print(filename);
    print("' in directory\n");
    vfs_list_directory(parent_path);

    return 0;
}

int vfs_create_directory(const char* path) {
    char parent_path[MAX_PATH];
    char* last_slash = kstrrchr(path, '/');
    const char* dirname;
    
    if (last_slash) {
        kstrncpy(parent_path, path, last_slash - path);
        parent_path[last_slash - path] = '\0';
        dirname = last_slash + 1;
    } else {
        parent_path[0] = '.';
        parent_path[1] = '\0';
        dirname = path;
    }
    
    vfs_node_t* parent = vfs_find_node(parent_path);
    if (!parent || parent->type != VFS_TYPE_DIRECTORY) return -1;
    
    vfs_node_t* new_node = vfs_create_node(dirname, VFS_TYPE_DIRECTORY);
    if (!new_node) return -1;
    
    new_node->parent = parent;
    new_node->next_sibling = parent->children;
    parent->children = new_node;
    
    return 0;
}

void vfs_list_directory(const char* path) {
    print("Listing directory: '");
    print(path);
    print("'\n");

    vfs_node_t* dir = path ? vfs_find_node(path) : vfs.current_dir;
    if (!dir || dir->type != VFS_TYPE_DIRECTORY) {
        print("Invalid directory\n");
        return;
    }
    
    print("Directory found, listing contents:\n");
    
    if (dir->children == NULL) {
        print("<empty directory>\n");
        return;
    }
    
    vfs_node_t* child = dir->children;
    while (child) {
        char type = (child->type == VFS_TYPE_DIRECTORY) ? 'd' : '-';
        char perm[4] = "---";
        if (child->permissions & VFS_PERM_READ) perm[0] = 'r';
        if (child->permissions & VFS_PERM_WRITE) perm[1] = 'w';
        if (child->permissions & VFS_PERM_EXEC) perm[2] = 'x';
        
        char info[MAX_FILENAME + 32];
        int i = 0;
        info[i++] = type;
        info[i++] = perm[0];
        info[i++] = perm[1];
        info[i++] = perm[2];
        info[i++] = ' ';
        int j = 0;
        while (child->name[j] && i < MAX_FILENAME + 30) {
            info[i++] = child->name[j++];
        }
        info[i++] = '\n';
        info[i] = '\0';
        
        print(info);
        child = child->next_sibling;
    }
}

int vfs_change_directory(const char* path) {
    vfs_node_t* new_dir = vfs_find_node(path);
    if (!new_dir || new_dir->type != VFS_TYPE_DIRECTORY) return -1;
    
    vfs.current_dir = new_dir;
    return 0;
}

int vfs_delete(const char* path, int recursive) {
    if (!path || !*path) {
        print("vfs_delete: invalid path\n");
        return -1;
    }

    print("vfs_delete: processing path: ");
    print(path);
    print("\n");

    vfs_node_t* node = vfs_find_node(path);
    if (!node) {
        print("vfs_delete: path not found: ");
        print(path);
        print("\n");
        return -1;
    }

    print("vfs_delete: node found, inode: ");
    print_int(node->inode);
    print(", type: ");
    print(node->type == VFS_TYPE_FILE ? "file" : "directory");
    print("\n");

    if (!vfs_can_delete_node(node)) {
        print("vfs_delete: cannot delete node\n");
        return -1;
    }

    if (node->type == VFS_TYPE_DIRECTORY && node->children && !recursive) {
        print("vfs_delete: directory not empty: ");
        print(path);
        print("\nUse -r flag for recursive deletion\n");
        return -1;
    }

    vfs_node_t* parent = node->parent;
    if (!parent) {
        print("vfs_delete: no parent found for: ");
        print(path);
        print("\n");
        return -1;
    }

    print("vfs_delete: parent found, inode: ");
    print_int(parent->inode);
    print("\n");

    if (parent->children == node) {
        print("vfs_delete: node is first child, updating parent->children\n");
        parent->children = node->next_sibling;
    } else {
        vfs_node_t* prev = parent->children;
        while (prev && prev->next_sibling != node) {
            prev = prev->next_sibling;
        }
        if (prev) {
            print("vfs_delete: found previous sibling, updating next_sibling\n");
            prev->next_sibling = node->next_sibling;
        } else {
            print("vfs_delete: node not found in parent's children list\n");
            return -1;
        }
    }

    vfs_delete_result_t result = {0, 0, 0, {0}};
    
    if (node->type == VFS_TYPE_DIRECTORY && recursive) {
        print("vfs_delete: recursive deletion starting\n");
        vfs_delete_recursive(node, &result);
    } else {
        print("vfs_delete: single node deletion\n");
        if (node->type == VFS_TYPE_FILE) {
            result.filesdeleted++;
        } else {
            result.dirsdeleted++;
        }
        vfs_free_node(node);
    }

    print("vfs_delete: deletion completed, files: ");
    print_int(result.filesdeleted);
    print(", dirs: ");
    print_int(result.dirsdeleted);
    print("\n");

    return result.errors == 0 ? 0 : -1;
}