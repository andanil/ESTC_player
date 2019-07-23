#ifndef SONGLIST_H
#define SONGLIST_H
#include "ff.h"
#include <stdlib.h>

struct List
{
  FILINFO file;
  struct List *next;
};
struct List *add(struct List *last, FILINFO data);

#endif /* SONGLIST_H */