#include "amf3_serializer.h"
#include <iostream>

int main()
{
	//{a:"123",b:[1,"2",0.3],c:"����"}
	ns_amf3::AmfObjectHandle amf= ns_amf3::AMFObject::Alloc();
	amf->setType(ns_amf3::AMF3_OBJECT);
	amf->setClassName("test_class_name");
	amf->addChild("123","a");
	ns_amf3::AmfObjectHandle child_array = amf->addChildWithType(ns_amf3::AMF3_ARRAY,"b");
	child_array->addChild(1);
	child_array->addChild("2");
	child_array->addChild(0.3);
	amf->addChild(std::string("����").c_str(),"c");

	// ��������
	ns_amf3::IOBufferHelper out;
	out.mallocBuffer(4096);
	ns_amf3::g_encode(&out,amf);
	// ����out.getBuffer(),����out.getPos()

	// ������ȡ������
	ns_amf3::IOBufferHelper receiveData;
	receiveData.initFromBuffer(out.getBuffer(),out.getPos());
	ns_amf3::AmfObjectHandle obj = ns_amf3::g_decode(&receiveData);

	ns_amf3::AmfObjectHandle obj_chs = obj->getChild("c");

	std::cout<<obj_chs->getStringValue()<<std::endl;
	// �����õ���������obj��

	// ����16��������
	ns_amf3::IOBufferHelper outhex;
	const char* hexdata = "00030000000100046e756c6c";
	DecodeHex(&outhex,hexdata);

	return 0;
}