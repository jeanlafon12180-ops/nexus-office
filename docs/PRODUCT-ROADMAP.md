# Feuille de route Nexus Office

Objectif : construire une suite bureautique complète, multiplateforme et fiable, avec des fonctions d'assistance supplémentaires. Les versions finales Windows, macOS et Linux seront préparées après la stabilisation des modules.

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

- [ ] Gérer plusieurs diapositives
- [ ] Ajouter thèmes, images, formes, tableaux et notes
- [ ] Ajouter transitions et animations
- [ ] Ajouter import/export PPTX

## Étape 4 — terminer Nexus PDF

- [ ] Intégrer un lecteur PDF dans l'application
- [ ] Ajouter navigation, recherche et zoom
- [ ] Ajouter annotations, surlignage et commentaires
- [ ] Ajouter impression et export

## Étape 5 — fonctions communes et différenciantes

- [ ] Modèles de documents et galerie de modèles
- [ ] Sauvegarde automatique et récupération après incident
- [ ] Gestion des documents récents et paramètres communs
- [ ] Assistant IA pour rédiger, corriger et résumer
- [ ] Assistance IA pour analyser les tableaux et préparer des présentations
- [ ] Étudier la collaboration et le partage de documents

## Étape 6 — qualité et versions finales

- [ ] Tests fonctionnels et tests de compatibilité des formats
- [ ] Tests de performance, stabilité, accessibilité et sécurité
- [ ] Vérifier les installations sur Linux, macOS et Windows
- [ ] Préparer les paquets natifs pour chaque système
- [ ] Documentation utilisateur, site officiel et mises à jour

## Limites connues

Nexus Office reste un prototype. Writer s'appuie sur LibreOffice pour convertir les fichiers DOCX et ne garantit pas encore une fidélité complète aux documents complexes. Le correcteur orthographique nécessite Hunspell et un dictionnaire français installés sur le système. Sheets importe et exporte les fichiers XLSX via LibreOffice, mais ne traite que la feuille active : les autres feuilles du classeur ne sont pas encore conservées. Les graphiques sont des aperçus temporaires, pas des objets enregistrés dans le fichier. L'analyse CSV reste simplifiée et ne gère pas encore les champs contenant des retours à la ligne. Slides ne gère qu'une diapositive et n'exporte pas PPTX. Le module PDF délègue encore l'ouverture au lecteur du système. Les fonctionnalités cochées décrivent le périmètre MVP validé, pas un niveau de finition commercial.
