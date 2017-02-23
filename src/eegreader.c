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
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <thread>
#include <mutex>

#include "eegreader.h"
#include "matplotlibcpp.h"

static void	usage(void);
static void	line_set(int);
static void	parser_init(struct parser *);
static int	parser_process_byte(struct parser *, uint8_t);
static void	packet_display(struct parser::packet *);
std::vector<double>		*moveLeft(std::vector<double> *v, struct parser);
	
extern char		*__progname;
static volatile bool	keepRunning = true;
std::mutex		mtx;

void sigintHandler(int i) {
  (void)i;
  keepRunning = false;
}

void
collector(struct parser *ctx, int fd)
{
  int nbytes;
  int i;
  unsigned char buf[64];
  mtx.lock();
  parser_init(ctx);
  while (1) {
    nbytes = read(fd, buf, sizeof buf);
    if (nbytes < 0) {
      if (errno != EINTR || errno != EAGAIN)
	continue;
      err(1, NULL);
    }
    for (i = 0; i < nbytes; ++i)
      if (parser_process_byte(ctx, buf[i])) {
	mtx.unlock();
	packet_display(&(ctx->packet));
	mtx.lock();
      }
  }
}

int
main(int argc, char *argv[])
{
  int ch;
  int fd;
  const char *line = "/dev/cuaU0";
  struct parser ctx;
  std::vector<double> *chanVectorCollector = new std::vector<double>[5];
  std::thread	t1;

  signal(SIGINT, sigintHandler);
  chanVectorCollector[0].push_back(0.0);
  chanVectorCollector[1].push_back(0.0);
  chanVectorCollector[2].push_back(0.0);
  chanVectorCollector[3].push_back(0.0);
  chanVectorCollector[4].push_back(0.0);
  matplotlibcpp::ion();
  matplotlibcpp::show();
  matplotlibcpp::draw();
  while ((ch = getopt(argc, argv, "l:")) != -1) {
    switch (ch) {
    case 'l':
      line = optarg;
      break;
    default:
      usage();
    }
  }
  fd = open(line, O_RDONLY | O_NONBLOCK);
  if (fd < 0)
    err(1, NULL);
  line_set(fd);
  t1 = std::thread(collector, &ctx, fd);
  while (keepRunning) {
    mtx.lock();
    std::cout << "normal thread" << std::endl;
    packet_display(&(ctx.packet));
    if (chanVectorCollector[4].size() == 30){
      matplotlibcpp::clf();
      chanVectorCollector = moveLeft(chanVectorCollector, ctx);
    }
    else
      {
	chanVectorCollector[4].push_back(chanVectorCollector[4].back() + 0.01);
	chanVectorCollector[0].push_back((double)ctx.packet.bytemap[STATE_CHAN1_LO]);
	chanVectorCollector[1].push_back((double)ctx.packet.bytemap[STATE_CHAN1_HI]);
	chanVectorCollector[2].push_back((double)ctx.packet.bytemap[STATE_CHAN2_LO]);
	chanVectorCollector[3].push_back((double)ctx.packet.bytemap[STATE_CHAN2_HI]);
      }
    mtx.unlock();
    matplotlibcpp::plot(chanVectorCollector[4], chanVectorCollector[0], "r");
    matplotlibcpp::plot(chanVectorCollector[4], chanVectorCollector[1], "g");
    matplotlibcpp::plot(chanVectorCollector[4], chanVectorCollector[2], "b");
    matplotlibcpp::plot(chanVectorCollector[4], chanVectorCollector[3], "y");
    matplotlibcpp::draw();
    matplotlibcpp::pause(0.001);
  }
  close(fd);
  return (0);
}

std::vector<double>	*moveLeft(std::vector<double> *stock, struct parser ctx)
{
  unsigned int	i;

  i = 0;
  while (i < stock[0].size() - 1)
    {
      stock[0][i] = stock[0][i + 1];
      stock[1][i] = stock[1][i + 1];
      stock[2][i] = stock[2][i + 1];
      stock[3][i] = stock[3][i + 1];
      i++;
    }
  stock[0][i] = (double)ctx.packet.bytemap[STATE_CHAN1_LO];
  stock[1][i] = (double)ctx.packet.bytemap[STATE_CHAN1_HI];
  stock[2][i] = (double)ctx.packet.bytemap[STATE_CHAN2_LO];
  stock[3][i] = (double)ctx.packet.bytemap[STATE_CHAN2_HI];
  return (stock);
}

static void
usage(void)
{
	fprintf(stderr, "usage: %s -l [-p] /dev/cuaUx\n", __progname);
	exit(1);
}

static void
line_set(int fd)
{
	struct termios	lio;
	struct termios	tio;

	if (ioctl(fd, TIOCEXCL) != 0)
		err(1, NULL);
	if (tcgetattr(fd, &lio) != 0)
		err(1, NULL);

	memcpy(&tio, &lio, sizeof tio);
	tio.c_lflag &= ~ICANON;
	tio.c_cflag &= ~CSIZE;
	tio.c_cflag |= CREAD|CS8|CLOCAL;
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
	if (ctx->state == STATE_SYNC_1) {
		if (byte != 0xa5)
			goto resync;
	}
	else if (ctx->state == STATE_SYNC_2) {
		if (byte != 0x5a)
			goto resync;
	}
	else if (ctx->state == STATE_VERSION) {
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
packet_display(struct parser::packet *packet)
{
	printf("%02x|%d|%d,%d|%d,%d|%d,%d|%d,%d|%d,%d|%d,%d|%d,%d|%d\n",
	    packet->bytemap[STATE_FRAME],
	    packet->bytemap[STATE_VERSION],

	    packet->bytemap[STATE_CHAN1_LO],
	    packet->bytemap[STATE_CHAN1_HI],

	    packet->bytemap[STATE_CHAN2_LO],
	    packet->bytemap[STATE_CHAN2_HI],

	    packet->bytemap[STATE_CHAN2_LO],
	    packet->bytemap[STATE_CHAN2_HI],

	    packet->bytemap[STATE_CHAN3_LO],
	    packet->bytemap[STATE_CHAN3_HI],

	    packet->bytemap[STATE_CHAN4_LO],
	    packet->bytemap[STATE_CHAN4_HI],

	    packet->bytemap[STATE_CHAN5_LO],
	    packet->bytemap[STATE_CHAN5_HI],

	    packet->bytemap[STATE_CHAN6_LO],
	    packet->bytemap[STATE_CHAN6_HI],

	    packet->bytemap[STATE_BUTTON_STATES]);


}
