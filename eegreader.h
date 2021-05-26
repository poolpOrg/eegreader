/*
 * Copyright (c) 2017 Gilles Chehade <gilles@poolp.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef	_EEGREADER_H_
#define	_EEGREADER_H_

enum {
	STATE_SYNC_1 = 0,
	STATE_SYNC_2,

	STATE_VERSION,
	STATE_FRAME,

	STATE_CHAN1_LO,
	STATE_CHAN1_HI,

	STATE_CHAN2_LO,
	STATE_CHAN2_HI,

	STATE_CHAN3_LO,
	STATE_CHAN3_HI,

	STATE_CHAN4_LO,
	STATE_CHAN4_HI,

	STATE_CHAN5_LO,
	STATE_CHAN5_HI,

	STATE_CHAN6_LO,
	STATE_CHAN6_HI,

	STATE_BUTTON_STATES,

	STATE_MAX
};

struct parser {
	uint8_t			state;
	struct timeval	tv;

	struct packet {
		uint8_t		bytemap[STATE_MAX];
	} __attribute__((packed)) packet;
};

#endif
