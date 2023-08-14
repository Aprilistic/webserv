#ifndef NGXCORE_HPP
#define NGXCORE_HPP

#include <algorithm>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <csignal>
#include <cassert>

// External functs
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <string.h>
#include <sys/event.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

//Process Thread
#include "NgxProcess.hpp"
#include "NgxThread.hpp"

#define NGX_PROCESS_MASTER (0)
#define NGX_PROCESS_WORKER (1)




#endif
