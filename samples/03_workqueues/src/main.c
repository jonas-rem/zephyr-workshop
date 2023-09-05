/*
 * Copyright (c) 2023 Phytec Messtechnik GmbH.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

#define MY_STACK_SIZE 512
#define MY_PRIORITY 5

K_THREAD_STACK_DEFINE(my_stack_area, MY_STACK_SIZE);

struct k_work_q my_work_q;


/*
 * This work item gets executed in the context a workqueue. The workqueue itself
 * can run in different thread contexts.
 */
static void my_work_handler(struct k_work *work) {
  struct k_thread *thread;
  const char *thread_name;

  thread = k_current_get();
  thread_name = k_thread_name_get(thread);
  printf("Work Item Executed - runtime context:\n");
  if (k_is_in_isr()) {
      printk(" ISR Context!\n\n");
      return;
  }
  printf(" Thread Name: %s \n", thread_name);
  printf(" Thread Priority: %d \n", thread->base.prio);
  printf("\n");
}

K_WORK_DEFINE(my_work, my_work_handler);

void my_timer_handler(struct k_timer *dummy)
{
	printf("Timer Expired!!\n");

	/* Executing work directly */
	my_work_handler(NULL);

	/* Executing work via work queue */
	k_work_submit(&my_work);
}

K_TIMER_DEFINE(my_timer, my_timer_handler, NULL);

int main(void)
{
	/* Directly run work item */
	my_work_handler(NULL);

	/* Submitting a work item to the system workqueue */
	k_work_submit(&my_work);

	/* Initializing a custom workqueue */
	k_work_queue_init(&my_work_q);
	k_work_queue_start(&my_work_q, my_stack_area,
			   K_THREAD_STACK_SIZEOF(my_stack_area), MY_PRIORITY,
			   NULL);
	k_thread_name_set(&(my_work_q.thread), "my_work_q_thread");

	/* Submitting a work item to a custom workqueue */
	k_work_submit_to_queue(&my_work_q, &my_work);

	/* Starting a single shot timer */
	k_timer_start(&my_timer, K_MSEC(1000), K_NO_WAIT);

	return 0;
}
