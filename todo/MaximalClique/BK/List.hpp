#ifndef __LIST__
#define __LIST__
#include<vector>
#include<memory>
// #define DEBUG

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
  inline T& operator[](const int id){return list[id];};
  inline T* GetNext(T* pos){return &list[next[pos - list]];};
  inline T* GetPrev(T* pos){return &list[prev[pos - list]];};
  void set(T elem, int id){list[id] = elem;};
  void remove(int id);
  void undo();
  inline int size(){return _size;}
  inline T* end(){return &list[tail];};
  inline T* begin(){return &list[next[tail]];};
  inline bool member(int id){return not removed[id];};
private:
  std::unique_ptr<T[]> list;
  std::unique_ptr<int[]> next, prev;
  std::unique_ptr<bool[]> removed;
  int tail, _size, head = -1;
};


//n-th element is sentinel
template<typename T>
void List<T>::init(std::vector<T> elem){
  _size   = tail = elem.size();
  list    = std::make_unique<T[]>(tail);
  next    = std::make_unique<int[]>(tail + 1);
  prev    = std::make_unique<int[]>(tail + 1);
  removed = std::make_unique<bool[]>(tail);
  for (int i = 0; i < tail; i++) {
    list[i] = elem[i];
    next[i] = i + 1;
    prev[i] = i - 1;
    removed[i] = false;
  }
  next[tail] = 0;
  prev[0] = tail;
}

//n-th element is sentinel
template<typename T>
void List<T>::init(int size) {
  _size   = tail = size;
  list    = std::make_unique<T[]>(tail);
  next    = std::make_unique<int[]>(tail + 1);
  prev    = std::make_unique<int[]>(tail + 1);
  removed = std::make_unique<bool[]>(tail);
  for (int i = 0; i < tail; i++) {
    next[i] = i + 1;
    prev[i] = i - 1;
    removed[i] = false;
  }
  next[tail] = 0;
  prev[0] = tail;
}


template<typename T>
void List<T>::remove(int id){
#ifdef DEBUG
  if(removed[id]){
    printf("%d is already removed.\n", id);
    std::exit(1);
  }
#endif
  removed[id] = true;
  _size--;
  prev[next[id]] = prev[id];
  next[prev[id]] = next[id];
  next[id] = head;
  head = id;
}

template<typename T>
void List<T>::undo(){
#ifdef DEBUG
  if(head == -1){
    printf("stack is empty. ");
    std::exit(1);
  }
#endif
  removed[head] = false;
  _size++;
  int id = head;
  head = next[id];
  next[id] = next[prev[id]];
  prev[next[id]] = id;
  next[prev[id]] = id;
}

#endif // __LIST__
