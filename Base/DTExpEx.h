/////////////////////////////////////////////////////////////////////////////
//Module: Extend Standard C Language arithmetic expression Parser
//Platform: Win32, DOS, PureC/C++
//Authur: Deng.Ting
//Edition: 2007-11-19
//
//MODIFICATION:
//2007-11-19: InRng() abs() Format()
//2007-11-05: Modified to keep quotation mark while calc StringVar... SStr() Result will contain quotation mark
//2007-09-07: ported from PCMoster to Quasar
//2007-02-03: replaced ReplacePara() by CStr internal feature
/////////////////////////////////////////////////////////////////////////////

#ifndef H_EXPRESSIONEX_H
#define H_EXPRESSIONEX_H

#include "DTExp.h"

#define MAX_PARA_COUNT	8
#define MAX_SSTR_TWINS	128
#define MAX_SNUM_TWINS	128
#define MAX_INRNG_TWINS 128
#define MAX_FORMAT_VAR	128

#define FORMAT_INTEGER	0
#define FORMAT_STRING	1
#define FORMAT_DOUBLE	2

//extern const char * EX_STR_TOP;
extern const char * EX_STR_SSTR;
extern const char * EX_STR_SNUM;
extern const char * EX_STR_INRNG;
extern const char * EX_STR_ABS;
extern const char * EX_STR_FORMAT;
extern const char * EX_STR_SPART;
extern const char * EX_STR_SLEN;
extern const char * EX_STR_SMID;
extern const char * EX_STR_SREP;
extern const char * EX_STR_EMPTY;
extern const char * EX_STR_COMMA;
extern const char * EX_STR_QUOTATION;
extern const char * EX_STR_EMPTY_QUOTATION;
extern const char * EX_STR_0;
extern const char * EX_STR_1;
extern const char * EX_STR_REP_DBPERCENT;
extern const char * EX_STR_REP_QUOTATION;

//extern const char * EX_STR_SMAP;

class CDTExpEx : public CDTExp
{
public:
	CDTExpEx();
	virtual ~CDTExpEx();
	PCHAR CalculateEx(LPCSTR expression, BOOL & float0str1);

private:
	VOID CalcFunSStr();//����SSTR
	VOID CalcFunSNum();//����SNUM
	VOID CalcFunInRng();//����INRNG
	VOID CalcFunAbs();//����ABS
	VOID CalcFunFormat();//����FORMAT
	VOID CalcFunSMid();//����SMID
	VOID CalcFunSRep();//����SREP

	VOID CalcFunSPart();
	VOID CalcFunSLen();

	INT FormatPartToType(CDTStr & str);

	//VOID ReplaceCLang2Internal(CDTStr & expstr);
	//VOID ReplaceInternal2CLang(CDTStr & expstr);
};

BOOL GRemoveExpUslessChar(CDTStr & expstr);//ɾ����Ч���ַ�
//���ݺ������ֵõ�������λ�õ���Ϣ
BOOL GFindFunWithBody(CDTStr & text, const char * funname, INT & funb, int & fune, CDTStr & body);
INT	GGetFunBodyPartCount(CDTStr & body);//�õ������������ԡ�,���ָ�
INT	GGetFunBodyPart(CDTStr & body, CDTStr & part, INT id);//�õ��Ժ���","�ָ�Ĳ���

VOID GAddStrStrQuotationMark(CDTStr & str);
VOID GRemoveStrStrQuotationMark(CDTStr & str);
INT	GRemoveFloatStrTailZero(CDTStr & str);

VOID GReplaceString2CLangConstString(CDTStr & str);
VOID GReplaceCLangConstString2String(CDTStr & str);

#endif
