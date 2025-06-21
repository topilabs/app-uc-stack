#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <FreeRTOS.h>
#include <task.h>

void webserver_task(void *pvParameters);

#endif // WEBSERVER_H 