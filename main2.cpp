#include "merkletree.h"
#include <time.h>

using namespace std;

void calSHA256(char* inp,char out_buff[65]){
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, inp, strlen(inp));
  SHA256_Final(hash, &sha256);

  //char buffx[65];
  for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
  {
    sprintf(out_buff + (i * 2), "%02x", hash[i]);
  }
  out_buff[65] = 0;
  //memcpy(out_buff,buffx,65);
}

int main(){

  int Leaf;
  scanf("%d",&Leaf);
  for(int i=0;i<33554432;i++);

  vector<char*> v(Leaf);

  for(int i =0;i<Leaf;i++){
    char *buff = new char[65];
    scanf("%65s\n",buff );
    v[i] = buff;
  }

	clock_t t;
	t = clock();
  merkletree mtree = merkletree(v);

  printf("root : %s\n",mtree.root());
	t = clock() - t;
	printf("[build mtree] took %d clocks (%f secs)\n",t,(float)t/CLOCKS_PER_SEC);

	t = clock();

	char* leaf = v[0];
//	printf("leaf : %s\nroot : %s\n",leaf,mtree.root());
	vector<ProofNode> proof = mtree.proof(leaf);

//	for(int i = 0 ;i<proof.size();i++){
//		printf("parent : %s\nleft : %s\nright : %s\n",proof[i].parent,proof[i].right,proof[i].left);
//	}

	t = clock() - t;
	printf("[get proof] took %d clocks (%f secs)\n",t,(float)t/CLOCKS_PER_SEC);

	t = clock();

        bool verproof = mtree.verifyProof(leaf,mtree.root(),proof);

        t = clock() - t;
        printf("[verify proof][ => %d ] took %d clocks (%f secs)\n",verproof,t,(float)t/CLOCKS_PER_SEC);


}
