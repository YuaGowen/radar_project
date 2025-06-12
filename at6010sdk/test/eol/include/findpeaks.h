#ifndef _HEAP_SORT_H_
#define _HEAP_SORT_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint32 value;
	int32 index;
} Peak;

void findTopPeaks(uint32 nums[], int32 n, Peak peaks[]);
void printHeap(Peak heap[], int size);

#ifdef __cplusplus
}
#endif
#endif	//_HEAP_SORT_H_


