
#ifndef __TABSORT_LIB_H
#define __TABSORT_LIB_H

#include <tab.h>

#ifdef __cplusplus

int TABSORTEDinsert(void*&,int&,int&,int,void*,int,int(*)(const void*,const void*));

template <class T> class TabSorted:public Tab<T>{
public:
  int (*sortfunc) (const T*,const T*);

  TabSorted() {ptr=NULL;total=used=0;sortfunc=NULL;}
  init() {ptr=NULL;total=used=0;sortfunc=NULL;}
  ~TabSorted() {if(ptr) free(ptr);}
  TabSorted(const TabSorted &a) {
    sortfunc=a.sortfunc;
    total=used=0;if(!(ptr=(T*)malloc(a.used*sizeof(T)))) return;
    total=used=a.used;memcpy(ptr,a.ptr,used*sizeof(T));
  }
  // оператор присваивания создает копию массива
  TabSorted& operator =(const TabSorted& a) {
    sortfunc=a.sortfunc;
    if(ptr) free(ptr);
    total=used=0;if(!(ptr=(T*)malloc(a.used*sizeof(T)))) return *this;
    total=used=a.used;memcpy(ptr,a.ptr,used*sizeof(T));
    return *this;
  }
  // добавить элемент
  // возвращает номер добавленного элемента или -1 при ошибке
  int insert(const T &t,int step=8) {
    return TABSORTEDinsert((void*&)ptr,total,used,sizeof(T),(void*)&t,step,
      (int (*) (const void*,const void*))sortfunc);
  }
  // найти элемент
  // возвращает номер элемента или -1, если он не найден
  int find(const T &t) {
    T *p=(T*)bsearch(&t,ptr,used,sizeof(T),(int (*) (const void*,const void*))sortfunc);
    if(!p) return -1;
    return p-ptr;
  }
  int find(int at,int n,const T &t) {
    T *p=(T*)bsearch(&t,ptr+at,n,sizeof(T),(int (*) (const void*,const void*))sortfunc);
    if(!p) return -1;
    return p-ptr;
  }
  void sort() {
    if(sortfunc) Tab<T>::sort(sortfunc);
  }
  void sort(int at,int n) {
    if(sortfunc) Tab<T>::sort(at,n,sortfunc);
  }
};

#endif

#endif
