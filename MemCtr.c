#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

typedef unsigned int uint;

typedef struct MemHeader *HPtr; 

struct MemHeader {
  HPtr ptr;
  uint size;
};

struct MemHeader *base;
uint HeaderSize;
char *prep;

uint morecore(uint need) {
	if (need < 4096) need = 4096;
  char *p = sbrk(need);
  if ((int)p < 0) return 0;
  return need;
}

int init() {
  HeaderSize = sizeof(struct MemHeader);	
  prep = base = sbrk(0);
  int ret = morecore(4096);
  if (ret > 0) {
    base->ptr = base + 1;
    base->size = 0;
    base->ptr->ptr = NULL;
    base->ptr->size = ret - HeaderSize * 2;
    return 0;
  }
  return -1;
}

void fragment() {
  struct MemHeader* i;
  for (i = base;i->ptr != NULL;i = i->ptr) {
    if (i!=base && (((void*)i) + HeaderSize + i->size == ((void*)i->ptr))) {
      i->size += i->ptr->size + HeaderSize;
      i->ptr = i->ptr->ptr;
    }
  }
}

void *umalloc(uint mem) {
  struct MemHeader* i,*pre = NULL;
  struct MemHeader* block = NULL,*preb = NULL;
  for (i = base;i != NULL;i = i->ptr) {
    if (i->size >= mem) {
      if (block == NULL) block = i,preb = pre;
      else if (block->size > i->size)
        block = i,preb = pre;
    }
    if (i->ptr == NULL && block == NULL) {
      i->size += morecore(mem - i->size);
      if (i->size >= mem) block = i,preb = pre; 
    }
    pre = i;
  }
  if (block == NULL) return NULL;
  if (block->size <= HeaderSize + mem) {
    preb->ptr = block->ptr;
    return ((void*)block) + HeaderSize;
  }
  struct MemHeader* newHeader = (struct MemHeader*)(((void*)block) + block->size - mem);
  newHeader->ptr = NULL;
  newHeader->size = mem;
  block->size -= mem + HeaderSize;
  return ((void*)newHeader) + HeaderSize;
}

void ufree(void *p) {
  struct MemHeader* i;
  struct MemHeader *header = (struct MemHeader*)(p - HeaderSize);
  for (i = base;i != NULL;i = i->ptr) {
    if (i->ptr == NULL || ((void*)i->ptr) > p) {
      header->ptr = i->ptr;
      i->ptr = header;
      return;
    }
  }
  fragment();
}

int main() {
  init();
  char *a,*b,*c;
  a = umalloc(100);
  b = umalloc(400);
  c = umalloc(50);
  ufree(b);
  b = umalloc(1000);
  puts("ok");
  return 0;
}

