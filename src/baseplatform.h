#ifndef BASEPLATFORM_H__
#define BASEPLATFORM_H__

#define STR(x) (((string)(x)).c_str())

#define MAP_HAS1(m,k) ((bool)((m).find((k))!=(m).end()))
#define MAP_HAS2(m,k1,k2) ((MAP_HAS1((m),(k1))==true)?MAP_HAS1((m)[(k1)],(k2)):false)
#define MAP_HAS3(m,k1,k2,k3) ((MAP_HAS1((m),(k1)))?MAP_HAS2((m)[(k1)],(k2),(k3)):false)

#define FOR_MAP(m,k,v,i) for(map< k , v >::iterator i=(m).begin();i!=(m).end();i++)
#define MAP_KEY(i) ((i)->first)
#define MAP_VAL(i) ((i)->second)

#define MAP_ERASE1(m,k) if(MAP_HAS1((m),(k))) (m).erase((k));
#define MAP_ERASE2(m,k1,k2) \
if(MAP_HAS1((m),(k1))){ \
    MAP_ERASE1((m)[(k1)],(k2)); \
    if((m)[(k1)].size()==0) \
        MAP_ERASE1((m),(k1)); \
}
#define MAP_ERASE3(m,k1,k2,k3) \
if(MAP_HAS1((m),(k1))){ \
    MAP_ERASE2((m)[(k1)],(k2),(k3)); \
    if((m)[(k1)].size()==0) \
        MAP_ERASE1((m),(k1)); \
}

#define FOR_VECTOR(v,i) for(uint32_t i=0;i<(v).size();i++)
#define FOR_VECTOR_ITERATOR(e,v,i) for(vector<e>::iterator i=(v).begin();i!=(v).end();i++)
#define FOR_VECTOR_WITH_START(v,i,s) for(uint32_t i=s;i<(v).size();i++)
#define ADD_VECTOR_END(v,i) (v).push_back((i))
#define ADD_VECTOR_BEGIN(v,i) (v).insert((v).begin(),(i))
#define VECTOR_VAL(i) (*(i))

#define MAKE_TAG8(a,b,c,d,e,f,g,h) ((uint64_t)(((uint64_t)(a))<<56)|(((uint64_t)(b))<<48)|(((uint64_t)(c))<<40)|(((uint64_t)(d))<<32)|(((uint64_t)(e))<<24)|(((uint64_t)(f))<<16)|(((uint64_t)(g))<<8)|((uint64_t)(h)))
#define MAKE_TAG7(a,b,c,d,e,f,g) MAKE_TAG8(a,b,c,d,e,f,g,0)
#define MAKE_TAG6(a,b,c,d,e,f) MAKE_TAG7(a,b,c,d,e,f,0)
#define MAKE_TAG5(a,b,c,d,e) MAKE_TAG6(a,b,c,d,e,0)
#define MAKE_TAG4(a,b,c,d) MAKE_TAG5(a,b,c,d,0)
#define MAKE_TAG3(a,b,c) MAKE_TAG4(a,b,c,0)
#define MAKE_TAG2(a,b) MAKE_TAG3(a,b,0)
#define MAKE_TAG1(a) MAKE_TAG2(a,0)

#endif /* BASEPLATFORM_H__ */

