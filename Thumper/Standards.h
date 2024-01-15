#pragma once

// for h files
/*
Author: Nathan Dunn
Module: module name

Functional description and intended use

Known bugs and limitations:
- a
- b
- c
*/

// for cpp files
/*
Author: Nathan Dunn
Module: module name

See associated header file for more information
*/

// includes: windows includes (WinWrapper.h), then C++ standard libraries, then external dependencies, then other module dependencies, then in module dependencies

// forward declare classes at the beginning of every header file
// use the namespace for that module
namespace EX
{
	class EXClass; 
}

// every class declares members in a specific order: public types, private types, public member functions, private member functions, public variables, private variables
class EX::EXClass
{
public:
	// public member types

private:
	// private member types

public:
	// public member functions
	// constructors first, then destructors, then everything else

private:
	// private member functions

public:
	// public member variables

private:
	// private member variables

};

// before functions
/*
Description of the function and its intended use

Arguments:
a		description
b		description
c		description

Return:
a		description

Safeties and known issues:
- a
- b
- c

*/

// Naming
/*
For variables:
firstSecond_modifier

For functions, name spaces, enums:
FirstSecond_Modifier

Acronyms and shortened words are always all caps
*/
