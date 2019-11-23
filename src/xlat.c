
#include "flags.h"
#include "sysent.h"
#include <stdio.h>

void sprintf_xlat_flag(int sysnum, int flag, char* buf, int ordinal){
	int find = 0;
	switch (sysnum){
		case SYSCALL_access:
			sprintf(buf,"%s,%s",buf,access_flags[flag]);
		break;
		case SYSCALL_openat:
			if (ordinal == 0){
				// fd
				if (flag == -100)
					sprintf(buf,"AT_FDCWD");
				else 
					sprintf(buf,"UNKNOWN_FD");
			} else if (ordinal !=3){
				sprintf(buf,"%s, %s",buf,open_access_modes[flag & 3]);
				// O_ACCMODE == 3
				for (int i = 0; i < sizeof(open_access_mode)/sizeof(struct xlat_data);i++){
					if (flag & open_access_mode[i].val){
						sprintf(buf,"%s|%s",buf,open_access_mode[i].str);
					}
	
				}
			}
			break;
		case SYSCALL_mmap:
		case SYSCALL_mprotect:{
		
			if (ordinal == 2){
				
				for (int i = 0; i< sizeof(mmap_protection)/sizeof(struct xlat_data);i++){
					if (flag & mmap_protection[i].val){
						if (find)
							sprintf(buf,"%s|%s",buf,mmap_protection[i].str);
						else {
							find = 1;
							sprintf(buf,"%s, %s",buf,mmap_protection[i].str);
						}
						flag &= ~mmap_protection[i].val;
					}
				}
			} else if (ordinal == 3) {
				for (int i = 0; i< sizeof(mmap_flags)/sizeof(struct xlat_data);i++){
					if (flag & mmap_flags[i].val){
						if (find)
						sprintf(buf,"%s|%s",buf,mmap_flags[i].str);
						else{
							find = 1;
							sprintf(buf,"%s, %s",buf,mmap_flags[i].str);
						}
						flag &= ~mmap_flags[i].val;
					}
				}
			}
			break;
		}
		case SYSCALL_arch_prctl:{
			find = 0;
			for (int i = 0; i< sizeof(arch_vals)/sizeof(struct xlat_data);i++){
					if (flag == arch_vals[i].val){
						sprintf(buf,"%s",arch_vals[i].str);
						flag &= ~arch_vals[i].val;
					}
				}
			break;
		}
		case SYSCALL_fstat:{
			for (int i = 0; i< sizeof(struct_stat_vals)/sizeof(struct xlat_data);i++){
				int result = flag & 00170000;
					if (result == struct_stat_vals[i].val){
						sprintf(buf,"%s, {st_mode=%s",buf, struct_stat_vals[i].str);
						flag &= ~result;
						
					}
				}
			sprintf(buf,"%s|%04o",buf,flag);
			break;
		}
		case SYSCALL_lseek:{
			for (int i = 0; i< sizeof(lseek_flags)/sizeof(struct xlat_data);i++){
					if (flag == lseek_flags[i].val){
						sprintf(buf,"%s, %s",buf,lseek_flags[i].str);
						break;
					}
				}
		}
		
	}

	
}
