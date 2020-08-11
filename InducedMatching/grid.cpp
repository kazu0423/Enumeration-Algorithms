#include<iostream>
using namespace std;

int main(){
  int n;
  std::cin >>  n;
  std::cout << n*n << std::endl;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n - 1; j++) {
      std::cout << i*n + j << " " << i*n + j + 1 << std::endl;
    }
    for (int j = 0; j < n and i < n - 1; j++) {
      std::cout << n*i + j << " " << n*(i + 1) + j << std::endl;
    }
  }
  return 0;
}
