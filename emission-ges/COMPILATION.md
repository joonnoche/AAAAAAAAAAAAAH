# Compilation (partie 1)

Toutes les commandes se lancent depuis le dossier `ges/`.

## Vérifier que le cœur compile

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -Iinclude -c src/ghg.c -o ghg.o
```

- `-Iinclude` : cherche `ghg.h` dans le dossier `include/`.
- `-c` : compile sans faire l'édition de liens et produit `ghg.o`.

Aucun message affiché = le code compile proprement.

## Compiler et lancer le programme d'essai

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -Iinclude src/ghg.c test.c -o test
./test
```

Résultat attendu :

```
statut = 0, émissions = 1318.50 g
```

## Compiler et lancer le programme `ges` (partie 2)

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -Iinclude src/ghg.c src/main.c -o ges
./ges 450 --transport=train,plane,car
./ges 450
./ges --help
```

Pour voir le code de sortie de la dernière commande : `echo $?`.

## Nettoyer

```bash
rm -f ghg.o test ges
```
