#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef uint32_t u32;
typedef int32_t i32;

struct process
{
  u32 pid;
  u32 arrival_time;
  u32 burst_time;

  TAILQ_ENTRY(process) pointers;

  /* Additional fields here */
  u32 running_time; /* amount of time process has been running in total */
  /*u32 time_last_ran;*/
  /* End of "Additional fields here" */
};

TAILQ_HEAD(process_list, process); /* brains of the list -- called process_list */

u32 next_int(const char **data, const char *data_end)
/* returns next integer from a sequence of bytes */
{
  u32 current = 0;
  bool started = false;
  while (*data != data_end)
  {
    char c = **data;

    if (c < 0x30 || c > 0x39)
    {
      if (started)
      {
        return current;
      }
    }
    else
    {
      if (!started)
      {
        current = (c - 0x30);
        started = true;
      }
      else
      {
        current *= 10;
        current += (c - 0x30);
      }
    }

    ++(*data);
  }

  printf("Reached end of file while looking for another integer\n");
  exit(EINVAL);
}

u32 next_int_from_c_str(const char *data)
/* also returns next integer but from a string */
{
  char c;
  u32 i = 0;
  u32 current = 0;
  bool started = false;
  while ((c = data[i++]))
  {
    if (c < 0x30 || c > 0x39)
    {
      exit(EINVAL);
    }
    if (!started)
    {
      current = (c - 0x30);
      started = true;
    }
    else
    {
      current *= 10;
      current += (c - 0x30);
    }
  }
  return current;
}

void init_processes(const char *path,
                    struct process **process_data,
                    u32 *process_size)
/* creates an array of process structs */
{
  int fd = open(path, O_RDONLY); /* open provided file */
  if (fd == -1)
  {
    int err = errno;             /* error opening file */
    perror("open");
    exit(err);
  }

  struct stat st;               /* get file status in st */
  if (fstat(fd, &st) == -1)
  {
    int err = errno;
    perror("stat");
    exit(err);
  }

  u32 size = st.st_size; /* map some memory for the file? */
  const char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data_start == MAP_FAILED)
  {
    int err = errno;
    perror("mmap");
    exit(err);
  }

  const char *data_end = data_start + size;
  const char *data = data_start;

  *process_size = next_int(&data, data_end);

  /* allocates an array of process structs */
  *process_data = calloc(sizeof(struct process), *process_size);
  if (*process_data == NULL)
  {
    int err = errno;
    perror("calloc");
    exit(err);
  }

  /* provide each process struct with its proper values */
  /* ordering of numbers in file represents: PID, arrive time, time to completion */
  for (u32 i = 0; i < *process_size; ++i)
  {
    (*process_data)[i].pid = next_int(&data, data_end);
    (*process_data)[i].arrival_time = next_int(&data, data_end);
    (*process_data)[i].burst_time = next_int(&data, data_end);
  }

  munmap((void *)data, size);  /* clean up the map */
  close(fd);                   /* close the file */
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    return EINVAL;
  }
  struct process *data; /* array of processes */
  u32 size; /* amount of elements in the array */
  init_processes(argv[1], &data, &size);

  u32 quantum_length = next_int_from_c_str(argv[2]);

  struct process_list list;
  TAILQ_INIT(&list);

  u32 total_waiting_time = 0;
  u32 total_response_time = 0;

  /* Your code here */

  /* initialize relevant values and add to queue */
  for(u32 i = 0; i < size; i++) {
	  data[i].running_time = 0;
	  /*data[i].time_last_ran = 0;

	  u32 PID = data[i].pid;
	  u32 arrival = data[i].arrival_time;
	  u32 burst_time = data[i].burst_time;
	  u32 running_time = data[i].running_time;
	  u32 time_last_ran = data[i].time_last_ran;*/
	  /*printf("PID: %d\n\tarrival: %d\n\tburst_time: %d\n\trunning: %d\n\tlast ran: %d\n",
			  PID, arrival, burst_time, running_time, time_last_ran);*/

	  if(TAILQ_EMPTY(&list)){
		  /*printf("\tadding to head of queue...");*/
		  TAILQ_INSERT_HEAD(&list, &data[i], pointers);
		  /*printf("done\n");*/
		  continue;
	  }


	  struct process *iterator = TAILQ_LAST(&list, process_list);
	  do {
		  if(data[i].arrival_time > iterator->arrival_time) {
			  /*printf("\tadding to queue...");*/
			  TAILQ_INSERT_AFTER(&list, iterator, &data[i], pointers);
			  /*printf("done\n");*/
			  break;
		  }
		  /*printf("\titerating...");*/
		  iterator = TAILQ_PREV(iterator, process_list, pointers);
		  /*printf("done\n");*/
	  } while( (iterator != TAILQ_FIRST(&list)) && (iterator != NULL) );
  }

  /*printf("\n");*/
  /* iterate through entire queue */
  u32 time = 0;
  struct process *current = TAILQ_FIRST(&list);
  struct process *tail = TAILQ_FIRST(&list);
  /*u32 current_running_time = 0;*/
  while(!TAILQ_EMPTY(&list)) {
	  /* get the next process to run */
	  /*current = TAILQ_FIRST(&list);*/
	  /*current_running_time = current->running_time;*/
	  /*printf("Time: %d - %d\n\tPID: %d\n\tburst: %d\n\trunning: %d -> %d\n",
			  time, time+quantum_length, current->pid, current->burst_time, current->running_time, (current->running_time)+quantum_length);*/

	  /* retrieve response time */
	  if(current->running_time == 0) {
		  total_response_time += time - current->arrival_time;
		  /*printf("\tretrieved response time\n");*/
	  }

	  /* run the process */
	  current->running_time += quantum_length;
	  time += quantum_length;

	  /* check if another process spawned during this quantum */
	  for (struct process *iter = TAILQ_NEXT(tail, pointers); iter != NULL; iter = TAILQ_NEXT(iter, pointers)) {
		  if(iter->arrival_time < time) {
			  tail = iter;
		  }
	  }
	  /*printf("\ttail: %d\n", tail->pid);*/
	  /*while ( (tail != NULL) && (tail->arrival_time < time) ){
		  tail = TAILQ_NEXT(tail, pointers);
	  }*/

	  /* check if current process is done */
	  /*printf("\tif: %s\n", current->running_time < current->burst_time ? "true" : "false");*/
	  if (current->running_time < current->burst_time) {
		  /*TAILQ_INSERT_AFTER(&list, tail, current, pointers);*/
		  /*printf("\trunning time: %d\n", current->running_time);*/
		  /*TAILQ_REMOVE(&list, TAILQ_FIRST(&list), pointers);*/
		  /*printf("\tadded to end of queue\n");*/
	  } else {
		  u32 waiting_time = time - current->arrival_time - (current->running_time - current->burst_time);
		  total_waiting_time += waiting_time;
		  /*printf("\twaiting time: %d\n", waiting_time);*/
		  TAILQ_REMOVE(&list, current, pointers);
		  if (total_waiting_time > 100) exit(1);
		  /*printf("\tremoved from queue\n");*/
	  }
	  /*TAILQ_REMOVE(&list, TAILQ_FIRST(&list), pointers);*/
	  current = TAILQ_NEXT(current, pointers);
	  if ( (current == NULL) || (current == TAILQ_NEXT(tail, pointers)) ) {
		  current = TAILQ_FIRST(&list);
	  }
  }
  
  /* End of "Your code here" */

  /* FORMULAE
   *     wait time = (time completed) - (time arrived)
   * response time = (time of first run) - (time of arrival)
   */
  printf("Average waiting time: %.2f\n", (float)total_waiting_time / (float)size);
  printf("Average response time: %.2f\n", (float)total_response_time / (float)size);

  free(data);
return 0;
}
