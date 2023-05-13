#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 100

char *read_command(void);
void execute_command(char *command);

/**
 * main - Entry point of the shell program
 *
 * Return: Always 0.
 */
int  main(void)
{
	char *command;

	while ((command = read_command()) != NULL)
	{
		execute_command(command);
		free(command);
	}
	return (0);
}

/**
 * read_command - reads a command from the user
 *
 * Return: The command entered by the user
 */
char *read_command(void)
{
	char *command = NULL;
	size_t bufsize = 0;
	ssize_t charsread;

	printf("$ ");
	fflush(stdout);

	charsread = getline(&command, &bufsize, stdin);

	if (charsread == -1)
	{
		if (feof(stdin))
		{
			printf("\n");
			return (NULL);
		}
		else
		{
			perror("getline");
			exit(EXIT_FAILURE);
		}
	}

	/* Remove the newline character */
	command[strcspn(command, "\n")] = '\0';

	return (command);
}

/**
 * execute_command - executes command
 * @command: command to be executed
 *
 * Return: nothing.
 */
void execute_command(char *command)
{
	pid_t pid = fork();

	if (pid < 0)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)
	{
		/* Child process */
		if (execlp(command, command, NULL) == -1)
		{
			perror("execlp");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		/* Parent process */
		int status;

		waitpid(pid, &status, 0);

		if (WIFEXITED(status) && WEXITSTATUS(status) == 127)
		{
			printf("Command not found: %s\n", command);
		}
	}
}
