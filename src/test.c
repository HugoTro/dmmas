#include "dmmas.h"
#include <stdio.h>
#include <time.h>

const uint64_t planned_n = 500;
const uint64_t initial_size = 4;


uint64_t random(int32_t t) {
    static uint64_t random_n = 0;
    if (!random_n) {
        random_n = (t<<5)*4578934;
    }
    else {
        random_n += (random_n<<5)*4578934;
    }
    return random_n;
}

int main() {
    struct timespec ts;
    uint64_t count = 0;
    int32_t state = -1;
    struct ElementInfo *array = createEmptyDynamicArray(&state);
    // printf("\nState after array creation: %d\n\tNumber of elements: %llu\n", state, count);
    // if (state==1) {
    //     printf("Allocation failed.");
    //     return 1;
    // }
    // state = -1;
    // array = addElement(array, &count, 64, &state);
    // printf("\nState after element creation: %d\n\tNumber of elements: %llu\n", state, count);
    // state = -1;
    // removeElement(array, &count, 456, &state);
    // state = -1;
    // uint64_t a = 0;
    // array = addElementOptimized(array, &count, 789, &a, &state);
    // state = -1;
    // printf("State after object addition: %d\n\tNumber of elements: %llu\n\tElement ID: %llu\n\tArray ptr: %p\n", state, count, a, array);
    // deleteDynamicArray(array, &count, &state);
    // printf("State after array deletion: %d\n\tNumber of elements: %llu\n", state, count);
    // state = -1;
    timespec_get(&ts, TIME_UTC);
    int64_t actual_number = (random(ts.tv_nsec)%planned_n)+1;
    for (uint64_t i = 0; i<actual_number; i++) {
        array = addElementOptimized(array, &count, (ts.tv_nsec%21359)/10, NULL, &state);
        if (state) {
            printf("Addition of element %llu failed with error code %d\n", i, state);
        }
        state = -1;
    }
    uint64_t arr_size = 0;
    for (uint64_t i = 0; i<getArrayLength(); i++) {
        arr_size+=(array+i)->element_size;
    }
    printf("Initial conditions:\n");
    printf("\tSize of the array: %f KiB\n\tElements: %llu\n\tSize of the information array: %f KiB\n", arr_size/1024.0, getArrayLength(), (getArrayLength()*sizeof(struct ElementInfo))/1024.0);
    if (actual_number>50) {
        for (int i = 0; i<25; i++) {
            removeElement(array, &count, 20+i, &state);
            state = -1;
        }
        arr_size = 0;
        for (uint64_t i = 0; i<getArrayLength(); i++) {
            arr_size+=(array+i)->element_size;
            if ((array+i)->element_size>5000) {
                printf("%llu, %llu", i, (array+i)->element_size);
            }
        }
        printf("Array size: %llu, Number of elements: %llu\n", getArrayLength(), count);
        addElement(array, &count, 1048576, &state);
        state = -1;
        printf("After addition of 1MiB object:\n");
        arr_size = 0;
        for (uint64_t i = 0; i<getArrayLength(); i++) {
            arr_size+=(array+i)->element_size;
            if ((array+i)->element_size>5000) {
                printf("ID: %llu, Size: %llu\n", i, (array+i)->element_size);
            }
        }
        printf("\tSize of actual array: %f KiB\n\tElements: %llu\n\tSize of the information array: %f KiB\n", arr_size/1024.0, getArrayLength(), (getArrayLength()*sizeof(struct ElementInfo))/1024.0);
        printf("\tArray size: %llu\n\tNumber of elements: %llu\n", getArrayLength(), count);
    }
    deleteDynamicArray(array, &count, &state);
    // printf("Array deleted, %llu elements.\n", count);
    return 0;
}