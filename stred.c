#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/device.h>
#define BUFF_SIZE 110
MODULE_LICENSE("Dual BSD/GPL");

dev_t my_dev_id;
static struct class *my_class;
static struct device *my_device;
static struct cdev *my_cdev;

char string[100];
//int pos = 0;
int endRead = 0;

int stred_open(struct inode *pinode, struct file *pfile);
int stred_close(struct inode *pinode, struct file *pfile);
ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);

struct file_operations my_fops =
{
	.owner = THIS_MODULE,
	.open = stred_open,
	.read = stred_read,
	.write = stred_write,
	.release = stred_close,
};


int stred_open(struct inode *pinode, struct file *pfile) 
{
		printk(KERN_INFO "Succesfully opened stred\n");
		return 0;
}

int stred_close(struct inode *pinode, struct file *pfile) 
{
		printk(KERN_INFO "Succesfully closed stred\n");
		return 0;
}

ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset) 
{
	int ret;
	char buff[BUFF_SIZE];
	int str_len = strlen(string);
	if (endRead){
		endRead = 0;
		return 0;
	}

	//if(pos > 0)
	//{
		//pos --;
		//len = scnprintf(buff, BUFF_SIZE, "%d ", string[pos]);
		strcat(buff,string);
		ret = copy_to_user(buffer, buff, str_len);
		if(ret)
			return -EFAULT;
		printk(KERN_INFO "Succesfully read\n");
	endRead = 1;
	//}
	//else
	//{
	//		printk(KERN_WARNING "Stred is empty\n"); 
	//}

	return str_len;
}

ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) 
{
	char buff[BUFF_SIZE];
	unsigned int word;
	int str_len = strlen(string);
	char *tmp;	
	int ret;
	int first_char=0;
	int i=0;

	ret = copy_from_user(buff, buffer, length);
	if(ret)
		return -EFAULT;
	buff[length-1] = '\0';
	if(!strncmp("string=",buff,7))
	{
		printk(KERN_INFO "Input string successfully stored in buffer!\n");
		strcpy(string, (buff+7));
	}
	else if(!strncmp("clear",buff,5))
	{
		for(i = 0; i < 100; i++) *(string+i) = 0;
		printk(KERN_INFO "Internal buffer cleared successfully!\n");
	}
	else if(!strncmp("shrink",buff,6))
	{
		tmp = strim(string);
		i = 0;
		while(string[i] != '\0')
		{
			i++;
			string[i] = (*(tmp + i));
		}
		string[i] = '\0';
		/*while(string[i] == ' ') i++;
		first_char = i;
		i = str_len - 2;
		while(string[i] == ' ') i--;
		string[i + 1] = 0;
		str_len = strlen(string);
		i = 0;
		while(i != str_len - 1) string[i] = string[first_char + i]; */
		printk(KERN_INFO "Shirnk completed successfully!\n");
	}
	else if(!strncmp("append=", buff, 7))
	{
		if(100 - (length - 1) - str_len > 0)
		{
			strcat(string,(buff + 7));
			printk(KERN_INFO "String appended successfully!\n");
		}
		else
			printk(KERN_WARNING "String too long!\n"); 
	}
	else if(!strncmp("truncate=", buff, 9))
	{
		ret = sscanf(buff,"%d",&word);
 		if( ret != 1 || word < (str_len - 1))
			printk(KERN_WARNING "Wrong input in function TRUNCATE %d %d \n",ret, word);
		else
		{
			string[(str_len - 1) - word] = '\0'; 
			printk(KERN_INFO "Last %d elements are removed.\n",word);
		}				
	}
	/*else if(!strcmp("remove=", buff, 6))
	{
		ret = sscanf(buff, *(buff+7), &word);
		i = ret;
		if(strstr(word, string) != NULL)
			while(i < (str_len - 1))
			{
				
				i++;
			}
		else printk(KERN_INFO "No matches!\n");
	}*/	

	else printk(KERN_WARNING "Invalid function call.\n");	

	/*if(pos<100)
	{
		//ret = sscanf(buff,"%d",&word);
		//if(ret==1)//one parameter parsed in sscanf
		{
			printk(KERN_INFO "Succesfully wrote value %d", word); 
			stred[pos] = value; 
			pos=pos+1;
		}
		else
		{
			printk(KERN_WARNING "Wrong command format\n");
		}
	}
	else
	{
		printk(KERN_WARNING "Input string doesn't meet requirements! Max 100 characters.\n"); 
	}
*/
	return length;

}

static int __init stred_init(void)
{
   int ret = 0;
	int i=0;

	//Initialize array
	for (i=0; i<100; i++)
		string[i] = 0;

   ret = alloc_chrdev_region(&my_dev_id, 0, 1, "stred");
   if (ret){
      printk(KERN_ERR "failed to register char device\n");
      return ret;
   }
   printk(KERN_INFO "char device region allocated\n");

   my_class = class_create(THIS_MODULE, "stred_class");
   if (my_class == NULL){
      printk(KERN_ERR "failed to create class\n");
      goto fail_0;
   }
   printk(KERN_INFO "class created\n");
   
   my_device = device_create(my_class, NULL, my_dev_id, NULL, "stred");
   if (my_device == NULL){
      printk(KERN_ERR "failed to create device\n");
      goto fail_1;
   }
   printk(KERN_INFO "device created\n");

	my_cdev = cdev_alloc();	
	my_cdev->ops = &my_fops;
	my_cdev->owner = THIS_MODULE;
	ret = cdev_add(my_cdev, my_dev_id, 1);
	if (ret)
	{
      printk(KERN_ERR "failed to add cdev\n");
		goto fail_2;
	}
   printk(KERN_INFO "cdev added\n");
   //printk(KERN_INFO "Hello world\n");

   return 0;

   fail_2:
      device_destroy(my_class, my_dev_id);
   fail_1:
      class_destroy(my_class);
   fail_0:
      unregister_chrdev_region(my_dev_id, 1);
   return -1;
}

static void __exit stred_exit(void)
{
   cdev_del(my_cdev);
   device_destroy(my_class, my_dev_id);
   class_destroy(my_class);
   unregister_chrdev_region(my_dev_id,1);
   //printk(KERN_INFO "Goodbye, cruel world\n");
}


module_init(stred_init);
module_exit(stred_exit);
