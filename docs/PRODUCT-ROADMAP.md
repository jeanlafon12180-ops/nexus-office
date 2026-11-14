# Feuille de route Nexus Office

## Prototype desktop 0.2.x

- [x] Interface Qt avec navigation et thèmes clair/sombre
- [x] Writer : édition riche de HTML et texte
- [x] Writer : conversion DOCX via LibreOffice installé séparément
- [x] Sheets : grille éditable et import/export CSV
- [x] Sheets : formules simples (opérations directes, références et SUM)
- [x] Slides : édition d'une diapositive simple et export PDF
- [x] PDF : ouverture avec le lecteur PDF du système
- [x] CI Linux et Windows avec génération de paquets Windows

## Prochaines étapes

- [ ] Tester le cycle ouverture/modification/enregistrement DOCX sur des documents réels
- [ ] Afficher les résultats de formules directement dans les cellules et améliorer le moteur de calcul
- [ ] Ajouter le format XLSX natif et les fonctions de feuille avancées
- [ ] Ajouter plusieurs diapositives, thèmes, images et export PPTX
- [ ] Intégrer un lecteur PDF avec navigation, recherche et annotations
- [ ] Ajouter des tests automatisés fonctionnels pour chaque module
- [ ] Valider l'installation sur Windows 10/11 et Linux ; étudier la distribution macOS

## Limites connues

Le tableur gère actuellement le CSV, pas le format XLSX. Slides crée une seule page et exporte en PDF, pas en PPTX. Le module PDF délègue l'ouverture au lecteur système. La prise en charge DOCX utilise LibreOffice installé sur le poste et ne garantit pas encore une fidélité parfaite de mise en page.
