# Coin Detection & Classification (OpenCV)

Ce projet est un outil de vision par ordinateur développé en **C++** avec **OpenCV**. Il permet de détecter, classifier et calculer la valeur totale de pièces de monnaie en Euro à partir d'images, avec une gestion spécifique des fonds complexes (textures, marbre, reflets).



## Fonctionnalité

- **Analyse de Texture Adaptative** : L'algorithme calcule l'écart-type (StdDev) des niveaux de gris pour différencier un fond lisse d'un fond texturé (ex: marbre).
- **Filtrage** :
    - **Bilateral Filter** : Utilisé sur les fonds texturés pour gommer le "bruit" du marbre tout en préservant les bords nets des pièces.
    - **Median Blur/Gaussian Blur** : Élimine le grain sur les fonds plus lisses.
- **Classification Colorimétrique HSV** : Distinction des trois familles de métaux (Cuivre, Or, Bi-métal) via l'espace de couleur HSV, plus stable que le RGB face aux variations d'éclairage.
- **Post-traitement rigoureux** :
    - Suppression des cercles superposés (logique de Non-Maximum Suppression).
    - Suppression des parasites basée sur le ratio de taille par rapport à la plus grande pièce détectée.

