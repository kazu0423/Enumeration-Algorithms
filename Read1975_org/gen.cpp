#include<iostream>
using namespace std;


int main(){
    int n = 15;
    cout << 2*n + 2 << " " << 4*n+4 << endl;
    cout << "0 1" << endl;
    cout << "0 2" << endl;
    for(int i = 1; i < 2*n-1; i+=2) {
        cout << i << " " << i + 2 << endl;
        cout << i << " " << i + 3 << endl;
        cout << i + 1 << " " << i + 2 << endl;
        cout << i + 1 << " " << i + 3 << endl;
    }
    cout << 2*n-1 << " " << 2*n + 1 << endl;
    cout << 2*n   << " " << 2*n + 1 << endl;
}
