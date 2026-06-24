#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct{
	char	*tape;
	size_t	t_index;
	size_t	*loop_stack;
	size_t	stack_depth;
}		brainfuck;

size_t	strlen(const char *str)
{
	size_t	i = 0;

	while (str[i])
		i++;
	return (i);
}

int	in_BF_dict(int c)
{
	if (!(c == '-'
		|| c == '+'
		|| c == ','
		|| c == '.'
		|| c == '['
		|| c == ']'
		|| c == '<'
		|| c == '>'))
		return (0);
	return (1);
}

void	*get_good_instruction(char *instruction)
{
	char	*ret = calloc(strlen(instruction) + 1, sizeof(*instruction));
	size_t	i = 0;
	size_t	it = 0;
	size_t	full_instruction_len = strlen(instruction);
	int		stack = 0;

	if (!ret)
	{
		printf("Failled to allocate full instruction tape.\n");
		return(NULL);
	}
	while (i < full_instruction_len)
	{
		if (in_BF_dict(instruction[i]))
			ret[it++] = instruction[i];
		if (instruction[i] == '[')
			stack++;
		else if (instruction[i] == ']')
		{
			if (stack == 0)
			{
				printf("Closing bracket should close an opening bracket.\n");
				return (free(ret), NULL);
			}
			stack--;
		}
		i++;
	}
	if (stack != 0)
	{
		printf("Opening bracket should be closed by a closing bracket.\n");
		return (free(ret), NULL);
	}
	return (ret);
}

void	run_brainfuck(brainfuck p, char *instruction)
{
	size_t	instruction_index = 0;
	size_t	instruction_len = strlen(instruction);

	while (instruction_index < instruction_len)
	{
		if (instruction[instruction_index] == '+')
			p.tape[p.t_index]++;
		else if (instruction[instruction_index] == '-')
			p.tape[p.t_index]--;
		else if (instruction[instruction_index] == '<')
		{
			if (p.t_index == 0)
				return ((void) printf("Cannot have negative memmory.\n"));
			p.t_index--;
		}
		else if (instruction[instruction_index] == '>')
		{
			p.t_index++;
			if (p.t_index > 30000)
				p.t_index = 0;
		}
		else if (instruction[instruction_index] == '.')
			printf("%c", (char) p.tape[p.t_index]);
		else if (instruction[instruction_index] == ',')
			read(0, &p.tape[p.t_index], 1);
		else if (instruction[instruction_index] == '[')
		{
			p.loop_stack[p.stack_depth] = instruction_index;
			p.stack_depth++;
		}
		else if (instruction[instruction_index] == ']')
		{
			if (p.stack_depth == 0)
				return ((void) printf("A closing bracket must have an open bracket befor hand.\n"));
			if (p.tape[p.t_index] != 0)
			{
				p.stack_depth--;
				instruction_index = p.loop_stack[p.stack_depth];
				continue ;
			}
		}
		instruction_index++;
	}
}

int	call_interpreter(char *instruction)
{
	brainfuck	program = {};
	char		*tape = calloc(30001, sizeof(char));

	program.loop_stack = calloc(strlen(instruction), sizeof(size_t));
	if (!program.loop_stack)
		return (free(tape), 1);
	program.stack_depth = 0;
	instruction = get_good_instruction(instruction);
	if (!instruction)
		return (free(program.loop_stack), free(tape), 1);
	if (!tape)
		return (free(program.loop_stack), free(tape), free(instruction), 1);
	program.tape = tape;
	program.t_index = 0;
	run_brainfuck(program, instruction);
	return (free(program.loop_stack), free(instruction), free(tape), 0);
}

int	main(int ac, char *av[])
{
	if (ac < 2)
	{
		printf("No BrainFuck instruction.\n");
		return (1);
	}
	return (call_interpreter(av[1]));
}