/*
 * JMPXRDS, an FM MPX signal generator with RDS support on
 * top of Jack Audio Connection Kit - RDS Configuration helpers
 *
 * Copyright (C) 2015 Nick Kossifidis <mickflemm@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "rds_encoder.h"
#include <string.h>		/* For memset/memcpy/strnlen */

uint16_t
rds_get_pi(const struct rds_encoder_state *st)
{
	if (st == NULL)
		return -1;
	return st->pi;
}

int
rds_set_pi(struct rds_encoder_state *st, uint16_t pi)
{
	if (st == NULL)
		return -1;
	st->pi = pi;
	return 0;
}

uint8_t
rds_get_ecc(const struct rds_encoder_state * st)
{
	if (st == NULL)
		return 0;
	return st->ecc;
}

int
rds_set_ecc(struct rds_encoder_state *st, uint8_t ecc)
{
	if (st == NULL)
		return -1;

	st->ecc = ecc;

	if (st->ecc == 0)
		st->ecc_set = 0;
	else
		st->ecc_set = 1;

	return 0;
}

uint16_t
rds_get_lic(const struct rds_encoder_state * st)
{
	if (st == NULL)
		return 0;
	return st->lic;
}

int
rds_set_lic(struct rds_encoder_state *st, uint16_t lic)
{
	if (st == NULL)
		return -1;

	st->lic = lic & 0xFFF;

	if (st->lic == 0)
		st->lic_set = 0;
	else
		st->lic_set = 1;

	return 0;
}

uint8_t
rds_get_pty(const struct rds_encoder_state * st)
{
	return st->pty;
}

int
rds_set_pty(struct rds_encoder_state *st, uint8_t pty)
{
	if (st == NULL)
		return -1;

	if (pty <= 31)
		st->pty = pty;
	else
		return -1;
	return 0;
}

uint8_t
rds_get_ta(const struct rds_encoder_state * st)
{
	if (st == NULL)
		return 0;
	return st->ta;
}

int
rds_set_ta(struct rds_encoder_state *st, uint8_t ta)
{
	if (st == NULL)
		return -1;
	st->ta = ta & 1;
	return 0;
}

uint8_t
rds_get_tp(const struct rds_encoder_state * st)
{
	if (st == NULL)
		return 0;
	return st->tp;
}

int
rds_set_tp(struct rds_encoder_state *st, uint8_t tp)
{
	if (st == NULL)
		return -1;
	st->tp = tp & 1;
	return 0;
}

uint8_t
rds_get_ms(const struct rds_encoder_state * st)
{
	if (st == NULL)
		return 0;
	return st->ms;
}

int
rds_set_ms(struct rds_encoder_state *st, uint8_t ms)
{
	if (st == NULL)
		return -1;
	st->ms = ms & 1;
	return 0;
}

uint8_t
rds_get_di(const struct rds_encoder_state * st)
{
	if (st == NULL)
		return 0;
	return st->di;
}

int
rds_set_di(struct rds_encoder_state *st, uint8_t di)
{
	if (st == NULL)
		return -1;

	st->di = di & 0xF;
	return 0;
}

char *
rds_get_ps(const struct rds_encoder_state *st)
{
	static char ps[RDS_PS_LENGTH + 1];
	if (st == NULL)
		return NULL;
	memset(ps, 0, RDS_PS_LENGTH + 1);
	memcpy(ps, st->ps, RDS_PS_LENGTH);
	return ps;
}

int
rds_set_ps(struct rds_encoder_state *st, const char *ps)
{
	int pslen = 0;
	int i = 0;

	if (st == NULL || ps == NULL)
		return -1;

	pslen = strnlen(ps, RDS_PS_LENGTH);
	if (!pslen || pslen > RDS_PS_LENGTH)
		return -2;

	memset(st->ps, 0, RDS_PS_LENGTH);

	if (pslen == 0) {
		st->ps_set = 0;
		return 0;
	}

	for (i = 0; i < pslen; i++) {
		if ((ps[i] >= 0x20) || (ps[i] == 0x7F))
			st->ps[i] = ps[i];
		else
			st->ps[i] = 0;
	}
	st->ps_set = 1;
	st->ps_idx = 0;

	return 0;
}

char *
rds_get_ptyn(const struct rds_encoder_state *st)
{
	static char ptyn[RDS_PTYN_LENGTH + 1];
	if (st == NULL || !st->ptyn_set)
		return NULL;

	memset(ptyn, 0, RDS_PTYN_LENGTH + 1);
	memcpy(ptyn, st->ptyn, RDS_PTYN_LENGTH);

	return ptyn;
}

int
rds_set_ptyn(struct rds_encoder_state *st, const char *ptyn)
{
	int ptynlen = 0;
	int i = 0;

	if (st == NULL || ptyn == NULL)
		return -1;

	ptynlen = strnlen(ptyn, RDS_PTYN_LENGTH);
	if (ptynlen > RDS_PTYN_LENGTH)
		return -1;

	if (ptynlen == 0) {
		st->ptyn_set = 0;
		memset(st->ptyn, 0, RDS_PTYN_LENGTH);
		return 0;
	}

	/* Flip A/B flag to flush PTYN buffer
	 * on receiver */
	if (st->ptyn_set)
		st->ptyn_flush = st->ptyn_flush ? 0 : 1;

	memset(st->ptyn, 0, RDS_PTYN_LENGTH);
	for (i = 0; i < ptynlen; i++) {
		if ((ptyn[i] >= 0x20) || (ptyn[i] == 0x7F))
			st->ptyn[i] = ptyn[i];
		else
			st->ptyn[i] = 0;
	}
	st->ptyn_set = 1;
	st->ptyn_idx = 0;

	return 0;
}

char *
rds_get_rt(const struct rds_encoder_state *st)
{
	static char rt[RDS_RT_LENGTH + 1];

	if (!st->rt_set)
		return NULL;

	memset(rt, 0, RDS_RT_LENGTH + 1);
	memcpy(rt, st->rt, RDS_RT_LENGTH);

	return rt;
}

int
rds_set_rt(struct rds_encoder_state *st, const char *rt, int flush)
{
	int rtlen = 0;
	int i = 0;

	if (st == NULL || rt == NULL)
		return -1;

	rtlen = strnlen(rt, RDS_RT_LENGTH);
	if (rtlen > RDS_RT_LENGTH)
		return -1;

	if (rtlen == 0) {
		st->rt_set = 0;
		memset(st->rt, 0, RDS_RT_LENGTH);
		return 0;
	}

	/* Flip A/B flag to flush RT buffer
	 * on receiver */
	if (flush && st->rt_set)
		st->rt_flush = st->rt_flush ? 0 : 1;

	memset(st->rt, 0, RDS_RT_LENGTH);
	for (i = 0; i < rtlen; i++) {
		if (((rt[i] < 0x20) &&
		     ((rt[i] != RDS_RT_CR) &&
		      (rt[i] != RDS_RT_LF) &&
		      (rt[i] != RDS_RT_END_OF_HEADLINE) &&
		      (rt[i] != RDS_RT_SOFT_HYPHEN))) || (rt[i] == 0x7F))
			st->rt[i] = 0;
		else
			st->rt[i] = rt[i];
	}

	while (i % 4 && i < RDS_RT_LENGTH - 1) {
		st->rt[i] = 0x20;
		i++;
	}

	/* According to the standard the RT message
	 * should end with a CR character but some
	 * receivers display a '-' instead
	 st->rt[i - 1] = RDS_RT_CR;
	 */

	st->rt_segments = i / 4;

	st->rt_set = 1;
	st->rt_idx = 0;

	return 0;
}

int
rds_get_af(const struct rds_encoder_state *st, float *freqs)
{
	int num_freqs = 0;
	int i = 0;

	if (st == NULL || freqs == NULL)
		return -1;

	if (!st->af_set)
		return 0;

	num_freqs = st->af_data[0] - RDS_AF_NUM_BASE;
	for (i = 0; i < num_freqs; i++)
		freqs[i] = 87.5 + ((float) st->af_data[i + 1] / 10.0);

	return num_freqs;
}

int
rds_set_af(struct rds_encoder_state *st, const float *freqs, int num_freqs)
{
	uint8_t af_data[RDS_AF_DATA_LEN] = {0};
	int af_len = 0;
	int i = 0;

	if (st == NULL || num_freqs < 0 || num_freqs > RDS_AF_MAX_FREQS)
		return -1;

	if (num_freqs == 0) {
		st->af_set = 0;
		memset(st->af_data, 0, RDS_AF_DATA_LEN);
		return 0;
	}

	if (freqs == NULL)
		return -1;

	af_data[0] = RDS_AF_NUM_BASE + num_freqs;
	for (i = 0; i < num_freqs; i++) {
		if (freqs[i] < RDS_AF_MIN_FREQ - 0.05 ||
		    freqs[i] > RDS_AF_MAX_FREQ + 0.05)
			return -2;
		/* AF code = (f - 87.5MHz) / 100KHz */
		af_data[i + 1] = (uint8_t) ((freqs[i] - 87.5) * 10.0 + 0.5);
	}

	/* Pad to a full block (two codes per group) */
	af_len = num_freqs + 1;
	if (af_len % 2)
		af_data[af_len++] = RDS_AF_FILLER;

	/* Disable AF while updating so that the encoder
	 * doesn't send a partial list */
	st->af_set = 0;
	memcpy(st->af_data, af_data, RDS_AF_DATA_LEN);
	st->af_len = af_len;
	st->af_idx = 0;
	st->af_set = 1;

	return 0;
}
