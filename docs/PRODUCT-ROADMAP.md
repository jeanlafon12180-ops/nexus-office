# Feuille de route Nexus Office

Objectif : construire une suite bureautique complète, multiplateforme et fiable, avec des fonctions bureautiques locales, sans service d'intelligence artificielle. Les versions finales Windows, macOS et Linux seront préparées après la stabilisation des modules.

## État actuel — prototype 0.2.x

- [x] Interface Qt avec navigation et thèmes clair/sombre
- [x] Writer : édition de texte enrichi et mise en forme de base
- [x] Writer : ouverture et enregistrement TXT / HTML
- [x] Writer : ouverture et enregistrement Markdown
- [x] Writer : recherche dans le document
- [x] Writer : annuler / rétablir
- [x] Writer : export PDF avec dialogue de mise en page
- [x] Writer : impression avec dialogue de mise en page
- [x] Writer : import/export DOCX par conversion LibreOffice
- [x] Writer : insertion de tableaux et d'images, listes et styles de titres
- [x] Writer : vérification orthographique française via Hunspell (dépendance système)
- [x] Sheets : grille éditable et import/export CSV
- [x] Sheets : formules simples et affichage de leurs résultats
- [x] Slides : édition d'une diapositive simple et export PDF
- [x] PDF : ouverture avec le lecteur PDF du système

## Étape 1 — Nexus Writer MVP

- [x] Édition de texte enrichi et mise en forme courante
- [x] Ouverture et enregistrement TXT, HTML et Markdown
- [x] Import et export DOCX via LibreOffice
- [x] Insertion de tableaux, images, listes et styles de titres
- [x] Recherche, annulation et rétablissement
- [x] Correcteur orthographique français via Hunspell
- [x] Réglages de page, impression et export PDF
- [x] Test automatisé DOCX aller-retour dans la CI
- [x] Compilation et tests automatisés validés dans la CI

**Étape 1 terminée au niveau MVP.** Cela signifie que le socle fonctionnel de Writer est présent et que la compilation ainsi que le test DOCX aller-retour passent dans la CI. Cela ne constitue pas une certification de compatibilité complète avec Microsoft Word.

## Améliorations Writer à reprendre dans l'étape qualité

- [ ] Tester un corpus varié de documents DOCX provenant de différentes versions de Word et LibreOffice
- [ ] Améliorer la fidélité DOCX et préserver davantage d'éléments de mise en page
- [ ] Ajouter des en-têtes, pieds de page et numéros de page répétitifs
- [ ] Ajouter des tests fonctionnels automatisés couvrant l'ouverture, la modification et l'enregistrement depuis l'interface
- [ ] Améliorer le correcteur avec suggestions et choix de dictionnaire

## Étape 2 — terminer Nexus Sheets

- [x] Afficher les résultats des formules directement dans les cellules
- [x] Ajouter un moteur de formules avec priorités opératoires, parenthèses, références de cellules et fonctions SUM, AVERAGE, MIN, MAX et COUNT
- [x] Valider le moteur de formules dans la CI (arithmétique, priorités, parenthèses, références, fonctions et erreurs)
- [x] Ajouter un premier ensemble de fonctions : SUM, AVERAGE, MIN, MAX et COUNT
- [x] Ajouter l'import et l'export XLSX via LibreOffice (feuille active uniquement)
- [x] Ajouter plusieurs feuilles dans l'interface
- [x] Ajouter le tri et le filtrage des lignes
- [x] Ajouter un aperçu de graphique en colonnes
- [ ] Étendre le moteur de calcul (fonctions supplémentaires, références inter-feuilles et gestion avancée des erreurs)
- [ ] Enregistrer et restaurer toutes les feuilles dans un même classeur XLSX
- [ ] Créer des graphiques persistants et modifiables dans le classeur

**Étape 2 terminée au niveau MVP.** Les fonctions de base du tableur sont disponibles et le build ainsi que les tests du moteur de formules passent dans la CI. Les classeurs XLSX multi-feuilles et les graphiques persistants restent des améliorations à réaliser.

## Étape 3 — terminer Nexus Slides

- [x] Gérer plusieurs diapositives : création, navigation et suppression
- [x] Ajouter trois thèmes visuels : clair, bleu et sombre
- [x] Insérer des images dans le contenu d'une diapositive
- [x] Ajouter des notes de présentateur (non imprimées)
- [x] Exporter une présentation multipage en PDF paysage
- [x] Ajouter l'insertion de tableaux éditables dans une diapositive
- [x] Ajouter des blocs de forme simples : encadré, bandeau et citation
- [x] Ajouter un mode diaporama avec transitions Fondu et Glissement
- [x] Ajouter l'import et l'export PPTX via LibreOffice Impress et un échange ODP
- [x] Ajouter un test UI de structure de l'éditeur Slides
- [x] Ajouter un test automatisé aller-retour PPTX vérifiant plusieurs diapositives et le texte

**Étape 3 terminée au niveau MVP.** Le mode diaporama propose les transitions Fondu et Glissement. L'import/export PPTX fonctionne par conversion LibreOffice via un fichier ODP intermédiaire. Les tests UI et le test d'aller-retour PPTX passent dans la CI. L'interopérabilité reste volontairement limitée : l'échange PPTX conserve le texte et les diapositives, mais ne préserve pas encore fidèlement les images, tableaux, formes, notes, thèmes et animations.

## Étape 4 — terminer Nexus PDF

- [x] Intégrer un lecteur PDF dans l'application avec Qt PDF
- [x] Ajouter navigation entre les pages, recherche textuelle et zoom
- [x] Ajouter des commentaires par page enregistrés dans un fichier annexe JSON
- [x] Ajouter l'impression du document PDF
- [x] Ajouter le surlignage persistant des passages textuels par recherche
- [x] Exporter un PDF annoté avec les surlignages intégrés et les commentaires ajoutés en pages dédiées

## Étape 5 — fonctions communes et différenciantes

- [x] Galerie locale de modèles Writer (lettre, CV, rapport scolaire, facture)
- [x] Copie de récupération locale périodique pour Writer et proposition de restauration au redémarrage
- [x] Gestion des documents récents (12 derniers chemins connus) et paramètres communs (thème enregistré localement)
- [x] Ajouter une première galerie de modèles bureautiques hors ligne pour Writer
- [x] Ajouter un raccourci clavier commun Ctrl+, pour ouvrir les paramètres
- [ ] Ajouter la gestion des préférences d'impression et d'export
- [x] Décider du périmètre de synchronisation et de partage : fonctionnement local hors ligne, sans compte ni cloud obligatoire ; partage par fichiers standards et services externes choisis par l’utilisateur

## Étape 6 — qualité et versions finales

- [ ] Tests fonctionnels et tests de compatibilité des formats
- [ ] Tests de performance, stabilité, accessibilité et sécurité
- [ ] Vérifier les installations sur Linux, macOS et Windows
- [ ] Préparer les paquets natifs pour chaque système
- [ ] Documentation utilisateur, site officiel et mises à jour

## Fonctions communes déjà intégrées

Les préférences d'apparence (thème clair/sombre) sont enregistrées localement avec QSettings. L'accueil affiche les 12 derniers fichiers ouverts ou exportés signalés par les modules ; un double-clic les ouvre avec l'application associée du système. Les fichiers récents sont des chemins locaux : si un fichier est déplacé ou supprimé, l'entrée n'est plus affichée. Le raccourci Ctrl+, ouvre les paramètres. Writer possède une copie de récupération locale périodique (toutes les 15 secondes lorsque le document est modifié), proposée au prochain lancement. Une galerie de modèles Writer hors ligne propose une lettre, un CV, un rapport scolaire et une facture. Ces fonctions ne couvrent pas encore les classeurs Sheets ni les présentations Slides. La synchronisation cloud n’est pas intégrée au périmètre 0.2.x : les documents restent locaux et peuvent être partagés via leurs formats standards (DOCX, XLSX, PPTX, PDF, CSV) ou par un service externe choisi séparément par l’utilisateur.

## Limites connues

Nexus Office reste un prototype. Writer s'appuie sur LibreOffice pour convertir les fichiers DOCX et ne garantit pas encore une fidélité complète aux documents complexes. Le correcteur orthographique nécessite Hunspell et un dictionnaire français installés sur le système. Sheets importe et exporte les fichiers XLSX via LibreOffice, mais ne traite que la feuille active : les autres feuilles du classeur ne sont pas encore conservées. Les graphiques sont des aperçus temporaires, pas des objets enregistrés dans le fichier. L'analyse CSV reste simplifiée et ne gère pas encore les champs contenant des retours à la ligne. Slides propose un diaporama avec transitions et un import/export PPTX de base via LibreOffice. Le passage PPTX ne conserve actuellement que les titres et le texte des diapositives ; les éléments riches (images, tableaux, formes, notes, thèmes et animations) ne sont pas encore préservés. Le module nécessite LibreOffice Impress et Python 3 pour l'échange PPTX. Le module PDF intègre un lecteur Qt PDF, la navigation, le zoom, la recherche textuelle, les commentaires par page (fichier annexe .nexus-notes.json) et l'impression. Le surlignage est enregistré dans un fichier annexe .nexus-highlights.json et intégré au PDF via l'export annoté ; les commentaires sont enregistrés dans .nexus-notes.json et ajoutés en pages dédiées à l'export. Les annotations ne sont pas encore des objets PDF natifs modifiables séparément dans d'autres lecteurs. Qt PDF est une dépendance de compilation. Les fonctionnalités cochées décrivent le périmètre MVP validé, pas un niveau de finition commercial.
