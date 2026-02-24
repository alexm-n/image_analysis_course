# 🪙 Euro Coin Detection & Classification (OpenCV)

Ce projet est un outil de vision par ordinateur développé en **C++** avec **OpenCV**. Il permet de détecter, classifier et calculer la valeur totale de pièces de monnaie en Euro à partir d'images, avec une gestion spécifique des fonds complexes (textures, marbre, reflets).



## 🌟 Points Forts

- **Analyse de Texture Adaptative** : L'algorithme calcule l'écart-type (`StdDev`) des niveaux de gris pour différencier un fond lisse d'un fond texturé (ex: marbre).
- **Filtrage Intelligent** : 
  - **Bilateral Filter** : Utilisé sur les fonds texturés pour gommer le "bruit" du marbre tout en préservant les bords nets des pièces.
  - **Median Blur** : Élimine le grain sur les fonds intermédiaires.
- **Classification Colorimétrique HSV** : Distinction des trois familles de métaux (Cuivre, Or, Bi-métal) via l'espace de couleur HSV, plus stable que le RGB face aux variations d'éclairage.
- **Post-traitement rigoureux** : 
  - Suppression des cercles superposés (logique de Non-Maximum Suppression).
  - Suppression des parasites basée sur le ratio de taille par rapport à la plus grande pièce détectée.

---

## 🛠️ Pipeline de Traitement

### 1. Prétraitement & Segmentation
L'image est redimensionnée pour optimiser les calculs. Selon la valeur de `StdDev` du fond :
- **Mode Marbre** : Filtrage bilatéral agressif + seuils de Hough stricts.
- **Mode Uni** : Lissage gaussien + détection sensible pour capturer les petites pièces (1 ct, 2 cts).



### 2. Détection de Cercles
Utilisation de la **Transformée de Hough Circulaire** avec des paramètres dynamiques (`param1` pour Canny et `param2` pour l'accumulateur) afin de minimiser les faux positifs sur les veines du marbre.

### 3. Classification
Chaque pièce détectée est analysée :
1. **Couleur** : Extraction de la moyenne HSV sur un masque réduit (55% du centre) pour éviter les reflets des bords.
2. **Taille** : Calcul du ratio de rayon par rapport à la plus grande pièce de l'image.
3. **Identification** : Logique conditionnelle pour mapper les caractéristiques vers les valeurs (0.01€ à 2.00€).



---

## 💻 Installation & Compilation

### Prérequis
- **OpenCV 4.x**
- Compilateur **g++** supportant le **C++17** (pour `<filesystem>`)

### Compiler sur Linux
```bash
g++ -std=c++17 main.cpp -o CoinDetector `pkg-config --cflags --libs opencv4`
