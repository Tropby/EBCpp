#pragma once

#include "EBObject.hpp"

namespace EBCpp
{

class EBIODevice : public EBObject
{
public:
    EBIODevice() : EBObject(){}

    typedef enum { READ_ONLY, WRITE_ONLY, READ_WRITE} DIRECTION;

    virtual bool open( DIRECTION direction ) = 0;
    virtual bool isOpened() = 0;
    virtual bool close() = 0;

    virtual int write( char * data, int length ) = 0;
    virtual int write( std::string data ) = 0;

    virtual int read( char * data, int maxLength ) = 0;
    virtual std::string readLine() = 0;

    void setFileName( std::string fileName )
    {
        this->fileName = fileName;
    }

    std::string getFileName()
    {
        return fileName;
    }

private:
    std::string fileName;    
};

} // namespace EBCpp
