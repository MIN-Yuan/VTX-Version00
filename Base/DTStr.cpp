#include "stdafx.h"
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "DTStr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const char * DT_STR_EMPTY="";

CDTStr::CDTStr(const CDTStr& str)
{
	Buffer=0;
	Length=0;
	Assign(str.Buffer);	//Assin 中已更新长度
}

CDTStr::CDTStr(const char * str)
{
	Buffer=0;
	Length=0;
	Assign(str);	//Assin 中已更新长度
}

CDTStr::CDTStr(const int var)
{
	Buffer=0;
	Length=0;
	Format("%d",var);
}

CDTStr::CDTStr(const double var)
{
	Buffer=0;
	Length=0;
	Format("%f",var);
}

CDTStr::CDTStr()
{
	Buffer=0;
	Length=0;
}

CDTStr::~CDTStr()
{
	if(Buffer){
		delete []Buffer;
		Buffer=0;
	}

	Length=0;
}


BOOL CDTStr::AssignAndZeroTail(LONG count)
{
	if(Buffer){
		delete []Buffer;
		Buffer=0;
	}

	Buffer=new char[count+1];

	if(Buffer==0){
		return FALSE;
	}

	Buffer[count]=0;	//结尾加0;

	Length=count;
	return TRUE;
}


CDTStr & CDTStr::AssignPathPart(const char * fullpath)
{
	Assign(fullpath);

	long pos=FindRev(-1,'\\',0);

	if(pos<0){
		return *this;
	}

	KeepLeft(pos+1);

	return *this;
}

CDTStr & CDTStr::AssignFilePart(const char * fullpath)
{
	Assign(fullpath);

	long pos=FindRev(-1,'\\',0);
	
	if(pos<0){
		return *this;
	}
	
	KeepRight(Length-(pos+1));
	
	return *this;
}


CDTStr & CDTStr::operator =(const char * s)
{
	if(s==NULL){
		Clean();
		return *this;
	}
	
	Assign(s);	//Assin中已更新长度
	return *this;
}


CDTStr & CDTStr::operator =(const CDTStr & s)
{
	if(s.Buffer==NULL){
		Clean();
		return *this;
	}

	if(s.Length<=0){
		Clean();
		return *this;
	}
	Assign(s.Buffer,s.Length); //Assin中已更新长度

	return *this;
}

CHAR CDTStr::RemoveChars(INT n, ...)
{
	char * temp=0;
	INT i;
	int j;
	int c;
	char uselesschar;
	va_list pa;////20080807库文件查到typedef char *  va_list

	char chars[32];

	if(Buffer==0){
		return 0;
	}

	if(strlen(Buffer)<1){
		return 0;
	}

	if(n>32){
		return 0;
	}

	va_start(pa,n);
	for(c=0; c<n; c++){
		chars[c]=va_arg(pa,char);
	}
	va_end(pa);

	temp=new char[Length+1];
	if(temp==0){
		return 0;
	}

	//Copy to new buffer onebyone contains 0
	j=0;
	for(i=0; i<Length; i++){

		//whether is useless char.
		uselesschar=0;
		for(c=0; c<n; c++){
			if(Buffer[i]==chars[c]){
				uselesschar=1;
				break;
			}
		}
		if(uselesschar){
			continue;
		}

		temp[j]=Buffer[i];
		j++;
	}

	temp[j]=0;

	delete []Buffer;
	Buffer=temp;
	Length=j;

	return 1;
}



LONG CDTStr::GetLength()
{
	return Length;
}

CHAR CDTStr::GetAt(LONG pos)
{
	if(pos<0 || pos>=Length){
		return 0;
	}

	return Buffer[pos];
}

//VOID CDTStr::VarToString(const double var)
//{
//	CHAR str[32];
//	UINT len;
//
//	gcvt(var, 16, str);
//	
//	len=strlen(str);
//
//	if (str[len-1] == '.'){
//		str[len-1] = '\0';
//	}
//
//	Assign(str);
//}


VOID CDTStr::Clean()
{
	if(Buffer){
		delete []Buffer;
		Buffer=0;
	}
	Length=0;
}

//return the number of characters written 
INT CDTStr::Format(const char * format, ...)
{
	int buffersize=256;
	char * buffer=NULL;
	int result;

    va_list vaptr;

    va_start(vaptr,format);

	do{
		buffersize*=2;
		if(buffer){
			delete[]buffer;
			buffer=NULL;
		}
		buffer=new char[buffersize];
		if(buffer==NULL){
			return -1;
		}

	}while((result=_vsnprintf(buffer,buffersize,format,vaptr))<0);

    va_end(vaptr);

	Assign(buffer);

	if(buffer){
		delete buffer;
		buffer=NULL;
	}

	return result;
}

CDTStr& CDTStr::operator +=(CDTStr & str)
{
	if(str.GetBuffer()==0){
		return *this;
	}

	if(Buffer==0){
		Assign(str);
		return *this;
	}

	UINT len=Length+ str.GetLength();
	char * p=new char[len+1];

	strcpy(p,Buffer);
	strcat(p,str.Buffer);
	Clean();
	Buffer=p;
	Length=len;

	return *this;
}


//Numeric conversition helper fun
//CHAR CDTStr::NumericParse(void* pvar, char flag)
//{
//	char* pTmpStr = Buffer;
//
//	// remove the leading ' ' and '\t' at the beginning
//	while (*pTmpStr == ' ' || *pTmpStr == '\t')
//		pTmpStr++;
//
//	// no desired character found
//	if (strlen(pTmpStr) == 0)
//		return 0;
//
//	char a = pTmpStr[0];
//	if ((flag == 'b' || flag == 'C' || flag == 'S' || 
//		flag == 'I' || flag == 'L') && a == '-')
//		return 0;
//
//	if (flag == 'b')
//	{
//		char var;
//		if (strcmp(pTmpStr, "true") == 0 || strcmp(pTmpStr, "1") == 0 ||
//			strcmp(pTmpStr, "TRUE") == 0) var = 1;
//		else if (strcmp(pTmpStr, "false") == 0 || strcmp(pTmpStr, "0") == 0 ||
//			strcmp(pTmpStr, "FALSE") == 0) var = 0;
//		else // failed
//			return 0;
//		memcpy(pvar, &var, sizeof(char));
//		return 1;
//	}
//	else
//	{
//		double tmpvar = strtod(pTmpStr, (char**)&pTmpStr);
//		if (tmpvar == 0.0 && a != '0')
//			return 0;   // convertion wrong
//
//		if (flag == 'f' || flag == 'd')
//		{
//			// allow any float value with one 'f' or 'F' terminated
//			if (*pTmpStr == 'f' || *pTmpStr == 'F') 
//				pTmpStr++;
//		}
//		else if (flag == 'l' || flag == 'L')
//		{
//			// allow any float value with one 'l' or 'L terminated
//			if (*pTmpStr == 'l' || *pTmpStr == 'L') 
//				pTmpStr++;
//		}
//
//		switch(flag)
//		{
//		case 'c':
//			{
//				//if (tmpvar < -(0xff / 2 + 1) || tmpvar > 0xff / 2)
//				if (tmpvar < -128 || tmpvar > 127)
//					return 0;   // onerflow
//				char var = (char)tmpvar;
//				memcpy(pvar, &var, sizeof(char));
//			}
//			break;
//		case 's':
//			{
//				//if (tmpvar < -(0xffff / 2 + 1) || tmpvar > 0xffff / 2)
//				if (tmpvar < -32768.0 || tmpvar > 32768.0)
//					return 0;   // onerflow
//				short var = (short)tmpvar;
//				memcpy(pvar, &var, sizeof(short));
//			}
//			break;
//		case 'i':
//			{
//				//if (tmpvar < -(0xffffffff / 2 + 1) || tmpvar > 0xffffffff / 2)
//				if (tmpvar < -2147483648.0 || tmpvar > 2147483647.0)
//					return 0;   // onerflow
//				int var = (int)tmpvar;
//				memcpy(pvar, &var, sizeof(int));
//			}
//			break;
//		case 'l':
//			{
//				//if (tmpvar < -(0xffffffff / 2 + 1) || tmpvar > 0xffffffff / 2)
//				if (tmpvar < -2147483648.0 || tmpvar > 2147483647.0)
//					return 0;   // onerflow
//				long var = (long)tmpvar;
//				memcpy(pvar, &var, sizeof(long));
//
//			}
//			break;
//		case 'C':
//			{
//				//if (tmpvar < 0 || tmpvar > 0xff)
//				if (tmpvar < 0.0 || tmpvar > 255)
//					return 0;   // onerflow
//				unsigned char var = (unsigned char)tmpvar;
//				memcpy(pvar, &var, sizeof(unsigned char));
//			}
//			break;
//		case 'S':
//			{
//				//if (tmpvar < 0 || tmpvar > 0xffff)
//				if (tmpvar < 0.0 || tmpvar > 65535.0)
//					return 0;   // onerflow
//				unsigned short var = (unsigned short)tmpvar;
//				memcpy(pvar, &var, sizeof(unsigned short));
//			}
//			break;
//		case 'I':
//			{
//				//if (tmpvar < 0 || tmpvar > 0xffffffff)
//				if (tmpvar < 0.0 || tmpvar > 4294967295.0)
//					return 0;   // onerflow
//				unsigned int var = (unsigned int)tmpvar;
//				memcpy(pvar, &var, sizeof(unsigned int));
//			}
//			break;
//		case 'L':
//			{
//				//if (tmpvar < 0 || tmpvar > 0xffffffff)
//				if (tmpvar < 0.0 || tmpvar > 4294967295.0)
//					return 0;   // onerflow
//				unsigned long var = (unsigned long)tmpvar;
//				memcpy(pvar, &var, sizeof(unsigned long));
//			}
//			break;
//		case 'f':
//			{
//				if (tmpvar < -3.402823466e+38 || tmpvar > 3.402823466e+38)
//					return 0;   // onerflow
//				float var = (float)tmpvar;
//				memcpy(pvar, &var, sizeof(float));
//			}
//			break;
//		case 'd':
//			memcpy(pvar, &tmpvar, sizeof(double));
//			break;
//		}
//
//		// remove the leading ' ' and '\t' at the end
//		while (*pTmpStr == ' ' || *pTmpStr == '\t')
//			pTmpStr++;
//
//		if (*pTmpStr != '\0')
//			return 0;   // non digital character detected
//
//		return 1;
//	}
//}

char CDTStr::Match(char * wildcards, char * str)
{
	char yes = 1;

	//iterate and delete '?' and '*' one by one
	while(*wildcards != '\0' && yes && *str != '\0'){
		if (*wildcards == '?'){
			str ++;
		}else if(*wildcards == '*'){
			yes = Scan(wildcards, str);
			wildcards --;
		}else{
			yes = (*wildcards == *str);
			str ++;
		}
		wildcards ++;
	}

	while (*wildcards == '*' && yes){
		wildcards ++;
	}

	return yes && *str == '\0' && *wildcards == '\0';
}

// scan '?' and '*'
CHAR CDTStr::Scan(char*& wildcards, char*& str)
{
	// remove the '?' and '*'
	for(wildcards ++; *str != '\0' && (*wildcards == '?' || *wildcards == '*'); wildcards ++)
		if (*wildcards == '?') str ++;
	while ( *wildcards == '*') wildcards ++;
	
	// if str is empty and wildcards has more characters or,
	// wildcards is empty, return 
	if (*str == '\0' && *wildcards != '\0') return 0;
	if (*str == '\0' && *wildcards == '\0')	return 1; 
	// else search substring
	else
	{
		char* wdsCopy = wildcards;
		char* strCopy = str;
		char  Yes     = 1;
		do 
		{
			if (!Match(wildcards, str))	strCopy ++;
			wildcards = wdsCopy;
			str		  = strCopy;
			while ((*wildcards != *str) && (*str != '\0')) str ++;
			wdsCopy = wildcards;
			strCopy = str;
		}while ((*str != '\0') ? !Match(wildcards, str) : (Yes = 0) != 0);

		if (*str == '\0' && *wildcards == '\0')	return 1;

		return Yes;
	}
}

CDTStr & CDTStr::Assign(const char * s, LONG count)
{
	if(s==0){
		return *this;
	}
	
	if(s==Buffer){
		return *this;
	}

	if(count<0){
		count=strlen(s);
	}

	Clean();

	Buffer=new char [count+1];

	if(count>0){
		strncpy(Buffer, s, count);
	}

	Buffer[count]=0;
	Length=count;

	return *this;
}

BOOL CDTStr::Insert(LONG pos, const char * str, LONG count)
{
	PCHAR p=0;
	LONG len;

	if(str==NULL){
		return FALSE;
	}

	if(pos>Length){
		return FALSE;
	}

	len=strlen(str);
	if(count>len){
		count=len;
	}

	if(count<0){
		count=len;
	}
	
	if(Buffer==0){
		Assign(str,count);
		return TRUE;
	}

	p=new char[Length+count+1];

	strncpy(p,Buffer,pos);
	strncpy(p+pos,str,count);
	strncpy(p+pos+count,Buffer+pos,Length-pos);
	p[Length+count]=0;
	
	delete []Buffer;
	Buffer=p;
	Length=Length+count;

	return TRUE;
}

BOOL CDTStr::Remove(UINT pos, UINT count)
{
	PCHAR p=0;
	//UINT len;

	if(Buffer==0){
		return FALSE;
	}

	if(count>Length-pos){
		count=Length-pos;
	}

	p=new char[Length-count+1];

	strncpy(p,Buffer,pos);
	strncpy(p+pos,Buffer+pos+count,Length-pos-count);
	p[Length-count]=0;

	delete []Buffer;
	Buffer=p;
	Length=Length-count;

	return TRUE;


}

//Dengting appdend for format whit !?* FormatWithDivider
//CHAR CDTStr::FormatWithDivider(const char * format)
//{
//	UINT i;
//	UINT len_fmt, len_src;
//	UINT pos_src, pos_dest;
//	
//	PCHAR pdest=0;
//
//	len_fmt=strlen(format);
//	len_src=GetLength();
//
//	if(Buffer==0 || len_src<1){
//		return 0;
//	}
//
//	pdest=new char[len_src+len_fmt+1];
//	if(pdest==0){
//		DEBUG("Error FormatWildcard mem\r\n")
//		return 0;
//	}
//
//	pos_src=0;
//	pos_dest=0;
//	pdest[0]=0;
//	for(i=0; i<len_fmt; i++){
//		if(format[i]=='?'){
//			pdest[pos_dest]=Buffer[pos_src];
//			pos_dest++;
//			pdest[pos_dest]=0;
//			pos_src++;
//		}else if(format[i]=='*'){
//			strcat(pdest,Buffer+pos_src);
//			pos_dest+=len_src-pos_src;
//			pos_src+=len_src-pos_src;
//		}else{
//			pdest[pos_dest]=format[i];
//			pos_dest++;
//			pdest[pos_dest]=0;
//		}
//	}
//
//	delete []Buffer;
//	Buffer=pdest;
//
//	return 1;
//}

//20080803:id为自己假定要寻找的次数
LONG CDTStr::Find(LONG b, const char * p, LONG len, LONG id)
{
	LONG i;
	LONG pos=-1;
	INT found=-1;

	if(Length<1){
		return -1;
	}

	if(len<0){
		len=strlen(p);
	}
	
	if(b+len>Length){
		return -1;
	}

	for(i=b; i<=Length-len; i++){
		if(memcmp(Buffer+i,p,len)==0){
			pos=i;
			found++;

			if(found>=id){
				return pos;
			}
		}
	}

	return -1;
}


//Begin pos for reverse search:
//B: -1 means tail pos
LONG CDTStr::FindRev(LONG b, const char * p, LONG len, LONG id)
{
	int i;
	LONG pos=-1;
	INT found=-1;

	if(Length<1){
		return -1;
	}

	if(b>=Length){
		b=Length-1;
	}

	if(b<0){
		b=Length-1;
	}

	if(len<0){
		len=strlen(p);
	}

	for(i=b-len; i>=0; i--){
		if(memcmp(Buffer+i,p,len)==0){
			pos=i;
			found++;

			if(found>=(long)id){
				return pos;
			}
		}
	}

	return -1;
}


BOOL CDTStr::KeepMid(LONG pos, LONG count)
{
	PCHAR p=0;

	if(pos+count>Length){
		return FALSE;
	}

	if(count<0){
		count=0;
	}

	p=new char[count+1];
	if(p==0){
		return FALSE;
	}

	if(count>0){
		strncpy(p,Buffer+pos,count);
	}
	p[count]=0;

	delete []Buffer;
	Buffer=p;
	Length=count;

	return TRUE;
}

BOOL CDTStr::KeepLeft(LONG count)
{
	return KeepMid(0,count);
}

BOOL CDTStr::KeepRight(LONG count)
{
	LONG len;
	len=GetLength();

	return KeepMid(len-count,count);
}

CDTStr CDTStr::Mid(LONG pos, LONG count)
{
	if(pos+count>GetLength()){
		return *this;
	}

	CDTStr str=DT_STR_EMPTY;

	if(count<0){
		return str;
	}

	str.Assign(Buffer+pos,count);
	return str;
}

CDTStr CDTStr::Left(LONG count)
{
	if(count>GetLength()){
		return *this;
	}

	CDTStr str=DT_STR_EMPTY;

	if(count<0){
		return str;
	}

	str.Assign(Buffer,count);
	return str;
}

CDTStr CDTStr::Right(LONG count)
{
	if(count>GetLength()){
		return *this;
	}

	CDTStr str=DT_STR_EMPTY;

	if(count<0){
		return str;
	}

	str.Assign(Buffer+GetLength()-count);
	return str;
}

VOID CDTStr::ReplaceAll(char c, char replaceto)
{
	int i;

	for(i=0; i<Length; i++){
		if(Buffer[i]==c){
			Buffer[i]=replaceto;
		}
	}
}


BOOL CDTStr::Replace(LONG pos, LONG len, const char * replacestr, LONG replacelen)
{
	if(pos+len>Length){
		return FALSE;
	}

	if(replacelen<0){
		replacelen=strlen(replacestr);
	}

	CHAR * p=NULL;
	INT newlen=Length-len+replacelen;
	p=new char[newlen+1];
	
	if(p==NULL){
		return FALSE;
	}
	
	memcpy(p,Buffer,pos);
	memcpy(p+pos,replacestr,replacelen);
	memcpy(p+pos+replacelen,Buffer+pos+len,Length-pos-len);
	p[newlen]=0;
	
	delete []Buffer;
	Buffer=p;
	Length=newlen;
	
	return TRUE;
}

BOOL CDTStr::Replace(const char * str, const char * replacestr, LONG replacelen, UINT id)
{
	LONG pos;
	INT len;
	len=strlen(str);

	if(replacelen<0){
		replacelen=strlen(replacestr);
	}
	
	pos=Find(0,str,len,id);
	if(pos==-1){
		return FALSE;
	}

	CHAR * p=NULL;
	INT newlen=Length-len+replacelen;
	p=new char[newlen+1];

	if(p==NULL){
		return FALSE;
	}

	memcpy(p,Buffer,pos);
	memcpy(p+pos,replacestr,replacelen);
	memcpy(p+pos+replacelen,Buffer+pos+len,Length-pos-len);
	p[newlen]=0;

	delete []Buffer;
	Buffer=p;
	Length=newlen;
	
	return TRUE;
}
/////引用到的例子Expression.ReplaceAll("]]","Y",1);
//return replaced count
INT CDTStr::ReplaceAll(const char * str, const char * replacestr, LONG replacelen)
{
	INT count=0;
	
	LONG pos;
	INT len;
	INT b=0;

	len=strlen(str);

	if(len<=0){
		return 0;
	}
	
	if(replacelen<0){
		replacelen=strlen(replacestr);
	}
	
	while((pos=Find(b,str,len,0))!=-1 ){
		Replace(pos,len,replacestr,replacelen);		
		b=pos+replacelen;
		count++;
	}

	return count;
}

BOOL CDTStr::ReplaceI(const char * str, const char * replacestr, UINT replacelen, UINT id)
{
	LONG pos;
	INT len;
	len=strlen(str);
	
	if(replacelen<0){
		replacelen=strlen(replacestr);
	}
	
	pos=FindI(0,str,len,id);
	if(pos==-1){
		return FALSE;
	}
	
	CHAR * p=NULL;
	INT newlen=Length-len+replacelen;
	p=new char[newlen+1];
	
	if(p==NULL){
		return FALSE;
	}
	
	memcpy(p,Buffer,pos);
	memcpy(p+pos,replacestr,replacelen);
	memcpy(p+pos+replacelen,Buffer+pos+len,Length-pos-len);
	p[newlen]=0;
	
	delete []Buffer;
	Buffer=p;
	Length=newlen;
	
	return TRUE;
}

INT CDTStr::ReplaceAllI(const char * str, const char * replacestr, UINT replacelen)
{
	INT count=0;
	LONG pos;
	INT len;
	INT b=0;
	len=strlen(str);
	
	if(len<=0){
		return 0;
	}
	
	if(replacelen<0){
		replacelen=strlen(replacestr);
	}
	
	while( (pos=FindI(b,str,len,0))!=-1 ){
		Replace(pos,len,replacestr,replacelen);
		count++;		
		b=pos+replacelen;
	}

	return count;
}


CDTStr CDTStr::GetDividedString(ULONG id, const char * divider)
{
	int len;
	int b;
	int e;

	if(Length<=0){
		return DT_STR_EMPTY;
	}
	
	if(Buffer==NULL){
		return DT_STR_EMPTY;
	}

	len=strlen(divider);

	//find begin pos
	if(id>0){
		b=Find(0,divider,len,id-1);//20080803找到第id-1个分割符号所在的位置。
		if(b==-1){//20080803:这个条件是说字符串都全部看了一遍还没找到divider,
			      //此时因为用了find函数，位置已经到了字符串末尾，所以b=Length;
			b=Length;
		}else{    //20080803:找到开始匹配到divider字符串的位置为b，b+=len是为了说位置到了找到字符位置+它的长度。
			b+=len;
		}
	}else{
		b=0;
	}

	//find end pos
	e=Find(b,divider,len,0);
	if(e==-1){
		e=Length;
	}

	if(b>e){
		b=e;
	}
//20080803：Mid(b,e-b)的意思是将把缓冲区+b位置开始的e-b个长度的一个缓冲区取出来
	return Mid(b,e-b);
}
//20080803:查找分割符中出现的次数。
ULONG CDTStr::GetDividedStringCount(const char * divider)
{
	LONG i;
	int len;
	ULONG count=1;

	len=strlen(divider);

	if(Length<=0){
		return 0;
	}

	if(Buffer==NULL){
		return 0;
	}


	for(i=0; i<Length; i++){
		if(strncmp(Buffer+i, divider,len)==0){
			count++;
		}
	}

	return count;
}


//09.11.20 by fanze. 将strtol转为strtoul 原因是若strtol只能转出最大为0x7fffffff的数字
LONG CDTStr::ToInteger(INT begin, INT base)
{
	char * p;

	if(Buffer==NULL){
		return 0;
	}

	if(begin>=Length){
		return 0;
	}
	//20080808strtol（将字符串转换成长整型数）
	/* 将字符串a，b，c 分别采用10，2，16进制转换成数字
	  long int strtol(const char *nptr,char **endptr,int base);
	  strtol()会将参数nptr字符串根据参数base来转换成长整型数。
	  参数base范围从2至36，参数base代表采用的进制方式，如 base值
	  为10则采用10进制，若base值为16则采用16进制等。当base值为0时
	  则是采用10进制做转换，但遇到如'0x'前置字符则会使用 16进制做
	  转换。
简单意思解释就是把nptr这个字符串指针所指向的字符串看成10进制还是其他进制

*/


	return strtoul(Buffer+begin, &p, base);
}

ULONG CDTStr::ToUnsignedInteger(INT begin, INT base)
{
	char * p;
	
	if(Buffer==NULL){
		return 0;
	}
	
	if(begin>=Length){
		return 0;
	}
	
	return strtoul(Buffer+begin, &p, base);
}



DOUBLE CDTStr::ToDouble(INT begin)
{
	if(begin>=Length){
		return 0;
	}
	
	return atof(Buffer+begin);
}


//b: begin pos
LONG CDTStr::Find(LONG b, const CHAR c, LONG id)
{
	LONG i;
	LONG found=-1;

	if(b<0){
		b=0;
	}

	for(i=b; i<Length; i++){
		if(Buffer[i]==c){
			found++;
			if(found>=id){
				return i;
			}
		}
	}

	return -1;
}

//b: begin search pos -1 means from Length-1;
LONG CDTStr::FindRev(LONG  b, const CHAR c, LONG id)
{
	LONG i;
	LONG found=-1;

	if(b<0){
		b=Length-1;
	}

	if(b>=Length){
		b=Length-1;
	}

	for(i=b; i>=0; i--){
		if(Buffer[i]==c){
			found++;
			if(found>=id){
				return i;
			}
		}
	}

	return -1;
}

VOID CDTStr::MakeUpper()
{
	if(strlen(Buffer)>0)
	{	_strupr(Buffer);
	}
}

VOID CDTStr::MakeLower()
{
	if(strlen(Buffer))	{
		_strlwr(Buffer);
	}
}

//Find str ignore case
LONG CDTStr::FindI(LONG b, const char * p, LONG len, LONG id)
{
	LONG i;
	LONG pos=-1;
	INT found=-1;

	if(Length<1){
		return -1;
	}

	if(len<0){
		len=strlen(p);
	}

	if(b+len>Length){
		return -1;
	}

	for(i=b; i<=Length-len; i++){
		if(_strnicmp(Buffer+i,p,len)==0){
			pos=i;
			found++;
			
			if(found>=id){
				return pos;
			}
		}
	}

	return -1;
}

//Find str ignore case
LONG CDTStr::FindRevI(LONG b, const char * p, LONG len, LONG id)
{
	int i;
	LONG pos=-1;
	INT found=-1;
	
	if(Length<1){
		return -1;
	}
	
	if(b>=Length){
		b=Length-1;
	}
	
	if(b<0){
		b=Length-1;
	}
	
	if(len<0){
		len=strlen(p);
	}
	
	for(i=b-len; i>=0; i--){
		if(_strnicmp(Buffer+i,p,len)==0){
			pos=i;
			found++;
			
			if(found>=(long)id){
				return pos;
			}
		}
	}
	
	return -1;
}

BOOL CDTStr::UpdateLength()
{
	Length=strlen(Buffer);
	return TRUE;
}

BOOL CDTStr::ReleaseBuffer()
{
	int len=strlen(Buffer);
	
	char * p=NULL;

	p=new char[len+1];
	if(p==NULL){
		return FALSE;
	}

	strcpy(p,Buffer);

	delete []Buffer;
	Buffer=p;
	Length=len;

	return TRUE;
}


BOOL CDTStr::LikeN(const char * str, int n)
{
	if(Buffer==0){
		return FALSE;
	}
	
	if(str==0){
		return FALSE;
	}

	if(n<=0){
		return FALSE;
	}

	return strncmp(str,Buffer,n)==0;
}

BOOL CDTStr::LikeNI(const char * str, int n)
{
	if(Buffer==0){
		return FALSE;
	}

	if(str==0){
		return FALSE;
	}
	
	if(n<=0){
		return FALSE;
	}

	return _strnicmp(str,Buffer,n)==0;
}


BOOL CDTStr::Like(const char *str)
{
	if(Buffer==0){
		return FALSE;
	}

	if(str==0){
		return FALSE;
	}

	return strcmp(str, Buffer)==0;
}


BOOL CDTStr::LikeI(const char *str)
{
	if(Buffer==0){
		return FALSE;
	}

	if(str==0){
		return FALSE;
	}

	return _stricmp(str, Buffer)==0;
}


BOOL CDTStr::IsFloat()
{
	if(Length<0){
		return FALSE;
	}

	if(Buffer==NULL){
		return FALSE;
	}

	int i;
	BOOL isfloat=TRUE;

	for(i=0; i<Length; i++){
		if(!((Buffer[i]>='0' && Buffer[i]<='9') || Buffer[i]=='-' || Buffer[i]=='.' )){
			isfloat=FALSE;
			break;
		}
	}

	return isfloat;
}

//CDTBuffer & CDTBuffer::operator =(const CDTBuffer & buffer)

CDTStr::operator char * ()
{
	if(Buffer==NULL){
		AssignAndZeroTail(0);
	}
	return Buffer;
}

CDTStr::operator const char * ()
{
	if(Buffer==NULL){
		AssignAndZeroTail(0);
	}
	return Buffer;
}

char * CDTStr::GetBuffer()
{
	if(Buffer==NULL){
		AssignAndZeroTail(0);
	}
	return Buffer;
}

BOOL CDTStr::MatchWildcard(LPCSTR wildcard)
{
	if(wildcard==NULL){
		return FALSE;
	}

	if(Buffer==NULL){
		return FALSE;
	}

	return Match((char*)wildcard,(char*)Buffer);

	return FALSE;
}

BOOL CDTStr::MatchWildcardGroup(LPCSTR group, LPCSTR divider)
{
	if(group==NULL){
		return FALSE;
	}

	if(Buffer==NULL){
		return FALSE;
	}
	
	BOOL result=FALSE;

	CDTStr str;
	INT i;
	INT b=0;
	INT e=0;
	INT len=strlen(group);

	INT divlen=strlen(divider);

	for(i=0; i<len-divlen; i++){
		if(strncmp(group+i,divider,divlen)==0){

			e=i;
			if(e>b){
				str.Assign(group+b,e-b);
				result=Match(str.GetBuffer(),(char*)Buffer);
				if(result){
					break;
				}
			}

			b=i+divlen;
			e=i+divlen;
		}
	}


	if(!result){
		e=len;
		if(e>b){
			str.Assign(group+b,e-b);
			result=Match(str.GetBuffer(),(char*)Buffer);
		}
	}

	return result;
}

BOOL CDTStr::MatchWildcardI(LPCSTR wildcard)
{
	if(wildcard==NULL){
		return FALSE;
	}
	
	if(Buffer==NULL){
		return FALSE;
	}

	CDTStr str=Buffer;
	str.MakeUpper();

	CDTStr wld=wildcard;
	wld.MakeUpper();	


	return Match(wld.GetBuffer(),str.GetBuffer());
}

BOOL CDTStr::MatchWildcardGroupI(LPCSTR group, LPCSTR divider)
{
	if(group==NULL){
		return FALSE;
	}
	
	if(Buffer==NULL){
		return FALSE;
	}
	
	CDTStr str=Buffer;
	str.MakeUpper();
	
	CDTStr grp=group;
	grp.MakeUpper();	
	
	
	return str.MatchWildcardGroup(grp.GetBuffer(),divider);
}
