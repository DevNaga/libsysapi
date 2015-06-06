#ifndef __SYSAPI_CIRQUE_H__
#define __SYSAPI_CIRQUE_H__

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

void *sysapi_cirque_init(int queues);

void sysapi_cirque_add(void *libctx, void *data);

void sysapi_cirque_for_each(void *libctx, void *cbdata, void (*cb_caller)(void *cbdata, void *data));

void sysapi_cirque_deinit(void *libctx);

#endif

