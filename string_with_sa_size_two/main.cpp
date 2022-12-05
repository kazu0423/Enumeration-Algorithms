#include<iostream>
#include<string>
#include<set>
#include<vector>

std::vector<long long int> ans;

std::pair<std::string,int> parent(std::string s){
    // std::cout << "par:" << s << std::endl;
    int n = s.length();
    if(s[0] == s[1]){
        if(s[0] == 'a') return std::pair<std::string, int>(s.substr(1, n - 1), 0);
        if(s[0] == 'b') return std::pair<std::string, int>(s.substr(1, n - 1), 1);
    }
    if(s[n-2] == s[n-1]) {
        if(s[n-1] == 'a') return std::pair<std::string, int>(s.substr(0, n - 1), 2);
        if(s[n-1] == 'b') return std::pair<std::string, int>(s.substr(0, n - 1), 3);
    }
    return std::pair<std::string, int>(s.substr(1, n - 1), 4);
}

bool isSA(int i, int j, std::string s){
    std::set<std::string> ss;
    for(int i = 0; i < s.length(); i++) {
        for(int j = i + 1; j < s.length(); j++) {
            ss.insert(s.substr(i, j-i));
        }
    }
    for(auto t: ss){
        bool flag = false;
        for(int i = 0; i < s.length() - t.length() and flag == false; i++) {
            for(int j = 0; j < t.length() and flag == false; j++) {
                if(t[j] != s[i + j]) continue;
                if(j == t.length()-1) flag = true;
            }
        }
        if(flag == false) return false;
    }
    return true;
}

bool GammaSize(std::string s){
    if(s.length() <= 2) return true;
    for(int i = 0; i < s.length(); i++) {
        for(int j = i + 1; j < s.length(); j++) {
            if(isSA(i, j, s))return true;
        }
    }
    return false;
}

long long int Enum(std::string s, int len){
    std::cout << "Enum start:" << s << std::endl;
    ans[s.length()] += 1;
    long long int val = 1;
    if(s.length() == 0){
        val += Enum(s+'a', len);
        val += Enum(s+'b', len);
    }else{
        if(s.length() < len and GammaSize('a' + s)){
            std::pair<std::string, int> res = parent('a' + s);
            if((res.second == 0 or res.second == 4) and res.first == s) val += Enum('a' + s, len);
        }
        if(s.length() < len and GammaSize('b' + s)) {
            std::pair<std::string, int> res = parent('b' + s);
            if((res.second == 1 or res.second == 4) and res.first == s) val += Enum('b' + s, len);
        }
        if(s.length() < len and GammaSize(s + 'a')) {
            std::pair<std::string, int> res = parent(s + 'a');
            if(res.second == 2 and res.first == s) val += Enum(s + 'a', len);
        }
        if(s.length() < len and GammaSize(s + 'b')) {
            std::pair<std::string, int> res = parent(s + 'b');
            if(res.second == 3 and res.first == s) val += Enum(s + 'b', len);
        }
    }
    return val;
}

int main(){
    std::string s = "";
    int n;
    std::cin >> n;
    ans.resize(n + 1, 0);
    std::cout << Enum(s, n) << std::endl;
    for(int i = 0; i <= n; i++) {
        std::cout << i << ":" << ans[i] << std::endl;
    }
    // std::cout << parent("aabb") << std::endl;
    // std::cout << parent("abab") << std::endl;
    // std::cout << parent("abbaaa") << std::endl;
    return 0;
}