#include <iostream>
#include <string>
#include <windows.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <wininet.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wininet.lib")

HANDLE hIcmpFile;
unsigned long ipaddr = INADDR_NONE;
DWORD dwRetVal = 0;
char SendData[32] = "Data Buffer";
LPVOID ReplyBuffer = NULL;
DWORD ReplySize = 0;

bool checkNetworkConnection(const std::string& ipAddress) {
    ipaddr = inet_addr(ipAddress.c_str());
    hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Unable to create ICMP file." << std::endl;
        return false;
    }
    ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
    ReplyBuffer = (VOID*)malloc(ReplySize);
    if (ReplyBuffer == NULL) {
        std::cerr << "Unable to allocate memory for reply buffer." << std::endl;
        return false;
    }
    dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), NULL, ReplyBuffer, ReplySize, 1000);
    if (dwRetVal != 0) {
        IcmpCloseHandle(hIcmpFile);
        free(ReplyBuffer);
        return true;
    }
    else {
        std::cerr << "ICMP echo request failed." << std::endl;
        IcmpCloseHandle(hIcmpFile);
        free(ReplyBuffer);
        return false;
    }
}

void resetNetworkAdapter() {
    system("ipconfig /release");
    system("ipconfig /renew");
}

bool checkInternetConnection() {
    return InternetCheckConnection(L"http://www.google.com", FLAG_ICC_FORCE_CONNECTION, 0);
}

void resetDNS() {
    system("ipconfig /flushdns");
    system("ipconfig /registerdns");
}

void runDiagnostic(const std::string& ipAddress) {
    if (!checkNetworkConnection(ipAddress)) {
        std::cout << "Network issue detected. Attempting to reset network adapter." << std::endl;
        resetNetworkAdapter();
        if (!checkInternetConnection()) {
            std::cout << "Internet connectivity issue detected. Resetting DNS." << std::endl;
            resetDNS();
        }
    }
    else {
        std::cout << "Network connection is stable." << std::endl;
    }
}

int main() {
    std::string ipAddress;
    int choice;

    do {
        std::cout << "Network Diagnostic Tool\n";
        std::cout << "1. Enter IP address for testing\n";
        std::cout << "2. Run diagnostics\n";
        std::cout << "3. Reset network adapter\n";
        std::cout << "4. Reset DNS settings\n";
        std::cout << "5. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        switch (choice) {
        case 1:
            std::cout << "Enter IP address: ";
            std::cin >> ipAddress;
            break;
        case 2:
            if (ipAddress.empty()) {
                std::cout << "No IP address entered. Please enter an IP address first.\n";
            }
            else {
                runDiagnostic(ipAddress);
            }
            break;
        case 3:
            resetNetworkAdapter();
            break;
        case 4:
            resetDNS();
            break;
        case 5:
            std::cout << "Exiting program.\n";
            break;
        default:
            std::cout << "Invalid choice, please try again.\n";
        }
    } while (choice != 5);

    return 0;
}
