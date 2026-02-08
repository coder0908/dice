#include "parser.h"
#include <string.h>
#include <dasm/keys.h>



static struct libdasm_opcode_define s_opcode_define_table[LIBDICE_OPCODE_CNT] = {
	{LIBDASM_OPCODE_TXT_SET, LIBDICE_OPCODE_SET, 2},
};

static void libdasm_init_parsed_line(struct libdasm_parsed_line *rdwr_parsed_line)
{
	memset(rdwr_parsed_line->m_label, 0, LIBDASM_TOKEN_MAX_LEN);
	rdwr_parsed_line->m_opcode = 0;
	memset(rdwr_parsed_line->m_operands, 0, sizeof(struct libdasm_operand) * LIBDICE_OPERAND_MAX_CNT);
	rdwr_parsed_line->m_operand_cnt = 0;
}


libdice_word_t libdasm_parse_line(struct libdasm_parsed_line *rdwr_parsed_line, const struct libdasm_token_line *rd_token_line)
{
	libdice_word_t token_idx = 0;
	libdice_word_t i = 0;
	struct libdasm_token *token = NULL;
	size_t len = 0;

	if (rd_token_line->m_token_cnt == 0) {
		return 0;
	}

	if (rd_token_line->m_token_cnt==1 && rd_token_line->m_tokens[0].m_token[0]=='\n') {
		return 0;
	}

	token = &(rd_token_line->m_tokens[token_idx]);
	if (token->m_token_type==LIBDASM_TOKEN_TYPE_LABEL) {
		/* label exist */
		len =  strlen(token->m_token);
		token->m_token[len-1] = '\0';
		strcpy(rdwr_parsed_line->m_label, token->m_token);
		token_idx++;
	}

	token = &(rd_token_line->m_tokens[token_idx]);
	for (i=0; i<LIBDICE_OPCODE_CNT; i++) {
		if (strcmp(s_opcode_define_table[i].m_mnemonic, token->m_token) == 0) {
			rdwr_parsed_line->m_opcode = s_opcode_define_table[i].m_opcode;
			rdwr_parsed_line->m_operand_cnt = s_opcode_define_table[i].m_operand_cnt;
			token_idx++;
			break;
		}
	}
	assert(i<LIBDICE_OPCODE_CNT);

	for (; token_idx<rd_token_line->m_token_cnt; token_idx++) {
		token = &(rd_token_line->m_tokens[token_idx]);
		
	}

}


libdice_word_t libdasm_parse_programme(struct libdasm_parsed_line rdwr_parsed_lines[], const libdice_word_t c_parsed_lines_len, const struct libdasm_token_line rd_token_lines[], const libdice_word_t c_token_lines_len )
{

}