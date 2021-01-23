#pragma once

#include <string>
#include <exception>

namespace EBCpp
{
    
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define EB_AT __FILE__ ":" TOSTRING( __LINE__ ) + std::string(" [")  + std::string( __FUNCTION__ ) + std::string( "]") 
#define EB_EXCEPTION( message ) throw EBCpp::EBException( std::string( message ) + "\n\t in " EB_AT )

class EBException : public std::exception
{
    public:
        EBException( std::string message ) : 
            std::exception(),
            message(message)
        {
            
        }

        std::string what()
        {
            return message;
        }

        virtual const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT
        {
            return message.c_str();
        }
    private:
        std::string message;
};

}