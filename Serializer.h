#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace Serializer
{
    const int HeaderSize = 4;
    const char StaticHeader[] = { 'S', 'A', 0x01, 0x00 };

    enum ObjectType : uint8_t
    {
        ObjectT = 0,
        ArrayT = 1,
        PrimativeObjectT = 2,
        PrimativeArrayT = 3
    };

    struct ObjectHeader
    {
        uint32_t ObjectSize; //Size of the object + objectname + object header
        uint8_t NameSize;    //Data comes after the name
        ObjectType type;
    };

    class PrimativeObject
    {
    private://TODO store the data in a char vector
        uint32_t datasize;
        std::shared_ptr<char> data;

    public:
        template <typename T>
        PrimativeObject(T& ob)
        {
            datasize = sizeof(T);
            data = std::shared_ptr<char>(sizeof(T));
            memcpy(data, &ob, sizeof(T));
        }
        template <typename T>
        T& GetObject()
        {
            if (sizeof(T) != datasize)
                throw "sizof T does not match with datasize";
            /*T r;
            memcpy(&r, data.get(), sizeof(T));*/
            T& r = *(T*)data.get();
            return r;
        }

        PrimativeObject(char* ptr, uint32_t datasize_);
        void PushToCharVec(std::vector<char>& cvec, std::string name);
        //~PrimativeObject();
    };

    class PrimativeArray
    {
    private://TODO store the data in a char vector
        uint32_t datasize;
        uint16_t objectsize;
        std::shared_ptr<char> data; //The first 2 bytes represents object size

    public:
        template <typename T>
        PrimativeArray(std::vector<T>& objvec)
        {
            datasize = sizeof(T) * objvec.size() + 2;
            objectsize = sizeof(T);
            data = std::shared_ptr<char>(datasize);
            memcpy(data.get() + 2, &objvec[0], sizeof(T) * objvec.size());
        }

        template <typename T>
        std::vector<T> GetObjectVec()
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

        PrimativeArray(char* ptr, uint32_t datasize_);
        void PushToCharVec(std::vector<char>& cvec, std::string name);
        //~PrimativeArray();
    };

    class Object;

    class Array
    {
    public:
        std::vector<Object> Objects;

    public:
        void PushToCharVec(std::vector<char>& cvec, std::string& name);
        Array(char* ptr, uint32_t dataSize);
        Array();
    };

    class Object
    {
    private:
        std::vector<std::pair<std::string, Object>> Objects;
        std::vector<std::pair<std::string, Array>> Arrays;
        std::vector<std::pair<std::string, PrimativeObject>> PObjects;
        std::vector<std::pair<std::string, PrimativeArray>> PArrays;

    public:
        void PushToCharVec(std::vector<char>& cvec, std::string& name);
        Object(char* ptr, uint32_t dataSize);
        Object();

        Object& GetObject(std::string& name);
        Object& GetObject(const char* nameptr);
        Array& GetArray(std::string& name);
        Array& GetArray(const char* nameptr);

        //Creates a new Object at vector and turns back a reference to it.
        inline Object& PushObject(const std::string& name, const Object& ob)
        {
            Objects.push_back(std::pair<std::string, Object>(name, ob));
            return Objects.back().second;
        }

        //Creates a new Object at vector and turns back a reference to it.
        inline Object& PushObject(const std::string& name)
        {
            Objects.push_back(std::pair<std::string, Object>(name, Object()));
            return Objects.back().second;
        }

        //Creates a new Array at vector and turns back a reference to it.
        inline Array& PushArray(const std::string& name, const Array& arr)
        {
            Arrays.push_back(std::pair<std::string, Array>(name, arr));
            return Arrays.back().second;
        }

        //Creates a new Array at vector and turns back a reference to it.
        inline Array& PushArray(const std::string& name)
        {
            Arrays.push_back(std::pair<std::string, Array>(name, Array()));
            return Arrays.back().second;
        }

        //const char*
        //Creates a new Object at vector and turns back a reference to it.
        inline Object& PushObject(const char* nameptr, const Object& ob)
        {
            Objects.push_back(std::pair<std::string, Object>(std::string(nameptr), ob));
            return Objects.back().second;
        }

        //Creates a new Object at vector and turns back a reference to it.
        inline Object& PushObject(const char* nameptr)
        {
            Objects.push_back(std::pair<std::string, Object>(std::string(nameptr), Object()));
            return Objects.back().second;
        }

        //Creates a new Array at vector and turns back a reference to it.
        inline Array& PushArray(const char* nameptr, const Array& arr)
        {
            Arrays.push_back(std::pair<std::string, Array>(std::string(nameptr), arr));
            return Arrays.back().second;
        }

        //Creates a new Array at vector and turns back a reference to it.
        inline Array& PushArray(const char* nameptr)
        {
            Arrays.push_back(std::pair<std::string, Array>(std::string(nameptr), Array()));
            return Arrays.back().second;
        }

        template<typename T>
        void PushPObject(const std::string& name, T& ob)
        {
            PObjects.push_back(std::pair<std::string, PrimativeObject>(name, PrimativeObject(ob)));
        }

        template<typename T>
        void PushPArray(const std::string& name, std::vector<T>& ob)
        {
            PArrays.push_back(std::pair<std::string, PrimativeArray>(name, PrimativeArray(ob)));
        }

        template<typename T>
        void PushPObject(const char* nameptr, T& ob)
        {
            PObjects.push_back(std::pair<std::string, PrimativeObject>(std::string(nameptr), PrimativeObject(ob)));
        }

        template<typename T>
        void PushPArray(const char* nameptr, std::vector<T>& ob)
        {
            PArrays.push_back(std::pair<std::string, PrimativeArray>(std::string(nameptr), PrimativeArray(ob)));
        }

        template<typename T>
        const T& GetPObject(std::string& name)
        {
            for (auto& p : PObjects)
            {
                if (p.first == name)
                    return p.second.GetObject<T>();
            }
            throw "Object with the given name : " + name + " couldn't found";
        }

        template<typename T>
        const T& GetPObject(const char* nameptr)
        {
            std::string name(nameptr);
            for (auto& p : PObjects)
            {
                if (p.first == name)
                    return p.second.GetObject<T>();
            }
            throw "Object with the given name : " + name + " couldn't found";
        }

        template<typename T>
        std::vector<T> GetPArray(std::string& name)
        {
            for (auto& p : PArrays)
            {
                if (p.first == name)
                    return p.second.GetObjectVec<T>();
            }
            throw "Object with the given name : " + name + " couldn't found";
        }

        template<typename T>
        std::vector<T> GetPArray(const char* nameptr)
        {
            std::string name(nameptr);
            for (auto& p : PArrays)
            {
                if (p.first == name)
                    return p.second.GetObjectVec<T>();
            }
            throw "Object with the given name : " + name + " couldn't found";
        }
    };
} // namespace Serializer