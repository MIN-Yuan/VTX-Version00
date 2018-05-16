 #include "stdafx.h"
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
//#include <stdio.h>
//#include <math.h>

#include "kstr.h"


CStr::CStr(const CStr& str)
{
	Buffer=0;
	Length=0;
	Assign(str.Buffer);	//Assin 中已更新长度
}

CStr::CStr(const char * str)
{
	Buffer=0;
	Length=0;
	Assign(str);	//Assin 中已更新长度
}

CStr::CStr(const double var)
{
	Buffer=0;
	Length=0;
	Format("%f",var);
}

CStr::CStr()
{
	Buffer=0;
	Length=0;
}

CStr::~CStr()
{
	if(Buffer){
		delete []Buffer;
		Buffer=0;
	}

	Length=0;
}

CStr & CStr::Assign(PCCHAR s)
{
	char * p=0;
	size_t len;

	if(s==0){
		return *this;
	}

	len=strlen(s);
	p=new char[len+1];
	if(p==0){
		return *this;
	}
	strcpy(p,s);

	if(Buffer){
		delete []Buffer;
	}

	Buffer=p;
	Length=len;
	
	return *this;
}


CStr & CStr::AssignFilePath(PCCHAR file)
{
	char * p=0;
	INT i;

	if(file==0){
		return *this;
	}

	UINT len=strlen(file);
	
	p=new char[len+1];
	
	if(p==0){
		return *this;
	}

	strcpy(p,file);
	for(i=len-1; i>=0; i--){
		if(p[i]=='\\'){
			p[i+1]=0;
			break;
		}
	}

	Assign(p);	//Assin中已更新长度
	
	delete []p;

	return *this;
}


CStr & CStr::operator =(PCCHAR s)
{
	Assign(s);	//Assin中已更新长度
	return *this;
}


CStr & CStr::operator =(const CStr & s)
{
	Assign(s.Buffer); //Assin中已更新长度
	return *this;
}

CHAR CStr::Trim(CHAR n, ...)
{
	char * temp=0;
	UINT i;
	int j;
	int c;
	char uselesschar;
	va_list pa;

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

UINT CStr::GetLength()
{
	return Length;
}

CHAR CStr::GetAt(UINT pos)
{
	if(pos>=Length){
		return 0;
	}

	return Buffer[pos];
}

//VOID CStr::VarToString(const double var)
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


VOID CStr::Clean()
{
	if(Buffer){
		delete []Buffer;
		Buffer=0;
	}
	Length=0;
}

UINT CStr::Format(PCCHAR format, ...)
{
	CHAR buf[2048];

    va_list vaptr;

    va_start(vaptr,format);
    vsprintf(buf,format,vaptr);
    va_end(vaptr);

	Assign(buf);

	return strlen(buf);
}

CStr& CStr::operator +=(CStr & str)
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
//CHAR CStr::NumericParse(void* pvar, char flag)
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

char CStr::Match(char* Wildcards, char* str)
{
	char Yes = 1;

	//iterate and delete '?' and '*' one by one
	while(*Wildcards != '\0' && Yes && *str != '\0')
	{
		if (*Wildcards == '?') str ++;
		else if (*Wildcards == '*')
		{
			Yes = Scan(Wildcards, str);
			Wildcards --;
		}
		else
		{
			Yes = (*Wildcards == *str);
			str ++;
		}
		Wildcards ++;
	}
	while (*Wildcards == '*' && Yes)  Wildcards ++;

	return Yes && *str == '\0' && *Wildcards == '\0';
}

// scan '?' and '*'
CHAR CStr::Scan(char*& Wildcards, char*& str)
{
	// remove the '?' and '*'
	for(Wildcards ++; *str != '\0' && (*Wildcards == '?' || *Wildcards == '*'); Wildcards ++)
		if (*Wildcards == '?') str ++;
	while ( *Wildcards == '*') Wildcards ++;
	
	// if str is empty and Wildcards has more characters or,
	// Wildcards is empty, return 
	if (*str == '\0' && *Wildcards != '\0') return 0;
	if (*str == '\0' && *Wildcards == '\0')	return 1; 
	// else search substring
	else
	{
		char* wdsCopy = Wildcards;
		char* strCopy = str;
		char  Yes     = 1;
		do 
		{
			if (!Match(Wildcards, str))	strCopy ++;
			Wildcards = wdsCopy;
			str		  = strCopy;
			while ((*Wildcards != *str) && (*str != '\0')) str ++;
			wdsCopy = Wildcards;
			strCopy = str;
		}while ((*str != '\0') ? !Match(Wildcards, str) : (Yes = 0) != 0);

		if (*str == '\0' && *Wildcards == '\0')	return 1;

		return Yes;
	}
}


CStr & CStr::Assign(PCCHAR s, UINT count)
{

	if(s==0){
		return *this;
	}

	Clean();

	Buffer=new char [count+1];

	strncpy(Buffer, s, count);

	Buffer[count]=0;
	Length=count;

	return *this;
}

CHAR CStr::Insert(UINT pos, PCCHAR str, UINT count)
{
	PCHAR p=0;
	UINT len;

	if(pos>Length){
		return 0;
	}

	len=strlen(str);
	if(count>len){
		count=len;
	}
	
	if(Buffer==0){
		Assign(str,count);
		return 1;
	}

	p=new char[Length+count+1];

	strncpy(p,Buffer,pos);
	strncpy(p+pos,str,count);
	strncpy(p+pos+count,Buffer+pos,Length-pos);
	p[Length+count]=0;
	
	delete []Buffer;
	Buffer=p;
	Length=Length+count;

	return 1;
}

CHAR CStr::Remove(UINT pos, UINT count)
{
	PCHAR p=0;
	//UINT len;

	if(Buffer==0){
		return 0;
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

	return 1;


}

//Dengting appdend for format whit !?* FormatWithDivider
//CHAR CStr::FormatWithDivider(PCCHAR format)
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


LONG CStr::Find(UINT b, PCCHAR p, UINT len, UINT id)
{
	UINT i;
	LONG pos=-1;
	INT found=-1;
	//UINT length;

	//length=GetLength();

	if(Length<1){
		return -1;
	}

	if(len<1){
		return -1;
	}

	if(b+len>Length){
		return -1;
	}

	for(i=b; i<=Length-len; i++){
		if(memcmp(Buffer+i,p,len)==0){
			pos=i;
			found++;

			if(found>=(LONG)id){
				return pos;
			}
		}
	}

	return -1;
}

LONG CStr::FindRev(UINT b, PCCHAR p, UINT len, UINT id)
{
	int i;
	LONG pos=-1;
	INT found=-1;
	//UINT length;

	//length=GetLength();

	if(Length<1){
		return -1;
	}

	if(b>Length){
		return -1;
	}

	if(len<1){
		return -1;
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


CHAR CStr::KeepMid(UINT pos, UINT count)
{
	PCHAR p=0;

	if(pos+count>Length){
		return 0;
	}

	p=new char[count+1];
	if(p==0){
		return 0;
	}

	strncpy(p,Buffer+pos,count);
	p[count]=0;

	delete []Buffer;
	Buffer=p;
	Length=count;

	return 1;
}

CHAR CStr::KeepLeft(UINT count)
{
	return KeepMid(0,count);
}

CHAR CStr::KeepRight(UINT count)
{
	UINT len;
	len=GetLength();

	return KeepMid(len-count,count);
}

CStr CStr::Mid(UINT pos, UINT count)
{
	if(pos+count>GetLength()){
		return *this;
	}

	CStr str;
	str.Assign(Buffer+pos,count);
	return str;
}

CStr CStr::Left(UINT count)
{
	if(count>GetLength()){
		return *this;
	}

	CStr str;
	str.Assign(Buffer,count);
	return str;
}

CStr CStr::Right(UINT count)
{
	if(count>GetLength()){
		return *this;
	}

	CStr str;
	str.Assign(Buffer+GetLength()-count);
	return str;
}

CHAR CStr::Replace(PCCHAR str, PCCHAR replacestr, UINT replacelen, UINT id)
{
	LONG pos;
	INT len;

	len=strlen(str);

	pos=Find(0,str,len,id);
	if(pos==-1){
		return 0;
	}

	Remove(pos,len);
	Insert(pos,replacestr,replacelen);	//Length updated

	return 1;
}

VOID CStr::ReplaceAll(PCCHAR str, PCCHAR replacestr, UINT replacelen)
{
	LONG pos;
	INT len;
	INT b=0;
	len=strlen(str);

	while( (pos=Find(b,str,len,0))!=-1 ){
		Remove(pos,len);
		Insert(pos,replacestr,replacelen);

		b=pos+replacelen;
	}
	
}

CStr CStr::GetDividedString(ULONG id, PCCHAR divider)
{
	int len;
	int b;
	int e;

	len=strlen(divider);

	//find begin pos
	if(id>0){
		b=Find(0,divider,len,id-1);
		if(b==-1){
			b=Length;
		}else{
			b++;
		}
	}else{
		b=0;
	}

	//find end pos
	e=Find(0,divider,len,id);
	if(e==-1){
		e=Length;
	}

	if(b>e){
		b=e;
	}

	return Mid(b,e-b);
}


ULONG CStr::GetDividedStringCount(PCCHAR divider)
{
	UINT i;
	int len;
	ULONG count=1;

	len=strlen(divider);

	for(i=0; i<Length; i++){
		if(strncmp(Buffer+i, divider,len)==0){
			count++;			
		}
	}

	return count;
}
