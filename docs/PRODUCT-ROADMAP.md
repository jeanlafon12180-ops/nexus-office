# Feuille de route Nexus Office

Objectif : construire une suite bureautique complète, multiplateforme et fiable, avec des fonctions d'assistance supplémentaires. Les versions finales Windows, macOS et Linux seront préparées après la stabilisation des modules.

## État actuel — prototype 0.2.x

- [x] Interface Qt avec navigation et thèmes clair/sombre
- [x] Writer : édition de texte enrichi et mise en forme de base
- [x] Writer : ouverture et enregistrement TXT / HTML
- [x] Writer : ouverture et enregistrement Markdown
- [x] Writer : recherche dans le document
- [x] Writer : annuler / rétablir
- [x] Writer : export PDF
- [x] Writer : import/export DOCX par conversion LibreOffice (dépendance externe, fidélité à valider)
- [x] Sheets : grille éditable et import/export CSV
- [x] Sheets : formules simples
- [x] Slides : édition d'une diapositive simple et export PDF
- [x] PDF : ouverture avec le lecteur PDF du système

## Étape 1 — terminer Nexus Writer

- [ ] Tester ouverture, modification et réenregistrement de DOCX variés
- [ ] Améliorer la fidélité DOCX et préserver les éléments de mise en page
- [ ] Ajouter tableaux, images, listes et styles de titres avancés
- [ ] Ajouter en-têtes, pieds de page, numéros de page et réglages de page
- [ ] Ajouter correcteur orthographique et dictionnaire
- [ ] Ajouter impression, aperçu avant impression et gestion des marges
- [ ] Ajouter tests automatisés d'ouverture et d'enregistrement des formats

## Étape 2 — terminer Nexus Sheets

- [ ] Afficher les résultats des formules directement dans les cellules
- [ ] Construire un moteur de calcul plus complet
- [ ] Ajouter import/export XLSX
- [ ] Ajouter plusieurs feuilles, graphiques, tris et filtres

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

Nexus Office reste un prototype. Writer ne garantit pas encore une fidélité complète aux documents DOCX. Sheets ne prend pas encore en charge XLSX. Slides ne gère qu'une diapositive et n'exporte pas PPTX. Le module PDF délègue encore l'ouverture au lecteur du système. Les fonctionnalités cochées indiquent uniquement ce qui existe dans le prototype, pas un niveau de finition commercial.
