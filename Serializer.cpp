#include "Serializer.h"

//includes

namespace Serializer
{


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
        header.type = ObjectType::ObjectT;
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

    Object& Object::GetObject(std::string& name)
    {
        for (auto& p : Objects)
        {
            if (p.first == name)
                return p.second;
        }
        throw "Object with the given name : " + name + " couldn't found";
    }
    Object& Object::GetObject(const char* nameptr)
    {
        std::string name(nameptr);
        for (auto& p : Objects)
        {
            if (p.first == name)
                return p.second;
        }
        throw "Object with the given name : " + name + " couldn't found";
    }
    Array& Object::GetArray(std::string& name)
    {
        for (auto& p : Arrays)
        {
            if (p.first == name)
                return p.second;
        }
        throw "Object with the given name : " + name + " couldn't found";
    }
    Array& Object::GetArray(const char* nameptr)
    {
        std::string name(nameptr);
        for (auto& p : Arrays)
        {
            if (p.first == name)
                return p.second;
        }
        throw "Object with the given name : " + name + " couldn't found";
    }

    Object::Object()
    {
        //do nothing
    }

    Array::Array()
    {
        //do nothing
    }

    std::vector<char> ParseToCharVector(Object& root)
    {
        std::vector<char> datavec(std::begin(StaticHeader), std::end(StaticHeader));
        //datavec.insert(datavec.end(),std::begin(StaticHeader),std::end(StaticHeader));
        std::string rootStr("root");
        root.PushToCharVec(datavec, rootStr);
        //TODO Add footer
        return datavec;
    }

    void ParseToObject(Object& root, std::vector<char>& stream)
    {
        char* ptr = &(*(stream.begin() + sizeof(StaticHeader)));
        ObjectHeader header = *(ObjectHeader*)ptr;
        char* datap = ptr + sizeof(header) + header.NameSize;
        uint32_t datasize_ = header.ObjectSize - (sizeof(header) + header.NameSize);
        root = Object(datap, datasize_);
        //TODO Add footer
    }
} // namespace Serializer