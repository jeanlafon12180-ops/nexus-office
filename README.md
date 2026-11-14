# Nexus Office

**Suite bureautique de bureau multiplateforme — prototype 0.2.0.**

Nexus Office est une application Qt pour Windows et Linux (macOS prévu). Elle regroupe plusieurs outils dans une interface unique.

## Modules disponibles

| Module | Fonctionnalités actuelles | Limites |
|---|---|---|
| **Nexus Writer** | Édition riche, mise en forme de base, recherche, annuler/rétablir, TXT, HTML, Markdown, export PDF et ouverture/export DOCX via LibreOffice | DOCX dépend de LibreOffice ; fidélité de mise en page encore à valider ; outils avancés à développer |
| **Nexus Sheets** | Grille éditable, import/export CSV, ajout de lignes/colonnes, formules simples | Pas de fichiers XLSX natifs ; formules limitées |
| **Nexus Slides** | Création d'une diapositive titre/contenu, export PDF | Pas encore de gestion multi-diapositives ni de PPTX |
| **Nexus PDF** | Ouvre un PDF avec le lecteur par défaut du système | Pas de lecteur PDF intégré ni d'édition/annotation |

## Formules Sheets prises en charge

- Références de cellules dans des opérations simples : `=A1+B1`, `=A1*B1`
- Opérations numériques directes : `=12*3`
- Somme d'une plage : `=SUM(A1:B4)`

Le résultat d'une formule est visible dans l'infobulle de la cellule. Les formules ne constituent pas encore un moteur de calcul complet.

## Développement

Technologies : C++17, Qt 6 Widgets, CMake 3.21+, Qt PrintSupport.

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Pour DOCX, installez LibreOffice et assurez-vous que son exécutable `soffice` est accessible dans le PATH (ou installé dans son emplacement standard sous Windows/macOS).

## Plateformes et distribution

Le workflow GitHub Actions construit et teste les versions Linux et Windows, puis prépare un installateur Windows Inno Setup et une archive portable. Les artefacts se téléchargent depuis l'onglet Actions du dépôt.

## État du projet

Nexus Office reste un prototype fonctionnel avec des fonctionnalités limitées. Il ne remplace pas encore Microsoft Office ou LibreOffice et ne doit pas être présenté comme pleinement compatible avec DOCX, XLSX ou PPTX.

## Licence et marques

Le modèle de licence du projet est à finaliser. Les composants tiers conservent leurs licences respectives. Nexus Office est un projet indépendant, non affilié à Microsoft ni à The Document Foundation.
