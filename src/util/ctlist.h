/************************************************************************
*
* собственная реализация списка вне зависимости от библиотек
* 20.08.1999 Азарный И.В.
*
************************************************************************/

#ifndef CTLIST_H
#define CTLIST_H
#include <stdio.h> // в принципе только для определения NULL

class Tclist {
  public:
    Tclist(int _Limit);
    virtual ~Tclist();
    inline void *At(int _Index) {
      return _Index >= aLimit?NULL:items[_Index];
    };                      //Указатель на элем по индексу
    inline int IndexOf(void *_Item) {
      for  ( int i=0;i<aLimit;i++ ) {
          if ( At(i)==_Item ) return i;
      }
      return -1;
    };                  //Индекс по указателю

    int Insert(void *_Item);                   //вставить элем
    void AtPut(int _Index, void *_Item);       // вставить элем по индексу

    void Free(void *_Item);                    // удаляет объект и по его индексу ставит NULL
    void Remove(void *_Item);                  // НЕ удаляет объект и по его индексу ставит NULL
    void AtFree(int _Index);                   // удаляет объект и по его индексу ставит NULL
    void AtRemove(int _Index);                 // НЕ удаляет объект и по его индексу ставит NULL

    void FreeAll(void);
    void RemoveAll(void);

    void Swap(int _Index1,int _Index2);
    void Assign(Tclist *_oldlist);

    void *NextItem(int _Index);
    void *PrevItem(int _Index);
    void *FirstItem(void);
    void *LastItem(void);

    int aCount;                                //Текущее кол-во элем в списке
    int aLimit;                                //Предельное кол-во элем
    int Check(void);

    void **items;

};


#endif
