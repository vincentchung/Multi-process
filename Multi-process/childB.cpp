//
//  childB.cpp
//  Multi-process
//
//  Created by VincentChung on 2016/8/29.
//  Copyright © 2016年 vincent. All rights reserved.
//

#include <stdio.h>
#include <math.h>
#include<iostream>
#include <sys/shm.h>
#include <stdlib.h>
#include "childB.hpp"


using namespace std;

int* shm;

float standard_deviation(int data[], int n);
void quicksort(int *data, int left, int right);

void processB(int shm_array_id)
{
    
    int arrayB[SHM_NUM_LIMIT]={0};
    int flag=0;
    int counting_elements=0;
    cout << "[childB] Child process started"<<endl;
    while(1)
    {
        //checking the index.. if it's different that's mean parent got new update from user input
        shm = (int*)shmat(shm_array_id, 0, 0);
        if(shm[0]!=flag)
        {
            if(shm[1]==-1)
                exit(0);
            
            cout << "[childB] Random Numbers Received:";
            for(int i=1;i<SHM_NUM_LIMIT;i++)
            {
                if(shm[i]!=0)
                {
                    cout<< shm[i] << " ";
                    arrayB[i-1]=shm[i];
                    counting_elements++;
                }else
                {
                    break;
                }
            }
            cout<<endl;
            //do the sorting
            quicksort(arrayB, 0, counting_elements-1);
            cout<<"[childB] Sorted Sequence:";
            for(int i=0;i<counting_elements;i++)
                cout<< arrayB[i] << " ";
            
            cout<<endl;
            //computing standard_deviation!!
            cout<<"[childB] Standard Deviation:"<<standard_deviation(arrayB,counting_elements)<<endl;
            counting_elements=0;
            flag++;
        }
        shmdt(shm);
        
    }

}

float standard_deviation(int data[], int n)
{
    float mean=0.0, sum_deviation=0.0;
    int i;
    for(i=0; i<n;++i)
    {
        mean+=(float)data[i];
    }
    mean=mean/n;
    for(i=0; i<n;++i)
        sum_deviation+=((float)data[i]-mean)*((float)data[i]-mean);
    return sqrt(sum_deviation/n);
}

void qkswap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
    
}

void quicksort(int *data, int left, int right)
{
    int pivot, i, j;
    
    if (left >= right) { return; }
    
    pivot = data[left];
    
    i = left + 1;
    j = right;
    
    while (1)
    {
        while (i <= right)
        {
            if (data[i] > pivot)
            {
                break;
            }
            
            i = i + 1;
        }
        
        while (j > left)
        {
            if (data[j] < pivot)
            {
                break;
            }
            
            j = j - 1;
        }
        
        if (i > j) { break; }
        
        qkswap(&data[i], &data[j]);
        
    }
    
    qkswap(&data[left], &data[j]);
    
    quicksort(data, left, j - 1);
    quicksort(data, j + 1, right);
}
