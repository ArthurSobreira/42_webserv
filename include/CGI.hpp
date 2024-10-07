#ifndef CGI_HPP
#define CGI_HPP

#include "Includes.hpp"
#include "Defines.hpp"

class CGI {
	// private:
	// 	int attribute;
		
	public:
		/* Constructor Method */
		CGI( void );
		
		/* Copy Constructor Method */
		CGI( const CGI &other );
		
		/* Copy Assignment Operator Overload */
		CGI &operator=( const CGI &other );
		
		/* Destructor Method */
		~CGI( void );
		
		/* Public Methods */
		// int    getAttribute( void ) const;
		// void   setAttribute( int value );
		
};

#endif
