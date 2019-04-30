//
//  main.c
//  fork scheduling
//
//  Created by user on 2019/4/22.
//  Copyright © 2019 jimmy. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/syscall.h>
#define GET_TIME 314
#define PRINTK 315

#define w 10000
#define critical 487630

#define UNIT_T()                \
{                        \
volatile unsigned long i;        \
for (i = 0; i < 1000000UL; i++);    \
}          \



typedef struct process{
    char name[32];
    int num;
    int ready;
    int exe;
    pid_t pid;
}proc;

int cmp1(const void *a, const void *b) {
    return ((struct process *)a)->ready - ((struct process *)b)->ready;
}

int cmp2(const void *a, const void *b) {
    return ((struct process *)a)->exe - ((struct process *)b)->exe;
}

int exec(struct process proc,char command[50]){
    int pid = fork();
    
    if (pid < 0) {
        perror("fork error");
        return -1;
    }
    if (pid == 0) {
        unsigned long start_sec, start_nsec, end_sec, end_nsec;
        char to_dmesg[200];
        clock_t start_t, end_t;
        start_t = clock();
        for (int con1 = 0; con1 < proc.exe*10; con1++) {
            UNIT_T();
        }
        end_t = clock();
        if(strcmp(command,"dmesg")==0)
            printf("[project1] %d %ld %ld\n", getpid(), start_t,end_t);
        exit(0);
    }
    return pid;
}

int main(int argc, char *argv[]){
    char pattern[5];
    int n,ready_hash[w],exe_hash[w],con1,t=0;
    struct process pr1[w],pr2[w],schedule[w];
    scanf("%s",pattern);
    scanf("%d",&n);
    if(strcmp(pattern,"SJF")!=0){
        perror("This should be SJF");
    }
    for(con1=0;con1<n;con1++){
        scanf("%s %d %d",pr1[con1].name,&pr1[con1].ready,&pr1[con1].exe);
        strcpy(pr2[con1].name,pr1[con1].name);
        pr2[con1].ready=pr1[con1].ready;
        pr2[con1].exe=pr1[con1].exe;
        pr1[con1].num=con1;
        pr2[con1].num=con1;
    }
    qsort(pr1,n,sizeof(struct process),cmp1);
    qsort(pr2,n,sizeof(struct process),cmp2);
    for(con1=0;con1<n;con1++){
        ready_hash[pr1[con1].num]=con1;
        exe_hash[pr2[con1].num]=con1;
    }
    schedule[0].num=pr1[0].num;
    schedule[0].ready=pr1[0].ready;
    schedule[0].exe=pr1[0].exe;
    strcpy(schedule[0].name,pr1[0].name);
    pr2[exe_hash[pr1[0].num]].ready=critical;
    t+=pr1[0].ready;
    t+=pr1[0].exe;
    int count=1;
    int if_sched=-1;
    while(count<=n-1){
        if_sched=-1;
        for(con1=0;con1<n;con1++){
            if(pr2[con1].ready<=t){
                if_sched=1;
                schedule[count].num=pr2[con1].num;
                schedule[count].ready=pr2[con1].ready;
                schedule[count].exe=pr2[con1].exe;
                strcpy(schedule[count].name,pr2[con1].name);
                t+=pr2[con1].exe;
                pr2[con1].ready=critical;
                count++;
                break;
            }
        }
        if(if_sched==-1){
            t++;
        }
    }
    
    int run=-1;
    int real_time=0;
    int next_time=0;
    count=0;
    
    while(count<=n-1){
        if(real_time>=schedule[count].ready && run==-1){
            schedule[count].pid = exec(schedule[count],argv[0]);
            run=1;
            next_time=real_time+schedule[count].exe;
        }
        if(real_time==next_time && run==1){
            waitpid(schedule[count].pid, NULL, 0);
            printf("%s %d\n", schedule[count].name, schedule[count].pid);
            count++;
            run=-1;
        }
        UNIT_T();
        real_time++;
    }
    return 0;
}


