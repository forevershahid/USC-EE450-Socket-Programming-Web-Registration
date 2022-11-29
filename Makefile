all:
	gcc -o serverM serverM.c
	gcc -o serverC serverC.c
	gcc -o serverEE serverEE.c
	gcc -o serverCS serverCS.c
	gcc -o client client.c