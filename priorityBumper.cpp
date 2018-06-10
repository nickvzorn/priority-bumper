/* 
 * priorityBumper.cpp
 * 
 * Copyright 2018 Nick V. Zorn <nickvzorn at gmail dot com>
 * 							   <github.com forwardslash nickvzorn>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 */

#include <string>
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <sched.h>

int main(int argc, char **argv)
{
	int argidx = 0;
	int ret_val = 0;
	int numpids, pididx;
	pid_t pid[128] = {0};
	char tempString[512] = {0};
	char *ptr1, *ptr2;
	
	// Display helper information
	printf("\n HELLO: Welcome to priorityBumper.\n");
	printf("\tWhen run as superuser, this application can bump one or more\n");
	printf("\trunning processes up to a desired priority while also assigning\n");
	printf("\tit/them to use Linux's SCHED_FIFO real-time scheduler. It can also\n");
	printf("\tassign the process(es) to a specific CPU core, such as one\n");
	printf("\tthat has already been cordoned off using 'isolcpus'...\n\n");
	printf("  INFO: if the application is a scripted one (shell/perl/python)\n");
	printf("\tALL processes that are scripted as such will all be bumped together... sorry.\n\n");
	printf("DANGER: With this utility, you might brick your device and need to reset. ");
	printf("PROCEED WITH CAUTION!\n\n");
	printf(" USAGE: sudo ./priorityBumper <arg1> <arg2> <arg3> | <arg4> <arg5> <arg6> ...|\n");
	printf("\t<arg1> = Application #1 name\n");
	printf("\t<arg2> = Application #1 desired real-time priority [1 min -> 99 max]\n");
	printf("\t<arg3> = Application #1 CPU assignment [0 -> (#CPUs-1), 999 = don't assign]\n");
	printf("\t\toptional arguments:\n");
	printf("\t<arg4> = Application #2 name\n");
	printf("\t<arg5> = Application #2 desired real-time priority (1 min -> 99 max]\n");
	printf("\t<arg6> = Application #2 CPU assignment [0 -> (#CPUs-1), 999 = don't assign]\n");
	printf("\t...\n\n");

	// Echo back the arguments sent in
	printf("  INFO: Arguments provided:\n");
	for( argidx=1; argidx<argc; argidx++ )
	{
		printf("\t<arg%d> = %s\n", argidx, argv[argidx]);
	}
	printf("\n");

	// If there aren't enough arguments supplied for all the applications
	// to be modified, quit and don't do any of it
	if ( ((argc-1) % 3) != 0 )
	{
		printf(" ERROR: Not enough arguments provided for all applications. Exiting.\n\n");
		return -1;
	}

	// Actually do stuff if you've made it this far
	for( argidx=1; argidx<argc; argidx=argidx+3 )
	{
		// Get the name of the process whose priority shall be bumped;
		// NOTE: if the application is a scripted one (shell/perl/python)
		// ALL processes that are scripted as such will all be bumped
		// together... sorry
		snprintf(tempString, 512, "pidof -x %s", argv[argidx]);
		//printf("DEBUG: tempString = %s\n", tempString);
		
		std::string command_string(tempString);
		memset((void *)tempString, 0x00, 512);
		FILE *return_string = popen(command_string.c_str(), "r");
		fgets((char *)tempString, 512, return_string);
		//printf("DEBUG: tempString = %s\n", tempString);
		
		// start getting PIDs out of the returned string (may be more than one)
		numpids = 0;
		pid[numpids] = strtoul(tempString, &ptr1, 10);
		if (pid[numpids] == 0)
		{
			printf(" ERROR: Couldn't find any PIDs for %s. Exiting.\n\n",
				argv[argidx]);
			return -1;
		}
		while (pid[numpids] != 0)
		{
			printf("  INFO: priorityBumper found PID %i for %s!\n\n", pid[numpids], argv[argidx]);
			numpids++;
			pid[numpids] = strtoul(ptr1, &ptr2, 10);
			ptr1 = ptr2;
		}
		pclose(return_string);
		memset((void *)tempString, 0x00, 512);
		
		for (pididx=0; pididx<numpids; pididx++)
		{
			// Set the scheduler for this process to SCHED_FIFO and assign
			// the desired real-time priority to it
			sched_param thread_sched_params;
			thread_sched_params.sched_priority = atoi(argv[argidx+1]);
			ret_val = sched_setscheduler(pid[pididx], SCHED_FIFO, &thread_sched_params);
			if (ret_val < 0)
			{
				printf(" ERROR: Couldn't sched_setscheduler for %s. Exiting.\n\n",
					argv[argidx]);
				return -1;
			}
			else
			{
				printf("  INFO: priorityBumper has set %s to priority %i\n\n",
					argv[argidx], atoi(argv[argidx+1]));
			}

			// If desired (as in, if the parameter is != 999), assign the
			// application or PID to a specific CPU - perhaps one that has
			// already been cordoned off using 'isolcpus'...
			if ((atoi(argv[argidx+2])) != 999)
			{
				cpu_set_t set_mask; // Declare a cpu_set_t bitmask
				CPU_ZERO(&set_mask); // Initialize it all to 0
				CPU_SET(atoi(argv[argidx+2]), &set_mask);// Set the mask's bit for the target core
				ret_val = sched_setaffinity(pid[pididx], sizeof(cpu_set_t), &set_mask); // Assign the PID to the core
				if (ret_val < 0)
				{
					printf(" ERROR: Couldn't sched_setaffinity for %s. Exiting.\n\n",
						argv[argidx]);
					return -1;
				}
				else
				{
					printf("  INFO: priorityBumper has assigned %s (PID %i) to CPU %i\n\n",
						argv[argidx], pid[pididx], atoi(argv[argidx+2]));

				}
			}
			else
			{
				printf("  INFO: priorityBumper has not assigned %s to a specific CPU\n\n",
					argv[argidx]);
			}

			pid[pididx] = 0;
			ret_val = 0;
		}
	}

	printf("  INFO: priorityBumper is exiting now.\n\n");
	return 0;
}
