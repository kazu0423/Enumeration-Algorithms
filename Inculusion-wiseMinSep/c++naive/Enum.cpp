#include<iostream>
#include<vector>
#include<memory>
#include<algorithm>

#include"Enum.hpp"

#define INF 1e9
using bigint = unsigned long long int;

EIMS::EIMS(std::vector<std::vector<int> > H){
    n = H.size();
    cc.resize(n);
    G.resize(n);
    for(int i = 0; i < n; i++) G[i] = H[i];
}

void EIMS::connected_component(std::vector<int> &separator){
    // std::cout << "start connected_component. " << std::endl;
    std::vector<bool> used(n, false);
    for(int v: separator) used[v] = true, cc[v] = -1;
    bool flag = true;
    while(flag){
        flag = false;
        for(int i = 0; i < n; i++) {
            if(used[i] == false) {
                que.push(i);
                used[i] = true;
                cc[i] = i;
                flag = true;
                break;
            }
        }
        while(not que.empty()){
            int v = que.front();
            que.pop();
            for(int u: G[v]) {
                if(used[u]) continue;
                cc[u] = cc[v];
                used[u] = true; 
                que.push(u);
            }
        }
    }
    // std::cout << "end connected_component. " << std::endl;
}

void EIMS::Enumerate(){

    // 極小セパレータの計算 start 
    std::vector<int> separator;
    for(int i = 0; i < n; i++) separator.push_back(i);
    std::cout << "sep size:" << separator.size() << std::endl;
    minimization(separator);
    std::cout << "sep size:" << separator.size() << std::endl;
    // 極小セパレータの計算 end

    std::queue<std::vector<int> > Q;
    Q.push(separator);
    ans.insert(separator);
    while(not Q.empty()){
        // std::cout << "ans size:" << ans.size() << std::endl;
        // std::cout << "hoge" << std::endl;
        separator = Q.front();
        Q.pop();
        // std::cout << "current separator." << std::endl;
        // for(int i = 0; i < separator.size(); i++) {
        //     std::cout << separator[i] << " ";
        // }
        // std::cout << std::endl;
        connected_component(separator);
        // for(int i = 0; i < n; i++) std::cout << cc[i] << " ";
        // std::cout << std::endl;
        
        // 極小セパレータ中の頂点 v を1つ選んで，ある連結成分 C にくっつける．その後，vの隣接点で C に含まれない頂点を全部削除する．
        for(int v: separator) {
            std::vector<bool> new_separator(n, false), tmp, tmp2;
            for(int i = 0; i < separator.size(); i++) {
                // std::cout << separator[i] << " ";
                new_separator[separator[i]] = true;            
            }
            // std::cout << std::endl;
            // std::cout << "hoge" << std::endl;
            new_separator[v] = false;
            std::set<int> cc_id;
            for(int u: G[v]) if(cc[u] != -1) cc_id.insert(cc[u]);
            tmp = new_separator;
            for(int id: cc_id){
                new_separator = tmp;
                for(int u: G[v]){
                    // std::cout << "u:" << u << " cc[" << u << "]:" << cc[u] << std::endl;
                    if(cc[u] != id and cc[u] != -1) new_separator[u] = true;
                }
                std::vector<int> ns;
                for(int i = 0; i < n; i++) if(new_separator[i]) ns.push_back(i);
                if(not is_supersep(ns)) {
                    for(int i = 0; i < n; i++) {
                        ns = std::vector<int>(0);
                        tmp2 = new_separator;
                        new_separator[i] = false;
                        for(int u: G[i]) new_separator[u] = true;
                        for(int i = 0; i < n; i++) if(new_separator[i]) ns.push_back(i);
                        if(not is_supersep(ns)) continue;
                        minimization(ns);
                        if(ans.find(ns) == ans.end()){
                            ans.insert(ns);
                            Q.push(ns);
                        }
                        new_separator = tmp2;
                    }
                }else{
                    minimization(ns);
                    if(ans.find(ns) == ans.end()){
                        ans.insert(ns);
                        Q.push(ns);
                    }
                }
            }
        }
    }
}

// inc. min. sep.の super set になっているかどうかのチェック
bool EIMS::is_supersep(std::vector<int> &separator) {
    // std::cout << "start disconnected." << std::endl;
    // for(int i = 0; i < separator.size(); i++) {
    //     std::cout << separator[i] << " ";
    // }
    // std::cout << std::endl;
    std::vector<bool> used(n, false);
    for(int v: separator) used[v] = true;
    for(int i = 0; i < n; i++) {
        if(not used[i]) {
            que.push(i);
            // std::cout << "i:" << i << std::endl;
            used[i] = true;
            break;
        }
    }
    if(que.size() == 0) return true;
    // std::cout << "hoge" << std::endl;
    int count = 1;
    while(not que.empty()){
        int v = que.front();
        que.pop();
        for(int u: G[v]) {
            // std::cout << "u:" << u << std::endl;
            if(not used[u]){
                // std::cout << "add u:" << u << std::endl;
                used[u] = true;
                que.push(u);
                count++;
            }
        }
    }
    // std::cout << separator.size() << " " << count << std::endl;
    if(separator.size() + count < n) return true;
    // std::cout << "fuga" << std::endl;
    for(int i = 0; i < n; i++) used[i] = false;
    for(int v: separator) used[v] = true;
    for(int v: separator) {
        for(int i = 0; i < G[v].size(); i++) {
            if(not used[G[v][i]]) break;
            if(i == G[v].size() - 1) return true;
        }
    }
    return false;
}

void EIMS::minimization(std::vector<int> &separator){    
    std::vector<int> loop = separator;
    for(int v: loop){
        std::vector<int> tmp;
        for(int u: separator) if(u != v) tmp.push_back(u);
        if(is_supersep(tmp)) separator = tmp;
    }
}

void EIMS::print(){
    std::cout << "ans size:" << ans.size() << std::endl;
    // for (auto v: ans){
    //     for(int i = 0; i < v.size(); i++) {
    //         std::cout << v[i] + 1 << " "; //bit表示の仕方忘れた
    //     }
    //     std::cout << std::endl;
    // }
}