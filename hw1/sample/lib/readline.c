#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>

#define PROMPT ""
const char *cmds[] = {"who", "name", "tell", "yell", "exit", (char *) NULL};

char **complete_func(const char *text, int start, int end);
char *generator_func(const char *text, int state);
const char **available_list;

void readline_initialize() {
	rl_attempted_completion_function = complete_func;
}

const char *get_line() {
	static char *line_read = (char *) NULL;
	static int first_in = 1;
	
	if(first_in) {
		readline_initialize();
		first_in = 0;
	}

	while(1) {
		if(line_read) {
			free(line_read);
			line_read = (char *) NULL;
		}

		line_read = readline(PROMPT);
		if(line_read && *line_read) {
			int cmd_index = 0;
			const char *cmd_search;
			while(cmd_search = cmds[cmd_index++]) {
				if(strncmp(cmd_search, line_read, strlen(cmd_search)) == 0) {
					add_history(line_read);
					break;
				}
			}

			return line_read;
		}
	}
}

//rl_line_buffer
char **complete_func(const char *text, int start, int end) {
	rl_attempted_completion_over = 1;

	if(start == 0) {
		available_list = cmds;
		return rl_completion_matches(text, generator_func);
	}

	return (char **) NULL;
}

char *generator_func(const char *text, int state) {
	static const char **list;
	static int index;
	static int len;

	if(state == 0) {
		list = available_list;
		index = 0;
		len = strlen(text);
	}

	const char *content;
	while(content = list[index++]) {
		if(strncmp(text, content, len) == 0) {
			char *match = (char *) malloc(strlen(content) + 1);
			strcpy(match, content);
			return match;
		}
	}

	return (char *) NULL;
}
