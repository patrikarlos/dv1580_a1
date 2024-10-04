#include "memory_manager.h"

struct memblock {
    size_t size;
    bool is_free;
    struct memblock *next;
};

static void *memory_ = NULL;
static struct memblock* metadata_ = NULL;
static size_t memsize_;

void mem_init(size_t size) {
    memory_ = malloc(size);

    if(memory_ == NULL) {
        fprintf(stderr, "Failed to allocate memory.\n");
    }
    
    metadata_ = (struct memblock*)memory_;
    metadata_->size = size - sizeof(struct memblock);
    metadata_->is_free = true;
    metadata_->next = NULL;

}

void *mem_alloc(size_t size) {
    struct memblock *current = metadata_;

    while(current != NULL) {
        if(current->size >= size && current->is_free){
            if(current->size > size ){
                struct memblock *new_block = (struct memblock*)((char*)current + sizeof(struct memblock) + size);

                new_block->size = current->size - size - sizeof(struct memblock);
                new_block->is_free = true;
                new_block->next =  current->next;

                current->size = size;
                current->next = new_block;

            }
            current->is_free = false;

            return (void*)((char*)current + sizeof(struct memblock));
        }
        current = current->next;
    }
    return NULL;
}

void mem_free(void *block) {
    if(!is_valid_block(block)) {
        return;
    }

    struct memblock *current = (struct memblock*)((char*)block - sizeof(struct memblock));
    current->is_free = true;

    struct memblock *previous = metadata_;
    while(previous->next != current && previous != NULL){
        previous = previous->next;
    }

    if(current->next != NULL && current->next->is_free) {
        current->size += current->next->size + sizeof(struct memblock);
        current->next = current->next->next;
    }
    if(previous != NULL && previous->is_free){
        previous->size += current->size + sizeof(struct memblock);
        previous->next = current->next;
    }

}

void *mem_resize(void *block, size_t size) {
    if(!is_valid_block(block))
    {
        return; //will make this into proper error in a bit..
    }

    struct memblock *current = (struct memblock*)(char*)block - sizeof(struct memblock);

    if(current->size == size) {
        return block;
    }

    if(current->size > size) {
        if(current->next != NULL && current->next->is_free && current->next->size + current->size + sizeof(struct memblock) >= size)
        current->size += current->next->size + sizeof(struct memblock);
    }
    if(current->size < size) {

    }
}

void mem_deinit() {
    free(memory_);
}


//helper function(s)

bool is_valid_block(void *block) {

    if(block == NULL){
        return false;
    }
    //if ((char*)block < (char*)memory_ || (char*)block >= (char*)memory_ + memsize_) {
    //    return false;
    //}
    struct memblock *current = metadata_;
    while(current != NULL) {
        if((char*)block == (char*)current + sizeof(struct memblock)) {
            return true;
        }
        current = current->next;
    }
    return false;
}