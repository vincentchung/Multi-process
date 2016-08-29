//
//  childA.cpp
//  Multi-process
//
//  Created by 鐘淳文 on 2016/8/29.
//  Copyright © 2016年 vincent. All rights reserved.
//

#include "childA.hpp"
#include<iostream>
#include<sys/types.h>
#include<unistd.h>
#include <stdlib.h>

using namespace std;

void processA(int parent_to_child)
{
    //1st child using pipe to communication!!
    cout << "[childA] Child process started"<<endl;
    while(1)
    {
        int array_num;
        //if(i==0)
        {
            int count=0;
            
            //using pipe
            read(parent_to_child, &array_num, sizeof(int));
            if(array_num==-1)
                exit(0);
            
            //cout << "child "<<i<<" get the num of array: " << array_num << endl;
            cout << "[childA] Random Numbers Received:";
            for(int j=0;j<array_num;j++)
            {
                int temp;
                read(parent_to_child, &temp, sizeof(int));
                
                cout<< temp << " ";
                count+=temp;
            }
            cout << endl;
            float av=(float)count/(float)array_num;
            cout << "[childA] Average:"<<av<<endl;
            count=0;
            
        }
    }
}