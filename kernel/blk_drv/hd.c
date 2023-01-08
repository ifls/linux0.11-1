//// 硬件端口字节输出函数。
// 参数：value - 欲输出字节；port - 端口。
#define outb(value, port) _outb((unsigned char)(value), (unsigned short)(port))
void _inline _outb(unsigned char value, unsigned short port) // passed
{
	_asm mov dx, port;
	_asm mov al, value;
	_asm out dx, al;
}
//__asm__ ( "outb %%al,%%dx":: "a" (value), "d" (port))

//// 硬件端口字节输入函数。
// 参数：port - 端口。返回读取的字节。
#define inb(port) _inb((unsigned short)(port))
unsigned char _inline _inb(unsigned short port)
{
	//	unsigned char _v;
	_asm mov dx, port;
	_asm in al, dx;
	//	_asm mov _v,al ;
	//	return _v;
}
/*
unsigned char _v; \
__asm__ volatile ( "inb %%dx,%%al": "=a" (_v): "d" (port)); \
_v; \
})*/

//// 带延迟的硬件端口字节输出函数。
// 参数：value - 欲输出字节；port - 端口。
/*
#define outb_p(value,port) \
_asm {\
	_asm mov al,value \
	_asm mov dx,port \
	_asm out dx,al \
	_asm jmp l1 \
_asm l1: jmp l2 \
_asm l2: \
}
__asm__ ( "outb %%al,%%dx\n" \
"\tjmp 1f\n" \
"1:\tjmp 1f\n" \
"1:":: "a" (value), "d" (port))
*/
#define outb_p(value, port) _outb_p((unsigned char)(value), (unsigned short)(port))
_inline void _outb_p(unsigned char value, unsigned short port)
{
	_asm mov al, value _asm mov dx, port _asm out dx, al _asm jmp l1 _asm l1 : jmp l2 _asm l2:
}

//// 带延迟的硬件端口字节输入函数。
// 参数：port - 端口。返回读取的字节。
/*
#define inb_p(port) { \
volatile unsigned char _v; \
_asm { \
	_asm mov dx,port \
	_asm in al,dx \
	_asm mov _v,al \
	_asm jmp l1 \
_asm l1: jmp l2 \
_asm l2: \
} \
_v; \
}
unsigned char _v; \
__asm__ volatile ( "inb %%dx,%%al\n" \
"\tjmp 1f\n" \
"1:\tjmp 1f\n" \
"1:": "=a" (_v): "d" (port)); \
_v; \
})*/
#define inb_p(port) _inb_p((unsigned short)(port))
_inline unsigned char _inb_p(unsigned short port)
{
	//	volatile unsigned char _v;
	_asm { 
		mov dx,port 
		in al,dx
			//		mov _v,al 
		jmp l1 
	l1: jmp l2 
	l2:
	}
	//	return _v;
}
void (*DEVICE_INTR)(void) = NULL;
#endif
static void(DEVICE_REQUEST)(void);

// 释放锁定的缓冲区。
extern _inline void
unlock_buffer(struct buffer_head *bh)
{
	if (!bh->b_lock) // 如果指定的缓冲区bh 并没有被上锁，则显示警告信息。
		printk(DEVICE_NAME ": free buffer being unlocked\n");
	bh->b_lock = 0;		  // 否则将该缓冲区解锁。
	wake_up(&bh->b_wait); // 唤醒等待该缓冲区的进程。
}

// 结束请求。
extern _inline void
end_request(int uptodate)
{
	DEVICE_OFF(CURRENT->dev); // 关闭设备。
	if (CURRENT->bh)
	{										// CURRENT 为指定主设备号的当前请求结构。
		CURRENT->bh->b_uptodate = uptodate; // 置更新标志。
		unlock_buffer(CURRENT->bh);			// 解锁缓冲区。
	}
	if (!uptodate)
	{ // 如果更新标志为0 则显示设备错误信息。
		printk(DEVICE_NAME " I/O error\n\r");
		printk("dev %04x, block %d\n\r", CURRENT->dev, CURRENT->bh->b_blocknr);
	}
	wake_up(&CURRENT->waiting); // 唤醒等待该请求项的进程。
	wake_up(&wait_for_request); // 唤醒等待请求的进程。
	CURRENT->dev = -1;			// 释放该请求项。
	CURRENT = CURRENT->next;	// 从请求链表中删除该请求项。
}

// 定义初始化请求宏。
#define INIT_REQUEST                                                                                \
	repeat:                                                                                         \
	if (!CURRENT) /* 如果当前请求结构指针为null 则返回。*/                           \
		return;                                                                                     \
	if (MAJOR(CURRENT->dev) != MAJOR_NR) /* 如果当前设备的主设备号不对则死机。*/   \
		panic(DEVICE_NAME ": request list destroyed");                                              \
	if (CURRENT->bh)                                                                                \
	{                                                                                               \
		if (!CURRENT->bh->b_lock) /* 如果在进行请求操作时缓冲区没锁定则死机。*/ \
			panic(DEVICE_NAME ": block not locked");                                                \
	}

#endif

#endif
// 设置每个硬盘的起始扇区号和扇区总数。其中编号i*5 含义参见本程序后的有关说明。
for (i = 0; i < NR_HD; i++)
{
	hd[i * 5].start_sect = 0;												 // 硬盘起始扇区号。
	hd[i * 5].nr_sects = hd_info[i].head * hd_info[i].sect * hd_info[i].cyl; // 硬盘总扇区数。
}

/*
 * 我们对CMOS 有关硬盘的信息有些怀疑：可能会出现这样的情况，我们有一块SCSI/ESDI/等的
 * 控制器，它是以ST-506 方式与BIOS 兼容的，因而会出现在我们的BIOS 参数表中，但却又不
 * 是寄存器兼容的，因此这些参数在CMOS 中又不存在。
 * 另外，我们假设ST-506 驱动器（如果有的话）是系统中的基本驱动器，也即以驱动器1 或2
 * 出现的驱动器。
 * 第1 个驱动器参数存放在CMOS 字节0x12 的高半字节中，第2 个存放在低半字节中。该4 位字节
 * 信息可以是驱动器类型，也可能仅是0xf。0xf 表示使用CMOS 中0x19 字节作为驱动器1 的8 位
 * 类型字节，使用CMOS 中0x1A 字节作为驱动器2 的类型字节。
 * 总之，一个非零值意味着我们有一个AT 控制器硬盘兼容的驱动器。
 */

// 这里根据上述原理来检测硬盘到底是否是AT 控制器兼容的。有关CMOS 信息请参见4.2.3.1 节。
if ((cmos_disks = CMOS_READ(0x12)) & 0xf0)
	if (cmos_disks & 0x0f)
		NR_HD = 2;
	else
		NR_HD = 1;
else
	NR_HD = 0;
// 若NR_HD=0，则两个硬盘都不是AT 控制器兼容的，硬盘数据结构清零。
// 若NR_HD=1，则将第2 个硬盘的参数清零。
for (i = NR_HD; i < 2; i++)
{
	hd[i * 5].start_sect = 0;
	hd[i * 5].nr_sects = 0;
}
// 读取每一个硬盘上第1 块数据（第1 个扇区有用），获取其中的分区表信息。
// 首先利用函数bread()读硬盘第1 块数据(fs/buffer.c,267)，参数中的0x300 是硬盘的主设备号
// (参见列表后的说明)。然后根据硬盘头1 个扇区位置0x1fe 处的两个字节是否为'55AA'来判断
// 该扇区中位于0x1BE 开始的分区表是否有效。最后将分区表信息放入硬盘分区数据结构hd 中。
for (drive = 0; drive < NR_HD; drive++)
{
	if (!(bh = bread(0x300 + drive * 5, 0)))
	{ // 0x300, 0x305 逻辑设备号。
		printk("Unable to read partition table of drive %d\n\r", drive);
		panic("");
	}
	if (bh->b_data[510] != 0x55 || (unsigned char)bh->b_data[511] != 0xAA)
	{ // 判断硬盘信息有效标志'55AA'。
		printk("Bad partition table on drive %d\n\r", drive);
		panic("");
	}
	p = (struct partition *)(0x1BE + bh->b_data); // 分区表位于硬盘第1 扇区的0x1BE 处。
	for (i = 1; i < 5; i++, p++)
	{
		hd[i + 5 * drive].start_sect = p->start_sect;
		hd[i + 5 * drive].nr_sects = p->nr_sects;
	}
	brelse(bh); // 释放为存放硬盘块而申请的内存缓冲区页。
}
if (NR_HD) // 如果有硬盘存在并且已读入分区表，则打印分区表正常信息。
	printk("Partition table%s ok.\n\r", (NR_HD > 1) ? "s" : "");
rd_load();	  // 加载（创建）RAMDISK(kernel/blk_drv/ramdisk.c,71)。
mount_root(); // 安装根文件系统(fs/super.c,242)。
return (0);
}

//// 判断并循环等待驱动器就绪。
// 读硬盘控制器状态寄存器端口HD_STATUS(0x1f7)，并循环检测驱动器就绪比特位和控制器忙位。
static int controller_ready(void)
{
	int retries = 10000;

	while (--retries && (inb_p(HD_STATUS) & 0xc0) != 0x40)
		;
	return (retries); // 返回等待循环的次数。
}

//// 检测硬盘执行命令后的状态。(win_表示温切斯特硬盘的缩写)
// 读取状态寄存器中的命令执行结果状态。返回0 表示正常，1 出错。如果执行命令错，
// 则再读错误寄存器HD_ERROR(0x1f1)。
static int win_result(void)
{
	int i = inb_p(HD_STATUS); // 取状态信息。

	if ((i & (BUSY_STAT | READY_STAT | WRERR_STAT | SEEK_STAT | ERR_STAT)) == (READY_STAT | SEEK_STAT))
		return (0); /* ok */
	if (i & 1)
		i = inb(HD_ERROR); // 若ERR_STAT 置位，则读取错误寄存器。
	return (1);
}

//// 向硬盘控制器发送命令块（参见列表后的说明）。
// 调用参数：
// drive - 硬盘号(0-1)；
// nsect - 读写扇区数；
// sect - 起始扇区；
// head - 磁头号；
// cyl - 柱面号；
// cmd - 命令码；
// *intr_addr() - 硬盘中断处理程序中将调用的C处理函数。
static void hd_out(unsigned int drive, unsigned int nsect, unsigned int sect,
				   unsigned int head, unsigned int cyl, unsigned int cmd,
				   void (*intr_addr)(void))
{
	register int port; // asm ("dx");	// port 变量对应寄存器dx。

	if (drive > 1 || head > 15) // 如果驱动器号(0,1)>1 或磁头号>15，则程序不支持。
		panic("Trying to write bad sector");
	if (!controller_ready()) // 如果等待一段时间后仍未就绪则出错，死机。
		panic("HD controller not ready");
	do_hd = intr_addr;							// do_hd 函数指针将在硬盘中断程序中被调用。
	outb_p(hd_info[drive].ctl, HD_CMD);			// 向控制寄存器(0x3f6)输出控制字节。
	port = HD_DATA;								// 置dx 为数据寄存器端口(0x1f0)。
	outb_p(hd_info[drive].wpcom >> 2, ++port);	// 参数：写预补偿柱面号(需除4)。
	outb_p(nsect, ++port);						// 参数：读/写扇区总数。
	outb_p(sect, ++port);						// 参数：起始扇区。
	outb_p(cyl, ++port);						// 参数：柱面号低8 位。
	outb_p(cyl >> 8, ++port);					// 参数：柱面号高8 位。
	outb_p(0xA0 | (drive << 4) | head, ++port); // 参数：驱动器号+磁头号。
	outb(cmd, ++port);							// 命令：硬盘控制命令。
}

//// 等待硬盘就绪。也即循环等待主状态控制器忙标志位复位。若仅有就绪或寻道结束标志
// 置位，则成功，返回0。若经过一段时间仍为忙，则返回1。
static int drive_busy(void)
{
	unsigned int i;

	for (i = 0; i < 10000; i++) // 循环等待就绪标志位置位。
		if (READY_STAT == (inb_p(HD_STATUS) & (BUSY_STAT | READY_STAT)))
			break;
	i = inb(HD_STATUS);						 // 再取主控制器状态字节。
	i &= BUSY_STAT | READY_STAT | SEEK_STAT; // 检测忙位、就绪位和寻道结束位。
	if (i == READY_STAT | SEEK_STAT)		 // 若仅有就绪或寻道结束标志，则返回0。
		return (0);
	printk("HD controller times out\n\r"); // 否则等待超时，显示信息。并返回1。
	return (1);
}

//// 诊断复位（重新校正）硬盘控制器。
static void reset_controller(void)
{
	int i;

	outb(4, HD_CMD); // 向控制寄存器端口发送控制字节(4-复位)。
	for (i = 0; i < 100; i++)
		nop();							 // 等待一段时间（循环空操作）。
	outb(hd_info[0].ctl & 0x0f, HD_CMD); // 再发送正常的控制字节(不禁止重试、重读)。
	if (drive_busy())					 // 若等待硬盘就绪超时，则显示出错信息。
		printk("HD-controller still busy\n\r");
	if ((i = inb(HD_ERROR)) != 1) // 取错误寄存器，若不等于1（无错误）则出错。
		printk("HD-controller reset failed: %02x\n\r", i);
}

//// 复位硬盘nr。首先复位（重新校正）硬盘控制器。然后发送硬盘控制器命令“建立驱动器参数”，
// 其中recal_intr()是在硬盘中断处理程序中调用的重新校正处理函数。
static void reset_hd(int nr)
{
	reset_controller();
	hd_out(nr, hd_info[nr].sect, hd_info[nr].sect, hd_info[nr].head - 1,
		   hd_info[nr].cyl, WIN_SPECIFY, &recal_intr);
}

//// 意外硬盘中断调用函数。
// 发生意外硬盘中断时，硬盘中断处理程序中调用的默认C 处理函数。在被调用函数指针为空时
// 调用该函数。参见(kernel/system_call.s,241 行)。
void unexpected_hd_interrupt(void)
{
	printk("Unexpected HD interrupt\n\r");
}

//// 读写硬盘失败处理调用函数。
static void bad_rw_intr(void)
{
	if (++CURRENT->errors >= MAX_ERRORS)  // 如果读扇区时的出错次数大于或等于7 次时，
		end_request(0);					  // 则结束请求并唤醒等待该请求的进程，而且
										  // 对应缓冲区更新标志复位（没有更新）。
	if (CURRENT->errors > MAX_ERRORS / 2) // 如果读一扇区时的出错次数已经大于3 次，
		reset = 1;						  // 则要求执行复位硬盘控制器操作。
}

//// 读操作中断调用函数。将在执行硬盘中断处理程序中被调用。
static void read_intr(void)
{
	if (win_result())
	{					 // 若控制器忙、读写错或命令执行错，
		bad_rw_intr();	 // 则进行读写硬盘失败处理
		do_hd_request(); // 然后再次请求硬盘作相应(复位)处理。
		return;
	}
	port_read(HD_DATA, CURRENT->buffer, 256); // 将数据从数据寄存器口读到请求结构缓冲区。
	CURRENT->errors = 0;					  // 清出错次数。
	CURRENT->buffer += 512;					  // 调整缓冲区指针，指向新的空区。
	CURRENT->sector++;						  // 起始扇区号加1，
	if (--CURRENT->nr_sectors)
	{						// 如果所需读出的扇区数还没有读完，则
		do_hd = &read_intr; // 再次置硬盘调用C 函数指针为read_intr()
		return;				// 因为硬盘中断处理程序每次调用do_hd 时
	}						// 都会将该函数指针置空。参见system_call.s
	end_request(1);			// 若全部扇区数据已经读完，则处理请求结束事宜，
	do_hd_request();		// 执行其它硬盘请求操作。
}

#define port_write(port,buf,nr) \
__asm__( "cld;rep;outsw":: "d" (port), "S" (buf), "c" (nr): "cx", "si")
_inline void port_write(unsigned short port, void* buf,unsigned short nr)
{_asm{
	mov dx,port
	mov esi,buf
	mov cx,nr
	cld
	rep outsw // rep指令是重复执行该指令后面的汇编代码，执行次数由寄存器ecx控制
	// outsw 字符串输出指令(Output String Instruction)
}}
//// 写扇区中断调用函数。在硬盘中断处理程序中被调用。
// 在写命令执行后，会产生硬盘中断信号，执行硬盘中断处理程序，此时在硬盘中断处理程序中调用的
// C 函数指针do_hd()已经指向write_intr()，因此会在写操作完成（或出错）后，执行该函数。
static void write_intr(void)
{
	if (win_result())
	{					 // 如果硬盘控制器返回错误信息，
		bad_rw_intr();	 // 则首先进行硬盘读写失败处理，
		do_hd_request(); // 然后再次请求硬盘作相应(复位)处理，
		return;			 // 然后返回（也退出了此次硬盘中断）。
	}
	if (--CURRENT->nr_sectors)
	{											   // 否则将欲写扇区数减1，若还有扇区要写，则
		CURRENT->sector++;						   // 当前请求起始扇区号+1，
		CURRENT->buffer += 512;					   // 调整请求缓冲区指针，
		do_hd = &write_intr;					   // 置硬盘中断程序调用函数指针为write_intr()，
		port_write(HD_DATA, CURRENT->buffer, 256); // 再向数据寄存器端口写256 字节。
		return;									   // 返回等待硬盘再次完成写操作后的中断处理。
	}
	end_request(1);	 // 若全部扇区数据已经写完，则处理请求结束事宜，
	do_hd_request(); // 执行其它硬盘请求操作。
}

//// 硬盘重新校正（复位）中断调用函数。在硬盘中断处理程序中被调用。
// 如果硬盘控制器返回错误信息，则首先进行硬盘读写失败处理，然后请求硬盘作相应(复位)处理。
static void recal_intr(void)
{
	if (win_result())
		bad_rw_intr();
	do_hd_request();
}

// 执行硬盘读写请求操作。
void do_hd_request(void)
{
	int i, r;
	unsigned int block, dev;
	unsigned int sec, head, cyl;
	unsigned int nsect;
	// 这里有一个 repeat标号
	INIT_REQUEST;			   // 检测请求项的合法性(参见kernel/blk_drv/blk.h,127)。
							   // 取设备号中的子设备号(见列表后对硬盘设备号的说明)。子设备号即是硬盘上的分区号。
	dev = MINOR(CURRENT->dev); // CURRENT 定义为(blk_dev[MAJOR_NR].current_request)。
	block = CURRENT->sector;   // 请求的起始扇区。
							   // 如果子设备号不存在或者起始扇区大于该分区扇区数-2，则结束该请求，并跳转到标号repeat 处
							   // （定义在INIT_REQUEST 开始处）。因为一次要求读写2 个扇区（512*2 字节），所以请求的扇区号
							   // 不能大于分区中最后倒数第二个扇区号。
	if (dev >= 5 * NR_HD || block + 2 > hd[dev].nr_sects)
	{
		end_request(0); // 移动到下一个请求
		goto repeat;	// 该标号在blk.h 最后面。
	}
	block += hd[dev].start_sect; // 将所需读的块对应到整个硬盘上的绝对扇区号。
	dev /= 5;					 // 此时dev 代表硬盘号（0 或1）。
								 // 下面嵌入汇编代码用来从硬盘信息结构中根据起始扇区号和每磁道扇区数计算在磁道中的
								 // 扇区号(sec)、所在柱面号(cyl)和磁头号(head)。
	sec = hd_info[dev].sect;
	_asm {
		mov eax,block
		xor edx,edx
		mov ebx,sec
		div ebx
		mov block,eax
		mov sec,edx
	}
	//__asm__ ("divl %4": "=a" (block), "=d" (sec):"" (block), "1" (0),
	//	   "r" (hd_info[dev].
	//		sect));
	head = hd_info[dev].head;
	_asm {
		mov eax,block
		xor edx,edx
		mov ebx,head
		div ebx
		mov cyl,eax
		mov head,edx
	}
	//__asm__ ("divl %4": "=a" (cyl), "=d" (head):"" (block), "1" (0),
	//	   "r" (hd_info[dev].
	//		head));
	sec++;
	nsect = CURRENT->nr_sectors; // 欲读/写的扇区数。
								 // 如果reset 置1，则执行复位操作。复位硬盘和控制器，并置需要重新校正标志，返回。
	if (reset)
	{
		reset = 0;
		recalibrate = 1;
		reset_hd(CURRENT_DEV);
		return;
	}
	// 如果重新校正标志(recalibrate)置位，则首先复位该标志，然后向硬盘控制器发送重新校正命令。
	if (recalibrate)
	{
		recalibrate = 0;
		hd_out(dev, hd_info[CURRENT_DEV].sect, 0, 0, 0,
			   WIN_RESTORE, &recal_intr); // WIN_RESTORE 驱动器重新校正
		return;
	}
	// 如果当前请求是写扇区操作，则发送写命令，循环读取状态寄存器信息并判断请求服务标志
	// DRQ_STAT 是否置位。DRQ_STAT 是硬盘状态寄存器的请求服务位（include/linux/hdreg.h，27）。
	if (CURRENT->cmd == WRITE)
	{
		hd_out(dev, nsect, sec, head, cyl, WIN_WRITE, &write_intr); // 写入磁盘 WIN_WRITE
		// 重试最多3000次?
		for (i = 0; i < 3000 && !(r = inb_p(HD_STATUS) & DRQ_STAT); i++)
			// 如果请求服务位 置位则退出循环。若等到循环结束也没有置位，则此次写硬盘操作失败，去处理
			// 下一个硬盘请求。否则向硬盘控制器数据寄存器端口HD_DATA 写入1 个扇区的数据。
			if (!r)
			{
				bad_rw_intr();
				goto repeat; // 该标号在blk.h 最后面，也即跳到301 行。
			}
		port_write(HD_DATA, CURRENT->buffer, 256); // 写入 256 * 2B = 512B 一个扇区
		// 如果当前请求是读硬盘扇区，则向硬盘控制器发送读扇区命令。
	}
	else if (CURRENT->cmd == READ)
	{
		hd_out(dev, nsect, sec, head, cyl, WIN_READ, &read_intr); // 读取磁盘 WIN_READ
	}
	else
		panic("unknown hd-command");
}

// 硬盘系统初始化。
void hd_init(void)
{
	blk_dev[MAJOR_NR].request_fn = DEVICE_REQUEST; // do_hd_request()
	set_intr_gate(0x2E, &hd_interrupt);			   // 设置硬盘中断门向量 int 0x2E(46)。
												   // hd_interrupt 在(kernel/system_call.s,221)。
	// 允许 硬盘向 8259A(可编程中断控制器) 发信中断请求, 再由 中断控制器向 cpu发送中断请求
	// 0x 1111 1011 将对应位置0
	outb_p(inb_p(0x21) & 0xfb, 0x21); // 复位接联的主8259A int2 的屏蔽位，允许从片
									  // 发出中断请求信号。
	// 0x 1011 1111
	outb(inb_p(0xA1) & 0xbf, 0xA1); // 复位硬盘的中断请求屏蔽位（在从片上），允许
	// 硬盘控制器发送中断请求信号。
}
