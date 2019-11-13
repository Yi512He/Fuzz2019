#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
	int status = 0;
	char str[100];
	while(1){
		printf("input:");
		scanf("%s", str);
		if(strcmp(str, "exit") == 0) {
			break;
		}
		else if(strcmp(str, "crash") == 0) {
			char *xxx = "crash!!";
            		xxx[1] = 'D';
		}
		else if(strcmp(str, "hang") == 0) {
			printf("hang!");
			while(1);
		}
		else {
			printf("%s\n", str);
		
		}
	
	}
	return 0;
}
