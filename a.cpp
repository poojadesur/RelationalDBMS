#include <bits/stdc++.h>

using namespace std;

int main(){

    int x = 0;
    int a = 1;

    do {
        cout << "hi" << endl;
        a += 1;
        if(a%5 == 0) x += 1;
    } while((x+1) %2 !=0);

    // vector <vector<vector <int> >> blocks(10, vector<vector <int> >(10, vector<int>(10, 0)));

    // int pageindex = -1;
    // int smmaxsize = 4;
    // int pitrack = 0;
    // int colsize = 9;
    // int rowsize = 9;

    // vector<int> rowCount(10,0);

    // for(int i=0;i<rowsize;i++)
    // {
    //     cout << "page index: " << pageindex << endl;

    //     if(i% smmaxsize == 0) 
    //     {
    //         cout << "i: " << i << " at a new block index, next row" << endl;
    //         pitrack = pageindex;
    //         // rowCount[pageindex] += 1;

    //     }
    //     else {
    //         cout << "at a new row, same block index" << endl;
    //         pageindex = pitrack;
    //         cout<<" pageIndex: "<<pageindex<<" i: "<<i<<" rowCount: "<<rowCount[pageindex]<<endl;
    //     }

    //     int pagerowindex = i % smmaxsize;

    //     for(int j=0; j < colsize;j++)
    //     {
    //         if(j% smmaxsize == 0) {
    //             pageindex++;
    //             rowCount[pageindex] += 1;
    //             cout<<"changing columns, at a new block index, pageIndex:"<<pageindex<<" rowCount:"<<rowCount[pageindex]<<endl;
    //         }
    //         int pagecolindex = j % smmaxsize;
            
    //         blocks[pageindex][pagerowindex][pagecolindex] = pageindex;

    //         cout << pageindex << " ";
    //     }

    //     cout << endl;

    // int num = 0;
    // for(int i=0;i<25;i++) {
    //     for(int j=0;j<25;j++){
    //         cout<<num<<",";
    //         num++;
    //     }
    //     cout<<endl;
    // }
    // }

    // for(int i=0;i<rowCount.size();i++){
    //     cout<<i<<" "<<rowCount[i]<<endl;
    // }
}

    // for(int i=0;i<blocks.size();i++)
    // {
    //     for(int j=0;j<blocks[i].size();j++)
    //     {
    //         for(int k=0;k<blocks[i][j].size();k++)
    //         {
    //             cout<<blocks[i][j][k]<<" ";
    //         }
    //         cout<<endl;
    //     }
    //     cout<<endl;
    // }