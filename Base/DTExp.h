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
	INT Begin;	//��ʼ
	INT Length;	//����
	INT Priority;//���ȼ�
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
	
	double Calculate(LPCSTR expression);//���㺯��
	CDTStr & GetResult();//���ؽ��

protected:
	
	CDTStr Expression;	//���ʽ
	LPCSTR PSourceExpression; //ָ��ԭ���ʽ,���ڳ���ʱ��ʾ.

	DT_EXP_VAR_INFO Var[MAX_VAR_COUNT];
	INT VarCount;	//��������

	INT OrgVarCount;	//note original expression varcount when first parsing.

	BOOL IsOperator(char c);//�Ƿ�Ϊ������
	INT GetPriority(char c);//���ȼ�

	INT IsVarChar(int pos, char c);//c�Ƿ�Ϸ�
	void RemoveEmptyBracket();//�Ƴ��հ׵�������
	BOOL CalcOneVar(int ab, int al, int bb, int bl);//����һ�Ա��ʽ
	BOOL ParseExpression();//�������ʽ
	BOOL FindTopMostPriorityID();//�õ����ȼ���ߵ�
	BOOL CalcVars();//�������б���

	void ReplaceHexConst();
	void ReplaceBinConst();
	void ReplaceCharConst();

	void CFmt2InternalFmt();

};

#endif