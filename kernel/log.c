#include "linux/sched.h"
#include "sys/stat.h"
#include <stdarg.h>
#include <stddef.h>

#include <linux/kernel.h>

static char logbuf[1024];
int fprintk(int fd, const char *fmt, ...)
{
    va_list args;
    int count;
    struct file * file;
    struct m_inode * inode;

    va_start(args, fmt);
    count=vsprintf(logbuf, fmt, args);
    va_end(args);

    if (fd < 3)    /* 如果输出到stdout或stderr，直接调用sys_write即可 */
    {
        __asm__("push %%fs\n\t"
            "push %%ds\n\t"
            "pop %%fs\n\t"
            "pushl %0\n\t"
            "pushl $logbuf\n\t" /* 注意对于Windows环境来说，是_logbuf,下同 */
            "pushl %1\n\t"
            "call sys_write\n\t" /* 注意对于Windows环境来说，是_sys_write,下同 */
            "addl $8,%%esp\n\t"
            "popl %0\n\t"
            "pop %%fs"
            ::"r" (count),"r" (fd):"ax","cx","dx");
    }
    else    /* 假定>=3的描述符都与文件关联。事实上，还存在很多其它情况，这里并没有考虑。*/
    {
        if (!(file=task[1]->filp[fd]))    /* 从进程0的文件描述符表中得到文件句柄 */
            return 0;
        inode=file->f_inode;

        __asm__("push %%fs\n\t"
            "push %%ds\n\t"
            "pop %%fs\n\t"
            "pushl %0\n\t"
            "pushl $logbuf\n\t"
            "pushl %1\n\t"
            "pushl %2\n\t"
            "call file_write\n\t"
            "addl $12,%%esp\n\t"
            "popl %0\n\t"
            "pop %%fs"
            ::"r" (count),"r" (file),"r" (inode):"ax","cx","dx");
    }
    return count;
}
/*#include <fcntl.h>*/
/*#include <linux/fs.h>*/
/*#include <linux/sched.h>*/

/*#define  log_fname "/var/kernel.log"*/
/*struct m_inode * krn_log_inode = NULL;*/
/*struct file krn_log_file;*/
/*char logbuf[80];*/
/*void logK_init(){*/
    /*const int err_code = open_namei(log_fname, O_CREAT|O_TRUNC|O_WRONLY, 0666, &krn_log_inode);*/
    
    /*krn_log_file.f_mode = krn_log_inode->i_mode;*/
	/*krn_log_file.f_flags = 0666;*/
	/*krn_log_file.f_count = 1;*/
	/*krn_log_file.f_inode = krn_log_inode;*/
	/*krn_log_file.f_pos = 0;*/
/*}*/

/*char * itoa(int num, char * p) {*/
    /*int mask = 1;*/
    /*while(mask*10 <= num) {*/
        /*mask *= 10;*/
    /*}*/
    /*const char i2c[]="0123456789";*/
    /*while(mask >= 1) {*/
        /**p++ = i2c[num / mask];*/
        /*mask /= 10;*/
    /*}*/
    /*return p;*/
/*}*/

/*int logK_info(const char * info, int count) {*/
    /*char * dst = logbuf;*/
    /*const char *src = "INFO";*/
    /*while(*src != '\0') {*/
        /**dst++ = *src++;*/
    /*}*/

    /**dst++ = ' ';*/

    /*const int pid = current->pid;*/
    /*dst = itoa(pid, dst);*/

    /*int i = 0;*/
    /*for(i = 0; i < count; ++i) {*/
        /**dst++ = *info++;*/
    /*}*/
    /**dst++ = '\n';*/
    /**dst++ = '\0';*/

    /*if(krn_log_inode == NULL) {*/
        /*logK_init();*/
    /*}*/
    /*[>printk("logK_info: %s\n", logbuf);<]*/
    /*[>file_write(krn_log_inode, &krn_log_file, buf, dst-buf);<]*/
    /*__asm__("push %%fs\n\t"*/
        /*"push %%ds\n\t"*/
        /*"pop %%fs\n\t"*/
        /*"pushl %0\n\t"*/
        /*"pushl $logbuf\n\t"*/
        /*"pushl %1\n\t"*/
        /*"pushl %2\n\t"*/
        /*"call file_write\n\t"*/
        /*"addl $12,%%esp\n\t"*/
        /*"popl %0\n\t"*/
        /*"pop %%fs"*/
        /*::"r" (count),"r" (&krn_log_file),"r" (krn_log_inode):"ax","cx","dx");*/
/*}*/
