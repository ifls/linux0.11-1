/*
* NOTE!!! memcpy(dest,src,n) assumes ds=es=normal data segment. This
* goes for all kernel functions (ds=es=kernel space, fs=local data,
* gs=null), as well as for all well-behaving user programs (ds=es=
* user data space). This is NOT a bug, as any user program that changes
* es deserves to die if it isn't careful.
*/
/*
* 注意!!!memcpy(dest,src,n)假设段寄存器ds=es=通常数据段。在内核中使用的
* 所有函数都基于该假设（ds=es=内核空间，fs=局部数据空间，gs=null）,具有良好
* 行为的应用程序也是这样（ds=es=用户数据空间）。如果任何用户程序随意改动了
* es 寄存器而出错，则并不是由于系统程序错误造成的。
*/
//// 内存块复制。从源地址src 处开始复制n 个字节到目的地址dest 处。
// 参数：dest - 复制的目的地址，src - 复制的源地址，n - 复制字节数。
// %0 - edi(目的地址dest)，%1 - esi(源地址src)，%2 - ecx(字节数n)，
/*extern _inline char* memcpy(char * dest, char * src, int n)
{
	char * __res;
	_asm{
		mov edi,dest
		mov esi,src
		mov ecx,n	// 共复制ecx(n)字节。
		cld
		rep movsb // 从ds:[esi]复制到es:[edi]，并且esi++，edi++。
		mov __res,edi
	}
	return __res;
}
#define memcpy(dest,src,n) ({ \
void * _res = dest; \
__asm__ ( "cld;rep;movsb" \
:: "D" ((long)(_res)), "S" ((long)(src)), "c" ((long) (n)) \
: "di", "si", "cx"); \
_res; \
})*/

get_fs_long (const unsigned long *addr)
{
//  unsigned long _v;

//__asm__ ("movl %%fs:%1,%0": "=r" (_v):"m" (*addr));
  _asm mov ebx,addr
  _asm mov eax,dword ptr fs:[ebx];
//  _asm mov _v,eax;
//  return _v;
}

//// ��һ�ֽڴ����fs ����ָ���ڴ��ַ����
// ������val - �ֽ�ֵ��addr - �ڴ��ַ��
// %0 - �Ĵ���(�ֽ�ֵval)��%1 - (�ڴ��ַaddr)��
extern _inline void
put_fs_byte (char val, char *addr)//passed
{
//  __asm__ ("movb %0,%%fs:%1"::"r" (val), "m" (*addr));
	_asm mov ebx,addr
	_asm mov al,val;
	_asm mov byte ptr fs:[ebx],al;
}

//// ��һ�ִ����fs ����ָ���ڴ��ַ����
// ������val - ��ֵ��addr - �ڴ��ַ��
// %0 - �Ĵ���(��ֵval)��%1 - (�ڴ��ַaddr)��
extern _inline void
put_fs_word (short val, short *addr)
{
//  __asm__ ("movw %0,%%fs:%1"::"r" (val), "m" (*addr));
	_asm mov ebx,addr
	_asm mov ax,val;
	_asm mov word ptr fs:[ebx],ax;
}

//// ��һ���ִ����fs ����ָ���ڴ��ַ����
// ������val - ����ֵ��addr - �ڴ��ַ��
// %0 - �Ĵ���(����ֵval)��%1 - (�ڴ��ַaddr)��
extern _inline void
put_fs_long (long val, unsigned long *addr)
{
//  __asm__ ("movl %0,%%fs:%1"::"r" (val), "m" (*addr));
	_asm mov ebx,addr
	_asm mov eax,val;
	_asm mov dword ptr fs:[ebx],eax;
}

/*
* ���Ҹ���GNU ������Ӧ����ϸ�������Ĵ��롣��Щ������ʹ�ã����Ҳ�֪���Ƿ�
* ����һЩС����
* --- TYT��1991 ��11 ��24 ��
* [ ��Щ����û�д���Linus ]
*/

//// ȡfs �μĴ���ֵ(ѡ���)��
// ���أ�fs �μĴ���ֵ��
extern _inline unsigned short
get_fs ()
{
//  unsigned short _v;
//__asm__ ("mov %%fs,%%ax": "=a" (_v):);
  _asm mov ax,fs;
//  _asm mov _v,ax;
//  return _v;
}

//// ȡds �μĴ���ֵ��
// ���أ�ds �μĴ���ֵ��
extern _inline unsigned short
get_ds ()
{
//  unsigned short _v;
//__asm__ ("mov %%ds,%%ax": "=a" (_v):);
  _asm mov ax,fs;
//  _asm mov _v,ax;
//  return _v;
}

//// ����fs �μĴ�����
// ������val - ��ֵ��ѡ�������
extern _inline void
set_fs (unsigned long val)
{
//  __asm__ ("mov %0,%%fs"::"a" ((unsigned short) val));
	_asm mov eax,val;
	_asm mov fs,ax;
}
