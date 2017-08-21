#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>

#include <linux/kernel.h>
#include <linux/sched.h>
#include <stdarg.h>
#include <asm/segment.h>

const char ps_info_head[]="pid state father counter start_time\n";
int proc_read(struct m_inode * inode, char * buf) {
    switch(inode->i_zone[0]) {
        case 0:
            /*ps info*/
            read_ps_info(buf);
            return;
            break;
        default:
            return -EINVAL;
    }
}
int sprintf(char *buf, const char *fmt, ...) {
    va_list args; int i;
    va_start(args, fmt);
    i=vsprintf(buf, fmt, args);
    va_end(args);
    return i;
}

/*void print_task(const task_struct * tsk, char * info) {*/
/*}*/
char * cp2usr(char * usr_buf, char * krn_buf) {
    while('\0' != *(krn_buf++)) {
        put_fs_byte(*krn_buf, usr_buf++);
    }
    return usr_buf;
}
int read_ps_info(char * usr_buf) {
    static int ps_info_id = 0;
    char info_line[64];
    int i = 0;
    int len = 0;
    int wr_cnt = 0;
    for(i = ps_info_id; i < NR_TASKS; ++i) {
        if(NULL != task[i]) {
            if(0 == i) {
                usr_buf = cp2usr(usr_buf, ps_info_head);
                wr_cnt += sizeof(ps_info_head);
            }
            len = sprintf(info_line, "#%ld %ld %ld %ld %ld\n", task[i]->pid, task[i]->state, task[i]->father, task[i]->counter, task[i]->start_time);
            wr_cnt += len;
            info_line[len] = '\0';
            usr_buf = cp2usr(usr_buf, info_line);
            break;
        }
    }

    if(i == NR_TASKS) {
        ps_info_id = 0;
        return 0;
    }else {
        put_fs_byte(0, usr_buf++);
        ps_info_id = i+1;
        return wr_cnt;
    }
    
}
