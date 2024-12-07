#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
    #include <windows.h>
    #define CLEAR "cls"
    #define sleep(seconds) Sleep((seconds) * 1000)
#else
    #include <unistd.h>
    #define CLEAR "clear"
#endif
void getIPInfo(char *ip) {
    char buffer[512];
    char command[256];
    snprintf(command, sizeof(command), "curl -s https://ipinfo.io/%s/json", ip);
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        printf("Error retrieving information for IP: %s\n", ip);
        return;
    }
    printf("Information for IP %s:\n", ip);
    int dataAvailable = 0;
    while (fgets(buffer, sizeof(buffer) - 1, fp) != NULL) {
        printf("%s", buffer);
        dataAvailable = 1;
    }

    if (!dataAvailable) {
        printf("No data found. Ensure you have internet connectivity.\n");
    }

    pclose(fp);
}
void dnsLookup(char *domain) {
    char buffer[256];
    char command[256];
    snprintf(command, sizeof(command), "nslookup %s", domain);
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        printf("Error performing DNS lookup for %s\n", domain);
        return;
    }

    printf("DNS Lookup for domain %s:\n", domain);
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }
    pclose(fp);
}
void traceroute(char *target) {
    char command[256];
#ifdef _WIN32
    snprintf(command, sizeof(command), "tracert %s", target);
#else
    snprintf(command, sizeof(command), "traceroute %s", target);
#endif

    printf("Tracerouting %s...\n", target);
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        printf("Error executing traceroute for %s\n", target);
        return;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fp)) {
        printf("%s", buffer);
    }
    pclose(fp);
}
void logToConsole(const char *message) {
    time_t rawTime;
    struct tm *timeInfo;
    time(&rawTime);
    timeInfo = localtime(&rawTime);
    printf("[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
           timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday,
           timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec, message);
}

void scanOpenPorts() {
    char buffer[1024];
#ifdef _WIN32
    FILE *fp = popen("netstat -an | findstr LISTENING", "r");
#else
    FILE *fp = popen("ss -tunlp | grep LISTEN", "r");
#endif

    if (fp == NULL) {
        printf("Error scanning open ports.\n");
        return;
    }
    logToConsole("Open Ports:");
    while (fgets(buffer, sizeof(buffer), fp)) {
        printf("%s", buffer);
    }
    pclose(fp);
}
void networkInterfaces() {
    char buffer[1024];
#ifdef _WIN32
    FILE *fp = popen("ipconfig", "r");
#else
    FILE *fp = popen("ifconfig", "r");
#endif

    if (fp == NULL) {
        printf("Error displaying network interfaces.\n");
        return;
    }
    logToConsole("Network Interfaces:");
    while (fgets(buffer, sizeof(buffer), fp)) {
        printf("%s", buffer);
    }
    pclose(fp);
}
void hashPassword() {
    char password[256];
    printf("Enter a password to hash: ");
    scanf("%255s", password);
    char command[512];
#ifdef _WIN32
    snprintf(command, sizeof(command), "CertUtil -hashfile \"%s\" SHA256", password);
#else
    snprintf(command, sizeof(command), "echo -n \"%s\" | sha256sum", password);
#endif

    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        printf("Error hashing the password.\n");
        return;
    }
    char buffer[1024];
    printf("SHA-256 Hash of the password:\n");
    while (fgets(buffer, sizeof(buffer), fp)) {
        printf("%s", buffer);
    }
    pclose(fp);
}
void listFilesInDirectory() {
    char buffer[1024];
#ifdef _WIN32
    FILE *fp = popen("dir", "r");
#else
    FILE *fp = popen("ls -la", "r");
#endif

    if (fp == NULL) {
        printf("Error listing files in the directory.\n");
        return;
    }
    printf("Files in the current directory:\n");
    while (fgets(buffer, sizeof(buffer), fp)) {
        printf("%s", buffer);
    }
    pclose(fp);
}
void scanRemotePorts(char *target) {
    char command[256];
    int startPort, endPort;
    printf("Enter start port: ");
    scanf("%d", &startPort);
    printf("Enter end port: ");
    scanf("%d", &endPort);

#ifdef _WIN32
    snprintf(command, sizeof(command), "for /L %%p in (%d,1,%d) do (echo | telnet %s %%p >nul && echo Port %%p - OPEN)", startPort, endPort, target);
#else
    snprintf(command, sizeof(command), "nmap -p %d-%d %s", startPort, endPort, target);
#endif

    printf("Scanning ports %d to %d on %s...\n", startPort, endPort, target);
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        printf("Error scanning remote ports.\n");
        return;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fp)) {
        printf("%s", buffer);
    }
    pclose(fp);
}
void arpLog() {
    char command[128];
#ifdef _WIN32
    strcpy(command, "arp -a");
#else
    strcpy(command, "arp -n");
#endif
    printf("Logging ARP table...\n");
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        printf("Error logging ARP table.\n");
        return;
    }
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fp)) {
        printf("%s", buffer);
    }
    pclose(fp);
}

void displayHosts() {
    char command[128];
#ifdef _WIN32
    strcpy(command, "arp -a");
#else
    strcpy(command, "nmap -sn 192.168.1.0/24");
#endif

    printf("Scanning network for hosts...\n");
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        printf("Error displaying hosts on network.\n");
        return;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fp)) {
        printf("%s", buffer);
    }
    pclose(fp);
}
void firewallStatus() {
#ifdef _WIN32
    system("netsh advfirewall show allprofiles");
#else
    system("sudo ufw status");
#endif
}
void systemUptime() {
#ifdef _WIN32
    system("net stats workstation");
#else
    system("uptime");
#endif
}
void whoAmI() {
#ifdef _WIN32
    system("whoami && hostname");
#else
    system("whoami && hostname && uname -a");
#endif
}
void showRoute() {
#ifdef _WIN32
    system("route print");
#else
    system("route -n");
#endif
}
void listServices() {
#ifdef _WIN32
    system("net start");
#else
    system("service --status-all");
#endif
}
void listProcesses() {
#ifdef _WIN32
    system("tasklist");
#else
    system("ps aux");
#endif
}
void osInfo() {
#ifdef _WIN32
    system("systeminfo");
#else
    system("uname -a");
#endif
}
void showHelp() {
    printf("[AVAILABLE COMMANDS]\n");
    printf("exit                 - Exit the terminal\n");
    printf("getip <IP>           - Get information for the specified IP\n");
    printf("dns <domain>         - Perform DNS lookup for a domain\n");
    printf("traceroute <host>    - Perform traceroute\n");
    printf("scan                 - Scan open ports on the local machine\n");
    printf("network              - Display network interfaces\n");
    printf("hash                 - Hash a password\n");
    printf("list                 - List files in the current directory\n");
    printf("scanremote <host>    - Scan remote ports for open ports\n");
    printf("arplog               - Log ARP table\n");
    printf("hosts                - Display hosts on the local network\n");
    printf("firewall             - Show firewall status\n");
    printf("uptime               - Show system uptime\n");
    printf("whoami               - Display current user information\n");
    printf("route                - Show routing table\n");
    printf("services             - List services\n");
    printf("processes            - List running processes\n");
    printf("osinfo               - Show operating system info\n");
    printf("help                 - Show this help message\n");
}

void commandProcessing(char *command) {
    if (strcmp(command, "help") == 0) {
        showHelp();
    } else if (strcmp(command, "exit") == 0) {
        exit(0);
    } else if (strncmp(command, "ddos", 4) == 0) {
        runDDOS();
    } else if (strncmp(command, "getip ", 6) == 0) {
        getIPInfo(command + 6);
    } else if (strncmp(command, "dns ", 4) == 0) {
        dnsLookup(command + 4);
    } else if (strncmp(command, "traceroute ", 11) == 0) {
        traceroute(command + 11);
    } else if (strcmp(command, "scan") == 0) {
        scanOpenPorts();
    } else if (strcmp(command, "network") == 0) {
        networkInterfaces();
    } else if (strcmp(command, "hash") == 0) {
        hashPassword();
    } else if (strcmp(command, "list") == 0) {
        listFilesInDirectory();
    } else if (strncmp(command, "scanremote ", 11) == 0) {
        scanRemotePorts(command + 11);
    } else if (strcmp(command, "arplog") == 0) {
        arpLog();
    } else if (strcmp(command, "hosts") == 0) {
        displayHosts();
    } else if (strcmp(command, "firewall") == 0) {
        firewallStatus();
    } else if (strcmp(command, "uptime") == 0) {
        systemUptime();
    } else if (strcmp(command, "whoami") == 0) {
        whoAmI();
    } else if (strcmp(command, "route") == 0) {
        showRoute();
    } else if (strcmp(command, "services") == 0) {
        listServices();
    } else if (strcmp(command, "processes") == 0) {
        listProcesses();
    } else if (strcmp(command, "osinfo") == 0) {
        osInfo();
    } else {
        printf("Unknown command: %s\n", command);
    }
}

int main() {
    char command[50];
    printf("[SYSTEM] Booting Into AskoOS...\n");
    sleep(3);
#ifdef _WIN32
    system(CLEAR);
#else
    system(CLEAR);
#endif
    printf("[SYSTEM] AskoOS Kernel\n");
    printf("[SYSTEM] Type 'help' to get a list of commands\n");
    while (1) {
        printf("aOS@localhost:~$ ");
        if (fgets(command, sizeof(command), stdin) != NULL) {
            command[strcspn(command, "\n")] = 0;
            commandProcessing(command);
        } else {
            printf("[-] Error reading input.\n");
        }
    }

    return 0;
}
