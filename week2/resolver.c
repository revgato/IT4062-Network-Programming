#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Reference: 
// http://www.gnu.org/software/libc/manual/html_node/Host-Names.html
// https://www.gta.ufrj.br/ensino/eel878/sockets/inet_ntoaman.html
// https://www.gta.ufrj.br/ensino/eel878/sockets/gethostbynameman.html


// Check input string is ip address or domain name

int is_ip_address(char *str)
{
    int i, dot_count = 0;
    for (i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '.')
            dot_count++;
        else if (str[i] < '0' || str[i] > '9')
            return 0;
    }
    if (dot_count != 3)
        return 0;
    return 1;
}


// Get host from ip address

char *get_host_from_ip(char *ip)
{
    struct hostent *host;
    struct in_addr addr;
    inet_aton(ip, &addr);
    host = gethostbyaddr(&addr, sizeof(addr), AF_INET);
    return host;
}

// Get host from domain name

char *get_host_from_domain(char *domain)
{
    struct hostent *host;
    host = gethostbyname(domain);
    return host;
}

int main(int argc, char *argv[]){
    struct hostent *host;

    if (argc != 2)
    {
        printf("Usage: %s <ip address or domain name>\n", argv[0]);
        return 1;
    }

    if (is_ip_address(argv[1]))
    {
        host = get_host_from_ip(argv[1]);
        if (host == NULL)
        {
            printf("Not found information\n");
            return 1;
        }
        printf("Official name: %s\n", host->h_name);

        // Print all alias name
        printf("Alias name: \n");
        int i = 0;
        while (host->h_aliases[i] != NULL)
        {
            printf("%s\n", host->h_aliases[i]);
            i++;
        }

    }
    else
    {
        host = get_host_from_domain(argv[1]);
        if (host == NULL)
        {
            printf("Not found information\n");
            return 1;
        }
        printf("Official IP: %s\n", inet_ntoa(*((struct in_addr *)host->h_addr)));
        
        // Print all alias IP
        printf("Alias IP: \n");
        int i = 1;
        while (host->h_addr_list[i] != NULL)
        {
            printf("%s\n", inet_ntoa(*((struct in_addr *)host->h_addr_list[i])));
            i++;
        }
    }

    return 0;
}
 