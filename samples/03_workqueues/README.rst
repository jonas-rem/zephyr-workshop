Workqueues
##########

A sample application that demonstrates how to use workqueues. The application
executes different work items in different workqueues. The work item
(my_work_handler) prints the runtime context in which it is running (e.g.
sysworkq) and the priority of the underlaying thread.

Negative priorities represent cooperative priorities that can not be
interrupted. Positive priorities represent preemptible threads, so threads that
can be interrupted. The lower the thread priority the higher the assigned
number. In this scheme, the idle thread has the lowest priority and therefore
the highest assigned numer.

Resources:
- https://docs.zephyrproject.org/latest/kernel/services/threads/index.html#thread-priorities
