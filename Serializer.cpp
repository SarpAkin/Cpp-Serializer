#include "Serializer.h"

//includes

namespace Serializer
{
    template <typename T>
    PrimativeObject::PrimativeObject(T &ob)
    {
        datasize = sizeof(T);
        data = std::make_unique<char>(sizeof(T));
        memcpy(data, &ob, sizeof(T));
    }

    template <typename T>
    PrimativeObject::PrimativeObject(T ob)
    {
        datasize = sizeof(T);
        data = std::make_unique<char>(sizeof(T));
        memcpy(data.get(), &ob, sizeof(T));
    }

    template <typename T>
    PrimativeArray::PrimativeArray(std::vector<T> &objvec)
    {
        datasize = sizeof(T) * objvec.size() + 2;
        objectsize = sizeof(T);
        data = std::make_unique<char>(datasize);
        memcpy(data.get() + 2, &objvec[0], sizeof(T) * objvec.size());
    }

    template <typename T>
    PrimativeArray::PrimativeArray(std::vector<T> objvec)
    {
        datasize = sizeof(T) * objvec.size() + 2;
        objectsize = sizeof(T);
        data = std::make_unique<char>(datasize);
        memcpy(data.get() + 2, &objvec[0], sizeof(T) * objvec.size());
    }

    PrimativeObject::PrimativeObject(char *ptr, uint32_t datasize_)
    {
        datasize = datasize_;
        data = std::make_unique<char>(sizeof(datasize));
        memcpy(data.get(), ptr, datasize);
    }

    PrimativeArray::PrimativeArray(char *ptr, uint32_t datasize_)
    {
        datasize = datasize_;
        data = std::make_unique<char>(sizeof(datasize));
        memcpy(data.get(), ptr, datasize);
        objectsize = *(uint16_t *)data.get();
    }

    template <typename T>
    T PrimativeObject::GetObject()
    {
        if (sizeof(T) != datasize)
            throw "sizof T does not match with datasize";
        T r;
        memcpy(&r, data.get(), sizeof(T));
        return r;
    }

    template <typename T>
    std::vector<T> PrimativeArray::GetObjectVec()
    {
        if ((datasize - 2) % sizeof(T) != 0)
            throw "sizof T does not match with datasize";
        int elementAmount = (datasize - 2) / objectsize;
        std::vector<T> retv;
        retv.reserve(elementAmount);
        char *endp = data.get() + datasize;
        for (char *ptr = data.get() + 2; ptr < endp; ptr += sizeof(T))
        {
            retv.push_back(*(T *)ptr);
        }
        return retv;
    }

    void PrimativeObject::PushToCharVec(std::vector<char> &cvec, std::string name)
    {
        ObjectHeader header;
        header.type = ObjectType::PrimativeObjectT;
        header.NameSize = name.size();
        header.ObjectSize = sizeof(ObjectHeader) + header.NameSize + datasize;
        cvec.reserve(cvec.size() + header.ObjectSize);
        cvec.insert(cvec.end(), (char *)&header, (char *)&header + sizeof(ObjectHeader));
        cvec.insert(cvec.end(), name.begin(), name.end());
        cvec.insert(cvec.end(), data.get(), data.get() + datasize);
    }

    void PrimativeArray::PushToCharVec(std::vector<char> &cvec, std::string name)
    {
        ObjectHeader header;
        header.type = ObjectType::PrimativeArrayT;
        header.NameSize = name.size();
        header.ObjectSize = sizeof(ObjectHeader) + header.NameSize + datasize;
        cvec.reserve(cvec.size() + header.ObjectSize);
        cvec.insert(cvec.end(), (char *)&header, (char *)&header + sizeof(ObjectHeader));
        cvec.insert(cvec.end(), name.begin(), name.end());
        cvec.insert(cvec.end(), data.get(), data.get() + datasize);
    }
} // namespace Serializer