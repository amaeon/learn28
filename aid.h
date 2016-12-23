long ipow(int a, int b);
int ntobin(long n, char *res);
int ntooct(long n, char *res);
int ntohex(long n, char *res);

// IMPLEMENTATION

long ipow(int a, int b)
{
	long res = 1;
	while(b>0){
		res=res*a;
		b--;
	}
	return res;
}

int ntobin(long n, char *res)
{
	char t[65];
	int i=0;
	t[0]='0';
	while(n>0){
		t[i]= (n%2)+'0';
		i++;
		n/=2;
	}
	int j;
	for(j=0,i--;i>=0;i--,j++){
		res[j]=t[i];
	}
	res[j]='\0';
	return 0;
}

int ntooct(long n, char *res)
{
	char t[23];
	int i=0;
	t[0]='0';
	while(n>0){
		t[i]= (n%8)+'0';
		i++;
		n/=8;
	}
	int j;
	for(j=0,i--;i>=0;i--,j++){
		res[j]=t[i];
	}
	res[j]='\0';
	return 0;
}

int ntohex(long n, char *res)
{
	char t[17];
	int i=0;
	char d;
	t[0]='0';
	while(n>0){
		t[i]= ((d=n%16)<10)?d+'0':(d-10+'A');
		i++;
		n/=16;
	}
	int j;
	for(j=0,i--;i>=0;i--,j++){
		res[j]=t[i];
	}
	res[j]='\0';
	return 0;
}