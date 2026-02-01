Pour installer la lib opencv (linux):
	sudo apt install libopencv-dev

Chaque exercice a son propre exécutable, généré par make dans le dossier bin. Des images sont fournies dans le dossier data et les images produites seront sauvegardées dans le même dossier data sous le nom out.png.

Tous les exécutable s'utilisent de la même manière :

./moyenneur <image_path>
./grad_sobel <image_path>
./median <image_path>

Exemple depuis le dossier tp_convolution :

./bin/grad_sobel data/camera.png
