/*\
 *  
 *  Copyright (C) 2004-2014 VOCORD, Inc. <info@vocord.com>
 *
 * This file is part of the P3SS API/ABI/VERIFICATION system.
 *
 * The P3SS API/ABI/VERIFICATION system is free software; you can
 * redistribute it and/or modify it under the terms of the
 * GNU Lesser General Public License
 * as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * The part of the P3SS API/ABI/VERIFICATION system
 * is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with the part of the P3SS API/ABI/VERIFICATION system;
 * if not, see <http://www.gnu.org/licenses/>.
 *
 *
 *       Component for conform portion of the system.
\*/

/*\
 *         Brief: файл заголовков <ui/osd.h>
 *
 *   Description:
 *     OSD helper decls
 *
 *       Remarks:
 *      Each module should have elementary testing functionality
 *
 *   Dependencies: <inttypes.h>
\*/
#ifndef __UI_OSD_H__
#define __UI_OSD_H__

#include <inttypes.h>
#include <sys/time.h>

#ifndef __CYGWIN__
#include <linux/input.h>
#endif

#define OSD_DECODER_REGISTER(X)	X, .max	= ARRAY_ELEMS(X)

#ifdef __cplusplus
extern "C" {
#endif
struct osd_ctx_s;
typedef struct osd_ctx_s {
	uint32_t	*data;
	uint16_t	*base;
	void		*base_;

	uint8_t		*font;
	uint8_t		*font_;

	int		w;
	int		h;

	int		f_w;
	int		f_dw;
	int		f_dh;

	int		f_size;

	uint32_t	*voxel;
	uint32_t	*colors;

	void		(*putc_xy)(struct osd_ctx_s *,  int x, int y, int);
	void		(*puts_xy)(struct osd_ctx_s *,  int x, int y, const char *);
	void		(*puts_xyc)(struct osd_ctx_s *, int x, int y, const char *, int);

	void		(*refresh)(struct osd_ctx_s *,  int flag);
	int		(*read)(struct osd_ctx_s *,  const char *name, long);

	int		flag;
	void		*lines;		/* ASCII Display */
	void		*ctrl_s;	/* controll context */
} osd_ctx_t;

typedef struct {
	const char	*title;
	int		val;
} osd_str_dec_t;

typedef struct {
	osd_str_dec_t	*dec;
	const char	*clr;		/* clear string */
	/* idx */
	int		max;

	int		new;
	int		old;

	int		flags;		/* flags */

	const char	*n_fmt;		/* new val string */
	int16_t		n_x;		/* new val X */
	int16_t		n_y;		/* new val Y */

	const char	*i_fmt;		/* indicator val string */
	int16_t		i_x;		/* indicator val X */
	int16_t		i_y;		/* indicator val Y */
} osd_mode_desc_t;

typedef struct point_dev_s {
	struct timeval	ts0;
	struct timeval	ts1;

	struct timeval	ts;
	struct timeval	tsd;

	int_least16_t	x;
	int_least16_t	y;

	int_least16_t	x0;
	int_least16_t	x1;

	int_least16_t	y0;
	int_least16_t	y1;

	int		state;
	int		mode;
	int		mode1;

	int		val;
	int		val_min;
	int		val_max;

	int		vec;
	int		vec_l;
	int		vec_r;

	int		sign;
	int		lin;
	int		step;
	int		exp;
	int		exp_min;
	int		exp_max;

	int_least16_t	sdx;
	int_least16_t	sdy;
	struct timeval	sts;

	int_least16_t	x_;
	int_least16_t	y_;

	void		*ui;	/* UI CTX */
	int		ui_cnt;	/* UI CTX count */

	osd_ctx_t	*osd;	/* OSD CTX */

	void		*ctx0;	/* priv ctx 0 */
	void		*ctx1;	/* priv ctx 1 */
} point_dev_t;

struct event_mode_s;
typedef int	(*event_cb_f)(point_dev_t *pos, struct input_event *event);
typedef int	(*event_act_f)(point_dev_t *pos);
typedef struct event_mode_s {
	char		*descr;
	osd_mode_desc_t	*mode;
	event_cb_f	f_abs;
	event_cb_f	f_tmout;

	int16_t		x;
	int16_t		y;
//	event_act_f	f_focus;
//	event_act_f	f_leave;
} event_mode_t;

extern void osd_str_showxyc(
	osd_ctx_t	*osd,
	int		x,
	int		y,
	const char	*str,
	int		color
);

extern void osd_font_show(osd_ctx_t *osd);
extern void osd_font_read(char *name, uint8_t *font);

#ifdef __cplusplus
}
#endif

#endif

