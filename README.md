# Architecture Logicielle et Materielle des Ordinateurs

## Objectif

Calculer 

$$\sum_{i=0}^{N} \sqrt{abs(a_i)}$$ 

le plus rapidement possible



## Leviers d'acceleration

 - Alignement mémoire (pour le cache)
 - Vectorisation
 - Parallelisation
 
## Utilisation 

Exécuter le script à partir du makefile :
```
make run (nb_thread={int}) (N={int})
```

Arguments optionnels :

nb_tread = nombre de threads à exécuter

N = taille du vecteur a
