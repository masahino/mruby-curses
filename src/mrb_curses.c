/*
** mrb_curses.c - Curses class
**
** Copyright (c) Kotaro Hibi 2014
**
** See Copyright Notice in LICENSE
*/

#include "mruby.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/string.h"
#include "mrb_curses.h"
#include <ncurses.h>

#define DONE mrb_gc_arena_restore(mrb, 0);

// static mrb_value rb_stdscr;

struct windata {
  WINDOW *window;
};

static void mrb_curses_window_free(mrb_state *mrb, void *ptr) {
  struct windata *winp = ptr;
  fprintf(stderr, "curses_window_free %p (stdscr = %p\n", winp->window, stdscr);
  if (winp->window && winp->window != stdscr) {
    //    delwin(winp->window);
  }
  winp->window = NULL;
  mrb_free(mrb, ptr);
}

WINDOW *echo_win = NULL;

/*
typedef struct {
  char *str;
  int len;
} mrb_curses_data;

static const struct mrb_data_type mrb_curses_data_type = {
  "mrb_curses_data", mrb_free,
};
*/

static const struct mrb_data_type mrb_curses_window_data_type = {
  "mrb_curses_window_data", mrb_curses_window_free,
};

/*
static mrb_value mrb_curses_init(mrb_state *mrb, mrb_value self)
{
  mrb_curses_data *data;

  data = (mrb_curses_data *)DATA_PTR(self);
  if (data) {
    mrb_free(mrb, data);
  }
  DATA_TYPE(self) = &mrb_curses_data_type;
  DATA_PTR(self) = NULL;

  data = (mrb_curses_data *)mrb_malloc(mrb, sizeof(mrb_curses_data));
  data->str = "dummy";
  data->len = 5;
  DATA_PTR(self) = data;

  return self;
}
*/

static mrb_value
mrb_curses_initscr(mrb_state *mrb, mrb_value self)
{
  // if (rb_stdscr) return rb_stdscr;
  initscr();
  /*
  if (stdscr == 0) {
    rb_raise(rb_eRuntimeError, "can't initialize curses");
  }
  */
  clear();
  // rb_stdscr = prep_window(cWindow, stdscr);
  // return rb_stdscr;
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_endwin(mrb_state *mrb, mrb_value self)
{
  endwin();
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_cbreak(mrb_state *mrb, mrb_value self)
{
  cbreak();
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_nocbreak(mrb_state *mrb, mrb_value self)
{
  nocbreak();
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_echo(mrb_state *mrb, mrb_value self)
{
  echo();
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_noecho(mrb_state *mrb, mrb_value self)
{
  noecho();
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_clear(mrb_state *mrb, mrb_value self)
{
  clear();
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_addstr(mrb_state *mrb, mrb_value self)
{
  mrb_value obj;

  mrb_get_args(mrb, "S", &obj);
  const char *body = mrb_string_value_ptr(mrb, obj);
  addstr(body);
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_keypad(mrb_state *mrb, mrb_value self)
{
  mrb_bool obj = TRUE;
  mrb_get_args(mrb, "|b", &obj);
  keypad(stdscr, obj);
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_move(mrb_state *mrb, mrb_value self)
{
  mrb_value v1, v2;
  mrb_get_args(mrb, "ii", &v1, &v2);
  move(mrb_fixnum(v1), mrb_fixnum(v2));
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_start_color(mrb_state *mrb, mrb_value self)
{
  start_color();
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_init_pair(mrb_state *mrb, mrb_value self)
{
  mrb_int pair, f, b;
  mrb_get_args(mrb, "iii", &pair, &f, &b);
  return mrb_fixnum_value(init_pair(pair, f, b));
//  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_init_color(mrb_state *mrb, mrb_value self)
{
  mrb_int color, r, g, b;
  mrb_int ret;
  mrb_get_args(mrb, "iiii", &color, &r, &g, &b);

  ret = init_color(color, r, g, b);
  ret = init_color(1, 0, 0, 0);
  if (ret == OK) {
    return mrb_true_value();
  } else {
    return mrb_false_value();
  }
}

static mrb_value
mrb_curses_coloron(mrb_state *mrb, mrb_value self)
{
  mrb_value v1;

  mrb_get_args(mrb, "i", &v1);
  int no = mrb_fixnum(v1);
  attron(COLOR_PAIR(no));
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_coloroff(mrb_state *mrb, mrb_value self)
{
  mrb_value v1;

  mrb_get_args(mrb, "i", &v1);
  int no = mrb_fixnum(v1);
  attroff(COLOR_PAIR(no));
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_wbkgd(mrb_state *mrb, mrb_value self)
{
  mrb_value v1;

  mrb_get_args(mrb, "i", &v1);
  int no = mrb_fixnum(v1);
  wbkgd(stdscr, COLOR_PAIR(no));
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_echoline(mrb_state *mrb, mrb_value self)
{
  int h, w;
  mrb_value height, width;

  mrb_get_args(mrb, "ii", &height, &width);
  h = mrb_fixnum(height);
  w = mrb_fixnum(width);

  if (echo_win == NULL) {
    echo_win = subwin(stdscr, 1, 100, h, w);
  }

  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_ewaddstr(mrb_state *mrb, mrb_value self)
{
  if (echo_win == NULL) {
    return mrb_bool_value(false);
  }

  mrb_value obj;
  mrb_get_args(mrb, "S", &obj);
  const char *body = mrb_string_value_ptr(mrb, obj);
  waddstr(echo_win, body);

  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_ewmove(mrb_state *mrb, mrb_value self)
{
  if (echo_win == NULL) {
    return mrb_bool_value(false);
  }

  mrb_value v1, v2;
  mrb_get_args(mrb, "ii", &v1, &v2);
  wmove(echo_win, mrb_fixnum(v1), mrb_fixnum(v2));
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_refresh(mrb_state *mrb, mrb_value self)
{
  refresh();
  if (echo_win != NULL) {
    wrefresh(echo_win);
  }

  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_ewgetstr(mrb_state *mrb, mrb_value self)
{
  if (echo_win == NULL) {
    return mrb_bool_value(false);
  }

  char str[100];
  mrb_value s;
  echo();
  nocbreak();
  wgetstr(echo_win, str);
  noecho();
  cbreak();
  s =  mrb_str_new_cstr(mrb, str);
  return s;
}

static mrb_value
mrb_curses_screen_rows(mrb_state *mrb, mrb_value self)
{
  int rows, cols;
  getmaxyx(stdscr, rows, cols);
  return mrb_fixnum_value(rows);
}

static mrb_value
mrb_curses_screen_cols(mrb_state *mrb, mrb_value self)
{
  int rows, cols;
  getmaxyx(stdscr, rows, cols);
  return mrb_fixnum_value(cols);
}

static mrb_value
mrb_curses_raw(mrb_state *mrb, mrb_value self)
{
  int ret;
  ret = raw();
  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_curses_noraw(mrb_state *mrb, mrb_value self)
{
  int ret;
  ret = noraw();
  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_curses_getch(mrb_state *mrb, mrb_value self)
{
  int ret;
  ret = getch();
  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_curses_curs_set(mrb_state *mrb, mrb_value self)
{
  int ret;
  mrb_int visibility;

  mrb_get_args(mrb, "i", &visibility);
  ret = curs_set(visibility);
  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_curses_stdscr(mrb_state *mrb, mrb_value self)
{
  mrb_value win_obj;
  struct windata *windata;
  if (stdscr == NULL) {
    initscr();
  }
  windata = (struct windata *)mrb_malloc(mrb, sizeof(struct windata));
  windata->window = stdscr;
  win_obj = mrb_obj_value(mrb_data_object_alloc(mrb, mrb_class_get_under(mrb, mrb_class_get(mrb, "Curses"), "Window"), windata, &mrb_curses_window_data_type));
  return win_obj;
}

static mrb_value
mrb_curses_lines(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(LINES);
}

static mrb_value
mrb_curses_cols(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(COLS);
}

static mrb_value
mrb_curses_use_default_colors(mrb_state *mrb, mrb_value self)
{
  use_default_colors();
  return mrb_nil_value();
}

static mrb_value
mrb_curses_window_init(mrb_state *mrb, mrb_value self)
{
  mrb_int nlines, ncols, begin_y, begin_x;
  struct windata *winp;
  WINDOW *win;

  winp = (struct windata *)mrb_malloc(mrb, sizeof(struct windata));
  DATA_TYPE(self) = &mrb_curses_window_data_type;
  DATA_PTR(self) = NULL;

  mrb_get_args(mrb, "iiii", &nlines, &ncols, &begin_y, &begin_x);
  win = newwin(nlines, ncols, begin_y, begin_x);
  winp->window = win;
  DATA_PTR(self) = winp;
  return self;
}

static mrb_value
mrb_curses_window_resize(mrb_state *mrb, mrb_value self)
{
  mrb_int lines, cols, ret;
  struct windata *winp = DATA_PTR(self);

  mrb_get_args(mrb, "ii", &lines, &cols);
  ret = wresize(winp->window, lines, cols);
  if (ret == OK) {
    return mrb_true_value();
  } else {
    return mrb_false_value();
  }
}

static mrb_value
mrb_curses_window_addstr(mrb_state *mrb, mrb_value self)
{
  mrb_int ret;
  char *str;
  struct windata *winp = DATA_PTR(self);

  mrb_get_args(mrb, "z", &str);
  ret = waddstr(winp->window, str);
  if (ret == OK) {
    return mrb_true_value();
  } else {
    return mrb_false_value();
  }
}

static mrb_value
mrb_curses_window_mvwin(mrb_state *mrb, mrb_value self)
{
  mrb_int x, y;
  struct windata *winp = DATA_PTR(self);

  mrb_get_args(mrb, "ii", &y, &x);
  mvwin(winp->window, y, x);
  return mrb_nil_value();
}

static mrb_value
mrb_curses_window_refresh(mrb_state *mrb, mrb_value self)
{
  struct windata *winp = DATA_PTR(self);

  wrefresh(winp->window);
  return mrb_nil_value();
}

static mrb_value
mrb_curses_window_attrset(mrb_state *mrb, mrb_value self)
{
  struct windata *winp = DATA_PTR(self);
  mrb_int attr;

  mrb_get_args(mrb, "i", &attr);
  return mrb_fixnum_value(wattrset(winp->window, attr));
}

static mrb_value
mrb_curses_window_bkgd(mrb_state *mrb, mrb_value self)
{
  struct windata *winp = DATA_PTR(self);
  mrb_int attr;

  mrb_get_args(mrb, "i", &attr);
  return mrb_fixnum_value(wbkgd(winp->window, attr));
}

static mrb_value
mrb_curses_window_clear(mrb_state *mrb, mrb_value self)
{
  struct windata *winp = DATA_PTR(self);

  return mrb_fixnum_value(wclear(winp->window));
}

static mrb_value
mrb_curses_window_move(mrb_state *mrb, mrb_value self)
{
  struct windata *winp = DATA_PTR(self);
  mrb_int x, y;

  mrb_get_args(mrb, "ii", &y, &x);
  return mrb_fixnum_value(wmove(winp->window, y, x));
}

void mrb_mruby_curses_gem_init(mrb_state *mrb)
{
    struct RClass *curses, *window;
    curses = mrb_define_class(mrb, "Curses", mrb->object_class);
    MRB_SET_INSTANCE_TT(curses, MRB_TT_DATA);

/*    mrb_define_method(mrb, curses, "initialize", mrb_curses_init, MRB_ARGS_NONE()); */
    mrb_define_class_method(mrb, curses, "initscr", mrb_curses_initscr, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "cbreak", mrb_curses_cbreak, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "nocbreak", mrb_curses_nocbreak, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "echo", mrb_curses_echo, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "noecho", mrb_curses_noecho, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "keypad", mrb_curses_keypad, MRB_ARGS_ANY());
    mrb_define_class_method(mrb, curses, "clear", mrb_curses_clear, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "addstr", mrb_curses_addstr, MRB_ARGS_REQ(1));
    mrb_define_class_method(mrb, curses, "move", mrb_curses_move, MRB_ARGS_ANY());
    mrb_define_class_method(mrb, curses, "endwin", mrb_curses_endwin, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "start_color", mrb_curses_start_color, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "init_pair", mrb_curses_init_pair, MRB_ARGS_ANY());
    mrb_define_class_method(mrb, curses, "init_color", mrb_curses_init_color, MRB_ARGS_REQ(4));
    mrb_define_class_method(mrb, curses, "coloron", mrb_curses_coloron, MRB_ARGS_REQ(1));
    mrb_define_class_method(mrb, curses, "coloroff", mrb_curses_coloroff, MRB_ARGS_REQ(1));
    mrb_define_class_method(mrb, curses, "wbkgd", mrb_curses_wbkgd, MRB_ARGS_REQ(1));

    mrb_define_class_method(mrb, curses, "echoline", mrb_curses_echoline, MRB_ARGS_ANY());
    mrb_define_class_method(mrb, curses, "ewaddstr", mrb_curses_ewaddstr, MRB_ARGS_REQ(1));
    mrb_define_class_method(mrb, curses, "ewmove", mrb_curses_ewmove, MRB_ARGS_ANY());
    mrb_define_class_method(mrb, curses, "refresh", mrb_curses_refresh, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "ewgetstr", mrb_curses_ewgetstr, MRB_ARGS_NONE());

    mrb_define_class_method(mrb, curses, "screen_rows", mrb_curses_screen_rows, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "screen_cols", mrb_curses_screen_cols, MRB_ARGS_NONE());

    mrb_define_class_method(mrb, curses, "raw", mrb_curses_raw, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "noraw", mrb_curses_noraw, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "getch", mrb_curses_getch, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "curs_set", mrb_curses_curs_set, MRB_ARGS_REQ(1));
    mrb_define_class_method(mrb, curses, "stdscr", mrb_curses_stdscr, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "lines", mrb_curses_lines, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "cols", mrb_curses_cols, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "mvwin", mrb_curses_cols, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "use_default_colors", mrb_curses_use_default_colors, MRB_ARGS_NONE());

    mrb_define_const(mrb, curses, "COLOR_BLACK",  mrb_fixnum_value(COLOR_BLACK));
    mrb_define_const(mrb, curses, "COLOR_RED",  mrb_fixnum_value(COLOR_RED));
    mrb_define_const(mrb, curses, "COLOR_GREEN",  mrb_fixnum_value(COLOR_GREEN));
    mrb_define_const(mrb, curses, "COLOR_YELLOW",  mrb_fixnum_value(COLOR_YELLOW));
    mrb_define_const(mrb, curses, "COLOR_BLUE",  mrb_fixnum_value(COLOR_BLUE));
    mrb_define_const(mrb, curses, "COLOR_MAGENTA",  mrb_fixnum_value(COLOR_MAGENTA));
    mrb_define_const(mrb, curses, "COLOR_CYAN",  mrb_fixnum_value(COLOR_CYAN));
    mrb_define_const(mrb, curses, "COLOR_WHITE",  mrb_fixnum_value(COLOR_WHITE));

    mrb_define_const(mrb, curses, "A_ATTRIBUTES",  mrb_fixnum_value(A_ATTRIBUTES));
    mrb_define_const(mrb, curses, "A_CHARTEXT",  mrb_fixnum_value(A_CHARTEXT));
    mrb_define_const(mrb, curses, "A_COLOR",  mrb_fixnum_value(A_COLOR));
    mrb_define_const(mrb, curses, "A_STANDOUT",  mrb_fixnum_value(A_STANDOUT));
    mrb_define_const(mrb, curses, "A_UNDERLINE",  mrb_fixnum_value(A_UNDERLINE));
    mrb_define_const(mrb, curses, "A_REVERSE",  mrb_fixnum_value(A_REVERSE));
    mrb_define_const(mrb, curses, "A_BLINK",  mrb_fixnum_value(A_BLINK));
    mrb_define_const(mrb, curses, "A_DIM",  mrb_fixnum_value(A_DIM));
    mrb_define_const(mrb, curses, "A_BOLD",  mrb_fixnum_value(A_BOLD));
    mrb_define_const(mrb, curses, "A_ALTCHARSET",  mrb_fixnum_value(A_ALTCHARSET));
    mrb_define_const(mrb, curses, "A_INVIS",  mrb_fixnum_value(A_INVIS));
    mrb_define_const(mrb, curses, "A_PROTECT",  mrb_fixnum_value(A_PROTECT));
    mrb_define_const(mrb, curses, "A_HORIZONTAL",  mrb_fixnum_value(A_HORIZONTAL));
    mrb_define_const(mrb, curses, "A_LEFT",  mrb_fixnum_value(A_LEFT));
    mrb_define_const(mrb, curses, "A_LOW",  mrb_fixnum_value(A_LOW));
    mrb_define_const(mrb, curses, "A_RIGHT",  mrb_fixnum_value(A_RIGHT));
    mrb_define_const(mrb, curses, "A_TOP",  mrb_fixnum_value(A_TOP));
    mrb_define_const(mrb, curses, "A_VERTICAL",  mrb_fixnum_value(A_VERTICAL));

    mrb_define_const(mrb, curses, "COLORS",  mrb_fixnum_value(COLORS));

    window = mrb_define_class_under(mrb, curses, "Window", mrb->object_class);
    MRB_SET_INSTANCE_TT(window, MRB_TT_DATA);
    mrb_define_method(mrb, window, "initialize", mrb_curses_window_init, MRB_ARGS_REQ(4));
    mrb_define_method(mrb, window, "resize", mrb_curses_window_resize, MRB_ARGS_REQ(2));
    mrb_define_method(mrb, window, "addstr", mrb_curses_window_addstr, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, window, "mvwin", mrb_curses_window_mvwin, MRB_ARGS_REQ(2));
    mrb_define_method(mrb, window, "refresh", mrb_curses_window_refresh, MRB_ARGS_NONE());
    mrb_define_method(mrb, window, "attrset", mrb_curses_window_attrset, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, window, "bkgd", mrb_curses_window_bkgd, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, window, "clear", mrb_curses_window_clear, MRB_ARGS_NONE());
    mrb_define_method(mrb, window, "move", mrb_curses_window_move, MRB_ARGS_REQ(2));

    DONE;
}

void mrb_mruby_curses_gem_final(mrb_state *mrb)
{
}
