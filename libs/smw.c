#include "smw.h"
#include <string.h>

smw g_smw;

int smw_init()
{
    memset(&g_smw, 0, sizeof(g_smw));

    for (int i = 0; i < smw_max_tasks; i++)
    {
        g_smw.tasks[i].context = NULL;
        g_smw.tasks[i].callback = NULL;
    }

    return 0;
}

smw_task *smw_create_task(void *context, void (*callback)(void *context, uint64_t mon_time))
{
    for (int i = 0; i < smw_max_tasks; i++)
    {
        if (g_smw.tasks[i].context == NULL && g_smw.tasks[i].callback == NULL)
        {
            g_smw.tasks[i].context = context;
            g_smw.tasks[i].callback = callback;
            return &g_smw.tasks[i];
        }
    }

    return NULL;
}

void smw_destroy_task(smw_task *task)
{
    if (task == NULL) return;

    for (int i = 0; i < smw_max_tasks; i++)
    {
        if (&g_smw.tasks[i] == task)
        {
            g_smw.tasks[i].context = NULL;
            g_smw.tasks[i].callback = NULL;
            break;
        }
    }
}

void smw_work(uint64_t mon_time)
{
    for (int i = 0; i < smw_max_tasks; i++)
    {
        if (g_smw.tasks[i].callback != NULL)
        {
            g_smw.tasks[i].callback(g_smw.tasks[i].context, mon_time);
        }
    }
}

int smw_get_task_count()
{
    int counter = 0;
    for (int i = 0; i < smw_max_tasks; i++)
    {
        if (g_smw.tasks[i].callback != NULL)
        {
            counter++;
        }
    }

    return counter;
}

void smw_dispose()
{
    for (int i = 0; i < smw_max_tasks; i++)
    {
        g_smw.tasks[i].context = NULL;
        g_smw.tasks[i].callback = NULL;
    }
}
