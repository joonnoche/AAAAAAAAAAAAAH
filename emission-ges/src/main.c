#define _GNU_SOURCE /* getopt_long n'est pas dans le C standard */

#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ghg.h"

#define PROGRAM_NAME "ges"
#define EXIT_USAGE 2

/* Masque de bits des transports sélectionnés. */
enum {
    TRANSPORT_FLAG_TRAIN = 1 << 0,
    TRANSPORT_FLAG_PLANE = 1 << 1,
    TRANSPORT_FLAG_CAR   = 1 << 2
};

/* Correspondance entre le nom donné en ligne de commande, le bit et le transport. */
typedef struct {
    const char *name;
    unsigned int flag;
    transport_mode_t mode;
} transport_info_t;

/* L'ordre de ce tableau est aussi l'ordre d'affichage. */
static const transport_info_t TRANSPORTS[] = {
    {"train", TRANSPORT_FLAG_TRAIN, TRANSPORT_TRAIN},
    {"plane", TRANSPORT_FLAG_PLANE, TRANSPORT_PLANE},
    {"car",   TRANSPORT_FLAG_CAR,   TRANSPORT_CAR}
};

#define NB_TRANSPORTS (sizeof(TRANSPORTS) / sizeof(TRANSPORTS[0]))

static void PrintUsage(FILE *stream)
{
    fprintf(stream, "Usage : %s DISTANCE [--transport=train,plane,car] [--help]\n", PROGRAM_NAME);
}

static void PrintHelp(void)
{
    PrintUsage(stdout);
    printf("\n"
           "Calcule les émissions de gaz à effet de serre d'un trajet.\n"
           "\n"
           "Arguments :\n"
           "  DISTANCE                distance parcourue en kilomètres (nombre positif)\n"
           "\n"
           "Options :\n"
           "  -t, --transport=LISTE   moyens de transport séparés par des virgules,\n"
           "                          parmi train, plane et car (défaut : train)\n"
           "  -h, --help              affiche cette aide et quitte\n");
}

/* Affiche une erreur d'usage sur stderr, suivie de la syntaxe. */
static void UsageError(const char *message, const char *value)
{
    fprintf(stderr, "%s: %s : %s\n", PROGRAM_NAME, message, value);
    PrintUsage(stderr);
}

/* Convertit text en distance positive. Renvoie 0 en cas de succès, -1 sinon. */
static int ParseDistance(const char *text, double *distance)
{
    char *end;
    double value;

    errno = 0;
    value = strtod(text, &end);
    if (end == text || *end != '\0' || errno != 0 || !isfinite(value) || value < 0.0) {
        return -1;
    }
    *distance = value;
    return 0;
}

/*
 * Ajoute à *mask les transports de la liste (ex. "train,car").
 * La chaîne est modifiée par strtok. Renvoie 0 en cas de succès, -1 sinon.
 */
static int ParseTransports(char *list, unsigned int *mask)
{
    char *token;
    size_t i;
    unsigned int found = 0;

    for (token = strtok(list, ","); token != NULL; token = strtok(NULL, ",")) {
        for (i = 0; i < NB_TRANSPORTS; i++) {
            if (strcmp(token, TRANSPORTS[i].name) == 0) {
                break;
            }
        }
        if (i == NB_TRANSPORTS) {
            UsageError("transport inconnu", token);
            return -1;
        }
        found |= TRANSPORTS[i].flag;
    }
    if (found == 0) {
        fprintf(stderr, "%s: liste de transports vide\n", PROGRAM_NAME);
        PrintUsage(stderr);
        return -1;
    }
    *mask |= found;
    return 0;
}

/* Vrai si arg ressemble à un nombre négatif (ex. "-12"), que getopt prendrait pour une option. */
static int IsNegativeNumber(const char *arg)
{
    return arg[0] == '-' && (isdigit((unsigned char)arg[1]) || arg[1] == '.');
}

int main(int argc, char *argv[])
{
    static const struct option long_options[] = {
        {"transport", required_argument, NULL, 't'},
        {"help",      no_argument,       NULL, 'h'},
        {NULL,        0,                 NULL, 0}
    };
    unsigned int mask = 0;
    double distance;
    int opt;
    int i;
    size_t t;
    int failed = 0;

    /* Une distance négative doit être signalée comme telle, pas comme une option inconnue. */
    for (i = 1; i < argc && strcmp(argv[i], "--") != 0; i++) {
        if (IsNegativeNumber(argv[i])) {
            UsageError("distance invalide", argv[i]);
            return EXIT_USAGE;
        }
    }

    while ((opt = getopt_long(argc, argv, "t:h", long_options, NULL)) != -1) {
        switch (opt) {
        case 't':
            if (ParseTransports(optarg, &mask) != 0) {
                return EXIT_USAGE;
            }
            break;
        case 'h':
            PrintHelp();
            return EXIT_SUCCESS;
        default:
            /* getopt_long a déjà écrit le message d'erreur sur stderr. */
            PrintUsage(stderr);
            return EXIT_USAGE;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "%s: distance manquante\n", PROGRAM_NAME);
        PrintUsage(stderr);
        return EXIT_USAGE;
    }
    if (optind + 1 < argc) {
        UsageError("argument en trop", argv[optind + 1]);
        return EXIT_USAGE;
    }
    if (ParseDistance(argv[optind], &distance) != 0) {
        UsageError("distance invalide", argv[optind]);
        return EXIT_USAGE;
    }

    if (mask == 0) {
        mask = TRANSPORT_FLAG_TRAIN;
    }

    for (t = 0; t < NB_TRANSPORTS; t++) {
        ghg_emission_t result;

        if ((mask & TRANSPORTS[t].flag) == 0) {
            continue;
        }
        if (CalculateGHGEmission(distance, TRANSPORTS[t].mode, &result) != GHG_OK) {
            fprintf(stderr, "%s: calcul impossible pour %s\n", PROGRAM_NAME, TRANSPORTS[t].name);
            failed = 1;
            continue;
        }
        printf("%-5s : %.2f kgCO2e\n", TRANSPORTS[t].name, result.emissions_g / 1000.0);
    }

    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
