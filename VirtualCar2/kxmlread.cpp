#include "stdafx.h"
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "base.h"

#include "dtxml.h"


//const char * XML_HEAD_FORMAT		="<%s>";
//const char * XML_TAIL_FORMAT		="</%s>";
//const char * XML_HEAD				="<";
//const char * XML_TAIL				=">";
//const char * XML_ENTER				="\r\n";
//const char * XML_TAB				="\t";
//CHAR XML_TAG_FIRST_CHAR				='<';
//CHAR XML_TAIL_TAG_SECOND_CHAR		='/';

CXMLReader::CXMLReader()
{
	Buffer=0;
	BufferSize=0;
	Depth=0;

	ReturnBuffer=0;
}

char CXMLReader::Close()
{
	if(Buffer){
		delete []Buffer;
		Buffer=0;
	}

	if(ReturnBuffer){
		delete []ReturnBuffer;
		ReturnBuffer=0;
	}

   return 1;
}

CXMLReader::~CXMLReader()
{
	if(Buffer){
		delete []Buffer;
	}

	if(ReturnBuffer){
		delete []ReturnBuffer;
	}
}


char CXMLReader::Open(const char * file)
{
	FILE * f;

	f=fopen(file,"r+b");
	if(f==0){
		return 0;
	}

	BufferSize=filelength(fileno(f));

	Buffer=new far char[BufferSize];

	fseek(f,0,SEEK_SET);
	fread(Buffer,BufferSize,1,f);

	fclose(f);

	Depth=0;
	Tag[Depth].Begin=2;
	//Tag[Depth].Begin=0;
	Tag[Depth].End=BufferSize;
	
	Tag[Depth].Tag.Empty();
	//Tag[Depth].Tag[0]='\0';

	return 1;
}


char CXMLReader::Enter(const char * tag)
{
	ULONG b,e;

	if(strlen(tag)>=XML_MAX_TAG_LEN-3){
		return 0;
	}

	if(!Search(tag,Tag[Depth].Begin+strlen(Tag[Depth].Tag)+XML_HEAD_ADDED_LEN, Tag[Depth].End, &b,&e)){
		return 0;
	}
	
	Depth++;

	//strcpy(Tag[Depth].Tag,tag);
	Tag[Depth].Tag=tag;

	Tag[Depth].Begin=b;
	Tag[Depth].End=e;

	return 1;
}

char CXMLReader::Back()
{
	Depth--;
	return 1;
}

long CXMLReader::GetInteger(const char * tag, long defaultvalue)
{
	char buf[25];
	if(!GetString(tag,buf,25)){
		return defaultvalue;
	}

	return atol(buf);
}

char CXMLReader::GetInteger(const char * tag, long far * value)
{
	char buf[25];
	if(!GetString(tag,buf,25)){
		return 0;
	}

	*value=atol(buf);
	return 1;
}



char * CXMLReader::GetString(const char * tag, const char * defaultvalue)
{
	ULONG b,e;

	if(ReturnBuffer){
		delete []ReturnBuffer;
		ReturnBuffer=0;
	}

	if(strlen(tag)>=XML_MAX_TAG_LEN-3){
		ReturnBuffer=NewCopy(defaultvalue);
		return ReturnBuffer;
	}

	if(!Search(tag,Tag[Depth].Begin+strlen(Tag[Depth].Tag)+XML_HEAD_ADDED_LEN, Tag[Depth].End, &b,&e)){
		ReturnBuffer=NewCopy(defaultvalue);
		return ReturnBuffer;
	}

	b+=strlen(tag)+XML_HEAD_ADDED_LEN;
	ReturnBuffer=new char[e-b+1];
	strncpy(ReturnBuffer,Buffer+b,e-b);
	ReturnBuffer[e-b]=0;

	return ReturnBuffer;
}

char CXMLReader::GetString(const char * tag, char * buf, int bufsize)
{
	ULONG b,e;

	if(strlen(tag)>=XML_MAX_TAG_LEN-3){
		return 0;
	}

	if(!Search(tag,Tag[Depth].Begin+strlen(Tag[Depth].Tag)+XML_HEAD_ADDED_LEN, Tag[Depth].End, &b,&e)){
		return 0;
	}

	if(e-b>=(ULONG)bufsize){
		return 0;
	}

	b+=strlen(tag)+XML_HEAD_ADDED_LEN;
	strncpy(buf,Buffer+b,e-b);
	buf[e-b]=0;
	
	return 1;
}


char CXMLReader::Search(const char * tag, ULONG begin, ULONG end, ULONG * b, ULONG * e)
{
	ULONG i;
	int headlen, taillen;
	int enteredsub=0;
	ULONG foundb=0,founde=0;
	char head[XML_MAX_TAG_LEN];
	char tail[XML_MAX_TAG_LEN];
	
	if(strlen(tag)>=XML_MAX_TAG_LEN-3){
		return 0;
	}

	sprintf(head,XML_HEAD_FORMAT,tag);
	sprintf(tail,XML_TAIL_FORMAT,tag);

	headlen=strlen(head);
	taillen=strlen(tail);

	for(i=begin; i<end; i++){
		if(Buffer[i]==XML_TAG_FIRST_CHAR){
			if(strncmp(Buffer+i,head,headlen)==0){
				if(enteredsub==0){
					foundb=i;
					break;
				}
			}

			if(Buffer[i+1]==XML_TAIL_TAG_SECOND_CHAR){
				enteredsub--;
			}else{
				enteredsub++;
			}
		}
	}

	if(foundb==0){
		return 0;
	}

	for(i=foundb+headlen; i<end; i++){
		if(Buffer[i]==XML_TAG_FIRST_CHAR){
			if(strncmp(Buffer+i,tail,taillen)==0){
				if(enteredsub==0){
					founde=i;
					break;
				}
			}

			if(Buffer[i+1]==XML_TAIL_TAG_SECOND_CHAR){
				enteredsub--;
			}else{
				enteredsub++;
			}
		}
	}

	if(founde==0){
		return 0;
	}

	*b=foundb;
	*e=founde;
	
	return 1;
}

