
#include "dr_api.h"
#include "drmgr.h"
#include "drsyscall.h"
#include "sysent.h"
#include "errnoent.h"
#include <stdio.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
static const char* err_name(int err){
    return errnoent[err];
}


#include <string.h>
#include <linux/limits.h>
extern void sprintf_xlat_flag();
#define OUTBUF_SIZE 2048

static bool event_filter_syscall(void *drcontext, int sysnum)
{
    return true; 
}

static char* simple_value(long unsigned int val,bool null,bool hex){
    char * ret = malloc(sizeof(int)*25);
    if (val > 255 && hex)
        sprintf(ret,"0x%lx",val);
    else if (val == 0 && null) 
        sprintf(ret,"%s",null?"NULL":"0");
    else sprintf(ret,"%ld",val);

    return ret; 
}
    static  bool is_print(char c)
    {
        return (c >= ' ') && (c < 0x7f);
    }
static void print_string(const char* str, int size,char* buf, bool only_printable){
   
    for (int i = 0; i< size; i ++){
        char c = str[i];
        switch (c){
            case '\"':
            case '\\':
            sprintf(buf,"%s\\%d",buf,c);
            break;
            case '\f':
            sprintf(buf,"%s\\f",buf);
            break;
            case '\n':
            sprintf(buf,"%s\\n",buf);
            break;
            case '\r':
            sprintf(buf,"%s\\r",buf);
            break;
            case '\t':
            sprintf(buf,"%s\\t",buf);
            break;
            case '\v':
            sprintf(buf,"%s\\v",buf);
            break;
            default:{
                if (is_print(c)){
                    sprintf(buf,"%s%c",buf,c);
                } 
                else 
                     if (c == 0){
                        sprintf(buf,"%s\\%d",buf,c);
                        continue;
                    }  
                    else {
                        sprintf(buf,"%s\\",buf);
                        if (only_printable) goto end;
                        if (i + 1 < size &&
                            str[i+1]>='0' && str[i+1]<='7'){
                            // Print \ooo
                            sprintf(buf,"%s\\0%d%d",buf,(c>>6),((c>>3)&0x7));
                        }
                        else {
                            // Print \[[o]o]o]
                            
                            if ((c >> 3) != 0){
                                if ((c >> 6) != 0){
                                    if ((c >> 6) < 0)
                                        sprintf(buf,"%s%d",buf,-(c>>6));
                                    else
                                        sprintf(buf,"%s%d",buf,(c>>6));
                                }
                                sprintf(buf,"%s%d",buf,(c>>3)&0x7);
                            }
                        }

                    sprintf(buf,"%s%d",buf,(c&0x7));
                }
            
        }

    }
        // dr_fprintf(STDERR,"%s\n",buf);
    }
    end:
    sprintf(buf,"%s\"...",buf);
}
void PrintBuffer(void* pBuff, unsigned int nLen)
{
    if (NULL == pBuff || 0 == nLen)
    {
        return;
    }

    const int nBytePerLine = 16;
    unsigned char* p = (unsigned char*)pBuff;
    char szHex[3*nBytePerLine+1] ;
  memset(szHex, 0, sizeof(szHex));
    dr_fprintf(STDERR,"[%p]:\n-----------------begin-------------------\n",pBuff);
    for (unsigned int i=0; i<nLen; ++i)
    {
        int idx = 3 * (i % nBytePerLine);
      
        dr_fprintf(STDERR,"%02x ",p[i]);

        // 以16个字节为一行，进行打印
        if (0 == ((i+1) % nBytePerLine))
        {
            dr_fprintf(STDERR,"\n");
        }
    }

    dr_fprintf(STDERR,"------------------end-------------------\n");
}

static void print_structure(drsys_arg_t *arg, char* buf){
  //  dr_fprintf(STDERR,"\n[%d], [%d]\n",arg->sysnum.number, arg->sysnum.number == SYSCALL_fstat);
    if (arg->sysnum.number == SYSCALL_fstat){
   //     dr_fprintf(STDERR,"fstat!\n");
        struct stat* st = (void*)arg->value64;
        sprintf_xlat_flag(arg->sysnum,st->st_mode,buf,arg->ordinal);
        if (st->st_size == 0){
            sprintf(buf,"%s, st_rdev=makedev(%d, %d), ...}",buf,major(st->st_rdev),minor(st->st_rdev));
        } else {
            sprintf(buf,"%s, st_size=%ld, ...}",buf,st->st_size);
     }

    } else if (arg->sysnum.number == SYSCALL_read){
        //PrintBuffer((void*)arg->value64,144);
        print_string((const char*)arg->value64,32,buf,false);
    } else if (arg->sysnum.number == SYSCALL_write){
        print_string((const char*)arg->value64,16,buf,true);
    }
}
static bool drsys_iter_arg_cb(drsys_arg_t *arg, void *user_data)
{
    char* buf = (char*)user_data;

    if (arg->ordinal == -1)
        return false;
    switch (arg->type){

        case DRSYS_TYPE_CSTRING:
            if (arg->ordinal == 0) sprintf(buf,"\"%s\"",(char*)arg->value); else sprintf(buf,"%s, \"%s\"",buf,(char*)arg->value);
            break;
        case DRSYS_TYPE_UNKNOWN:{
            switch (arg->sysnum.number){
                case SYSCALL_lseek:{
                    if(arg->ordinal == 0){
                        sprintf(buf,"%s",simple_value(arg->value64,true,true));
                    }
                    if(arg->ordinal == 1){
                        sprintf(buf,"%s, %d",buf,(short)arg->value);
                    } else if (arg->ordinal == 2){
                        sprintf_xlat_flag(arg->sysnum,arg->value64,buf,arg->ordinal);
                    }
                    break;
                }
                case SYSCALL_access:
                case SYSCALL_openat:
                    //dr_fprintf(STDERR,"%s",arg->enum_name);
                    //sprintf(buf,"%s,%s",arg->enum_name);
                    sprintf_xlat_flag(arg->sysnum,arg->value64,buf,arg->ordinal);
                    break;
                case SYSCALL_brk:
                    sprintf(buf,"%s",simple_value(arg->value64,true,true));
                    break;
                case SYSCALL_fstat:
                case SYSCALL_close:
                    if (arg->ordinal == 0){
                        sprintf(buf,"%s",simple_value(arg->value64,false,true));
                    }
                    break;
                case SYSCALL_read:
                case SYSCALL_write:{
                    switch (arg->ordinal){
                        case 0:
                            sprintf(buf,"%s, \"",simple_value(arg->value64,true,true));
                            break;
                        case 2:
                            sprintf(buf,"%s, %s",buf,simple_value(arg->value64,true,false));
                            break;

                    }
                    break;
                }

                case SYSCALL_mmap:
                case SYSCALL_munmap:
                case SYSCALL_mprotect:
                    switch (arg->ordinal){
                        case 0:
                            sprintf(buf,"%s",simple_value(arg->value64,true,true));

                            break;
                        case 1:
                            sprintf(buf,"%s, %s",buf,simple_value(arg->value64,true,false));
                            break;
                        case 4:
                            sprintf(buf,"%s, %d",buf,(short)arg->value);
                        break;
                        case 5:
                            sprintf(buf,"%s, %s",buf,simple_value(arg->value64,false,true));
                        break;
                        case 2:
                        case 3:
                            sprintf_xlat_flag(arg->sysnum,arg->value64,buf,arg->ordinal);
                        break;
                    }
                    break;
                case SYSCALL_arch_prctl:
                    
                    if (arg->ordinal == 1){
                        sprintf(buf,"%s, %s",buf,simple_value(arg->value64,true,true));
                    }
                    break;
                
                default:
                   //sprintf(buf,"%s,%d",buf,arg->sysnum,simple_value(arg->value64,false,false));
                    break;

            }
            break;
       
        }
        case DRSYS_TYPE_SIGNED_INT:{
            int sysnum = arg->sysnum.number;
            if (sysnum == SYSCALL_arch_prctl)
                sprintf_xlat_flag(arg->sysnum,arg->value64,buf,arg->ordinal);
            break;
        }
        case DRSYS_TYPE_STRUCT:{
            print_structure(arg,buf);
            break;
        }
            
            break;
    }
    return true;

}

static bool event_pre_syscall(void *drcontext, int sysnum)
{
    if (sysnum == SYSCALL_ioctl)
      
        return false;
    if (sysnum == SYSCALL_exit_group){
        dr_fprintf(STDERR,"%-40s= %s\n","exit_group(0)","?");

    }
    return true; /* execute normally */
}





static void event_post_syscall(void *drcontext, int sysnum)
{

    drsys_syscall_t *syscall;
    const char *name = "<unknown>";
    if (drsys_cur_syscall(drcontext, &syscall) == DRMF_SUCCESS){
        drsys_syscall_name(syscall, &name);
    }
    char buf[OUTBUF_SIZE/2];
    memset(buf, 0, sizeof(buf));
    drsys_iterate_args(drcontext, drsys_iter_arg_cb, buf);
    char final[OUTBUF_SIZE];

    sprintf(final,"%s(%s)",name,buf);
    dr_fprintf(STDERR, "%-39s",final);
    // syscall name, parameters


    // return value 
    bool success;
    uint64 result;
    int errno;
    char* return_prefix = 0;
    char* error_msg = "";
    // if (sysnum == SYSCALL_fstat){
    //     char* buf="";
    //     buf = malloc(sizeof(char)*OUTBUF_SIZE);
 
    //     drsys_itttate_args(drcontext, drsys_iter_arg_cb, buf);
    // }gtt
    
    if (drsys_cur_syscall_result(drcontext, &success, &result, &errno) == DRMF_SUCCESS){
        if(result > 1024)
            return_prefix = "0x";
        
        
        if (errno != 0){
            // https://github.com/strace/strace/blob/92e347b556bb183ebaaf6fedb839da222c8c67b4/linux/x86_64/get_error.c
            // According to strace implementation, the return value should be -1
            dr_fprintf(STDERR,"= %d %s (%s)",-1,err_name(errno),strerror(errno));
        } 
        else if (return_prefix){
           dr_fprintf(STDERR," = %s%lx ",return_prefix,result);     
        }
        else
        {
           dr_fprintf(STDERR," = %d",result);
        }       
        dr_fprintf(STDERR,"\n");
        
    }
    fflush(stderr);
    

}

static void event_exit(void)
{
 
 //dr_fprintf(STDERR,"exiting...");
    if (drsys_exit() != DRMF_SUCCESS)
        DR_ASSERT(false);
    drmgr_exit();
}



DR_EXPORT void dr_client_main(client_id_t id, int argc, const char *argv[])
{
   // fclose(stdout);
  //  freopen("/dev/null", "w", stdout);
    drsys_options_t ops = {sizeof(ops),0};

    drmgr_init();

    dr_register_filter_syscall_event(event_filter_syscall);
    drmgr_register_pre_syscall_event(event_pre_syscall);
    drmgr_register_post_syscall_event(event_post_syscall);
    if (drsys_init(id, &ops) != DRMF_SUCCESS)
        DR_ASSERT(false);
    dr_register_exit_event(event_exit);

}
