## Usage

```cpp

#include "merkletree.h"

using namespace std;

int main(){

	// initialize leaves
	vector<char*> leaves(5);
	leaves[0] = "5feceb66ffc86f38d952786c6d696c79c2dbc239dd4e91b46729d73a27fb57e9";
	leaves[1] = "6b86b273ff34fce19d6b804eff5a3f5747ada4eaa22f1d49c01e52ddb7875b4b";
	leaves[2] = "d4735e3a265e16eee03f59718b9b5d03019c07d8b6c51f90da3a666eec13ab35";
	leaves[3] = "4e07408562bedb8b60ce05c1decfe3ad16b72230967de01f640b7e4729b49fce";
	leaves[4] = "4b227777d4dd1fc61c6f884f48641d02b4d121d3fd328cb08b5531fcacdabf8a";

	// initialize merkletree
	merkletree mtree = merkletree(leaves);
	char* root = mtree.root();

	// get proof
	char* sample_leaf = leaves[0];
	vector<ProofNode> proof = mtree.proof(sample_leaf);

	// verify proof
	bool verified = mtree.verifyProof(sample_leaf,root,proof);
}

```
## To-do

## Inspiration
https://github.com/blockai/merkletree
