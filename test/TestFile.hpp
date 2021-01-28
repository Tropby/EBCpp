#pragma once

#include "doctest/doctest/doctest.h"

#include "../src/EBFile.hpp"

TEST_CASE("testing binary file read") 
{
    std::ofstream out;
    out.open("test.txt");
    out << "Test a String\nWith a second line!";
    out.close();

    EBCpp::EBFile file(nullptr);
    file.setFileName("test.txt");
    file.open(EBCpp::EBFile::READ_ONLY);
    char buffer[1024];

    CHECK( file.read(buffer, sizeof(buffer)) == 33 );
    CHECK( file.atEnd() );
    CHECK( file.read(buffer, sizeof(buffer)) == 0 );

    file.close();
}
