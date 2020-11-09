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
    private:
        uint32_t datasize;
        std::unique_ptr<char> data;

    public:
        template <typename T>
        PrimativeObject(T& ob);
        template <typename T>
        PrimativeObject(T ob);
        template <typename T>
        T& GetObject();

        PrimativeObject(char* ptr, uint32_t datasize_);
        void PushToCharVec(std::vector<char>& cvec, std::string name);
        //~PrimativeObject();
    };

    class PrimativeArray
    {
    private:
        uint32_t datasize;
        uint16_t objectsize;
        std::unique_ptr<char> data; //The first 2 bytes represents object size

    public:
        template <typename T>
        PrimativeArray(std::vector<T>& objvec);
        template <typename T>
        PrimativeArray(std::vector<T> objvec);
        template <typename T>
        std::vector<T> GetObjectVec();

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
        Array(char* ptr,uint32_t dataSize);
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
        Object(char* ptr,uint32_t dataSize);
    };
} // namespace Serializer