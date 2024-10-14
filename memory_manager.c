#include "memory_manager.h"

typedef struct memblock_ {
    void* address;
    size_t size;
    bool is_free;
    struct memblock_* next;
} memblock_;

static void *memory_ = NULL;
static memblock_* metadata_ = NULL;

//helper functions

memblock_* is_valid_block(void *block) {
    //checks if the block mathces any existing memory block.
    //if it does, returns that metadata
    if(block == NULL){
        return NULL;
    }
    memblock_ *current = metadata_;

    while(current != NULL) {
        if(current->address == block) {
            return current;
        }
        current = current->next;
    }
    //if it doesnt, returns NULL
    return NULL;
}

memblock_* setblock(size_t size, bool is_free, void *address, memblock_ *next) {
    //"constructor" for memblock_
    memblock_* block = malloc(sizeof(memblock_));
    if (block == NULL) {
        return NULL;
    }
    block->is_free = is_free;
    block->size = size;
    block->address = address;
    block->next = next;
    return block;
}

void free_metadata(memblock_* datablock) {
    //recursively free all memblock_ nodes
    if(datablock->next != NULL) {
        free_metadata(datablock->next);
    }
    free(datablock);
}

//real functions

void mem_init(size_t size) {
    memory_ = malloc(size);
    if (memory_ == NULL) {
        fprintf(stderr, "Failed to allocate memory.\n");
        return;
    }
    
    metadata_ = setblock(
        size,
        true,
        memory_, 
        NULL);
}

void *mem_alloc(size_t size) {
    memblock_ *current = metadata_;

    while (current != NULL) {

        if (current->is_free && current->size >= size) {
            //if we have extra space, lets turn that into a new block

            if (current->size > size) {
                size_t remaining_size = current->size - size;
                void* new_address = (char*)current->address +size;
                memblock_ *new_block = setblock(
                remaining_size,
                true, 
                new_address,
                current->next
                );
                if (new_block == NULL) {
                    return NULL;
                }
                current->size = size;
                current->is_free = false;
                current->next = new_block;
            }
            //this will always happen if we have space
            
            else{
                current->is_free = false;
                }
            return current->address;
        }
        current = current->next;
    }
    return NULL;
}

void mem_free(void *block) {
//sets given memory block as free, and merges it will adjacent free memory blocks, if there are any    

    memblock_ *current = is_valid_block(block);
    if(current == NULL) {
        fprintf(stderr, "Error: Invalid memory block.\n");
        return;
    }
    current->is_free = true;

    memblock_ *previous = metadata_;
    while(previous != NULL && previous->next != current) {
        previous = previous->next;
    }

    if(current->next != NULL && current->next->is_free) {
        memblock_* toremove = current->next;
        current->size += current->next->size;
        current->next = current->next->next;
        free(toremove);
    }

    if(previous != NULL && previous->is_free) {
        previous->size += current->size;
        previous->next = current->next;
        free(current);
    }
}

void *mem_resize(void *block, size_t size) {
    memblock_* current = is_valid_block(block);
    if(current == NULL)
    {
        fprintf(stderr, "Error: Invalid memory block.\n");
        return current;
    }
    //first case: resize to same size
    if(current->size == size) {
        return block;
    }
    //second case: resize to larger size
    if(current->size < size) {
        
        if(current->next != NULL && current->next->is_free && current->next->size + current->size >= size){
            //if size can fit in the current space: just expand it
            memblock_* empty_block = current->next;
            empty_block->size += current->size - size;
            current->size = size;
            empty_block->address = (char*)current->address + current->size;

            return block;
        }
        else {
            //if size cant fit in current space: we have to move the memory
            void *new_block = mem_alloc(size);
            if(new_block != NULL) {
                memcpy(new_block, block, current->size < size ? current->size : size);
                mem_free(block);
                current->is_free = true;

                return new_block;
            }
            return NULL;
        }
    }
    //third case: resize to smaller size
    if(current->size > size){
       //make a new memory block for the free memory, and insert it into the metadata
        memblock_ *new_memblock = setblock(
            current->size - size, 
            true, 
            (char*)current->address + size, 
            current->next);

        current->size = size;
        current->next = new_memblock;
        return block;
    }
    fprintf(stderr, "Error: Not enough memory to resize this block.\n");
    return NULL;

}

void mem_deinit() {
    if(memory_ != NULL){
        free(memory_);
        memory_ = NULL;
        free_metadata(metadata_);
        metadata_ = NULL;
    }
}
