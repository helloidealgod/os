int main(void){
//	con_init();
//	trap_init();
//	con_init();	
	printk("hello world! \r\n");
	while(1);
	return 0;
}
/*
void test(const char* format,...){
	char *c = *(&format);
	printk(c);
	char c1 = *((char *)(&format + 1));
	if(c1 == 'a'){
		printk("success");
	}else{
		printk("failed");
	}
	int c2 = *((int *)(&format + 2));
	if(123 == c2){
		printk(" success");
	}else{
		printk(" failed");
	}
	long c3 = *((long *)(&format + 3));
	if(456 == c3){
		printk(" success");
	}else{
		printk(" failed");
	}
	double c4 = *((double *)(&format + 4));
	if(9876 == c4){
		printk(" success");
	}else{
		printk(" failed");
	}else{
		printk(" failed");
	}

}*/
