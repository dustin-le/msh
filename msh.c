/*
  Name: Dustin Le
  ID: 1001130689
*/
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 11     // Mav shell only supports five arguments

// Function handlers.
void handle_sigint()
{
}

void handle_sigtstp()
{
}

void handle_sigchld()
{
}

void handle_sigcont()
{
}

int main()
{
  // Handle the signals for:
  // Ctrl-Z, Ctrl-C, bg, and the return of a child.
  signal(SIGINT, handle_sigint);
  signal(SIGTSTP, handle_sigtstp);
  signal(SIGCHLD, handle_sigchld);
  signal(SIGCONT, handle_sigcont);

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
  char *history[50];
  int pids[15];
  int history_index = 0, pid_index = 0;

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;                                         
                                                           
    char *working_str  = strdup( cmd_str );                

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }
	
	// Inputs every cmd_str into the history until 50.
	// Once 50 hits, the array shifts one place with the new value,
	// getting rid of the oldest value.
	if (token[0] != '\0' && *token[0] != '!')
	{
		if (history_index < 50)
		{	
			history[history_index] = strdup(cmd_str);
			history_index++;
		}

		else
		{
			int index;
			for (index = 0; index < 49; index++)
			{
				history[index] = history[index+1];
			}
			history[49] = strdup(cmd_str);
			history_index++;
		}

		if (!strcmp(token[0],"quit") || !strcmp(token[0],"exit"))
		{
		  exit(0);
		}

		if (!strcmp(token[0], "cd"))
		{
			if(chdir(token[1]) == -1)
			{
				printf("Directory not found.\n");
			}
			continue;
		}	
		
		// Print out the contents of the history array,
		// accounting for the case that there are less 
		// than the maximum 50 values. Same for listpids.
		if (!strcmp(token[0], "history"))
		{
			int i;
			if (history_index < 50)
			{
				for (i = 0; i < history_index; i++)
				{
					printf("%d: %s", i, history[i]);
				}
			}
			
			else
			{
				for (i = 0; i < 50; i++)
				{
					printf("%d: %s", i, history[i]);
				}
			}
			continue;
		}

		if (!strcmp(token[0], "listpids"))
		{
			int i;
			if (pid_index < 15)
			{
				for (i = 0; i < pid_index; i++)
				{
					printf("%d: %d\n", i, pids[i]);
				}
			}

			else
			{
				for (i = 0; i < 15; i++)
				{
					printf("%d: %d\n", i, pids[i]);
				}
			}
			continue;
		}
	}

	int child_pid = fork();

	// Inputs every child_pid into the listpids until 15.
	// Once 15 hits, the array shifts one place with the new value,
	// getting rid of the oldest value.
	int index;
	if (pid_index < 15)
	{	
		pids[pid_index] = child_pid;
		pid_index++;
	}
	
	else
	{
		for (index = 0; index < 14; index++)
		{
			pids[index] = pids[index+1];
		}
		pids[14] = child_pid;
	}

	int status;	
	
	if (child_pid == 0)
	{
		if (!strcmp(token[0], "bg"))
		{
			int temp = getpid();
			kill(temp, SIGCONT);
		}

		else if (*token[0] == '!')
		{
			// Get the value after the !,
			// change it to an integer,
			// and use it to index the arrays.
			// Call _exit(1) when done because we are in the child.
			char* token2;
			token2 = strtok(cmd_str, "!");
			int num = atoi(token2);
			if (num > history_index-1)
			{
				printf("Command not in history.\n");
				_exit(1);
			}

            history[num][strlen(history[num])-1] = '\0';

			// If the value at the index ocntains a "cd",
			// execute chdir with the next, tokenized arguments.
			if (strstr(history[num], "cd") != NULL)
			{
				token2 = strtok(history[num], " ");
				token2 = strtok(NULL, " \n");
				if(chdir(token2) == -1)
				{
					printf("Directory not found.\n");
				}
				_exit(1);
			}	

			// Print out the contents of the history array,
			// accounting for the case that there are less 
			// than the maximum 50 values. Same for listpids.
			if (!strcmp(history[num], "history"))
			{
				int i;
				if (history_index < 50)
				{
					for (i = 0; i < history_index; i++)
					{
						printf("%d: %s", i, history[i]);
					}
				}
				
				else
				{
					for (i = 0; i < 50; i++)
					{
						printf("%d: %s", i, history[i]);
					}
				}
				_exit(1);
			}

			if (!strcmp(history[num], "listpids"))
			{
				int i;
				if (pid_index < 15)
				{
					for (i = 0; i < pid_index; i++)
					{
						printf("%d: %d\n", i, pids[i]);
					}
				}

				else
				{
					for (i = 0; i < 15; i++)
					{
						printf("%d: %d\n", i, pids[i]);
					}
				}
				_exit(1);
			}

			int test = execlp(history[num], history[num], NULL);

			if (test == -1)
			{
				printf("Command not found.\n");
				exit(0);
			}
		}

		else
		{
			int test = execlp(token[0], token[0], token[1], token[2], token[3], token[4], token[5], token[6], token[7], token[8], token[9], token[10], NULL);
			if (test == -1)
			{
				printf("%s: Command not found.\n", token[0]);
				exit(0);
			}
		}
	}

	waitpid(child_pid, &status, 0);

    free( working_root );

  }
  return 0;
}
