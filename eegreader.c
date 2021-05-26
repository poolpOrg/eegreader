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

#include <sys/types.h>
#include <sys/ioctl.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "eegreader.h"

static void usage(void);
static void line_set(int);
static void parser_init(struct parser *);
static int parser_process_byte(struct parser *, uint8_t);
static void packet_display(struct packet *);
static void packet_display_json(struct packet *);

extern char *__progname;

int main(int argc, char *argv[])
{
	int i;
	int ch;
	int fd;
	int nbytes;
	int json = 0;
	unsigned char buf[sizeof(struct packet)];
	const char *line = "/dev/cuaU0";
	struct parser ctx;

	while ((ch = getopt(argc, argv, "jl:")) != -1)
	{
		switch (ch)
		{
		case 'j':
			json = 1;
			break;
		case 'l':
			line = optarg;
			break;
		default:
			usage();
		}
	}

	fd = open(line, O_RDONLY | O_NONBLOCK);
	if (fd < 0)
		err(1, "%s", line);

	line_set(fd);

	parser_init(&ctx);
	for (;;)
	{
		nbytes = read(fd, buf, sizeof buf);
		if (nbytes < 0)
		{
			if (errno != EINTR || errno != EAGAIN)
				continue;
			err(1, NULL);
		}

		for (i = 0; i < nbytes; ++i)
			if (parser_process_byte(&ctx, buf[i])) {
				if (json)
					packet_display_json(&ctx.packet);
				else
					packet_display(&ctx.packet);
			}
	}

	close(fd);

	return (0);
}

static void
usage(void)
{
	fprintf(stderr, "usage: %s -l /dev/cuaUx [-s]\n", __progname);
	exit(1);
}

static void
line_set(int fd)
{
	struct termios lio;
	struct termios tio;

	if (ioctl(fd, TIOCEXCL) != 0)
		err(1, NULL);
	if (tcgetattr(fd, &lio) != 0)
		err(1, NULL);

	memcpy(&tio, &lio, sizeof tio);
	tio.c_lflag &= ~ICANON;
	tio.c_cflag &= ~CSIZE;
	tio.c_cflag |= CREAD | CS8 | CLOCAL;
	tio.c_cc[VMIN] = 1;
	tio.c_cc[VTIME] = 0;
	cfsetspeed(&tio, 57600);

	if (tcsetattr(fd, TCSAFLUSH, &tio) != 0)
		err(1, NULL);
}

static void
parser_init(struct parser *ctx)
{
	memset(ctx, 0, sizeof *ctx);
}

static int
parser_process_byte(struct parser *ctx, uint8_t byte)
{
	if (ctx->state == STATE_SYNC_1)
	{
		if (byte != 0xa5)
			goto resync;
	}
	else if (ctx->state == STATE_SYNC_2)
	{
		if (byte != 0x5a)
			goto resync;
	}
	else if (ctx->state == STATE_VERSION)
	{
		if (byte != 0x02)
			goto resync;
	}

	ctx->packet.bytemap[ctx->state++] = byte;
	if (ctx->state == STATE_MAX)
		goto complete;

	return 0;

complete:
	ctx->state = STATE_SYNC_1;
	return 1;

resync:
	ctx->state = STATE_SYNC_1;
	return 0;
}

static void
packet_display(struct packet *packet)
{
	printf("%d|%d|%d|%d|%d|%d|%d|%d|%d\n",
		   packet->bytemap[STATE_VERSION],
		   packet->bytemap[STATE_FRAME],
		   packet->bytemap[STATE_CHAN1_LO] << 8 | packet->bytemap[STATE_CHAN1_HI],
		   packet->bytemap[STATE_CHAN2_LO] << 8 | packet->bytemap[STATE_CHAN2_HI],
		   packet->bytemap[STATE_CHAN3_LO] << 8 | packet->bytemap[STATE_CHAN3_HI],
		   packet->bytemap[STATE_CHAN4_LO] << 8 | packet->bytemap[STATE_CHAN4_HI],
		   packet->bytemap[STATE_CHAN5_LO] << 8 | packet->bytemap[STATE_CHAN5_HI],
		   packet->bytemap[STATE_CHAN6_LO] << 8 | packet->bytemap[STATE_CHAN6_HI],
		   packet->bytemap[STATE_BUTTON_STATES] & 0xf);
}

static void
packet_display_json(struct packet *packet)
{
	printf("{\"version\":%d,"
			"\"frame\":%d,"
			"\"channel_1\":%d,"
			"\"channel_2\":%d,"
			"\"channel_3\":%d,"
			"\"channel_4\":%d,"
			"\"channel_5\":%d,"
			"\"channel_6\":%d,"
			"\"button_states\":%d}\n",
		   packet->bytemap[STATE_VERSION],
		   packet->bytemap[STATE_FRAME],
		   packet->bytemap[STATE_CHAN1_LO] << 8 | packet->bytemap[STATE_CHAN1_HI],
		   packet->bytemap[STATE_CHAN2_LO] << 8 | packet->bytemap[STATE_CHAN2_HI],
		   packet->bytemap[STATE_CHAN3_LO] << 8 | packet->bytemap[STATE_CHAN3_HI],
		   packet->bytemap[STATE_CHAN4_LO] << 8 | packet->bytemap[STATE_CHAN4_HI],
		   packet->bytemap[STATE_CHAN5_LO] << 8 | packet->bytemap[STATE_CHAN5_HI],
		   packet->bytemap[STATE_CHAN6_LO] << 8 | packet->bytemap[STATE_CHAN6_HI],
		   packet->bytemap[STATE_BUTTON_STATES] & 0xf);
}