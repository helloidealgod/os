volatile void panic(const char * s){
	printk("Kernel panic:%s\n",s);
	for(;;);
}
