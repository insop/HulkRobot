#ifndef _LIST_H_
#define _LIST_H_

//
// This is an implementation of list class based on C++ template
//
// written by Yunho Jeon
// KIST
//

const int ListDefaultBlockSize = 10;

template <class Type>
class list
{
private:
  int numItems;
  int maxItems;
  int blockSize;
  Type **items;

public:

  list(int _blockSize = ListDefaultBlockSize) 		
  { 
    blockSize=_blockSize; 
    numItems=maxItems=0;
    items=(Type **)NULL;
  }

  ~list()			{ free(items); 			}

  Type * operator [](int index)	{ return items[index];		}
  int getNumItems(void)		{ return numItems;		}

  Type *addItem(Type *item)	
  {
    if (numItems>=maxItems) {
      maxItems+=blockSize;
      if (!items)
	items=(Type **)malloc(sizeof(Type *)*maxItems);
      else
	items=(Type **)realloc(items, sizeof(Type *)*maxItems);
    }
    items[numItems++]=item;
    return item;
  }

  void removeItem(int index)
  {
    memcpy(&items[index], &items[index+1], (--numItems)-index);
  }

  void removeItem(Type *item)
  {
    for(int i=0;i<numItems;i++)
      if (items[i]==item) break;
    if (i<numItems) removeItem(i);
  }

  void removeAllItems(void)
  {
    numItems=0;
  }

/*
  void delAllItem(void)
  {
    for(int i=0;i<numItems;i++)
      delete items[i];
    numItems=0;
  }
*/
};

#endif // _LIST_H_
