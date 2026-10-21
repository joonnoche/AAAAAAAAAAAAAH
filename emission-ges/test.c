#include <stdio.h>
#include "ghg.h"

int main(void)
{
    ghg_emission_t e;
    int status = CalculateGHGEmission(450, TRANSPORT_TRAIN, &e);
    printf("statut = %d, émissions = %.2f g\n", status, e.emissions_g);
    return 0;
}
