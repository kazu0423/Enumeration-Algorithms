// AddibleList, List, FixedStack, FixedQueue
#ifndef __BASICDATASTRUCTRUE__
#define __BASICDATASTRUCTRUE__
#include<vector>
#include<iostream>
#include<memory>

template<typename T>
class AddibleList{
public:
  AddibleList(){};
  AddibleList(std::vector<T> elem){init(elem);};
  AddibleList(int size){init(size);};
  ~AddibleList(){};
  AddibleList(const AddibleList<T> &&x){};
  void init(std::vector<T> elem);
  void init(int size);
  inline T& operator[](const int id){return list[id];};
  inline int GetNext(int id){return next[id];};
  inline int GetPrev(int id){return prev[id];};
  void set(T elem, int id){list[id] = elem;}
  int remove(int id);
  int add(int id);
  void undo();
  inline int size(){return _size;}
  inline int end(){return tail;};
  inline T back(){return list[prev[end()]];};
  inline int begin(){return next[tail];};
  inline bool empty(){return _size == 0;};
  inline bool member(int id){return not removed[id];};
private:
  std::unique_ptr<T[]> list;
  std::unique_ptr<int[]> next, prev, removed;
  int head, _size = 0, tail;
};


//n-th element is sentinel
template<typename T>
void AddibleList<T>::init(std::vector<T> elem){
  tail = elem.size();
  list = std::make_unique<T[]>(tail + 1);
  next   = std::make_unique<int[]>(tail + 1);
  prev   = std::make_unique<int[]>(tail + 1);
  removed = std::make_unique<int[]>(tail + 1);
  
  for (int i = 0; i < elem.size(); i++) {
    list[i] = elem[i];
    removed[i] = -1;
  }
  next[tail] = prev[tail] = tail;
}


//n-th element is sentinel
template<typename T>
void AddibleList<T>::init(int s) {
  tail = s;
  list = std::make_unique<T[]>(tail + 1);
  next = std::make_unique<int[]>(tail + 1);
  prev = std::make_unique<int[]>(tail + 1);
  removed = std::make_unique<int[]>(tail + 1);
  
  for (int i = 0; i < s; i++) removed[i] = -1;
  next[tail] = prev[tail] = tail;
}


template<typename T>
int AddibleList<T>::add(int id){
#ifdef DEBUG
  if(not removed[id]){
    printf("addible: %d is already added. ", id);
    std::exit(1);
  }
#endif
  _size++;
  next[id] = next[tail];
  prev[next[tail]] = id;
  prev[id] = tail;
  next[tail] = id;
  removed[id] = head;
  head = id;
  return id;
}

template<typename T>
int AddibleList<T>::remove(int id){
#ifdef DEBUG
  if(removed[id]){
    printf("addble: %d is already removed. \n", id);
    std::exit(1);
  }
#endif
  removed[id] = -1;
  _size--;
  prev[next[id]] = prev[id];
  next[prev[id]] = next[id];
  next[id] = head;
  head = id;
  return prev[id];
}

template<typename T>
void AddibleList<T>::undo(){
  int id = head;
#ifdef DEBUG
  if(head == -1){
    printf("addble:stack is empty. ");
    std::exit(1);
  }
#endif
  // removed[id] = not removed[id];
  if(id == next[prev[id]]){//add
    _size--;
    head = removed[id];
    removed[id] = -1;
    next[prev[id]] = next[id];
    prev[next[id]] = prev[id];
  }else{//removed
    _size++;
    head = next[id];
    next[id] = next[prev[id]];
    prev[next[id]] = id;
    next[prev[id]] = id;
  }
}


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
  inline int GetNext(int id){return next[id];};
  inline int GetPrev(int id){return prev[id];};
  void set(T elem, int id){list[id] = elem;};
  int remove(int id);
  void undo();
  inline int size(){return _size;}
  inline int maxSize(){return tail;}
  inline int end(){return tail;};
  inline int begin(){return next[tail];};
  inline T back(){return list[prev[end()]];};
  inline bool member(int id){return not removed[id];};
  inline bool empty(){return _size == 0;};
private:
  std::unique_ptr<T[]> list;
  std::unique_ptr<int[]> next, prev;
  std::unique_ptr<bool[]> removed;
  int tail, _size, head = -1;
};

template<typename T>
class FixedStack{
public:
  FixedStack(){};
  FixedStack(int n):cap(n){stack = std::make_unique<T[]>(n);}
  inline void resize(int n){cap = n, stack = std::make_unique<T[]>(n);}
  inline bool push(T item);
  inline T top();
  inline bool pop();
  inline bool empty(){return s == 0;};
  inline bool clear(){s = 0; return true;};
  inline int size(){return s;};
private:
  std::unique_ptr<T[]> stack;
  int s = 0, cap;
};

template<typename T>
class FixedQueue{
public:
  FixedQueue(){};
  FixedQueue(int n){que = std::make_unique<T[]>(n + 1), cap = n;}
  inline void resize(int n){que = std::make_unique<T[]>(n + 1), cap = n;}
  inline bool push(T item);
  inline T front();
  inline bool pop();
  inline bool empty(){return tail == head;};
  inline bool clear(){tail = head = 0; return true;};
  inline int size(){return (tail - head < 0)?tail + cap - head:tail - head;};
  inline int end(){return tail;}
  inline T operator[](const int p){return que[p];}
private:
  std::unique_ptr<T[]> que;
  int tail = 0, head = 0, cap;
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
  prev[tail] = tail - 1;
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
  for (int i = 0; i <= tail; i++) {
    next[i] = i + 1;
    prev[i] = i - 1;
    removed[i] = false;
  }
  next[tail] = 0;
  prev[0] = tail;
}


template<typename T>
int List<T>::remove(int id){
#ifdef DEBUG
  if(removed[id]){
    printf("%d is already removed.\n", id);
    std::exit(1);
  }
#endif
  if(removed[id])return prev[id];
  removed[id] = true;
  _size--;
  prev[next[id]] = prev[id];
  next[prev[id]] = next[id];
  next[id] = head;
  head = id;
  return prev[id];
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




template<typename T>
bool FixedStack<T>::push(T item){
#ifdef DEBUG
  if(cap == s){
    std::cerr << "FixedStack size is over " << cap << std::endl;
    return false;
  }
#endif
  stack[s++] = item;
  return true;
}

template<typename T>
T FixedStack<T>::top(){
#ifdef DEBUG
  if(s == 0){
    std::cerr << "FixedStack is empty" << std::endl;
    exit(1);
  }
#endif
  return stack[s - 1];
};

template<typename T>
bool FixedStack<T>::pop(){
#ifdef DEBUG
  if(cap == 0){
    std::cerr << "FixedStack size is zero" << std::endl;
    return false;
  }
#endif
  --s;
  return true;
}

template<typename T>
bool FixedQueue<T>::push(T item){
#ifdef DEBUG
  if(cap == size()){
    std::cerr << "FixedQueue size is over " << cap << std::endl;
    return false;
  }
#endif
  que[tail++] = item;
  if(tail >= cap) tail = 0;
  return true;
}

template<typename T>
T FixedQueue<T>::front(){
#ifdef DEBUG
  if(empty()){
    std::cerr << "FixedQueue is empty" << std::endl;
    exit(1);
  }
#endif
  return que[head];
};

template<typename T>
bool FixedQueue<T>::pop(){
#ifdef DEBUG
  if(empty()){
    std::cerr << "FixedQueue size is zero" << std::endl;
    return false;
  }
#endif
  if(++head >= cap) head = 0;
  return true;
}

#endif // __BASICDATASTRUCTRUE__
