#include "galois/runtime/SyncStructures.h"

GALOIS_SYNC_STRUCTURE_REDUCE_ADD(nout, unsigned int);
GALOIS_SYNC_STRUCTURE_BROADCAST(nout, unsigned int);
GALOIS_SYNC_STRUCTURE_BITSET(nout);

GALOIS_SYNC_STRUCTURE_REDUCE_ADD(partialSum, float);
GALOIS_SYNC_STRUCTURE_BROADCAST(partialSum, float);
GALOIS_SYNC_STRUCTURE_BITSET(partialSum);
