# Nexus Office

**Suite bureautique de bureau multiplateforme — prototype 0.2.x.**

Nexus Office est un projet indépendant en C++17 et Qt 6. Il regroupe quatre modules dans une seule application : Writer, Sheets, Slides et PDF. Le projet vise une utilisation locale et hors ligne pour les fonctions principales. Il ne prévoit pas d'assistant IA ni de service IA obligatoire.

## Modules

| Module | Fonctionnalités présentes | Limites principales |
|---|---|---|
| **Nexus Writer** | Texte enrichi, polices, tailles, styles de titres, alignements, listes, tableaux, images, recherche, annuler/rétablir, Hunspell français, TXT, HTML, Markdown, DOCX via LibreOffice, PDF et impression | En-têtes/pieds de page et fidélité DOCX avancée à compléter |
| **Nexus Sheets** | Grille éditable, feuilles multiples dans l'interface, CSV, import/export XLSX via LibreOffice (feuille active), formules arithmétiques, références, plages, SUM, AVERAGE, MIN, MAX, COUNT, MOYENNE, NB, tri, filtre et aperçu de graphique | Classeur XLSX multi-feuilles non conservé ; graphiques temporaires ; fonctions limitées |
| **Nexus Slides** | Plusieurs diapositives, titres/textes, images, tableaux, formes simples, notes, thèmes clair/bleu/sombre, diaporama, transitions Fondu/Glissement, export PDF, import/export PPTX via LibreOffice Impress | Échange PPTX limité : titres et texte principalement ; fidélité des objets riches à améliorer |
| **Nexus PDF** | Lecteur intégré, navigation, zoom, recherche textuelle, commentaires par page, surlignage textuel par recherche, impression et export PDF annoté | Surlignages et commentaires stockés en fichiers annexes ; export aplati en pages images et commentaires ajoutés en pages dédiées |

## Annotations PDF

Les annotations sont conservées à côté du fichier source :
- `.nexus-notes.json` pour les commentaires par page ;
- `.nexus-highlights.json` pour les passages surlignés.

« Exporter PDF annoté » produit un nouveau PDF avec les surlignages intégrés visuellement et les commentaires ajoutés en pages dédiées. Le PDF exporté est aplati : les annotations ne sont pas des objets PDF natifs indépendamment modifiables.

## Dépendances

- C++17
- CMake 3.21+
- Qt 6 Widgets, PrintSupport, Pdf et PdfWidgets
- LibreOffice pour les conversions DOCX/XLSX/PPTX prises en charge
- LibreOffice Impress et Python 3 pour l'échange PPTX
- Hunspell et dictionnaire français pour la correction orthographique

## Compilation et tests

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Feuille de route

1. Writer — MVP fonctionnel, améliorations de fidélité et de mise en page restantes.
2. Sheets — MVP fonctionnel, fonctions et conservation XLSX à compléter.
3. Slides — MVP fonctionnel, compatibilité PPTX à améliorer.
4. PDF — lecteur, recherche, zoom, commentaires, surlignage et export annoté.
5. Fonctions communes — modèles, sauvegarde, récupération, paramètres et documents récents.
6. Qualité et versions finales — compatibilité, stabilité, accessibilité, documentation et paquets Windows/macOS/Linux.

Aucun installateur de version finale n'est publié pendant le développement : les paquets seront préparés après stabilisation des modules.

## Licence et marques

Le modèle de licence du projet est à finaliser. Les composants tiers conservent leurs licences respectives. Nexus Office est indépendant et n'est affilié ni à Microsoft ni à The Document Foundation.
