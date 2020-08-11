#ifndef __LIST__
#define __LIST__
#include<vector>
#include<memory>
// #define DEBUG

template<typename T>
class Element{
public:
  T elem;
  int next, prev;
  bool removed;
};

template<typename T>
class List{
public:
  List(){};
  List(std::vector<T> elem){init(elem);};
  List(int size){init(size);};
  ~List(){};
  List(List&& x){};
  void init(std::vector<T> elem);
  void init(int size);
  inline T& operator[](const int id){return list[id].elem;};
  inline int GetNext(int id){return list[id].next;};
  inline int GetPrev(int id){return list[id].prev;};
  void set(T elem, int id){list[id].elem = elem;};
  int remove(int id);
  int undo();
  inline int size(){return _size;}
  inline bool empty(){return _size == 0;}
  inline int maxSize(){return tail;}
  inline int end(){return tail;};
  inline int begin(){return list[tail].next;};
  inline T back(){return list[list[end()].prev].elem;};
  inline bool member(int id){return not list[id].removed;};
private:
  std::unique_ptr<Element<T>[] > list;
  int tail, _size, head = -1;
};


//n-th element is sentinel
template<typename T>
void List<T>::init(std::vector<T> elem){
  _size   = tail = elem.size();
  list    = std::make_unique<Element<T>[]>(tail + 1);
  for (int i = 0; i < tail; i++) {
    list[i].elem = elem[i];
    list[i].next = i + 1;
    list[i].prev = i - 1;
    list[i].removed = false;
  }
  list[tail].next = 0;
  list[tail].prev = tail - 1;
  list[0].prev = tail;
}

//n-th element is sentinel
template<typename T>
void List<T>::init(int size) {
  _size   = tail = size;
  list    = std::make_unique<Element<T>[]>(tail + 1);
  for (int i = 0; i < tail; i++) {
    list[i].next = i + 1;
    list[i].prev = i - 1;
    list[i].removed = false;
  }
  list[tail].next = 0;
  list[tail].prev = tail - 1;
  list[0].prev = tail;
}


template<typename T>
int List<T>::remove(int id){
#ifdef DEBUG
  if(list[id].removed){
    printf("%d is already removed.\n", id);
    std::exit(1);
  }
#endif
  if(list[id].removed)return list[id].prev;
  list[id].removed = true;
  _size--;
  list[list[id].next].prev = list[id].prev;
  list[list[id].prev].next = list[id].next;
  list[id].next = head;
  head = id;
  return list[id].prev;
}

template<typename T>
int List<T>::undo(){
#ifdef DEBUG
  if(head == -1){
    printf("stack is empty. ");
    std::exit(1);
  }
#endif
  list[head].removed = false;
  _size++;
  int id = head;
  head = list[id].next;
  list[id].next = list[list[id].prev].next;
  list[list[id].next].prev = id;
  list[list[id].prev].next = id;
  return id;
}

#endif // __LIST__
