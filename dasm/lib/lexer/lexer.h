


#ifndef dasm_lexer_h
#define dasm_lexer_h

#include <ae2f/c90/StdBool.h>
#include <dasm/err.h>
#include <libdice/abi.h>
#include <libdice/type.h>
#include "./tok.h"

enum DASM_LEXER_STATE_ {
	DASM_LEXER_STATE_IDLE,
	DASM_LEXER_STATE_IDENT,		/** LABEL, MNEMONIC, DIRECTIVE etc... */
	DASM_LEXER_STATE_INT_IMM,	/** memory, integer etc... */
	DASM_LEXER_STATE_CHAR_IMM,	/** 'A' */
	DASM_LEXER_STATE_STRING_IMM,	/** "string" */
	DASM_LEXER_STATE_STAR		/** *, **, ***, etc... */
};

struct dasm_lexer {
	libdice_word_t m_src_cnt;
	
	enum DASM_LEXER_STATE_ m_state;
};

DICECALL void dasm_lexer_init(struct dasm_lexer *rdwr_lexer);
DICECALL void dasm_lexer_deinit(struct dasm_lexer *rdwr_lexer);
DICECALL enum DASM_ERR_ dasm_lexer_execute(struct dasm_lexer *rdwr_lexer, 
	struct dasm_tok_stream *rdwr_tok_stream, 
	const char rd_src[], const libdice_word_t c_src_len);

#endif /* dasm_lexer_h */
