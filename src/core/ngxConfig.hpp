#ifndef NGXCONFIG_HPP
#define NGXCONFIG_HPP

#include "ngxCore.hpp"

class ngxConfig
{
	/* methods */
	public:
		
	protected:
	private:

	/* variables */
	protected:
	private:
		/* MainContext */
		int worker_processes = 4;

		/* EventsContext */
		int worker_connections = 1024;
		
		/* HttpContext */
		
		/* ServerContext */

		/* LocationContext */
};

#endif
