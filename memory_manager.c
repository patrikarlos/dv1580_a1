#include "memory_manager.h"

static void *virmem_ = NULL;
static size_t memsize_;

void mem_init(size_t size) {

}

void *mem_alloc(size_t size) {

}

void mem_free(void *block) {

}

void *mem_resize(void *block, size_t size) {
    
}

void mem_deinit() {
    free(virmem_);
}
