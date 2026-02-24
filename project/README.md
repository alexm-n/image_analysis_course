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
  - **Ecart-type (StdDev)** : Définition : Mesure statistique de la dispersion des niveaux de gris. Si StdDev > seuil, application d'un **Filtre Bilatéral** (lissage préservant les contours) pour ignorer les veines du marbre.
