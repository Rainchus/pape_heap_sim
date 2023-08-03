#include "../include/main.h"
u16 heap_nextMallocID = 0;

void* _memcpy(void* s1, const void* s2, u32 n) {
    char* su1 = (char*)s1;
    const char *su2 = (const char*)s2;
    while (n > 0) {
        *su1++ = *su2++;
        n--;
    }
    return (void*)s1;
}

HeapNode* _heap_create(HeapNode* addr, u32 size) {
    if (size < 32) {
        return (HeapNode*)-1;
    } else {
        HeapNode* heapNode = (HeapNode*)ALIGN16((u32)addr);

        size -= ((u8*)heapNode - (u8*)addr);
        heapNode->next = NULL;
        heapNode->length = size - sizeof(HeapNode);
        heapNode->allocated = 0;
        heapNode->capacity = size;
        return heapNode;
    }
}

void* _heap_malloc(HeapNode* head, u32 size) {
    HeapNode* nextHeapNode;
    HeapNode* pPrevHeapNode = NULL;
    u32 newBlockSize;
    u32 curBlockLength;
    HeapNode* curHeapNode;
    u32 smallestBlockFound;
    u16 HeapEntryID;
    u16 HeapEntryID2;

    // must allocate 16 bytes or more at minimum or fail
    size = ALIGN16(size);
    if (!size) {
        return NULL;
    }

    smallestBlockFound = 0;
    nextHeapNode = NULL;

    // find the smallest block we can fit into in the free list
    for (curHeapNode = head; ; curHeapNode = curHeapNode->next) {
        if (!curHeapNode->allocated) {
            curBlockLength = curHeapNode->length;
            if ((curBlockLength >= size) && (curBlockLength < smallestBlockFound || !smallestBlockFound)) {
                pPrevHeapNode = curHeapNode;
                smallestBlockFound = curBlockLength;
                nextHeapNode = curHeapNode->next;
            }
        }
        if (!curHeapNode->next) {
            break;
        }
    }


    // find out the required block size with header
    newBlockSize = size + sizeof(HeapNode);

    // if we found a block see if we need to split it up
    if (smallestBlockFound) {
        if (smallestBlockFound >= newBlockSize) {
            // update previous to the proper size for the block being returned
            pPrevHeapNode->next = (HeapNode*)((u8*)pPrevHeapNode + newBlockSize);
            pPrevHeapNode->length = size;

            // update the entry id on allocation
            HeapEntryID = heap_nextMallocID;
            pPrevHeapNode->allocated = TRUE;
            heap_nextMallocID = HeapEntryID + 1;
            pPrevHeapNode->entryID = HeapEntryID;

            // setup the new heap block entry
            curHeapNode = pPrevHeapNode->next;
            curHeapNode->next = nextHeapNode;
            curHeapNode->length = smallestBlockFound - newBlockSize;
            curHeapNode->allocated = FALSE;
        } else {
            // take this entry out of the free linked list and mark as allocated
            pPrevHeapNode->next = nextHeapNode;
            pPrevHeapNode->length = smallestBlockFound;

            // update the entry id on allocation
            // note, usage of a single ID from above will result in wrong code
            HeapEntryID2 = heap_nextMallocID;
            pPrevHeapNode->allocated = TRUE;
            heap_nextMallocID = HeapEntryID2 + 1;
            pPrevHeapNode->entryID = HeapEntryID2;
        }
        return (u8*)pPrevHeapNode + sizeof(HeapNode);
    }
    return NULL;
}

void* _heap_malloc_tail(HeapNode* head, u32 size) {
    HeapNode* curNode;
    u32 newNodeSize;
    u32 foundNodeLength;
    HeapNode* foundNode;
    HeapNode* nextNode;

    size = ALIGN16(size);
    foundNode = NULL;

    // make sure we have a size to allocate
    if (!size) {
        return NULL;
    }

    foundNodeLength = 0;
    nextNode = NULL;

    // find the smallest block we can fit into
    for (curNode = head; ; curNode = curNode->next) {
        if (!curNode->allocated) {
            if (curNode->length >= size) {
                foundNode = curNode;
                foundNodeLength = curNode->length;
                nextNode = curNode->next;
            }
        }

        if (!curNode->next) {
            break;
        }
    }

    newNodeSize = size + sizeof(HeapNode);
    if (foundNodeLength != 0) {
        curNode = foundNode;

        // we found a block to use, see if we can split it and return a portion
        // or if we just need to return the whole block
        if (foundNodeLength >= newNodeSize) {
            // add a free block before this one
            // this is where this function differs from heap_malloc, it returns
            // the end of the block instead of the beginning when splitting it up
            curNode->next = (HeapNode*)((u8*)curNode + foundNodeLength - size);
            curNode->length = foundNodeLength - newNodeSize;
            curNode->allocated = FALSE;

            curNode = curNode->next;
            curNode->next = nextNode;
            curNode->length = size;
            curNode->allocated = TRUE;

        } else {
            // just return this actual block
            curNode->next = nextNode;
            curNode->length = foundNodeLength;
            curNode->allocated = TRUE;
        }

        return (u8*)curNode + sizeof(HeapNode);
    }

    // did not find a block
    return NULL;
}

u32 _heap_free(HeapNode* heapNodeList, void* addrToFree) {
    u32 curNodeLength;
    HeapNode* nextNode;
    HeapNode* nodeToFreeHeader;
    HeapNode* tempNode;
    HeapNode* outNode;

    // if no address to free then return
    if (addrToFree == NULL) {
        return TRUE;
    }

    // if we are not allocated then ignore this request
    nodeToFreeHeader = (HeapNode*)((u8*)addrToFree - sizeof(HeapNode));
    if (!nodeToFreeHeader->allocated) {
        return TRUE;
    }

    nextNode = nodeToFreeHeader->next;
    curNodeLength = nodeToFreeHeader->length;
    outNode = nextNode;

    // see if the next node after us is allocated, if not then adjust our size
    // to include it and point nextNode to be the node after as it must be allocated
    if (nextNode && !nextNode->allocated) {
        curNodeLength += nextNode->length + sizeof(HeapNode);
        nextNode = nextNode->next;
    }

    // walk the full heap node list looking for the block before our current entry
    tempNode = heapNodeList;
    while (1) {
        // get the pointer to the next block, if it matches the block being freed then
        // exit the search
        heapNodeList = tempNode->next;
        if (heapNodeList == nodeToFreeHeader) {

            // we found the node prior to us, if it is not allocated then adjust our total
            // size to include it and change the header node pointer to point that block
            if (!tempNode->allocated) {
                curNodeLength += sizeof(HeapNode) + tempNode->length;
                nodeToFreeHeader = tempNode;
            }
            break;
        }

        // if the node being freed is before the current node being looked at then we
        // moved past our current node, bail out. Also bail if we hit the end of the list
        if (nodeToFreeHeader < tempNode || !heapNodeList) {
            break;
        }

        // move to the next node
        tempNode = tempNode->next;
    }

    // update the node being free'd with a proper size and pointer to the next node that is
    // allocated
    outNode = nodeToFreeHeader;
    outNode->next = nextNode;
    outNode->length = curNodeLength;
    outNode->allocated = FALSE;
    return FALSE;
}

void* _heap_realloc(HeapNode* heapNodeList, void* addr, u32 newSize) {
    u32 newSizeAligned;
    HeapNode* nextNode;
    HeapNode* curHeapAlloc;
    HeapNode* newFreeBlock;
    u32 newNodeLength;
    HeapNode* nodeToUpdate;

    curHeapAlloc = (HeapNode*)((u8*)addr - sizeof(HeapNode));
    newSizeAligned = ALIGN16(newSize);

    // check if the realloc is on an allocated node otherwise fail
    if (!curHeapAlloc->allocated) {
        return NULL;
    }

    nextNode = curHeapAlloc->next;
    newNodeLength = curHeapAlloc->length;

    // if we have a node after us and it isn't allocated then
    // adjust the length and next node pointers to include the node after us
    nodeToUpdate = nextNode;
    if (nextNode && !nextNode->allocated) {
        nextNode = nextNode->next;
        newNodeLength += sizeof(HeapNode) + nodeToUpdate->length;
    }

    // check if the current block or current + next block (if free) are too small
    nodeToUpdate = curHeapAlloc;
    if (newNodeLength < newSizeAligned) {
        // too small, allocatr a new node, copy data to it then free the current one
        curHeapAlloc = _heap_malloc(heapNodeList, newSizeAligned);
        if (curHeapAlloc == NULL) {
            return NULL;
        }

        // minor interest note, copy the size of the newly allocated size
        // instead of just how much data used to be stored, this results in copying
        // excessive data
        _memcpy(curHeapAlloc, addr, newSizeAligned);
        _heap_free(heapNodeList, addr);
        return curHeapAlloc;
    }

    // see if there is room to add a new free block after us
    if (newSizeAligned + sizeof(HeapNode) < newNodeLength) {
        // room for a free block, create it
        newFreeBlock = (HeapNode*)((u8*)addr + newSizeAligned);

        // update current node
        nodeToUpdate->next = newFreeBlock;
        nodeToUpdate->length = newSizeAligned;

        // create new node after the current one
        nodeToUpdate = newFreeBlock;
        nodeToUpdate->next = nextNode;
        nodeToUpdate->length = (newNodeLength - newSizeAligned) - sizeof(HeapNode);
        nodeToUpdate->allocated = FALSE;
    } else {
        // no room, update our next and length
        nodeToUpdate->next = nextNode;
        nodeToUpdate->length = newNodeLength;
    }

    // return the location we were at
    return addr;
}