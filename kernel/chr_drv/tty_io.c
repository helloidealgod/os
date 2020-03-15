void copy_to_cooked(){
	printk("hello copy_to_cooked\n");
}

void do_tty_interrupt(int tty){
	copy_to_cooked();
}

