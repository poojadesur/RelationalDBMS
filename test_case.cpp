#include<bits/stdc++.h>

using namespace std;

int main() {
    int count = 1;
    ofstream myfile;
    myfile.open ("./X.csv");
    cout<<"File 1:";
    for(int i=0;i<100;i++) {
        for(int j=0;j<100;j++) {
            myfile << count << ",";
            count++;
        }
        myfile<<"\n";
        cout<<i<<"\r";
    }

    myfile.close();
    myfile.open("./Y.csv");
    cout<<endl;
    cout<<"File 2:";

    count = 100001;
    for(int i=0;i<100;i++) {
        for(int j=0;j<100;j++) {
            myfile << count << ",";
            count++;
        }
        myfile<<"\n";
        cout<<i<<"\r";
    }
}
