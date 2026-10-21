#ifndef GHG_H
#define GHG_H

/* Moyens de transport pris en charge. */
typedef enum {
    TRANSPORT_TRAIN,
    TRANSPORT_PLANE,
    TRANSPORT_CAR
} transport_mode_t;

/* Résultat d'un calcul d'émissions pour un trajet. */
typedef struct {
    transport_mode_t transport;
    double distance_km;
    double emissions_g; /* émissions en gCO2e */
} ghg_emission_t;

/* Codes de statut renvoyés par les fonctions de la bibliothèque. */
typedef enum {
    GHG_OK                    =  0,
    GHG_ERR_NULL_POINTER      = -1,
    GHG_ERR_INVALID_DISTANCE  = -2,
    GHG_ERR_UNKNOWN_TRANSPORT = -3
} ghg_status_t;

/*
 * Calcule les émissions de GES d'un trajet.
 *
 * distance  : distance parcourue en kilomètres (>= 0).
 * transport : moyen de transport utilisé.
 * emissions : structure qui reçoit le résultat ; non modifiée en cas d'erreur.
 *
 * Retour : GHG_OK en cas de succès, un code ghg_status_t négatif sinon.
 */
int CalculateGHGEmission(double distance, transport_mode_t transport, ghg_emission_t *emissions);

#endif /* GHG_H */
