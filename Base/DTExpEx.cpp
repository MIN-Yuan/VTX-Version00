#include "stdafx.h"
#include "DTExpEx.h"
#include "DTXml.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//表达式计算
//核心为：递归

//const char * EX_STR_TOP		="TOP";
const char * EX_STR_SSTR	="SSTR";
const char * EX_STR_SNUM	="SNUM";
const char * EX_STR_INRNG	="INRNG";
const char * EX_STR_ABS		="ABS";
const char * EX_STR_FORMAT	="FORMAT";
const char * EX_STR_SPART	="SPART";
const char * EX_STR_SLEN	="SLEN";
const char * EX_STR_SMID	="SMID";
const char * EX_STR_SREP	="SREP";
const char * EX_STR_EMPTY	="";
const char * EX_STR_COMMA	=",";
const char * EX_STR_QUOTATION="\"";
const char * EX_STR_EMPTY_QUOTATION="\"\""; 
const char * EX_STR_0="0";
const char * EX_STR_1="1";
const char * EX_STR_REP_DBPERCENT="\x02";	//double %
const char * EX_STR_REP_QUOTATION="\x03";	//\"


// const char * EX_STR_SMAP	="SMap";
// extern const char * STR_FOLDER_ROOT;
// extern CDTStr GCurPath;
// extern char STR_LANGTAG[];

CDTExpEx::CDTExpEx()
{

}

CDTExpEx::~CDTExpEx()
{

}



PCHAR  CDTExpEx::CalculateEx(LPCSTR expression, BOOL & float0str1)
{
	if(strlen(expression) >= MAX_EXPRESSION_LEN-8){ //表达式长度
		DMSG_ERROR("Exp Too long");
		return 0;
	}

	Expression = expression;
	PSourceExpression = expression;

	//GRemoveExpUslessChar(Expression);	//去除空格等,在装载表达式时处理了,因为替换变量时,也载要去除空格
	ReplaceCharConst();

	CalcFunFormat();
	CalcFunAbs();
	CalcFunInRng();
	CalcFunSStr();
	CalcFunSNum();
	CalcFunSRep();
	CalcFunSMid();
	CalcFunSPart();
	CalcFunSLen();

	BOOL has_quotation_mark = FALSE; 
	if(Expression.Find(0,'\"',0)>=0)
	{
		has_quotation_mark=TRUE;
	}

	if(!has_quotation_mark)
	{
		CFmt2InternalFmt();
		ReplaceHexConst();
		//ReplaceBinConst();

		if(!CalcVars())
		{
			Expression=EX_STR_0;
		}
	}

	if(has_quotation_mark)
	{
		float0str1=1;	//str
	}else if(OrgVarCount<=1)//如果表达式变量个数<1
	{	
		if(Expression.IsFloat())
		{//float类型
			float0str1=0;
		}else
		{
			float0str1=1;
		}
	}else
	{
		float0str1 = 0;
	}

	//如果是字符串结果
	if(float0str1)
	{
		//还原表达式中的字符串中的\"引号
		//ReplaceInternal2CLang(Expression);
	}else
	{
		GRemoveFloatStrTailZero(Expression);
	}


	return Expression.GetBuffer();
}


// INRNG
// In Range
// 功能：返回一个整数，属于哪一个范围（可指几定的n个数范围）。
// 格式：INRNG（整数，范围1最小值，范围1最大值，范围2最小值，范围2最大值，……）
// 返回结果：整数、范围的ID号。如果返回为0，则表示不在任何一个范围中。
VOID CDTExpEx::CalcFunInRng()
{
	INT funb,fune;
	CDTStr funbody;

	CDTStr str;
	ULONG partcount;
	LONG main_exp_result;
	INT i;

	CDTExpEx expex;
	BOOL float0str1;

	int twinscount;
	LONG a[MAX_INRNG_TWINS];
	LONG b[MAX_INRNG_TWINS];	//不将rtnum转换成数字,因为后面还是要转成字符串..
	BOOL ismatched;

	while(GFindFunWithBody(Expression,EX_STR_INRNG,funb,fune,funbody))
	{

		partcount=GGetFunBodyPartCount(funbody);//得到，分割个数

		if(partcount%2==0)
		{ 
			//参数个数错误
			DMSG(GMsgProgrammer,("Error, %s() twins-para not be matched\r\n",EX_STR_INRNG));
			partcount--;
		}

		if(partcount<1)
		{ 
			//参数个数
			DMSG(GMsgProgrammer,("Error, %s() para < 1\r\n",EX_STR_INRNG));
			Expression.Replace(funb,fune-funb,EX_STR_0);
			continue;
		}

		//确保表达式个数在合法范围内
		if(partcount>1+MAX_INRNG_TWINS*2)
		{
			partcount=1+MAX_INRNG_TWINS*2;
		}


		//calc main exp reuslt计算主表达式结果
		GGetFunBodyPart(funbody, str, 0);
		expex.CalculateEx(str,float0str1);//计算表达式
		if(float0str1)
		{
			DMSG(GMsgProgrammer,("Error, %s() result of main exp is not a number\r\n",EX_STR_INRNG));		
			Expression.Replace(funb,fune-funb,EX_STR_0);
			continue;
		}
		main_exp_result=expex.GetResult().ToInteger(0,10);//转换为整数

		
		
		
		//Calc twins num-rtnum
		twinscount = (partcount-1) / 2;
		for(i=0; i<twinscount; i++)
		{
			//calc range a
			GGetFunBodyPart(funbody,str,1+i*2);
			expex.CalculateEx(str,float0str1);
			if(float0str1)
			{
				DMSG(GMsgProgrammer,("Error, %s() twins-para %d num is not a number\r\n",EX_STR_INRNG,i));
				a[i]=0;
			}else{
				a[i]=expex.GetResult().ToInteger(0,10);
			}

			
			//calc range b
			GGetFunBodyPart(funbody,str,1+i*2+1);
			expex.CalculateEx(str,float0str1);
			if(float0str1)
			{
				DMSG(GMsgProgrammer,("Error, %s() twins-para %d rtnum is not a number\r\n",EX_STR_INRNG,i));
				b[i]=0;
			}else{
				b[i]=expex.GetResult().ToInteger(0,10);
			}
		}


		//Findout matched num
		ismatched=FALSE;
		for(i=0; i<twinscount; i++)
		{

			//如果a大b小,则颠倒
			if(a[i]>b[i])
			{
				a[i]^=b[i];
				b[i]^=a[i];
				a[i]^=b[i];
			}

			if(main_exp_result>=a[i] && main_exp_result<=b[i]){
				str.Format("%d",i+1);
				Expression.Replace(funb,fune-funb,str);
				ismatched=TRUE;
				break;
			}
		}
		
		//Set Default value;
		if(ismatched==FALSE)
		{
			Expression.Replace(funb,fune-funb,EX_STR_0);
		}
		
	}//end of while
}

// ABS
// ABSolute value
// 功能：求绝对值。
// 格式：ABS(浮点数)
// 返回类型：DOUBLE
VOID CDTExpEx::CalcFunAbs()
{
	INT funb,fune;
	CDTStr funbody;

	ULONG partcount;

	CDTStr str;
	CDTExpEx expex;
	BOOL float0str1;

	while(GFindFunWithBody(Expression,EX_STR_ABS,funb,fune,funbody))
	{
		
		partcount=GGetFunBodyPartCount(funbody);

		if(partcount != 1)
		{
			DMSG(GMsgProgrammer,("Error, %s() para cnt must be 1\r\n",EX_STR_ABS));
			Expression.Replace(funb,fune-funb,EX_STR_0);
			continue;
		}

		//calc main exp reuslt
		expex.CalculateEx(funbody,float0str1);//计算
		if(float0str1)
		{
			DMSG(GMsgProgrammer,("Error, %s() result of main exp is not a number\r\n",EX_STR_ABS));		
			Expression.Replace(funb,fune-funb,EX_STR_0);
			continue;
		}
		str=expex.GetResult();
		if(str.GetAt(0)=='-')
		{
			str.Remove(0,1);
		}
		Expression.Replace(funb,fune-funb,str);
	}	
}


// SNUM
// Select Number
// 功能：通过一个数，选择另一个数。
// 格式：
// SNUM(浮点数，默认结果浮点数，匹配浮点数1，结果浮点数1，匹配浮点数2，结果浮点数2，……)
// 返回结果：DOUBLE

VOID CDTExpEx::CalcFunSNum()
{
	INT funb,fune;
	CDTStr funbody;

	CDTStr str;
	CDTStr defaultstr;
	ULONG partcount;
	LONG main_exp_result;
	INT i;

	CDTExpEx expex;
	BOOL float0str1;

	int twinscount;
	LONG nums[MAX_SNUM_TWINS];
	CDTStr strs[MAX_SNUM_TWINS];	//不将rtnum转换成数字,因为后面还是要转成字符串..
	BOOL ismatched;

	while(GFindFunWithBody(Expression,EX_STR_SNUM,funb,fune,funbody))
	{
		
		partcount=GGetFunBodyPartCount(funbody);//get body part count
		
		if(partcount%2){
			DMSG(GMsgProgrammer,("Error, %s() twins-para not be matched\r\n",EX_STR_SNUM));
			partcount--;
		}

		if(partcount<2){
			DMSG(GMsgProgrammer,("Error, %s() para < 2\r\n",EX_STR_SNUM));
			Expression.Replace(funb,fune-funb,EX_STR_0);
			continue;
		}
		
		if(partcount>2+MAX_SNUM_TWINS*2){
			partcount=2+MAX_SNUM_TWINS*2;
		}


		//calc main exp reuslt
		GGetFunBodyPart(funbody,str,0);
		expex.CalculateEx(str,float0str1);
		if(float0str1){
			DMSG(GMsgProgrammer,("Error, %s() result of main exp is not a number\r\n",EX_STR_SNUM));		
			Expression.Replace(funb,fune-funb,EX_STR_0);
			continue;
		}
		main_exp_result=expex.GetResult().ToInteger(0,10);



		//Note down default num
		GGetFunBodyPart(funbody,str,1);
		expex.CalculateEx(str,float0str1);
		if(float0str1){
			DMSG(GMsgProgrammer,("Error, %s() default value must be a numer\r\n",EX_STR_SNUM));		
			Expression.Replace(funb,fune-funb,EX_STR_0);
			continue;
		}
		defaultstr=expex.GetResult();
		
		
		
		//Calc twins num-rtnum
		twinscount=(partcount-2)/2;
		for(i=0; i<twinscount; i++)
		{
				
			//calc num
			GGetFunBodyPart(funbody,str,2+i*2);
			expex.CalculateEx(str,float0str1);
			if(float0str1){
				DMSG(GMsgProgrammer,("Error, %s() twins-para %d num is not a number\r\n",EX_STR_SNUM,i));
				nums[i]=0;
			}else{
				nums[i]=expex.GetResult().ToInteger(0,10);
			}

			
			//calc rtnum
			GGetFunBodyPart(funbody,str,2+i*2+1);
			expex.CalculateEx(str,float0str1);
			if(float0str1){
				DMSG(GMsgProgrammer,("Error, %s() twins-para %d rtnum is not a number\r\n",EX_STR_SNUM,i));
				strs[i]=EX_STR_0;
			}else{
				strs[i]=expex.GetResult();
			}
		}


		//Findout matched num
		ismatched=FALSE;
		for(i=0; i<twinscount; i++){
			if(main_exp_result==nums[i]){
				Expression.Replace(funb,fune-funb,strs[i]);
				ismatched=TRUE;
				break;
			}
		}
		
		//Set Default value;
		if(ismatched==FALSE){
			Expression.Replace(funb,fune-funb,defaultstr);
		}
		
	}
}

// SMID
// 功能：返回字符串中的子字符串。
// 格式SMID(str,begin,len)
// 返回：成功返回子字符串，失败返回空字符串
// str：原字符串
// begin：起始位置。（负数表示倒数）
// len：取字符串的长度。
//SMid(str,begin,len)
VOID CDTExpEx::CalcFunSMid()
{
	INT funb,fune;
	CDTStr funbody;
	CDTStr fmt;

	
	CDTStr str;
	INT begin;
	INT len;


	CDTStr tempstr;

	CDTExpEx expex;
	BOOL float0str1;


	while(GFindFunWithBody(Expression,EX_STR_SMID,funb,fune,funbody))
	{
		if(GGetFunBodyPartCount(funbody)!=3)
		{
			DMSG(GMsgConfiger,("Error, %s() para\r\n",EX_STR_SMID));
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;
		}

		//Get str
		GGetFunBodyPart(funbody,tempstr,0);
		expex.CalculateEx(tempstr,float0str1);
		if(float0str1==0)
		{
			DMSG(GMsgProgrammer,("Error, %s() str\r\n",EX_STR_SMID));		
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;
		}
		str=expex.GetResult();
		GRemoveStrStrQuotationMark(str);
		if(str.GetLength()<=0){
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;
		}

	
		//Get begin
		GGetFunBodyPart(funbody,tempstr,1);
		expex.CalculateEx(tempstr,float0str1);
		if(float0str1){
			DMSG(GMsgProgrammer,("Error, %s() len\r\n",EX_STR_SMID));		
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;
		}
		begin=expex.GetResult().ToInteger(0,10);
		while(begin<0){
			begin+=str.GetLength();
		}
		if(begin>=str.GetLength()){
			DMSG(GMsgProgrammer,("Error, %s() begin out upper\r\n",EX_STR_SMID));		
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;			
		}



		//Get len
		GGetFunBodyPart(funbody,tempstr,2);
		expex.CalculateEx(tempstr,float0str1);
		if(float0str1){
			DMSG(GMsgProgrammer,("Error, %s() len\r\n",EX_STR_SMID));		
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;
		}
		len=expex.GetResult().ToInteger(0,10);
		if(begin+len>str.GetLength()){
			len=str.GetLength()-begin;
		}


		GReplaceCLangConstString2String(str);	
		tempstr=str.Mid(begin,len);
		GReplaceString2CLangConstString(tempstr);
		GAddStrStrQuotationMark(tempstr);

		Expression.Replace(funb,fune-funb,tempstr.GetBuffer(),tempstr.GetLength());
	}
}

//SLen(str)
// SLEN
// 功能：获到字符串的长度。
// 格式：SLEN(字符串)
// 返回：整数
VOID CDTExpEx::CalcFunSLen()
{
	INT funb,fune;
	CDTStr funbody;
	
	CDTStr str;
	
	CDTStr tempstr;	
	CDTExpEx expex;
	BOOL float0str1;
	
	while(GFindFunWithBody(Expression,EX_STR_SLEN,funb,fune,funbody))
	{
		
		if(GGetFunBodyPartCount(funbody)!=1)
		{
			DMSG(GMsgConfiger,("Error, %s() para\r\n",EX_STR_SLEN));
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;
		}
		
		//Get str
		GGetFunBodyPart(funbody,tempstr,0);
		expex.CalculateEx(tempstr,float0str1);
		if(float0str1==0) //非字符串
		{
			DMSG(GMsgProgrammer,("Error, %s() str\r\n",EX_STR_SLEN));		
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;
		}
		str=expex.GetResult();
		
		GRemoveStrStrQuotationMark(str);
		GReplaceCLangConstString2String(str);
		tempstr.Format("%d",str.GetLength());

		Expression.Replace(funb,fune-funb,tempstr.GetBuffer(),tempstr.GetLength());
	}
}
// SPART
// 功能：获取字符串中由特定“分隔字符串”分隔的子字符串。
//SPart(str,divider,id,defaultstr)
VOID CDTExpEx::CalcFunSPart()
{
	INT funb,fune;
	CDTStr funbody;
	
	CDTStr str;
	INT id;
	CDTStr divider;
	CDTStr defaultstr;
	
	CDTStr tempstr;	
	CDTExpEx expex;
	BOOL float0str1;

	while(GFindFunWithBody(Expression,EX_STR_SPART,funb,fune,funbody)){

		if(GGetFunBodyPartCount(funbody)!=4){
			DMSG(GMsgConfiger,("Error, %s() para\r\n",EX_STR_SPART));
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;
		}


		//Get defaultstr
		GGetFunBodyPart(funbody,tempstr,3);
		expex.CalculateEx(tempstr,float0str1);
		if(float0str1==0){
			DMSG(GMsgProgrammer,("Error, %s() defaultstr\r\n",EX_STR_SPART));		
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;
		}
		defaultstr=expex.GetResult();
		
		
		//Get str
		GGetFunBodyPart(funbody,tempstr,0);
		expex.CalculateEx(tempstr,float0str1);
		if(float0str1==0){
			DMSG(GMsgProgrammer,("Error, %s() str\r\n",EX_STR_SPART));
			Expression.Replace(funb,fune-funb,defaultstr.GetBuffer(),defaultstr.GetLength());
			continue;
		}
		str=expex.GetResult();


		//Get divider
		GGetFunBodyPart(funbody,tempstr,1);
		expex.CalculateEx(tempstr,float0str1);
		if(float0str1==0){
			DMSG(GMsgProgrammer,("Error, %s() divider\r\n",EX_STR_SPART));
			Expression.Replace(funb,fune-funb,defaultstr.GetBuffer(),defaultstr.GetLength());
			continue;
		}
		divider=expex.GetResult();
	
		
		//Get ID
		GGetFunBodyPart(funbody,tempstr,2);
		expex.CalculateEx(tempstr,float0str1);
		if(float0str1){
			DMSG(GMsgProgrammer,("Error, %s() id\r\n",EX_STR_SPART));		
			Expression.Replace(funb,fune-funb,defaultstr.GetBuffer(),defaultstr.GetLength());
			continue;
		}
		id=expex.GetResult().ToInteger(0,10);
		

		//查是否ID越界	
		GRemoveStrStrQuotationMark(str);
		GReplaceCLangConstString2String(str);

		GRemoveStrStrQuotationMark(divider);
		GReplaceCLangConstString2String(divider);
		if(id<0 || id>=(INT)str.GetDividedStringCount(divider)){
			DMSG(GMsgProgrammer,("Warning, %s() id\r\n",EX_STR_SPART));		
			Expression.Replace(funb,fune-funb,defaultstr.GetBuffer(),defaultstr.GetLength());
			continue;
		}


		//找到子串
		tempstr=str.GetDividedString(id,divider);
		GReplaceString2CLangConstString(tempstr);
		GAddStrStrQuotationMark(tempstr);

		Expression.Replace(funb,fune-funb,tempstr.GetBuffer(),tempstr.GetLength());
	}
}

// SREP
// 功能：用“新的字符”串替换“原字符串”中的指定段。可实现“插入”、“删除”、“替换”。
// 格式：SREP(str,begin,len,newstr)
// 返回：成功返回已被替换的字符串，失败返回空字符串
// str：原字符串
// begin：str中要被替换的段，的起始位置，（负数表示倒数）
// len：str中要被替换的段的长度（字节数）。
// newstr：新的（替代品）字符串。
//SRep(str,begin,len,newstr)
VOID CDTExpEx::CalcFunSRep()
{
	INT funb,fune;
	CDTStr funbody;
	//CDTStr fmt;
	
	CDTStr str;
	INT begin;
	INT len;
	CDTStr newstr;
	
	CDTStr tempstr;

	CDTExpEx expex;
	BOOL float0str1;

	while(GFindFunWithBody(Expression,EX_STR_SREP,funb,fune,funbody))
	{
		if(GGetFunBodyPartCount(funbody)!=4)
		{
			DMSG(GMsgConfiger,("Error, %s() para\r\n",EX_STR_SREP));
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;
		}
		

		//Get str
		GGetFunBodyPart(funbody,tempstr,0);
		expex.CalculateEx(tempstr,float0str1);
		if(float0str1==0)
		{
			DMSG(GMsgProgrammer,("Error, %s() str\r\n",EX_STR_SREP));		
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;
		}
		str=expex.GetResult();
		GRemoveStrStrQuotationMark(str);

		
		
		//Get begin
		GGetFunBodyPart(funbody,tempstr,1);
		expex.CalculateEx(tempstr,float0str1);
		if(float0str1)
		{
			DMSG(GMsgProgrammer,("Error, %s() len\r\n",EX_STR_SREP));		
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;
		}
		begin=expex.GetResult().ToInteger(0,10);
		while(begin<0)
		{
			begin+=str.GetLength();
		}
		if(begin>=str.GetLength())
		{
			DMSG(GMsgProgrammer,("Error, %s() begin out upper\r\n",EX_STR_SREP));		
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;			
		}
		
		
		
		//Get len
		GGetFunBodyPart(funbody,tempstr,2);
		expex.CalculateEx(tempstr,float0str1);
		if(float0str1)
		{
			DMSG(GMsgProgrammer,("Error, %s() len\r\n",EX_STR_SREP));		
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;
		}
		len=expex.GetResult().ToInteger(0,10);
		if(begin+len>str.GetLength())
		{
			len=str.GetLength()-begin;
		}


		//Get newstr
		GGetFunBodyPart(funbody,tempstr,3);
		expex.CalculateEx(tempstr,float0str1);
		if(float0str1==0)
		{
			DMSG(GMsgProgrammer,("Error, %s() newstr\r\n",EX_STR_SREP));		
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;
		}
		newstr=expex.GetResult();
		GRemoveStrStrQuotationMark(newstr);

	
		GReplaceCLangConstString2String(str);
		GReplaceCLangConstString2String(newstr);
		str.Replace(begin,len,newstr.GetBuffer(),newstr.GetLength());
		GReplaceString2CLangConstString(str);
		GAddStrStrQuotationMark(str);
		//替换
		Expression.Replace(funb,fune-funb,str.GetBuffer(),str.GetLength());
	}
}




VOID CDTExpEx::CalcFunFormat()
{
	INT funb,fune;
	CDTStr funbody;
	CDTStr fmt;

	INT i;
	CDTStr fmt_front_part;
	INT paracnt;
	CDTStr varname;
	
	CDTStr str;
	CDTStr var;
	INT offset;
	INT len;
	INT end;
	CDTStr fmtpart;
	INT type;
	CDTStr outpart;


	CDTExpEx expex;
	BOOL float0str1;


	while(GFindFunWithBody(Expression,EX_STR_FORMAT,funb,fune,funbody))
	{
		if(GGetFunBodyPartCount(funbody)<1)
		{
			DMSG(GMsgConfiger,("Error, %s() para < 1 \r\n",EX_STR_FORMAT));
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;
		}

		if(GGetFunBodyPartCount(funbody)>1+MAX_FORMAT_VAR)
		{
			DMSG(GMsgConfiger,("Error, %s() too mant para\r\n",EX_STR_FORMAT));
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;
		}

		//Get Fmt String
		GGetFunBodyPart(funbody,str,0);
		str.ReplaceAll("%%",EX_STR_REP_DBPERCENT);

		//Removed it already be replaced on GReplaceExpConstStrSpecialCharToInterFormat()
		//str.ReplaceAllI("\\R","\r");
		//str.ReplaceAllI("\\N","\r");

		expex.CalculateEx(str,float0str1);
		if(float0str1==0)
		{
			DMSG(GMsgConfiger,("Error, %s() format string error\r\n",EX_STR_FORMAT));
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;
		}
		fmt=expex.GetResult();


		paracnt=GGetFunBodyPartCount(funbody)-1;


		//如果参数为0
		if(paracnt<=0)
		{
			fmt.ReplaceAll(EX_STR_REP_DBPERCENT,"%");
			Expression.Replace(funb,fune-funb,fmt);
			continue;
		}


		//检查最后一个%,para与%数量是否匹配
		if(paracnt>0)
		{
			//Find the last one '%', for check match paracnt.
			if(fmt.Find(0,'%',paracnt-1)<0)
			{
				DMSG(GMsgConfiger,("Error, %s() format %% & para cnt not matched %d\r\n",EX_STR_FORMAT ,paracnt));
				Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
				continue;
			}
		}
	

		for(i=0; i<paracnt; i++)
		{

			//calc var
			GGetFunBodyPart(funbody,str,1+i);
			expex.CalculateEx(str,float0str1);
			var=expex.GetResult();
			if(float0str1)
			{
				GRemoveStrStrQuotationMark(var);
			}


			if(i==0)
			{	//when first one '%xxx" part, note down the fmt_front_part incidentally
				offset=fmt.Find(0,'%',i);

				end=fmt.Find(offset+1,'%',0);
				if(end<0)
				{
					end=fmt.GetLength();
				}
				len=end-offset;
					
				//顺便记下第一个%号前东东
				fmt_front_part.Assign(fmt.GetBuffer(),offset);

			}else
			{//middle...
				offset=fmt.Find(0,'%',i);
	
				end=fmt.Find(offset+1,'%',0);
				if(end<0){
					end=fmt.GetLength();	
				}
				len=end-offset;
			}


			//Get data-type
			fmtpart.Assign(fmt.GetBuffer()+offset,len);
			type=FormatPartToType(fmtpart);
			
			
			if(type==FORMAT_STRING){
				outpart.Format(fmtpart,var.GetBuffer());
			}else if(type==FORMAT_DOUBLE){
				outpart.Format(fmtpart,(float)var.ToDouble(0));
			}else{
				outpart.Format(fmtpart,var.ToInteger(0,10));	
			} 
			
			fmt_front_part+=outpart;
		}

		//GRemoveStrStrQuotationMark(fmt_front_part);
		fmt_front_part.ReplaceAll(EX_STR_REP_DBPERCENT,"%");
		Expression.Replace(funb,fune-funb,fmt_front_part);

	//	DMSG(GMsgProgrammer,("CalcVars ==>> %s  " ,Expression.GetBuffer()));

	
	}
}

INT CDTExpEx::FormatPartToType(CDTStr & str)
{
	//Find out first char behind the '%', it signs data-type
	char c;
	int pos=1;
	int type;
	
	do{
		c=str.GetAt(pos);
		
		//非数字或点
		if(!
			((c>='0' && c<='9') || c=='.')
			)
		{
			break;
		}
		pos++;
	}while(c!=0);
	
	switch(c){
	case 'l': case 'L':
	case 'u': case 'U':
	case 'd': case 'D': default:
	case 'x': case 'X':
		type=FORMAT_INTEGER;
		break;
	case 'f': case 'F':
	case 'g': case 'G':	
		type=FORMAT_DOUBLE;
		break;
	case 's': case 'S':
		type=FORMAT_STRING;
		break;
	}
	
	return type;
}

// SSTR
// Select String
// 功能：通过一个数，选择一个字符串。
// SSTR（浮点数，默认结果字符串，匹配浮点数1，结果字符串1，匹配浮点数2，结果字符串2，……）
// 示例：当X+1的结果为1时返回字符串“ONE”，为2时返回“TWO”，为3时返回“THREE”，如果不为1、2、3中的任何一个，则返回“None”。
// 返回类型：STRING 
VOID CDTExpEx::CalcFunSStr()
{
	INT funb,fune;//函数的起始和结束位置
	CDTStr funbody;//函数名？

	CDTStr str;
	CDTStr defaultstr;
	ULONG partcount;
	LONG main_exp_result;
	INT i;

	CDTExpEx expex;
	BOOL float0str1;
	
	int twinscount;
	LONG nums[MAX_SSTR_TWINS]; //记录浮点数
	CDTStr strs[MAX_SSTR_TWINS];//记录字符串
	BOOL ismatched;
		
	while(GFindFunWithBody(Expression,EX_STR_SSTR,funb,fune,funbody))
	{

		partcount=GGetFunBodyPartCount(funbody);

		if(partcount%2)
		{
			DMSG(GMsgProgrammer,("Error, %s() twins-para not be matched\r\n",EX_STR_SSTR));
			partcount--;
		}

		if(partcount<2)
		{
			DMSG(GMsgProgrammer,("Error, %s() para < 2\r\n",EX_STR_SSTR));
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;
		}

		if(partcount>2+MAX_SSTR_TWINS*2)
		{
			partcount=2+MAX_SSTR_TWINS*2;
		}


		//Note down default str
		GGetFunBodyPart(funbody,str,1);
		expex.CalculateEx(str,float0str1);
		if(float0str1==0)
		{
			DMSG(GMsgProgrammer,("Error, %s() default value must be a string\r\n",EX_STR_SSTR));		
			Expression.Replace(funb,fune-funb,EX_STR_EMPTY_QUOTATION);
			continue;
		}
		defaultstr=expex.GetResult();


		//calc main exp reuslt
		GGetFunBodyPart(funbody,str,0);
		expex.CalculateEx(str,float0str1);
		if(float0str1)
		{
			DMSG(GMsgProgrammer,("Error, %s() result of main exp is not a number\r\n",EX_STR_SSTR));		
			Expression.Replace(funb,fune-funb,defaultstr,defaultstr.GetLength());
			continue;
		}
		main_exp_result=expex.GetResult().ToInteger(0,10);



		//Calc twins num-str
		twinscount=(partcount-2)/2;
		for(i=0; i<twinscount; i++)
		{


			//calc num
			GGetFunBodyPart(funbody,str,2+i*2);
			expex.CalculateEx(str,float0str1);
			if(float0str1)
			{
				DMSG(GMsgProgrammer,("Error, %s() twins-para %d num is not a number\r\n",EX_STR_SSTR,i));
				nums[i]=0;
			}else
			{
				nums[i]=expex.GetResult().ToInteger(0,10);
			}

			//calc str
			GGetFunBodyPart(funbody,str,2+i*2+1);
			expex.CalculateEx(str,float0str1);
			if(float0str1==0)
			{
				DMSG(GMsgProgrammer,("Error, %s() twins-para %d rtstr is not a string\r\n",EX_STR_SSTR,i));
				strs[i]=EX_STR_EMPTY_QUOTATION;
			}else
			{
				strs[i]=expex.GetResult();
			}
		}


		//Findout matched num
		ismatched=FALSE;
		for(i=0; i<twinscount; i++)
		{
			if(main_exp_result==nums[i])
			{
				Expression.Replace(funb,fune-funb,strs[i]);
				ismatched=TRUE;
				break;
			}
		}

		//Set Default value;
		if(ismatched==FALSE){
			Expression.Replace(funb,fune-funb,defaultstr);
		}
	}//end of while
}

BOOL GRemoveExpUslessChar(CDTStr & expstr)//删除无用的字符
{
	int len;
	int i;

	len=expstr.GetLength();
	if(len<=0){
		return FALSE;
	}
	PCHAR p2=expstr.GetBuffer();

		
	//复制一份
	CDTStr acopy;
	acopy=expstr;
	//先替换成替代符
	PCHAR p=acopy.GetBuffer();
	for(i=0; i<len; i++){
		if(p[i]=='\\')
		{
			if(p[i+1]=='\\')
			{		//为什么\\也要替换呢,因为"hello\\\\\\\\\\""
				p[i]='\x01';		//替换成没用的字符
				p[i+1]='\x02';		//替换成没用的字符
			}
			else if(p[i+1]=='\"')
			{
				p[i]='\x01';		//替换成没用的字符
				p[i+1]='\x02';		//替换成没用的字符			
			}
		}
	}
	

	INT in_quotation=0;	
	for(i=0; i<len; i++){
		if(in_quotation){	
			if(p[i]=='\"'){
				in_quotation=0;
				continue;
			}
		}else{
			if(p[i]=='\"'){
				in_quotation=1;
				continue;
			}
			

			if(p[i]==' '){	//如果为空格,将其改为0x04(标记出来),后面一起用TRIM删除,率效高
				if(i>0 && i<len-1){
					if(p[i-1]=='\'' && p[i+1]=='\''){	//如果是 ' '字符常量,是不去除
						continue;
					}
				}
				p2[i]=0x04;
			}
		}
	}


	expstr.RemoveChars(1,0x04);

	return TRUE;
}

//得到参数个数，以“,”分割
INT GGetFunBodyPartCount(CDTStr & body)
{
	int len;
	int i;

	len=body.GetLength();

	if(len <= 0)
	{
		return 0;
	}

	//复制一份
	CDTStr acopy;
	acopy=body;
	//先替换成替代符
	PCHAR p=acopy.GetBuffer();
	for(i=0; i<len; i++)
	{
		if(p[i]=='\\')
		{
			if(p[i+1]=='\\')
			{		//为什么\\也要替换呢,因为"hello\\\\\\\\\\""
				p[i]='\x01';		//替换成没用的字符
				p[i+1]='\x02';		//替换成没用的字符
			}else if(p[i+1]=='\"')
			{
				p[i]='\x01';		//替换成没用的字符
				p[i+1]='\x02';		//替换成没用的字符			
			}
		}
	}


	INT in_bracket=0;
	INT in_quotation=0;
	int count=1;

	for(i=0; i<len; i++)
	{

		if(in_quotation)
		{	
			if(p[i]=='\"')
			{	// for "hello\\" or "hello\"abc"
				in_quotation=0;
				continue;
			}
		}else
		{
			if(p[i]=='\"')
			{
				in_quotation=1;
				continue;
			}

		
			if(p[i]=='(')
			{
				in_bracket++;//括号数量++
				continue;
			}else if(p[i]==')')
			{
				in_bracket--;
				continue;
			}


			if(in_bracket==0)
			{
				if(p[i]==',')
				{
					count++;
					continue;
				}
			}	
		}
	}

	return count;
}
//20080807猜测这个函数的作用是得到   body中由，分割的第id那部分
BOOL GGetFunBodyPart(CDTStr & body, CDTStr & part, INT id)
{
	int len;
	int i;
	
	len=body.GetLength();
	if(len<=0)
	{
		return FALSE;
	}

	if(id<0)
	{
		return FALSE;
	}

	//复制一份
	CDTStr acopy;
	acopy=body;
	//先替换成替代符
	PCHAR p=acopy.GetBuffer();
	for(i=0; i<len; i++)
	{
		if(p[i]=='\\')
		{
			if(p[i+1]=='\\')
			{		//为什么\\也要替换呢,因为"hello\\\\\\\\\\""
				p[i]='\x01';		//替换成没用的字符
				p[i+1]='\x02';		//替换成没用的字符
			}else if(p[i+1]=='\"')
			{
				p[i]='\x01';		//替换成没用的字符
				p[i+1]='\x02';		//替换成没用的字符			
			}
		}
	}

	
	INT in_bracket=0;
	INT in_quotation=0;
	INT cur_part=0;
	INT b=0;
	INT e=0;

	for(i=0; i<len; i++)
	{
		if(in_quotation)
		{	
			if(p[i]=='\"')
			{
				in_quotation=0;
				continue;
			}
		}else
		{
			if(p[i]=='\"')
			{
				in_quotation=1;
				continue;
			}

		
			if(p[i]=='(')
			{
				in_bracket++;
				continue;
			}else if(p[i]==')')
			{
				in_bracket--;
				continue;
			}

			
			if(in_bracket==0)
			{
				if(p[i]==',')
				{
					if(cur_part>=id)
					{
						e=i;
						break;
					}
					b=i+1;
					cur_part++;
				}
			}
		}
	}


	if(cur_part!=id)
	{
		return FALSE;
	}
	
	//the last part
	if(e<=b)
	{
		e=body.GetLength();
	}

	part.Assign(body.GetBuffer()+b,e-b);

	return TRUE;
}

/*20080803：比如有这个一个123SLEN(....),其中SLEN(....)为表达式
body为表达式里面内容的开始地址，funb为字符S的位置值的指针，fune为)的位置值的指针。
*/
BOOL GFindFunWithBody(CDTStr & text, const char * funname, INT & funb, int & fune, CDTStr & body)
{
	INT i;	
	INT in_bracket; //括弧
	INT in_quotation;
	INT bodyb,bodye;//body 起始和body end
	CDTStr str;

	int textlen=text.GetLength();
	if(textlen <= 0)
	{
		return FALSE;
	}

	int funnamelen=strlen(funname); //函数的长度
	if(funnamelen<=0)
	{
		return FALSE;
	}

	//复制一份
	CDTStr acopy;
	acopy=text;
	//先替换成替代符
	PCHAR p=acopy.GetBuffer();
	for(i=0; i<textlen; i++)
	{
		if(p[i]=='\\')
		{
			if(p[i+1]=='\\')
			{		//为什么\\也要替换呢,因为"hello\\\\\\\\\\""
				p[i]='\x01';		//替换成没用的字符
				p[i+1]='\x02';		//替换成没用的字符
			}else if(p[i+1]=='\"')
			{
				p[i]='\x01';		//替换成没用的字符
				p[i+1]='\x02';		//替换成没用的字符			
			}
		}
	}


	//find funname(
	//查找函数名称
	str=funname;
	str+="(";
	in_quotation=0;
	funb=-1;
	//查找引号 "  
	for(i=0; i<textlen-funnamelen-1; i++)
	{

		if(in_quotation)
		{	
			if(p[i]=='\"')
			{
				in_quotation=0;
				continue;
			}
		}else
		{
			if(p[i]=='\"')
			{
				in_quotation=1;
				continue;
			}
//20080803：funb是表达式第一个字符被找到的位置，如SREP（。。。。。）fudb就是S字符被找到的位置		
			if(strncmp(p+i,str.GetBuffer(),str.GetLength())==0)
			{	//区分大小写快
				funb=i;
				break;
			}
		}
	}

//////////////////////////////////////////////////////////////////////////
	if(funb<0)
	{
		return FALSE;
	}
//20080803：bodyb的含义是从“功能表达式（”后开始里面内容的开始位置

	//find bodyb & bodye
	bodyb=funb+str.GetLength();
	in_bracket=1;
	bodye=-1;
	in_quotation=0;

	for(i=bodyb; i<textlen; i++)
	{
		if(in_quotation)
		{	
			if(p[i]=='\"')
			{
				in_quotation=0;
				continue;
			}
		}else
		{
			if(p[i]=='\"')
			{
				in_quotation=1;
				continue;
			}

			if(p[i]=='(')
			{
				in_bracket++;
			}else if(p[i]==')')
			{
				in_bracket--;
			}

			if(in_bracket<=0)
			{	//found bodye.
				bodye=i;
				break;
			}
		}
	}
	
	if(bodye<bodyb)
	{
		return FALSE;
	}
	
	body.Assign(text.GetBuffer()+bodyb,bodye-bodyb);
	
	fune=bodye+1;//20080803：因为后面还有一个）号。
	
	return TRUE;
}

//添加引用标记
VOID GAddStrStrQuotationMark(CDTStr & str)
{
	if(str.GetAt(0)!='\"')
	{
		str.Insert(0,"\"",1);
		str+="\"";
	}
}

//删除字符串中的引用标记
VOID GRemoveStrStrQuotationMark(CDTStr & str)
{
	int len;
	
	len=str.GetLength();
	if(str.GetAt(0)=='\"')
	{
		str.KeepRight(len-1);
		len--;
	}

	if(str.GetAt(len-1)=='\"')
	{
		str.KeepLeft(len-1);
	}
}


// VOID CDTExpEx::ReplaceCLang2Internal(CDTStr & expstr)
// {
// 	int i;
// 	BOOL inquotation=FALSE;
// 	char * p;
// 	CDTStr tempstr;
// 
// 	for(i=0; i<expstr.GetLength(); i++){
// 		p=expstr.GetBuffer();
// 		if(p[i]=='\"'){
// 			inquotation=!inquotation;
// 		}else if(inquotation){
// 
// 			if(p[i]=='\\'){
// 				if(p[i+1]=='\"'){
// 					expstr.Replace(i,2,EX_STR_REP_QUOTATION,1);	//x03替代引号
// 					i+=1;
// 					continue;
// 				}/*else if(p[i+1]=='r'){
// 					expstr.Replace(i,2,"\r",1);
// 					continue;
// 				}else if(p[i+1]=='n'){
// 					expstr.Replace(i,2,"\n",1);
// 					continue;
// 				}else if(p[i+1]=='\\'){
// 					expstr.Replace(i,2,"\\",1);
// 					continue;
// 				}else if(p[i+1]=='x'){
// 					tempstr.Assign(expstr.GetBuffer()+i+2,2);
// 					expstr.Remove(i,3);
// 					expstr.GetBuffer()[i]=(char)tempstr.ToInteger(0,16);
// 				}*/
// 			}
// 		}
// 	}
// 
// // 
// }
// 
// VOID CDTExpEx::ReplaceInternal2CLang(CDTStr & expstr)
// {
// 	int i;
// 	BOOL inquotation=FALSE;
// 	char * p;
// 	CDTStr tempstr;
// 
// 	for(i=0; i<expstr.GetLength(); i++){
// 		p=expstr.GetBuffer();
// 		if(p[i]=='\"'){
// 			inquotation=!inquotation;
// 		}else if(inquotation){
// 			if(p[i]==EX_STR_REP_QUOTATION[0]){
// 				expstr.Replace(i,1,"\\\"");
// 				i+=1;	
// 			}
// 		}
// 	}
// 
// }

INT GRemoveFloatStrTailZero(CDTStr & str)
{
	if(str.Find(0,'.',0)<0)
	{
		return 0;
	}

	int len=str.GetLength();
	int i;
	char * p=str.GetBuffer();
	int rt_len=len;

	for(i=len-1; i>=0; i--)
	{
		if(p[i] != '0')
		{
			if(p[i]=='.')
			{
				rt_len--;
			}
			break;
		}
		rt_len--;
	}

	str.KeepLeft(rt_len);

	return len-rt_len;	
}


//替换 \" 为\\", "\t", "\\t"
VOID GReplaceString2CLangConstString(CDTStr & str)
{

	str.ReplaceAllI("\\","\\\\",2);
	str.ReplaceAllI("\"","\\\"",2);
	str.ReplaceAllI("\r","\\r",2);
	str.ReplaceAllI("\n","\\n",2);
	str.ReplaceAllI("\t","\\t",2);

}

//这函数名，真搞人！囧~
VOID GReplaceCLangConstString2String(CDTStr & str)
{
	int i;
	CDTStr tempstr;

	for(i=0; i<str.GetLength(); i++)
	{
		if(str.GetAt(i)=='\\')
		{
			if(str.GetAt(i+1)=='\"')
			{
				str.Replace(i,2,"\"",1);
				continue;
			}else if(str.GetAt(i+1)=='r')
			{
				str.Replace(i,2,"\r",1);
				continue;
			}else if(str.GetAt(i+1)=='n')
			{
				str.Replace(i,2,"\n",1);
				continue;
			}else if(str.GetAt(i+1)=='t')
			{
				str.Replace(i,2,"\t",1);
				continue;
			}else if(str.GetAt(i+1)=='\\')
			{
				str.Replace(i,2,"\\",1);
				continue;
			}else if(str.GetAt(i+1)=='x')
			{
				tempstr.Assign(str.GetBuffer()+i+2,2);
				str.Remove(i,3);
				str.GetBuffer()[i]=(char)tempstr.ToInteger(0,16);
			}
		}
	}
}