/*
驱动程序
*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h> //copy_to_user, copy_from_user
#include <linux/io.h> //ioremap, writeb, readb等

MODULE_LICENSE("GPL");
dev_t devno;

struct class *cls;

volatile unsigned int *GPH3CON = NULL;
volatile unsigned int *GPH3DAT = NULL;

//准备一个open函数，用来服务open请求
int myopen(struct inode *ino, struct file *file)
{
    printk(KERN_CRIT"fake file was open!\n");
    GPH3CON = ioremap(0xE0200C60, 4);//地址重映射
    GPH3DAT = ioremap(0xE0200C64, 4);
    writel(0x1 << 8, GPH3CON); //gpio设置成输出功能
    return 0;
}

//close函数
int myclose(struct inode *ino, struct file *file)
{
    printk(KERN_CRIT"fake file was close!\n");
    iounmap(GPH3CON);//取消映射
    iounmap(GPH3DAT);
    return 0;
}

//read函数,从内核空间将数据拷贝到用户空间
ssize_t myread(struct file *file, char __user *buffer, size_t len, loff_t *off)
{
    int num = 2017;
    ssize_t ret = copy_to_user(buffer, &num, sizeof num);
    return ret;
}

//write函数，从用户空间将数据拷贝到内核空间
ssize_t mywrite(struct file *file, const char __user *buffer, size_t len, loff_t *off)
{
    unsigned int num;
    ssize_t re = copy_from_user(&num, buffer, len);
    writel(0x00, GPH3DAT);
    writel(~(0x1 << num), GPH3DAT);

    return re;
}

//准备好文件操作结构体
struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = myopen,//将自定义的open服务函数交给它
    .release = myclose,
    .read = myread,
    .write = mywrite
};


int __init mydriverinit(void)
{
    alloc_chrdev_region(&devno, 10, 1, "mydriver");
    //注册驱动
    register_chrdev(MAJOR(devno), "mydriver", &fops);
    cls = class_create(THIS_MODULE, "myclass");
    device_create(cls, NULL, devno, NULL, "mydevice");
    return 0;
}

module_init(mydriverinit);

void __exit mydriverexit(void)
{
    device_destroy(cls, devno);
    class_destroy(cls);
    unregister_chrdev(MAJOR(devno), "mydriver");
    unregister_chrdev_region(devno, 1);

}

module_exit(mydriverexit);
