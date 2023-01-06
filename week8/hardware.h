#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

// Define structure of hardware
typedef struct hardware_t
{
    char sysname[50];
    char nodename[50];
    char release[50];
    char version[50];
    char machine[50];
    // char ip[50];
} hardware;

hardware make_hardware()
{
    // Read OS information and copy to hardware stucture
    hardware sys_info;
    struct utsname uts;
    uname(&uts);
    strcpy(sys_info.sysname, uts.sysname);
    strcpy(sys_info.nodename, uts.nodename);
    strcpy(sys_info.release, uts.release);
    strcpy(sys_info.version, uts.version);
    strcpy(sys_info.machine, uts.machine);
    return sys_info;
}

void copy_hardware(hardware *hardware2, hardware hardware1)
{
    strcpy((*hardware2).sysname, hardware1.sysname);
    strcpy((*hardware2).nodename, hardware1.nodename);
    strcpy((*hardware2).release, hardware1.release);
    strcpy((*hardware2).version, hardware1.version);
    strcpy((*hardware2).machine, hardware1.machine);
    // strcpy((*hardware2).ip, hardware1.ip);
}

void print_hardware(hardware hwr)
{
    printf("OS: %s\n", hwr.sysname);
    printf("Hostname: %s\n", hwr.nodename);
    printf("Release: %s\n", hwr.release);
    printf("Version: %s\n", hwr.version);
    printf("Machine: %s\n", hwr.machine);
    // printf("ip: %s\n", hwr.ip);
}

void save_log(char *ip_address, hardware hwr)
{
    printf("\n");
    print_hardware(hwr);
    char file_name[300];
    int i;
    // Replace "." symbol in ip_address to "_" symbol
    for (i = 0; i < strlen(ip_address); i++)
    {
        if (ip_address[i] == '.')
            ip_address[i] = '_';
    }
    // File name format: log_sysname_ip_address.txt
    strcpy(file_name, "log_");
    strcat(file_name, hwr.nodename);
    strcat(file_name, "_");
    strcat(file_name, ip_address);
    strcat(file_name, ".txt");

    // Restore true format of ip_address
    for (i = 0; i < strlen(ip_address); i++)
    {
        if (ip_address[i] == '_')
            ip_address[i] = '.';
    }
    FILE *f = fopen(file_name, "w");
    if (f != NULL)
    {
        fprintf(f, "Ip address: %s\n", ip_address);
        fprintf(f, "OS: %s\n", hwr.sysname);
        fprintf(f, "Hostname: %s\n", hwr.nodename);
        fprintf(f, "Release: %s\n", hwr.release);
        fprintf(f, "Version: %s\n", hwr.version);
        fprintf(f, "Machine: %s\n", hwr.machine);
        fclose(f);
        printf("\nSaved log in %s\n", file_name);
    }
    else
    {
        printf("Cannot open %s file!\n", file_name);
    }
}

#endif