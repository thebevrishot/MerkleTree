#include "binarytree.h"
#include <string.h>
#include <iostream>
#include <openssl/sha.h>
#include <cstdio>

#define SHA256_LENGTH 64
#define NODE_LENGTH 65

using namespace std;

struct ProofNode {
  char* hash;
  bool isRight;
  ProofNode(){isRight= false;hash = new char[SHA256_LENGTH];}
};

// buff 
char* serialize(vector<ProofNode>& proof) // Writes the given OBJECT data to the given file name.
{
	char *buff = new char[proof.size()*NODE_LENGTH+1];
	buff[proof.size()*NODE_LENGTH]=0;
	for(int i =0;i<proof.size();i++){
		memcpy(buff+NODE_LENGTH*i+1,proof[i].hash,SHA256_LENGTH);
    buff[NODE_LENGTH*i] = proof[i].isRight? '1':'0';
	}
	return buff;
};

vector<ProofNode> deserialize(const char* strdata) // Reads the given file and assigns the data to the given OBJECT.
{
	size_t datalen = strlen(strdata);
	vector<ProofNode> proof(datalen/NODE_LENGTH);

	for(int i = 0 ;i<proof.size();i++){
		memcpy(proof[i].hash,strdata+NODE_LENGTH*i+1,SHA256_LENGTH);
    proof[i].isRight = strdata[NODE_LENGTH*i]!='0';
	}

	return proof;
};

// combin and hash by sha256
static void combin(char* leftData,char* rightData,char out_buff[65]){

  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256,leftData,strlen((const char*)leftData) );
  SHA256_Update(&sha256,rightData,strlen((const char*)rightData) );
  SHA256_Final(hash, &sha256);

  for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
  {
    sprintf(out_buff + (i * 2), "%02x", hash[i]);
  }
  out_buff[64] = 0;
}

bool verifyProof(char* verifyLeaf,char* expectedMerkleRoot,vector<ProofNode> proof){

  size_t vLeafLen = strlen(verifyLeaf);
  char* leaf = (char*)malloc(vLeafLen+1);
  memcpy(leaf,verifyLeaf,vLeafLen);
  if(proof.size() ==0 ){
    if( strcmp(leaf,expectedMerkleRoot)==0)
      return true;
    return false;
  }
 
  // the merkle root should be the parent of the last part
  char* actualMekleRoot = proof[proof.size() -1].hash;
 
  if( strcmp(actualMekleRoot,expectedMerkleRoot)!=0 )
    return false;

  for(int pIdx =0;pIdx<proof.size()-1;pIdx++){
  
    if (proof[pIdx].isRight){
      combin(leaf,proof[pIdx].hash,leaf);
    }else{
      combin(proof[pIdx].hash,leaf,leaf);
    }


  }

  bool valid = strcmp(leaf,expectedMerkleRoot) == 0;
  free(leaf);
  return valid;
}

class merkletree{
public:
  vector<char*> tree;

  merkletree(){}
  merkletree(vector<char*> leaves){
    tree = computeTree(combin,leaves);
  }

  size_t size(){return tree.size();}
  char* root(){return tree[0];}

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
    out_buff[64] = 0;
  }



  vector<char*> computeTree(void (*combineFn)(char*,char*,char*),vector<char*> leaves){
    // compute nodeCount and create vector<T> tree
    int nodeCount = leafCountToNodeCount(leaves.size());
    int delta = nodeCount - leaves.size();
    vector<char*> tree(nodeCount);

    // deep copy
    for(int i = 0 ;i<leaves.size();i++){
        tree[delta + i] = new char[65];
        memcpy(tree[delta + i],leaves[i],65);
        tree[delta + i][64] = 0;
    }

    int idx = nodeCount-1;
    while(idx > 0){
      int parent = (idx -1)/2;

      tree[parent] = new char[65];
      combineFn(tree[idx-1],tree[idx],tree[parent]);
      tree[parent][64] = 0;

      idx-=2;
    }

    return tree;
  }



  vector<ProofNode> proof(char* leafData){
    int idx = findLeaf(tree,leafData);
    if(idx == -1)
      return vector<ProofNode>();
    int proofArrSize = floor( log(tree.size())/ log(2) )+1;
    vector<ProofNode> proof(proofArrSize);

    // record seld
    int proofIdx = 0;


    while(idx > 0 ){


      idx = getBro(tree,idx);
      proof[proofIdx].isRight = idx%2==0;
      proof[proofIdx++].hash = tree[idx];
      idx = getParent(tree,idx);
    }

    // push root
    proof[proofIdx].isRight = false;
    proof[proofIdx++].hash = tree[idx];


	  proof.resize(proofIdx);

    return proof;
  }


	void pushleaf(char* leaf){
		pushleafworker(combin,leaf);
	}


	void pushleafworker(void (*combineFn)(char*,char*,char*),char* leaf){

		// push two
		tree.push_back(new char[65]);
		tree.push_back(new char[65]);

		int pidx = getParent(tree,tree.size()-1);

		// push parent and newleaf
		memcpy(tree[tree.size()-2],tree[pidx],65);
		memcpy(tree[tree.size()-1],leaf,65);

		// climb up and compute
		int idx = tree.size()-1;
		while(idx > 0){
			idx = getParent(tree,idx);
			//cout<<&combineFn<<'\n';
      char *buff = new char[65];
      combineFn(tree[getLeft(tree,idx)],tree[getRight(tree,idx)],buff);
      tree[idx] = buff;
		}

		// done!
	}


};
