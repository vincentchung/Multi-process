//
//  main.cpp
//  Multi-process
//
//  Created by Vincent Chung on 2016/8/26.
//  Copyright © 2016年 vincent. All rights reserved.
//
#include<iostream>
#include <stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include "childB.hpp"
#include "childA.hpp"
#include <signal.h>
#include <ostream>
namespace Color {
    enum Code {
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_YELLOW   = 33,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49
    };
    class Modifier {
        Code code;
    public:
        Modifier(Code pCode) : code(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const Modifier& mod) {
            return os << "\033[" << mod.code << "m";
        }
    };
}

using namespace std;

//control the number of travel agents - easy to change for test runs
const int NUMBER_OF_CHILDREN = 2;

//required as a paramater for sending ints down a pipe
const int SIZE_OF_INT = sizeof(int);
//the limit of input number
//#define SHM_NUM_LIMIT 64

float standard_deviation(int data[], int n);
void quicksort(int *data, int left, int right);

int main(){
    
    //create 2 pipes to talk from children to parent and parent to children
    //create an int array of size 2, then declared a pipe with the pipe() function
    //using the array as a parameter as so:
    int parent_to_child[2];
    pipe(parent_to_child);
    
    //array of 2 pidIDs.  used for joining threads at end of program
    //pid_t is the variable type needed
    pid_t shut_down[NUMBER_OF_CHILDREN];
    
    int array_num = 0;
    int *array=NULL;
    //share memory
    int shm_array_id=-1;//for the array size
    int *shm=NULL;
    int input_counter=0;
    
    shm_array_id = shmget(IPC_PRIVATE, SHM_NUM_LIMIT*sizeof(int), IPC_CREAT | 0666);
    shm = (int*)shmat(shm_array_id, 0, 0);
    for(int i=0;i<SHM_NUM_LIMIT;i++)
        shm[i]=0;
    
    shmdt(shm);
    int pid;
#if 1
    //prcess A
    {
        pid = fork();
        if (pid == 0 )
        {
         processA(parent_to_child[0]);
        }else
        {
            //else
            {
                //build array of pids to wait for them to die
                shut_down[0] = pid;
            }
        }
        
    }
    
#endif
    
    //process B
    pid = fork();
    if (pid == 0 )
    {
        processB(shm_array_id);
    }else
    {
        //else
        {
            //build array of pids to wait for them to die
            shut_down[1] = pid;
        }
    }
    
    if (pid>0){ //parent and control function
        bool loop = true;
        //in a loop
        while (1){
            int user_input=0;
            int i=0;
            
            Color::Modifier yellow(Color::FG_YELLOW);
            Color::Modifier def(Color::FG_DEFAULT);
            //cout << "This ->" << yellow << "word" << def << "<- is red." << endl;
            
            cout<< yellow<<"[mainParent] Enter a positive integer or ­1 to exit:"<<def<<endl;
            //scanf("%d" , &user_input);
            //cout<< yellow;
            cin>>user_input;
            //cout<< def;
            //cout<<def<<endl;
            array_num=user_input;
            if(array_num==-1)
            {
                cout<<"exiting..."<<endl;
                if(array!=NULL)
                    free(array);
                
                array_num=-1;
                write(parent_to_child[1], &(array_num), SIZE_OF_INT);
                
                shm = (int*)shmat(shm_array_id, 0, 0);
                input_counter++;
                shm[0]=input_counter;
                shm[1]=-1;
                shmdt(shm);
                
                for (int i = 0; i < NUMBER_OF_CHILDREN; i++)
                {
                    waitpid(shut_down[i], NULL, 0);
                    //kill(shut_down[i],SIGKILL);
                }
                break;
            }
            //array_num>(SHM_NUM_LIMIT-1)
            
            if(array!=NULL)
                free(array);
            cout<< yellow<<"[mainParent] Generating "<<array_num<<" random integers:";
            array=(int*)malloc(sizeof(int)*array_num);
            write(parent_to_child[1], &array_num, SIZE_OF_INT);
            
            for( i=0;i<array_num;i++)
            {
                array[i]=rand() % 50 + 1;
                cout<<array[i] <<" ";
            }
            cout<<def<<endl;
            
            for( i=0;i<array_num;i++)
            {
                write(parent_to_child[1], &(array[i]), SIZE_OF_INT);
            }
        //for process B
            //using sharememory to pass the data
            //data array  format: [index][.....array element.......]
            input_counter++;
            shm = (int*)shmat(shm_array_id, 0, 0);
            
            shm[0]=input_counter;
            for(i=1;i<=array_num;i++)
            {
                shm[i]=array[i-1];
            }
            
            for(i=(array_num+1);i<SHM_NUM_LIMIT;i++)
            {
                shm[i]=0;
            }

            shmdt(shm);
        }
        
        
        wait(NULL);
    }
    
    
    cout << "There will be 3 of us if we did.  PID: "  << getpid() << endl;
    if (pid>0)
        cout << "process cleaned" << endl;
    
    return 0;
}


