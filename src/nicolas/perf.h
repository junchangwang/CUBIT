/* ---------------------------------------------------------------------------
 ** This software is in the public domain, furnished "as is", without technical
 ** support, and with no warranty, express or implied, as to its usefulness for
 ** any purpose.
 **
 ** perf.h
 ** Contains functions for getting the values of perf counters.
 **
 ** Author: Kostas Zoumpatianos <zoumpatianos@disi.unitn.it>
 ** -------------------------------------------------------------------------*/

/**
 ===============================================
 +                   EXAMPLE                   +
 ===============================================

 #include <stdlib.h>
 #include <stdio.h>
 #include <unistd.h>
 #include <string.h>
 #include <sys/ioctl.h>
 #include <linux/perf_event.h>
 #include <asm/unistd.h>
 #include "perf.h"

 #define EVENTC          18
 #define DOUBLE_EVENTC   (EVENTC * 2)
 #define EVENTVALC       (EVENTC + 1)

 int
 main(int argc, char **argv)
 {
     // Number of event counters to be monitored
     int eventc = EVENTC;

     // Counters to be monitored
     int eventv[DOUBLE_EVENTC] = {PERF_COUNT_HW_CPU_CYCLES,              PERF_TYPE_HARDWARE,
                                 PERF_COUNT_HW_INSTRUCTIONS,             PERF_TYPE_HARDWARE,
                                 PERF_COUNT_HW_CACHE_REFERENCES,         PERF_TYPE_HARDWARE,
                                 PERF_COUNT_HW_CACHE_MISSES,             PERF_TYPE_HARDWARE,
                                 PERF_COUNT_HW_BRANCH_INSTRUCTIONS,      PERF_TYPE_HARDWARE,
                                 PERF_COUNT_HW_BRANCH_MISSES,            PERF_TYPE_HARDWARE,
                                 PERF_COUNT_HW_BUS_CYCLES,               PERF_TYPE_HARDWARE,
                                 PERF_COUNT_HW_STALLED_CYCLES_FRONTEND,  PERF_TYPE_HARDWARE,
                                 PERF_COUNT_HW_STALLED_CYCLES_BACKEND,   PERF_TYPE_HARDWARE,
                                 PERF_COUNT_SW_CPU_CLOCK,                PERF_TYPE_SOFTWARE,
                                 PERF_COUNT_SW_TASK_CLOCK,               PERF_TYPE_SOFTWARE,
                                 PERF_COUNT_SW_PAGE_FAULTS,              PERF_TYPE_SOFTWARE,
                                 PERF_COUNT_SW_CONTEXT_SWITCHES,         PERF_TYPE_SOFTWARE,
                                 PERF_COUNT_SW_CPU_MIGRATIONS,           PERF_TYPE_SOFTWARE,
                                 PERF_COUNT_SW_PAGE_FAULTS_MIN,          PERF_TYPE_SOFTWARE,
                                 PERF_COUNT_SW_PAGE_FAULTS_MAJ,          PERF_TYPE_SOFTWARE,
                                 PERF_COUNT_SW_ALIGNMENT_FAULTS,         PERF_TYPE_SOFTWARE,
                                 PERF_COUNT_SW_EMULATION_FAULTS,         PERF_TYPE_SOFTWARE
     };

     // Array where counts are going to be stored
     long long eventval[EVENTVALC] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

     // Start measuring
     int *fd = start_measuring(eventc, eventv);

     // Do something
     printf("Measuring counts for this printf\n");

     // Stop measuring
     stop_measuring(fd, eventc, eventval);

     // Pring values
     print_event_counts(eventc, eventval);
 }
 */
#ifndef _PERF_H
#define _PERF_H


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

long
perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                int cpu, int group_fd, unsigned long flags)
{
    int ret;

    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                  group_fd, flags);
    return ret;
}

int *
start_measuring(int eventc, int *eventv) {
    int *fd = (int *) malloc(sizeof(int) * eventc);
    struct perf_event_attr *pe = (struct perf_event_attr *) malloc(sizeof(struct perf_event_attr) * eventc);

    int i,j;
    for(j=0; j<(eventc*2); j+=2) {
        // PERF EVENT SETTINGS
        memset(&pe[i], 0, sizeof(struct perf_event_attr));
        pe[i].type = eventv[j+1];                       // event type
        pe[i].size = sizeof(struct perf_event_attr);    // size of event
        pe[i].config = eventv[j];                       // event code
        pe[i].exclude_kernel = 1;                       // exclude kernel events
        pe[i].exclude_hv = 1;                           // exclude hypervisor events

        if(i==0) {
            pe[i].read_format=PERF_FORMAT_GROUP;
            pe[i].disabled = 1;
        }
        else {
            pe[i].disabled = 0;
        }

        // PERF EVENT CONTEXT
        pid_t pid = getpid();            // just the current process (-1 for all)
        int cpu = -1;                    // all cpus (-1)
        unsigned long flags = 0;         // no flags

        // OPEN
        if(i==0) {
            int group_fd = -1;
            fd[i] = perf_event_open(&pe[i], pid, cpu, group_fd, flags);
            if (fd[i] == -1) {
                fprintf(stderr, "Error opening leader %llx\n", pe[i].config);
                exit(EXIT_FAILURE);
            }
        }
        else {
            int group_fd = fd[0];
            fd[i] = perf_event_open(&pe[i], pid, cpu, group_fd, flags);
            if (fd[i] == -1) {
                fprintf(stderr, "Error opening leader %llx\n", pe[i].config);
                exit(EXIT_FAILURE);
            }
        }
        i++;
    }

    free(pe);

    // START
    ioctl(fd[0], PERF_EVENT_IOC_RESET, 0);
    ioctl(fd[0], PERF_EVENT_IOC_ENABLE, 0);

    return fd;
}

void
stop_measuring(int *fd, int eventvalc, long long *eventvalv) {
    ioctl(fd[0], PERF_EVENT_IOC_DISABLE, 0);
    int ret = read(fd[0], eventvalv, sizeof(long long) * (eventvalc+1));
    int i;
    for(i=0; i<eventvalc; i++)
        close(fd[i]);
    free(fd);
}

void
print_event_counts(int eventvalc, long long *eventvalv) {
    int i;
    printf("PERF");
    for(i=1;i<=eventvalc;i++) {
        printf("\t%lld", eventvalv[i]);
    }
    printf("\n");
}
#endif
