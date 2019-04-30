//
//  main.c
//  fork scheduling
//
//  Created by user on 2019/4/22.
//  Copyright © 2019 jimmy. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#define GET_TIME 314
#define PRINTK 315

#define w 100000
#define critical 487630

#define UNIT_T()                \
{                        \
volatile unsigned long i;        \
for (i = 0; i < 1000000UL; i++);    \
}                        \

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

int wakeup(int pid)
{
    struct sched_param param;
    
    
    param.sched_priority = 0;
    
    int ret = sched_setscheduler(pid, SCHED_OTHER, &param);
    
    if (ret < 0) {
        perror("sched_setscheduler");
        return -1;
    }
    
    return ret;
}

int stop(int pid)
{
    struct sched_param param;
    
    
    param.sched_priority = 0;
    
    int ret = sched_setscheduler(pid, SCHED_IDLE, &param);
    
    if (ret < 0) {
        perror("sched_setscheduler");
        return -1;
    }
    
    return ret;
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



int main(int argc, char *argv[]) {
    char pattern[5];
    int n,con1,time=0;
    struct process pr1[w],pr2[w],schedule[w];
    scanf("%s",pattern);
    scanf("%d",&n);
    for(con1=0;con1<n;con1++){
        scanf("%s %d %d",pr1[con1].name,&pr1[con1].ready,&pr1[con1].exe);
        strcpy(pr2[con1].name,pr1[con1].name);
        pr2[con1].ready=pr1[con1].ready;
        pr2[con1].exe=pr1[con1].exe;
        pr1[con1].num=con1;
        pr2[con1].num=con1;
    }
    qsort(pr1,n,sizeof(struct process),cmp1);
    
    
    int run=-1;
    int ready_n=0;
    int real_time=0;
    int count=0;
    for(con1=0;con1<n;con1++){
        printf("%d is %s ",(con1+1),schedule[con1].name);
    }
    printf("\n");
    
    while(count<=n-1){
        if(run!=-1 && pr1[run].exe==0){
            waitpid(pr1[run].pid, NULL, 0);
            printf("%s %d\n", pr1[run].name, pr1[run].pid);
            pr1[run].exe=critical;
            run = -1;
            count++;
        }
        for(con1=ready_n;con1<n;con1++){
            if(pr1[con1].ready<=real_time){
                exec(pr1[con1],argv[0]);
                block(pr1[con1].pid);
                ready_n++;
            }
        }
        for(con1=0;con1<ready_n;con1++){
            if(pr1[con1].exe<pr1[run].exe && run!=-1){
                block(pr1[run].pid);
                wakeup(pr1[con1].pid);
                run=con1;
            }
        }
        UNIT_T();
        if(run!=-1){
            pr1[run].exe--;
        }
        real_time++;
    }
    
    return 0;
}


