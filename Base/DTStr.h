///////////////////////////////////////////////////////////////////////////////////////
//字符串类
//Author: Dengting
//Edition: 2008-05-21
//
//Modification:
//2008-05-21: Addend ToUnsignedInteger() method.
//2008-05-15: Append UpdateLength() method.
//2008-05-14: Improved Mid() Left() Right() KeepMid().
//2007-12-14: Append MatchWildcard() MatchWildcardI() MatchWildcardGroup() MatchWildcardGroupI()
//2007-12-14: Modified Operator char * and Operator char * and GetBuffer(), when Buffer==NULL, then AssignAndZeroTail(0) first.
//2007-12-14: Modified Assing(length) to AssignAndZeroTail(), Zero the tail after new-memory;
//2007-11-27: Fixed Length update of Assign(CharCount) & ReleaseBuffer 
//2007-11-16: Removed Assign(const char * s); replaced by Assign(const char * s, LONG count=-1);
//2007-11-16: Add Replace(pos,len,replacestr,replacelen);
//2007-11-16: Improved Speed Find() FindI FindRev FindRevI Replace() ReplaceI() ReplaceAll ReplaceAllI...
//2007-11-16: Removed GetDividedStringCountEx() & GetDividedStringEx() ..:-(
//2007-11-16: Added GetDividedStringCountEx() & GetDividedStringEx() for C Language expression liked comma-mark-divided... 逗号标记分开
//2007-10-28: Modified base GetString(... *outlen,...), Append outlen para. Fixed buf GetInteger() returns 0 for "", shuld be DEFAULT value,
//2007-10-28: Modified ToInteger(BASE) base one 2,4,8,16
//2007-09-06: Modified Insert(Count=-1) -1 means full-length
//2007-09-06: Append Like();
//2007-08-30: Append FindI() & FindRevI ignore case
//2007-08-30: Append MakeUpper() MakeLower()
//2007-08-30: Improved FindRev(Str) & Find(Str) support -1 len,
//2007-08-30: Appened 2 Find() function for char
//2007-08-30: Appened == != Operators
//2007-07-10: Append ToInteger() and ToDouble()
//2007-07-10: Port to VC6 & WINCE
//2007-03-04: Changed IDtype of para to UINT for (Find FindRev Replace) 
//2007-03-03: addpend GetDividedString() & GetDividedStringCount()
//2007-03-03: add Length member.
//2007-03-02: append real Left() Mid() Right by return substring ... -_-!!以前没问题。
//2007-03-02: changed original Left() Mid() Right() to KeepLeft() KeepMid() KeepRight(),
//2007-02-08: removed Replace() and ReplaceAll for a LONG integer,
//2007-02-08: changed Find(id) first id is 0
///////////////////////////////////////////////////////////////////////////////////////
#ifndef H_K_STR_H
#define H_K_STR_H

#include <string.h>


class CDTStr
{
public:

	//构造函数
	CDTStr(const CDTStr & str);
	CDTStr(const char far * str);
	CDTStr(const int var);
	CDTStr(const double var);
	CDTStr();
	~CDTStr();

	//移除字符
	CHAR RemoveChars(INT n, ...);

	//重新分配count长度，并且填充末尾
	BOOL AssignAndZeroTail(LONG count);
	
	CDTStr & Assign(const char * s, LONG count=-1);

	CDTStr & AssignPathPart(const char * fullpath);
	CDTStr & AssignFilePart(const char * fullpath);

	//运算符
	CDTStr & operator =(const char * s);
	CDTStr & operator =(const CDTStr& s);
	CDTStr & operator +=(int str)	{ return *this += (CDTStr)str; }
	CDTStr & operator +=(double str)	{ return *this += (CDTStr)str; }
	CDTStr & operator +=(const char * str)	{ return *this += (CDTStr)str; }
	CDTStr & operator +=(CDTStr & str);

	BOOL operator ==(char *str){return strcmp(str, Buffer) == 0;}
	BOOL operator ==(const char *str){return strcmp(str, Buffer) == 0;}
	BOOL operator ==(CDTStr &str){return strcmp(str.Buffer, Buffer)==0;}

	BOOL operator !=(char *str){return strcmp(str, Buffer);}
	BOOL operator !=(const char *str){return strcmp(str, Buffer);}
	BOOL operator !=(CDTStr &str){return strcmp(str.Buffer, Buffer);}

	BOOL IsFloat();

	BOOL Like(const char *str);
	BOOL Like(CDTStr &str) {return Like(str.Buffer);}


	BOOL LikeN(const char * str, int n);
	BOOL LikeNI(const char * str, int n);
	

	BOOL LikeI(const char *str);
	BOOL LikeI(CDTStr &str) {return LikeI(str.Buffer);}

	
	// c type string conversion
	operator char * ();
	operator const char * ();
	CHAR * GetBuffer();

	void Clean();

	char GetAt(LONG pos);
	LONG GetLength();

	//Release tail unused memory...
	BOOL ReleaseBuffer();
	BOOL UpdateLength();


	INT Format(const char * format, ...);

	BOOL Remove(UINT begin, UINT count);

	BOOL Insert(LONG pos, const char * str, LONG count=-1);

	VOID ReplaceAll(char c, char replaceto);
	BOOL Replace(LONG pos, LONG len, const char * replacestr, LONG replacelen=-1);

	BOOL Replace(const char * str, const char * replacestr, LONG replacelen=-1, UINT id=0);
	INT ReplaceAll(const char * str, const char * replacestr, LONG replacelen=-1);

	BOOL ReplaceI(const char * str, const char * replacestr, UINT replacelen=-1, UINT id=0);
	INT ReplaceAllI(const char * str, const char * replacestr, UINT replacelen=-1);

	CDTStr Mid(LONG pos, LONG count);
	CDTStr Left(LONG count);
	CDTStr Right(LONG count);

	BOOL KeepMid(LONG pos, LONG count);
	BOOL KeepLeft(LONG count);
	BOOL KeepRight(LONG count);

	LONG Find(LONG b, const CHAR c, LONG id);
	LONG FindRev(LONG b, const CHAR c, LONG id);

	LONG Find(LONG b, const char * p, LONG len, LONG id);
	LONG Find(const char * p){return Find(0,p,-1,0);}

	LONG FindRev(LONG b, const char * p, LONG len, LONG id);
	LONG FindRev(const char * p){return FindRev(0,p,-1,0);}

	LONG FindI(LONG b, const char * p, LONG len, LONG id);
	LONG FindI(const char * p){return FindI(0,p,-1,0);}
	
	LONG FindRevI(LONG b, const char * p, LONG len, LONG id);
	LONG FindRevI(const char * p){return FindRevI(0,p,-1,0);}

	VOID MakeUpper();
	VOID MakeLower();

	CDTStr GetDividedString(ULONG id, const char * divider);
	ULONG GetDividedStringCount(const char * divider);


	LONG ToInteger(INT begin, INT base);
	ULONG ToUnsignedInteger(INT begin, INT base);
	DOUBLE ToDouble(INT begin);

	BOOL MatchWildcard(LPCSTR wildcard);
	BOOL MatchWildcardGroup(LPCSTR group, LPCSTR divider);

	BOOL MatchWildcardI(LPCSTR wildcard);
	BOOL MatchWildcardGroupI(LPCSTR group, LPCSTR divider);

	char Match(char * wildcards, char * str);

private:

//	// numeric conversion helpers
//	CHAR NumericParse(void* pvar, char flag);
//	char GetVar(bool& var)				{ return NumericParse((void*)&var, 'b'); }
//	char GetVar(char& var)				{ return NumericParse((void*)&var, 'c'); }
//	char GetVar(short& var)				{ return NumericParse((void*)&var, 's'); }
//	char GetVar(int& var)				{ return NumericParse((void*)&var, 'i'); }
//	char GetVar(long& var)				{ return NumericParse((void*)&var, 'l'); }
//	char GetVar(float& var)				{ return NumericParse((void*)&var, 'f'); }
//	char GetVar(double& var)			{ return NumericParse((void*)&var, 'd'); }
//	char GetVar(unsigned char& var)		{ return NumericParse((void*)&var, 'C'); }
//	char GetVar(unsigned short& var)	{ return NumericParse((void*)&var, 'S'); }
//	char GetVar(unsigned int& var)		{ return NumericParse((void*)&var, 'I'); }
//	char GetVar(unsigned long& var)		{ return NumericParse((void*)&var, 'L'); }

	char Scan(char*&, char*&);

protected:
	LONG Length;
	char * Buffer;
};
#endif