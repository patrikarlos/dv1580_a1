#include "memory_manager.h"

struct memblock {
    size_t size;
    bool is_free;
    struct memblock *next;
};

static void *memory_ = NULL;
static struct memblock* metadata_ = NULL;
static size_t memsize_;
static size_t totmemused_;

//helper function(s)

bool is_valid_block(void *block) {
    if(block == NULL){
        return false;
    }
    struct memblock *current = metadata_;
    while(current != NULL) {
        if((char*)block == (char*)current + sizeof(struct memblock)) {
            return true;
        }
        current = current->next;
    }
    return false;
}

//real functions

void mem_init(size_t size) {
    if (size <= sizeof(struct memblock)) {
        fprintf(stderr, "Error: Provided size is too small to fit the metadata\n");
        return;
    }

    memory_ = malloc(size + (100*sizeof(struct memblock)));
    if (memory_ == NULL) {
        fprintf(stderr, "Failed to allocate memory.\n");
        return;
    }
    
    metadata_ = (struct memblock*)memory_;

    metadata_->size = size;  
    metadata_->is_free = true;  
    metadata_->next = NULL;     
    memsize_ = size;
    totmemused_ = 0;
}

void *mem_alloc(size_t size) {
    struct memblock *current = metadata_;
    if(totmemused_ + size > memsize_){
        return NULL;
    }


    while (current != NULL) {
        if (current->is_free && current->size >= size + sizeof(struct memblock)) {
            
            if (current->size >= size + 2 * sizeof(struct memblock)) {
                struct memblock *new_block = (struct memblock*)((char*)current + sizeof(struct memblock) + size);

                new_block->size = current->size - size - sizeof(struct memblock);
                new_block->is_free = true;
                new_block->next = current->next;
                
                current->next = new_block;
            }
            
            current->size = size;
            current->is_free = false;
            totmemused_ += size;

            return (void*)((char*)current + sizeof(struct memblock));
        }
        current = current->next;
    }
    return NULL;
}

void mem_free(void *block) {
    if(!is_valid_block(block)) {
        fprintf(stderr, "Error: Invalid memory block.\n");
        return;
    }
    struct memblock *current = (struct memblock*)((char*)block - sizeof(struct memblock));
    printf("in free: Current block size: %zu, is_free: %d\n", current->size, current->is_free);
    totmemused_ -= current->size;
    current->is_free = true;

    struct memblock *previous = metadata_;
    while(previous != NULL && previous->next != current){
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
        fprintf(stderr, "Error: Invalid memory block.\n");
        return NULL;
    }

    struct memblock *current = (struct memblock*)(char*)block - sizeof(struct memblock);
    size_t old_size = current->size;

    if(old_size == size) {
        return block;
    }

    if(old_size < size) {
        if(current->next != NULL && current->next->is_free && current->next->size + current->size + sizeof(struct memblock) >= size){
            
            current->size += current->next->size + sizeof(struct memblock);
            current->next = current->next->next;

            if(current->size > size + sizeof(struct memblock))
            {
                struct memblock *new_memblock = (struct memblock*)((char*)current +sizeof(struct memblock) + size);
                new_memblock->size = current->size - size - sizeof(struct memblock);
                new_memblock->is_free = true;
                new_memblock->next = current->next;

                current->next=new_memblock;
                current->size = size;
            }
            totmemused_ += (current->size - old_size);
            return block;
        }

    }
    if(old_size > size){
        if(old_size > size + sizeof(struct memblock)) {
            struct memblock *new_memblock = (struct memblock*)((char*)current + sizeof(struct memblock)+size);
            new_memblock->size = old_size - size - sizeof(struct memblock);
            new_memblock->is_free = true;
            new_memblock->next = current->next;

            current->next = new_memblock;
        }
        current->size = size;
        totmemused_ -= (old_size -size);
        return block;
    }

    void *new_memblock = mem_alloc(size);
    if(new_memblock != NULL) {
        memcpy(new_memblock, block, old_size < size ? old_size : size);
        mem_free(block);
        return new_memblock;
    }
    fprintf(stderr, "Error: Not enough memory to resize this block.\n");
    return NULL;

}

void mem_deinit() {
    if(memory_ != NULL){
        free(memory_);
        memory_ = NULL;
        metadata_ = NULL;
        memsize_ = 0;
        totmemused_ = 0;
    }

}
