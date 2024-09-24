#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Includes.hpp"

class Logger {
	private:
		std::ofstream _debugLog;
		std::ofstream _logAccess;
		std::ofstream _logError;

		/* Private Methods */
		std::string _currentDateTime( void ) const;
		bool _isTerminal( std::ostream &os ) const;

	public:
		/* Constructor Method */
		Logger( const std::string &log, const std::string &logAccess, 
			const std::string &logError );

		/* Destructor Method */
		~Logger( void );

		/* Public Methods */
		void logDebug( const std::string &severity,
			const std::string &message, bool tty = false );
		void logAccess( const std::string &severity,
			const std::string &message, bool tty = false );
		void logError( const std::string &severity,
			const std::string &message, bool tty = false );
		
};

#endif
