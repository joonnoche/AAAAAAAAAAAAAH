#include "ghg.h"

#include <stddef.h>

/* Facteurs d'émission de la Base Empreinte (ADEME), en gCO2e par km. */
#define FACTOR_TRAIN_G_PER_KM 2.93  /* par passager.km */
#define FACTOR_PLANE_G_PER_KM 259.0 /* par passager.km */
#define FACTOR_CAR_G_PER_KM   218.0 /* par véhicule.km, sans covoiturage */

/* Renvoie le facteur du transport dans *factor, ou une erreur s'il est inconnu. */
static int GetEmissionFactor(transport_mode_t transport, double *factor)
{
    switch (transport) {
    case TRANSPORT_TRAIN:
        *factor = FACTOR_TRAIN_G_PER_KM;
        return GHG_OK;
    case TRANSPORT_PLANE:
        *factor = FACTOR_PLANE_G_PER_KM;
        return GHG_OK;
    case TRANSPORT_CAR:
        *factor = FACTOR_CAR_G_PER_KM;
        return GHG_OK;
    default:
        return GHG_ERR_UNKNOWN_TRANSPORT;
    }
}

int CalculateGHGEmission(double distance, transport_mode_t transport, ghg_emission_t *emissions)
{
    double factor;
    int status;

    if (emissions == NULL) {
        return GHG_ERR_NULL_POINTER;
    }
    /* Écrit ainsi pour rejeter aussi NaN, pour qui toute comparaison est fausse. */
    if (!(distance >= 0.0)) {
        return GHG_ERR_INVALID_DISTANCE;
    }

    status = GetEmissionFactor(transport, &factor);
    if (status != GHG_OK) {
        return status;
    }

    /* *emissions n'est écrit qu'une fois toutes les vérifications passées. */
    emissions->transport = transport;
    emissions->distance_km = distance;
    emissions->emissions_g = distance * factor;
    return GHG_OK;
}
