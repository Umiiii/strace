char* access_flags[] = {"F_OK","X_OK","W_OK","??_OK","R_OK"};
char* open_access_modes[] = {"O_RDONLY","O_WRONLY","O_RDWR"};

struct xlat_data
{
	const char *str;
	int val;
};

struct xlat_data open_access_mode[] = 
{
	{"O_CREAT",0100},
	{"O_EXCL",0200},
	{"O_NOCTTY",0400},
	{"O_TRUNC",01000},
	{"O_APPEND",02000},
	{"O_NONBLOCK",04000},
	{"O_SYNC",010000},
	{"O_ASYNC",020000},
	{"O_DIRECT",040000},
	{"O_DIRECTORY",0200000},
	{"O_NOFOLLOW",0400000},
	{"O_NOATIME",01000000},
	{"O_CLOEXEC",02000000}
};

struct xlat_data mmap_protection[] = 
{
	{"PROT_NONE",0},
	{"PROT_READ",1},
	{"PROT_WRITE",2},
	{"PROT_EXEC",4}
};

struct xlat_data mmap_flags[] = 
{
	{"MAP_FILE",0},
	{"MAP_SHARED",1},
	{"MAP_PRIVATE",2},
	{"MAP_FIXED",0x10},
	{"MAP_ANONYMOUS",0x20},
	{"MAP_DENYWRITE",0x800},
	{"MAP_TYPE",0xf},
	{"MAP_FAILED",-1},

};

struct xlat_data arch_vals[] = 
{
	{"ARCH_SET_GS",	0x1001},
	{"ARCH_SET_FS",	0x1002},
	{"ARCH_GET_FS",	0x1003},
	{"ARCH_GET_GS",	0x1004},
	{"ARCH_GET_CPUID",	0x1011},
	{"ARCH_SET_CPUID",	0x1012},
	{"ARCH_MAP_VDSO_X32",	0x2001},
	{"ARCH_MAP_VDSO_32",	0x2002},
	{"ARCH_MAP_VDSO_64",	0x2003}

};

struct xlat_data struct_stat_vals[] = {
	{"S_IFSOCK", 0140000},
	{"S_IFLNK",  0120000},
	{"S_IFREG",  0100000},
	{"S_IFBLK",  0060000},
	{"S_IFDIR",  0040000},
	{"S_IFCHR",  0020000},
	{"S_IFIFO",  0010000},
	{"S_ISUID",  0004000},
	{"S_ISGID",  0002000},
	{"S_ISVTX",  0001000}
};

struct xlat_data lseek_flags[] = {
	{"SEEK_CUR",1},
	{"SEEK_SET",0},
	{"SEEK_END",2}
};