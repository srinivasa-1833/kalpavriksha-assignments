#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME_LEN 50
#define MAX_BLOCKS_PER_FILE 50
#define INPUT_BUF 4096

typedef enum { FILE_NODE, DIR_NODE } NodeType;

typedef struct FreeBlock {
    int index;
    struct FreeBlock *next;
    struct FreeBlock *prev;
} FreeBlock;

typedef struct FileNode {
    char name[MAX_NAME_LEN + 1];
    NodeType type;
    struct FileNode *next;
    struct FileNode *prev;
    struct FileNode *parent;
    struct FileNode *child;
    int blockPointers[MAX_BLOCKS_PER_FILE];
    int blockCount;
    int size;
} FileNode;

unsigned char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];
FreeBlock *freeListHead = NULL, *freeListTail = NULL;
FileNode *root = NULL, *cwd = NULL;

int isValidName(const char *name) {
    if (!name) return 0;
    size_t len = strlen(name);
    if (len == 0 || len > MAX_NAME_LEN) return 0;
    for (size_t i = 0; i < len; ++i) {
        if (name[i] == '/' || iscntrl((unsigned char)name[i]) || name[i] == ' ') return 0;
    }
    return 1;
}

void initFreeList() {
    freeListHead = freeListTail = NULL;
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        FreeBlock *b = (FreeBlock*)malloc(sizeof(FreeBlock));
        b->index = i;
        b->next = NULL;
        b->prev = freeListTail;
        if (freeListTail) freeListTail->next = b;
        else freeListHead = b;
        freeListTail = b;
    }
}

FreeBlock* allocateBlockNode() {
    if (!freeListHead) return NULL;
    FreeBlock *b = freeListHead;
    freeListHead = freeListHead->next;
    if (freeListHead) freeListHead->prev = NULL;
    else freeListTail = NULL;
    b->next = b->prev = NULL;
    return b;
}

void freeBlockIndex(int index) {
    FreeBlock *b = (FreeBlock*)malloc(sizeof(FreeBlock));
    b->index = index;
    b->next = NULL;
    b->prev = freeListTail;
    if (freeListTail) freeListTail->next = b;
    else freeListHead = b;
    freeListTail = b;
}

void cleanupFreeList() {
    FreeBlock *cur = freeListHead;
    while (cur) {
        FreeBlock *n = cur->next;
        free(cur);
        cur = n;
    }
    freeListHead = freeListTail = NULL;
}

FileNode* createNode(const char *name, NodeType type) {
    FileNode *n = (FileNode*)malloc(sizeof(FileNode));
    strncpy(n->name, name, MAX_NAME_LEN);
    n->name[MAX_NAME_LEN] = '\0';
    n->type = type;
    n->next = n->prev = n->child = NULL;
    n->parent = cwd;
    n->blockCount = 0;
    n->size = 0;
    for (int i = 0; i < MAX_BLOCKS_PER_FILE; ++i)
        {
            n->blockPointers[i] = -1;
        }
    return n;
}

void insertChild(FileNode **childRef, FileNode *node) {
    if (!*childRef) {
        *childRef = node;
        node->next = node->prev = node;
        return;
    }
    FileNode *first = *childRef;
    FileNode *last = first->prev;
    last->next = node;
    node->prev = last;
    node->next = first;
    first->prev = node;
}

FileNode* findChild(FileNode *parent, const char *name) {
    if (!parent || !parent->child) return NULL;
    FileNode *cur = parent->child;
    do {
        if (strcmp(cur->name, name) == 0)
        {
            return cur;
        }
        cur = cur->next;
    } while (cur != parent->child);
    return NULL;
}

void unlinkFromParent(FileNode *node) {
    if (!node || !node->parent) return;
    FileNode *parent = node->parent;
    if (!parent->child) return;
    if (node->next == node && node->prev == node) {
        parent->child = NULL;
    } else {
        if (parent->child == node)
        {
            parent->child = node->next;
        }
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    node->next = node->prev = NULL;
    node->parent = NULL;
}

int countFreeBlocks() {
    int c = 0;
    for (FreeBlock *t = freeListHead; t; t = t->next) ++c;
    return c;
}

void freeAllNodes(FileNode *node) {
    if (!node) return;
    if (node->type == DIR_NODE) {
        while (node->child) {
            FileNode *child = node->child;
            unlinkFromParent(child);
            freeAllNodes(child);
        }
        free(node);
    } else {
        for (int i = 0; i < node->blockCount; ++i) {
            if (node->blockPointers[i] >= 0)
            {
                freeBlockIndex(node->blockPointers[i]);
            }
        }
        free(node);
    }
}

void mkdirCmd(char *name) {
    if (!isValidName(name)) { 
        printf("Invalid directory name.\n");
        return; 
    }
    if (findChild(cwd, name)) {
        printf("Directory or file already exists.\n");
        return;
    }
    FileNode *dir = createNode(name, DIR_NODE);
    insertChild(&cwd->child, dir);
    printf("Directory '%s' created successfully.\n", name);
}

void createCmd(char *name) {
    if (!isValidName(name)) {
        printf("Invalid file name.\n");
        return;
    }
    if (findChild(cwd, name)) {
        printf("File or directory already exists.\n");
        return;
    }
    FileNode *file = createNode(name, FILE_NODE);
    insertChild(&cwd->child, file);
    printf("File '%s' created successfully.\n", name);
}

void writeCmd(char *name, char *data) {
    if (!isValidName(name)) 
    { 
        printf("Invalid file name.\n");
        return;
    }
    FileNode *file = findChild(cwd, name);
    if (!file) {
        printf("File not found.\n");
        return;
    }
    if (file->type != FILE_NODE) {
        printf("Target is a directory.\n");
        return;
    }
    int len = (int)strlen(data);
    int required = (len + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (len == 0) required = 0;
    if (required > MAX_BLOCKS_PER_FILE) {
        printf("File too large. Limit is %d blocks.\n", MAX_BLOCKS_PER_FILE);
        return;
    }
    for (int i = 0; i < file->blockCount; ++i) {
        if (file->blockPointers[i] >= 0) freeBlockIndex(file->blockPointers[i]);
        file->blockPointers[i] = -1;
    }
    file->blockCount = 0;
    if (required > 0) {
        if (required > countFreeBlocks()) { 
            printf("Disk full: needed=%d, free=%d\n", required, countFreeBlocks());
            return;
        }
        for (int b = 0; b < required; ++b) {
            FreeBlock *bn = allocateBlockNode();
            if (!bn) {
                printf("Unexpected allocation failure.\n");
                return;
            }
            file->blockPointers[file->blockCount++] = bn->index;
            memcpy(virtualDisk[bn->index], data + b * BLOCK_SIZE, (size_t)((len - b * BLOCK_SIZE) > BLOCK_SIZE ? BLOCK_SIZE : (len - b * BLOCK_SIZE)));
            free(bn);
        }
    }
    file->size = len;
    printf("Data written successfully (size=%d bytes, blocks=%d).\n", file->size, file->blockCount);
}

void readCmd(char *name) {
    if (!isValidName(name)) {
        printf("Invalid file name.\n");
        return;
    }
    FileNode *file = findChild(cwd, name);
    if (!file) {
        printf("File not found.\n");
        return;
    }
    if (file->type != FILE_NODE) {
        printf("Target is a directory.\n"); 
        return;
    }
    if (file->size == 0) { printf("(empty)\n"); return; }
    int bytesLeft = file->size;
    for (int i = 0; i < file->blockCount && bytesLeft > 0; ++i) {
        int take = bytesLeft > BLOCK_SIZE ? BLOCK_SIZE : bytesLeft;
        fwrite(virtualDisk[file->blockPointers[i]], 1, take, stdout);
        bytesLeft -= take;
    }
    printf("\n");
}

void deleteCmd(char *name) {
    if (!isValidName(name)) {
        printf("Invalid file name.\n");
        return;
    }
    FileNode *file = findChild(cwd, name);
    if (!file) {
        printf("File not found.\n");
        return;
    }
    if (file->type != FILE_NODE) {
        printf("Target is a directory. Use rmdir.\n");
        return;
    }
    unlinkFromParent(file);
    for (int i = 0; i < file->blockCount; ++i){
        if (file->blockPointers[i] >= 0){
            freeBlockIndex(file->blockPointers[i]);
        }
    }
    free(file);
    printf("File deleted successfully.\n");
}

void rmdirCmd(char *name) {
    if (!isValidName(name)) { printf("Invalid directory name.\n"); return; }
    FileNode *dir = findChild(cwd, name);
    if (!dir) { printf("Directory not found.\n"); return; }
    if (dir->type != DIR_NODE) { printf("Target is not a directory.\n"); return; }
    if (dir->child) { printf("Directory not empty.\n"); return; }
    unlinkFromParent(dir);
    free(dir);
    printf("Directory removed successfully.\n");
}

void lsCmd() {
    if (!cwd->child) {
        printf("(empty)\n");
        return;
    }
    FileNode *cur = cwd->child;
    do {
        printf("%s%s\n", cur->name, cur->type == DIR_NODE ? "/" : "");
        cur = cur->next;
    } while (cur != cwd->child);
}

void cdCmd(char *name) {
    if (!name) {
        printf("Usage: cd <dirname>\n");
        return;
    }
    if (strcmp(name, "..") == 0) {
        if (cwd->parent) {
            cwd = cwd->parent;
        }
        printf("Moved to %s\n", cwd == root ? "/" : cwd->name);
        return;
    }
    if (strcmp(name, "/") == 0) { cwd = root; printf("Moved to /\n"); return; }
    FileNode *dir = findChild(cwd, name);
    if (!dir) { printf("Directory not found.\n"); return; }
    if (dir->type != DIR_NODE) {
        printf("Target is not a directory.\n");
        return;
    }
    cwd = dir;
    printf("Moved to %s\n", cwd == root ? "/" : cwd->name);
}

void pwdCmd() {
    if (cwd == root) { printf("/\n"); return; }
    int depth = 0;
    FileNode *t = cwd;
    while (t && t != root) { ++depth; t = t->parent; }
    char **parts = (char**)malloc(sizeof(char*) * (depth + 1));
    t = cwd;
    int idx = 0;
    while (t && t != root && idx <= depth) {
        parts[idx++] = t->name;
        t = t->parent;
    }
    for (int i = idx - 1; i >= 0; --i) {
        printf("/%s", parts[i]);
    }
    printf("\n");
    free(parts);
}

void dfCmd() {
    int freeCount = countFreeBlocks();
    int used = NUM_BLOCKS - freeCount;
    double usage = (NUM_BLOCKS == 0) ? 0.0 : ((double)used / (double)NUM_BLOCKS) * 100.0;
    printf("Total Blocks: %d\nUsed Blocks: %d\nFree Blocks: %d\nDisk Usage: %.2f%%\n", NUM_BLOCKS, used, freeCount, usage);
}

void exitCmd() {
    if (root) {
        while (root->child) {
            FileNode *c = root->child;
            unlinkFromParent(c);
            freeAllNodes(c);
        }
        free(root);
        root = NULL;
        cwd = NULL;
    }
    cleanupFreeList();
    printf("Memory released. Exiting program...\n");
    exit(0);
}

int main() {
    initFreeList();
    root = createNode("/", DIR_NODE);
    root->parent = NULL;
    cwd = root;
    printf("Compact VFS - ready. Type 'exit' to quit.\n");
    char input[INPUT_BUF];
    while (1) {
        printf("%s > ", cwd == root ? "/" : cwd->name);
        if (!fgets(input, sizeof(input), stdin)) { exitCmd(); }
        size_t L = strlen(input);
        while (L > 0 && (input[L-1] == '\n' || input[L-1] == '\r')) { input[--L] = '\0'; }
        if (L == 0) continue;
        char cmd[64], a1[MAX_NAME_LEN + 1], content[INPUT_BUF];
        if (sscanf(input, "mkdir %50s", a1) == 1) mkdirCmd(a1);
        else if (sscanf(input, "create %50s", a1) == 1) createCmd(a1);
        else if (sscanf(input, "write %50s \"%[^\"]\"", a1, content) == 2) writeCmd(a1, content);
        else if (sscanf(input, "read %50s", a1) == 1) readCmd(a1);
        else if (sscanf(input, "delete %50s", a1) == 1) deleteCmd(a1);
        else if (sscanf(input, "rmdir %50s", a1) == 1) rmdirCmd(a1);
        else if (strcmp(input, "ls") == 0) lsCmd();
        else if (sscanf(input, "cd %50s", a1) == 1) cdCmd(a1);
        else if (strcmp(input, "pwd") == 0) pwdCmd();
        else if (strcmp(input, "df") == 0) dfCmd();
        else if (strcmp(input, "exit") == 0) exitCmd();
        else printf("Unknown or invalid command.\n");
    }
    return 0;
}
