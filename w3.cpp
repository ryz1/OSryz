#include <cstdio>
#include <cstdlib>

unsigned char Memory[4096];
int PDBR;

int LookUpPageDirectory(int PDI) {
	return (Memory[PDI + PDBR]);
}

int LookUpPageTable(int PTA,int PTI) {
	return (Memory[(PTA << 5) + PTI]);
}

int quary(int VA) {
	printf("Virtual Address %04x:\n",VA);
	int PDI = (VA >> 10); // page directory index
	int PTI = ((VA >> 5) & 31);
	int PA = (VA & 31);
	int PTA = LookUpPageDirectory(PDI);
	printf("pde index:%02x pde contents:(valid %d, pfn %02x)\n",PDI,(PTA & 128) >> 7,PTA & 127);	
	if ((PTA & 128) == 0) {
		puts("Fault (page directory entry not valid)");
		return 0;
	}
	PTA = (PTA & 127);
	int PI = LookUpPageTable(PTA,PTI);
	printf("pte index:%02x pte contents:(valid %d, pfn %02x)\n",PTI,(PI & 128) >> 7,PI & 127);
	if ((PI & 128) == 0) {
		puts("Fault (page table entry not valid)");
		return 0;
	}
	PI = (PI & 127);
	int address = (PI << 5) + PA;
	int content = Memory[address];
	printf("Translates to Physical Address %03x --> Value: %d\n",address,content);
	return 0;
}

int main() {
	FILE* f = fopen("memory","r");
	fscanf(f,"%x",&PDBR);
	for (int i = 0;i < 4096;i++) {
		int a;
		fscanf(f,"%x",&a);
		Memory[i] = a;
	}
	while (1) {
		int q;
		scanf("%x",&q);
		quary(q);
	}
	return 0;
}