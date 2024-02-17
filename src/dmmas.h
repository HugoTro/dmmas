#ifndef DMMAS_H
#define DMMAS_H
#include <stdint.h>
#include <stdlib.h>

struct ElementInfo {
    void *element_ptr;
    uint64_t id;
    uint64_t element_size;
};

uint64_t getArrayLength();

struct ElementInfo *createDynamicArray(uint64_t, uint64_t, uint64_t *, int32_t *);
struct ElementInfo *createEmptyDynamicArray(int32_t *);
void deleteDynamicArray(struct ElementInfo *, uint64_t *, int32_t *);

struct ElementInfo *addElement(struct ElementInfo *, uint64_t *, uint64_t, int32_t *);
struct ElementInfo *addElementOptimized(struct ElementInfo *, uint64_t *, uint64_t, uint64_t *, int32_t *);
void removeElement(struct ElementInfo *, uint64_t *, uint64_t, int32_t *);

#endif