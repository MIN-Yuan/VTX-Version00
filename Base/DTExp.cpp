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
需要注意的是，在一个DEXXP类，对应于一个表达式
如：a+b

所有的操作，都是针对这个类中所记录的表达式进行
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



double CDTExp::Calculate(LPCSTR expression)  //计算
{
	//表达式长度过长
	if(strlen(expression) >= MAX_EXPRESSION_LEN-8)
	{
		return 0;
	}

	Expression = expression;//表达式
	PSourceExpression = expression;//源表达式

	Expression.RemoveChars(1,'\"');//去掉"号

	CFmt2InternalFmt(); //转换为内部格式
	ReplaceHexConst(); //替换16进制常型
	//ReplaceBinConst();
	ReplaceCharConst();

	if(!CalcVars()) //计算所有表达式组
	{
		return 0;
	}

	return atof(Expression.GetBuffer());
}

void CDTExp::CFmt2InternalFmt()//C格式转换为内部格式
{
	//Expression.Trim(4,' ','\'','\"',';');	

	//Expression.Trim(2,' ',';'); //去除字符串中的常量
	//具体x需要转换的字符，参考文档vedix.doc 种XML文件规范

	Expression.ReplaceAll("]]","Y",1);	//为了XML格式支持
	Expression.ReplaceAll("[[","Z",1);	//为了XML格式支持

	Expression.ReplaceAll("]=","}",1);	//为了XML格式支持	
	Expression.ReplaceAll("[=","{",1);	//为了XML格式支持
	
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

void CDTExp::ReplaceCharConst()//处理字符串常量
{
	CDTStr tempstr;
	BOOL inquotation=FALSE;

	INT i;
	char * p;
	for(i=0; i<Expression.GetLength()-1; i++)
	{
		p=Expression.GetBuffer();

		if(p[i]=='"')//找成对的"号
		{
			inquotation=!inquotation;
		}else
		{
			if(!inquotation) //
			{
				if(p[i]=='\'')
				{	//如果在非字符中找到'号
					if(p[i+1]=='\\' && p[i+3]=='\'')
					{	//特殊'\?'常量
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
						}else if(p[i+2]=='r') //替换为\r
						{
							tempstr.Format(EX_STR_D_FMT,'\r');
							Expression.Replace(i,4,tempstr.GetBuffer(),tempstr.GetLength());
							continue;
						}else if(p[i+2]=='n')
						{
							tempstr.Format(EX_STR_D_FMT,'\n');
							Expression.Replace(i,4,tempstr.GetBuffer(),tempstr.GetLength());
							continue;
						}else if(p[i+2]=='t')//替换为\t
						{
							tempstr.Format(EX_STR_D_FMT,'\t');
							Expression.Replace(i,4,tempstr.GetBuffer(),tempstr.GetLength());
							continue;
						}

					}else if(p[i+2]=='\'')
					{	//普通''常量
						tempstr.Format(EX_STR_D_FMT,Expression.GetAt(i+1));
						Expression.Replace(i,3,tempstr.GetBuffer(),tempstr.GetLength());
						continue;
					}
				}
			}
		}
	}


}


void CDTExp::ReplaceBinConst()//处理二进制常量
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

		//找出二进制数结尾
		for(i=b+2; i<e; i++)
		{
			c=Expression.GetAt(i);
			if(c!='0' && c!='1') //非0和1，不是二进制，结束
			{
				break;
			}
		}
		e=i; //结束位置

		if(e-b-2>32)//长度大于32
		{
			e=b+2+32;
		}

		//提取2进制数据部分
		str.Assign(Expression.GetBuffer()+b+2,e-b-2); 

		ul=strtoul(str.GetBuffer(),&endpos,2);//转换为 unsigned long 形式

		str.Format("%u",ul);

		//Expression.Remove(b,e-b);
		//Expression.Insert(b,str.GetBuffer(),str.GetLength());
		Expression.Replace(b,e-b,str.GetBuffer(),str.GetLength());//替换原来的表达式
	}

}


void CDTExp::ReplaceHexConst()//处理HEX常量,函数作用参考：ReplaceBinConst()
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

		//找出HEX常量串的数字字数
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
// 表达式中正确的操作符
//////////////////////////////////////////////////////////////////////////////////////
BOOL CDTExp::IsOperator(char c)
{
	if(    c=='+'
		|| c=='-' 
		|| c=='*' 
		|| c=='/'
		|| c=='@'	//位与
		|| c=='|'	//位或
		|| c=='%'	//取余
		|| c=='='
		|| c==']'
		|| c=='['
		|| c=='}'
		|| c=='{'	
		|| c=='('	
		|| c==')'	//括号
		|| c=='^'	//异或
		|| c=='N'	//!=
		|| c=='W'	//与
		|| c=='R'	//或
		|| c=='Z'	//左移
		|| c=='Y'	//右移
		|| c=='!'	//非
		|| c=='~'	//反
		){
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
//操作符的优先级
//最高级   12   ()[] →
//         11   ！~ ++ -- -(type) * & sizeof
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
//最低级
//////////////////////////////////////////////////////////////////////////////////////
int CDTExp::GetPriority(char c)//得到字符[操作符]的优先级
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
//是否为有效的元素字符
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
//去除空的()对.
/////////////////////////////////////////////////////////////////////////////////////////
void CDTExp::RemoveEmptyBracket()
{
	int i;

	//去除单元素外的括号对,如(元素)
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
//将表达式分解成元素,记录
//Begin[] 开始位置
//Len[] 长度
//Priority[] 优先级
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CDTExp::ParseExpression()
{
	int cur_pri=0;
	int i;
	int var_hold=0;
	int len;
	char c;

	VarCount=0;
	len=Expression.GetLength(); //表达式长度
	char * p=Expression.GetBuffer();

	for(i=0; i<len+1; i++)
	{
		c=p[i];
		if(IsVarChar(i,c))//有效的元素字符
		{
			if(!var_hold)
			{
				Var[VarCount].Begin=i;
				Var[VarCount].Priority=0;	//初始化
				VarCount++;					//变量数++
				if(VarCount > MAX_VAR_COUNT)
				{
					DMSG(GMsgProgrammer,("Error, Too many vars\r\n"));
				}
			}
			var_hold=1;
		}else//非有效的元素字符
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
// 				Var[VarCount].Priority=0;	//初始化
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
//找到最高优先级元素的ID
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
//计算1对元素
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CDTExp::CalcOneVar(int ab, int al, int bb, int bl)//参数作何解？
{
	//参数猜测
	//ab在表达式中的POSTION位置 al表示长度？
	//bb参数理解同上
	//
	double a,b;
	CDTStr buf;

	buf.Assign(Expression.GetBuffer()+ab,al);//得到表达式
	a=atof(buf.GetBuffer());//得到元素a
	
	buf.Assign(Expression.GetBuffer()+bb,bl);
	b=atof(buf.GetBuffer());//得到元素b
	//expression[a+b] 元素a后面的运算符。例如：ab+ac ab=0 al=2 
	switch(Expression.GetAt(ab+al))//表达式a，紧随其后的是运算符
	{
	case '+':
		buf.Format(EX_STR_F_FMT,a+b); //加法
		break;
	case '-':
		buf.Format(EX_STR_F_FMT,a-b); //减法
		break;
	case '*':
		buf.Format(EX_STR_F_FMT,a*b); //乘法
		break;
	case '/':
		if(b==0)	//除法，并且判断除数不能为0
		{				
			buf="0"; //直接设为0
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
//计算完所有元素
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CDTExp::CalcVars()
{
	char hid;

	RemoveEmptyBracket();//去掉空的括弧
	ParseExpression();//解析表达式
	OrgVarCount=VarCount;
	hid=FindTopMostPriorityID();//得到最高优先级索引

//	DMSG(GMsgProgrammer,("CalcVars ==>> %s " ,Expression.GetBuffer()));

	while(VarCount > 1)
	{


		if(hid<VarCount-1)
		{
			//循环，计算var中保存的表达式位置、长度及优先级等信息
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



CDTStr & CDTExp::GetResult()//最终的计算结果
{
	return Expression;
}

