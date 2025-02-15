#ifndef _TIME_H
#define _TIME_H

#ifndef _TIME_T
#define _TIME_T
typedef long time_t;		// 从GMT 1970 年1 月1 日开始的以秒计数的时间（日历时间）。
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

#define CLOCKS_PER_SEC 100	// 系统时钟滴答频率，100HZ。

typedef long clock_t;		// 从进程开始系统经过的时钟滴答数。

struct tm
{
  int tm_sec;			// 秒数 [0，59]。
  int tm_min;			// 分钟数 [ 0，59]。
  int tm_hour;			// 小时数 [0，59]。
  int tm_mday;			// 1 个月的天数 [0，31]。
  int tm_mon;			// 1 年中月份 [0，11]。
  int tm_year;			// 从1900 年开始的年数。
  int tm_wday;			// 1 星期中的某天 [0，6]（星期天 =0）。
  int tm_yday;			// 1 年中的某天 [0，365]。
  int tm_isdst;			// 夏令时标志。
};

// 以下是有关时间操作的函数原型。
// 确定处理器使用时间。返回程序所用处理器时间（滴答数）的近似值。
clock_t clock (void);
// 取时间（秒数）。返回从1970.1.1:0:0:0 开始的秒数（称为日历时间）。
time_t time (time_t * tp);
// 计算时间差。返回时间time2 与time1 之间经过的秒数。
double difftime (time_t time2, time_t time1);
// 将tm 结构表示的时间转换成日历时间。
time_t mktime (struct tm *tp);

// 将tm 结构表示的时间转换成一个字符串。返回指向该串的指针。
char *asctime (const struct tm *tp);
// 将日历时间转换成一个字符串形式，如“Wed Jun 30 21:49:08:1993\n”。
char *ctime (const time_t * tp);
// 将日历时间转换成tm 结构表示的UTC 时间（UTC - 世界时间代码Universal Time Code）。
struct tm *gmtime (const time_t * tp);
// 将日历时间转换成tm 结构表示的指定时间区(timezone)的时间。
struct tm *localtime (const time_t * tp);
// 将tm 结构表示的时间利用格式字符串fmt 转换成最大长度为smax 的字符串并将结果存储在s 中。
size_t strftime (char *s, size_t smax, const char *fmt, const struct tm *tp);
// 初始化时间转换信息，使用环境变量TZ，对zname 变量进行初始化。
// 在与时间区相关的时间转换函数中将自动调用该函数。
void tzset (void);

#endif
	void exit(int status);
	void _exit(int status);
	int fcntl(int fildes, int cmd, ...);
	int fork(void);
	int getpid(void);
	int getuid(void);
	int geteuid(void);
	int getgid(void);
	int getegid(void);
	int ioctl(int fildes, int cmd, ...);
	int kill(pid_t pid, int signal);
	int link(const char * filename1, const char * filename2);
	int lseek(int fildes, off_t offset, int origin);
	int mknod(const char * filename, mode_t mode, dev_t dev);
	int mount(const char * specialfile, const char * dir, int rwflag);
	int nice(int val);
	int open(const char * filename, int flag, ...);
	int pause(void);
	int pipe(int * fildes);
	int read(int fildes, char * buf, off_t count);
	int setpgrp(void);
	int setpgid(pid_t pid,pid_t pgid);
	int setuid(uid_t uid);
	int setgid(gid_t gid);
	void (*signal(int sig, void (*fn)(int)))(int);
	int stat(const char * filename, struct stat * stat_buf);
	int fstat(int fildes, struct stat * stat_buf);
	int stime(time_t * tptr);
	int sync(void);
	time_t time(time_t * tloc);
	time_t times(struct tms * tbuf);
	int ulimit(int cmd, long limit);
	mode_t umask(mode_t mask);
	int umount(const char * specialfile);
	int uname(struct utsname * name);
	int unlink(const char * filename);
	int ustat(dev_t dev, struct ustat * ubuf);
	int utime(const char * filename, struct utimbuf * times);
	pid_t waitpid(pid_t pid,int * wait_stat,int options);
	pid_t wait(int * wait_stat);
	int write(int fildes, const char * buf, off_t count);
	int dup2(int oldfd, int newfd);
	int getppid(void);
	pid_t getpgrp(void);
	pid_t setsid(void);
#endif
