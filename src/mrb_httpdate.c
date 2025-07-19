#include <string.h>
#include <time.h>
#include <mruby.h>
#include <mruby/class.h>
#include <mruby/presym.h>
#include <mruby/string.h>
#include <mruby/error.h>

#if (__GNUC__ >= 3) || (__INTEL_COMPILER >= 800) || defined(__clang__)
# define likely(x) __builtin_expect(!!(x), 1)
# define unlikely(x) __builtin_expect(!!(x), 0)
#else
# define likely(x) (x)
# define unlikely(x) (x)
#endif

#ifdef _WIN32
#ifdef _MSC_VER
/* Win32 platform do not provide gmtime_r/localtime_r; emulate them using gmtime_s/localtime_s */
#define gmtime_r(tp, tm)    ((gmtime_s((tm), (tp)) == 0) ? (tm) : NULL)
#else
#define NO_GMTIME_R
#endif
#endif
#ifdef __STRICT_ANSI__
/* Strict ANSI (e.g. -std=c99) do not provide gmtime_r/localtime_r */
#define NO_GMTIME_R
#endif

#ifdef NO_GMTIME_R
#define gmtime_r(t,r) gmtime(t)
#endif

static mrb_value
mrb_time_httpdate(mrb_state *mrb, mrb_value self)
{
  mrb_int timer = mrb_integer(mrb_type_convert(mrb, mrb_funcall_id(mrb, self, MRB_SYM(gmtime), 0), MRB_TT_INTEGER, MRB_SYM(to_i)));
  struct tm tp;
  struct tm *tm = gmtime_r(&timer, &tp);
  if (unlikely(!tm)) {
    mrb_sys_fail(mrb, "gmtime_r");
  }
  static const char *wday_name[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  static const char *mon_name[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  mrb_value httpdate = mrb_str_new(mrb, NULL, 29);
  char *ptr = RSTRING_PTR(httpdate);

  memcpy(ptr, wday_name[tm->tm_wday], 3);
  ptr += 3;
  *ptr++ = ',';
  *ptr++ = ' ';
  *ptr++ = '0' + (tm->tm_mday / 10);
  *ptr++ = '0' + (tm->tm_mday % 10);
  *ptr++ = ' ';
  memcpy(ptr, mon_name[tm->tm_mon], 3);
  ptr += 3;
  *ptr++ = ' ';
  *ptr++ = '0' + ((tm->tm_year + 1900) / 1000 % 10);
  *ptr++ = '0' + ((tm->tm_year + 1900) / 100 % 10);
  *ptr++ = '0' + ((tm->tm_year + 1900) / 10 % 10);
  *ptr++ = '0' + ((tm->tm_year + 1900) % 10);
  *ptr++ = ' ';
  *ptr++ = '0' + (tm->tm_hour / 10);
  *ptr++ = '0' + (tm->tm_hour % 10);
  *ptr++ = ':';
  *ptr++ = '0' + (tm->tm_min / 10);
  *ptr++ = '0' + (tm->tm_min % 10);
  *ptr++ = ':';
  *ptr++ = '0' + (tm->tm_sec / 10);
  *ptr++ = '0' + (tm->tm_sec % 10);
  *ptr++ = ' ';
  memcpy(ptr, "GMT", 3);

  return httpdate;
}

void
mrb_mruby_time_httpdate_gem_init(mrb_state* mrb)
{
  struct RClass *tc = mrb_class_get_id(mrb, MRB_SYM(Time));
  mrb_define_method_id(mrb, tc, MRB_SYM(httpdate), mrb_time_httpdate, MRB_ARGS_NONE());
}

void mrb_mruby_time_httpdate_gem_final(mrb_state* mrb) {}