#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // Pour sleep()

int main() {
    int nombre_secret, guess;
    const char *nom_dossier = "System32"; // Nom du dossier à supprimer

    // Initialisation du générateur de nombres aléatoires
    srand(time(NULL));
    nombre_secret = rand() % 6 + 1; // Nombre aléatoire entre 1 et 6

    printf("Bienvenue dans la roulette russe !\n");
    printf("Devinez un nombre entre 1 et 6 pour survivre : ");
    scanf("%d", &guess);

    if (guess == nombre_secret) {
        printf("Félicitations ! Vous avez survécu !\n");
    } else {
        printf("Dommage ! Le nombre était %d. Vous avez perdu...\n", nombre_secret);
        printf("Suppression du dossier %s dans...\n", nom_dossier);

        // Compte à rebours avant la suppression
        for (int i = 3; i > 0; i--) {
            printf("%d...\n", i);
            sleep(1); // Pause d'une seconde
        }

        // Suppression du dossier
        if (rmdir(nom_dossier) == 0) {
            printf("Le dossier %s a été supprimé avec succès.\n", nom_dossier);
        } else {
            perror("Erreur lors de la suppression du dossier");
        }
    }

    return 0;
}
