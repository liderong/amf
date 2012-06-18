#include "amf3_serializer.h"
int main()
{
	//{a:"123",b:[1,"2",0.3],c:"汉字"}
	ns_amf3::AmfObjectHandle amf= ns_amf3::AMFObject::Alloc();
	amf->setType(ns_amf3::AMF3_OBJECT);
	amf->addChild("a",std::string("123"));
	ns_amf3::AmfObjectHandle child_array = amf->addChild("b",ns_amf3::AMF3_ARRAY);
	child_array->addChild("",(int)1);
	child_array->addChild("",std::string("2"));
	child_array->addChild("",(double)0.3);
	amf->addChild("c",std::string("汉字"));

	// 编码数据
	ns_amf3::IOBufferHelper out;
	out.mallocBuffer(4096);
	ns_amf3::g_encode(&out,amf);
	// 发送out.getBuffer(),长度out.getPos()

	// 解析获取的数据
	ns_amf3::IOBufferHelper receiveData;
	receiveData.initFromBuffer(out.getBuffer(),out.getPos());
	ns_amf3::AmfObjectHandle obj = ns_amf3::g_decode(&receiveData);
	// 解析得到的数据在obj中
	return 0;
}