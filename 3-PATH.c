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
void execute_command(char **arguments, char **envp);

/**
 * read_command - Reads input commands
 *
 * Return: The command entered.
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
			return (NULL); /* EOF */
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
 * parse_arguments - parses the commandline into arguments
 * @command: the commandline input
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
 * execute_command - Executes a command with arguments
 * @arguments: array of arguments
 * @envp: array of environment variables
 *
 * Return: Nothing.
 */
void execute_command(char **arguments, char **envp)
{
	char *command_path = NULL;
	char *path;
	int i = 0;
	int exists = 0;

	/* Check if the command exists in the PATH */
	while (envp[i])
	{
		if (strncmp(envp[i], "PATH", 5) == 0)
		{
			path = strtok(envp[i] + 5, ":");
			while (path != NULL)
			{
				command_path = malloc(strlen(path) + strlen(arguments[0]) + 2);
				sprintf(command_path, "%s/%s", path, arguments[0]);
				if (access(command_path, X_OK) == 0)
				{
					exists = 1;
					break;
				}
				free(command_path);
				command_path = NULL;
				path = strtok(NULL, ":");
			}
			break;
		}
		i++;
	}

	if (!exists)
	{
		printf("Command not found: %s\n", arguments[0]);
		return;
	}

	pid_t pid = fork();

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
		/* parent process */
		int status;

		waitpid(pid, &status, 0);

		if (WIFEXITED(status) && WEXITSTATUS(status) == 127)
		{
			printf("Command not found: %s\n", arguments[0]);
		}
	}
}

/**
 * main - Entry point of the shell program.
 * @argc: Argument count
 * @argv: Argumrnt vector
 * @envp: array of environment variables
 *
 * Return: Always 0.
 */
int main(int argc, char **argv, char **envp)
{
	char *command;
	char **args;

	while ((command = read_command()) != NULL)
	{
		args = parse_arguments(command);
		execute_command(args, envp);

		free(command);
		free(args);
	}

	return (0);
}
