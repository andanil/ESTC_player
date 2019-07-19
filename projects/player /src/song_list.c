#include "song_list.h"

struct List *add(struct List *last, FILINFO data)
{
  struct List *current;
  current = (struct List*)malloc(sizeof(struct List));
  current->file = data;
  current->next = 0;
  if(last != 0)
  {
    last->next = current;
  }
return current;
}