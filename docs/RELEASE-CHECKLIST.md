# Checklist de publication

## Validation technique

- [ ] CI Linux, Windows et macOS verte sur le même commit.
- [ ] Tests unitaires et tests d'interface verts.
- [ ] Tests DOCX et PPTX aller-retour verts.
- [ ] Ajouter des tests d'import/export XLSX et CSV exécutés par le code de Nexus Sheets.
- [ ] Tester les documents DOCX/XLSX/PPTX complexes et documenter les pertes connues.
- [ ] Vérifier récupération après incident, documents récents et préférences.
- [ ] Audit des dépendances, permissions, chemins et données locales.
- [ ] Vérifier clavier, navigation accessible, contraste et lecteurs d'écran.
- [ ] Mesurer démarrage, mémoire et réactivité sur des fichiers de taille représentative.

## Paquets

- [ ] Windows x64 : build Release, déploiement Qt, installateur Inno Setup.
- [ ] macOS : bundle app, déploiement Qt, image DMG.
- [ ] Linux x86_64 : paquet DEB et dépendances déclarées.
- [ ] Installer et lancer chaque paquet sur une machine propre ou une VM.
- [ ] Vérifier désinstallation, raccourcis, fichiers de ressources et mises à jour.
- [ ] Signature Windows et signature/notarisation macOS avant diffusion publique.
- [ ] Vérifier architecture CPU, dépendances LibreOffice/Hunspell et mentions légales.

## Publication

- [ ] Choisir et publier la licence du code Nexus Office.
- [ ] Finaliser l'inventaire des licences et avis tiers.
- [ ] Valider les noms, icônes, captures et site officiel.
- [ ] Écrire les notes de version et les limites connues.
- [ ] Créer un tag de version et publier les paquets vérifiés.
- [ ] Définir une procédure de signalement et de correction des vulnérabilités.

Ne pas présenter un artefact CI non signé comme une version stable destinée au grand public.
