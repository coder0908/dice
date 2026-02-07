

#include <dasm.h>
#include <libdice/opcode.h>
#include <dasm/keys.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef char token_t[DASM_TOKEN_MAX_LEN];


typedef token_t token_buf_t[DASM_INSTRUCTION_MAX_TOKEN_CNT];

/**
 * @return instruction_words' length
 */

/* typedef libdice_word_t (*fn_libdasm_parse_instruction_t)(const instruction_data_t *data, token_buf_t token_buf, libdice_word_t token_cnt, libdice_word_t *instruction_words, libdice_word_t instruction_words_len); */
typedef struct instruction_data
{
	const char opcode_str[DASM_TOKEN_MAX_LEN];
	enum LIBDICE_OPCODE_ machine_code;
	libdice_word_t instruction_len;		/* word length*/	
	libdice_word_t (*parser)(const struct instruction_data *data, token_buf_t token_buf, libdice_word_t token_cnt, libdice_word_t *instruction_words, libdice_word_t instruction_words_len);
} instruction_data_t;	



typedef instruction_data_t instruction_data_table_t[DASM_OPCODE_CNT];



/*------------------------------------Parse functions------------------------------------*/

static libdice_word_t parser_opcode_set(const instruction_data_t *data, token_buf_t token_buf, libdice_word_t token_cnt, libdice_word_t *instruction_words, libdice_word_t instruction_words_len)
{

	char *end;

	if (strcmp(data->opcode_str, token_buf[0]) != 0)
	{
		return DASM_ERR_RET;
	}

	if (instruction_words_len < data->instruction_len || token_cnt != data->instruction_len)
	{
		return DASM_ERR_RET;
	}

	instruction_words[0] = (libdice_word_t)(data->machine_code);
	instruction_words[1] = (libdice_word_t)strtol(token_buf[1],  &end, 10);
	if (end == token_buf[1] || *end != '\0')
	{
		return DASM_ERR_RET;
	}
	instruction_words[2] = (libdice_word_t)strtol(token_buf[2],  &end, 10);
	if (end == token_buf[2] || *end != '\0')
	{
		return DASM_ERR_RET;
	}

	return data->instruction_len;
}


/*------------------------------------Parse functions------------------------------------*/

static const instruction_data_table_t s_instruction_data_table = {
	{DASM_SET, LIBDICE_OPCODE_SET, 3, parser_opcode_set},
	{DASM_NOP, LIBDICE_OPCODE_NOP, 1, NULL},
	{DASM_EOP, LIBDICE_OPCODE_EOP, 1, NULL},
	{DASM_IADD, LIBDICE_OPCODE_IADD, 6, NULL},
	{DASM_ISUB, LIBDICE_OPCODE_ISUB, 6, NULL},
	{DASM_PUTI, LIBDICE_OPCODE_PUTI, 3, NULL},
	{DASM_JMP, LIBDICE_OPCODE_JMP, 3, NULL},
	{DASM_JMPZ, LIBDICE_OPCODE_JMPZ, 5, NULL}
};



static libdice_word_t is_delimiter(char c)
{
	return c==' ' || c=='\n' || c=='\t';
}


static libdice_word_t is_ascii(const token_t token)
{

	if (strlen(token) != 3)
	{
		return 0;
	}
	if (token[0] == '\'' && token[2] == '\'' && ((unsigned char)token[1] < 0x80))
	{
		return 1;
	}

	return 0;
}

static libdice_word_t parse_ascii(token_t token)
{
	char tmp = token[1];
	
	if (snprintf(token, DASM_TOKEN_MAX_LEN, "%hhd", tmp) >= DASM_TOKEN_MAX_LEN)
	{
		return DASM_ERR_RET;
	}

	return 0;
	
}

static libdice_word_t is_deref(const token_t token)
{
	libdice_word_t i = 0;
	libdice_word_t token_len = strlen(token);

	if (!token_len)
	{
		return 0;
	}

	for (i=0; i<token_len; i++)
	{
		if (token[i] != '*')
		{
			return 0;
		}
	}

	return 1;
}

static libdice_word_t parse_deref(token_t token)
{
	libdice_word_t token_len = strlen(token);

	if (snprintf(token, DASM_TOKEN_MAX_LEN, "%u", token_len) >= DASM_TOKEN_MAX_LEN)
	{
		return DASM_ERR_RET;
	}

	return 0;
}

/* TODO : is_label, parse_label */
static libdice_word_t preprocessor(token_buf_t token_buf,  const libdice_word_t token_cnt)
{
	libdice_word_t i = 0;
	for (i=0; i<token_cnt; i++)
	{
		if (is_ascii(token_buf[i]))
		{
			if (parse_ascii(token_buf[i]) == DASM_ERR_RET)
			{
				return DASM_ERR_RET;
			}
		}
		else if (is_deref(token_buf[i]))
		{
			if (parse_deref(token_buf[i]) == DASM_ERR_RET)
			{
				return DASM_ERR_RET;	
			}
		}
	}

	return 1;
}

static libdice_word_t get_token_length(const char *str, const libdice_word_t str_len)
{
	libdice_word_t token_len = 0;

	assert(str);

	if (!str_len)
	{
		return DASM_ERR_RET;
	}

	if (str[0] == '*')
	{
		for (token_len = 1; token_len<str_len && str[token_len] == '*'; token_len++);
		return token_len;	
	}
	if (str[0] == '\'')
	{
		if (str_len < 3)
		{
			return DASM_ERR_RET;
		}

		if (str[2] != '\'')
		{
			return DASM_ERR_RET;
		}
		return 3;
	}

	for (token_len=0; token_len<str_len; token_len++)
	{
		if (is_delimiter(str[token_len]) || str[token_len] == ';')
		{
			break;
		}
	}

	return (token_len==0)?DASM_ERR_RET:token_len;
}

/**
 * @return token count, DASM_ERR_RET means failed
 * @param instruction is single line of instruction
 * **/
static libdice_word_t tokenize_instruction(const char *instruction, const libdice_word_t instruction_len, token_buf_t token_buf)
{
	libdice_word_t pc = 0;	
	libdice_word_t token_cnt = 0;
	libdice_word_t token_len = 0;

	assert(instruction);
	assert(token_buf);

	memset(token_buf, 0, sizeof(token_buf_t));

	while(pc < instruction_len)
	{
		while (pc < instruction_len && is_delimiter(instruction[pc]))
		{
			/* remove whitespace */
			pc++;
		}

		if (pc >= instruction_len) {
			break;
		}

		if (instruction[pc] == ';')
		{
			/* comment */
			break;
		}

		if (token_cnt >= DASM_INSTRUCTION_MAX_TOKEN_CNT)
		{
			/* token_buf insufficient */
			return DASM_ERR_RET;
		}

		token_len = get_token_length(&instruction[pc], instruction_len - pc);

		if (token_len == DASM_ERR_RET)
		{
			return DASM_ERR_RET;
		}

		/* DASM_INSTRUCTION_MAX_TOKEN_CNT include nul-character */
		if (token_len + 1 > DASM_TOKEN_MAX_LEN)
		{
			return DASM_ERR_RET;
		}

		memcpy(token_buf[token_cnt], &instruction[pc], token_len);
		token_buf[token_cnt][token_len] = '\0';

		token_cnt++;
		pc += token_len;
	}

	return token_cnt;
	
}




/** 
 * @param opcode nul-terminated
 * **/
static libdice_word_t find_instruction_table_idx(const instruction_data_table_t instruction_table, token_t opcode)
{
	libdice_word_t table_idx = 0;

	assert(instruction_table);
	assert(opcode);

	for (table_idx=0; table_idx<DASM_OPCODE_CNT; table_idx++)
	{
		if (strcmp(instruction_table[table_idx].opcode_str, opcode) == 0)
		{
			return table_idx;
		}
	}

	return DASM_ERR_RET;
	
}

DICEIMPL libdice_word_t dasm_assemble_line(
		ae2f_LP(c_num_ret) libdice_word_t* ae2f_restrict	ret_buf,
		const libdice_word_t					ret_buf_len,
		ae2f_LP(str_len) const char* ae2f_restrict		rd_instruction,
		const libdice_word_t					instruction_len
		)
{
	libdice_word_t token_cnt = 0;
	token_buf_t token_buf = {0,};
	libdice_word_t table_idx = 0;

	assert(ret_buf);
	assert(rd_instruction);

	token_cnt = tokenize_instruction(rd_instruction, instruction_len, token_buf);
	if (token_cnt == DASM_ERR_RET) {
		return DASM_ERR_RET;
	}
	if (preprocessor(token_buf, token_cnt) == DASM_ERR_RET)
	{
		return DASM_ERR_RET;
	}

	table_idx = find_instruction_table_idx(s_instruction_data_table, token_buf[0]);
	if (table_idx == DASM_ERR_RET)
	{
		return DASM_ERR_RET;
	}

	if (!s_instruction_data_table[table_idx].parser)
	{
		return DASM_ERR_RET;
	}

	return s_instruction_data_table[table_idx].parser(&s_instruction_data_table[table_idx], token_buf, token_cnt, ret_buf, ret_buf_len);
}



int main(void)
{

	printf("hello");

	return 0;
}

