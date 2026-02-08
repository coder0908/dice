#ifndef libdasm_parser_h
#define libdasm_parser_h

#include <libdice/type.h>
#include <libdice/opcode.h>
#include "tokenizer.h"

enum e_libdasm_operand_type {
	LIBDASM_OPERAND_TYPE_NUMBER,	/* immediate number or address */
	LIBDASM_OPERAND_TYPE_LABEL,
	LIBDASM_OPERAND_TYPE_CHAR,
	LIBDASM_OPERAND_TYPE_STRING,
	LIBDASM_OPERAND_TYPE_NREF
};

struct libdasm_operand {
	enum e_libdasm_operand_type m_operand_type;
	char m_text[LIBDASM_TOKEN_MAX_LEN];
};

struct libdasm_parsed_line {
	char m_label[LIBDASM_TOKEN_MAX_LEN];	/* NULL or address */
	
	enum LIPDICE_OPCODE_ m_opcode;
	struct libdasm_operand m_operands[LIBDICE_OPERAND_MAX_CNT];
	libdice_word_t m_operand_cnt;
};

struct libdasm_opcode_define {
	const char m_mnemonic[LIBDASM_TOKEN_MAX_LEN];
	const enum LIPDICE_OPCODE_ m_opcode;
	const libdice_word_t m_operand_cnt;
};

libdice_word_t libdasm_parse_programme(struct libdasm_parsed_line rdwr_parsed_lines[], const libdice_word_t c_parsed_lines_len, const struct libdasm_token_line rd_token_lines[], const libdice_word_t c_token_lines_len );

#endif /* libdasm_parser_h */