/////////////////////////////////////////////////////////////////////////////
//Module: Standard C Language arithmetic expression Parser
//Platform: Win32, DOS, PureC/C++
//Authur: Deng.Ting ting@ode.cn
//Edition: 2007-11-05
//
//Modification:
//2008-02-12: Replace && symbol from A to W, the opertator must be a non-HEX char
//2007-11-05: Append bin const support
//2007-11-05: Append 'char' const support
//2007-09-07: Ported from PCMonster to Quasar
//2007-02-08: Support std-C format
//2007-02-08: Fixed 0 Divisor bug 
/////////////////////////////////////////////////////////////////////////////

#ifndef K_EXPRESSION_H
#define K_EXPRESSION_H

#include "dtstr.h"

#define MAX_EXPRESSION_LEN 82920

typedef struct{
	INT Begin;	//起始
	INT Length;	//长度
	INT Priority;//优先级
}DT_EXP_VAR_INFO;

#define MAX_VAR_COUNT		64


extern const char * EX_STR_U_FMT;
extern const char * EX_STR_D_FMT;
extern const char * EX_STR_F_FMT;
extern const char * EX_STR_G_FMT;


class CDTExp
{
public:
	CDTExp();
	virtual~CDTExp();
	
	double Calculate(LPCSTR expression);//计算函数
	CDTStr & GetResult();//返回结果

protected:
	
	CDTStr Expression;	//表达式
	LPCSTR PSourceExpression; //指向原表达式,用于出错时显示.

	DT_EXP_VAR_INFO Var[MAX_VAR_COUNT];
	INT VarCount;	//变量个数

	INT OrgVarCount;	//note original expression varcount when first parsing.

	BOOL IsOperator(char c);//是否为操作符
	INT GetPriority(char c);//优先级

	INT IsVarChar(int pos, char c);//c是否合法
	void RemoveEmptyBracket();//移除空白的括弧对
	BOOL CalcOneVar(int ab, int al, int bb, int bl);//计算一对表达式
	BOOL ParseExpression();//分析表达式
	BOOL FindTopMostPriorityID();//得到优先级最高的
	BOOL CalcVars();//计算所有变量

	void ReplaceHexConst();
	void ReplaceBinConst();
	void ReplaceCharConst();

	void CFmt2InternalFmt();

};

#endif