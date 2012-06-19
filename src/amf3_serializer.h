//////////////////////////////////////////////////////////////////////
// 根据项目AMF3Cplusplus做了一些简单的修改，支持去头尾的amf版协议
// hui.wang.chn@gmail.com
// 2011-06-05
// sample:
//      编码:
//	ns_amf3::AmfObjectHandle amf= ns_amf3::AMFObject::Alloc();
//	amf->setType(ns_amf3::AMF3_ARRAY);
//	amf->addChild("",std::string((*item)[psz_key_api].asCString()));
//	ns_amf3::AmfObjectHandle& child = amf->addChild("",ns_amf3::AMF3_OBJECT);
//	child->addChild("version",std::string("1.0.1.120224.3"));
//	child->addChild("isAMF",(bool)true);
// 	child->addChild("clientTime",(int)1330326040);
//	child->addChild("ct",(int)1330326040404);
//	解码:
//	ns_amf3::IOBufferHelper receiveData;
//	receiveData.initFromString(receiveBody);
//	ns_amf3::AmfObjectHandle obj = ns_amf3::g_decode(&receiveData);	
//////////////////////////////////////////////////////////////////////

#ifndef AMF3_H__
#define AMF3_H__

#include <string>
#include <vector>
#include "platform.h"

namespace ns_amf3
{
#define DYNAMIC_OBJ_MAX_PROPERTY	512   //避免解析错误出过大object，最大支持的动态子元素个数
#define REFERENCE_BIT	0x01                  //引用标志位
#define EMPTY_STRING	0x01                  //空串

//IO 读写函数
typedef int  (*Func_Read)(void* file,size_t size,unsigned char* buf);
typedef void (*Func_Write)(void* file,const unsigned char* buf,size_t size);

//数据类型定义
enum AMFDataType
{
	AMF3_UNDEFINED  = 0x00,	  //unsupport
	AMF3_NULL       = 0x01,
	AMF3_FALSE      = 0x02,
	AMF3_TRUE       = 0x03,
	AMF3_INTEGER    = 0x04,
	AMF3_DOUBLE     = 0x05,
	AMF3_STRING     = 0x06,
	AMF3_XMLDOC     = 0x07,	  //unsupport
	AMF3_DATE       = 0x08,
	AMF3_ARRAY      = 0x09,
	AMF3_OBJECT     = 0x0A,
	AMF3_XML        = 0x0B,	  //unsupport
	AMF3_BYTEARRAY  = 0x0C		
};

class AMFObject;

//----------------------------------------------------------
//obj handle 用于object的引用计数自动加减
class AmfObjectHandle
{
public:
	AmfObjectHandle();
	AmfObjectHandle(const AmfObjectHandle &rhs);
	AmfObjectHandle(AMFObject* obj);
	~AmfObjectHandle(void);

	bool isNULL();
	void release();

	AmfObjectHandle& operator=(const AmfObjectHandle &rhs);
	AmfObjectHandle& operator=(AMFObject* obj);
	bool operator==(const AmfObjectHandle &rhs) const;

	// 优先使用->操作
	AMFObject* operator->() const;
	AMFObject* getObject() const { return pObject_;}
private:
	AMFObject* pObject_;
};

typedef std::vector<AmfObjectHandle> Amf_Ref_Vec_Type;

//------------------------------------------------------
//AMFObject 元素数据对象
class AMFObject
{
	friend class AmfObjectHandle;
public:
	static AmfObjectHandle Alloc();
	void add_ref();
	void release();

protected:
	AMFObject();
	~AMFObject(void);
public:
	//更新节点状态为相应的数值，并设置节点类型,默认节点名称为空
	void initObject(double num,const char* name="");
	void initObject(int num,const char* name="");
	void initObject(bool bValue,const char* name="");
	void initObject(const char* value,const char* name="");

	// 添加子节点
	// 根据指定的类型添加子节点返回节点指针进行后续的操作
	AmfObjectHandle addChild(AMFDataType type,const char* name="");
	void addChild(const AmfObjectHandle& obj);
	void addChild(double value, const char* name="");
	void addChild(int value, const char* name="");
	void addChild(bool value, const char* name="");
	void addChild(const char* strValue, const char* name="");

	// date相关操作
	double getTimeSeed();
	void   setTimeSeed(double t);

	// 重置状态,并不删除子对象
	void clearValue();
	// 重置状态，删除子对象
	void clearValueWithChildren();

	// 设置当前对象为number，并更新type
	void setAsUnsignedNumber(unsigned int num);
	void setAsInt(int num);
	void setAsDouble(double num);

	// 名称为name的孩子节点是否存在
	bool hasChild(const char* name);
	// 获取名称为name的孩子节点,字符串比较区分大小写
	AmfObjectHandle getChild(const char* name);


	AMFDataType getType() const { return type_;}
	void setType(AMFDataType type) { type_ = type;}

	void setStringValue(std::string& value) { stringValue_ = value;}
	std::string& getStringValue() { return stringValue_;}
	
	std::string& getObjectName() { return objectName_;}
	void setObjectName(const std::string& name) { objectName_ = name;}
	
	int getIntValue() const { return intValue_;}
	void setIntValue(int value) { intValue_ = value;}
	
	double getDoubleValue()const { return doubleValue_;}
	void setDoubleValue(double value) { doubleValue_ = value;}
	
	bool getBoolValue() const { return boolValue_;}
	void setBoolValue(bool bValue) { boolValue_ = bValue;}

	std::string& getClassName() { return className_;}
	void setClassName(std::string& name) {className_ = name;}

	int getByteArrayLength() const { return byteArrayLength_;}
	void setByteArrayLength(int length) { byteArrayLength_ = length;}

	unsigned char* getByteArrayValue() const { return byteArrayValue_;}
	void setByteArrayValue(unsigned char* value) { byteArrayValue_ = value;}

	int getChildrenCount() const { return childrens_.size();}

	// 根据节点名称排序，主要是为了显示方便，一般不需要调用此方法
	void sortChildByName();
protected:
	std::string objectName_; 
	std::string stringValue_;
	std::string className_;
	double doubleValue_; 
	int intValue_;
	bool boolValue_;
	AMFDataType type_;
	//SYSTEMTIME dateValue;
	int byteArrayLength_;
	unsigned char* byteArrayValue_;
	long reference_;
public:
	std::vector<AmfObjectHandle> childrens_;
};


int read_data_callback(void* file,size_t size,unsigned char* buf);
void write_data_callback(void* file,const unsigned char* buf,size_t size);

///////////////////////////////////////////////////////////////////////////////
class AMFContext
{
public:
	AMFContext(Func_Read pRFunc=read_data_callback,Func_Write pWFunc=write_data_callback);
	~AMFContext();
public:
	void* inBufferHandle_;
	void* outBufferHandle_;
	Func_Read  readFunction_;
	Func_Write writeFunction_;
	unsigned char* readBuffer_;
	uint32 readBufferSize_;
	Amf_Ref_Vec_Type mReadObjectRefVec_;
	Amf_Ref_Vec_Type mReadStrRefVec_;
};

class IOBufferHelper
{
public:
	IOBufferHelper() : buffer_(NULL),bufferLength_(0),pos_(0){}
	~IOBufferHelper();

	void initFromBuffer(void* buffer,int length);
	void initFromString(const std::string& str);
	void mallocBuffer(int length);
	
	unsigned char* getBuffer(int pos=0) const { return buffer_+pos;}
	uint32 getBufferLength() const { return bufferLength_;}
	
	uint32 getPos() const { return pos_;}
	void setPos(int pos) { pos_=pos;}
	
	void incrementPosition(int offset) { pos_ += offset;}

	void write(const void * data, size_t bytes);
	uint32 getFreeSpace() const { return bufferLength_-pos_;}
	void trimBuffer(uint32 pos) { if(pos>bufferLength_) return; buffer_[pos]=0;}
private:
	unsigned char* buffer_;
	uint32 bufferLength_;
	uint32 pos_;
};

unsigned int DecodeHex(const char* pData,unsigned char* pOut,int inlen);
void DecodeHex(IOBufferHelper* io, const char* pData);

AmfObjectHandle g_decode(void* inBuffer);
void g_encode(void* outBuffer,AmfObjectHandle& obj);

}//end namespace ns_amf3
#endif
