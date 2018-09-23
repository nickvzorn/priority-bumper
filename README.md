# priority-bumper
This tool can make a running process use the Linux "real-time" scheduler with a desired priority and give that process specific CPU core affinity.

I wrote this to try to raise (bump) the priority of software processes/applications on personal Raspberry Pi projects such as a simple obstacle-avoidance robot. It might be useful to you on your projects too. Follow these steps if you would like to try it out.

1) Log into the machine/device. If you've logged in through the GUI, open a Terminal. If you've ssh'd in from elsewhere, you're already in a terminal.

2) (Perform this step if you wish to assign processes to specific CPUs...) Use Linux's built-in 'isolcpus' kernel boot parameter to constrain the operating system (OS) to do all of its normal stuff on only CPUs #0 and #1 (assuming a 4-core Raspberry Pi, for example). This means you will isolate, or cordon off, cores #2 and #3 for exclusive use by your own application(s). Follow this example to add the text 'isolcpus=2,3' to /boot/cmdline.txt. You will need to reboot after this.
    http://www.runeaudio.com/forum/the-new-4-core-raspberry-pi-t862-40.html#p5401

3) As superuser, install gcc, make, top and htop, if they're not already installed:

    sudo apt-get install gcc
    
    sudo apt-get install make
    
    sudo apt-get install top #this is a generally useful utility
    
    sudo apt-get install htop #this is another useful tool

4) Navigate into the directory where you have cloned priorityBumper code, such as
     /home/username/projects/priority-bumper/

5) To compile the application, issue the following two commands in succession:
    
    make clean
    
    make

6) Start up your own application(s), whose priority you'll want to bump.

7) Now run priorityBumper, like this:

    username@rpi01:~$ sudo /home/username/projects/priority-bumper/priorityBumper
    
    You will get some 'help' output that will tell you how to call it with appropriate arguments (which would come after 'priorityBumper' in the command above) in order to bump the priority of the stuff you started up in #6.

8) You should be able to start from step 6 each successive time you do this. Steps 2-4 won't need to be repeated, and step 5 will only need to be re-run if you alter the code.

Other notes:

If you ever want to see detail about all the processes running currently, run 'htop' in a terminal. It shows you process priority, memory/CPU usage and CPU core assignment, among other things.

You will probably want to disable the GUI just to take some of the load off of the two cores you've now tasked with doing all the OS stuff.
    http://ask.xmodulo.com/disable-desktop-gui-raspberry-pi.html

Other references I used in this process:
    http://man7.org/linux/man-pages/man7/cpuset.7.html
    http://man7.org/linux/man-pages/man2/sched_setscheduler.2.html
    http://man7.org/linux/man-pages/man2/sched_setaffinity.2.html
    https://unix.stackexchange.com/questions/204334/can-one-core-on-a-multicore-linux-system-be-dedicated-to-one-user-space-app

My hope is that this might be useful to others.
