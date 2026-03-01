#ifndef dasm_ast_h
#define dasm_ast_h

#include <ae2f/c90/StdBool.h>
#include <libdice/abi.h>
#include <libdice/type.h>
#include "dasm/keys.h"
#include <libdice/opcode.h>


#define DASM_INSTRUCTION_OPERAND_MAX_CNT	6

enum DASM_AST_OPERAND_TYPE_ {
	DASM_AST_OPERAND_TYPE_INT_IMM,
	DASM_AST_OPERAND_TYPE_CHAR_IMM,
	DASM_AST_OPERAND_TYPE_STR_IMM,
	DASM_AST_OPERAND_TYPE_DEREF_CNT
};

struct dasm_ast_operand {
	enum DASM_AST_OPERAND_TYPE_ m_type;
	
	const char *m_str;
	libdice_word_t m_str_len;
};

struct dasm_ast_instruction {
	enum LIBDICE_OPCODE_ m_opcode;
	struct dasm_ast_operand m_operands[DASM_INSTRUCTION_OPERAND_MAX_CNT];
	libdice_word_t m_operand_cnt;
};

struct dasm_ast_directive {
	enum DASM_DIRECTIVE_CODE_ m_dcode;
	struct dasm_ast_operand *m_operands;
	libdice_word_t m_operands_len;		/* Dynamic allocated */
};

enum DASM_AST_STATEMENT_TYPE_ {
	DASM_AST_STATEMENT_TYPE_INSTRUCTION,
	DASM_AST_STATEMENT_TYPE_DIRECTIVE,
};

struct dasm_ast_statement {
	enum DASM_AST_STATEMENT_TYPE_ m_type;
	
	union {
		struct dasm_ast_instruction m_instruction;
		struct dasm_ast_directive m_directive;
	};
};

struct dasm_ast_label {
	const char *m_str;
	libdice_word_t m_str_len;
	libdice_word_t m_addr;
};

enum DASM_AST_LINE_TYPE_ {
	DASM_AST_LINE_TYPE_LABEL,
	DASM_AST_LINE_TYPE_STATEMENT
};

struct dasm_ast_line {
	enum DASM_AST_LINE_TYPE_ m_type;
	union {
		struct dasm_ast_label m_label;
		struct dasm_ast_statement m_statement;
	};
};

struct dasm_ast_programme {
	struct dasm_ast_line *m_lines;
	libdice_word_t m_line_cnt;
	libdice_word_t m_lines_len;
};

DICECALL void dasm_ast_programme_init(struct dasm_ast_programme *rdwr_ast_prog, struct dasm_ast_line rdwr_lines[], const libdice_word_t c_lines_len);
DICECALL void dasm_ast_programme_deinit(struct dasm_ast_programme *rdwr_ast_prog);

#endif /* dasm_ast_h */