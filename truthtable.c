#include <stdio.h>
#include <ctype.h>

#define MAXLINE 1000
#define MAXVARS 26
struct
{
	char infix[MAXLINE];
	char postfix[MAXLINE];
	char vars[MAXVARS];
	short order[MAXVARS];
}
typedef Expression;

// vars for stack
#define MAXSTACK 200
char stk[MAXSTACK];
char *sp = stk;
// methods for stack
char pop();
void push(char c);
void printStk();
int emptyStk();
int topOp();

void initExpression(Expression *expr);
int fgetline(char *dest, int lim);

// all for operations
#define EMPTY ' '
#define NEG '~'
#define AND '*'
#define OR '+'
#define XOR '^'
#define THEN '>'
#define EQ '='
int priority(char c);


int infixToPostfix(Expression *expr);
void makeTableTXT();
char eval();

int main(int argc, char const *argv[])
{
	Expression e;
	initExpression(&e);
	infixToPostfix(&e);

	printf("%s\n",e.postfix);


	return 0;
}

void initExpression(Expression *expr)
{
	fgetline(expr->infix,MAXLINE);
}

int fgetline(char *dest, int lim)
{
	FILE *f;
	f = fopen("input.txt","r");

	int i;
	char *tp = dest;
	char c;
	for(i=0; i<lim-1 && (c=fgetc(f))!='\n' && c!=EOF; i++){
		*tp=c;
		tp++;
	}
	*tp='\0';

	fclose(f);
	return i;
}

void makeTableTXT(char *pfx)
{
	FILE *f;
	f = fopen("table.txt","w");


	fclose(f);
}

int infixToPostfix(Expression *expr)
{
	char *tp = expr->infix;
	char *mp = expr->postfix;
	char op1;
	char op2 = EMPTY; // HACK: EMPTY has the highest priority, so it will not be pushed in the beginning

	// Implementation of the Shunting-yard algotithm
	while((op1=*tp)!='\0'){
		//printf("\nINSTRING %c",*tp);
		if(isalpha(op1)){
			*mp=toupper(op1);
			mp++;
		}
		else switch(op1){
			case NEG:{
				push(op1);
				break;
			}
			case AND:{
				while( topOp() && priority(op2=pop())>=priority(AND) ){
					*mp = op2;
					mp++;
				}
				if(priority(op2)<priority(op1)) push(op2);
				push(AND);
				break;
			}
			case OR:{
				while( topOp() && priority(op2=pop())>=priority(OR) ){
					*mp = op2;
					mp++;
				}
				if(priority(op2)<priority(op1)) push(op2);
				push(OR);
				break;
			}
			case XOR:{
				while( topOp() && priority(op2=pop())>=priority(XOR) ){
					*mp = op2;
					mp++;
				}
				if(priority(op2)<priority(op1)) push(op2);
				push(XOR);
				break;
			}
			case THEN:{
				while( topOp() && priority(op2=pop())>=priority(THEN) ){
					*mp = op2;
					mp++;
				}
				if(priority(op2)<priority(op1)) push(op2);
				push(THEN);
				break;
			}
			case EQ:{
				while( topOp() && priority(op2=pop())>=priority(EQ) ){
					*mp = op2;
					mp++;
				}
				if(priority(op2)<priority(op1)) push(op2);
				push(EQ);
				break;
			}
			case '(':{
				push('(');
				break;
			}
			case ')':{
				while( !emptyStk() && (op2=pop())!='(' ){
					*mp = op2;
					mp++;
				}
				if(emptyStk() && op2!='('){
					fprintf(stderr,"Incorrect sequence: some brackets missing!\n");
					return -1;
				}
				if(op2=='(');
					op2=EMPTY; // Reset of op2, otherwise it will be pushed back
				break;
			}
			default:{
				break;
			}
		}
		tp++;
	}
	while(!emptyStk()){
		if (topOp()){
			*mp = pop();
			mp++;
		}
		else{
			fprintf(stderr,"Incorrect sequence: brackets not closed!\n");
			return -1;
			break;
		}
	}
	*mp='\0';
	return 0;
}

char pop()
{
	//printf("\nPOP %c",*(sp-1));
	if(sp!=stk){
		sp--;
		//printStk(); // -
		return *(sp);
	}
	else{
		fprintf(stderr, "Stack is empty! Can't pop!\n" );
		//printStk(); // -
		return 0;
	}
}

void push(char c){
	//printf("\nPUSH %c",c);
	if(sp-stk+1<MAXSTACK){
		*sp=c;
		sp++;
	}
	else
		fprintf(stderr, "Stack is full! Can't push!\n" );
	//printStk();
}

void printStk()
{
	char *tp = sp;
	printf("\nSTACK: ");
	while(tp>stk){
		tp--;
		printf("%c",*tp); 
	}
}

int emptyStk()
{
	return !(sp>stk);
}

int priority(char c)
{
	switch(c){
		case EMPTY:{return 999; break;}
		case NEG:{return 5; break;}
		case AND:{return 4; break;}
		case OR:{return 3; break;}
		case XOR:{return 3; break;}
		case THEN:{return 2; break;}
		case EQ:{return 1; break;}
		case '(':{return 0; break;}
		default: {return -1; break;}
	}
}

int topOp()
{
	if(emptyStk())
		return 0;
	if(*(sp-1)=='(')
		return 0;
	else
		return 1;
}