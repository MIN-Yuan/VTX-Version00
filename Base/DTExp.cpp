#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>

#include "DTExp.h"
#include "DTStr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
��Ҫע����ǣ���һ��DEXXP�࣬��Ӧ��һ�����ʽ
�磺a+b

���еĲ�����������������������¼�ı��ʽ����
*/
const char * EX_STR_U_FMT ="%u";
const char * EX_STR_D_FMT ="%d";
const char * EX_STR_F_FMT ="%f";
//const char * EX_STR_G_FMT ="%f";




CDTExp::CDTExp()
{

}

CDTExp::~CDTExp()
{

}



double CDTExp::Calculate(LPCSTR expression)  //����
{
	//���ʽ���ȹ���
	if(strlen(expression) >= MAX_EXPRESSION_LEN-8)
	{
		return 0;
	}

	Expression = expression;//���ʽ
	PSourceExpression = expression;//Դ���ʽ

	Expression.RemoveChars(1,'\"');//ȥ��"��

	CFmt2InternalFmt(); //ת��Ϊ�ڲ���ʽ
	ReplaceHexConst(); //�滻16���Ƴ���
	//ReplaceBinConst();
	ReplaceCharConst();

	if(!CalcVars()) //�������б��ʽ��
	{
		return 0;
	}

	return atof(Expression.GetBuffer());
}

void CDTExp::CFmt2InternalFmt()//C��ʽת��Ϊ�ڲ���ʽ
{
	//Expression.Trim(4,' ','\'','\"',';');	

	//Expression.Trim(2,' ',';'); //ȥ���ַ����еĳ���
	//����x��Ҫת�����ַ����ο��ĵ�vedix.doc ��XML�ļ��淶

	Expression.ReplaceAll("]]","Y",1);	//Ϊ��XML��ʽ֧��
	Expression.ReplaceAll("[[","Z",1);	//Ϊ��XML��ʽ֧��

	Expression.ReplaceAll("]=","}",1);	//Ϊ��XML��ʽ֧��	
	Expression.ReplaceAll("[=","{",1);	//Ϊ��XML��ʽ֧��
	
	Expression.ReplaceAll("&&","W",1);
	Expression.ReplaceAll("&","@",1);
	Expression.ReplaceAll("||","R",1);
	Expression.ReplaceAll("!=","N",1);
	Expression.ReplaceAll("!","0!",2);	
	Expression.ReplaceAll("~","0~",2);	
	Expression.ReplaceAll("==","=",1);
	Expression.ReplaceAll(">>","Y",1);
	Expression.ReplaceAll("<<","Z",1);
	Expression.ReplaceAll(">=","}",1);
	Expression.ReplaceAll("<=","{",1);
	Expression.ReplaceAll(">","]",1);
	Expression.ReplaceAll("<","[",1);
}

void CDTExp::ReplaceCharConst()//�����ַ�������
{
	CDTStr tempstr;
	BOOL inquotation=FALSE;

	INT i;
	char * p;
	for(i=0; i<Expression.GetLength()-1; i++)
	{
		p=Expression.GetBuffer();

		if(p[i]=='"')//�ҳɶԵ�"��
		{
			inquotation=!inquotation;
		}else
		{
			if(!inquotation) //
			{
				if(p[i]=='\'')
				{	//����ڷ��ַ����ҵ�'��
					if(p[i+1]=='\\' && p[i+3]=='\'')
					{	//����'\?'����
						if(p[i+2]=='\"')
						{
							tempstr.Format(EX_STR_D_FMT,'\"');
							Expression.Replace(i,4,tempstr.GetBuffer(),tempstr.GetLength());
							continue;
						}else if(p[i+2]=='\\')
						{
							tempstr.Format(EX_STR_D_FMT,'\\');//%d
							Expression.Replace(i,4,tempstr.GetBuffer(),tempstr.GetLength());
							continue;
						}else if(p[i+2]=='r') //�滻Ϊ\r
						{
							tempstr.Format(EX_STR_D_FMT,'\r');
							Expression.Replace(i,4,tempstr.GetBuffer(),tempstr.GetLength());
							continue;
						}else if(p[i+2]=='n')
						{
							tempstr.Format(EX_STR_D_FMT,'\n');
							Expression.Replace(i,4,tempstr.GetBuffer(),tempstr.GetLength());
							continue;
						}else if(p[i+2]=='t')//�滻Ϊ\t
						{
							tempstr.Format(EX_STR_D_FMT,'\t');
							Expression.Replace(i,4,tempstr.GetBuffer(),tempstr.GetLength());
							continue;
						}

					}else if(p[i+2]=='\'')
					{	//��ͨ''����
						tempstr.Format(EX_STR_D_FMT,Expression.GetAt(i+1));
						Expression.Replace(i,3,tempstr.GetBuffer(),tempstr.GetLength());
						continue;
					}
				}
			}
		}
	}


}


void CDTExp::ReplaceBinConst()//��������Ƴ���
{
	long b,e;
	int i=0;
	CDTStr str;
	char c;
	unsigned long ul;
	char * endpos;

	while((b=Expression.FindI(0,"0b",2,0))!=-1)
	{

		e=Expression.GetLength();

		//�ҳ�����������β
		for(i=b+2; i<e; i++)
		{
			c=Expression.GetAt(i);
			if(c!='0' && c!='1') //��0��1�����Ƕ����ƣ�����
			{
				break;
			}
		}
		e=i; //����λ��

		if(e-b-2>32)//���ȴ���32
		{
			e=b+2+32;
		}

		//��ȡ2�������ݲ���
		str.Assign(Expression.GetBuffer()+b+2,e-b-2); 

		ul=strtoul(str.GetBuffer(),&endpos,2);//ת��Ϊ unsigned long ��ʽ

		str.Format("%u",ul);

		//Expression.Remove(b,e-b);
		//Expression.Insert(b,str.GetBuffer(),str.GetLength());
		Expression.Replace(b,e-b,str.GetBuffer(),str.GetLength());//�滻ԭ���ı��ʽ
	}

}


void CDTExp::ReplaceHexConst()//����HEX����,�������òο���ReplaceBinConst()
{
	long b,e;
	long i;
	CDTStr str;
	char * endpos;
	unsigned long ul;
	char c;

	while((b=Expression.FindI(0,"0x",2,0))!=-1)
	{

		e=Expression.GetLength();

		//�ҳ�HEX����������������
		for(i=b+2; i<e; i++)
		{
			c=Expression.GetAt(i);
			if(c==0 || IsOperator(c))
			{
				break;
			}
		}

		e=i;

		if(e-(b+2)>8)
		{
			e=b+2+8;
		}

		str.Assign(Expression.GetBuffer()+b+2,e-b-2);

		ul=strtoul(str.GetBuffer(),&endpos,16);
		
		str.Format(EX_STR_U_FMT,ul);

		//Expression.Remove(b,e-b);
		//Expression.Insert(b,str.GetBuffer(),str.GetLength());
		Expression.Replace(b,e-b,str.GetBuffer(),str.GetLength());
	}
	
}

//////////////////////////////////////////////////////////////////////////////////////
// ���ʽ����ȷ�Ĳ�����
//////////////////////////////////////////////////////////////////////////////////////
BOOL CDTExp::IsOperator(char c)
{
	if(    c=='+'
		|| c=='-' 
		|| c=='*' 
		|| c=='/'
		|| c=='@'	//λ��
		|| c=='|'	//λ��
		|| c=='%'	//ȡ��
		|| c=='='
		|| c==']'
		|| c=='['
		|| c=='}'
		|| c=='{'	
		|| c=='('	
		|| c==')'	//����
		|| c=='^'	//���
		|| c=='N'	//!=
		|| c=='W'	//��
		|| c=='R'	//��
		|| c=='Z'	//����
		|| c=='Y'	//����
		|| c=='!'	//��
		|| c=='~'	//��
		){
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
//�����������ȼ�
//��߼�   12   ()[] ��
//         11   ��~ ++ -- -(type) * & sizeof
//         10    * / %
//         9    + -
//         8    << >>
//         7    <= >=
//         6    == !=
//         5    & ^ |
//         4    &&
//         3    ||
//         2    ?
//         1    = += -= *= /=           
//��ͼ�
//////////////////////////////////////////////////////////////////////////////////////
int CDTExp::GetPriority(char c)//�õ��ַ�[������]�����ȼ�
{
	switch(c)
	{
	case '+':
	case '-':
		return 9;
		//break;
	case '*':
	case '/':
	case '%':
		return 10;
		//break;
	case '@':
	case '|':
	case '^':
		return 5;
		//break;
	case '=':
	case 'N':
		return 6;
		//break;
	case '[':
	case ']':
	case '{':
	case '}':
		return 7;
		//break;
	case 'Z':
	case 'Y':
		return 8;
		//break;
	case 'W':	//&&
		return 4;
		//break;
	case 'R':
		return 3;
		//break;
	case '!':
	case '~':
		return 11;
		//break;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
//�Ƿ�Ϊ��Ч��Ԫ���ַ�
/////////////////////////////////////////////////////////////////////////////////////////
int CDTExp::IsVarChar(int pos, char c)
{
	if( (c>='0' && c<='9')
		|| (pos==0 && c=='-')
		|| (pos > 0 && c=='-' && IsOperator(Expression.GetAt(pos-1)))
		|| c=='.'
		)
	{
		return 1;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
//ȥ���յ�()��.
/////////////////////////////////////////////////////////////////////////////////////////
void CDTExp::RemoveEmptyBracket()
{
	int i;

	//ȥ����Ԫ��������Ŷ�,��(Ԫ��)
	ParseExpression();
	for(i=0; i<VarCount; i++)
	{
		if(Var[i].Begin>0)
		{
			if(Expression.GetAt(Var[i].Begin-1)=='(' && Expression.GetAt(Var[i].Begin+Var[i].Length)==')')
			{
				Expression.Remove(Var[i].Begin+Var[i].Length,1);
				Expression.Remove(Var[i].Begin-1,1);
				ParseExpression();
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//�����ʽ�ֽ��Ԫ��,��¼
//Begin[] ��ʼλ��
//Len[] ����
//Priority[] ���ȼ�
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CDTExp::ParseExpression()
{
	int cur_pri=0;
	int i;
	int var_hold=0;
	int len;
	char c;

	VarCount=0;
	len=Expression.GetLength(); //���ʽ����
	char * p=Expression.GetBuffer();

	for(i=0; i<len+1; i++)
	{
		c=p[i];
		if(IsVarChar(i,c))//��Ч��Ԫ���ַ�
		{
			if(!var_hold)
			{
				Var[VarCount].Begin=i;
				Var[VarCount].Priority=0;	//��ʼ��
				VarCount++;					//������++
				if(VarCount > MAX_VAR_COUNT)
				{
					DMSG(GMsgProgrammer,("Error, Too many vars\r\n"));
				}
			}
			var_hold=1;
		}else//����Ч��Ԫ���ַ�
		{	//operator && \0 && '(' && ')'
			if(var_hold)
			{
				if(VarCount)
				{
					Var[VarCount-1].Length=i-Var[VarCount-1].Begin;
					Var[VarCount-1].Priority=cur_pri+GetPriority(c);
				}
				var_hold=0;
			}

			if(c=='(')
			{
				cur_pri+=12;
			}else if(c==')'){
				cur_pri-=12;
			}
		}
	}// end of for

	return 1;
}

// char CDTExp::ParseExpression()
// {
// 	char p=0;
// 	int i, hold=0;
// 	int len;
// 	
// 	VarCount=0;
// 	len=Expression.GetLength();
// 	
// 	for(i=0; i<len; i++){
// 		if(IsVarChar(i)){
// 			if(!hold){
// 				Var[VarCount].Begin=i;
// 				Var[VarCount].Priority=0;	//��ʼ��
// 				VarCount++;
// 				if(VarCount>MAX_VAR_COUNT){
// 					DMSG(GMsgProgrammer,("Too many vars\r\n"));
// 				}
// 			}
// 			hold=1;
// 		}else{
// 			if(hold){
// 				if(VarCount){
// 					Var[VarCount-1].Length=i-Var[VarCount-1].Begin;
// 					Var[VarCount-1].Priority=p+GetPriority(Expression.GetAt(i));
// 				}
// 				hold=0;
// 			}
// 			
// 			if(Expression.GetAt(i)=='('){
// 				p+=12;
// 			}else if(Expression.GetAt(i)==')'){
// 				p-=12;
// 			}
// 		}
// 	}
// 	
// 	//the last var.
// 	if(hold){
// 		if(VarCount){
// 			Var[VarCount-1].Length=i-Var[VarCount-1].Begin;
// 			Var[VarCount-1].Priority=p+GetPriority(Expression.GetAt(i));
// 		}
// 		hold=0;
// 	}
// 	
// 	
// 	return 1;
// }
// 
// 
/////////////////////////////////////////////////////////////////////////////////////////
//�ҵ�������ȼ�Ԫ�ص�ID
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CDTExp::FindTopMostPriorityID()
{
	char i;
	char p=0;
	char id=0;

	for(i=0; i<VarCount; i++)
	{
		if(p<Var[i].Priority)
		{
			p=Var[i].Priority;
			id=i;
		}
	}

	return id;
}

/////////////////////////////////////////////////////////////////////////////////////////
//����1��Ԫ��
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CDTExp::CalcOneVar(int ab, int al, int bb, int bl)//�������ν⣿
{
	//�����²�
	//ab�ڱ��ʽ�е�POSTIONλ�� al��ʾ���ȣ�
	//bb�������ͬ��
	//
	double a,b;
	CDTStr buf;

	buf.Assign(Expression.GetBuffer()+ab,al);//�õ����ʽ
	a=atof(buf.GetBuffer());//�õ�Ԫ��a
	
	buf.Assign(Expression.GetBuffer()+bb,bl);
	b=atof(buf.GetBuffer());//�õ�Ԫ��b
	//expression[a+b] Ԫ��a���������������磺ab+ac ab=0 al=2 
	switch(Expression.GetAt(ab+al))//���ʽa�����������������
	{
	case '+':
		buf.Format(EX_STR_F_FMT,a+b); //�ӷ�
		break;
	case '-':
		buf.Format(EX_STR_F_FMT,a-b); //����
		break;
	case '*':
		buf.Format(EX_STR_F_FMT,a*b); //�˷�
		break;
	case '/':
		if(b==0)	//�����������жϳ�������Ϊ0
		{				
			buf="0"; //ֱ����Ϊ0
		}else
		{
			buf.Format(EX_STR_F_FMT,a/b);
		}
		break;
	case '@':
		buf.Format(EX_STR_U_FMT,(UINT)a&(UINT)b);
		break;
	case '|':
		buf.Format(EX_STR_U_FMT,(UINT)a|(UINT)b);
		break;
	case '%':
		buf.Format(EX_STR_U_FMT,(UINT)a%(UINT)b);
		break;
	case '=':
		buf.Format(EX_STR_D_FMT,a==b);
		break;
	case ']':
		buf.Format(EX_STR_D_FMT,a>b);
		break;
	case '[':
		buf.Format(EX_STR_D_FMT,a<b);
		break;
	case '}':
		buf.Format(EX_STR_D_FMT,a>=b);
		break;
	case '{':
		buf.Format(EX_STR_D_FMT,a<=b);
		break;
	case 'N':
		buf.Format(EX_STR_D_FMT,a!=b);
		break;
	case 'W':	//&&
		buf.Format(EX_STR_D_FMT,a&&b);
		break;
	case 'R':
		buf.Format(EX_STR_D_FMT,a||b);
		break;
	case '!':
		buf.Format("%u",!b);
		break;
	case '~':
		if(b>0xffff)
		{
			unsigned long i;
			i=~((unsigned long)b);
			buf.Format("%lu",i);
		}else if(b>0xff){
			unsigned short i;
			i=~((unsigned short)b);
			buf.Format("%u",i);
		}else{
			unsigned char i;
			i=~((unsigned char)b);
			buf.Format("%u",i);
		}
		break;
	case 'Z':
		if(a>0xffff){
			buf.Format("%lu",(unsigned long)a<<(unsigned long)b);
		}else if(a>0xff){
			buf.Format("%u",(unsigned short)a<<(unsigned short)b);
		}else{
			buf.Format("%u",(unsigned char)a<<(unsigned char)b);
		}
		break;
	case 'Y':
		if(a>0xffff){
			buf.Format("%lu",(unsigned long)a>>(unsigned long)b);
		}else if(a>0xff){
			buf.Format("%u",(unsigned short)a>>(unsigned short)b);
		}else{
			buf.Format("%u",(unsigned char)a>>(unsigned char)b);
		}
		break;
	case '^':
		if(a>0xffff || b>0xffff){
			buf.Format("%lu",(unsigned long)a^(unsigned long)b);
		}else if(a>0xff || b>0xff){
			buf.Format("%u",(unsigned short)a^(unsigned short)b);
		}else{
			buf.Format("%u",(unsigned char)a^(unsigned char)b);
		}
		break;
	default:
		DMSG(GMsgProgrammer,("Error, Exp: %s, operator:%c, a=%d, b=%d\r\n",PSourceExpression, Expression.GetAt(ab+al),a,b));
		return FALSE;
		break;
	}


	//Expression.Remove(ab, (bb+bl)-ab);
	//Expression.Insert(ab, buf, buf.GetLength());
	Expression.Replace(ab,(bb+bl)-ab,buf.GetBuffer(),buf.GetLength());

	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////
//����������Ԫ��
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CDTExp::CalcVars()
{
	char hid;

	RemoveEmptyBracket();//ȥ���յ�����
	ParseExpression();//�������ʽ
	OrgVarCount=VarCount;
	hid=FindTopMostPriorityID();//�õ�������ȼ�����

//	DMSG(GMsgProgrammer,("CalcVars ==>> %s " ,Expression.GetBuffer()));

	while(VarCount > 1)
	{


		if(hid<VarCount-1)
		{
			//ѭ��������var�б���ı��ʽλ�á����ȼ����ȼ�����Ϣ
			if(!CalcOneVar(Var[hid].Begin,Var[hid].Length,Var[hid+1].Begin,Var[hid+1].Length)){
				return 0;
			}
		}else
		{
			DMSG(GMsgProgrammer,("Error, %s, VarCount:%d, hid:%d\r\n",Expression.GetBuffer(), VarCount,hid));
			return 0;
		}

		//DEBUG2("%s, %d\r\n",Expression, VarCount)
		//getchar();

		RemoveEmptyBracket();
		ParseExpression();
		hid=FindTopMostPriorityID();


		//DEBUG2("%s, %d<--Recheck \r\n",Expression, VarCount)
		//getchar();
	}//end of while

	return 1;
}



CDTStr & CDTExp::GetResult()//���յļ�����
{
	return Expression;
}

