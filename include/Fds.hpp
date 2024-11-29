#ifndef FDS_HPP
#define FDS_HPP

#include "Includes.hpp"

class Fds {
	private:
		std::vector<int> _serverFds;

	public:
		/* Constructor Method */
		Fds( void );

		/* Destructor Method */
		~Fds( void );

		/* Public Methods */
		void	addFdToServer( int fd );
		bool	isFdInServer( int fd );
};

#endif // FDS_HPP