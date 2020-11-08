#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace Serializer
{
    const int HeaderSize = 4;
    const char StaticHeader[] = {'S', 'A', 0x01, 0x00};

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
    public://TODO Declare the functions
        template <typename T>
        PrimativeObject(T ob);
        template <typename T>
        PrimativeObject(char *ptr, uint32_t datasize);
        template <typename T>
        T GetObject();

        void PushToCharVec(std::vector<char>& cvec,std::string name);
        //~PrimativeObject();
    };

    class PrimativeArray
    {
    private:
        uint32_t datasize;
        uint16_t objectsize;
        std::unique_ptr<char> data; //The first 2 bytes represents object size
    public://TODO Declare the functions
        template <typename T>
        PrimativeArray(std::vector<T> objvec);
        template <typename T>
        PrimativeArray(char *ptr, uint32_t datasize);
        template <typename T>
        std::vector<T> GetObjectVec();

        void PushToCharVec(std::vector<char>& cvec,std::string name);
        //~PrimativeArray();
    };

    class Object;

    class Array
    {
        std::vector<Object> Objects;
    };

    class Object
    {
    private:
        std::map<std::string, Object> Objects;
        std::map<std::string, Array> Arrays;
        std::map<std::string, PrimativeObject> PObjects;
        std::map<std::string, PrimativeArray> PArrays;
    };
} // namespace Serializer