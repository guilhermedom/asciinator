OS = $(shell uname)

#Para caso seja executado em windows, porém não testado em windows.
#Testado apenas em Linux.
ifeq ($(OS),Windows_NT) 
	EXEC = start Main
else
	EXEC = ./Main
endif

#Compila e executa o Main.c
main:
	gcc -o Main Main.cpp
	$(EXEC)

compile:
	gcc -o Main Main.cpp

clean:
	rm -f Main

#Printa em tela todos os caracteres ascii presentes para print da imagem.
ascii:
	gcc -o Main Main.cpp
	$(EXEC) -ascii

#Testa para todas as imagens presente no vetor nomeImagens contido no Main.c
#e joga o resultado no arquivo output.txt
testAll:
	gcc -o Main Main.cpp
	$(EXEC) -testAll > output.txt

	
