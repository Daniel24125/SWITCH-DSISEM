## 2.1) Which function is executed when this module is loaded?
The function blinker_init

## 2.2) How do you show that this is a character device driver?
There are two main indicators in the code that prove this is a character device driver:

The Registration Function: Inside blinker_init, the module calls register_chrdev(...). This is the standard Linux kernel function used to register a character device.

File Operations Structure: The code defines a struct file_operations named blinker_fops. Character devices rely on this structure to map standard system calls (like read, write, open, and release) to the specific functions implemented in the driver.

## 2.3) What is the major number for this device driver?
The major number is dynamically allocated by the kernel.

## 2.4) When is the pisca_read function called? How is that defined?
When it's called: It is executed whenever a user-space program (or a terminal command like cat) attempts to read from the device file (e.g., /dev/blinker).

How it's defined: It is linked to the system's read action via the blinker_fops structure, specifically on the line .read = blinker_read,.

## 2.5) When is the pisca_write function called? How is that defined?
When it's called: It is executed whenever a user-space program (or a terminal command like echo "500" > /dev/blinker) attempts to send data to the device file.

How it's defined: It is linked to the system's write action via the blinker_fops structure, specifically on the line .write = blinker_write,.

## 2.6) When is the my_timer_func function called? How is that defined?
When it's called: It is called periodically whenever the kernel timer my_timer expires. The first execution happens after blink_delay jiffies (set during module initialization). Inside the function itself, it re-arms the timer (my_timer.expires += blink_delay; add_timer(&my_timer);), creating an infinite periodic loop until the module is removed.

How it's defined: It is registered as a timer callback inside blinker_init using the timer_setup(&my_timer, my_timer_func, 0); macro.