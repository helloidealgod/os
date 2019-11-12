void itoa(int n,char s[]){
	int i,j,sign;
	char temp;
	if((sign=n) < 0)
		n = -n;
	i = 0;
	do{
		s[i++]=n%10+'0';
	}while((n/=10)>0);
	if(sign < 0)
		s[i++]='-';
	s[i]='\0';
	for(j=0;j<i/2;j++){
		temp = s[j];
		s[j] = s[i-j-1];
		s[i-j-1] = temp;
	}
}
