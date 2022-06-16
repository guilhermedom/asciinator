#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QTD_SIMBOLOS 27


// constante usada para teste.
#define QTD_IMAGENS 6

/**
 * Para alterar os tamanhos dos blocos MxN
 * deve-se alterar estas constanstes e
 * recompilar o código.
 */
#define M 1
#define N 2


//original 
//unsigned char simbolos[] = {'#','$','O','=','+','|','-','^','.',' '};

//7-bits ASCII, se alterar lembrar de alterar QTD_SIMBOLOS!!
unsigned char simbolos[] = {'@','#','&','8','X','%','$','S','G','O','=','+','}','?','|','!','*','=','-','^','~',';',',','\"','\'','.',' '};


//vetor usado para teste, se alterar lembrar de alterar QTD_IMAGENS!! (ver testAll)
char *nomeImagens[QTD_IMAGENS] = {"socrates.bmp", "mikey.bmp", "smiley.bmp", "canada.bmp", "sight.bmp", "hp.bmp"};


//struct para armazenamento de dados que serão uteis durante a execução do programa.
typedef struct{
	int largura, altura, bitsPixel;
} t_DadosHeader;

/**
 * Abre e verifica se o tal arquivo é um arquivo .bmp, caso o
 * arquivo não consiga abrir ou o arquivo não seja .bmp o programa
 * fechará com flag de erro. Caso contrario será retornado o arquivo
 * aberto.
 */
FILE *abrirArquivo(char *pathEntrada);

/**
 * lê as informações uteis ao projeto no cabeçalho,
 * e deixa o pronto para ler os pixels da imagem. 
 */
t_DadosHeader lerCabecalho(FILE *arq);

/**
 * Carrega a imagem em memoria.
 */
int **carregarImage(FILE *arq, t_DadosHeader dados);

/**
 * Returna o caracter adequado à media da matriz que representa os pixels.
 */
char getChar(int mat[N][M]);

/**
 * Processa e pinta a imagem na saida padrão.
 */
void printarSaida(int **imagem, t_DadosHeader dados);

/**
 * Libera a matriz de imagem.
 */
void liberar(int **imagem, t_DadosHeader dados);

/**
 * Quando passado -ascii como argumento ao main, printa na saida padrão
 * todos os caracteres ascii e seu respectivo range.
 */
void printAllASCII();

/**
 * Quando passado -testAll como argumento ao main, testa-se todas as imagens com o nome
 * presente em nomeImagens e printa o resultado na saida padrão, (recomendavel redirecionar
 * o resultado da stdout para um arquivo, usando o operador '>', e.g. "./Main -testAll > Saida.txt").
 */
void testAll();

/*************** CODIGO ABAIXO ***************/

int main(int argc, char **argv){
	char pathEntrada[100];
	int **imagem;
	t_DadosHeader dados;
	char op;
	
	FILE *arq;
	
	do{
		if(argc > 1){
			if(!strcmp(argv[1],"-ascii")){ //printa todos os caracteres ascii e seu range.
				printAllASCII();
				exit(0);
			}
			if(!strcmp(argv[1],"-testAll")){ //testa todas as imagens com o nome presente em nomeImagens.
				testAll();
				printf("\n");
				exit(0);
			}
			if(argv[1][0] != '-'){ //para passar o nome da imagem por argumento ao main.
				arq = abrirArquivo(argv[1]);
			}	
		}
		else{
			printf("Digite o path do arquivo de entrada: ");
			while(fgets(pathEntrada, sizeof(pathEntrada), stdin)[0] == '\n');
			pathEntrada[strlen(pathEntrada) - 1] = '\0';
			arq = abrirArquivo(pathEntrada);
		}
	
		dados = lerCabecalho(arq);
	
		imagem  = carregarImage(arq, dados);
	
		printarSaida(imagem, dados);
	
		liberar(imagem, dados);
	
		fclose(arq);
		
		
		//ir novamente
		printf("\n\nDeseja executar novamente? (S/N): ");
		while((op = getchar()) == '\n');
		
		while(op != 'n' && op != 'N' && op != 's' && op != 'S'){
			printf("Opção invalida, digite S/N: ");
			while((op = getchar()) == '\n');
		}		
	}while(op == 's' || op == 'S');
}

/**
 * Abre e verifica se o tal arquivo é um arquivo .bmp, caso o
 * arquivo não consiga abrir ou o arquivo não seja .bmp o programa
 * fechará com flag de erro. Caso contrario será retornado o arquivo
 * aberto.
 */
FILE *abrirArquivo(char *pathEntrada){
	char tipoArq[3];
	char aux[100];
	FILE *arq;
	arq = fopen(pathEntrada,"rb");
	if(arq == NULL){ 
		sprintf(aux,"images/%s",pathEntrada);
		arq = fopen(aux,"rb");
		if(arq == NULL){ 
			printf("Falha na abertura do arquivo \"%s\" (não esquecer da extensão do arquivo). \n\n\n\n",pathEntrada);
			exit(EXIT_FAILURE);
		}
	}
	fread(tipoArq,2,1,arq);
	
	if(strcmp(tipoArq,"BM") != 0){
		printf("Arquivo %s não é um arquivo .bmp. \n\n\n\n",pathEntrada);
		exit(EXIT_FAILURE);
	}
	
	return arq;
}

/**
 * lê as informações uteis ao projeto no cabeçalho,
 * e deixa o pronto para ler os pixels da imagem. 
 */
t_DadosHeader lerCabecalho(FILE *arq){
	t_DadosHeader dados;	
	long pos = 0;
	dados.largura = 0;
	dados.altura = 0;
	dados.bitsPixel = 0;
	
	fseek(arq,10,SEEK_SET);
	fread(&pos,4,1,arq);
	
	fseek(arq,18,SEEK_SET);

	
	fread(&dados.largura,4,1,arq);
	fread(&dados.altura,4,1,arq);
	
	fseek(arq,28,SEEK_SET);
	
	
	fread(&dados.bitsPixel,2,1,arq);	
	
	fseek(arq,pos, SEEK_SET);
	
	
	return dados;
}

/**
 * Carrega a imagem em memoria.
 */
int **carregarImage(FILE *arq, t_DadosHeader dados){
	int **imagem = (int**) malloc(dados.altura*sizeof(int*));
	unsigned char cbuf[4];
	int tamB = dados.bitsPixel/8;
	
	//aloca a matriz.
	if(imagem == NULL){
		printf("Problema na alocação da matriz");
		exit(EXIT_FAILURE);
	}
	for(int i = 0; i < dados.altura; i++){
		imagem[i] = (int*) malloc(dados.largura*sizeof(int));
		if(imagem[i] == NULL){
			printf("Problema na alocação da matriz");
			exit(EXIT_FAILURE);
		}
	}
	
	//le matriz
	for(int i = dados.altura - 1; i >= 0; i--){
		for(int j = 0; j < dados.largura; j++){	
			fread(cbuf,1,tamB,arq);
			imagem[i][j] = (int)((short)cbuf[2]+(short)cbuf[1]+(short)cbuf[0])/3;
		}
	}


	return imagem;
}

/**
 * Returna o caracter adequado à media da matriz que representa os pixels.
 */
char getChar(int mat[N][M]){
	float media;
	float intervalo = 256/((float) QTD_SIMBOLOS);
	int soma = 0;
	int aux;
	
	for(int i = 0; i < N; i++)
		for(int j = 0; j < M; j++)
			soma += mat[i][j];
	
	media = ((float) soma/((float) N*M));
	
	for(int i = 1; i <= QTD_SIMBOLOS; i++){
		aux = ((int) i*intervalo - 1);
		if(media <= aux)
			return simbolos[i-1];
	}
	
	return 'a';
}

/**
 * Processa e pinta a imagem na saida padrão.
 */
void printarSaida(int **imagem, t_DadosHeader dados){
	int larguraS = (dados.largura/M), alturaS = (dados.altura/N);
	int mat[N][M];
	
	printf("\n");
	for(int i = 0; i < alturaS; i++){
		for(int j = 0; j < larguraS; j++){
			for(int k = 0; k < N; k++){
				for(int l = 0; l < M; l++){
					mat[k][l] = imagem[i*N+k][j*M+l];
				}
			}
			printf("%c", getChar(mat));
		}
		printf("\n");
	}	

}

/**
 * Libera a matriz de imagem.
 */
void liberar(int **imagem, t_DadosHeader dados){
	for(int i = 0; i < dados.altura; i++)
		free(imagem[i]);
	free(imagem);
}

/**
 * Quando passado -ascii como argumento ao main, printa na saida padrão
 * todos os caracteres ascii e seu respectivo range.
 */
void printAllASCII(){
	float intervalo = 256/((float) QTD_SIMBOLOS);
	int aux, aux2;
	printf(" Média RGB -> ASCII\n");
	for(int i = 0; i < QTD_SIMBOLOS; i++){
		aux = ((int) i*intervalo); // devido a problemas com cast
		aux2 = ((int) (i+1)*intervalo-1);  // devido a problemas com cast
		printf("%3d to %3d -> \'%c\' \n", aux, aux2, simbolos[i]);
	}
	printf("\n");
}

/**
 * Quando passado -testAll como argumento ao main, testa-se todas as imagens com o nome
 * presente em nomeImagens e printa o resultado na saida padrão, (recomendavel redirecionar
 * o resultado da stdout para um arquivo, usando o operador '>', e.g. "./Main -testAll > Saida.txt").
 */
void testAll(){
	int **imagem;
	t_DadosHeader dados;
	FILE *arq;
	
	for(int i = 0; i < QTD_IMAGENS; i++){
		arq = abrirArquivo(nomeImagens[i]);
		
		dados = lerCabecalho(arq);
	
		imagem  = carregarImage(arq, dados);
	
		printarSaida(imagem, dados);
	
		liberar(imagem, dados);
	
		fclose(arq);
	}
}

