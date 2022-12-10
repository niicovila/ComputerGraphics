
Computer Graphics
Lab 5
NIA 1 = 240230
NIA 2 = 240446
Nicolás Vila Alarcón (nicolas.vila01@estudiant.upf.edu)
Pere Quílez Codina (pere.quilez01@estudiant.upf.edu)


Funcionament de l’aplicació:


Hem creat una aplicació on hi ha 4 modes principals; un per cada tasca:
El primer mode (scancode 1), pinta a en “Lee” a pantalla amb la textura: “color_texture” i l’il·luminem a través d’una llum, seguint la tècnica de Phong.
En segon lloc (scancode 2),  fà el mateix que en el cas anterior, però en aquest cas quan il·luminem a en “Lee” seguint la tècnica de Phong hi afegim la component especular “ks”.
El tercer mode (scancode 3), pinta a en “Lee” a pantalla amb la textura que estavem utilitzant  en els altres dos modes, però ara hi afegim una segona textura: “normal_texture” i l’il·luminem a través d’una llum, seguint la tècnica de Phong amb el coeficient especular “ks”.
Per últim, el quart mode (scancode 4), utilitzem la mateixa configuració que en la tasca 3, però ara podem afegir i eliminar meshes, les quals es pintaran a esquerra i dreta de la primera mesh. Cada una d’aquestes noves meshes, “Lee” tindrà unes propietats de material completament diferents les quals s'inicia litzen de forma aleatòria quan n’afegim una. Per tal de poder controlar el número de meshes, “lee”, ho fem a través de les tecles N (eliminar 1 model) i M (afegir 1 model).


A part, en els quatre modes principals disposem d’unes comandes per tal de poder controlar la càmera desde el teclat:


• La posició “center” es controla mitjançant les fletxes (només a la dimensió X,Y).
• El “fov” es controla mitjançant les tecles F i G. (zoom in/out)


També disposem de un comande per fer que les meshes girin sobre si mateixes, aquest mode s’activa a través de la tecla espai (scancode space).


Per últim, per la tasca, aplicar textures en la pràctica 3, el mode s’activa a través de la tecla scancode 4 i disposa dels mateixos comandes que la resta de tasques per moure la càmera.