#ifndef SMW_H
#define SMW_H

#include <stdint.h>

#define smw_max_tasks 16

typedef struct
{
    void* context;
    void (*callback)(void* context, uint64_t mon_time);
} smw_task;

typedef struct
{
    smw_task tasks[smw_max_tasks];
} smw;

extern smw g_smw;

int smw_init();
smw_task* smw_create_task(void* context, void(*callback)( void* context, uint64_t mon_time));
void smw_destroy_task(smw_task* task);
void smw_work(uint64_t mon_time);
int smw_get_task_count();

void smw_dispose();

#endif