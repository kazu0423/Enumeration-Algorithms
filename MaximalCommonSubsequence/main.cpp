//////////////////////////////////////
/*                                  */
/* Maximal Common Subseqeunce Enum. */
/* PrefixSpan base?                 */
/* kurita                           */
/*                                  */
//////////////////////////////////////

#include<iostream>
#include<vector>
#include<fstream>
#include<chrono>
// #define DEBUG


int main(int argc, char *argv[]){
  bool m_flag = false; // for maximal common subsequence enumeration
  bool c_flag = false; // for closed common subsequence enumeration
  if(argc != 2){
    std::cerr << "Error : The number of input file is not 2" <<std::endl;
    return 0;
  }
  
  std::ifstream ist(argv[1], std::ios::in);
  if(!ist){
    std::cerr << "can't open input file: " << argv[1] << std::endl;
    return 0;
  }
  int n, t, cnt = 0;
  std::string tmp;
  getline(ist, tmp);
  sscanf(tmp.data(), "%d %d", &n, &t);
  std::vector<std::string> S;
  while(getline(ist, tmp)){
    std::cout << tmp << std::endl;
    S.push_back(tmp);
  }
  
  auto start = std::chrono::system_clock::now();
  std::cout << n << " " << t << std::endl;
  for(int i = 0; i < n; i++) {
    std::cout << S[i] << std::endl;
  }
  auto end = std::chrono::system_clock::now();
  auto diff = end - start;
  std::cout << "elapsed time = "
            << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count()
            << " msec."
            << std::endl;  
  // eds.print();
  // std::cout << "time:"
  //           << std::chrono::duration_cast<std::chrono::milliseconds>(eds.time).count()
  //           << std::endl;
  return 0;
}
