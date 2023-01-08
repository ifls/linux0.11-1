//// 读取fs 段中指定地址处的字节。
// 参数：addr - 指定的内存地址。
// %0 - (返回的字节_v)；%1 - (内存地址addr)。
// 返回：返回内存fs:[addr]处的字节。
extern _inline unsigned char
get_fs_byte (const char *addr)
{
//  unsigned register char _v;

// __asm__ ("movb %%fs:%1,%0": "=r" (_v):"m" (*addr));
  _asm mov ebx,addr
  _asm mov al,byte ptr fs:[ebx];
//  _asm mov _v,al;
//  return _v;
}

//// 读取fs 段中指定地址处的字。
// 参数：addr - 指定的内存地址。
// %0 - (返回的字_v)；%1 - (内存地址addr)。
// 返回：返回内存fs:[addr]处的字。
extern _inline unsigned short
get_fs_word (const unsigned short *addr)
{
//  unsigned short _v;

//__asm__ ("movw %%fs:%1,%0": "=r" (_v):"m" (*addr));
  _asm mov ebx,addr
  _asm mov ax,word ptr fs:[ebx];
//  _asm mov _v,ax;
//  return _v;
}

//// 读取fs 段中指定地址处的长字(4 字节)。
// 参数：addr - 指定的内存地址。
// %0 - (返回的长字_v)；%1 - (内存地址addr)。
// 返回：返回内存fs:[addr]处的长字。
extern _inline unsigned long
get_fs_long (const unsigned long *addr)
{
//  unsigned long _v;

//__asm__ ("movl %%fs:%1,%0": "=r" (_v):"m" (*addr));
  _asm mov ebx,addr
  _asm mov eax,dword ptr fs:[ebx];
//  _asm mov _v,eax;
//  return _v;
}

//// 将一字节存放在fs 段中指定内存地址处。
// 参数：val - 字节值；addr - 内存地址。
// %0 - 寄存器(字节值val)；%1 - (内存地址addr)。
extern _inline void
put_fs_byte (char val, char *addr)//passed
{
//  __asm__ ("movb %0,%%fs:%1"::"r" (val), "m" (*addr));
	_asm mov ebx,addr
	_asm mov al,val;
	_asm mov byte ptr fs:[ebx],al;
}

//// 将一字存放在fs 段中指定内存地址处。
// 参数：val - 字值；addr - 内存地址。
// %0 - 寄存器(字值val)；%1 - (内存地址addr)。
extern _inline void
put_fs_word (short val, short *addr)
{
//  __asm__ ("movw %0,%%fs:%1"::"r" (val), "m" (*addr));
	_asm mov ebx,addr
	_asm mov ax,val;
	_asm mov word ptr fs:[ebx],ax;
}

//// 将一长字存放在fs 段中指定内存地址处。
// 参数：val - 长字值；addr - 内存地址。
// %0 - 寄存器(长字值val)；%1 - (内存地址addr)。
extern _inline void
put_fs_long (long val, unsigned long *addr)
{
//  __asm__ ("movl %0,%%fs:%1"::"r" (val), "m" (*addr));
	_asm mov ebx,addr
	_asm mov eax,val;
	_asm mov dword ptr fs:[ebx],eax;
}

/*
* 比我更懂GNU 汇编的人应该仔细检查下面的代码。这些代码能使用，但我不知道是否
* 含有一些小错误。
* --- TYT，1991 年11 月24 日
* [ 这些代码没有错误，Linus ]
*/

//// 取fs 段寄存器值(选择符)。
// 返回：fs 段寄存器值。
extern _inline unsigned short
get_fs ()
{
//  unsigned short _v;
//__asm__ ("mov %%fs,%%ax": "=a" (_v):);
  _asm mov ax,fs;
//  _asm mov _v,ax;
//  return _v;
}

//// 取ds 段寄存器值。
// 返回：ds 段寄存器值。
extern _inline unsigned short
get_ds ()
{
//  unsigned short _v;
//__asm__ ("mov %%ds,%%ax": "=a" (_v):);
  _asm mov ax,fs;
//  _asm mov _v,ax;
//  return _v;
}

//// 设置fs 段寄存器。
// 参数：val - 段值（选择符）。
extern _inline void
set_fs (unsigned long val)
{
//  __asm__ ("mov %0,%%fs"::"a" ((unsigned short) val));
	_asm mov eax,val;
	_asm mov fs,ax;
}
	{
		verify_area(tloc, 4);				   // 验证内存容量是否够（这里是4 字节）。
		put_fs_long(i, (unsigned long *)tloc); // 也放入用户数据段tloc 处。
	}
	return i;
}

/*
 * Unprivileged users may change the real user id to the effective uid
 * or vice versa.
 */
/*
 * 无特权的用户可以见实际用户标识符(real uid)改成有效用户标识符(effective uid)，反之也然。
 */
// 设置任务的实际以及/或者有效用户ID（uid）。如果任务没有超级用户特权，那么只能互换其
// 实际用户ID 和有效用户ID。如果任务具有超级用户特权，就能任意设置有效的和实际的用户ID。
// 保留的uid（saved uid）被设置成与有效uid 同值。
int sys_setreuid(int ruid, int euid)
{
	int old_ruid = current->uid;

	if (ruid > 0)
	{
		if ((current->euid == ruid) || (old_ruid == ruid) || suser())
			current->uid = ruid;
		else
			return (-EPERM);
	}
	if (euid > 0)
	{
		if ((old_ruid == euid) || (current->euid == euid) || suser())
			current->euid = euid;
		else
		{
			current->uid = old_ruid;
			return (-EPERM);
		}
	}
	return 0;
}

// 设置任务用户号(uid)。如果任务没有超级用户特权，它可以使用setuid()将其有效uid
// （effective uid）设置成其保留uid(saved uid)或其实际uid(real uid)。如果任务有
// 超级用户特权，则实际uid、有效uid 和保留uid 都被设置成参数指定的uid。
int sys_setuid(int uid)
{
	return (sys_setreuid(uid, uid));
}

// 设置系统时间和日期。参数tptr 是从1970 年1 月1 日00:00:00 GMT 开始计时的时间值（秒）。
// 调用进程必须具有超级用户权限。
int sys_stime(long *tptr)
{
	if (!suser()) // 如果不是超级用户则出错返回（许可）。
		return -EPERM;
	startup_time = get_fs_long((unsigned long *)tptr) - jiffies / HZ;
	return 0;
}

// 获取当前任务时间。tms 结构中包括用户时间、系统时间、子进程用户时间、子进程系统时间。
int sys_times(struct tms *tbuf)
{
	if (tbuf)
	{
		verify_area(tbuf, sizeof *tbuf);
		put_fs_long(current->utime, (unsigned long *)&tbuf->tms_utime);
		put_fs_long(current->stime, (unsigned long *)&tbuf->tms_stime);
		put_fs_long(current->cutime, (unsigned long *)&tbuf->tms_cutime);
		put_fs_long(current->cstime, (unsigned long *)&tbuf->tms_cstime);
	}
	return jiffies;
}

// 当参数end_data_seg 数值合理，并且系统确实有足够的内存，而且进程没有超越其最大数据段大小
// 时，该函数设置数据段末尾为end_data_seg 指定的值。该值必须大于代码结尾并且要小于堆栈
// 结尾16KB。返回值是数据段的新结尾值（如果返回值与要求值不同，则表明有错发生）。
// 该函数并不被用户直接调用，而由libc 库函数进行包装，并且返回值也不一样。
int sys_brk(unsigned long end_data_seg)
{
	if (end_data_seg >= current->end_code &&		 // 如果参数>代码结尾，并且
		end_data_seg < current->start_stack - 16384) // 小于堆栈-16KB，
		current->brk = end_data_seg;				 // 则设置新数据段结尾值。
	return current->brk;							 // 返回进程当前的数据段结尾值。
}

/*
 * This needs some heave checking ...
 * I just haven't get the stomach for it. I also don't fully
 * understand sessions/pgrp etc. Let somebody who does explain it.
 */
/*
 * 下面代码需要某些严格的检查…
 * 我只是没有胃口来做这些。我也不完全明白sessions/pgrp 等。还是让了解它们的人来做吧。
 */
// 将参数pid 指定进程的进程组ID 设置成pgid。如果参数pid=0，则使用当前进程号。如果
// pgid 为0，则使用参数pid 指定的进程的组ID 作为pgid。
// 如果该函数用于将进程从一个进程组移到另一个进程组，则这两个进程组必须属于同一个会话(session)。
// 在这种情况下，参数pgid 指定了要加入的现有进程组ID，此时该组的会话ID 必须与将要加入进程的相同(193 行)。
// pid, pgid
// 0    0     
// 0    val
// val  0
// val  val
int sys_setpgid(int pid, int pgid)
{
	int i;

	if (!pid) // 如果参数pid=0，则使用当前进程号。
		pid = current->pid;
	if (!pgid)					   // 如果pgid 为0，则使用当前进程pid 作为pgid。
		pgid = current->pid;	   // [??这里与POSIX 的描述有出入]
	for (i = 0; i < NR_TASKS; i++) // 扫描任务数组，查找指定进程号的任务。
		if (task[i] && task[i]->pid == pid)
		{
			if (task[i]->leader) // 如果该任务已经是首领，则出错返回。 会话leader 无法切换组
				return -EPERM;
			if (task[i]->session != current->session) // 如果该任务的会话ID
				return -EPERM;						  // 与当前进程的不同，则出错返回。
			task[i]->pgrp = pgid;					  // 设置该任务的pgrp。
			return 0;
		}
	return -ESRCH;
}

// 返回当前进程的组号。与getpgid(0)等同。
int sys_getpgrp(void)
{
	return current->pgrp;
}

// 创建一个会话(session)（即设置其leader=1），并且设置其会话=其组号=其进程号。
int sys_setsid(void)
{
	if (current->leader && !suser())				 // 如果当前进程已是会话首领并且不是超级用户
		return -EPERM;								 // 则出错返回。
	current->leader = 1;							 // 设置当前进程为新会话首领。
	current->session = current->pgrp = current->pid; // 设置本进程session = pid。
	current->tty = -1;								 // 表示当前进程没有控制终端。
	return current->pgrp;							 // 返回会话ID。
}

// 获取系统信息。其中utsname 结构包含5 个字段，分别是：本版本操作系统的名称、网络节点名称、
// 当前发行级别、版本级别和硬件类型名称。
int sys_uname(struct utsname *name)
{
	static struct utsname thisname = {// 这里给出了结构中的信息，这种编码肯定会改变。
									  "linux .0", "nodename", "release ", "version ", "machine "};
	int i;

	if (!name)
		return -ERROR;				   // 如果存放信息的缓冲区指针为空则出错返回。
	verify_area(name, sizeof *name);   // 验证缓冲区大小是否超限（超出已分配的内存等）。
	for (i = 0; i < sizeof *name; i++) // 将utsname 中的信息逐字节复制到用户缓冲区中。
		put_fs_byte(((char *)&thisname)[i], i + (char *)name);
	return 0;
}

// 设置当前进程创建文件属性屏蔽码为mask & 0777。并返回原屏蔽码。
int sys_umask(int mask)
{
	int old = current->umask;

	current->umask = mask & 0777;
	return (old);
}
