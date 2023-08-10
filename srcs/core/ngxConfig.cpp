#include "ngxConfig.hpp"

const int ngxConfig::GetWorkerCount() const 
{
	return mWorkerCount;
}

const int ngxConfig::GetWorkerConnectionCount() const 
{
	return mWorkerConnectionCount;
}