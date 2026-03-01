#ifndef dasm_parser_h
#define dasm_parser_h


#include <ae2f/c90/StdBool.h>
#include <dasm/err.h>
#include <libdice/abi.h>
#include <libdice/type.h>
#include "../lexer/tok.h"
#include "./ast.h"


struct dasm_parser {
	struct dasm_ast_programme m_ast_prog;
	struct dasm_tok_stream *m_tok_stream;
};

DICECALL enum DASM_ERR_ dasm_parser_execute(struct dasm_parser *rdwr_parser,
	struct dasm_ast_programme *rdwr_ast_prog, struct dasm_tok_stream *rdwr_tok_stream);

#endif /* dasm_parser_h */