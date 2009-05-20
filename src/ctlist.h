/************************************************************************
*
* ����������� ���������� ������ ��� ����������� �� ���������
* 20.08.1999 ������� �.�.
*
************************************************************************/

#ifndef CTLIST_H
#define CTLIST_H
#include <stdio.h> // � �������� ������ ��� ����������� NULL

class Tclist {
  public:
    Tclist(int _Limit);
    virtual ~Tclist();
    inline void *At(int _Index) {
      return _Index >= aLimit?NULL:items[_Index];
    };                      //��������� �� ���� �� �������
    inline int IndexOf(void *_Item) {
      for  ( int i=0;i<aLimit;i++ ) {
          if ( At(i)==_Item ) return i;
      }
      return -1;
    };                  //������ �� ���������

    int Insert(void *_Item);                   //�������� ����
    void AtPut(int _Index, void *_Item);       // �������� ���� �� �������

    void Free(void *_Item);                    // ������� ������ � �� ��� ������� ������ NULL
    void Remove(void *_Item);                  // �� ������� ������ � �� ��� ������� ������ NULL
    void AtFree(int _Index);                   // ������� ������ � �� ��� ������� ������ NULL
    void AtRemove(int _Index);                 // �� ������� ������ � �� ��� ������� ������ NULL

    void FreeAll(void);
    void RemoveAll(void);

    void Swap(int _Index1,int _Index2);
    void Assign(Tclist *_oldlist);

    void *NextItem(int _Index);
    void *PrevItem(int _Index);
    void *FirstItem(void);
    void *LastItem(void);

    int aCount;                                //������� ���-�� ���� � ������
    int aLimit;                                //���������� ���-�� ����
    int Check(void);

    void **items;

};


#endif
