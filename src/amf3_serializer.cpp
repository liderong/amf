//////////////////////////////////////////////////////////////////////
// 根据项目AMF3Cplusplus做了一些简单的修改，支持去头尾的amf版协议
// hui.wang.chn@gmail.com
// 2011-06-05
//////////////////////////////////////////////////////////////////////

#include "amf3_serializer.h"
#include <algorithm>
#include "coding_utf8.h"

namespace ns_amf3
{
	/* Use correct types for x64 platforms, too */
	class AMFObject_Sort
	{
	public:  
		bool operator () (const AmfObjectHandle& a,const AmfObjectHandle& b) const  
		{   
			return a.getObject()->getObjectName()<b.getObject()->getObjectName();
		}
	};

int64 swap_i64(int64 data)
{
	unsigned char buf[8];
	::memcpy(buf,&data,8);

	int64 retvalue = 0;
	int64 tmp;

	tmp = buf[0];
	retvalue = tmp<<56;

	tmp = buf[1];
	retvalue |= tmp<<48;

	tmp = buf[2];
	retvalue |= tmp<<40;

	tmp = buf[3];
	retvalue |= tmp<<32;

	tmp = buf[4];
	retvalue |= tmp<<24;

	tmp = buf[5];
	retvalue |= tmp<<16;

	tmp = buf[6];
	retvalue |= tmp<<8;

	tmp = buf[7];
	retvalue |= tmp;

	return retvalue;
}

uint64 swap_u64(uint64 data)
{
	unsigned char buf[8];
	::memcpy(buf,&data,8);

	uint64 retvalue = 0;
	uint64 tmp;

	tmp = buf[0];
	retvalue = tmp<<56;

	tmp = buf[1];
	retvalue |= tmp<<48;

	tmp = buf[2];
	retvalue |= tmp<<40;

	tmp = buf[3];
	retvalue |= tmp<<32;

	tmp = buf[4];
	retvalue |= tmp<<24;

	tmp = buf[5];
	retvalue |= tmp<<16;

	tmp = buf[6];
	retvalue |= tmp<<8;

	tmp = buf[7];
	retvalue |= tmp;

	return retvalue;
}

AmfObjectHandle::AmfObjectHandle():pObject_(0)
{
}

AmfObjectHandle::AmfObjectHandle(const AmfObjectHandle &rhs)
{
	pObject_ = rhs.pObject_;

	if(pObject_) 
		pObject_->add_ref();
}

AmfObjectHandle::AmfObjectHandle(AMFObject* obj)
{
	pObject_ = obj;
	if(pObject_!=0)
		pObject_->add_ref();
}

AmfObjectHandle::~AmfObjectHandle(void)
{
	if(pObject_!=0)
		pObject_->release();
}

bool AmfObjectHandle::isNULL()
{
	return pObject_==0;
}

void AmfObjectHandle::release()
{
	if(pObject_!=0)
		pObject_->release();

	pObject_ = 0;
}

AmfObjectHandle& AmfObjectHandle::operator=(const AmfObjectHandle &rhs)
{
	if(pObject_==rhs.pObject_) 
		return *this;

	AMFObject* old = pObject_;
	pObject_ = rhs.pObject_;
	
	if(pObject_!=NULL) 
		pObject_->add_ref();

	if(old!=NULL) 
		old->release();

	return *this;
}

AmfObjectHandle& AmfObjectHandle::operator = (AMFObject* obj)
{
	if(pObject_==obj) 
		return *this;

	AMFObject* old = pObject_;
	pObject_ = obj;
	
	if(pObject_!=NULL) 
		pObject_->add_ref();

	if(old!=NULL) 
		old->release();

	return *this;
}

AMFObject* AmfObjectHandle::operator -> () const
{
	return pObject_;
}

bool AmfObjectHandle::operator ==(const AmfObjectHandle &rhs) const
{
	return pObject_==rhs.pObject_;
}

////////////////////////////////////////
AmfObjectHandle AMFObject::Alloc()
{
	AmfObjectHandle hobj = new AMFObject();
	hobj->release();
	return hobj;
}

AMFObject::AMFObject()
{
	reference_ = 1;
	byteArrayLength_ = 0;
	byteArrayValue_ = 0;
	clearValue();
}

AMFObject::~AMFObject(void)
{
	clearValue();
}

void AMFObject::clearValue()
{
	type_ = AMF3_UNDEFINED;
	
	boolValue_ = false;
	intValue_ = 0;
	doubleValue_ = 0.0f;
	stringValue_.clear();

	byteArrayLength_ = 0;
	if(byteArrayValue_!=0)
	{
		delete byteArrayValue_;
		byteArrayValue_ = 0;
	}
	//::memset(&dateValue,0,sizeof(SYSTEMTIME));
}


void AMFObject::clearValueWithChildren()
{
	clearValue();
	childrens_.clear();
}


void AMFObject::add_ref()
{
	++reference_;
	//::InterlockedIncrement(&reference_);
}

void AMFObject::release()
{
	--reference_;
	if(reference_==0)
		delete this;
}

#define EPOCH_DIFF 116444736000000000 //FILETIME starts from 1601-01-01 UTC, epoch from 1970- 01-01

double AMFObject::getTimeSeed()
{
// 	TIME_ZONE_INFORMATION zinfo;
// 	::GetTimeZoneInformation(&zinfo);
// 
// 	SYSTEMTIME st;
// 	::TzSpecificLocalTimeToSystemTime(&zinfo,&dateValue,&st);
// 
// 	dateValue.wYear = st.wYear;
// 	dateValue.wMonth = st.wMonth;
// 	dateValue.wDay = st.wDay;
// 	dateValue.wDayOfWeek = st.wDayOfWeek;
// 	dateValue.wHour = st.wHour;
// 	dateValue.wMinute = st.wMinute;
// 	dateValue.wSecond = st.wSecond;
// 	dateValue.wMilliseconds = st.wMilliseconds;
// 	
// 	FILETIME filetime;
// 	::SystemTimeToFileTime(&dateValue,&filetime);
// 	int64 t = filetime.dwHighDateTime;
// 	t = t<<32 | filetime.dwLowDateTime;
// 	t -= EPOCH_DIFF;
// 	return (double)(t/10000);
	return 1;
}

void AMFObject::setTimeSeed(double t)
{
// 	int64 lft = (int64)t;
// 	lft = lft * 10000 + EPOCH_DIFF;
// 
// 	FILETIME ft;
// 	ft.dwLowDateTime = (DWORD)lft;
// 	ft.dwHighDateTime = (DWORD)(lft>>32);
// 	::FileTimeToSystemTime(&ft,&dateValue);
// 	
// 	TIME_ZONE_INFORMATION zinfo;
// 	::GetTimeZoneInformation(&zinfo);
// 
// 	SYSTEMTIME st;
// 	::SystemTimeToTzSpecificLocalTime(&zinfo,&dateValue,&st);
// 
// 	dateValue.wYear = st.wYear;
// 	dateValue.wMonth = st.wMonth;
// 	dateValue.wDay = st.wDay;
// 	dateValue.wDayOfWeek = st.wDayOfWeek;
// 	dateValue.wHour = st.wHour;
// 	dateValue.wMinute = st.wMinute;
// 	dateValue.wSecond = st.wSecond;
// 	dateValue.wMilliseconds = st.wMilliseconds;
}

void AMFObject::setAsUnsignedNumber(unsigned int num)
{
	if(num<=0x0FFFFFFF)
	{
		type_ = AMF3_INTEGER;
		intValue_ = num;
	}
	else
	{
		type_ = AMF3_DOUBLE;
		doubleValue_ = (double)num;
	}
}


void AMFObject::initObject(const std::string& name,double num)
{
	objectName_ = name;
	setAsDouble(num);
}

void AMFObject::initObject(const std::string& name, const std::string &strValue )
{
	objectName_ = name;
	type_ = AMF3_STRING;
	stringValue_ = strValue;
}

void AMFObject::initObject( const std::string& name, int num )
{
	objectName_ = name;
	setAsInt(num);
}

void AMFObject::initObject( const std::string& name, bool bValue )
{
	objectName_ = name;
	type_ = bValue?AMF3_TRUE:AMF3_FALSE;
}

void AMFObject::addChild(const AmfObjectHandle& obj)
{
	childrens_.push_back(obj);
}

AmfObjectHandle AMFObject::addChild( const std::string& name, AMFDataType type )
{
	AmfObjectHandle obj = AMFObject::Alloc();
	obj->setObjectName(name);
	obj->setType(type);
	childrens_.push_back(obj);
	return obj;
}

void AMFObject::addChild( const std::string& name, double value )
{
	if(type_!=AMF3_ARRAY && type_!=AMF3_OBJECT){
		return;
	}
	AmfObjectHandle obj = AMFObject::Alloc();
	obj->initObject(name,value);
	childrens_.push_back(obj);
}

void AMFObject::addChild( const std::string& name, const std::string& strValue )
{
	if(type_!=AMF3_ARRAY && type_!=AMF3_OBJECT){
		return;
	}
	AmfObjectHandle obj = AMFObject::Alloc();
	obj->initObject(name,strValue);
	childrens_.push_back(obj);
}

void AMFObject::addChild( const std::string& name, int value )
{
	if(type_!=AMF3_ARRAY && type_!=AMF3_OBJECT){
		return;
	}
	AmfObjectHandle obj = AMFObject::Alloc();
	obj->initObject(name,value);
	childrens_.push_back(obj);
}

void AMFObject::addChild( const std::string& name, bool value )
{
	if(type_!=AMF3_ARRAY && type_!=AMF3_OBJECT){
		return;
	}
	AmfObjectHandle obj = AMFObject::Alloc();
	obj->initObject(name,value);
	childrens_.push_back(obj);
}

bool AMFObject::hasChild(const char* name)
{
	for(size_t i=0;i<childrens_.size();i++)
	{
		if(childrens_[i]->objectName_==name)
			return true;
	}

	return false;
}

AmfObjectHandle AMFObject::getChild(const char* name)
{
	for(size_t i=0; i<childrens_.size(); i++) {
		if(childrens_[i]->objectName_==name)
			return childrens_[i];
	}
	return 0;
}

void AMFObject::sortChildByName()
{
	std::sort(childrens_.begin(),childrens_.end(),AMFObject_Sort());
	for (uint32 i=0; i<childrens_.size(); ++i) {
		childrens_[i]->sortChildByName();
	}
}

void AMFObject::setAsInt(int num )
{
	if(num>0)
	{
		if(num<=0x0FFFFFFF)
		{
			type_ = AMF3_INTEGER;
			intValue_ = (int)num;
		}
		else
		{
			type_ = AMF3_DOUBLE;
			doubleValue_ = (double)num;
		}
	}
	else
	{
		type_ = AMF3_DOUBLE;
		doubleValue_ = (double)num;
	}
}

void AMFObject::setAsDouble( double num )
{
	type_ = AMF3_DOUBLE;
	doubleValue_ = (double)num;
}


unsigned char* realloc_read_buf(AMFContext* ctx,size_t size)
{
	if(ctx->readBufferSize_<size)
	{
		free(ctx->readBuffer_);
		ctx->readBuffer_ = (unsigned char*)malloc(size);
		ctx->readBufferSize_ = size;
	}

	return ctx->readBuffer_;
}

// 索引相关操作，支持不是很好，少用
//////////////////////////////////////////////////////////////
//refrence
bool is_refrence(int header)
{
	return 0 == (header & REFERENCE_BIT);
}

AmfObjectHandle get_ref_tab(Amf_Ref_Vec_Type &tab,uint32 idx)
{
	if(idx>=0 && idx<tab.size())
		return tab[idx];

	return 0;
}

int add_ref_tab(Amf_Ref_Vec_Type &tab,AmfObjectHandle obj)
{
	tab.push_back(obj);
	return tab.size()-1;
}

//refrence
//////////////////////////////////////////////////////////////


AmfObjectHandle read_elem(AMFContext* ctx);

int read(AMFContext* ctx,size_t size,void* buf)
{
	return ctx->readFunction_(ctx->inBufferHandle_,size,(unsigned char*)buf);
}

unsigned char read_byte(AMFContext* ctx)
{
	unsigned char data;
	read(ctx,1,&data);
	return data;
}

AMFDataType read_type(AMFContext* ctx)
{
	return (AMFDataType)read_byte(ctx);
}

int read_int(AMFContext* ctx)
{
	unsigned char byte = read_byte(ctx);
	int readnum = 0;
	int value = 0;
	while(((byte & 0x80)!=0) && (readnum<3))
	{
		value = (value<<7) | (byte & 0x7F);
		++readnum;
		byte = read_byte(ctx);
	}

	//最后一字节
	if(readnum<3)
		value = (value<<7) | (byte & 0x7F);
	else
		value = (value<<8) | (byte & 0xFF);

	//第29位的符号位
	if((value&0x10000000)!=0)
		value = (value&0x10000000) | 0x80000000;
	
	return value;
}

int64 read_i64(AMFContext* ctx)
{
	int64 data = 0;
	read(ctx,8,(unsigned char*)&data);
	return swap_i64(data);
}

uint64 read_u64(AMFContext* ctx)
{
	uint64 data = 0;
	read(ctx,8,(unsigned char*)&data);
	return swap_u64(data);
}

double read_double(AMFContext* ctx)
{
	double data = 0.0f;
	uint64 tmp = read_u64(ctx);
	::memcpy(&data,&tmp,8);
	return data;
}

void read_string(AMFContext* ctx,std::string &str)
{
	str.clear();

	int head = read_int(ctx);
	if(head==EMPTY_STRING) return;
	
	if(is_refrence(head))
	{
		int idx = head>>1;
		AmfObjectHandle obj = get_ref_tab(ctx->mReadStrRefVec_,idx);
		if(!obj.isNULL() && obj->getType()==AMF3_STRING)
		{
			str = obj->getStringValue();
		}
	}
	else
	{
		int len = head>>1;
		if(len<=0)
		{
			return;
		}

		char* buf = (char*)realloc_read_buf(ctx,len+1);
		read(ctx,len,buf);
		buf[len] = '\0';

		str = utfs2s(buf);
		//str = UTF8_2_OEM(buf);

		AmfObjectHandle obj = AMFObject::Alloc();
		obj->setType(AMF3_STRING);
		obj->setStringValue(str);
		add_ref_tab(ctx->mReadStrRefVec_,obj);
	}
}

AmfObjectHandle read_date(AMFContext* ctx)
{
	AmfObjectHandle obj;

	int head = read_int(ctx);
	if(is_refrence(head))
	{
		//索引号
		int idx = head>>1;
		AmfObjectHandle refobj = get_ref_tab(ctx->mReadObjectRefVec_,idx);
		if(!refobj.isNULL() && refobj->getType()==AMF3_DATE)
		{
			return refobj;
		}
	}
	else
	{
		double time = read_double(ctx)/1000;

		obj = AMFObject::Alloc();
		obj->setType(AMF3_DATE);
		obj->setTimeSeed(time);
	
		add_ref_tab(ctx->mReadObjectRefVec_,obj);
	}

	return obj;
}

AmfObjectHandle read_array(AMFContext* ctx)
{
	AmfObjectHandle obj;

	int head = read_int(ctx);
	if(is_refrence(head))
	{
		int idx = head>>1;
		AmfObjectHandle refobj = get_ref_tab(ctx->mReadObjectRefVec_,idx);
		if(!refobj.isNULL() && refobj->getType()==AMF3_ARRAY)
		{
			return refobj;
		}
	}
	else
	{
		obj = AMFObject::Alloc();
		obj->setType(AMF3_ARRAY);

		int subnum = head>>1;
		std::string name;
		read_string(ctx,name);

		//读取子元素
		for(int i=0;i<subnum;i++)
		{
			AmfObjectHandle sub = read_elem(ctx);
			if(!sub.isNULL())
				obj->addChild(sub);
			else
				obj->addChild(AMFObject::Alloc());
		}

		add_ref_tab(ctx->mReadObjectRefVec_,obj);
	}

	return obj;
}

AmfObjectHandle read_obj(AMFContext* ctx)
{
	AmfObjectHandle obj;

	int objref = read_int(ctx);
	bool dynamic = false;
	if(is_refrence(objref))
	{
		int idx = objref>>1;

		AmfObjectHandle refobj = get_ref_tab(ctx->mReadObjectRefVec_,idx);
		if(!refobj.isNULL() && refobj->getType()==AMF3_OBJECT)
		{
			return refobj;
		}
	}
	else
	{
		int subnum = 0;

		int maskbit2 = ((objref>>1)&0x1);
		int maskbit3 = ((objref>>2)&0x1);
		int maskbit4 = ((objref>>3)&0x1);

		if(maskbit2==0)
		{
			int idx = objref>>2;

			AmfObjectHandle refobj = get_ref_tab(ctx->mReadObjectRefVec_,idx);
			if(!refobj.isNULL() && refobj->getType()==AMF3_OBJECT)
			{
				return refobj;
			}
		}
		else
		{
			if(maskbit3==0)
			{
				dynamic = maskbit4!=0;
				subnum = objref>>4;
			}
			else
			{
				//暂不支持
			}
		}

		obj = AMFObject::Alloc();
		obj->setType(AMF3_OBJECT);

		read_string(ctx,obj->getClassName());

		std::string subname;
		std::vector<std::string> subs;
		for(int i=0;i<subnum;i++)
		{
			read_string(ctx,subname);
			subs.push_back(subname);
		}

		for(int i=0;i<subnum;i++)
		{
			AmfObjectHandle sub = read_elem(ctx);
			if(!sub.isNULL())
			{
				sub->setObjectName(subs[i]);
				obj->addChild(sub);
			}
		}

		//读取动态属性
		if(dynamic)
		{
			for(int i=0;i<DYNAMIC_OBJ_MAX_PROPERTY;i++)
			{
				read_string(ctx,subname);
				if(subname.empty()) break;

				AmfObjectHandle sub = read_elem(ctx);
				if(!sub.isNULL())
				{
					sub->setObjectName(subname);
					obj->addChild(sub);
				}
			}
		}

		add_ref_tab(ctx->mReadObjectRefVec_,obj);
	}

	return obj;
}

AmfObjectHandle read_bytearray(AMFContext* ctx)
{
	AmfObjectHandle obj = AMFObject::Alloc();
	obj->setType(AMF3_BYTEARRAY);

	int objref = read_int(ctx);
	if(!is_refrence(objref))
	{
		int len = objref>>1;
		
		unsigned char* buf = (unsigned char*)malloc(len);
		read(ctx,len,buf);

		obj->setByteArrayValue(buf);
		obj->setByteArrayLength(len);

		return obj;
	}

	//可能有误，暂时不支持引用表
	int idx = objref>>1;
	obj = get_ref_tab(ctx->mReadObjectRefVec_,idx);
	if(!obj.isNULL() && obj->getType()==AMF3_ARRAY)
	{
		return obj;
	}

	return NULL;
}

AmfObjectHandle read_elem(AMFContext* ctx)
{
	AmfObjectHandle obj = AMFObject::Alloc();

	obj->setType(read_type(ctx));
	switch(obj->getType())
	{
	case AMF3_NULL:
	break;
	case AMF3_FALSE:
		obj->setBoolValue(false);
	break;
	case AMF3_TRUE:
		obj->setBoolValue(true);
	break;
	case AMF3_INTEGER:
		obj->setIntValue(read_int(ctx));
	break;
	case AMF3_DOUBLE:
		obj->setDoubleValue(read_double(ctx));
	break;
	case AMF3_STRING:
		read_string(ctx,obj->getStringValue());
	break;
	case AMF3_DATE:
		obj = read_date(ctx);
	break;
	case AMF3_ARRAY:
		obj = read_array(ctx);
	break;
	case AMF3_OBJECT:
		obj = read_obj(ctx);
	break;
	case AMF3_BYTEARRAY:
		obj = read_bytearray(ctx);
	break;
	default:
		return 0;
	break;
	}

	return obj;
}

////////////////////////////////////////////////////////////////////////////

void write_elem(AMFContext* ctx,AmfObjectHandle& obj);

void write(AMFContext* ctx,const void* buf,size_t size)
{
	ctx->writeFunction_(ctx->outBufferHandle_,(unsigned char*)buf,size);
}

void write_byte(AMFContext* ctx,unsigned char data)
{
	write(ctx,&data,1);
}

void write_type(AMFContext* ctx,AMFDataType type)
{
	write_byte(ctx,type);
}

bool write_u29(AMFContext* ctx,unsigned int data)
{
	if(data<=0x7F)
	{
		write_byte(ctx,(unsigned char)data);
		return true;
	}

	unsigned int tmp;
	unsigned char buf[4];
	if(data<=0x3FFF)
	{
		tmp = data>>7 | 0x80;
		buf[0] = tmp;

		buf[1] = data & 0x7F;

		write(ctx,buf,2);

		return true;
	}

	if(data<=0x001FFFFF)
	{
		tmp = data>>14 | 0x80;
		buf[0] = tmp;

		tmp = ((data>>7) & 0x7F) | 0x80;
		buf[1] = tmp;

		buf[2] = data & 0x7F;

		write(ctx,buf,3);
		return true;
	}

	if(data<=0x0FFFFFFF)
	{
		tmp = data>>22 | 0x80;
		buf[0] = tmp;

		tmp = ((data>>15) & 0x7F) | 0x80;
		buf[1] = tmp;

		tmp = ((data>>8) & 0x7F) | 0x80;
		buf[2] = tmp;

		buf[3] = data & 0xFF;

		write(ctx,buf,4);

		return true;
	}

	return false;
}

void write_u64(AMFContext* ctx,uint64 data)
{
	uint64 vdata = swap_u64(data);
	write(ctx,&vdata,8);
}

void write_i64(AMFContext* ctx,int64 data)
{
	int64 vdata = swap_i64(data);
	write(ctx,&vdata,8);
}

void write_double(AMFContext* ctx,double data)
{
	uint64 tmp;
	::memcpy(&tmp,&data,8);
	write_u64(ctx,tmp);
}

void write_string(AMFContext* ctx,std::string str)
{
	//str = OEM_2_UTF8(str.c_str());
	str = s2utfs(str);


	int len = str.length();
	if(len==0)
	{
		write_byte(ctx,0x1);
	}
	else
	{
		int objref = len<<1 | 1;
		write_u29(ctx,objref);
		write(ctx,str.c_str(),len);
	}
}

void write_date(AMFContext* ctx,AmfObjectHandle obj)
{
	write_u29(ctx,1);
	write_double(ctx,obj->getTimeSeed());
}

void write_array(AMFContext* ctx,AmfObjectHandle obj)
{
	int objref = obj->childrens_.size();
	objref = objref<<1 | 1;
	write_u29(ctx,objref);
	
	write_string(ctx,"");
	for(uint32 i=0;i<obj->childrens_.size();i++)
	{
		write_elem(ctx,obj->childrens_[i]);
	}
}

void write_obj(AMFContext* ctx,AmfObjectHandle obj)
{
	int objref = 0x0B;
	write_u29(ctx,objref);
	write_string(ctx,"");

	for(uint32 i=0;i<obj->childrens_.size();i++)
	{
		write_string(ctx,obj->childrens_[i]->getObjectName());
		write_elem(ctx,obj->childrens_[i]);
	}

	write_string(ctx,"");
}

void write_bytearray(AMFContext* ctx,const unsigned char* data,int len)
{
	int objref = len<<1 | 1;
	write_u29(ctx,objref);
	write(ctx,data,len);
}

void write_elem( AMFContext* ctx,AmfObjectHandle& obj )
{
	if(obj.isNULL()) return;

	write_type(ctx,obj->getType());

	switch(obj->getType())
	{
	case AMF3_UNDEFINED:
	case AMF3_NULL:
	case AMF3_FALSE:
	case AMF3_TRUE:
	break;
	case AMF3_INTEGER:
		write_u29(ctx,obj->getIntValue());
	break;
	case AMF3_DOUBLE:
		write_double(ctx,obj->getDoubleValue());
	break;
	case AMF3_STRING:
		write_string(ctx,obj->getStringValue());
	break;
	case AMF3_DATE:
		write_date(ctx,obj);
	break;
	case AMF3_ARRAY:
		write_array(ctx,obj);
	break;
	case AMF3_OBJECT:
		write_obj(ctx,obj);
	break;
	case AMF3_BYTEARRAY:
		write_bytearray(ctx,obj->getByteArrayValue(),obj->getByteArrayLength());
	break;
	}
}
////////////////////////////////////////////////////////////////////////////////////


AmfObjectHandle g_decode(void* inBuffer)
{
	ns_amf3::AMFContext ctx;
	ctx.inBufferHandle_ = inBuffer;
	AmfObjectHandle hobj = read_elem(&ctx);
	return hobj;
}


void g_encode(void* outBuffer,AmfObjectHandle& obj)
{
	ns_amf3::AMFContext ctx;
	ctx.outBufferHandle_ = outBuffer;
	write_elem(&ctx,obj);
}

int read_data_callback(void* file,size_t size,unsigned char* buf )
{
	IOBufferHelper* data = (IOBufferHelper*)file;

	if((data->getBufferLength()-data->getPos())<size) 
		return 0;

	::memcpy(buf,data->getBuffer(data->getPos()),size);
	data->incrementPosition(size);
	return size;
}

void write_data_callback( void* file,const unsigned char* buf,size_t size )
{
	IOBufferHelper* data = (IOBufferHelper*)file;
	if((data->getBufferLength()-data->getPos())>=size) 
	{
		::memcpy(data->getBuffer(data->getPos()),buf,size);
		data->incrementPosition(size);
	}
}

#define CHAR2BYTE(x) ((x)<=0x39?(x-0x30):((x)<0x61?(x)-0x37:(x)-0x57))
#define HEX2BYTE(h,l) (CHAR2BYTE(h)<<4|CHAR2BYTE(l))

bool IsHexChar(char chr)
{
	if(chr>=0x30 && chr<=0x39) return true;
	if(chr>=0x41 && chr<=0x46) return true;
	if(chr>=0x61 && chr<=0x66) return true;
	return false;
}

void DecodeHex( IOBufferHelper* io,const char* pData )
{
	static unsigned char* s_decode_buf =(unsigned char*)malloc(65536);
	int length = DecodeHex(pData,s_decode_buf,65536);
	io->initFromBuffer(s_decode_buf,length);
	free(s_decode_buf);
}

unsigned int DecodeHex( const char* pData,unsigned char* pOut,int inlen )
{
	unsigned int decodesize = 0;
	if(inlen==0) 
		inlen = (int)::strlen(pData);

	char *pBinPtr = const_cast<char*>(pData);
	char *end = pBinPtr+inlen;
	unsigned char *pWrite = pOut;

	for(pBinPtr; pBinPtr < end; pBinPtr+=2)
	{
		if(!IsHexChar(*pBinPtr) || !IsHexChar(*(pBinPtr+1))) break;
		++decodesize;
		*(pWrite++) = HEX2BYTE(*pBinPtr,*(pBinPtr+1));
	}
	return decodesize;
}


IOBufferHelper::~IOBufferHelper()
{
	if(buffer_)
		free(buffer_);
}

void IOBufferHelper::mallocBuffer( int length )
{
	if(buffer_) free(buffer_);
	bufferLength_ = length;
	buffer_ = (unsigned char*)malloc(bufferLength_);
	pos_ = 0;
}

void IOBufferHelper::initFromBuffer(void* buffer,int length)
{
	mallocBuffer(length);
	::memcpy(buffer_,buffer,length);
	pos_ = 0;
}

void IOBufferHelper::initFromString(const std::string& str)
{
	initFromBuffer((void*)str.c_str(),str.length());
}

void IOBufferHelper::write(const void * data, size_t bytes)
{
	if(bufferLength_-pos_+1 < bytes) return;
	::memcpy(buffer_+pos_,data,bytes);
	pos_+=bytes;
}

AMFContext::AMFContext( Func_Read pRFunc/*=read_data*/,Func_Write pWFunc/*=write_data*/ )
{
	inBufferHandle_ = 0;
	outBufferHandle_ = 0;
	readBufferSize_ = 0;
	readBuffer_ = 0;

	readFunction_  = pRFunc?pRFunc:read_data_callback ;
	writeFunction_ = pWFunc?pWFunc:write_data_callback;
}

AMFContext::~AMFContext()
{
	if(readBufferSize_>0) {
		free(readBuffer_);
		readBufferSize_ = 0;
		readBuffer_ = 0;
	}
	inBufferHandle_ = 0;
	outBufferHandle_ = 0;
	mReadObjectRefVec_.clear();
	mReadStrRefVec_.clear();
}

}//end namespace AMF3

