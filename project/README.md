# Projet Vision : Détection, Identification et Comptage de Pièces d'Euros

**Contexte** : Le projet repose sur l'analyse d'images représentant un ensemble de pièces de monnaie en euros. Les photographies ont été capturées par un appareil mobile standard (téléphone) en vue de dessus (plongée). Les pièces sont disposées sur une table présentant des fonds de natures diverses (homogènes, texturés, à motifs).
**Objectif** : Concevoir et implémenter un algorithme de vision par ordinateur capable, à partir d'une simple image en entrée, de :
- **Détecter et compter** le nombre total de pièces présentes.
- **Identifier** la valeur de chaque pièce.
- **Estimer** la somme totale en euros représentée sur l'image.
  
# Méthodologie
L'approche finale repose sur un pipeline de traitement adaptatif qui ajuste ses paramètres en fonction de la complexité de l'image.
- **Segmentation adaptative en fonction de la texture**
  Au lieu d'un filtrage statique, le système analyse d'abord la scène :
  - **Ecart-type (StdDev)** : Définition : Mesure statistique de la dispersion des niveaux de gris.
    - Si StdDev est grand, application d'un **Filtre Bilatéral** (lissage préservant les contours) pour ignorer les veines du marbre.
    - Si StdDev est moyen, combinaison d'un Filtre Médian (élimination du bruit "poivre et sel") et d'un Flou Gaussien.
    - si StdDev est petit, lissage léger pour optimiser la détection.

- **Détection de cercles (HoughCircles)**
**Transformée de Hough Circulaire** : Définition : Algorithme de détection de formes basé sur l'accumulation de votes dans un espace paramétrique. Le système utilise des paramètres de sensibilité ajustés selon le mode détecté.

- **Classification par Consensus de Ratio**
C'est le cœur du système. Plutôt que de mesurer une taille fixe en pixels (qui change selon la distance du téléphone), le système utilise une matrice de décision :
  - **Extraction de Couleur (Espace HSV)** : Définition : Teinte, Saturation, Valeur. Un masque réduit (60% du rayon) est appliqué au centre pour extraire la couleur sans subir l'influence du bord ou de l'ombre. On sépare ainsi les groupes Or, Argent, et Cuivre.
  - **Vote par Ratio** : Chaque pièce détectée est comparée à toutes les autres. On cherche quelle identité (ex: 2€ vs 50cts) minimise l'erreur globale de ratio sur toute l'image.

# Méthodes Testées et Écartées
- **Détection par Gradients Spatiaux (Gradient-Based)**
  - **Définition - Gradients (Sobel)** : Calcul de la variation d'intensité lumineuse dans les directions X et Y.
  - **Test effectué** : Accumulateur radial basé sur la direction des gradients pour valider les bords des cercles.
  - **Résultat** : Très efficace pour ignorer les ombres, mais extrêmement sensible aux textures du fond (veines du bois ou marbre) qui génèrent des faux gradients "circulaires".

- **FitCircle / FitEllipse**
  - **Définition** : Ajustement mathématique d'un cercle parfait sur un nuage de points de contours (Canny).
  - **Résultat** : Échec total dès que les pièces sont collées ou qu'une ombre fusionne avec le bord de la pièce, car le nuage de points n'est plus circulaire.
