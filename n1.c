#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include "aid.h"

#define N1TYPES 3

void mk1_1();	// binary 1/0 in HEX numbers
void mk1_2();	// binary 1/0 in OCT numbers
void mk1_3();	// find sum of BIN, HEX, OCT

int main(int argc, char const *argv[])
{
	setlocale(LC_ALL,"Russian");

	int type0;
	srand(time(NULL));
	type0 = rand() % N1TYPES;
	switch(type0){
		case 0: {mk1_1(); break;}
		case 1: {mk1_2(); break;}
		case 2: {mk1_3(); break;}
		default: {}
	}
/*	for(type0=0; type0<99; type0++){
		mk1_1();
		printf("\n");
	}*/
	return 0;
}

#define N1_1_HEXDIG 4
void mk1_1()
{
	int number;
	number=0;
	number+= rand() % ipow(16,N1_1_HEXDIG-1) + (rand() % 15+1)*ipow(16,N1_1_HEXDIG-1);

	int type1 = rand()%2;

	printf("Сколько %s в двоичной записи шестнадцатеричного числа %X?\n", type1?"единиц":"значащих нулей",number);

	int n0, n1;
	n0=n1=0;
	while(number>0){
		if(number%2==1)
			n1++;
		else
			n0++;
		number/=2;
	}

	number = getchar();
	printf("Ответ: %d\n", type1?n1:n0);
	//return 0;
}

#define N1_2_OCTDIG 4
void mk1_2()
{
	int number;
	number=0;
	number+= rand() % ipow(8,N1_2_OCTDIG-1) + (rand() % 7+1)*ipow(8,N1_2_OCTDIG-1);

	int type1 = rand()%2;

	printf("Сколько %s в двоичной записи восьмеричного числа %o?\n", type1?"единиц":"значащих нулей",number);

	int n0, n1;
	n0=n1=0;
	while(number>0){
		if(number%2==1)
			n1++;
		else
			n0++;
		number/=2;
	}

	number = getchar();
	printf("Ответ: %d\n", type1?n1:n0);
	// return 0;
}

#define N1_3TYPES 2
#define N1_3_BINDIG 8
#define N1_3_SHFTMAX 13
#define N1_3_RESTMAX 40
void mk1_3()
{
	char s1[100];
	char s2[100];
	char s3[100];
	int n1,n2,n3,answer;
	int type2=rand()%N1_3TYPES;

	n1 = rand()%ipow(2,N1_3_BINDIG-1) + ipow(2,N1_3_BINDIG);
	n2 = n1-(rand()%N1_3_SHFTMAX);
	n3 = rand() % N1_3_RESTMAX;
	answer = n1-n2+n3;
	ntobin(n1,s1);
	printf("Вычислить:\n");

	switch(type2){
		case 0:{	// BIN - OCT + HEX
			ntooct(n2,s2);
			ntohex(n3,s3);
			printf("%s_2 - %s_8 + %s_16\n",s1,s2,s3);
			break;}
		case 1:{	// BIN - HEX + OCT
			ntohex(n2,s2);
			ntooct(n3,s3);
			printf("%s_2 - %s_16 + %s_8\n",s1,s2,s3);
			break;}
		default:{}
	}
	printf("Ответ дать в десятичной системе счисления\n");

	n1 = getchar();
	printf("Ответ: %d\n",answer);
}