#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include "aid.h"

#define MAXLINE 1000
#define MAXVARS 26
struct
{
	char infix[MAXLINE];
	char postfix[MAXLINE];
	char vnum[MAXVARS];		// stores on which place stands i-th var (if stands >=0)
	char letter[MAXVARS];	// stores which letter stands in i-th place in table sets
	char nvars;
}
typedef Expression;

#define ORDER_ALPHA 0 // alphabetic order
#define ORDER_EXPR 1  // vars go as they appear in expression
#define ORDER_RAND 2  // random order
void initExpression(Expression *expr);
int infixToPostfix(Expression *expr);
void setOrder(Expression *expr, int order_type);
void makeTableTXT(Expression expr);
char eval(Expression expr, char *set);

// vars for stack
#define MAXSTACK 200
char stk[MAXSTACK];
char *sp = stk;
// methods for stack
char pop();
void push(char c);
void printStk();	// has been used for debug
int stackEmpty();
int topOp();
void resetStack();

int fgetline(FILE *f, char *dest, int lim);

// all for operations
#define EMPTY ' '
#define NEG '!'
#define NEG2 '~'
#define AND '*'
#define OR '+'
#define XOR '^'
#define THEN '>'
#define EQ '='
int priority(char c);	// returns relative priority of an operation 

// some things for list
struct node
{
	char val;
	struct node *next;
}
typedef Node;

// list made to be with first fictive element, for simplicity
Node* initList();
void addNode(Node *head, char c);
char extractNode(Node *head, int n);
void printList(Node *head);	// has been used for debug


//======================================//
//              DEFINITION              //
//======================================//
int main(int argc, char const *argv[])
{
	Expression e;
	initExpression(&e);
	//setOrder(&e,ORDER_RAND);
	makeTableTXT(e);

	return 0;
}

void initExpression(Expression *expr)
{
	// read line from file
	FILE *f;
	f = fopen("input.txt","r");
	if (f==NULL){
		fprintf(stderr, "No input file! Expression must be in input.txt\nDefault expression used\n");
		*(expr->infix)=NEG;
		*(expr->infix+1)='A';
		*(expr->infix+2)='\0';
	}
	else{
		fgetline(f,expr->infix,MAXLINE);
		fclose(f);
	}
	*(expr->postfix)='\0';
	// impossimble numbers for each var
	for(int i=0; i<MAXVARS; i++){
		expr->vnum[i]=-1;
		expr->letter[i]=-1;
	}
	expr->nvars=0;
	setOrder(expr,ORDER_ALPHA);	// alphabetic order by default, can be reset later
	infixToPostfix(expr);
}

int fgetline(FILE *f, char *dest, int lim)
{
	int i;
	char *tp = dest;
	char c;
	for(i=0; i<lim-1 && (c=fgetc(f))!='\n' && c!=EOF; i++){
		*tp=c;
		tp++;
	}
	*tp='\0';
	return i;
}

int infixToPostfix(Expression *expr)
{
	char *tp = expr->infix;
	char *mp = expr->postfix;
	char op1;
	char op2 = EMPTY; // HACK: EMPTY has the highest priority, so it will not be pushed in the beginning

	// Implementation of the Shunting-yard algotithm
	while((op1=*tp)!='\0'){
		if(isalpha(op1)){
			*mp=toupper(op1);
			mp++;
		}
		else switch(op1){
			case NEG: case NEG2:{
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
				while( !stackEmpty() && (op2=pop())!='(' ){
					*mp = op2;
					mp++;
				}
				if(stackEmpty() && op2!='('){
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
	// pop all remaining from stack
	while(!stackEmpty()){
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
	// place line ending for prefix form
	*mp='\0';
	return 0;
}

void setOrder(Expression *expr, int order_type)
{
	for(int i=0; i<MAXVARS; i++){
		expr->vnum[i]=-1;
		expr->letter[i]=-1;
	}
	char *p = expr->infix;
	char var;
	char *vnum = expr->vnum;
	char *letter = expr->letter;

	switch(order_type){
		case ORDER_ALPHA:{
			while(*p!='\0'){
				if (isalpha(*p)){
					var = toupper(*p)-'A';
					if(vnum[var]<0)
						vnum[var]=0;
				}
				p++;
			}
			int n=0;
			for(int i=0; i<MAXVARS; i++)
				if(vnum[i]==0){
					vnum[i]=n;
					letter[n]=i;
					n++;
				}
			expr->nvars=n;
			break;
		}
		case ORDER_EXPR:{
			int n=0;
			while(*p!='\0'){
				if(isalpha(*p)){
					var = toupper(*p)-'A';
					if(vnum[var]<0){
						vnum[var]=n;
						n++;
					}
				}
				p++;
			}
			for(int i=0; i<MAXVARS; i++)
				if(vnum[i]>=0)
					letter[vnum[i]]=i;
			expr->nvars=n;
			break;
		}
		case ORDER_RAND:{
			int n=0;
			while(*p!='\0'){
				if(isalpha(*p)){
					var = toupper(*p)-'A';
					if(vnum[var]<0){
						vnum[var]=0;
						n++;
					}
				}
				p++;
			}
			expr->nvars=n;
			Node *numlist = initList();
			for(int i=0; i<n; i++)
				addNode(numlist,i);
			srand(time(NULL));
			for(int i=0; i<MAXVARS; i++)
				if(vnum[i]==0){
					vnum[i]=extractNode(numlist,rand()%n);
					letter[vnum[i]]=i;
					n--;
				}
			break;
		}
		default:{	// alphabetic order by default
			setOrder(expr,ORDER_ALPHA);
			return;
		}
	}
}

void makeTableTXT(Expression expr)
{
	FILE *f;
	f = fopen("table.txt","w");

	for(int i=0; i<expr.nvars; i++)
		fprintf(f,"%c",expr.letter[i]+'A');
	fprintf(f," %s\n",expr.infix);
	for(int i=0; i<expr.nvars +1 +10; i++)
		fprintf(f, "-");
	fprintf(f, "\n");

	int nlines = ipow(2,expr.nvars);
	int ntrue = 0;
	char set[MAXVARS];
	int t;
	char val;
	for(int i=0; i<nlines; i++){
		t=i;
		for(int j=expr.nvars-1; j>=0; j--){
			set[j]=t%2;
			t>>=1;
		}
		for(int j=0; j<expr.nvars; j++)
			fprintf(f,"%c",set[j]+'0');
		if((val=eval(expr,set))!=0) ntrue++;
		fprintf(f," %c\n",val+'0');
	}

	fprintf(f, "\nTRUE: %d\nFALSE: %d\n", ntrue, nlines-ntrue);

	fclose(f);
}

char eval(Expression expr, char *set)
{
	resetStack();
	char *line = expr.postfix;
	char *vnum = expr.vnum;
	char op2;
	while(line-(expr.postfix)+1<MAXLINE && *line!='\0'){
		switch(*line){
			case NEG: case NEG2:{
				if(pop()==1)
					push(0);
				else
					push(1);
				break;
			}
			case AND:{
				op2=pop();
				if(pop()==1 && op2==1)
					push(1);
				else
					push(0);
				break;
			}
			case OR:{
				op2=pop();
				if(pop()==0 && op2==0)
					push(0);
				else
					push(1);
				break;
			}
			case XOR:{
				op2=pop();
				if(pop()==op2)
					push(0);
				else
					push(1);
				break;
			}
			case THEN:{
				op2=pop();
				if(pop()==1 && op2==0)
					push(0);
				else
					push(1);
				break;
			}
			case EQ:{
				op2=pop();
				if(pop()==op2)
					push(1);
				else
					push(0);
				break;
			}
			default:{
				if(isalpha(*line))
					push(set[vnum[*line-'A']]);
				break;
			}
		}
		line++;
	}
	if (!stackEmpty())
		return pop();
	else
		return -1;
}

char pop()
{
	if(sp!=stk){
		sp--;
		return *(sp);
	}
	else{
		fprintf(stderr, "Stack is empty! Can't pop!\n" );
		return 0;
	}
}

void push(char c){
	if(sp-stk+1<MAXSTACK){
		*sp=c;
		sp++;
	}
	else
		fprintf(stderr, "Stack is full! Can't push!\n" );
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

int stackEmpty()
{
	return !(sp>stk);
}

int priority(char c)
{
	switch(c){
		case EMPTY:{return 999; break;}
		case NEG: case NEG2:{return 5; break;}
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
	if(stackEmpty())
		return 0;
	if(*(sp-1)=='(')
		return 0;
	else
		return 1;
}

void resetStack()
{
	sp = stk;
}

Node* initList()
{
	Node *p;
	p = (Node*)malloc(sizeof(Node));
	p->next=NULL;
	p->val=-1;
}

void addNode(Node *head, char c)
{
	Node *p = head;
	while(p->next!=NULL)
		p=p->next;
	p->next= (Node*)malloc(sizeof(Node));
	p=p->next;
	p->next=NULL;
	p->val=c;
}

char extractNode(Node *head, int n)
{
	Node *p = head;
	int i;
	for(i=n; i>0 && p->next!=NULL; i--)
		p=p->next;
	if(i!=0 || head->next==NULL){	// list ended
		fprintf(stderr,"\nElement #%d not found in list!\n",n);
		return -1; // possible char with value = -1 here?
	}
	Node *nxt = p->next->next;
	char val=p->next->val;
	free(p->next);
	p->next = nxt;
	return val;
}

void printList(Node *head)
{
	Node *p = head->next;
	while(p!=NULL){
		printf("%d ",p->val);
		p=p->next;
	}
	printf("\n");
}