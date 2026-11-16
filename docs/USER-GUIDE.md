# Guide utilisateur — Nexus Office 0.2.x

## Présentation

Nexus Office est une application de bureau composée de quatre modules : Writer, Sheets, Slides et PDF. Les fonctions principales sont locales ; aucun compte ni service cloud n'est nécessaire. Le projet ne contient pas d'assistant IA.

## Configuration requise

- Windows 10/11 64 bits, macOS 14 ou version compatible avec Qt 6.8, ou Linux x86_64 récent.
- Les bibliothèques d'exécution Qt 6 sont incluses dans les paquets de déploiement.
- LibreOffice est requis pour les conversions DOCX, XLSX et PPTX.
- LibreOffice Impress et Python 3 sont requis pour l'échange PPTX de Slides.
- Hunspell avec dictionnaire français est requis pour la correction orthographique française de Writer.

Les conversions de formats Office sont des fonctions assistées par LibreOffice ; la fidélité des documents complexes n'est pas garantie. Sheets n'exporte que la feuille active au format XLSX. Slides ne conserve pas encore tous les objets riches PPTX.

## Installation

Les paquets générés par la CI sont des artefacts de validation non signés. Ils ne constituent pas encore une publication stable certifiée.

### Windows

1. Télécharger `NexusOffice-Setup-0.2.0-x64.exe`.
2. Lancer l'installateur et suivre l'assistant.
3. Lancer Nexus Office depuis le menu Démarrer.
4. Installer LibreOffice séparément si les conversions DOCX/XLSX/PPTX sont nécessaires.

### macOS

1. Ouvrir `NexusOffice-macOS.dmg`.
2. Faire glisser Nexus Office dans le dossier Applications.
3. Au premier lancement, macOS peut afficher un avertissement car l'application n'est pas encore signée et notariée. Ne contourner les protections que si le paquet provient du dépôt officiel et que son intégrité a été vérifiée.
4. Installer LibreOffice séparément pour les conversions Office.

### Linux Debian / Ubuntu

1. Télécharger le paquet `.deb` correspondant à l'architecture.
2. Installer avec le gestionnaire de paquets graphique, ou exécuter `sudo apt install ./nexus-office-0.2.0-*.deb`.
3. Installer LibreOffice séparément pour les conversions Office.

## Modules

### Writer
Rédiger du texte enrichi, utiliser les styles et les listes, insérer tableaux et images, rechercher, annuler/rétablir, vérifier l'orthographe et enregistrer en TXT, HTML, Markdown ou DOCX. L'export PDF et l'impression sont disponibles.

### Sheets
Modifier les cellules, créer plusieurs feuilles dans l'interface, utiliser les formules arithmétiques et les fonctions SUM, AVERAGE, MIN, MAX et COUNT, trier, filtrer et afficher un aperçu de graphique. CSV et XLSX sont pris en charge avec les limites décrites ci-dessus.

### Slides
Créer plusieurs diapositives, ajouter titres, textes, images, tableaux, formes simples et notes, choisir un thème, lancer le diaporama, exporter en PDF et échanger des fichiers PPTX via LibreOffice Impress.

### PDF
Ouvrir un PDF, naviguer entre les pages, rechercher du texte, zoomer, ajouter des commentaires par page et surligner des passages. Les fichiers annexes `.nexus-notes.json` et `.nexus-highlights.json` doivent rester à côté du PDF pour conserver les annotations. L'export annoté crée une copie aplatie.

## Données et confidentialité

Les documents sont ouverts et enregistrés dans les emplacements choisis par l'utilisateur. Les préférences et les copies de récupération sont stockées localement dans le répertoire de données de l'application. Aucun compte Nexus ni synchronisation cloud n'est requis.

## Dépannage

- **DOCX/XLSX/PPTX ne s'ouvre pas ou ne s'exporte pas :** vérifier que LibreOffice est installé et que la commande `soffice` est accessible.
- **Échange PPTX indisponible :** installer LibreOffice Impress et Python 3.
- **Correction orthographique absente :** installer Hunspell et le dictionnaire français.
- **PDF annoté différent du PDF original :** l'export intègre visuellement les surlignages et ajoute les commentaires en pages séparées ; il ne produit pas encore des annotations PDF natives.

## Signaler un problème

Avant de signaler un bug, préciser le système, sa version, la version de Nexus Office, le module concerné, les étapes de reproduction et joindre si possible un petit fichier de test non confidentiel.
