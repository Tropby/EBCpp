#pragma once

#include <map>
#include <string>

#include "../EBObject.hpp"
#include "../EBUtils.hpp"

namespace EBCpp
{

/**
 * @brief Class that parses the http header
 * 
 */
class EBHttpHeader : public EBObject
{
public:
    /**
     * @brief Construct a new EBHttpHeader object
     * 
     * @param parent Parent object
     */
    EBHttpHeader( EBObject* parent ) :
        EBObject(parent)
    {

    }

    /**
     * @brief Parse a header line
     * 
     * @param line One line of the Http header
     */
    void processLine( std::string line )
    {
        std::string key = EBUtils::trim( line.substr( 0 , line.find(':') ) );
        std::string value = EBUtils::trim( line.substr( line.find(':') + 1 ) );
        values[key] = value;
    }

    /**
     * @brief Get a value of an header key
     * 
     * @param key Header key
     * @return std::string Value of the key
     */
    std::string getValue( const std::string& key )
    {
        return values[key];
    }

    /**
     * @brief Checks if the header contains the key
     * 
     * @param key searched key
     * @return true if the header contains the key
     * @return false if the header does not contain the key
     */
    bool contains( const std::string&  key ) const
    {
        return values.find( key ) != values.end();
    }

    /**
     * @brief Get the header as string to send to a client
     * 
     * @return std::string The header as string representation
     */
    std::string getHeader() const
    {
        std::string result = "";
        for( auto it : values )
        {
            result += it.first + ": " + it.second + "\r\n";
        }
        return result;
    }

private:
    std::map<std::string, std::string> values;

};

}