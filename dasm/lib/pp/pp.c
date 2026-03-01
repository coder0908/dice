#include <ae2f/cc.h>
#include "./pp.h"
#include <dasm/err.h>
#include <stdlib.h>
#include <assert.h>

DICEIMPL bool dasm_pp_init(struct dasm_pp *rdwr_pp)
{
        assert(rdwr_pp);

        rdwr_pp->m_dst_cnt = 0;
        rdwr_pp->m_src_cnt = 0;

	rdwr_pp->m_state = DASM_PP_STATE_NORMAL;

        return true;
}

DICEIMPL void dasm_pp_deinit(struct dasm_pp *rdwr_pp)
{
	assert(rdwr_pp);

	rdwr_pp->m_dst_cnt = 0;
	rdwr_pp->m_src_cnt = 0;

	rdwr_pp->m_state = DASM_PP_STATE_NORMAL;
}

static ae2f_inline enum DASM_ERR_ dasm_pp_execute_line(struct dasm_pp *rdwr_pp,
	char rdwr_dst[], const libdice_word_t c_dst_len, 
	const char rd_src[], const libdice_word_t c_src_len)
{
	char prev_ch = 'A';	/* Not ' ', '\t' */
	char ch;

        assert(rdwr_pp);
	assert(rdwr_dst);
	assert(rd_src);

        while (rdwr_pp->m_dst_cnt < c_dst_len
		&& rdwr_pp->m_src_cnt < c_src_len) {

		ch = rd_src[rdwr_pp->m_src_cnt++];

                switch (rdwr_pp->m_state) {
		case DASM_PP_STATE_NORMAL:
			switch (ch) {
			case '{':
				rdwr_pp->m_state = DASM_PP_STATE_BLOCK_COMMENT;
				break;
			case ';':
				rdwr_pp->m_state = DASM_PP_STATE_LINE_COMMENT;
				break;
			case '}':
				rdwr_pp->m_src_cnt--;
				return DASM_ERR_INVAL_COMMENT;
			case '\"':
				rdwr_dst[rdwr_pp->m_dst_cnt++] = ch;
				rdwr_pp->m_state = DASM_PP_STATE_STRING_IMM;
				break;
			case '\r':
				break;
			case '\n':
			case '\0':
				rdwr_dst[rdwr_pp->m_dst_cnt++] = ch;
				return DASM_ERR_OK;
			case '\t':
				ch = ' ';
				ae2f_fallthrough;
			case ' ':
				if (prev_ch == ' ') {
					break;
				}
				ae2f_fallthrough;
			default:
				
				rdwr_dst[rdwr_pp->m_dst_cnt++] = ch;
				prev_ch = ch;
				break;
			}
			break;
		case DASM_PP_STATE_BLOCK_COMMENT:
			switch (ch) {
			case '}':
				rdwr_pp->m_state = DASM_PP_STATE_NORMAL;
				break;
			case '\n':
			case '\0':
				rdwr_dst[rdwr_pp->m_dst_cnt++] = ch;
				return DASM_ERR_OK;
			default:
				break;
			}
			break;
		case DASM_PP_STATE_LINE_COMMENT:
			switch (ch) {
			case '\n':
			case '\0':
				rdwr_dst[rdwr_pp->m_dst_cnt++] = ch;
				rdwr_pp->m_state = DASM_PP_STATE_NORMAL;
				return DASM_ERR_OK;
			default:
				break;
			}
			break;
		case DASM_PP_STATE_STRING_IMM:
			switch (ch) {
			case '\n':
			case '\0':
				rdwr_pp->m_src_cnt--;
				return DASM_ERR_INVAL_STRING_IMM;
			case '\"':
				rdwr_dst[rdwr_pp->m_dst_cnt++] = ch;
				rdwr_pp->m_state = DASM_PP_STATE_NORMAL;
				break;
			default:
				rdwr_dst[rdwr_pp->m_dst_cnt++] = ch;
				break;
			}			
			break;
		default:
			return DASM_ERR_UNKNOWN;
		}
        }
        
        if (rdwr_pp->m_dst_cnt >= c_dst_len) {
                return DASM_ERR_MEM_INSUF;
        }

        if (rdwr_pp->m_src_cnt >= c_src_len) {
                return DASM_ERR_NO_TERM;
        }

        return DASM_ERR_UNKNOWN;
}

DICEIMPL enum DASM_ERR_ dasm_pp_execute(struct dasm_pp *rdwr_pp, 
	char rdwr_dst[], const libdice_word_t c_dst_len, 
	const char rd_src[], const libdice_word_t c_src_len)
{
	enum DASM_ERR_ err = DASM_ERR_OK;

	if (!rdwr_pp) {
		return DASM_ERR_UNKNOWN;
	}

	while (rdwr_pp->m_dst_cnt < c_dst_len
		&& rdwr_pp->m_src_cnt < c_src_len) {

		err = dasm_pp_execute_line(rdwr_pp,
			rdwr_dst, c_dst_len,
			rd_src, c_src_len);

		if (err != DASM_ERR_OK) {
			break;
		}
	}
	
	return err;
}

