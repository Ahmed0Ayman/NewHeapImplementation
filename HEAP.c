

#include "HEAP.h"

static _Bool Insert_Block(void *block);
static void Heap_Init(void);

typedef struct _Node NodeTypedef_t;

struct _Node {
	NodeTypedef_t *next;
	uint32_t BlockSize;
};

static _Bool INIT_FLAG;                               // flag used as indication as initialized heap
static uint32_t TotalHeapSize[HEAP_REQUIRED_SIZE];   // total heap size static allocation
static NodeTypedef_t FirstNode, EndNode;            // static var. used to control of our heap

/*
 * brief   @ initialize function just called at first time when we deal with the heap
 */
static void Heap_Init(void) {
	NodeTypedef_t *StartBlock = (NodeTypedef_t*) HeapAlignAddress;
	// first initialize first and end node
	FirstNode.BlockSize = 0;
	FirstNode.next = StartBlock;

	EndNode.BlockSize = HeapNewSize;
	EndNode.next = NULL;

	// initialize the heap as a big node in the list
	StartBlock->BlockSize = HeapNewSize;
	StartBlock->next = &EndNode;

	INIT_FLAG = true; // now we will set flag that indicate that  the heap is initialized successfully

}


/*
 * brief   @ this function is the main control fun on our linked list
 * param   @pointer block point to the our freed block
 */
static _Bool Insert_Block(void *block) {
	NodeTypedef_t *Node_Temp = block;
	if ((block == NULL)||(block > (void*)((uint32_t) HeapAlignAddress + HeapNewSize))||block<HeapAlignAddress) // we still need to check if the block address is in heap boundary
		return false;

	NodeTypedef_t *iterator = NULL;
   // find the correct position in the list
	for (iterator = &FirstNode; iterator->next < Node_Temp;
			iterator = iterator->next);

	if (((uint32_t)iterator + iterator->BlockSize) ==(uint32_t) Node_Temp)  // combine the previous block with the current block
			{
		iterator->BlockSize += Node_Temp->BlockSize;
	}
	if (((uint32_t) Node_Temp + Node_Temp->BlockSize) == (uint32_t)iterator->next) // combine the current block with the nwxt block
			{
		Node_Temp->BlockSize += iterator->next->BlockSize;
		Node_Temp->next = iterator->next->next;
		iterator->next = Node_Temp;
	}

	return true;

}

void* MALLOC_FUN(uint32_t size) {
	uint32_t Requiredsize = size + StructNodeSize;
	NodeTypedef_t *iterator = &FirstNode, *ReturnBlock, *NewBlock = NULL;

	// first check if the required size not equal to zero
	if (Requiredsize < 0)
		return NULL;
	// check if heap is still not initialized
	if (INIT_FLAG == false)
		Heap_Init();

	//we need to align our block that we need to generate
	size  += TARGET_ALIGN_SIZE - (size - (size& ~(TARGET_ALIGN_SIZE)));

	// iterate on the list until we get a suitable size
	while (iterator->next->BlockSize < Requiredsize) {
		iterator = iterator->next;
	}

	if(iterator->next == &EndNode)
		return NULL;


	ReturnBlock = iterator->next;

	if (iterator->next->BlockSize > (Requiredsize + MINI_BLOCK_SIZE)) {
		NewBlock = (NodeTypedef_t*)((uint32_t) ReturnBlock + Requiredsize);
		NewBlock->next = ReturnBlock->next;
		NewBlock->BlockSize = ReturnBlock->BlockSize - Requiredsize;

		ReturnBlock->next = NewBlock;
		ReturnBlock->BlockSize = Requiredsize;

	}

	iterator->next = ReturnBlock->next;
	return (void*)((uint32_t) ReturnBlock + StructNodeSize);

}

/* brief   @ this function work as malloc but instead it create a contiguous blocks of memory
 * so we can deal with it as malloc function
 * param   @ size its a size of the variable that need to allocated
 * param   @ NumOfitem number of items that need to be allocated
 * return  @ pointer to the block of memory that allocated
 */
void* CALLOC_FUN(uint32_t NumOfItems, uint32_t size) {
	void *Block = MALLOC_FUN(size * NumOfItems);
	return Block;
}

/* brief   @ this function reallocate a block of memory to bigger or smaller mew size
 * param   @ size var it a size of the new block size that need to allocated
 * param   @ ptr pointer to the old  block of memory that allocated
 * return  @ pointer to the block of memory that allocated
 */
void* REALLOC_FUN(void *ptr, uint32_t size) {
	void *RetunedPtr = NULL;
	NodeTypedef_t *Current_Block, *Next_Block, *New_Block, *iterator = NULL;
	uint32_t  NeededBytes = 0;

	if (ptr == NULL)                     // check if the user pass a null pointer to the function
		return MALLOC_FUN(size);         // new block is generated and returned

	if (size == 0) {                   // free the block of memory and return null
		FREE_FUN(ptr);
		return NULL;
	}

	size  += TARGET_ALIGN_SIZE - (size - (size& ~(TARGET_ALIGN_SIZE)));

	Current_Block = (NodeTypedef_t*)((uint32_t) ptr - StructNodeSize);
	NeededBytes = size - Current_Block->BlockSize + StructNodeSize;
	for (iterator = &FirstNode; iterator->next < Current_Block; iterator =
			iterator->next)
		;
	Next_Block = iterator->next;
	if (((Next_Block) == Current_Block->next)
			&& (Next_Block->BlockSize > (NeededBytes))) // we can extend on the same block
			{
		// now we need to check if we can fragment this block or not
		if (Next_Block->BlockSize
				> (NeededBytes + StructNodeSize + MINI_BLOCK_SIZE)) // we can divide this block
				{
			New_Block = (NodeTypedef_t*)((uint32_t)Next_Block + NeededBytes);
			iterator->next = New_Block;
			New_Block->BlockSize = Next_Block->BlockSize - NeededBytes;
			New_Block->next = Next_Block->next;
			Current_Block->next = New_Block;
			Current_Block->BlockSize += NeededBytes;
			return ptr;
		} else {

			Current_Block->next = Next_Block->next;
			Current_Block->BlockSize += Next_Block->BlockSize;
			return ptr;
		}
	} else {
		RetunedPtr = MALLOC_FUN(size + Current_Block->BlockSize);
		for (uint32_t i = 0; i < Current_Block->BlockSize; i++) // copy the old content into the new location
				{
			*((uint8_t*) ((uint32_t) RetunedPtr + i)) = *((uint8_t*) ptr + i);
		}
		FREE_FUN(ptr);
		return (RetunedPtr);
	}
}

/* brief   @ this function free the block of memory that just allocated
 *
 * param   @ Block is the pointer to the block of memory that created by the MALLOC OR CALLOC
 *
 */
void FREE_FUN(void *Block) {
	/*
	 * first check that this pointer is not null and check that address belong to our heap ;
	 */
	if ((Block == NULL) || (Block < HeapAlignAddress)
			|| (Block > (void*)((uint32_t) HeapAlignAddress + HeapNewSize)))
		return;

	/*
	 * subtract structnodesize to point to the control block of our allocated block
	 */
	void *TotalBlock = (void*)((uint32_t)Block - StructNodeSize);
	/*
	 * return the block to our linked list
	 */
	Insert_Block(TotalBlock);
}
