#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGUMENTS 10

char *read_command(void);
char **parse_arguments(char *command);
void execute_command(char **arguments);

/**
 * main - Entry point of the shell program.
 *
 * Return: always 0.
 */
int main(void)
{
	char *command;
	char **args;

	while ((command = read_command()) != NULL)
	{
		args = parse_arguments(command);
		execute_command(args);

		free(command);
		free(args);
	}

	return (0);
}

/**
 * read_command - Reads a command from the user.
 *
 * Return: The command entered by the user.
 */
char *read_command(void)
{
	char *command = NULL;
	size_t bufsize = 0;
	ssize_t charsread;

	printf("$ ");

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
 * parse_arguments - Parses the command line into arguments.
 * @command: The command line input.
 *
 * Return: An array of arguments.
 */
char **parse_arguments(char *command)
{
	char **args = malloc((MAX_ARGUMENTS + 1) * sizeof(char *));
	char *arg;
	int i = 0;

	arg = strtok(command, " ");
	while (arg != NULL && i < MAX_ARGUMENTS)
	{
		args[i] = strdup(arg);
		arg = strtok(NULL, " ");
		i++;
	}
	args[i] = NULL; /* EOF */

	return (args);
}

/**
 * execute_command - Executes a command with arguments.
 * @arguments: The array of arguments.
 *
 * Return: Nothing.
 */
void execute_command(char **arguments)
{
	pid_t pid = fork();
	int status;

	if (pid < 0)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)
	{
		/* Child process */
		if (execvp(arguments[0], arguments) == -1)
		{
			perror("execvp");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		/* Parent process */
		waitpid(pid, &status, 0);
		if (WIFEXITED(status) && WEXITSTATUS(status) == 127)
		{
			printf("Command not found: %s\n", arguments[0]);
		}
	}
}
