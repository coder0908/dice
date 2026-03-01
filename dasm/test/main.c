#include <stdio.h>
#include "../lib/pp/pp.h"
#include "../lib/lexer/lexer.h"
#include "libdice/type.h"
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#define PROGRAMME	"iadd 1000 10, 10 	   \r\n"	\
			"  label0:   \"	dsdsf 	 \"   \n"	\
			"      	\"\"...	puti \'a\' label0		\n"		


int main(void)
{
	enum DASM_ERR_ err;
	struct dasm_pp pp;
	char dst[100] = {0,};
	const char src[100] = PROGRAMME;

	struct dasm_tok_stream tok_stream;
	struct dasm_tok toks[100];

	struct dasm_lexer lexer;

	if (!dasm_pp_init(&pp)) {
		printf("[ERROR0]\n");
	}

	err = dasm_pp_execute(&pp, dst, 100, src, strlen(src)+1);
	if (err != DASM_ERR_OK) {
		printf("[ERROR1] %u\n", err);
		exit(1);
	}

	dasm_tok_stream_init(&tok_stream, toks, 100);

	dasm_lexer_init(&lexer);
	err = dasm_lexer_execute(&lexer, &tok_stream, dst, pp.m_dst_cnt);
	if (err != DASM_ERR_OK) {
		printf("[ERROR2] %u\n", err);
		exit(1);		
	}


	if (!dasm_tok_stream_seek(&tok_stream, 0, DASM_TOK_STREAM_WHENCE_SET)) {
		printf("[ERROR3]\n");
		exit(1);
	}

	do {
		struct dasm_tok *tok;
		libdice_word_t i;

		tok = dasm_tok_stream_peek(&tok_stream);
		if (!tok) {
			printf("[ERROR4]\n");
			break;
		}

		for (i=0; i<tok->m_lexeme_len; ++i) {
			printf("%c", tok->m_lexeme[i]);
		}
		printf("\t tok_type = %u\n", tok->m_type);

	} while (dasm_tok_stream_advance(&tok_stream));

	dasm_tok_stream_deinit(&tok_stream);
	dasm_lexer_deinit(&lexer);
	dasm_pp_deinit(&pp);

	return 0;
}