#include "./ast.h"
#include <stdlib.h>
#include <assert.h>

DICEIMPL void dasm_ast_programme_init(struct dasm_ast_programme *rdwr_ast_prog, struct dasm_ast_line rdwr_lines[], const libdice_word_t c_lines_len)
{
	assert(rdwr_ast_prog);
	assert(rdwr_lines);

	rdwr_ast_prog->m_lines = rdwr_lines;
	rdwr_ast_prog->m_lines_len = c_lines_len;
	rdwr_ast_prog->m_line_cnt = 0;
}

DICEIMPL void dasm_ast_programme_deinit(struct dasm_ast_programme *rdwr_ast_prog)
{
	libdice_word_t i;

	assert(rdwr_ast_prog);

	/** free directive's operands */
	for (i=0; i<rdwr_ast_prog->m_line_cnt; ++i) {
		struct dasm_ast_line *line = &rdwr_ast_prog->m_lines[i];
		if (line->m_type == DASM_AST_LINE_TYPE_STATEMENT) {
			struct dasm_ast_statement *statement = &line->m_statement;
			if (statement->m_type == DASM_AST_STATEMENT_TYPE_DIRECTIVE) {
				struct dasm_ast_directive *directive = &statement->m_directive;
				if (directive->m_operands) {
					free(directive->m_operands);
				}
			}
		}
	}

	rdwr_ast_prog->m_lines = NULL;
	rdwr_ast_prog->m_lines_len = 0;
	rdwr_ast_prog->m_line_cnt = 0;
}




