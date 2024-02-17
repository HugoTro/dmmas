#include "dmmas.h"

uint64_t array_length = 0;

uint64_t getArrayLength() {
    return array_length;
}

struct ElementInfo *createDynamicArray(uint64_t element_size_initial, uint64_t n, uint64_t *element_count, int32_t *state_var) {
    struct ElementInfo *ptr;
    ptr = (struct ElementInfo *)malloc(sizeof(struct ElementInfo)*n);
    if (!ptr) {
        *state_var = 1; // 1: Element info array allocation failed.
        return NULL;
    }
    array_length = n;

    // Element allocation loop
    void *temp_ptr;
    for (uint64_t i = 0; i<n; i++) {
        temp_ptr = malloc(element_size_initial); // Cannot cast to a certain type, as element itself is variable
        if (!temp_ptr) {
            // Free everything, cannot allocate full array
            for (uint64_t j = *element_count; j>0; j--) {
                free((ptr+(j-1))->element_ptr);
                (*element_count)--;
            }
            free(ptr);
            array_length = 0;
            *state_var = 2; // 2: One of the initial elements' allocation has failed.
            return NULL;
        }
        (ptr+i)->element_ptr = temp_ptr;
        (ptr+i)->id = i;
        (ptr+i)->element_size = element_size_initial;
        temp_ptr = NULL;
        (*element_count)++;
    }
    *state_var = 0; // 0: Successful allocation of all memory spaces.
    return ptr;
}

struct ElementInfo *createEmptyDynamicArray(int32_t *state_var) {
    struct ElementInfo *ptr;
    ptr = (struct ElementInfo *)malloc(sizeof(struct ElementInfo));
    if (!ptr) {
        *state_var = 1; // 1: Element info array allocation failed.
        return NULL;
    }
    array_length = 1;
    ptr->element_ptr = NULL;
    ptr->id = 0;
    ptr->element_size = 0;
    *state_var = 0; // 0: Successful allocation of all memory spaces.
    return ptr;
}

void deleteDynamicArray(struct ElementInfo *array_ptr, uint64_t *element_count, int32_t *state_var) {
    if (array_length==0) {
        *state_var = 1; // Array does not exist 
        return;
    }
    for (uint64_t i = array_length; i>0; i--) {
        if ((array_ptr+(i-1))->element_ptr) {
            struct ElementInfo *temp_ptr = array_ptr+(i-1);
            free((array_ptr+(i-1))->element_ptr);
            (*element_count)--;
        }
    }
    free(array_ptr);
    array_length = 0;
    *state_var = 0;
    return;
}

struct ElementInfo *addElement(struct ElementInfo *array_ptr, uint64_t *element_count, uint64_t element_size, int32_t *state_var) {
    struct ElementInfo *new_ptr = (struct ElementInfo *)realloc(array_ptr, (array_length+1)*sizeof(struct ElementInfo));
    if (!new_ptr) {
        *state_var = 1; // Allocation of space for the new ElementInfo array failed, so we pass the old one (still valid with realloc).
        return array_ptr;
    }
    array_length+=1;
    void *obj_ptr = malloc(element_size);
    if (!obj_ptr) {
        *state_var = 2; // Allocation for the actual element has failed.
        return new_ptr; // Is OK because the actual length of the array is not decreased.
    }
    (new_ptr+array_length-1)->element_ptr = obj_ptr;
    (new_ptr+array_length-1)->id = *element_count;
    (new_ptr+array_length-1)->element_size = element_size;
    (*element_count)++;
    *state_var = 0;
    return new_ptr;
}

struct ElementInfo *addElementOptimized(struct ElementInfo *array_ptr, uint64_t *element_count, uint64_t element_size, uint64_t *element_index, int32_t *state_var) {
    struct ElementInfo *new_ptr = NULL;
    uint64_t offset = 0;
    uint8_t space_reused = 0;

    // Search for empty space
    for (uint64_t i = 0; i<array_length; i++) { // Not using element count because we are checking the whole array, including previous failed allocations or deleted elements.
        if (!((array_ptr+i)->element_ptr)) {
            new_ptr = array_ptr;
            offset = i;
            space_reused = 1;
            if (element_index) {
                *element_index = offset;
            }
            break;
        }
    }

    // If there is no free space that we can use we have to realloc().
    if (!new_ptr) {
        new_ptr = (struct ElementInfo *)realloc(array_ptr, (array_length+1)*sizeof(struct ElementInfo)); // Theoretically in this case array_size==*element_count because there is no space in the array.
        if (!new_ptr) {
            *state_var = 1; // Array does not have free space and reallocation has failed.
            return array_ptr;
        }
        array_length+=1;
        offset = *element_count; // Since *element_count is not yet incremented, we can just do that.
        if (element_index) {
            *element_index = offset;
        }
    }
    
    // Element add part
    void *obj_ptr = malloc(element_size);
    if (!obj_ptr) {
        *state_var = 2; // Allocation for the actual element has failed.
        return new_ptr; // will return the array like if upon object deletion -> pointer for element array_ptr+offset will be NULL and array_length will remain unchanged.
    }
    (new_ptr+offset)->element_ptr = obj_ptr;
    if (!space_reused) {
        (new_ptr+offset)->id = *element_count; // Possible since *element_count is not yet incremented.
    }
    (new_ptr+offset)->element_size = element_size;
    (*element_count)++; // Everything is setup, object count is incremented
    *state_var = 0;
    return new_ptr;
}

void removeElement(struct ElementInfo *array_ptr, uint64_t *element_count, uint64_t id, int32_t *state_var) {
    free((array_ptr+id)->element_ptr);
    (array_ptr+id)->element_ptr = NULL;
    (array_ptr+id)->element_size = 0;
    (*element_count)--;
    *state_var = 0;
    return;
}