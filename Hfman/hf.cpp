    #include <bits/stdc++.h>
using namespace std;
//Abstract Function for building Hoffman HoffTree that will define basic parameters of HoffTree

struct HoffTree{

    int frequency;
    unsigned char character;

    HoffTree *left = NULL;
    HoffTree *right = NULL;
};

class HoffTreeComparator{

  public:
    bool operator()(HoffTree *a, HoffTree *b){
        return a->frequency > b->frequency;
    }
};

//Building Hoffman HoffTree by taking frequency and character pair

HoffTree *buildHuffmanHoffTree(vector<pair<unsigned char, int> > freqtable){

    priority_queue<HoffTree *, vector<HoffTree *>, HoffTreeComparator> huffqueue;
    for (int i = 0; i < freqtable.size(); i++){

        HoffTree *node = new HoffTree();

        node->frequency = freqtable[i].second;
        node->character = freqtable[i].first;

        huffqueue.push(node);
    }



    while (huffqueue.size() > 1){

        HoffTree *a, *b;
        a = huffqueue.top();
        huffqueue.pop();

        b = huffqueue.top();
        huffqueue.pop();

        HoffTree *c = new HoffTree();
        c->frequency = a->frequency + b->frequency;
        c->left = a;
        c->right = b;
        huffqueue.push(c);
    }

    return huffqueue.top();
}

string toBinary(unsigned  char a){

    string output  = "";
    while(a!=0){

        string bit = a%2==0?"0":"1";
        output+=bit;
        a/=2;
    }

    if(output.size()<8){

        int deficit = 8 - output.size();
        for(int i=0; i<deficit; i++){

            output+="0";
        }
    }

    reverse(output.begin(), output.end());
    return output;

}

void traverseHuffmanHoffTree(HoffTree *root, string prev, string toAppend, map<unsigned char, string> &codemap){

    prev+=toAppend;

    if (root->right == NULL && root->left == NULL){
        codemap[root->character] = prev;
    }
    if (root->right != NULL){
        traverseHuffmanHoffTree(root->right, prev, "1", codemap);
    }

    if (root->left != NULL){
        traverseHuffmanHoffTree(root->left, prev, "0", codemap);
    }
}

// File reading oparations where we will open a file in both io stream and store it's content in buffer

unsigned char *readFileIntoBuffer(char *path, int &buffsize){

    FILE *fp = fopen(path, "rb");
    buffsize = 0;
    fseek(fp, 0, SEEK_END);
    buffsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    unsigned char *buffer = (unsigned char *)malloc(buffsize);
    fread(buffer, 1, buffsize, fp);
    return buffer;
}

// File writing oparations where we will write content in file that is stored in our buffer

void writeFileFromBuffer(char *path, unsigned char *buffer, int buffsize, int flag){

    FILE *fp;
    if(flag==0){
        fp = fopen(path, "wb");
    }

    else{
        fp = fopen(path, "ab");
    }

    fwrite(buffer, 1, buffsize, fp);

    fclose(fp);
}

//Here we convert map to vector

vector<pair<unsigned char, int> > convertToVector(map<unsigned char, int> codes){

    vector<pair<unsigned char, int> > codesV;

    for (map<unsigned char, int>::iterator i = codes.begin(); i != codes.end(); i++)
    {
        codesV.push_back(make_pair(i->first, i->second));
    }

    return codesV;
}

//Getting the Hoffman code that is the losslessly compressed text here

string getHuffmanBitstring(unsigned char *buffer, map<unsigned char, string> codes, int buffsize, int& paddedBits){

    string outputBuffer="";
    for(int i=0; i<buffsize; i++){

        outputBuffer=outputBuffer+codes[buffer[i]];
    }

    if(outputBuffer.size()%8!=0){

        int deficit = 8*((outputBuffer.size()/8)+1)-outputBuffer.size();
        paddedBits = deficit;
        for(int i=0; i<deficit; i++){
            outputBuffer+="0";
        }
    }

    //Instead of adding zeroes to the end, add zero to the begining of the last byte

    return outputBuffer;

}

unsigned char* getBufferFromString(string bitstring, vector<unsigned char>&outputBuffer, int& buffsize){

    int interval = 0;
    unsigned char bit = 0;

    for(int i=0; i<buffsize; i++){

         bit = (bit<<1)|(bitstring[i]-'0');

        interval++;
        if(interval==8){

            interval = 0;
            outputBuffer.push_back(bit);
            bit = 0;

        }
    }
    buffsize = outputBuffer.size();
    return outputBuffer.data();
}

string getStringFromBuffer(unsigned char* buffer, int buffsize)
{
    string bitstring = "";
    for(int i=0; i<buffsize; i++){

        bitstring+=toBinary(buffer[i]);
    }

    return bitstring;
}

//Decoding the Hoffman code and again converting it into text

unsigned char* getDecodedBuffer(string bitstring, vector<unsigned char>&buffer, map<unsigned char, string> codes, int &buffsize, int paddedBits){

    string bit = "";
    map<string, unsigned char> reversecodes;

    for(map<unsigned char, string>::iterator i = codes.begin(); i!=codes.end(); i++){

        reversecodes[i->second] = i->first;
    }

    for(int i=0; i<bitstring.size()-paddedBits; i++){

        bit+=string(1, bitstring[i]);
        if(reversecodes.find(bit)!=reversecodes.end()){

           buffer.push_back(reversecodes[bit]);
           bit = "";
        }
    }

    buffsize = buffer.size();
    return buffer.data();
}

//Handling headerfiles through both these functions

void writeHeader(char* path,map<unsigned char, string> codes,  int paddedBits){

    int size = codes.size();
    writeFileFromBuffer(path, (unsigned char*)&paddedBits, sizeof(int), 0);
    writeFileFromBuffer(path, (unsigned char*)&size, sizeof(int), 1);
    char nullBit = '\0';
    for(map<unsigned char, string>::iterator i = codes.begin(); i!=codes.end(); i++){

        writeFileFromBuffer(path, (unsigned char*)&i->first, 1, 1);
        int len = i->second.size();
        writeFileFromBuffer(path, (unsigned char*)&len, sizeof(int), 1);
        writeFileFromBuffer(path, (unsigned char*)i->second.c_str(), i->second.size(), 1);
    }
}


unsigned char* readHeader(unsigned char* buffer, map<unsigned char, string> &codes, int& paddedBits, int &buffsize){

   paddedBits = *((int*)buffer);
   cout<<paddedBits<<"PADDED"<<endl;
   buffer = buffer+4;
   buffsize-=4;
   int size = *((int*)buffer);
   buffer = buffer+4;
   buffsize-=4;
   for(int i=0; i<size; i++){

       unsigned char key = buffer[0];
       buffer++;
       buffsize--;
       int len = *((int*)buffer);
       buffer+=4;
       buffsize-=4;
       char* value = (char*)malloc(len+1);

       for(int j = 0; j<len; j++){

           value[j]=buffer[j];
       }
       buffer+=len;
       buffsize-=len;
       value[len]='\0';
       codes[key] = value;
       cout<<key<<" "<<value<<endl;
   }

   return buffer;
}

//add amount padded

void compressFile(char *path, char *output_path, map<unsigned char, string> &codes){

    int buffsize = 0;
    int paddedBits = 0;
    map<unsigned char, int> freqtable;

    unsigned char *buffer = readFileIntoBuffer(path, buffsize);

    for (int i = 0; i < buffsize; i++){

            freqtable[buffer[i]]++;
    }

    HoffTree *root = buildHuffmanHoffTree(convertToVector(freqtable));
    cout<<root<<endl;

    traverseHuffmanHoffTree(root, "", "", codes);

    string outputString = getHuffmanBitstring(buffer, codes, buffsize, paddedBits);

    buffsize  = outputString.size();

    vector<unsigned char> outputBufferV;

    getBufferFromString(outputString, outputBufferV, buffsize);

    unsigned char* outputBuffer = outputBufferV.data();

    writeHeader(output_path, codes, paddedBits);

    writeFileFromBuffer(output_path, outputBuffer, buffsize, 1);
}

void decompressFile( char* inputPath,  char* outputPath){

    int buffsize = 0;
    map<unsigned char, string> codes;
    int paddedBits = 0;

    unsigned char* fileBuffer = readFileIntoBuffer(inputPath, buffsize);

    fileBuffer = readHeader(fileBuffer, codes, paddedBits, buffsize);

    string fileBitString = getStringFromBuffer(fileBuffer, buffsize);

    vector<unsigned char> outputBufferV;

    unsigned char* outputBuffer;

    getDecodedBuffer(fileBitString,outputBufferV, codes, buffsize, paddedBits);

    outputBuffer = outputBufferV.data();

    writeFileFromBuffer(outputPath, outputBuffer,buffsize, 0);

}

int main(int argc, char* argv[]){

    char* file_to_be_compresssed = "test.txt";

    char* hoffman_file = "target.txt";

    char* decompressed_file = "decoded.txt";

    cout<<"press 1 if you want to set path and 0 if want to keep it default"<<"\n";
    bool setpath;
    cin>>setpath;

    if(setpath==1){
        string s1,s2,s3;
        cout<<"Source file name:";
        cin>>s1;
        cout<<"Target file name:";
        cin>>s2;
        cout<<"Decoded file name:";
        cin>>s3;

        char* file_to_be_compresssed = "s1";

        char* hoffman_file = "s2";

        char* decompressed_file = "s3";
    }

    if(argc==4){

        file_to_be_compresssed  = argv[1];
        hoffman_file = argv[2];
        decompressed_file = argv[3];
    }
    map<unsigned char, string> codes;

    int control;
    cout<<"press 1 to compress 2 to decompress and 0 for both"<<"\n";
    cin>>control;
    if(control==1){
    compressFile(file_to_be_compresssed, hoffman_file, codes);
    }
    else if(control==2){
    decompressFile(hoffman_file, decompressed_file);
    }
    else if(control==0){
    compressFile(file_to_be_compresssed, hoffman_file, codes);
    decompressFile(hoffman_file, decompressed_file);
    }
    else{
        cout<<"press valid number";
    }
}





