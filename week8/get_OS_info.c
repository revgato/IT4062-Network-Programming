// Print OS information
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

int main()
{
    struct utsname uts;
    uname(&uts);
    printf("OS: %s\n", uts.sysname);
    printf("Hostname: %s\n", uts.nodename);
    printf("Release: %s\n", uts.release);
    printf("Version: %s\n", uts.version);
    printf("Machine: %s\n", uts.machine);
    return 0;
}