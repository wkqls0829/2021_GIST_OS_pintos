#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/synch.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "filesys/off_t.h"
#include "devices/block.h"
#include "devices/shutdown.h"

void syscall_handler (struct intr_frame *f);
void halt (void);
void exit (int status);
pid_t exec (const char *cmd_line);
int wait (pid_t pid);
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
// int open (const char *file);
// int filesize (int fd);
// int read (int fd, void *buffer, unsigned size);
// int write (int fd, const void *buffer, unsigned size);
// void seek (int fd, unsigned position);
// unsigned tell (int fd);
// void close (int fd);

/* 추가적으로 메모리 위치 확인에 도움을 주는 함수를 초기에 선언한다. */

// 해당 영역이 user address 공간인지 확인하는 함수
void uaddr_check (const void *vaddr);


/* System call을 호출하기 위한  System call Handler 구현 */
void syscall_handler (struct intr_frame *f) 
{
	  // 현재 esp 구역의 주소를 받아오기, esp : 스택 맨 아랫부분
	  void *stack_ptr = f->esp;
	    // 현재 주소가 user address에 포함 되어있는지 확인, 안되어있으면 따로 구현한 check_user_vaddr()함수가 exit(-1);을 호출하여 종료시킨다.
	    uaddr_check(stack_ptr);

		  /* 구현하지 못한 System call은 주석처리함.*/
		  // 기존에 받은 stack_ptr을 switch 구문에 넣어 해당하는 system call을 호출하도록 구현한다. 
		  switch (*(uint32_t *)stack_ptr)
			    {
					    case SYS_HALT:                  
							      // 인자를 받지 않고 halt()실행.
							      halt();
								        break;
											
										    case SYS_EXIT:                
										      uaddr_check (stack_ptr + 4);
											        // 인자를 1개 받는다.
											        exit( *(uint32_t *)(stack_ptr + 4));
													      break;
														      
														      case SYS_EXEC:                  
														        uaddr_check (stack_ptr + 4);
																      // 인자를 1개 받는다.
																      f->eax = exec ( (const char *)*(uint32_t *)(stack_ptr + 4));
																	        break;
																			      
																			    case SYS_WAIT:                   
																			      uaddr_check (stack_ptr + 4);
																				        // 인자를 1개 받는다.
																				        f->eax = wait ( (pid_t *)*(uint32_t *)(stack_ptr + 4));
																						      break;
																							        
																							      case SYS_CREATE:                 
																							        uaddr_check (stack_ptr + 4);
																									      // 인자를 2개 받고, 리턴값을 eax에 넣기
																									      // 2개 받을 때는 esp보다 64비트 공간을 더 이동해준다.
																									      f->eax = create ( (const char *)*(uint32_t *)(stack_ptr + 4),  (unsigned)*(uint32_t *)(stack_ptr + 8));
																										        break;

																												    case SYS_REMOVE:
																												      uaddr_check (stack_ptr + 4);
																													        // 인자를 1개 받고, 리턴값을 eax에 넣기
																													        f->eax = remove ( (const char *)*(uint32_t *)(stack_ptr + 4));
																															      break;
																																      
																																    }
}

/*-- system call 구현 --*/

void halt ()
{
	  // Shutdown_power_off()를 사용하여 Pintos를 종료시킨다. 
	  shutdown_power_off ();
}

void exit (int status)
{
	  // 현재 실행중인 thread에서 exit_code에 status를 저장한다.
	  thread_current() -> exit_code = status;
	    // Problem 1이 제시한대로 process이름과 exit code를 출력해준다.
	    printf("%s: exit(%d)\n", thread_name(), status);
		  // Thread 종료시킨다. 
		  thread_exit();
}

pid_t exec (const char *cmd_line)
{
	  // 해당 process 실행 수 해당하는 process의 pid를 리턴해준다.
	  return process_execute(cmd_line);
}

int wait (pid_t pid)
{
	  // wait 하는 process의 pid를 리턴해준다.
	  return process_wait(pid);
}

bool create (const char *file, unsigned initial_size)
{
	  // file poitner가 NULL이면 종료.
	  if (file == NULL) exit(-1);
	    bool create_success;
		  create_success = filesys_create (file, initial_size);
		    // file create 생성 여부를 bool형태로 리턴한다. 
		    return create_success;
}

bool remove (const char *file)
{
	  // file poitner가 NULL이면 종료.
	  if (file == NULL) exit(-1);
	    bool remove_success;
		  // file remove 제거 여부를 bool 형태로 리턴한다. 
		  remove_success = filesys_remove(file);
		    return remove_success;
}
                                       


/* 추가적으로 메모리 위치 확인에 도움을 주는 함수를 초기에 선언한다. */

// 해당 영역이 user address 공간인지 확인하는 함수
void uaddr_check (const void *vaddr){
		// PHY_BASE보다 큰지 작은지 판단해주는 함수가 vaddr.h에 저장되어 있기에 아래와 같이 구현함.
	  // 이것으로 Systam call handler에서 User 영역을 체크해준다.
	  if (!is_user_vaddr (vaddr)) exit(-1);
}


