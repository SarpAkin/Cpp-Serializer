#include "Serializer.h"

//includes

namespace Serializer
{
    template <typename T>
    PrimativeObject::PrimativeObject(T& ob)
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
    PrimativeArray::PrimativeArray(std::vector<T>& objvec)
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

    PrimativeObject::PrimativeObject(char* ptr, uint32_t datasize_)
    {
        datasize = datasize_;
        data = std::make_unique<char>(sizeof(datasize));
        memcpy(data.get(), ptr, datasize);
    }

    PrimativeArray::PrimativeArray(char* ptr, uint32_t datasize_)
    {
        datasize = datasize_;
        data = std::make_unique<char>(sizeof(datasize));
        memcpy(data.get(), ptr, datasize);
        objectsize = *(uint16_t*)data.get();
    }

    template <typename T>
    T& PrimativeObject::GetObject()
    {
        if (sizeof(T) != datasize)
            throw "sizof T does not match with datasize";
        /*T r;
        memcpy(&r, data.get(), sizeof(T));*/
        T& r = *(T*)data.get();
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
        char* endp = data.get() + datasize;
        for (char* ptr = data.get() + 2; ptr < endp; ptr += sizeof(T))
        {
            retv.push_back(*(T*)ptr);
        }
        return retv;
    }

    void PrimativeObject::PushToCharVec(std::vector<char>& cvec, std::string name)
    {
        ObjectHeader header;
        header.type = ObjectType::PrimativeObjectT;
        header.NameSize = name.size();
        header.ObjectSize = sizeof(ObjectHeader) + header.NameSize + datasize;
        cvec.reserve(cvec.size() + header.ObjectSize);
        cvec.insert(cvec.end(), (char*)&header, (char*)&header + sizeof(ObjectHeader));
        cvec.insert(cvec.end(), name.begin(), name.end());
        cvec.insert(cvec.end(), data.get(), data.get() + datasize);
    }

    void PrimativeArray::PushToCharVec(std::vector<char>& cvec, std::string name)
    {
        ObjectHeader header;
        header.type = ObjectType::PrimativeArrayT;
        header.NameSize = name.size();
        header.ObjectSize = sizeof(ObjectHeader) + header.NameSize + datasize;
        cvec.reserve(cvec.size() + header.ObjectSize);
        cvec.insert(cvec.end(), (char*)&header, (char*)&header + sizeof(ObjectHeader));
        cvec.insert(cvec.end(), name.begin(), name.end());
        cvec.insert(cvec.end(), data.get(), data.get() + datasize);
    }

    void Object::PushToCharVec(std::vector<char>& cvec, std::string& name)
    {
        std::vector<char> datavec;
        /*
        uint16_t elementCount =
            Objects.size() + Arrays.size() + PObjects.size() + PArrays.size();*/
            //datavec.insert(datavec.end(), (char*)&elementCount, (char*)&elementCount + 2);
        for (auto& p : Objects)
        {
            p.second.PushToCharVec(datavec, p.first);
        }
        for (auto& p : Arrays)
        {
            p.second.PushToCharVec(datavec, p.first);
        }
        for (auto& p : PObjects)
        {
            p.second.PushToCharVec(datavec, p.first);
        }
        for (auto& p : PArrays)
        {
            p.second.PushToCharVec(datavec, p.first);
        }
        ObjectHeader header;
        header.type = ObjectType::PrimativeArrayT;
        header.NameSize = name.size();
        header.ObjectSize = sizeof(ObjectHeader) + header.NameSize + datavec.size();
        cvec.reserve(cvec.size() + header.ObjectSize);
        cvec.insert(cvec.end(), (char*)&header, (char*)&header + sizeof(ObjectHeader));
        cvec.insert(cvec.end(), name.begin(), name.end());
        cvec.insert(cvec.end(), datavec.begin(), datavec.end());
    }

    void Array::PushToCharVec(std::vector<char>& cvec, std::string& name)
    {
        std::vector<char> datavec;
        //uint16_t elementCount = Objects.size();
        //datavec.insert(datavec.end(), (char*)&elementCount, (char*)&elementCount + 2);
        std::string blankString = std::string("");
        for (auto& p : Objects)
        {
            p.PushToCharVec(datavec, blankString);
        }
        ObjectHeader header;
        header.type = ObjectType::PrimativeArrayT;
        header.NameSize = name.size();
        header.ObjectSize = sizeof(ObjectHeader) + header.NameSize + datavec.size();
        cvec.reserve(cvec.size() + header.ObjectSize);
        cvec.insert(cvec.end(), (char*)&header, (char*)&header + sizeof(ObjectHeader));
        cvec.insert(cvec.end(), name.begin(), name.end());
        cvec.insert(cvec.end(), datavec.begin(), datavec.end());
    }

    Object::Object(char* ptr, uint32_t dataSize)
    {
        char* pend = ptr + dataSize;
        while (ptr < pend)
        {
            ObjectHeader header = *(ObjectHeader*)ptr;
            char* datap = ptr + sizeof(header) + header.NameSize;
            char* strp = ptr + sizeof(header);
            ptr += header.ObjectSize;
            uint32_t datasize_ = header.ObjectSize - (sizeof(header) + header.NameSize);
            std::string name = std::string(strp, strp + header.NameSize);
            switch (header.type)
            {
            case ObjectT:
                Objects.push_back(std::pair<std::string, Object>(name, Object(datap, datasize_)));
                break;
            case ArrayT:
                Arrays.push_back(std::pair<std::string, Array>(name, Array(datap, datasize_)));
                break;
            case PrimativeObjectT:
                PObjects.push_back(std::pair<std::string, PrimativeObject>(name, PrimativeObject(datap, datasize_)));
                break;
            case PrimativeArrayT:
                PArrays.push_back(std::pair<std::string, PrimativeArray>(name, PrimativeArray(datap, datasize_)));
                break;
            }
        }

    }

    Array::Array(char* ptr, uint32_t dataSize)
    {
        char* pend = ptr + dataSize;
        while (ptr < pend)
        {
            ObjectHeader header = *(ObjectHeader*)ptr;
            char* datap = ptr + sizeof(header) + header.NameSize;
            char* strp = ptr + sizeof(header);
            ptr += header.ObjectSize;
            uint32_t datasize_ = header.ObjectSize - (sizeof(header) + header.NameSize);
            Objects.push_back(Object(datap, datasize_));
        }
    }
} // namespace Serializer